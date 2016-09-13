/*
    MYCP is a HTTP and C++ Web Application Server.
    Copyright (C) 2009-2010  Akee Yang <akee.yang@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef WIN32
#pragma warning(disable:4267 4819 4996)
#endif // WIN32

#include "PpHttp.h"
#include "Base64.h"
//#include <boost/format.hpp>

namespace mycp {

const size_t MAX_HTTPHEAD_SIZE		= 4*1024;
const size_t INCREASE_BODY_SIZE		= 20*1024;
const char * SERVERNAME		= "MYCP/2.0";

#define USES_BUFFER_ALLOC
#ifdef USES_ZLIB
inline bool IsDisableContentEncoding(const tstring& sContentType)
{
	//theContentTypeInfoList.insert("gz",CContentTypeInfo::create("application/x-gzip",true));
	//theContentTypeInfoList.insert("tgz",CContentTypeInfo::create("application/x-gzip",true));
	//theContentTypeInfoList.insert("bz",CContentTypeInfo::create("application/x-bzip2",true));
	//theContentTypeInfoList.insert("tbz",CContentTypeInfo::create("application/x-bzip2",true));
	//theContentTypeInfoList.insert("zip",CContentTypeInfo::create("application/zip",true));
	//theContentTypeInfoList.insert("rar",CContentTypeInfo::create("application/x-rar",true));
	//theContentTypeInfoList.insert("tar",CContentTypeInfo::create("application/x-tar",true));
	//theContentTypeInfoList.insert("7z",CContentTypeInfo::create("application/x-7z-compressed",true));
	return (sContentType.find("zip")!=std::string::npos ||
		sContentType.find("rar")!=std::string::npos ||
		sContentType.find("tar")!=std::string::npos ||
		sContentType.find("7z")!=std::string::npos ||
		sContentType.find("octet-stream")!=std::string::npos)?true:false;
}
#endif

CPpHttp::CPpHttp(void)
: m_host("127.0.0.1"), m_account(""), m_secure(""), m_moduleName(""), m_functionName("doHttpFunc"), m_httpVersion("HTTP/1.1"),m_restVersion("v01"), m_contentLength(0), m_method(HTTP_NONE), m_bHttpResponse(false), m_pResponseSaveFile(NULL)
, m_requestURL(""), m_requestURI(""), m_queryString(""),m_postString(""), m_fileName("")
, m_nRangeFrom(0), m_nRangeTo(0)
, m_keepAlive(true), m_keepAliveInterval(0), /*m_contentData(NULL), */m_contentSize(0),m_receiveSize(0)/*,m_nCookieExpiresMinute(0)*/
, m_statusCode(STATUS_CODE_200), m_addDateHeader(false),m_addContentLength(true), m_sReqContentType(""),m_sReqContentEncoding(""),m_sReqAcceptEncoding(""), m_sResContentType("text/html"), m_sLocation("")
, m_forwardFromURL("")
, m_pHeaderBufferTemp(NULL), m_pHeaderTemp(NULL), m_pCookieTemp(NULL)
{
	m_bTransferEncoding = false;
	m_bResponseHeader = false;
	m_bResponseBodyOnly = false;
#ifdef USES_ZLIB
	m_nZipCallBackDataSize = 0;
#endif
	m_bodySize = 0;
	m_bodyBufferSize = INCREASE_BODY_SIZE;
#ifdef USES_BUFFER_ALLOC
	m_resultBuffer = (char*)malloc(MAX_HTTPHEAD_SIZE+m_bodyBufferSize+1);
	if (m_resultBuffer==NULL)
		m_bodyBufferSize = 0;
#else
	m_resultBuffer = new char[MAX_HTTPHEAD_SIZE+m_bodyBufferSize+1];
#endif
}
CPpHttp::~CPpHttp(void)
{
	if (m_pResponseSaveFile!=NULL)
	{
		fclose(m_pResponseSaveFile);
		m_pResponseSaveFile = NULL;
	}
	//m_multiparts.clear();
	m_propertys.clear();
	//m_propertys.cleanAllPropertys();
	m_pReqHeaders.clear();
	//m_pReqHeaders.cleanAllPropertys();
	m_pReqCookies.cleanAllPropertys();

	//if (m_contentData)
	//	delete[] m_contentData;
#ifdef USES_BUFFER_ALLOC
	if (m_resultBuffer!=NULL)
	{
		free(m_resultBuffer);
		m_resultBuffer = NULL;
	}
#else
	delete[] m_resultBuffer;
#endif
	if (m_pHeaderBufferTemp!=NULL)
		delete[] m_pHeaderBufferTemp;
	if (m_pHeaderTemp!=NULL)
		delete[] m_pHeaderTemp;
	if (m_pCookieTemp!=NULL)
		delete[] m_pCookieTemp;

	if (theUpload.getDeleteFile() && m_fileSystemService.get() != NULL)
	{
		for (size_t i=0; i<m_files.size(); i++)
		{
			if (!m_files[i]->getUploadFile()->getFilePath().empty())
				m_fileSystemService->callService("delete", CGC_VALUEINFO(m_files[i]->getUploadFile()->getFilePath()));
		}
	}
	m_files.clear();
}

bool CPpHttp::getUploadFile(std::vector<cgcUploadFile::pointer>& outFiles) const
{
	for (size_t i=0; i<m_files.size(); i++)
	{
		outFiles.push_back(m_files[i]->getUploadFile());
	}
	return !m_files.empty();
	//return m_currentMultiPart.get() == NULL ? cgcNullUploadFile : m_currentMultiPart->getUploadFile();
}

tstring CPpHttp::getCookie(const tstring & name, const tstring& defaultValue) const
{
	const cgcValueInfo::pointer valueInfo = m_pReqCookies.getProperty(name);
	return valueInfo.get() == NULL ? defaultValue : valueInfo->getStr();
}

tstring CPpHttp::getHeader(const tstring & header, const tstring& defaultValue) const
{
	tstring sHeaderLower(header);
	std::transform(sHeaderLower.begin(), sHeaderLower.end(), sHeaderLower.begin(), ::tolower);
	cgcParameter::pointer pParameter;
	return m_pReqHeaders.find(sHeaderLower,pParameter)?pParameter->getStr():defaultValue;
	//const cgcValueInfo::pointer valueInfo = m_pReqHeaders.getProperty(sHeaderLower);
	//return valueInfo.get() == NULL ? defaultValue : valueInfo->getStr();
}

bool CPpHttp::getHeaders(std::vector<cgcKeyValue::pointer>& outHeaders) const
{
	{
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_pReqHeaders.mutex()));
		CLockMap<tstring,cgcParameter::pointer>::const_iterator pIter = m_pReqHeaders.begin();
		for (; pIter!=m_pReqHeaders.end(); pIter++)
		{
			const cgcParameter::pointer pParameter = pIter->second;
			outHeaders.push_back(CGC_KEYVALUE(pParameter->getName(),CGC_VALUEINFO(pParameter->getStr())));
		}
		return m_pReqHeaders.empty(false)?false:true;
	}
}
cgcValueInfo::pointer CPpHttp::getParameter(const tstring & paramName) const
{
	cgcValueInfo::pointer result;
	m_propertys.find(paramName,result);
	return result;
}
bool CPpHttp::getParameters(std::vector<cgcKeyValue::pointer>& outParameters) const
{
	AUTO_CONST_RLOCK(m_propertys);
	CLockMap<tstring, cgcValueInfo::pointer>::const_iterator iter;
	for (iter=m_propertys.begin(); iter!=m_propertys.end(); iter++)
	{
		 outParameters.push_back(CGC_KEYVALUE(iter->first, iter->second));
	}
	return !m_propertys.empty(false);
}


