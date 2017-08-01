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

// PpHttp.h file here
// http protocol parser
#ifndef __pp_http_h__
#define __pp_http_h__

#define USES_ZLIB

#include "dlldefine.h"
#include "../CGCBase/cgcParserHttp.h"
#include "AttributesImpl.h"
#include "XmlParseUpload.h"
#ifdef USES_ZLIB
#include "../ThirdParty/stl/zlib.h"
#endif

namespace mycp {

class CPpHttp
	: public cgcParserHttp
{
private:
	virtual tstring serviceName(void) const {return _T("PARSERHTTP");}
	virtual void stopService(void) {}

	tstring m_host;
	tstring m_account;
	tstring m_secure;

	// 1 servlet	: "GET /servlet/ModuleName?p=v&... HTTP/1.1"
	// 2 servlet	: "GET /servlet/ModuleName/FunctionName?p=v&... HTTP/1.1"
	// 3 csp		: "GET /ModuleName/FunctionName.csp?p=v&... HTTP/1.1"
	tstring m_moduleName;		// 12 "ModuleName"; 3 ""
	tstring m_functionName;		// 1 "doGET"; 2 "FunctionName"; 3 ""
	tstring m_httpVersion;		// "HTTP/1.1"
	tstring m_restVersion;		// v01,...

	size_t m_contentLength;
	HTTP_METHOD m_method;		// GET
	bool m_bHttpResponse;
	tstring m_sResponseSaveFile;
	FILE * m_pResponseSaveFile;
	tstring m_requestURL;		// "/path/file.csp?p=v&..."
	tstring m_requestURI;		// "/path/file.csp"
	tstring m_queryString;		// "p=v&..."
	tstring m_postString;		// "p=v&..."
	tstring m_fileName;			// file.csp
	unsigned int m_nRangeFrom;		// default 0=start
	unsigned int m_nRangeTo;			// default 0=end

	bool m_keepAlive;
	int m_keepAliveInterval;
	//char * m_contentData;
	size_t m_contentSize;
	size_t m_receiveSize;

	CLockMap<tstring,cgcParameter::pointer> m_pReqHeaders;
	//AttributesImpl m_pReqHeaders;
	tstring m_sMyCookieSessionId;
	AttributesImpl m_pReqCookies;
	CLockMap<tstring,cgcValueInfo::pointer> m_propertys;
	//AttributesImpl m_propertys;
	CLockMap<tstring,cgcValueInfo::pointer> m_pResHeaders;
	//std::vector<cgcKeyValue::pointer> m_pResHeaders;
	tstring m_sResCookieSessionId;
	//int m_nCookieExpiresMinute;		// COOKIE过期时间，单位分钟，默认0
	CLockMap<tstring,cgcCookieInfo::pointer> m_pResCookies;
	//CLockMap<tstring,cgcValueInfo::pointer> m_pResCookies;
	//std::vector<cgcKeyValue::pointer> m_pResCookies;

	// application/octet-stream

	// Content-Disposition: form-data; name="file"; filename="a.txt"

	// image/gif或者image/jpg

	//Content-Disposition: form-data; name="filename"; filename="D:\Temp\TestFile.txt"
	// Content-Type: text/plain
	// Content-Type属性没有的话表示普通的字符串数据，如"company"＝XX

	HTTP_STATUSCODE m_statusCode;
	bool m_addDateHeader;
	bool m_addContentLength;
	bool m_bTransferEncoding;
	bool m_bResponseHeader;
	bool m_bResponseBodyOnly;
	tstring m_sReqContentType;
	tstring m_sReqContentEncoding;	// Content-Encoding
	tstring m_sReqAcceptEncoding;		// Accept-Encoding
	tstring m_sResContentType;
	tstring m_sResCharset;	// utf-8,gb2312,...
	tstring m_sLocation;

	char * m_resultBuffer;
	size_t m_bodySize;
	size_t m_bodyBufferSize;

	tstring m_forwardFromURL;

	std::vector<cgcMultiPart::pointer> m_files;
	cgcMultiPart::pointer m_currentMultiPart;
	tstring	m_sCurrentParameterData;

	cgcServiceInterface::pointer m_fileSystemService;
	//tstring m_sTempSavePath;
protected:
	//////////////////////////////////////////////////
	// Request
	const tstring& getHost(void) const {return m_host;}

	virtual const tstring& getModuleName(void) const {return m_moduleName;}
	virtual const tstring& getFunctionName(void) const {return m_functionName;}

	virtual const tstring& getHttpVersion(void) const {return m_httpVersion;}
	virtual const tstring& getRestVersion(void) const {return m_restVersion;}

	virtual HTTP_METHOD getHttpMethod(void) const {return m_method;}
	virtual bool isServletRequest(void) const {return !m_moduleName.empty() && !m_functionName.empty();}
	//virtual size_t getContentLength(void) const {return m_contentLength;}
	virtual const tstring& getRequestURL(void) const {return m_requestURL;}
	virtual const tstring& getRequestURI(void) const {return m_requestURI;}
	virtual const tstring& getQueryString(void) const {return m_queryString;}
	virtual const tstring& getFileName(void) const {return m_fileName;}
	virtual unsigned int getRangeFrom(void) const {return m_nRangeFrom;}
	virtual unsigned int getRangeTo(void) const {return m_nRangeTo;}
	virtual bool isKeepAlive(void) const {return m_keepAlive&&m_keepAliveInterval>=0;}
	virtual int getKeepAlive(void) const {return m_keepAliveInterval;}
	virtual void setKeepAlive(int nKeepAliveInterval) {m_keepAliveInterval = nKeepAliveInterval;}	// -1:Connection: close
	virtual size_t getContentLength(void) const {return m_contentSize;}
	virtual const char* getContentData(void) const {return m_queryString.c_str();}
	//virtual const char* getContentData(void) const {return m_contentData;}

	virtual cgcMultiPart::pointer getMultiPart(void) const{return m_currentMultiPart;}
	virtual bool getUploadFile(std::vector<cgcUploadFile::pointer>& outFiles) const;
	// cookie
	virtual bool isEmptyCookieMySessionId(void) const {return m_sMyCookieSessionId.empty();}
	virtual tstring getCookieMySessionId(void) const {return m_sMyCookieSessionId;}
	//virtual void setCookieExpiresMinute(unsigned int nExpiresMinute = 10) {m_nCookieExpiresMinute = nExpiresMinute;}
	virtual tstring getCookie(const tstring & name, const tstring& defaultValue) const;
	virtual bool getCookies(std::vector<cgcKeyValue::pointer>& outCookies) const {return m_pReqCookies.getSPropertys(outCookies);}
	// Head
	virtual tstring getHeader(const tstring & header, const tstring& defaultValue) const;
	virtual bool getHeaders(std::vector<cgcKeyValue::pointer>& outHeaders) const;
	//virtual bool getHeaders(std::vector<cgcKeyValue::pointer>& outHeaders) const {return m_pReqHeaders.getSPropertys(outHeaders);}
	// Parameter
	virtual cgcValueInfo::pointer getParameter(const tstring & paramName) const;
	//virtual cgcValueInfo::pointer getParameter(const tstring & paramName) const {return m_propertys.getProperty(paramName);}
	//bool getParameter(const tstring & paramName, std::vector<cgcValueInfo::pointer>& outParameters) const {return m_propertys.getProperty(paramName, outParameters);}
	virtual bool getParameters(std::vector<cgcKeyValue::pointer>& outParameters) const;
	//bool getParameters(std::vector<cgcKeyValue::pointer>& outParameters) const {return m_propertys.getSPropertys(outParameters);}
#ifdef USES_ZLIB
	uLong m_nZipCallBackDataSize;
	static bool MyZipDataCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned long nUserData);
	static bool UnZipPostStringCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned long nUserData);
	static bool UnZipWriteFileCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned long nUserData);
	static bool UnZipWriteCurrentMultiPartCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned long nUserData);
