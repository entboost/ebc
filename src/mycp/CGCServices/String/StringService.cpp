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
#pragma warning(disable:4996)
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

#include <string.h>
#include <stdarg.h>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
//#include <CGCBase/app.h>
#include <CGCBase/cgcService.h>
#include "md5.h"
#include "Base64.h"

// cgc head
#include "cgcString.h"
using namespace cgc;

class CStringService
	: public cgcString
{
public:
	typedef boost::shared_ptr<CStringService> pointer;

	static CStringService::pointer create(void)
	{
		return CStringService::pointer(new CStringService());
	}

	virtual tstring serviceName(void) const {return _T("STRINGSERVICE");}

protected:
	virtual bool callService(const tstring& function, const cgcValueInfo::pointer& inParam, cgcValueInfo::pointer outParam)
	{
		if (function == "md5")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;

			const tstring & sInString = inParam->getStr();
			MD5 md5;
			md5.update((const unsigned char*)sInString.c_str(), (unsigned int)sInString.size());
			md5.finalize();
			outParam->totype(cgcValueInfo::TYPE_STRING);
			outParam->setStr(std::string((const char*)md5.hex_digest()));
		//}else if (function == "tohex")
		//{
		//	if (inParam.get() == NULL) return false;

		//	const tstring & sInString = inParam->getStr();
		//	tstring sOutString = toHex(sInString);
		//	//返回结果
		//	if (outParam.get() == NULL)
		//	{
		//		inParam->setStr(sOutString);
		//	}else
		//	{
		//		outParam->totype(cgcValueInfo::TYPE_STRING);
		//		outParam->setStr(sOutString);
		//	}
		}else if (function == "url_enc")
		{
			if (inParam.get() == NULL) return false;

			const tstring & sInString = inParam->getStr();
			std::string sOutString = URLEncode(sInString.c_str());

			//返回结果
			if (outParam.get() == NULL)
			{
				inParam->setStr(sOutString);
			}else
			{
				outParam->totype(cgcValueInfo::TYPE_STRING);
				outParam->setStr(sOutString);
			}
		}else if (function == "url_dec")
		{
			if (inParam.get() == NULL) return false;

			const tstring & sInString = inParam->getStr();
			std::string sOutString = URLDecode(sInString.c_str());

			//返回结果
			if (outParam.get() == NULL)
			{
				inParam->setStr(sOutString);
			}else
			{
				outParam->totype(cgcValueInfo::TYPE_STRING);
				outParam->setStr(sOutString);
			}
		}else if (function == "utf8")
		{
			if (inParam.get() == NULL) return false;

			const tstring & sInString = inParam->getStr();
#ifdef WIN32
			std::string sOutString = convert(sInString.c_str(), CP_ACP, CP_UTF8);
#else
			std::string sOutString;
#endif
			//返回结果
			if (outParam.get() == NULL)
			{
				inParam->setStr(sOutString);
			}else
			{
				outParam->totype(cgcValueInfo::TYPE_STRING);
				outParam->setStr(sOutString);
			}
		}else if (function == "utf82ansi")
		{
			if (inParam.get() == NULL) return false;

			const tstring & sInString = inParam->getStr();
#ifdef WIN32
			std::string sOutString = convert(sInString.c_str(), CP_UTF8, CP_ACP);
#else
			std::string sOutString;
#endif
			//返回结果
			if (outParam.get() == NULL)
			{
				inParam->setStr(sOutString);
			}else
			{
				outParam->totype(cgcValueInfo::TYPE_STRING);
				outParam->setStr(sOutString);
			}
		}else if (function == "base64_enc")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			const tstring & sInString = inParam->getStr();

			outParam->totype(cgcValueInfo::TYPE_STRING);
			if (sInString.empty())
			{
				outParam->setStr(tstring(_T("")));
			}else
			{
				int size = sInString.size()*1.4+1;
				char * buffer = new char[size];
				unsigned int len = Base64Encode(buffer, (const unsigned char*)sInString.c_str(), (unsigned int)sInString.size());
				if (len > 0)
				{
					outParam->setStr(std::string(buffer, len));
				}else
				{
					outParam->setStr(tstring(_T("")));
				}
				delete[] buffer;
			}
		}else if (function == "base64_dec")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			const tstring & sInString = inParam->getStr();

			outParam->totype(cgcValueInfo::TYPE_STRING);
			if (sInString.empty())
			{
				outParam->setStr(tstring(_T("")));
			}else
			{
				int size = sInString.size()*0.8+1;
				char * buffer = new char[size];
				unsigned int len = Base64Decode((unsigned char*)buffer, sInString.c_str());
				if (len > 0)
				{
					outParam->setStr(std::string(buffer, len));
				}else
				{
					outParam->setStr(tstring(_T("")));
				}
				delete[] buffer;
			}
		}else if (function == "upper")
		{
			if (inParam.get() == NULL) return false;
			tstring sInString = inParam->getStr();

			boost::to_upper(sInString);
			if (outParam.get() == NULL)
			{
				inParam->setStr(sInString);
			}else
			{
				outParam->totype(cgcValueInfo::TYPE_STRING);
				outParam->setStr(sInString);
			}
		}else if (function == "lower")
		{
			if (inParam.get() == NULL) return false;
			tstring sInString = inParam->getStr();
			boost::to_lower(sInString);
			if (outParam.get() == NULL)
			{
				inParam->setStr(sInString);
			}else
			{
				outParam->totype(cgcValueInfo::TYPE_STRING);
				outParam->setStr(sInString);
			}
		}else if (function == "parse_httpurl")
		{
			if (inParam.get() == NULL || outParam.get() == NULL) return false;
			const tstring & sInString = inParam->getStr();

			outParam->totype(cgcValueInfo::TYPE_MAP);
			outParam->reset();
			if (!sInString.empty())
			{
				tstring parameter;
				std::string::size_type find = 0;
				do
				{
					// Get [parameter=value]
					tstring::size_type findParameter = sInString.find("&", find+1);
					if (findParameter == std::string::npos)
					{
						parameter = sInString.substr(find, sInString.size()-find);
					}else
					{
						parameter = sInString.substr(find, findParameter-find);
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

					tstring p = parameter.substr(0, findParameter);
					tstring v = parameter.substr(findParameter+1, parameter.size()-findParameter);
					outParam->insertMap(p, CGC_VALUEINFO(v));
				}while (find != std::string::npos);
			}

		}else
		{
			return false;
		}

		return true;
	}

	//std::string toHex(const std::string &src)
	//{
	//	std::string result;
	//	const char * srcBuffer = src.c_str();

	//	for (size_t i=0; i<src.size(); i++)
	//	{
	//		char srcChar = srcBuffer[i];
	//		unsigned char hexBuffer[3];
	//		if (srcChar > 0xF)
	//		{
	//			hexBuffer[2] = 0;
	//			hexBuffer[1] = toHex(srcChar & 0xF);
	//			hexBuffer[0] = toHex((srcChar>>4) & 0xF);
	//		}else
	//		{
	//			hexBuffer[1] = 0;
	//			hexBuffer[0] = toHex(srcChar & 0xF);
	//		}
	//		result.append((const char*)hexBuffer);
	//	}
	//	return result;
	//}

	unsigned char toHex(const unsigned char &x)
	{
		return x > 9 ? x -10 + 'A': x + '0';
	}

	unsigned char fromHex(const unsigned char &x)
	{
		return isdigit(x) ? x-'0' : x-'A'+10;
	}

	std::string URLEncode(const char *sIn)
	{
		std::string sOut;
		for( size_t ix = 0; ix < strlen(sIn); ix++ )
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
	};

	std::string URLDecode(const char *sIn)
	{
		std::string sOut;
		for( size_t ix = 0; ix < strlen(sIn); ix++ )
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



	//////////////////////////////////////////////////////////////
	virtual std::string W2Char(const wchar_t * strSource) const
	{
		std::string result = "";
		size_t targetLen = wcsrtombs(NULL, &strSource, 0, NULL);
		if (targetLen > 0)
		{
			char * pTargetData = new char[targetLen];
			memset(pTargetData, 0, (targetLen)*sizeof(char));
			wcsrtombs(pTargetData, &strSource, targetLen, NULL);
			result = pTargetData;
			delete[] pTargetData;
		}
		return result;
	}
	virtual std::wstring Char2W(const char * strSource) const
	{
		std::wstring result = L"";
		size_t targetLen = mbsrtowcs(NULL, (const char**)&strSource, 0, 0);
		if (targetLen > 0)
		{
			targetLen += 1;

			wchar_t * pTargetData = new wchar_t[targetLen];
			memset(pTargetData, 0, (targetLen)*sizeof(wchar_t));
			mbsrtowcs(pTargetData, &strSource, targetLen, 0);
			result = pTargetData;
			delete[] pTargetData;   
		}
		return result;
	}
	static void W2C(const wchar_t *pw, char *pc)
	{
		if (*pw < 255)
			*pc = *pw;
		else
		{
			*pc++ = *pw >> 8 ;
			*pc = *pw;
		}
	}
	//
	// char to wchar_t
	static void C2W(const char * pc, wchar_t * pw)
	{
		if (*pc >= 0)
			*pw = *pc;
		else
			*pw = (((pc[0] & 0xff) << 8) + (pc[1] & 0xff));
	}

	//
	// 把Unicode转换成UTF-8
	static void UnicodeToUTF_8(char* pOut, const wchar_t* pText)
	{
		// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
		char* pchar = (char *)pText;
		pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
		pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
		pOut[2] = (0x80 | (pchar[0] & 0x3F));
	}

	// 把UTF-8转换成Unicode
	static void UTF_8ToUnicode(wchar_t* pOut, const char *pText)
	{
		char* uchar = (char *)pOut;
		uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
		uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
	}
	//////////////////////////////////////////////////////////////
#ifdef WIN32
	virtual std::string convert(const char * strSource, int sourceCodepage, int targetCodepage) const
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

		std::string result = pTargetData;
		//	tstring result(pTargetData, targetLen);
		delete[] pUnicode;
		delete[] pTargetData;
		return   result;
	}
