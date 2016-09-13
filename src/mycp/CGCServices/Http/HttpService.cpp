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

#include "CgcTcpClient.h"

#ifdef WIN32
#pragma warning(disable:4800)
#include <windows.h>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif // WIN32


// cgc head
//#include <CGCBase/app.h>
#include <CGCBase/cgcService.h>
using namespace cgc;

tstring getipbyname(const char * name)
{
	struct   hostent   *   pHost;
	int   i;  
	pHost   =   gethostbyname(name);  
	for(   i   =   0;   pHost!=   NULL   &&   pHost-> h_addr_list[i]!=   NULL;   i++   )  
	{
		return inet_ntoa   (*(struct   in_addr   *)pHost-> h_addr_list[i]);
	}
	return _T("");
}


const tstring const_http_version = "HTTP/1.1";

class CHttpService
	: public cgcServiceInterface
{
public:
	typedef boost::shared_ptr<CHttpService> pointer;

	static CHttpService::pointer create(void)
	{
		return CHttpService::pointer(new CHttpService());
	}

protected:
	virtual tstring serviceName(void) const {return _T("HTTPSERVICE");}

	bool GetRequestInfo(const cgcValueInfo::pointer& inParam,tstring& pOutHost,tstring& pOutPort,tstring& pOutUrl,tstring& pOutHeader,tstring& pOutData) const
	{
		if (inParam.get() == NULL) return false;
		if (inParam->getType() == cgcValueInfo::TYPE_MAP)
		{
			cgcValueInfo::pointer varFind;
			if (!inParam->getMap().find("host", varFind))
				return false;
			pOutHost = varFind->toString();
			if (inParam->getMap().find("port", varFind))
				pOutPort = varFind->toString();
			else
				pOutPort = "80";
			if (!inParam->getMap().find("url", varFind))
				return false;
			pOutUrl = varFind->toString();
			if (inParam->getMap().find("header", varFind))
				pOutHeader = varFind->getStr();
			if (inParam->getMap().find("data", varFind))
				pOutData = varFind->toString();
			return true;
		}else if (inParam->getType() == cgcValueInfo::TYPE_STRING)
		{
			// http://ip:port/aaa.html?xxx=aaa
			// http://www.entboost.com/abc.csp?xxx=bbb
			tstring sFullUrl = inParam->getStr();
			tstring::size_type find = sFullUrl.find("://");
			if (find != tstring::npos)	// remove before http://
			{
				sFullUrl = sFullUrl.substr(find+3);	// www.entboost.com/abc.csp?xxx=bbb
			}
			// find url
			find = sFullUrl.find("/");
			if (find != tstring::npos)
			{
				pOutUrl = sFullUrl.substr(find);		// "/abc.csp?xxx=bbb"
				sFullUrl = sFullUrl.substr(0,find);	// "www.entboost.com" OR "ip:port"
			}
			// find port
			find = sFullUrl.find(":");
			if (find != tstring::npos)
			{
				pOutHost = sFullUrl.substr(0,find);	// "www.entboost.com" OR "ip"
				pOutPort = sFullUrl.substr(find+1);
			}else
			{
				pOutPort = "80";
			}
			return true;
		}else if (inParam->getType() == cgcValueInfo::TYPE_VECTOR && !inParam->empty())
		{
			// 0=url	// http://ip:port/abc.html?xxx=xxx
			// 1=data	// for post data
			const tstring sFullUrl = inParam->getVector()[0]->getStr();
			if (!GetRequestInfo(CGC_VALUEINFO(sFullUrl),pOutHost,pOutPort,pOutUrl,pOutHeader,pOutData))
				return false;
			if (inParam->size()>=2)
			{
				pOutData = inParam->getVector()[1]->getStr();
			}
			return true;
		}else
		{
			return false;
		}
	}
	virtual bool callService(const tstring& function, const cgcValueInfo::pointer& inParam, cgcValueInfo::pointer outParam)
	{
		if (function == "GET")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			tstring sHost;		// www.abc.com
			tstring sPort;		// default 80
			tstring sUrl;		// "/abc.csp?p=v"
			tstring sHeader;
			tstring sData;
			if (!GetRequestInfo(inParam,sHost,sPort,sUrl,sHeader,sData))
				return false;
			tstring sHostIp(sHost);
			//tstring sHostIp = getipbyname(sHost.c_str());
			//if (sHostIp.empty()) return false;

			tstring sHttpRequest;
			sHttpRequest.append(_T("GET "));
			sHttpRequest.append(sUrl);
			sHttpRequest.append(_T(" HTTP/1.1\r\n"));
			sHttpRequest.append(_T("Host: "));
			sHttpRequest.append(sHost);
			sHttpRequest.append(_T("\r\n"));
			sHttpRequest.append(_T("User-Agent: MYCP HttpService/5.0\r\n"));
			sHttpRequest.append(_T("Accept: */*\r\n"));
			sHttpRequest.append(_T("Accept-Language: zh-cn,zh;q=0.5\r\n"));
			sHttpRequest.append(_T("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7\r\n"));
			sHttpRequest.append(_T("Keep-Alive: 115\r\n"));
			sHttpRequest.append(_T("Connection: keep-alive\r\n"));
			if (!sHeader.empty())
				sHttpRequest.append(sHeader);
			sHttpRequest.append(_T("\r\n"));

			sHostIp.append(":");
			sHostIp.append(sPort);			// default 80
			return HttpRequest(sHostIp, sHttpRequest, outParam);
		}else if (function == "POST")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			if (inParam->getType() != cgcValueInfo::TYPE_MAP) return false;

			tstring sHost;		// www.abc.com
			tstring sPort;		// default 80
			tstring sUrl;		// "/abc.csp?p=v"
			tstring sHeader;
			tstring sData;		// for POST command
			if (!GetRequestInfo(inParam,sHost,sPort,sUrl,sHeader,sData))
				return false;

			//cgcValueInfo::pointer varFind;
			//if (!inParam->getMap().find("host", varFind))
			//	return false;
			//sHost = varFind->toString();
			//if (!inParam->getMap().find("port", varFind))
			//	return false;
			//sPort = varFind->toString();
			//if (!inParam->getMap().find("url", varFind))
			//	return false;
			//sUrl = varFind->toString();
			//if (inParam->getMap().find("data", varFind))
			//	sData = varFind->toString();

			tstring sHostIp(sHost);
			//tstring sHostIp = getipbyname(sHost.c_str());
			//if (sHostIp.empty()) return false;

			tstring sHttpRequest;
			sHttpRequest.append(_T("POST "));
			sHttpRequest.append(sUrl);
			sHttpRequest.append(_T(" HTTP/1.1\r\n"));
			sHttpRequest.append(_T("Host: "));
			sHttpRequest.append(sHost);
			sHttpRequest.append(_T("\r\n"));
			sHttpRequest.append(_T("User-Agent: MYCP HttpService/5.0\r\n"));
			sHttpRequest.append(_T("Accept: */*\r\n"));
			sHttpRequest.append(_T("Accept-Language: zh-cn,zh;q=0.5\r\n"));
			sHttpRequest.append(_T("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7\r\n"));
			if (!sHeader.empty())
				sHttpRequest.append(sHeader);
			if (!sData.empty())
			{
				char sContentLength[20];
				sprintf(sContentLength, "%d", sData.size());
				sHttpRequest.append(_T("Content-Length: "));
				sHttpRequest.append(sContentLength);
				sHttpRequest.append(_T("\r\n\r\n"));
				sHttpRequest.append(sData.c_str());
			}
			sHttpRequest.append(_T("\r\n"));

			sHostIp.append(":");
			sHostIp.append(sPort);			// default 80
			return HttpRequest(sHostIp, sHttpRequest, outParam);
		}else
		{
			return false;
		}
		return true;
	}

	protected:
		bool HttpRequest(const tstring & sHostIp, const tstring & sHttpRequest, cgcValueInfo::pointer outParam)
		{
			bool result = false;
			cgc::CgcTcpClient::pointer tcpClient = cgc::CgcTcpClient::create();
			if (tcpClient->startClient(sHostIp, 0) == 0)
			{
				tcpClient->sendData((const unsigned char *)sHttpRequest.c_str(), sHttpRequest.size());
				int counter = 0;
				do
				{
#ifdef WIN32
					Sleep(500);
#else
					usleep(500000);
#endif
				}while ((++counter < 2*30) && !tcpClient->IsDisconnection() && tcpClient->GetReceiveData().empty());	// 30S
				tstring::size_type responseSize = tcpClient->GetReceiveData().size();
				counter = 0;
				while (++counter < 6 && responseSize > 0)	// wait more 6S
				{
#ifdef WIN32
					Sleep(1000);
#else
					usleep(1000000);
#endif
					if (tcpClient->GetReceiveData().size() == responseSize)
						break;
					responseSize = tcpClient->GetReceiveData().size();
				}
				const tstring & sResponse = tcpClient->GetReceiveData();

				// Transfer-Encoding: chunked


				if (!sResponse.empty() && sResponse.size() > const_http_version.size())
				{
					outParam->totype(cgcValueInfo::TYPE_VECTOR);
					outParam->reset();
					tstring::size_type find = sResponse.find(const_http_version);
					bool bFindHttpState = false;
					if (find != tstring::npos)
					{
						tstring::size_type find2 = sResponse.find("\r\n", find+const_http_version.size());
						if (find2 != tstring::npos)
						{
							bFindHttpState = true;
							tstring sHttpState = sResponse.substr(find+const_http_version.size()+1, find2 - find - const_http_version.size()-1);
							outParam->operator +=(CGC_VALUEINFO(sHttpState));
						}
					}
					if (!bFindHttpState)
					{
						outParam->operator +=(CGC_VALUEINFO((tstring)"200"));
						//return false;
					}

					// 特殊处理，用于淘宝应用
					find = sResponse.find("top-bodylength: ", const_http_version.size());
					if (find == tstring::npos)
					{
						find = sResponse.find("Transfer-Encoding: chunked\r\n", const_http_version.size());
						if (find != tstring::npos)
						{
							// 先简单处理，直接取出二个回车后一回车开始位置，到0回车结束位置
//HTTP/1.1 200 OK
//Server: Apache-Coyote/1.1
//Content-Language: cn-ZH
//Transfer-Encoding: chunked
//Date: Wed, 29 Jun 2011 02:26:52 GMT
//
//10
//eyJzdGF0dXMiOjB9
//0

							find = sResponse.find("\r\n\r\n", find+10);
							// 后一回车开始位置
							find = sResponse.find("\r\n", find+5);
							tstring::size_type findend = sResponse.find("0\r\n", find+4);
							if (find != tstring::npos && findend != tstring::npos)
							{
								tstring sHttpData = sResponse.substr(find+2, findend-find-2);
								outParam->operator +=(CGC_VALUEINFO(sHttpData));
							}else
							{
								//outParam->operator +=(CGC_VALUEINFO((tstring)""));
							}
						}else
						{
							find = sResponse.find("\r\n\r\n", const_http_version.size());
							if (find != tstring::npos)
							{
								tstring sHttpData = sResponse.substr(find+4);
								outParam->operator +=(CGC_VALUEINFO(sHttpData));
							}else
							{
								//outParam->operator +=(CGC_VALUEINFO((tstring)""));
							}
						}
					}else
					{
						// 淘宝应用
						find = sResponse.find("<?xml", const_http_version.size());
						if (find != tstring::npos)
						{
							tstring::size_type find2 = sResponse.find("<!--top", find);
							if (find2 == tstring::npos)
							{
								tstring sHttpData = sResponse.substr(find);
								outParam->operator +=(CGC_VALUEINFO(sHttpData));
							}else
							{
								tstring sHttpData = sResponse.substr(find, find2-find);
								outParam->operator +=(CGC_VALUEINFO(sHttpData));
							}
						}
					}
				}else
				{
					outParam->totype(cgcValueInfo::TYPE_STRING);
					outParam->setStr(sResponse);
				}

				result = true;
			}

			tcpClient->stopClient();
			tcpClient.reset();

			return result;
		}

};

CHttpService::pointer gServicePointer;

extern "C" bool CGC_API CGC_Module_Init(void)
{
	return true;
}

extern "C" void CGC_API CGC_Module_Free(void)
{
	gServicePointer.reset();
}

extern "C" void CGC_API CGC_GetService(cgcServiceInterface::pointer & outService, const cgcValueInfo::pointer& parameter)
{
	if (gServicePointer.get() == NULL)
	{
		gServicePointer = CHttpService::create();
	}
	outService = gServicePointer;
}