#endif
	/////////////////////////////////////////////
	// Response
	virtual void println(const char * text, size_t size);
	virtual void println(const tstring& text);
	virtual void write(const char * text, size_t size);
	virtual void write(const tstring& text);
	virtual void newline(void);
	virtual void reset(void);
	virtual void init(void);

	virtual void setStatusCode(HTTP_STATUSCODE statusCode) {m_statusCode = statusCode;}
	virtual HTTP_STATUSCODE getStatusCode(void) const {return m_statusCode;}
	virtual void addDateHeader(bool v=true) {m_addDateHeader = v;}
	virtual void addContentLength(bool v=true) {m_addContentLength = v;}
	virtual size_t getBodySize(void) const {return m_bodySize;}
	virtual const tstring& getForwardFromURL(void) const {return m_forwardFromURL;}

	virtual void setCookieMySessionId(const tstring& sMySessionId);
	virtual void setCookie(const tstring& name, const tstring& value);
	virtual void setCookie(const cgcCookieInfo::pointer& pCookieInfo);
	virtual void setHeader(const tstring& name, const tstring& value);
	virtual void setContentType(const tstring& contentType);
	virtual const tstring& getContentType(void) const {return m_sResContentType;}
	virtual const tstring& getReqContentType(void) const {return m_sReqContentType;}

	virtual tstring getAccount(void) const {return m_account;}
	virtual tstring getSecure(void) const {return m_secure;}

	virtual void forward(const tstring& url);
	virtual void location(const tstring& url);

	virtual const char * getHttpResult(size_t& outSize) const;
	virtual void setParseCallback(cgcParserCallback* pCallback) {}
	virtual bool doParse(const unsigned char * requestData, size_t requestSize,const char* sEncoding);
	void GeServletInfo(void);
	void GetPropertys(const std::string& sString, bool bUrlDecode);
	void GeRequestInfo(void);
	bool IsComplete(const char * httpRequest, size_t requestSize,bool& pOutHeader);
	const char * strstrl(const char * sourceBuffer, const char * findBuffer, size_t sourceSize, size_t fineSize);
	bool sotpCompare(const char * pBuffer, const char * pCompare, int & leftIndex);
	bool isGreaterMaxSize(void) const;
	unsigned char toHex(const unsigned char &x);
	unsigned char fromHex(const unsigned char &x);
	std::string URLEncode(const char *sIn);
	std::string URLDecode(const char *sIn);