#endif

	//////////////////////////////////////////////////////////////
	virtual std::string format(const char * format,...) const
	{
		if (format == 0) return "";

		char formatmsg[MAX_STRING_FORMAT_SIZE+1];
		va_list   vl;
		va_start(vl, format);
		int len = vsnprintf(formatmsg, MAX_STRING_FORMAT_SIZE, format, vl);
		va_end(vl);
		if (len > MAX_STRING_FORMAT_SIZE)
			len = MAX_STRING_FORMAT_SIZE;

		formatmsg[len] = '\0';

		return std::string(formatmsg, len);
	}
	virtual const tstring & replace(tstring & strSource, const tstring & strFind, const tstring &strReplace) const
	{
		tstring::size_type pos=0;
		tstring::size_type findlen=strFind.size();
		tstring::size_type replacelen=strReplace.size();
		while ((pos=strSource.find(strFind, pos)) != tstring::npos)
		{
			strSource.replace(pos, findlen, strReplace);
			pos += replacelen;
		}
		return strSource;
	}

	virtual const tstring & toUpper(tstring & strSource) const
	{
		std::transform(strSource.begin(), strSource.end(), strSource.begin(), toupper);
		return strSource;
	}
	virtual const tstring & toLower(tstring & strSource) const
	{
		std::transform(strSource.begin(), strSource.end(), strSource.begin(), tolower);
		return strSource;
	}

	//////////////////////////////////////////////////////////////
	virtual int toInt(const char * strValue, int defaultValue) const
	{
		if (strValue == 0) return defaultValue;
		try
		{
			return atoi(strValue);
		}catch(std::exception const &)
		{
		}catch(...)
		{}
		return defaultValue;
	}
	virtual long toLong(const char * strValue, long defaultValue) const
	{
		if (strValue == 0) return defaultValue;
		try
		{
			return atol(strValue);
		}catch(std::exception const &)
		{
		}catch(...)
		{
		}
		return defaultValue;
	}
	virtual unsigned long toULong(const char * strValue, unsigned long defaultValue) const
	{
		if (strValue == 0) return defaultValue;
		try
		{
			char * stopstring;
			int base = 10;

			return strtoul(strValue, &stopstring, base);
		}catch(std::exception const &)
		{
		}catch(...)
		{
		}
		return defaultValue;
	}
	virtual double toDouble(const char * strValue, double defaultValue) const
	{
		if (strValue == 0) return defaultValue;
		try
		{
			return atof(strValue);
		}catch(std::exception const &)
		{
		}catch(...)
		{
		}
		return defaultValue;
	}

};

CStringService::pointer gServicePointer;

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
		gServicePointer = CStringService::create();
	}
	outService = gServicePointer;
}