/////////////////////////////////////////////
// Response
void CPpHttp::println(const char * text, size_t size)
{
	write(text, size);
	write("\r\n", 2);
}
void CPpHttp::println(const tstring& text)
{
	write(text.c_str(), text.size());
	write("\r\n", 2);
}
void CPpHttp::write(const char * text, size_t size)
{
	if (size == std::string::npos || size==0 || text == NULL) return;

	if (m_method == HTTP_HEAD) return;

	if (m_bodySize+size > m_bodyBufferSize)
	{
#ifdef USES_BUFFER_ALLOC
		const size_t nSizeTemp = m_bodyBufferSize + (size > INCREASE_BODY_SIZE ? (size+INCREASE_BODY_SIZE) : INCREASE_BODY_SIZE);
		if (m_resultBuffer==NULL)
		{
			m_resultBuffer = (char*)malloc(MAX_HTTPHEAD_SIZE+nSizeTemp+1);
			if (m_resultBuffer==NULL)
			{
				m_resultBuffer = (char*)malloc(MAX_HTTPHEAD_SIZE+nSizeTemp+1);
				if (m_resultBuffer==NULL)
					return;
			}
		}else
		{
			char * bufferTemp = (char*)realloc(m_resultBuffer,MAX_HTTPHEAD_SIZE+nSizeTemp+1);
			if (bufferTemp==NULL)
			{
				bufferTemp = (char*)realloc(m_resultBuffer,MAX_HTTPHEAD_SIZE+nSizeTemp+1);
				if (bufferTemp==NULL)
					return;
			}
			m_resultBuffer = bufferTemp;
		}
		m_bodyBufferSize = nSizeTemp;
#else
		char * bufferTemp = NULL;
		if (m_bodySize>0)
		{
			bufferTemp = new char[m_bodySize+1];
			memcpy(bufferTemp, m_resultBuffer+MAX_HTTPHEAD_SIZE, m_bodySize);
		}
		delete[] m_resultBuffer;

		m_bodyBufferSize += (size > INCREASE_BODY_SIZE ? (size+INCREASE_BODY_SIZE) : INCREASE_BODY_SIZE);
		m_resultBuffer = new char[MAX_HTTPHEAD_SIZE+m_bodyBufferSize+1];
		if (m_resultBuffer==NULL) // *
			m_resultBuffer = new char[MAX_HTTPHEAD_SIZE+m_bodyBufferSize+1];
		if (bufferTemp!=NULL)
		{
			memcpy(m_resultBuffer+MAX_HTTPHEAD_SIZE, bufferTemp, m_bodySize);
			delete[] bufferTemp;
		}
#endif
	}
	memcpy(m_resultBuffer+MAX_HTTPHEAD_SIZE+m_bodySize, text, size);
	m_bodySize += size;
	m_resultBuffer[MAX_HTTPHEAD_SIZE+m_bodySize] = '\0';
}
void CPpHttp::write(const tstring& text)
{
	write(text.c_str(), text.size());
}
void CPpHttp::newline(void)
{
	write("\r\n", 2);
}
void CPpHttp::reset(void)
{
	//m_keepAliveInterval	// ***这个不能重设
	m_statusCode = STATUS_CODE_200;
	m_pResHeaders.clear();
	//m_sMyCookieSessionId.clear();
	//m_nCookieExpiresMinute = 0;
	m_pResCookies.clear();
	m_sResContentType = "text/html";
	m_sResCharset = "";
	m_sLocation = "";
	m_moduleName = "";
	m_functionName = "";
	m_addDateHeader = false;
	m_addContentLength = true;
	m_bTransferEncoding = false;
	m_bResponseHeader = false;
	m_bResponseBodyOnly = false;
	m_bodySize = 0;
	if (m_resultBuffer!=NULL && m_bodyBufferSize>0)
		memset(m_resultBuffer, 0, m_bodyBufferSize);
	m_bHttpResponse = false;
	m_sResponseSaveFile = "";
	if (m_pResponseSaveFile!=NULL)
	{
		fclose(m_pResponseSaveFile);
		m_pResponseSaveFile = NULL;
	}
}
void CPpHttp::init(void)
{
	reset();
	m_account = "";
	m_secure = "";
	m_sMyCookieSessionId = "";
	m_contentLength = 0;
	m_method = HTTP_NONE;
	m_requestURL = "";
	m_requestURI = "";
	m_queryString = "";
	m_postString = "";
	m_fileName = "";
	m_nRangeFrom = 0;
	m_nRangeTo = 0;
	m_keepAlive = true;
	m_contentSize = 0;
	m_receiveSize = 0;
	m_sReqContentType = "";
	m_sReqContentEncoding = "";
	m_sReqAcceptEncoding = "";
	m_forwardFromURL = "";
	m_fileName = "";
	if (m_bodyBufferSize>1*1024*1024)	// 1MB
	{
		m_bodyBufferSize = INCREASE_BODY_SIZE;
#ifdef USES_BUFFER_ALLOC
		free(m_resultBuffer);
		m_resultBuffer = (char*)malloc(MAX_HTTPHEAD_SIZE+m_bodyBufferSize+1);
		if (m_resultBuffer==NULL)
			m_bodyBufferSize = 0;
#else
		delete[] m_resultBuffer;
		m_resultBuffer = new char[MAX_HTTPHEAD_SIZE+m_bodyBufferSize+1];
#endif
	}
	if (theUpload.getDeleteFile() && m_fileSystemService.get() != NULL)
	{
		for (size_t i=0; i<m_files.size(); i++)
		{
			if (!m_files[i]->getUploadFile()->getFilePath().empty())
				m_fileSystemService->callService("delete", CGC_VALUEINFO(m_files[i]->getUploadFile()->getFilePath()));
		}
	}
	m_files.clear();
	m_currentMultiPart.reset();
	m_sCurrentParameterData = "";

	m_propertys.clear();
	//m_propertys.cleanAllPropertys();
	m_pReqHeaders.clear();
	//m_pReqHeaders.cleanAllPropertys();
	m_pReqCookies.cleanAllPropertys();
}

	
void CPpHttp::setCookieMySessionId(const tstring& sMySessionId)
{
	//bool bNewMySessionId = (bool)(m_sMyCookieSessionId != sMySessionId);
	m_sMyCookieSessionId = sMySessionId;
	//if (bNewMySessionId)
	{
		setCookie(cgcCookieInfo::create(Http_CookieSessionId,m_sMyCookieSessionId));
	}
}

void CPpHttp::setCookie(const tstring& name, const tstring& value)
{
	if (!name.empty())
	{
		//m_pResCookies.remove(name);
		m_pResCookies.insert(name,cgcCookieInfo::create(name,value));
	}
}
void CPpHttp::setCookie(const cgcCookieInfo::pointer& pCookieInfo)
{
	if (pCookieInfo.get()!= NULL && !pCookieInfo->m_sCookieName.empty())
	{
		//m_pResCookies.remove(pCookieInfo->m_sCookieName);
		m_pResCookies.insert(pCookieInfo->m_sCookieName,pCookieInfo);
	}
}

void CPpHttp::setHeader(const tstring& name, const tstring& value)
{
	if (!name.empty())
	{
		tstring sStringTemp(name);
		std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), ::tolower);
		if (sStringTemp == "content-type")	// Http_ContentType
		{
			this->setContentType(value);
			return;
		}else if (sStringTemp == "content-length")
		{
			addContentLength(false);
		//}else if (sStringTemp == "reset-body-size")
		//{
		//	// **
		//	m_bodySize = 0;
		//	return;
		}else if (!m_bResponseBodyOnly && sStringTemp == "response-body-only")
		{
			// **
			m_bResponseBodyOnly = true;
			return;
		}else if (!m_bTransferEncoding && sStringTemp == "transfer-encoding")
		{
			// **
			m_bTransferEncoding = true;
		}
		
		//m_pResHeaders.remove(name);
		m_pResHeaders.insert(name,CGC_VALUEINFO(value));
		//m_pResHeaders.push_back(CGC_KEYVALUE(name,CGC_VALUEINFO(value)));
	}
}
void CPpHttp::setContentType(const tstring& contentType)
{
	// application/json; charset=UTF-8;
	m_sResContentType = contentType;
	//std::string sStringTemp(m_sResContentType);
	//std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), ::tolower);
	//std::string::size_type find = sStringTemp.find("charset=");
	//if (find==std::string::npos)
	//{
	//	m_sResCharset.clear();
	//}else
	//{
	//	m_sResCharset = sStringTemp.substr(find+8);
	//	find = m_sResCharset.find(";");
	//	if (find!=std::string::npos)
	//		m_sResCharset = m_sResCharset.substr(0,find);
	//}
}

//void CPpHttp::setHeader(const tstring & header)
//{
//	if (!header.empty())
//	{
//		m_pResHeaders.push_back(header);
//	}
//}

void CPpHttp::forward(const tstring& url)
{
	m_forwardFromURL = m_requestURL;
	reset();

	//m_requestURL = url.empty() ? "/" : url;
	const std::string::size_type find = m_requestURL.rfind("/");
	if (url.empty())
		m_requestURL = "/";
	else if (find==std::string::npos || url.substr(0,1)=="/" || url.substr(0,3)=="www" || url.substr(0,4)=="http")
		m_requestURL = url;
	else
	{
		const std::string sTemp = m_requestURL.substr(0,find+1);
		m_requestURL = sTemp;
		m_requestURL.append(url);
	}
	//printf("******* m_forwardFromURL=%s\n",m_forwardFromURL.c_str());
	//printf("******* forward(): =%s\n",m_requestURL.c_str());
	GeServletInfo();
	GeRequestInfo();
}
void CPpHttp::location(const tstring& url)
{
	// /eb/login.html?type=config/config.csp
	//printf("******* location(): url=%s\n",url.c_str());
	const std::string::size_type find = m_requestURL.rfind("/");
	if (url.empty())
		m_sLocation = "/";
	else if (find==std::string::npos || url.substr(0,1)=="/" || url.substr(0,3)=="www" || url.substr(0,4)=="http")
		m_sLocation = url;
	else
	{
		const std::string sTemp = m_requestURL.substr(0,find+1);
		m_sLocation = sTemp;
		m_sLocation.append(url);
	}
	//printf("******* location(): =%s\n",m_sLocation.c_str());
}

inline std::string GetWeekdayName(int nWeekday)
{
	switch(nWeekday)
	{
	case 0:
		return "Sun";
	case 1:
		return "Mon";
	case 2:
		return "Tue";
	case 3:
		return "Wed";
	case 4:
		return "Thu";
	case 5:
		return "Fri";
	default:
		return "Sta";
	}
}
inline std::string GetMonthName(int nMonth)
{
	// = "Jan" | "Feb" | "Mar" | "Apr"| "May" | "Jun" | "Jul" | "Aug" | "Sep" | "Oct" | "Nov" | "Dec" 
	switch(nMonth)
	{
	case 0:
		return "Jan";
	case 1:
		return "Feb";
	case 2:
		return "Mar";
	case 3:
		return "Apr";
	case 4:
		return "May";
	case 5:
		return "Jun";
	case 6:
		return "Jul";
	case 7:
		return "Aug";
	case 8:
		return "Sep";
	case 9:
		return "Oct";
	case 10:
		return "Nov";
	default:
		return "Dec";
	}
}