public:
	CPpHttp(void);
	virtual ~CPpHttp(void);

	char * m_pHeaderBufferTemp;
	char * m_pHeaderTemp;
	char * m_pCookieTemp;

	XmlParseUpload theUpload;
	void setFileSystemService(cgcServiceInterface::pointer v) {m_fileSystemService = v;}
	bool isHttpResponse(void) const {return m_bHttpResponse;}
	void SetResponseSaveFile(const tstring& v) {m_sResponseSaveFile = v;}
	size_t GetReceiveSize(void) const {return m_receiveSize;}
};

#ifdef WIN32
inline std::string cgc_str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
{
	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strSource, -1, NULL, 0);
	if (unicodeLen <= 0) return "";

	wchar_t * pUnicode = new wchar_t[unicodeLen];
	memset(pUnicode,0,(unicodeLen)*sizeof(wchar_t));

	MultiByteToWideChar(sourceCodepage, 0, strSource, -1, (wchar_t*)pUnicode, unicodeLen);

	char * pTargetData = 0;
	int targetLen = WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);
	if (targetLen <= 0)
	{
		delete[] pUnicode;
		return "";
	}

	pTargetData = new char[targetLen];
	memset(pTargetData, 0, targetLen);

	WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);

	std::string result(pTargetData);
	//	tstring result(pTargetData, targetLen);
	delete[] pUnicode;
	delete[] pTargetData;
	return   result;
}
inline std::string CGC_ACP2UTF8(const char* sString)
{
	return cgc_str_convert(sString,CP_ACP,CP_UTF8);
}
inline std::string CGC_UTF82ACP(const char* sString)
{
	return cgc_str_convert(sString,CP_UTF8,CP_ACP);
}
#else
#include <iconv.h>
inline int cgc_code_convert(const char *from_charset,const char *to_charset,const char *inbuf,size_t inlen,char *outbuf,size_t outlen)  
{  
	iconv_t cd = 0;  
	//int rc;  
	char **pin = (char**)&inbuf;  
	char **pout = &outbuf;  

	int ret = 0;
	try
	{
		cd = iconv_open(to_charset,from_charset);  
		if (cd==0)  
			return -1;  
		memset(outbuf,0,outlen);  
		if ((int)iconv(cd,pin,&inlen,pout,&outlen) == -1)  
		{
			ret = -1;
			//return -1;  
		}
	}catch(std::exception const &)
	{
	}catch(...){
	}
	iconv_close(cd);  
	return ret;
}  
inline int cgc_u2g(const char *inbuf,size_t inlen,char *outbuf,size_t outlen)  
{  
	return cgc_code_convert("utf-8","gbk",inbuf,inlen,outbuf,outlen);  
	//return cgc_code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);  
}  
inline int cgc_g2u(const char *inbuf,size_t inlen,char *outbuf,size_t outlen)  
{  
	return cgc_code_convert("gbk","utf-8",inbuf,inlen,outbuf,outlen);  
	//return cgc_code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);  
}
inline int CGC_XXX2UTF8(const char* from_charset, const char *inbuf,size_t inlen, std::string& pOut)
{
	char* pBuf = new char[inlen*2+10];
	const int ret = cgc_code_convert(from_charset,"utf-8",inbuf,inlen,pBuf,inlen*2+10);
	if (ret==0)
		pOut = pBuf;
	delete[] pBuf;
	return ret;
}
#endif

} // namespace mycp

#endif // __pp_http_h__