#ifdef USES_ZLIB
bool CPpHttp::MyZipDataCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned int nUserData)
{
	if (nSize>0 && nUserData!=0)
	{
		CPpHttp * pHttp = (CPpHttp*)nUserData;
		memcpy(pHttp->m_resultBuffer+(MAX_HTTPHEAD_SIZE+pHttp->m_nZipCallBackDataSize), pData, nSize);
		pHttp->m_nZipCallBackDataSize += nSize;
	}
	return true;
}
bool CPpHttp::UnZipPostStringCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned int nUserData)
{
	if (nSize>0 && nUserData!=0)
	{
		CPpHttp * pHttp = (CPpHttp*)nUserData;
		pHttp->m_postString.append((const char*)pData,nSize);
	}
	return true;
}
bool CPpHttp::UnZipWriteFileCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned int nUserData)
{
	if (nSize>0 && nUserData!=0)
	{
		FILE * f = (FILE*)nUserData;
		fwrite(pData,1,nSize,f);
	}
	return true;
}
bool CPpHttp::UnZipWriteCurrentMultiPartCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned int nUserData)
{
	if (nSize>0 && nUserData!=0)
	{
		CPpHttp * pHttp = (CPpHttp*)nUserData;
		pHttp->m_currentMultiPart->write((const char*)pData,nSize);
	}
	return true;
}
#endif
const char * CPpHttp::getHttpResult(size_t& outSize) const
{
	if (m_resultBuffer==NULL) return NULL;
	//if (m_bResponseBodyOnly)
	//{
	//	return ;
	//}
	// Make Response
	if (m_pHeaderBufferTemp==NULL)
	{
		const_cast<CPpHttp*>(this)->m_pHeaderBufferTemp = new char[MAX_HTTPHEAD_SIZE];
		if (m_pHeaderBufferTemp==NULL) return NULL;
	}
	if (m_pHeaderTemp==NULL)
	{
		const_cast<CPpHttp*>(this)->m_pHeaderTemp = new char[1024*4];
		if (m_pHeaderTemp==NULL) return NULL;
	}
	if (m_pCookieTemp==NULL)
	{
		const_cast<CPpHttp*>(this)->m_pCookieTemp = new char[1024*4];
		if (m_pCookieTemp==NULL) return NULL;
	}

	if (m_bResponseBodyOnly || (m_bTransferEncoding && m_bResponseHeader))
	{
		outSize = m_bodySize;
		return m_resultBuffer+MAX_HTTPHEAD_SIZE;
	}else if (m_bTransferEncoding)
	{
		const_cast<CPpHttp*>(this)->m_bResponseHeader = true;
	}
	// get all headers
	//std::string sHeaders;
	if (m_sResContentType.empty())
	{
		sprintf(m_pHeaderBufferTemp, "HTTP/1.1 %s\r\n",cgcGetStatusCode(m_statusCode).c_str());
	}else
	{
		sprintf(m_pHeaderBufferTemp, "HTTP/1.1 %s\r\nContent-Type: %s\r\n",cgcGetStatusCode(m_statusCode).c_str(), m_sResContentType.c_str());
	}
	bool bFindContentionHeader = false;	// Http_Contention
	bool bFindContentEncodingHeader = false;	// Http_ContentEncoding
	size_t nHeadSize = strlen(m_pHeaderBufferTemp);
	{
		CLockMap<tstring,cgcValueInfo::pointer>::const_iterator pIter = m_pResHeaders.begin();
		for (;pIter!=m_pResHeaders.end();pIter++)
		{
			const tstring sKey(pIter->first);
			if (!bFindContentEncodingHeader)
			{
				tstring sStringTemp(sKey);
				std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), ::tolower);
				if (sStringTemp=="content-encoding")
					bFindContentEncodingHeader = true;
			}
			if (!bFindContentionHeader)
			{
				tstring sStringTemp(sKey);
				std::transform(sStringTemp.begin(), sStringTemp.end(), sStringTemp.begin(), ::tolower);
				if (sStringTemp=="connection")
					bFindContentionHeader = true;
			}
			const cgcValueInfo::pointer pValue = pIter->second;
			sprintf(m_pHeaderTemp,"%s: %s\r\n",sKey.c_str(),pValue->getStr().c_str());
			//sHeaders.append(m_pHeaderTemp);
			strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
			nHeadSize += strlen(m_pHeaderTemp);
		}
	}
	// Cookies
	std::string sDTTemp;
	{
		CLockMap<tstring,cgcCookieInfo::pointer>::const_iterator pIter = m_pResCookies.begin();
		for (;pIter!=m_pResCookies.end();pIter++)
		{
			const tstring sKey(pIter->first);
			const cgcCookieInfo::pointer pCookieInfo = pIter->second;
			//const cgcValueInfo::pointer pValue = pIter->second;
			sprintf(m_pCookieTemp,"%s=%s; path=%s",sKey.c_str(),pCookieInfo->m_sCookieValue.c_str(),pCookieInfo->m_sCookiePath.c_str());
			if (pCookieInfo->m_tExpiresTime > 0)
			{
				// 设置cookie过期时间
				if (sDTTemp.empty())
				{
					struct tm *newtime;
					time_t ltime;
					time(&ltime);
					ltime += pCookieInfo->m_tExpiresTime*60;
					newtime = gmtime(&ltime);
					char szDT[128];
					//strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S GMT", newtime);
					// Sat,03 May 2025 17:44:22 GMT
					sprintf(szDT,"%s, %02d %s %d %02d:%02d:%02d GMT", GetWeekdayName(newtime->tm_wday).c_str(),newtime->tm_mday,
						GetMonthName(newtime->tm_mon).c_str(),(newtime->tm_year+1900),newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
					sDTTemp = szDT;
					//					printf("**** DATE=%s\n",sDTTemp.c_str());
					//					printf("**** DATE1=%s,m_sResCharset=%s\n",szDT,m_sResCharset.c_str());
					//					if (m_sResCharset=="utf-8")
					//					{
					//#ifdef WIN32
					//						sDTTemp = CGC_ACP2UTF8(szDT);
					//#else
					//						CGC_XXX2UTF8("gb2312",szDT,strlen(szDT),sDTTemp);
					//#endif
					//					}else
					//					{
					//						sDTTemp = szDT;
					//					}
				}
				sprintf(m_pHeaderTemp, "Set-Cookie: %s; expires=%s\r\n", m_pCookieTemp,sDTTemp.c_str());
			}else
			{
				sprintf(m_pHeaderTemp, "Set-Cookie: %s\r\n", m_pCookieTemp);
			}
			strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
			nHeadSize += strlen(m_pHeaderTemp);
			//sHeaders.append(m_pHeaderTemp);
		}
	}
	// Date: xxx
	if (m_addDateHeader)
	{
		// Obtain current GMT date/time
		if (sDTTemp.empty())
		{
			struct tm *newtime;
			time_t ltime;
			time(&ltime);
			newtime = gmtime(&ltime);
			char szDT[128];
			sprintf(szDT,"%s, %02d %s %d %02d:%02d:%02d GMT", GetWeekdayName(newtime->tm_wday).c_str(),newtime->tm_mday,
				GetMonthName(newtime->tm_mon).c_str(),(newtime->tm_year+1900),newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
			sDTTemp = szDT;
			//			strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S GMT", newtime);
			//			if (m_sResCharset=="utf-8")
			//			{
			//#ifdef WIN32
			//				sDTTemp = CGC_ACP2UTF8(szDT);
			//#else
			//				CGC_XXX2UTF8("gb2312",szDT,strlen(szDT),sDTTemp);
			//#endif
			//			}else
			//			{
			//				sDTTemp = szDT;
			//			}
		}
		sprintf(m_pHeaderTemp, "Date: %s\r\n", sDTTemp.c_str());
		strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
		nHeadSize += strlen(m_pHeaderTemp);
		//sHeaders.append(m_pHeaderTemp);
	}
	// Location: xxx
	if (!m_sLocation.empty())
	{
		sprintf(m_pHeaderTemp, "Location: %s\r\n", m_sLocation.c_str());
		strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
		nHeadSize += strlen(m_pHeaderTemp);
		//sHeaders.append(m_pHeaderTemp);
	}
	// WWW-Authenticate: xxx
	if (m_statusCode == STATUS_CODE_401)
	{
		sprintf(m_pHeaderTemp, "WWW-Authenticate: Basic realm=\"%s\"\r\n", m_host.c_str());
		strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
		nHeadSize += strlen(m_pHeaderTemp);
		//sHeaders.append(m_pHeaderTemp);
	}
	if (!bFindContentionHeader)
	{
		// Connection: xxx
		if (m_keepAlive && m_keepAliveInterval>=0)
		{
			sprintf(m_pHeaderTemp, "Connection: Keep-Alive\r\n");
			//sprintf(m_pHeaderTemp, "Connection: Keep-Alive\r\nKeep-Alive: 300\r\n");
		}else
		{
			sprintf(m_pHeaderTemp, "Connection: close\r\n");
		}
		strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
		nHeadSize += strlen(m_pHeaderTemp);
	}
	//sHeaders.append(m_pHeaderTemp);
	// Server: xxx
	sprintf(m_pHeaderTemp,"Server: %s\r\n",SERVERNAME);
	strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
	nHeadSize += strlen(m_pHeaderTemp);
	//sHeaders.append(m_pHeaderTemp);
	// Content-Length: xxx
#ifdef USES_ZLIB
	////sprintf(m_pHeaderTemp,"Accept-Encoding: gzip\r\n");
	//sprintf(m_pHeaderTemp,"Accept-Encoding: gzip, deflate\r\n");
	//strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
	//nHeadSize += strlen(m_pHeaderTemp);
	if (!m_bTransferEncoding && 
		!bFindContentEncodingHeader && m_bodySize>=128  && !IsDisableContentEncoding(m_sResContentType))
	{
		// gzip,deflate,compress
		if (m_sReqAcceptEncoding.find("gzip")!=std::string::npos)
		{
			const_cast<CPpHttp*>(this)->m_nZipCallBackDataSize = 0;
			if (GZipDataCb((const unsigned char*)m_resultBuffer+MAX_HTTPHEAD_SIZE,(uLong)m_bodySize,Z_DEFAULT_COMPRESSION,MyZipDataCallBack,(unsigned int)this)==Z_OK)
			{
				//printf("*********** GZipDataCb m_sReqAcceptEncoding=%s,bodysize=%d,m_nZipCallBackDataSize=%d\n",m_sReqAcceptEncoding.c_str(),(int)m_bodySize,(int)m_nZipCallBackDataSize);
				const_cast<CPpHttp*>(this)->m_bodySize = m_nZipCallBackDataSize;
				sprintf(m_pHeaderTemp,"Content-Encoding: gzip\r\n");
				strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
				nHeadSize += strlen(m_pHeaderTemp);
			}
		}else if (m_sReqAcceptEncoding.find("deflate")!=std::string::npos)
		{
			const_cast<CPpHttp*>(this)->m_nZipCallBackDataSize = 0;
			if (ZipDataCb((const unsigned char*)m_resultBuffer+MAX_HTTPHEAD_SIZE,(uLong)m_bodySize,0,Z_DEFAULT_COMPRESSION,MyZipDataCallBack,(unsigned int)this)==Z_OK)
			{
				//printf("*********** GZipDataCb m_sReqAcceptEncoding=%s,bodysize=%d,m_nZipCallBackDataSize=%d\n",m_sReqAcceptEncoding.c_str(),(int)m_bodySize,(int)m_nZipCallBackDataSize);
				const_cast<CPpHttp*>(this)->m_bodySize = m_nZipCallBackDataSize;
				sprintf(m_pHeaderTemp,"Content-Encoding: deflate\r\n");
				strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
				nHeadSize += strlen(m_pHeaderTemp);
			}
		}
	}
#endif
	if (!m_bTransferEncoding && 
		m_addContentLength && m_bodySize>0)
	{
		sprintf(m_pHeaderTemp,"Content-Length: %d\r\n",m_bodySize);
		strcpy(m_pHeaderBufferTemp+nHeadSize,m_pHeaderTemp);
		nHeadSize += strlen(m_pHeaderTemp);
		//sHeaders.append(m_pHeaderTemp);
	}
	strcpy(m_pHeaderBufferTemp+nHeadSize,"\r\n");
	nHeadSize += 2;

	// Transfer-Encoding: chunked\r\n 一直当接收不完整，会空白
	// Accept-Ranges: bytes\r\n
	//sprintf(m_pHeaderBufferTemp, "HTTP/1.1 %s\r\n%sContent-Type: %s\r\n\r\n",
	//	cgcGetStatusCode(m_statusCode).c_str(), sHeaders.c_str(), m_sResContentType.c_str());

	const size_t headerSize = nHeadSize;
	//const size_t headerSize = strlen(m_pHeaderBufferTemp);
	outSize = m_bodySize + headerSize;
	memcpy(m_resultBuffer, m_pHeaderBufferTemp, headerSize);
	memmove(m_resultBuffer+headerSize, m_resultBuffer+MAX_HTTPHEAD_SIZE, m_bodySize);	// ***
	//memcpy(m_resultBuffer+headerSize, m_resultBuffer+MAX_HTTPHEAD_SIZE, m_bodySize);	// xxx
	m_resultBuffer[outSize] = '\0';
	const_cast<CPpHttp*>(this)->m_bodySize = 0;
	return m_resultBuffer;
}

bool CPpHttp::doParse(const unsigned char * requestData, size_t requestSize,const char* sEncoding)
{
	if (requestData == NULL) return false;

	bool bGetHeader = false;
	const bool ret = IsComplete((const char*)requestData,requestSize,bGetHeader);
	//printf("**** doParse ret=%d\n",(int)(ret?1:0));
	if (bGetHeader)
	{
		tstring sConnection(getHeader(Http_Connection, ""));
		std::transform(sConnection.begin(), sConnection.end(), sConnection.begin(), ::tolower);
		m_keepAlive = sConnection == "keep-alive"?true:false;
		const tstring sKeepAlive(getHeader(Http_KeepAlive, "0"));
		m_keepAliveInterval = atoi(sKeepAlive.c_str());
		m_sReqContentType = getHeader(Http_ContentType, "");
		std::transform(m_sReqContentType.begin(), m_sReqContentType.end(), m_sReqContentType.begin(), ::tolower);
//#ifdef USES_ZLIB
//		m_sReqContentEncoding = getHeader(Http_ContentEncoding, "");
//		m_sReqAcceptEncoding = getHeader(Http_AcceptEncoding, "");
//		std::transform(m_sReqAcceptEncoding.begin(), m_sReqAcceptEncoding.end(), m_sReqAcceptEncoding.begin(), ::tolower);
//		//printf("*********** %s : %s\n",Http_AcceptEncoding.c_str(),m_sReqAcceptEncoding.c_str());
//#endif
		m_host = getHeader(Http_Host, "");
		m_sMyCookieSessionId = getCookie(Http_CookieSessionId, "");
		const tstring sRange = getHeader(Http_Range, "");
		if (!sRange.empty())
		{
			//Range: bytes=500-      表示读取该文件的500-999字节，共500字节。
			//Range: bytes=500-599   表示读取该文件的500-599字节，共100字节。
			std::string::size_type find = sRange.find("bytes=");
			if (find != std::string::npos)
			{
				m_nRangeFrom = atoi(sRange.substr(find+6).c_str());
				find = sRange.find("-",7);
				if (find != std::string::npos)
					m_nRangeTo = atoi(sRange.substr(find+1).c_str());
			}
		}

		if (!m_bHttpResponse)
		{
			tstring authorization = getHeader(Http_Authorization, "");
			if (!authorization.empty())
			{
				if (authorization.substr(0, 6) == "Basic ")
				{
					char * buffer = new char[int(authorization.size()*0.8)];
					long len = Base64Decode((unsigned char*)buffer, authorization.c_str()+6);
					if (len > 0)
					{
						authorization = std::string(buffer, len);
						std::string::size_type find = authorization.find(":");
						if (find != std::string::npos)
						{
							m_account = authorization.substr(0, find);
							m_secure = authorization.substr(find+1);
						}
					}
					delete[] buffer;
				}
			}
		}
	}
	if (ret)
	{
		GeRequestInfo();
	}
	return ret;
}

void CPpHttp::GeServletInfo(void)
{
	short nfindsize = 6;	// "/rest."
	std::string::size_type findServlet1 = m_requestURL.find("/rest.");
	if (findServlet1==std::string::npos)
	{
		nfindsize = 9;		// "/servlet."
		findServlet1 = m_requestURL.find("/servlet.");
	}
	if (findServlet1 != std::string::npos)
	{
		std::string::size_type findServlet2 = 0;
		if (nfindsize == 6)
		{
			// 支持版本号
			// 格式：/rest.vvv.module.func?
			findServlet2 = m_requestURL.find(".", nfindsize+1);
			if (findServlet2 != std::string::npos)
			{
				m_restVersion = m_requestURL.substr(nfindsize,findServlet2-nfindsize);
				nfindsize = findServlet2+1;	// **必须放后面
			}
		}
		findServlet2 = m_requestURL.find("?", nfindsize+1);
		if (findServlet2 == std::string::npos)
			m_moduleName = m_requestURL.substr(nfindsize, m_requestURL.size()-findServlet1-nfindsize);
		else
			m_moduleName = m_requestURL.substr(nfindsize, findServlet2-findServlet1-nfindsize);

		findServlet2 = m_moduleName.find(".");
		if (findServlet2 != std::string::npos)
		{
			m_functionName = "do";
			m_functionName.append(m_moduleName.substr(findServlet2+1, m_moduleName.size()-findServlet2-1));
			m_moduleName = m_moduleName.substr(0, findServlet2);
		}
		//printf("============%s,%s,%s=============\n",m_restVersion.c_str(),m_moduleName.c_str(),m_functionName.c_str());
	}
}

void CPpHttp::GetPropertys(const std::string& sString, bool bUrlDecode)
{
	if (sString.empty()) return;
	tstring parameter;
	std::string::size_type find = 0;
	do
	{
		// Get [parameter=value]
		std::string::size_type findParameter = sString.find("&", find+1);
		if (findParameter == std::string::npos)
		{
			parameter = sString.substr(find, sString.size()-find);
		}else
		{
			parameter = sString.substr(find, findParameter-find);
			findParameter += 1;
		}
		find = findParameter;

		// Get parameter/value
		findParameter = parameter.find("=", 1);
		if (findParameter == std::string::npos)
		{
			// ERROR
			break;
		}

		tstring p(parameter.substr(0, findParameter));
		if (bUrlDecode && !p.empty())
			p = URLDecode(p.c_str());
		const size_t nPSize = p.size();
		bool bIsVector = false;
		if (nPSize>2 && p.substr(nPSize-2,2)=="[]")
		{
			bIsVector = true;
			p = p.substr(0,nPSize-2);
		}
		tstring v(parameter.substr(findParameter+1, parameter.size()-findParameter));
		if (bUrlDecode && !v.empty())
			v = URLDecode(v.c_str());
		//printf("**** GetPropertys, %s:%s\n",p.c_str(),v.c_str());
		cgcValueInfo::pointer pValueInfo = CGC_VALUEINFO(v);
		cgcValueInfo::pointer pOldValueInfo;
		m_propertys.insert(p,pValueInfo,false,&pOldValueInfo);
		if (bIsVector)
		{
			 if (pOldValueInfo.get()!=NULL)
			 {
				 pOldValueInfo->totype(cgcValueInfo::TYPE_VECTOR);
				 pOldValueInfo->addVector(pValueInfo);
			 }else
			 {
				 pValueInfo->totype(cgcValueInfo::TYPE_VECTOR);
			 }
		}
		//m_propertys.setProperty(p, CGC_VALUEINFO(v));
	}while (find != std::string::npos);
}

void CPpHttp::GeRequestInfo(void)
{
	if (m_bHttpResponse) return;
	m_requestURI = m_requestURL;	
	//printf("******* GeRequestInfo(): m_requestURI=%s\n",m_requestURI.c_str());
	//if (m_method != HTTP_POST)
	//if (m_method == HTTP_GET)
	{
		std::string::size_type find = m_requestURL.find("?");
		if (find != std::string::npos)
		{
			m_requestURI = m_requestURL.substr(0, find);
			m_queryString = m_requestURL.substr(find+1);
		}
		const std::string::size_type findFileName = m_requestURI.rfind("/");
		if (findFileName != std::string::npos)
		{
			m_fileName = m_requestURI.substr(findFileName+1);
		}
	}
	const std::string::size_type findExt = m_requestURI.rfind(".");
	tstring sFileExt = (findExt!=std::string::npos)?m_requestURI.substr(findExt+1):"";
	if (findExt!=std::string::npos)
		std::transform(sFileExt.begin(), sFileExt.end(), sFileExt.begin(), ::tolower);
	const bool bPHP = sFileExt=="php"?true:false;

	const std::string::size_type nFindUrlEncoded = bPHP?std::string::npos:m_sReqContentType.find("application/x-www-form-urlencoded");
	const bool bUrlDecode = (nFindUrlEncoded!=std::string::npos)?true:false;
	if (!bPHP)
		GetPropertys(m_queryString, bUrlDecode);
	if (!bPHP && !m_postString.empty() && m_postString!=m_queryString)
		GetPropertys(m_postString, bUrlDecode);
	if (!bPHP && bUrlDecode)
	{
		m_queryString = URLDecode(m_queryString.c_str());
		if (!m_postString.empty())
			m_postString = URLDecode(m_postString.c_str());
	}
	if ((m_method == HTTP_POST&&m_queryString.empty()) || !m_postString.empty())
		m_queryString = m_postString;
}

//#define USES_PRINT_DEBUG

inline bool FindHttpHeader(const char * httpRequest, const char ** pOutFind1, const char ** pOutFind2, short* pOutFind1Offset)
{
	int nOffset = 0;
	*pOutFind1 = NULL;
	while (httpRequest[nOffset]!='\0')
	{
		if (*pOutFind1==NULL && httpRequest[nOffset]==':')
		{
			*pOutFind1 = httpRequest+nOffset;
			*pOutFind1Offset = httpRequest[nOffset+1]==' '?2:1;
		}else if (httpRequest[nOffset]=='\r' && httpRequest[nOffset+1]=='\n')
		{
			*pOutFind2 = httpRequest+nOffset;
			return *pOutFind1==NULL?false:true;
		}
		nOffset++;
	}
	return false;
}

bool CPpHttp::IsComplete(const char * httpRequest, size_t requestSize,bool& pOutHeader)
{
	tstring multipartyBoundary = "";
	const char * httpRequestOld = httpRequest;
	m_contentLength = requestSize;
#ifdef USES_PRINT_DEBUG
	static int theIndex = 0;
	static FILE * f = NULL;
	if (f==NULL)
		f = fopen("c:\\http_recv.txt","wb");
	if (f!=NULL)
	{
		char lpszBuf[100];
		sprintf(lpszBuf,"\r\n**** index=%d, size=%d\r\n",(theIndex++),requestSize);
		fwrite(lpszBuf,1,strlen(lpszBuf),f);
		fwrite(httpRequest,1,requestSize,f);
		fflush(f);
	}
#endif

	//printf("CPpHttp::IsComplete  size=%d\n",requestSize);
	// Check HTTP Method
	int leftIndex = 0;
	if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "HTTP/", leftIndex))
	{
		// HTTP/1.1 200 OK
		leftIndex += 5;
		m_bHttpResponse = true;
		m_method = HTTP_NONE;
		//m_functionName = "doGET";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "GET", leftIndex))
	{
		// GET /index.asp HTTP/1.0
		leftIndex += 4;
		m_method = HTTP_GET;
		m_functionName = "doGET";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "HEAD", leftIndex))
	{
		leftIndex += 5;
		m_method = HTTP_HEAD;
		m_functionName = "doHEAD";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "POST", leftIndex))
	{
		leftIndex += 5;
		m_method = HTTP_POST;
		m_functionName = "doPOST";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "PUT", leftIndex))
	{
		leftIndex += 4;
		m_method = HTTP_PUT;
		m_functionName = "doPUT";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "DELETE", leftIndex))
	{
		leftIndex += 7;
		m_method = HTTP_DELETE;
		m_functionName = "doDELETE";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "OPTIONS", leftIndex))
	{
		leftIndex += 8;
		m_method = HTTP_OPTIONS;
		m_functionName = "doOPTIONS";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "TRACE", leftIndex))
	{
		leftIndex += 6;
		m_method = HTTP_TRACE;
		m_functionName = "doTRACE";
		m_sCurrentParameterData.clear();
	}else if (m_method==HTTP_NONE && !m_bHttpResponse && sotpCompare(httpRequest, "CONNECT", leftIndex))
	{
		leftIndex += 8;
		m_method = HTTP_CONNECT;
		m_functionName = "doCONNECT";
		m_currentMultiPart.reset();
		m_sCurrentParameterData.clear();
	}else
	{
		if (m_currentMultiPart.get() == NULL) return false;
		/*
		// getMaxFileSize
		if (theUpload.getMaxFileSize() > 0 &&
		(int)(m_currentMultiPart->getUploadFile()->getFileSize() / 1024) > theUpload.getMaxFileSize())
		{
		return false;
		}
		*/
		//printf("**** m_contentSize=%d,%d\n",m_contentSize,m_receiveSize+requestSize);
		if (m_contentSize == requestSize && m_currentMultiPart->getBoundary().empty())
		{
			m_currentMultiPart.reset();

			unsigned int nUnZipSize = 0;
			bool bUnZipOk = false;
			if (m_bHttpResponse && !m_sResponseSaveFile.empty())
			{
				if (m_pResponseSaveFile!=NULL)
				{
#ifdef USES_ZLIB
					if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
					{
						if (UnGZipDataCb((const unsigned char*)httpRequest,(uLong)requestSize,&nUnZipSize,UnZipWriteFileCallBack,(unsigned int)m_pResponseSaveFile)==Z_OK)
						{
							bUnZipOk = true;
						}
					}
#endif
					if (!bUnZipOk)
						fwrite(httpRequest,1,requestSize,m_pResponseSaveFile);
					fclose(m_pResponseSaveFile);
					m_pResponseSaveFile = NULL;
				}
			}else
			{
#ifdef USES_ZLIB
				if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
				{
					m_postString.clear();
					if (UnGZipDataCb((const unsigned char*)httpRequest,(uLong)requestSize,&nUnZipSize,UnZipPostStringCallBack,(unsigned int)this)==Z_OK)
					{
						bUnZipOk = true;
						m_queryString = m_postString;
					}
				}
#endif
				if (!bUnZipOk)
				{
					m_queryString = httpRequest;
					m_postString = m_queryString;
				}
			}
			if (bUnZipOk)
				m_receiveSize = nUnZipSize;
			else
				m_receiveSize = requestSize;
			return true;
		}else if (m_contentSize >= (m_receiveSize + requestSize) && m_currentMultiPart->getBoundary().empty())
		{
			//strncpy(m_contentData+m_receiveSize,httpRequest,requestSize);
			unsigned int nUnZipSize = 0;
			bool bUnZipOk = false;
			if (m_bHttpResponse && !m_sResponseSaveFile.empty())
			{
				if (m_pResponseSaveFile!=NULL)
				{
#ifdef USES_ZLIB
					if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
					{
						if (UnGZipDataCb((const unsigned char*)httpRequest,(uLong)requestSize,&nUnZipSize,UnZipWriteFileCallBack,(unsigned int)m_pResponseSaveFile)==Z_OK)
						{
							bUnZipOk = true;
						}
					}
#endif
					if (!bUnZipOk)
						fwrite(httpRequest,1,requestSize,m_pResponseSaveFile);
				}
			}else
			{
#ifdef USES_ZLIB
				if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
				{
					if (UnGZipDataCb((const unsigned char*)httpRequest,(uLong)requestSize,&nUnZipSize,UnZipPostStringCallBack,(unsigned int)this)==Z_OK)
					{
						bUnZipOk = true;
						m_queryString = m_postString;
					}
				}
#endif
				if (!bUnZipOk)
				{
					m_queryString.append(httpRequest);
					m_postString.append(httpRequest);
				}
			}
			if (bUnZipOk)
				m_receiveSize += nUnZipSize;
			else
				m_receiveSize += requestSize;
			if (m_receiveSize>=m_contentSize)
			{
				m_currentMultiPart.reset();
				if (m_pResponseSaveFile!=NULL)
				{
					fclose(m_pResponseSaveFile);
					m_pResponseSaveFile = NULL;
				}
				return true;
			}
			return false;
		}

		bool findBoundary = false;
		if (!m_sCurrentParameterData.empty() && m_currentMultiPart->getFileName().empty() && m_currentMultiPart->getName().empty() && !m_currentMultiPart->getBoundary().empty())
		{
			// 普通参数，前面有处理未完成数据；
			// * by hd 2016-07-23
			//m_queryString.append(httpRequest);
			//m_postString.append(httpRequest);
			//m_receiveSize += requestSize;
			findBoundary = true;
			multipartyBoundary = m_currentMultiPart->getBoundary();
			m_sCurrentParameterData.append(httpRequest,requestSize);
			//if (sotpCompare(m_sCurrentParameterData.c_str(), Http_ContentDisposition.c_str(), leftIndex))
			{
				httpRequest = m_sCurrentParameterData.c_str();
				requestSize = m_sCurrentParameterData.size();
			}
			bool bUnZipOk = false;
#ifdef USES_ZLIB
			if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
			{
				unsigned int nUnZipSize = 0;
				if (UnGZipDataCb((const unsigned char*)httpRequest,(uLong)requestSize,&nUnZipSize,UnZipPostStringCallBack,(unsigned int)this)==Z_OK)
				{
					bUnZipOk = true;
					m_queryString = m_postString;
					m_receiveSize += nUnZipSize;
				}
			}
#endif
			if (!bUnZipOk)
			{
				m_queryString.append(httpRequest);
				m_postString.append(httpRequest);
				m_receiveSize += requestSize;
			}
		}else if (m_currentMultiPart->getFileName().empty() ||				// 普通参数，不是文件
			requestSize >= m_currentMultiPart->getBoundary().size()+2)		// 文件
		{
			if (sotpCompare(httpRequest, m_currentMultiPart->getBoundary().c_str(), leftIndex))
			{
				findBoundary = true;
				multipartyBoundary = m_currentMultiPart->getBoundary();
				httpRequest += multipartyBoundary.size() + 2;
			}else
			{
				tstring boundaryEnd("\r\n");
				boundaryEnd.append(m_currentMultiPart->getBoundary());
				const char * find = strstrl(httpRequest, boundaryEnd.c_str(), requestSize, boundaryEnd.size());
				if (find != NULL)
				{
					findBoundary = true;
					if (m_currentMultiPart->getUploadFile()->getFileSize() > 0)
					{
						bool bUnZipOk = false;
#ifdef USES_ZLIB
						if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
						{
							if (UnGZipDataCb((const unsigned char*)httpRequest,(uLong)(find-(const char*)httpRequest),NULL,UnZipWriteCurrentMultiPartCallBack,(unsigned int)this)==Z_OK)
							{
								bUnZipOk = true;
							}
						}
#endif
						if (!bUnZipOk)
							m_currentMultiPart->write((const char*)httpRequest, find-(const char*)httpRequest);
						m_currentMultiPart->close();
						m_currentMultiPart->setParser(cgcNullParserBaseService);
						m_files.push_back(m_currentMultiPart);
						multipartyBoundary = m_currentMultiPart->getBoundary();
						m_currentMultiPart.reset();

						// getMaxFileCount
						if (theUpload.getMaxFileCount() > 0 && (int)m_files.size() >= theUpload.getMaxFileCount())
						{
							return true;
						}
					}

					if (sotpCompare(find+boundaryEnd.size(), "--\r\n", leftIndex))
					{
						return true;
					}

					httpRequest = find;
				}
			}
		}else
		{
			m_currentMultiPart->close();
			m_currentMultiPart->setParser(cgcNullParserBaseService);
			m_files.push_back(m_currentMultiPart);
			m_currentMultiPart.reset();
			return true;
		}

		if (!findBoundary)
		{
			bool bUnZipOk = false;
#ifdef USES_ZLIB
			if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
			{
				if (UnGZipDataCb((const unsigned char*)httpRequest,(uLong)requestSize,NULL,UnZipWriteCurrentMultiPartCallBack,(unsigned int)this)==Z_OK)
				{
					bUnZipOk = true;
				}
			}
#endif
			if (!bUnZipOk)
				m_currentMultiPart->write((const char*)httpRequest, requestSize);
			// getMaxFileSize & isGreaterMaxSize
			if ((theUpload.getMaxFileSize() > 0 && (int)(m_currentMultiPart->getUploadFile()->getFileSize() / 1024) > theUpload.getMaxFileSize())
				|| isGreaterMaxSize())
			{
				m_statusCode = STATUS_CODE_413;
				m_currentMultiPart->close();
				m_currentMultiPart->setParser(cgcNullParserBaseService);
				m_files.push_back(m_currentMultiPart);
				m_currentMultiPart.reset();
				return true;	// 由api去处理
			}
			return false;
		}
	}

	const char * findSearch = NULL;
	const char * findSearchEnd = NULL;
	if (multipartyBoundary.empty())
	{
		// Get file name
		findSearch = strstr(httpRequest+leftIndex, " ");
		//printf("**** findSearch=%d, m_bHttpResponse=%d\n",(int)findSearch,(int)(m_bHttpResponse?1:0));
		if (findSearch == NULL)
			return false;
		if (m_bHttpResponse)
		{
			// HTTP/1.1 200 OK
			m_httpVersion = tstring(httpRequest+(leftIndex-5), findSearch-httpRequest-(leftIndex-5));
			//findSearchEnd = strstr(findSearch+1, " ");
			//if (findSearchEnd == NULL) return false;
			m_statusCode = (HTTP_STATUSCODE)atoi(findSearch+1);
			findSearchEnd = strstr(findSearch+1, "\r\n");
			//printf("**** m_httpVersion=%s, m_statusCode=%d,findSearchEnd=%d\n",m_httpVersion.c_str(),(int)m_statusCode,(int)(findSearchEnd));
			if (findSearchEnd == NULL) return false;
		}else
		{
			m_requestURL = tstring(httpRequest+leftIndex, findSearch-httpRequest-leftIndex);
			//printf("******* IsComplete(): Get file name;%s,%s\n",httpRequest,m_requestURL.c_str());
			m_requestURL = URLDecode(m_requestURL.c_str());
			leftIndex += (m_requestURL.size()+1);

			GeServletInfo();
			findSearchEnd = strstr(findSearch, "\r\n");
			if (findSearchEnd == NULL) return false;

			m_httpVersion = tstring(findSearch+1, findSearchEnd-findSearch-1);
		}
		httpRequest = findSearchEnd + 2;
	}
	while (httpRequest != NULL)
	{
		if (!multipartyBoundary.empty())
		{
			tstring boundaryFind("\r\n");
			boundaryFind.append(multipartyBoundary);
			if (m_currentMultiPart.get() == NULL)
			{
				if (sotpCompare(httpRequest, boundaryFind.c_str(), leftIndex))
				{
					if (sotpCompare(httpRequest+boundaryFind.size()+leftIndex, "--\r\n", leftIndex))
					{
						// 最后完成1；
						//printf("******* final ok1 ************\n");
						return true;
					}
					httpRequest += boundaryFind.size() + 2;
					m_currentMultiPart = CGC_MULTIPART(multipartyBoundary);
					continue;
				}
			}else if (m_currentMultiPart->getFileName().empty())
			{
				// 不是文件参数，普通参数；
				//printf("***** httpRequest1=%s\n",httpRequest);
				if (sotpCompare(httpRequest, "\r\n", leftIndex))
				{
					httpRequest += (leftIndex + 2);
					//printf("***** httpRequest2=%s\n",httpRequest);
					findSearchEnd = strstr(httpRequest, boundaryFind.c_str());
					if (findSearchEnd == NULL) break;

					const std::string::size_type nFind = m_sReqContentType.find("application/x-www-form-urlencoded");
					const bool bUrlDecode = nFind != std::string::npos?true:false;

					// 查找到一个参数；
					tstring p(m_currentMultiPart->getName());
					if (bUrlDecode && !p.empty())
						p = URLDecode(p.c_str());
					const size_t nPSize = p.size();
					bool bIsVector = false;
					if (nPSize>2 && p.substr(nPSize-2,2)=="[]")
					{
						bIsVector = true;
						p = p.substr(0,nPSize-2);
					}
					tstring v(httpRequest,findSearchEnd-httpRequest);
					if (bUrlDecode && !v.empty())
						v = URLDecode(v.c_str());
					cgcValueInfo::pointer pValueInfo = CGC_VALUEINFO(v);
					cgcValueInfo::pointer pOldValueInfo;
					m_propertys.insert(p,pValueInfo,false,&pOldValueInfo);
					if (bIsVector)
					{
						if (pOldValueInfo.get()!=NULL)
						{
							pOldValueInfo->totype(cgcValueInfo::TYPE_VECTOR);
							pOldValueInfo->addVector(pValueInfo);
						}else
						{
							pValueInfo->totype(cgcValueInfo::TYPE_VECTOR);
						}
					}
					//m_propertys.setProperty(p, CGC_VALUEINFO(v));
					m_currentMultiPart->close();
					m_currentMultiPart->setParser(cgcNullParserBaseService);
					m_currentMultiPart.reset();
					//httpRequest = findSearchEnd;	// ***不处理下面代码，为了保持可读性；-2是前面
					//continue;
					// ***直接使用下面代码，效率更高；
					httpRequest = findSearchEnd+boundaryFind.size();
					if (sotpCompare(httpRequest, "--\r\n", leftIndex))
					{
						// 最后完成2；
						//printf("******* final ok2 ************\n");
						return true;
					}
					httpRequest += 2;
					m_currentMultiPart = CGC_MULTIPART(multipartyBoundary);
				}
			}
		}

		short nOffset = 1;	// 带空格2，不带空格1
		if (!FindHttpHeader(httpRequest,&findSearch,&findSearchEnd,&nOffset)) break;
		const tstring sParamReal(httpRequest, findSearch-httpRequest);
		tstring param(sParamReal);
		std::transform(param.begin(), param.end(), param.begin(), ::tolower);
		std::string value(findSearch+nOffset, findSearchEnd-findSearch-nOffset);
		//printf("**** %s : %s\n",sParamReal.c_str(), value.c_str());

		//findSearchEnd = strstr(httpRequest, "\r\n");
		//if (findSearchEnd == NULL) break;
		//const std::string sLine(httpRequest,findSearchEnd-httpRequest);
		//const std::string::size_type findParam = sLine.find(":");
		//if (findParam==std::string::npos) break;
		//const tstring sParamReal(sLine.substr(0,findParam));
		//tstring param(sParamReal);
		//std::transform(param.begin(), param.end(), param.begin(), ::tolower);
		//const short nOffset = sLine.c_str()[findParam+1]==' '?2:1;	// 带空格2，不带空格1
		//tstring value(sLine.substr(findParam+nOffset));

		//findSearch = strstr(httpRequest, ":");
		//if (findSearch == NULL) break;
		//findSearchEnd = strstr(findSearch+1, "\r\n");
		//if (findSearchEnd == NULL) break;
		//const tstring sParamReal(httpRequest, findSearch-httpRequest);
		//tstring param(sParamReal);
		//std::transform(param.begin(), param.end(), param.begin(), ::tolower);
		//const short nOffset = findSearch[1]==' '?2:1;	// 带空格2，不带空格1
		//tstring value(findSearch+nOffset, findSearchEnd-findSearch-nOffset);

		pOutHeader = true;
#ifdef USES_ZLIB
		//printf("IsComplete: %s: %s\n",sParamReal.c_str(),value.c_str());
		if (param=="content-encoding")
		{
			m_sReqContentEncoding = value;
			std::transform(m_sReqContentEncoding.begin(), m_sReqContentEncoding.end(), m_sReqContentEncoding.begin(), ::tolower);
			//printf("*********** %s : %s\n",Http_ContentEncoding.c_str(),m_sReqContentEncoding.c_str());
		}else if (param=="accept-encoding")
		{
			m_sReqAcceptEncoding = value;
			std::transform(m_sReqAcceptEncoding.begin(), m_sReqAcceptEncoding.end(), m_sReqAcceptEncoding.begin(), ::tolower);
		}
#endif
		if (param != "content-disposition")	// Http_ContentDisposition
		{
			m_pReqHeaders.insert(param, CGC_PARAMETER(sParamReal,value),false);
			//printf("IsComplete: %s: %s\n",sParamReal.c_str(),value.c_str());
			//cgcParameter::pointer pOldParameter;
			//m_pReqHeaders.insert(param, CGC_PARAMETER(sParamReal,value),false,&pOldParameter);
			//if (pOldParameter.get()!=NULL)
			//{
			//	pOldParameter->totype(cgcValueInfo::TYPE_VECTOR);
			//	pOldParameter->addVector(CGC_VALUEINFO(value));
			//}
			//m_pReqHeaders.setProperty(param, CGC_VALUEINFO(value));
		}
		//printf("IsComplete: %s: %s\n",sParamReal.c_str(),value.c_str());

		if (m_currentMultiPart.get() != NULL && param == "content-disposition")	// Http_ContentDisposition
		{
			bool doContinue = false;
			int leftIndexTemp = 0;
			while (!value.empty())
			{
				tstring valuetemp;
				std::string::size_type find = value.find(";");
				if (find == std::string::npos)
				{
					valuetemp = value;
					value.clear();
				}else
				{
					valuetemp = value.substr(0, find);
					value = value.substr(find+1);
				}

				if (sotpCompare(valuetemp.c_str(), "form-data;", leftIndexTemp))
				{
					// ?
					continue;
				}else if (sotpCompare(valuetemp.c_str(), "name=\"", leftIndexTemp))
				{
					m_currentMultiPart->setName(valuetemp.substr(leftIndexTemp+6, valuetemp.size()-leftIndexTemp-7));
					//printf("name=%s\n",m_currentMultiPart->getName().c_str());
				}else if (sotpCompare(valuetemp.c_str(), "filename=\"", leftIndexTemp))
				{
					tstring filename(valuetemp.substr(leftIndexTemp+10, valuetemp.size()-leftIndexTemp-11));
					find = filename.rfind("\\");
					if (find == std::string::npos)
						find = filename.rfind("/");
					if (find != std::string::npos)
						filename = filename.substr(find+1);
					m_currentMultiPart->setFileName(filename);
					if (m_currentMultiPart->getUploadFile()->getFileName().empty())
					{
						m_currentMultiPart->close();
						m_currentMultiPart->setParser(cgcNullParserBaseService);
						m_currentMultiPart.reset();
						tstring boundaryFind("\r\n");
						boundaryFind.append(multipartyBoundary);
						httpRequest = strstrl(httpRequest, boundaryFind.c_str(), requestSize-(httpRequest-httpRequestOld),boundaryFind.size());
						doContinue = true;
						break;
					}
				}
			}

			if (doContinue)
				continue;
		}else if (param == "content-type")	// Http_ContentType
		{
			int leftIndexTemp = 0;
			if (sotpCompare(value.c_str(), "multipart/form-data;", leftIndexTemp))
			{
				// isEnableUpload
				if (!theUpload.isEnableUpload())
					return false;

				value = value.substr(leftIndexTemp+20);
				if (sotpCompare(value.c_str(), "boundary=", leftIndexTemp))
				{
					multipartyBoundary = "--";
					multipartyBoundary.append(value.substr(leftIndexTemp+9));
					if (m_currentMultiPart.get() != NULL)
					{
						m_currentMultiPart->close();
						m_currentMultiPart->setParser(cgcNullParserBaseService);
						m_currentMultiPart.reset();
					}
				}
			}else if (m_currentMultiPart.get() != NULL && !m_currentMultiPart->getBoundary().empty())
			{
				// isEnableContentType
				if (!theUpload.isEnableContentType(value))
				{
					//printf("**** disable content type: %s\n",value.c_str());
					m_currentMultiPart->close();
					m_currentMultiPart->setParser(cgcNullParserBaseService);
					m_currentMultiPart.reset();
					return false;
				}
				m_currentMultiPart->setContentType(value);

				static tstring theTempSavePath;
				if (theTempSavePath.empty())
				{
					char tempSavePath[256];
					//std::string::size_type findpath = theUpload.getTempPath().find("/");
					//if (findpath == std::string::npos)
					//	findpath = theUpload.getTempPath().find("\\");
					//if (findpath == std::string::npos)
					{
						//sprintf(tempSavePath, "%s/%s", theApplication->getAppConfPath().c_str(), theUpload.getTempPath().c_str());
#ifdef WIN32
						if (theUpload.getTempPath().substr(1,1)==":")
							sprintf(tempSavePath, "%s", theUpload.getTempPath().c_str());
						else
							sprintf(tempSavePath, "c:\\%s", theUpload.getTempPath().c_str());
#else
						if (theUpload.getTempPath().substr(0,1)=="/")
							sprintf(tempSavePath, "%s", theUpload.getTempPath().c_str());
						else
							sprintf(tempSavePath, "/%s", theUpload.getTempPath().c_str());
#endif
						theTempSavePath = tempSavePath;
						if (m_fileSystemService.get() != NULL)
						{
							cgcValueInfo::pointer pOut = CGC_VALUEINFO(true);
							m_fileSystemService->callService("exists", CGC_VALUEINFO(theTempSavePath),pOut);
							if (!pOut->getBoolean())
							{
								m_fileSystemService->callService("create_directory", CGC_VALUEINFO(theTempSavePath));
							}
						}
						//}else
						//{
						//	sprintf(tempSavePath, "%s", theUpload.getTempPath().c_str());
					}
					theTempSavePath = tempSavePath;
				}
				m_currentMultiPart->open(theTempSavePath);

				tstring boundaryEnd("\r\n");
				boundaryEnd.append(m_currentMultiPart->getBoundary());
				const char * find = strstrl((const char*)findSearchEnd, boundaryEnd.c_str(), requestSize-(findSearchEnd-httpRequestOld), boundaryEnd.size());
				if (find == NULL)
				{
					bool bUnZipOk = false;
#ifdef USES_ZLIB
					if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
					{
						if (UnGZipDataCb((const unsigned char*)findSearchEnd+4,(uLong)(m_contentLength-size_t(findSearchEnd-httpRequestOld)-4),NULL,UnZipWriteCurrentMultiPartCallBack,(unsigned int)this)==Z_OK)
						{
							bUnZipOk = true;
						}
					}
#endif
					if (!bUnZipOk)
						m_currentMultiPart->write(findSearchEnd+4, m_contentLength-size_t(findSearchEnd-httpRequestOld)-4);
					return false;
				}
				bool bUnZipOk = false;
#ifdef USES_ZLIB
				if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
				{
					if (UnGZipDataCb((const unsigned char*)findSearchEnd+4,(uLong)(find-findSearchEnd-4),NULL,UnZipWriteCurrentMultiPartCallBack,(unsigned int)this)==Z_OK)
					{
						bUnZipOk = true;
					}
				}
#endif
				if (!bUnZipOk)
					m_currentMultiPart->write(findSearchEnd+4, find-findSearchEnd-4);
				m_currentMultiPart->close();
				m_currentMultiPart->setParser(cgcNullParserBaseService);
				m_files.push_back(m_currentMultiPart);

				// getMaxFileSize
				if (theUpload.getMaxFileSize() > 0 &&
					(int)(m_currentMultiPart->getUploadFile()->getFileSize() / 1024) > theUpload.getMaxFileSize())
				{
					m_statusCode = STATUS_CODE_413;
					m_currentMultiPart.reset();
					return true;	// 由api去处理
				}

				m_currentMultiPart.reset();
				// getMaxFileCount
				if (theUpload.getMaxFileCount() > 0 && (int)m_files.size() >= theUpload.getMaxFileCount())
				{
					return true;
				}

				httpRequest = find;
				continue;
			}
		}else if (param == "content-length")	// Http_ContentLength
		{
			m_contentSize = atoi(value.c_str());
			//printf("IsComplete: m_contentSize=%d\n",m_contentSize);
			if (m_contentSize > 0)
			{
				if ((theUpload.getMaxFileSize() > 0 && (int)(m_contentSize / 1024) > theUpload.getMaxFileSize())
					|| isGreaterMaxSize())
				{
					m_statusCode = STATUS_CODE_413;
					if (m_currentMultiPart.get()!=NULL)
					{
						m_currentMultiPart->close();
						m_currentMultiPart->setParser(cgcNullParserBaseService);
						m_files.push_back(m_currentMultiPart);
						m_currentMultiPart.reset();
					}
					return true;	// 由api去处理；
				}

				//if (multipartyBoundary.empty())
				{
					const char * findContent = strstrl(httpRequest, "\r\n\r\n", requestSize-(httpRequest-httpRequestOld), 4);
					//const char * find = strstrl(httpRequest, "\r\n\r\n", strlen(httpRequest), 4);
					if (findContent == NULL)
					{
						m_receiveSize = 0;
					}else
					{
						findContent += 4;
						m_receiveSize = requestSize-(int)(findContent-httpRequestOld);
					}
					//m_receiveSize = strlen(find);
					//printf("**** m_contentSize=%d,m_receiveSize=%d\n",m_contentSize,m_receiveSize);

					if (multipartyBoundary.empty())
					{
						if (m_receiveSize == 0)
						{
							m_currentMultiPart = CGC_MULTIPART("");
							//return false;
						}else
						{
							//int offset = 0;
							//if (httpRequest[strlen(httpRequest)-1] == '\n')
							//	offset += 1;
							//if (httpRequest[strlen(httpRequest)-2] == '\r')
							//	offset += 1;

							//if (m_contentData)
							//	delete[] m_contentData;
							//m_contentData = new char[m_contentSize+1];
							//strncpy(m_contentData, find, m_receiveSize);
							////strncpy(m_contentData, httpRequest+strlen(httpRequest)-m_contentSize-offset, m_contentSize);
							//m_contentData[m_receiveSize] = '\0';
							////printf("=================\n%s\n================\n",m_contentData);
							//m_queryString = m_contentData;
							if (m_bHttpResponse && !m_sResponseSaveFile.empty())
							{
								if (m_pResponseSaveFile==NULL)
								{
									m_pResponseSaveFile = fopen(m_sResponseSaveFile.c_str(), "wb");
								}
								if (m_pResponseSaveFile!=NULL && findContent!=NULL)
								{
									bool bUnZipOk = false;
#ifdef USES_ZLIB
									if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
									{
										if (UnGZipDataCb((const unsigned char*)findContent,(uLong)m_receiveSize,NULL,UnZipWriteFileCallBack,(unsigned int)m_pResponseSaveFile)==Z_OK)
										{
											bUnZipOk = true;
										}
									}
#endif
									if (!bUnZipOk)
										fwrite(findContent,1,m_receiveSize,m_pResponseSaveFile);
									if (m_receiveSize>=m_contentSize)
									{
										fclose(m_pResponseSaveFile);
										m_pResponseSaveFile = NULL;
									}
								}
							}else
							{
								if (findContent!=NULL)
									m_queryString = findContent;
								if (m_queryString.size()>m_contentSize)
									m_queryString = m_queryString.substr(0,m_contentSize);
								bool bUnZipOk = false;
#ifdef USES_ZLIB
								if (!m_queryString.empty())
								{
									if (m_sReqContentEncoding.find("gzip")!=std::string::npos)
									{
										m_postString.clear();
										if (UnGZipDataCb((const unsigned char*)m_queryString.c_str(),(uLong)m_queryString.size(),NULL,UnZipPostStringCallBack,(unsigned int)this)==Z_OK)
										{
											bUnZipOk = true;
											m_queryString = m_postString;
										}
									}
								}
#endif
								if (!bUnZipOk)
								{
									m_postString = m_queryString;
								}
							}
							if (m_contentSize > m_receiveSize)
							{
								m_currentMultiPart = CGC_MULTIPART("");
								// 下面会返回false
							}
						}
					}
				}
			}
		}else if (param == "cookie")	// Http_Cookie
		{
			while (!value.empty())
			{
				// 去掉头尾空格
				value.erase(0,value.find_first_not_of(" "));
				value.erase(value.find_last_not_of(" ") + 1);
				std::string::size_type find1 = value.find("=");
				if (find1 == std::string::npos) break;

				std::string::size_type find2 = value.find(";",find1+1);
				if (find2 == std::string::npos)
				{
					tstring cookieName = value.substr(0,find1);
					tstring cookieValue = value.substr(find1+1);
					m_pReqCookies.setProperty(cookieName, CGC_VALUEINFO(cookieValue));
					break;
				}
				tstring cookieName = value.substr(0,find1);
				tstring cookieValue = value.substr(find1+1,find2-find1-1);
				m_pReqCookies.setProperty(cookieName, CGC_VALUEINFO(cookieValue));
				value = value.substr(find2+1);
			}
		}
		httpRequest = findSearchEnd + 2;
	}

	if (!multipartyBoundary.empty() && m_currentMultiPart.get() == NULL)
	{
		m_currentMultiPart = CGC_MULTIPART(multipartyBoundary);
		return false;
	}else if (m_currentMultiPart.get() != NULL)
	//}else if (m_currentMultiPart.get() != NULL && m_currentMultiPart->getBoundary().empty())
	{
		// 数据未收完整
		if (httpRequest!=NULL && m_currentMultiPart->getFileName().empty() && m_currentMultiPart->getName().empty() && !m_currentMultiPart->getBoundary().empty())
		{
			// 普通参数，不是文件，并且当前未解析到名称，说明协议被截包；
			// 记下当前参数内容，下次再解析多一次；预防截包	如：Content-Disposition: form-data; name="
			m_sCurrentParameterData = httpRequest;
		}

		return false;
	}

	return true;
}

const char * CPpHttp::strstrl(const char * sourceBuffer, const char * findBuffer, size_t sourceSize, size_t fineSize)
{
	if (sourceBuffer == NULL || findBuffer == NULL) return NULL;
	if (fineSize > sourceSize) return NULL;

	size_t sourceIndex = 0;
	size_t findIndex = 0;
	while (sourceIndex < sourceSize)
	{
		if ((char)sourceBuffer[sourceIndex] != (char)findBuffer[findIndex])
		{
			if (findIndex == 0)
				sourceIndex++;
			else
				findIndex = 0;
			continue;
		}
		sourceIndex++;
		if (++findIndex == fineSize)
			return sourceBuffer + sourceIndex - findIndex;
	}

	return NULL;
}

bool CPpHttp::sotpCompare(const char * pBuffer, const char * pCompare, int & leftIndex)
{
	int i1 = 0, i2 = 0;
	leftIndex = 0;
	// 判断前面空格或者‘TAB’键；
	while (' ' == pBuffer[leftIndex] || '\t' == pBuffer[leftIndex])
	{
		leftIndex++;
	}

	i1 = leftIndex;
	while (pCompare[i2] != '\0')
	{
		if (pCompare[i2++] != pBuffer[i1] || '\0' == pBuffer[i1])
		{
			return false;
		}
		i1++;
	}
	return true;
}

bool CPpHttp::isGreaterMaxSize(void) const
{
	if (theUpload.getMaxUploadSize() <= 0) return false;

	int uploadSizes = 0;
	for (size_t i=0; i<m_files.size(); i++)
	{
		uploadSizes += m_files[i]->getUploadFile()->getFileSize();
		if (uploadSizes > theUpload.getMaxUploadSize())
			return true;
	}
	return false;
}
unsigned char CPpHttp::toHex(const unsigned char &x)
{
	return x > 9 ? x -10 + 'A': x + '0';
}

unsigned char CPpHttp::fromHex(const unsigned char &x)
{
	return isdigit(x) ? x-'0' : x-'A'+10;
}

std::string CPpHttp::URLEncode(const char *sIn)
{
	const size_t nLen = strlen(sIn);
	std::string sOut;
	for( size_t ix = 0; ix < nLen; ix++ )
	{
		unsigned char buf[4];
		memset( buf, 0, 4 );
		if( isalnum( (unsigned char)sIn[ix] ) )
		{      
			buf[0] = sIn[ix];
		}
		else
		{
			buf[0] = '%';
			buf[1] = toHex( (unsigned char)sIn[ix] >> 4 );
			buf[2] = toHex( (unsigned char)sIn[ix] % 16);
		}
		sOut += (char *)buf;
	}
	return sOut;
}

std::string CPpHttp::URLDecode(const char *sIn)
{
	const size_t nLen = strlen(sIn);
	std::string sOut;
	for( size_t ix = 0; ix < nLen; ix++ )
	{
		unsigned char ch = 0;
		if(sIn[ix]=='%')
		{
			ch = (fromHex(sIn[ix+1])<<4);
			ch |= fromHex(sIn[ix+2]);
			ix += 2;
		}
		else if(sIn[ix] == '+')
		{
			ch = ' ';
		}
		else
		{
			ch = sIn[ix];
		}
		sOut += (char)ch;
	}

	return sOut;

}

} // namespace mycp
