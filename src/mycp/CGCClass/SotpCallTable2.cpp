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

#ifdef _MSC_VER //WIN32
#pragma warning(disable:4267 4819 4996)
#endif // WIN32

#include "SotpCallTable2.h"
//#include <boost/format.hpp>

namespace mycp {
//#ifdef _UNICODE
//typedef boost::wformat tformat;
//#else
//typedef boost::format tformat;
//#endif // _UNICODE

#ifdef WIN32
#include "windows.h"
std::string str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
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

SotpCallTable2::SotpCallTable2(void)
: m_sEncoding(_T("GBK"))
, m_sSessionId(_T("")), m_sAppName(_T(""))
, m_sAccount(_T("")), m_sPasswd(_T(""))
, m_et(ModuleItem::ET_NONE)
, m_nSetAcceptEncoding(SOTP_DATA_ENCODING_UNKNOWN)
, m_nCurrentCallId(0)
, m_nCurrentSeq(0)
{}

std::string SotpCallTable2::toAckString(SOTP_PROTO_VERSION nVersion,unsigned short seq) const
{
	char lpszBuffer[60];
	if (nVersion==SOTP_PROTO_VERSION_21)
		sprintf(lpszBuffer,"B SOTP/2.1\n4%d\n",seq);
	else
		sprintf(lpszBuffer,"ACK SOTP/2.0\nSeq: %d\n",seq);
	return lpszBuffer;
}

std::string SotpCallTable2::toSesString(SOTP_PROTO_VERSION nVersion,int pt, const tstring & sValue, unsigned long cid, unsigned short seq, bool bNeedAck, const tstring& sPublicKey) const
{
	if (sValue.empty()) return _T("");
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		if (pt == SOTP_PROTO_TYPE_OPEN)
		{
			if (sPublicKey.empty())
			{
				char lpszBuffer[250];
				sprintf(lpszBuffer,(_T("%c SOTP/2.1\n")
					_T("%s")
					_T("F%d\n")	// * accept-encoding: gzip,deflate(?)
					_T("3%lu\n")
					_T("A%s\n")),SotpInt2Char(pt),sNeedAck.c_str(),m_nSetAcceptEncoding,cid,sValue.c_str());
				return lpszBuffer;
			}else
			{
				char lpszBuffer[8*1024];
				sprintf(lpszBuffer,(_T("%c SOTP/2.1\n")
					_T("%s")
					_T("F%d\n")	// * accept-encoding: gzip,deflate
					_T("3%lu\n")
					_T("A%s\n")
					_T("B%d\n%s\n")),SotpInt2Char(pt),sNeedAck.c_str(),m_nSetAcceptEncoding,cid,sValue.c_str(),(int)sPublicKey.size(),sPublicKey.c_str());
				return lpszBuffer;
			}
		}else
		{
			char lpszBuffer[250];
			sprintf(lpszBuffer,(_T("%c SOTP/2.1\n")
				_T("%s")
				_T("3%lu\n")
				_T("1%s\n")),SotpInt2Char(pt),sNeedAck.c_str(),cid,sValue.c_str());
			return lpszBuffer;
		}
	}else
	{
		std::string sProtocolType = _T("");
		std::string sNeedAck = _T("");
		switch (pt)
		{
		case SOTP_PROTO_TYPE_OPEN:
			sProtocolType = _T("OPEN");
			break;
		case SOTP_PROTO_TYPE_CLOSE:
			sProtocolType = _T("CLOSE");
			break;
		case SOTP_PROTO_TYPE_ACTIVE:
			sProtocolType = _T("ACTIVE");
			break;
		}
		/*
		unsigned int nBufferSize = 50+sValue.size();
		char * pResult = new char[nBufferSize];
		memset(pResult, 0, nBufferSize);
		int n = sprintf(pResult, _T("%s SOTP/2.0\n")
		_T("Cid: %d\n")
		_T("App: %s\n"), sProtocolType.c_str(), cid, sValue.c_str());
		pResult[n] = '\0';
		pOutSize = n;
		return pResult;
		*/
		if (bNeedAck)
		{
			sNeedAck = _T("NAck: 1\n");
			char lpszSeq[16];
			sprintf(lpszSeq,"Seq: %d\n",seq);
			sNeedAck.append(lpszSeq);
		}

		if (pt == SOTP_PROTO_TYPE_OPEN)
		{
			if (sPublicKey.empty())
			{
				char lpszBuffer[250];
				sprintf(lpszBuffer,(_T("%s SOTP/2.0\n")
					_T("%s")
					_T("Cid: %lu\n")
					_T("App: %s\n")),sProtocolType.c_str(),sNeedAck.c_str(),cid,sValue.c_str());
				return lpszBuffer;
			}else
			{
				char lpszBuffer[10240];
				sprintf(lpszBuffer,(_T("%s SOTP/2.0\n")
					_T("%s")
					_T("Cid: %lu\n")
					_T("App: %s\n")
					_T("Ssl: %d\n%s\n")),sProtocolType.c_str(),sNeedAck.c_str(),cid,sValue.c_str(),(int)sPublicKey.size(),sPublicKey.c_str());
				return lpszBuffer;
			}
		}else
		{
			char lpszBuffer[250];
			sprintf(lpszBuffer,(_T("%s SOTP/2.0\n")
				_T("%s")
				_T("Cid: %lu\n")
				_T("Sid: %s\n")),sProtocolType.c_str(),sNeedAck.c_str(),cid,sValue.c_str());
			return lpszBuffer;
		}
	}
}

std::string SotpCallTable2::toOpenSesString(SOTP_PROTO_VERSION nVersion,unsigned long cid, unsigned short seq, bool bNeedAck, const tstring& sPublicKey) const
{
	if (m_sAppName.empty()) return _T("");
	if (m_sAccount.empty())
	{
		return toSesString(nVersion,SOTP_PROTO_TYPE_OPEN, m_sAppName, cid, seq, bNeedAck, sPublicKey);
	}else
	{
		if (nVersion==SOTP_PROTO_VERSION_21)
		{
			std::string sNeedAck;
			if (bNeedAck)
			{
				char lpszSeq[20];
				sprintf(lpszSeq,"51\n4%d\n",seq);
				sNeedAck = lpszSeq;
			}

			if (sPublicKey.empty())
			{
				char lpszBuffer[1024];
				sprintf(lpszBuffer,(_T("1 SOTP/2.1\n")
					_T("%s")
					_T("F%d\n")	// * accept-encoding: gzip,deflate(?)
					_T("3%lu\n")
					_T("A%s\n")
					_T("C%s;pwd=%s;enc=%s\n")),sNeedAck.c_str(),m_nSetAcceptEncoding,cid,m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),ModuleItem::getEncryption(m_et).c_str());
				return lpszBuffer;
			}else
			{
				char lpszBuffer[8*1024];
				sprintf(lpszBuffer,(_T("1 SOTP/2.1\n")
					_T("%s")
					_T("F%d\n")	// * accept-encoding: gzip,deflate
					_T("3%lu\n")
					_T("A%s\n")
					_T("C%s;pwd=%s;enc=%s\n")
					_T("B%d\n%s\n")),sNeedAck.c_str(),m_nSetAcceptEncoding,cid,m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),ModuleItem::getEncryption(m_et).c_str(),(int)sPublicKey.size(),sPublicKey.c_str());
				return lpszBuffer;
			}
		}else
		{
			std::string sNeedAck = _T("");
			if (bNeedAck)
			{
				sNeedAck = _T("NAck: 1\n");
				char lpszSeq[16];
				sprintf(lpszSeq,"Seq: %d\n",seq);
				sNeedAck.append(lpszSeq);
			}

			/*
			unsigned int nBufferSize = 50+m_sAppName.size()+m_sAccount.size()+m_sPasswd.size();
			char * pResult = new char[nBufferSize];
			memset(pResult, 0, nBufferSize);
			int n = sprintf(pResult, _T("OPEN SOTP/2.0\n")
			_T("Cid: %d\n")
			_T("App: %s\n")
			_T("Ua: %s;pwd=%s;enc=%s\n"), cid, m_sAppName.c_str(), m_sAccount.c_str(), m_sPasswd.c_str(), ModuleItem::getEncryption(m_et).c_str());
			pResult[n] = '\0';
			pOutSize = n;
			return pResult;
			*/

			if (sPublicKey.empty())
			{
				char lpszBuffer[1024];
				sprintf(lpszBuffer,(_T("OPEN SOTP/2.0\n")
					_T("%s")
					_T("Cid: %lu\n")
					_T("App: %s\n")
					_T("Ua: %s;pwd=%s;enc=%s\n")),sNeedAck.c_str(),cid,m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),ModuleItem::getEncryption(m_et).c_str());
				return lpszBuffer;
			}else
			{
				char lpszBuffer[10240];
				sprintf(lpszBuffer,(_T("OPEN SOTP/2.0\n")
					_T("%s")
					_T("Cid: %lu\n")
					_T("App: %s\n")
					_T("Ua: %s;pwd=%s;enc=%s\n")
					_T("Ssl: %d\n%s\n")),sNeedAck.c_str(),cid,m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),ModuleItem::getEncryption(m_et).c_str(),(int)sPublicKey.size(),sPublicKey.c_str());
				return lpszBuffer;
			}
		}
	}
}

std::string SotpCallTable2::toAppCallString(SOTP_PROTO_VERSION nVersion,unsigned long cid, unsigned long nCallSign, const tstring & sCallName, unsigned short seq, bool bNeedAck)
{
	if (sCallName.empty()) return _T("");

	const std::string sParameters = GetParametersString(nVersion);
	m_parameters.clear();
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		char lpszBuffer[1024];
		if (m_sSessionId.empty())
		{
			// 2.0
			sprintf(lpszBuffer,(_T("A SOTP/2.1\n")
				_T("%s")
				_T("F%d\n")	// * accept-encoding: gzip,deflate
				_T("A%s\n")
				_T("C%s;pwd=%s;enc=\n")
				_T("3%lu\n")
				_T("6%lu\n")
				_T("7%s\n")),sNeedAck.c_str(),m_nSetAcceptEncoding,m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),cid,nCallSign,sCallName.c_str());
		}else
		{
			sprintf(lpszBuffer,(_T("A SOTP/2.1\n")
				_T("%s")
				_T("F%d\n")	// * accept-encoding: gzip,deflate
				_T("1%s\n")
				_T("3%lu\n")
				_T("6%lu\n")
				_T("7%s\n")),sNeedAck.c_str(),m_nSetAcceptEncoding,m_sSessionId.c_str(),cid,nCallSign,sCallName.c_str());
		}
		tstring result(lpszBuffer);
		result.append(sParameters);
		return result;
	}else
	{
		std::string sNeedAck = _T("");
		if (bNeedAck)
		{
			sNeedAck = _T("NAck: 1\n");
			char lpszSeq[16];
			sprintf(lpszSeq,"Seq: %d\n",seq);
			sNeedAck.append(lpszSeq);
		}

		char lpszBuffer[1024];
		if (m_sSessionId.empty())
		{
			// 2.0
			sprintf(lpszBuffer,(_T("CALL SOTP/2.0\n")
				_T("%s")
				_T("App: %s\n")
				_T("Ua: %s;pwd=%s;enc=\n")
				_T("Cid: %lu\n")
				_T("Sign: %lu\n")
				_T("Api: %s\n")),sNeedAck.c_str(),m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),cid,nCallSign,sCallName.c_str());
		}else
		{
			sprintf(lpszBuffer,(_T("CALL SOTP/2.0\n")
				_T("%s")
				_T("Sid: %s\n")
				_T("Cid: %lu\n")
				_T("Sign: %lu\n")
				_T("Api: %s\n")),sNeedAck.c_str(),m_sSessionId.c_str(),cid,nCallSign,sCallName.c_str());
		}
		tstring result(lpszBuffer);
		result.append(sParameters);
		return result;
	}
}

std::string SotpCallTable2::toAppCallHead(SOTP_PROTO_VERSION nVersion) const
{
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		char lpszBuffer[60];
		if (m_sSessionId.empty())
			sprintf(lpszBuffer,"A SOTP/2.1\nF%d\n",m_nSetAcceptEncoding);	// * accept-encoding: gzip,deflate
		else
			sprintf(lpszBuffer,"A SOTP/2.1\n1%s\nF%d\n",m_sSessionId.c_str(),m_nSetAcceptEncoding);
		return lpszBuffer;
	}else
	{
		if (m_sSessionId.empty())
			return "CALL SOTP/2.0\n";
		char lpszBuffer[60];
		sprintf(lpszBuffer,"CALL SOTP/2.0\nSid: %s\n",m_sSessionId.c_str());
		return lpszBuffer;
	}
}

std::string SotpCallTable2::toAppCallData(SOTP_PROTO_VERSION nVersion,unsigned long cid, unsigned long nCallSign, const tstring & sCallName, unsigned short seq, bool bNeedAck)
{
	if (sCallName.empty()) return _T("");

	const std::string sParameters = GetParametersString(nVersion);
	m_parameters.clear();
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		char lpszBuffer[1024];
		// 2.0
		sprintf(lpszBuffer,(_T("%s")
			_T("A%s\n")
			_T("C%s;pwd=%s;enc=\n")
			_T("3%lu\n")
			_T("6%lu\n")
			_T("7%s\n")),sNeedAck.c_str(),m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),cid,nCallSign,sCallName.c_str());
		tstring result(lpszBuffer);
		result.append(sParameters);
		return result;
	}else
	{
		std::string sNeedAck = _T("");
		if (bNeedAck)
		{
			sNeedAck = _T("NAck: 1\n");
			char lpszSeq[16];
			sprintf(lpszSeq,"Seq: %d\n",seq);
			sNeedAck.append(lpszSeq);
		}

		char lpszBuffer[1024];
		// 2.0
		sprintf(lpszBuffer,(_T("%s")
			_T("App: %s\n")
			_T("Ua: %s;pwd=%s;enc=\n")
			_T("Cid: %lu\n")
			_T("Sign: %lu\n")
			_T("Api: %s\n")),sNeedAck.c_str(),m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),cid,nCallSign,sCallName.c_str());
		tstring result(lpszBuffer);
		result.append(sParameters);
		return result;
	}
}
std::string SotpCallTable2::toSyncCallString(SOTP_PROTO_VERSION nVersion,unsigned long cid, const tstring& sSyncName, unsigned short seq, bool bNeedAck)
{
	//if (sCallName.empty()) return _T("");

	const std::string sParameters = GetParametersString(nVersion);
	m_parameters.clear();
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		char lpszBuffer[256];
		if (m_sSessionId.empty())
		{
			// 2.1
			sprintf(lpszBuffer,(_T("5 SOTP/2.1\n")
				_T("%s")
				_T("F%d\n")	// * accept-encoding: gzip,deflate
				_T("A%s\n")
				_T("3%lu\n")
				_T("7%s\n")),sNeedAck.c_str(),m_nSetAcceptEncoding,m_sAppName.c_str(),cid,sSyncName.c_str());
		}else
		{
			sprintf(lpszBuffer,(_T("5 SOTP/2.1\n")
				_T("%s")
				_T("F%d\n")	// * accept-encoding: gzip,deflate(?)
				_T("1%s\n")
				_T("3%lu\n")
				_T("7%s\n")),sNeedAck.c_str(),m_nSetAcceptEncoding,m_sSessionId.c_str(),cid,sSyncName.c_str());
		}
		tstring result(lpszBuffer);
		result.append(sParameters);
		return result;
	}else
	{
		return "";
		//std::string sNeedAck = _T("");
		//if (bNeedAck)
		//{
		//	sNeedAck = _T("NAck: 1\n");
		//	char lpszSeq[16];
		//	sprintf(lpszSeq,"Seq: %d\n",seq);
		//	sNeedAck.append(lpszSeq);
		//}

		//char lpszBuffer[1024];
		//if (m_sSessionId.empty())
		//{
		//	// 2.0
		//	sprintf(lpszBuffer,(_T("CALL SOTP/2.0\n")
		//		_T("%s")
		//		_T("App: %s\n")
		//		_T("Ua: %s;pwd=%s;enc=\n")
		//		_T("Cid: %lu\n")
		//		_T("Sign: %lu\n")
		//		_T("Api: %s\n")),sNeedAck.c_str(),m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),cid,nCallSign,sCallName.c_str());
		//	tstring result(lpszBuffer);
		//	result.append(sParameters);
		//	return result;
		//}else
		//{
		//	sprintf(lpszBuffer,(_T("CALL SOTP/2.0\n")
		//		_T("%s")
		//		_T("Sid: %s\n")
		//		_T("Cid: %lu\n")
		//		_T("Sign: %lu\n")
		//		_T("Api: %s\n")),sNeedAck.c_str(),m_sSessionId.c_str(),cid,nCallSign,sCallName.c_str());
		//	tstring result(lpszBuffer);
		//	result.append(sParameters);
		//	return result;
		//}
	}
}
std::string SotpCallTable2::toSyncCallHead(SOTP_PROTO_VERSION nVersion) const
{
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		char lpszBuffer[60];
		if (m_sSessionId.empty())
			sprintf(lpszBuffer,"5 SOTP/2.1\nF%d\n",m_nSetAcceptEncoding);
		else
			sprintf(lpszBuffer,"5 SOTP/2.1\n1%s\n",m_sSessionId.c_str());
		return lpszBuffer;
	}else
	{
		return "";
		//if (m_sSessionId.empty())
		//	return "CALL SOTP/2.0\n";
		//char lpszBuffer[60];
		//sprintf(lpszBuffer,"CALL SOTP/2.0\nSid: %s\n",m_sSessionId.c_str());
		//return lpszBuffer;
	}
}
std::string SotpCallTable2::toSyncCallData(SOTP_PROTO_VERSION nVersion,unsigned long cid, const tstring& sSyncName, unsigned short seq, bool bNeedAck)
{
	//if (sCallName.empty()) return _T("");

	const std::string sParameters = GetParametersString(nVersion);
	m_parameters.clear();
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		char lpszBuffer[1024];
		// 2.0
		sprintf(lpszBuffer,(_T("%s")
			_T("A%s\n")
			_T("C%s;pwd=%s;enc=\n")
			_T("3%lu\n")
			_T("7%s\n")),sNeedAck.c_str(),m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),cid,sSyncName.c_str());
		tstring result(lpszBuffer);
		result.append(sParameters);
		return result;

		//char lpszBuffer[256];
		//// 2.0
		//sprintf(lpszBuffer,(_T("%s")
		//	_T("A%s\n")
		//	_T("3%lu\n")
		//	_T("7%s\n")),sNeedAck.c_str(),m_sAppName.c_str(),cid,sSyncName.c_str());
		//tstring result(lpszBuffer);
		//result.append(sParameters);
		//return result;
	}else
	{
		return "";
		//std::string sNeedAck = _T("");
		//if (bNeedAck)
		//{
		//	sNeedAck = _T("NAck: 1\n");
		//	char lpszSeq[16];
		//	sprintf(lpszSeq,"Seq: %d\n",seq);
		//	sNeedAck.append(lpszSeq);
		//}

		//char lpszBuffer[1024];
		//// 2.0
		//sprintf(lpszBuffer,(_T("%s")
		//	_T("App: %s\n")
		//	_T("Ua: %s;pwd=%s;enc=\n")
		//	_T("Cid: %lu\n")
		//	_T("Sign: %lu\n")
		//	_T("Api: %s\n")),sNeedAck.c_str(),m_sAppName.c_str(),m_sAccount.c_str(),m_sPasswd.c_str(),cid,nCallSign,sCallName.c_str());
		//tstring result(lpszBuffer);
		//result.append(sParameters);
		//return result;
	}
}

unsigned char * SotpCallTable2::toAttachString(SOTP_PROTO_VERSION nVersion,const cgcAttachment::pointer& pAttach, unsigned int & pOutSize) const
{
	if (pAttach.get() == NULL || !pAttach->isHasAttach()) return NULL;
	int nBufferSize = 50+pAttach->getName().size()+pAttach->getAttachSize();
	unsigned char * pResult = new unsigned char[nBufferSize];
	memset(pResult, 0, nBufferSize);
	int n = 0;
	if (nVersion==SOTP_PROTO_VERSION_21)
		n = sprintf((char*)pResult, "9%s;at=%lld;ai=%lld;al=%d\n", pAttach->getName().c_str(), pAttach->getTotal(), pAttach->getIndex(), pAttach->getAttachSize());
	else
		n = sprintf((char*)pResult, "At: %s;at=%lld;ai=%lld;al=%d\n", pAttach->getName().c_str(), pAttach->getTotal(), pAttach->getIndex(), pAttach->getAttachSize());
	memcpy(pResult+n, pAttach->getAttachData(), pAttach->getAttachSize());
	pResult[n+pAttach->getAttachSize()] = '\n';
	pOutSize = n+pAttach->getAttachSize() + 1;
	return pResult;
}
unsigned char * SotpCallTable2::toSslDataString(SOTP_PROTO_VERSION nVersion,const unsigned char * pSslData, int nSize, unsigned int & pOutSize) const
{
	if (pSslData == NULL || nSize<=0) return NULL;
	int nBufferSize = nSize+30;
	unsigned char * pResult = new unsigned char[nBufferSize];
	memset(pResult, 0, nBufferSize);
	int n  = 0;
	if (nVersion==SOTP_PROTO_VERSION_21)
		n = sprintf((char*)pResult, "2%d\n", nSize);
	else
		n = sprintf((char*)pResult, "Sd: %d\n", nSize);
	memcpy(pResult+n, pSslData, nSize);
	pResult[n+nSize] = '\n';
	pOutSize = n+nSize + 1;
	return pResult;
}


std::string SotpCallTable2::toSessionResult(SOTP_PROTO_VERSION nVersion,int prototype, unsigned long cid, int retCode, const tstring & sSessionId, unsigned short seq, bool bNeedAck, const tstring& sSslPublicKey, int nAcceptEncoding) const
{
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		//std::string sType = _T("");
		//switch (prototype)
		//{
		//case SOTP_PROTO_TYPE_OPEN:
		//	sType = _T("OPEN");
		//	break;
		//case SOTP_PROTO_TYPE_CLOSE:
		//	sType = _T("CLOSE");
		//	break;
		//case SOTP_PROTO_TYPE_ACTIVE:
		//	sType = _T("ACTIVE");
		//	break;
		//case 10:
		//	sType = _T("OPEN");	// 用于临时打开SESSION
		//	break;
		//default:
		//	sType = _T("UNKNOWN");
		//	break;
		//}
		if (prototype==SOTP_PROTO_TYPE_CALL)
			prototype=SOTP_PROTO_TYPE_OPEN;

		if (prototype==SOTP_PROTO_TYPE_OPEN && !sSslPublicKey.empty())
		{
			char lpszBuffer[8*1024];
			sprintf(lpszBuffer,(_T("%c SOTP/2.1 %d\n")
				_T("1%s\n")
				_T("F%d\n")	// * accept-encoding: gzip,deflate(?)
				_T("%s")
				_T("3%lu\n")
				_T("B%d\n%s\n")),SotpInt2Char(prototype),retCode,sSessionId.c_str(),nAcceptEncoding,sNeedAck.c_str(),cid,(int)sSslPublicKey.size(),sSslPublicKey.c_str());
			return lpszBuffer;
		}else
		{
			char lpszBuffer[100];
			sprintf(lpszBuffer,(_T("%c SOTP/2.1 %d\n")
				_T("1%s\n")
				_T("F%d\n")	// * accept-encoding: gzip,deflate(?)
				_T("%s")
				_T("3%lu\n")),SotpInt2Char(prototype),retCode,sSessionId.c_str(),nAcceptEncoding,sNeedAck.c_str(),cid);
			return lpszBuffer;
		}
	}else
	{
		std::string sNeedAck = _T("");
		if (bNeedAck)
		{
			sNeedAck = _T("NAck: 1\n");
			char lpszSeq[16];
			sprintf(lpszSeq,"Seq: %d\n",seq);
			sNeedAck.append(lpszSeq);
		}

		std::string sType = _T("");
		switch (prototype)
		{
		case SOTP_PROTO_TYPE_OPEN:
			sType = _T("OPEN");
			break;
		case SOTP_PROTO_TYPE_CLOSE:
			sType = _T("CLOSE");
			break;
		case SOTP_PROTO_TYPE_ACTIVE:
			sType = _T("ACTIVE");
			break;
		case 10:
			sType = _T("OPEN");	// 用于临时打开SESSION
			break;
		default:
			sType = _T("UNKNOWN");
			break;
		}

		if (prototype==SOTP_PROTO_TYPE_OPEN && !sSslPublicKey.empty())
		{
			char lpszBuffer[10240];
			sprintf(lpszBuffer,(_T("%s SOTP/2.0 %d\n")
				_T("%s")
				_T("Cid: %lu\n")
				_T("Sid: %s\n")
				_T("Ssl: %d\n%s\n")),sType.c_str(),retCode,sNeedAck.c_str(),cid,sSessionId.c_str(),(int)sSslPublicKey.size(),sSslPublicKey.c_str());
			return lpszBuffer;
		}else
		{
			char lpszBuffer[100];
			sprintf(lpszBuffer,(_T("%s SOTP/2.0 %d\n")
				_T("%s")
				_T("Cid: %lu\n")
				_T("Sid: %s\n")),sType.c_str(),retCode,sNeedAck.c_str(),cid,sSessionId.c_str());
			return lpszBuffer;
		}
	}
}

std::string SotpCallTable2::toAppCallResult(SOTP_PROTO_VERSION nVersion,unsigned long cid, unsigned long sign, int retCode, unsigned short seq, bool bNeedAck)
{
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		const std::string responseValues = GetParametersString(nVersion);
		m_parameters.clear();
		char lpszBuffer[100];
		sprintf(lpszBuffer,(_T("A SOTP/2.1 %d\n")
			_T("%s")
			_T("3%lu\n")
			_T("6%lu\n")),retCode,sNeedAck.c_str(),cid,sign);
		tstring result(lpszBuffer);
		result.append(responseValues);
		return result;
	}else
	{
		std::string sNeedAck = _T("");
		if (bNeedAck)
		{
			sNeedAck = _T("NAck: 1\n");
			char lpszSeq[16];
			sprintf(lpszSeq,"Seq: %d\n",seq);
			sNeedAck.append(lpszSeq);
		}

		const std::string responseValues = GetParametersString(nVersion);
		m_parameters.clear();
		char lpszBuffer[100];
		sprintf(lpszBuffer,(_T("CALL SOTP/2.0 %d\n")
			_T("%s")
			_T("Cid: %lu\n")
			_T("Sign: %lu\n")),retCode,sNeedAck.c_str(),cid,sign);
		tstring result(lpszBuffer);
		result.append(responseValues);
		return result;
	}
}
std::string SotpCallTable2::toAppCallResultHead(SOTP_PROTO_VERSION nVersion,int retCode)
{
	char lpszBuffer[32];
	if (nVersion==SOTP_PROTO_VERSION_21)
		sprintf(lpszBuffer,"A SOTP/2.1 %d\n",retCode);
	else
		sprintf(lpszBuffer,"CALL SOTP/2.0 %d\n",retCode);
	return lpszBuffer;
}
std::string SotpCallTable2::toAppCallResultData(SOTP_PROTO_VERSION nVersion,unsigned long cid, unsigned long sign, unsigned short seq, bool bNeedAck)
{
	if (nVersion==SOTP_PROTO_VERSION_21)
	{
		std::string sNeedAck;
		if (bNeedAck)
		{
			char lpszSeq[20];
			sprintf(lpszSeq,"51\n4%d\n",seq);
			sNeedAck = lpszSeq;
		}

		const std::string responseValues = GetParametersString(nVersion);
		m_parameters.clear();
		char lpszBuffer[100];
		sprintf(lpszBuffer,(_T("%s")
			_T("3%lu\n")
			_T("6%lu\n")),sNeedAck.c_str(),cid,sign);
		tstring result(lpszBuffer);
		result.append(responseValues);
		return result;
	}else
	{
		std::string sNeedAck = _T("");
		if (bNeedAck)
		{
			sNeedAck = _T("NAck: 1\n");
			char lpszSeq[16];
			sprintf(lpszSeq,"Seq: %d\n",seq);
			sNeedAck.append(lpszSeq);
		}

		const std::string responseValues = GetParametersString(nVersion);
		m_parameters.clear();
		char lpszBuffer[100];
		sprintf(lpszBuffer,(_T("%s")
			_T("Cid: %lu\n")
			_T("Sign: %lu\n")),sNeedAck.c_str(),cid,sign);
		tstring result(lpszBuffer);
		result.append(responseValues);
		return result;
	}
}

std::string SotpCallTable2::toP2PTry(SOTP_PROTO_VERSION nVersion) const
{
	char lpszBuffer[32];
	if (nVersion==SOTP_PROTO_VERSION_21)
		sprintf(lpszBuffer,(_T("C SOTP/2.1\n")));
	else
		sprintf(lpszBuffer,(_T("P2P SOTP/2.0\n")));
	return lpszBuffer;
}

unsigned char* SotpCallTable2::toRtpCommand(const tagSotpRtpCommand& pRtpCommand, unsigned char* pSendBuffer, size_t& pOutSize)
{
	if (pSendBuffer==NULL)
		pSendBuffer = new unsigned char[64];	// SOTP_RTP_COMMAND_SIZE=25
	pOutSize = sprintf((char*)pSendBuffer,"4 SOTP/2.1\nD");
	memcpy(pSendBuffer+pOutSize,&pRtpCommand,SOTP_RTP_COMMAND_SIZE);
	pOutSize += (SOTP_RTP_COMMAND_SIZE+1);
	pSendBuffer[pOutSize-1] = '\n';
	return pSendBuffer;
}
unsigned char* SotpCallTable2::toRtpData(const tagSotpRtpDataHead& pRtpDataHead, const cgcAttachment::pointer& pAttachment, unsigned char* pSendBuffer, size_t& pOutSize)
{
	if (pSendBuffer==NULL)
	{
		const size_t nSizeTemp = 20+SOTP_RTP_DATA_HEAD_SIZE+pAttachment->getAttachSize();
		pSendBuffer = new unsigned char[nSizeTemp];
	}
	pOutSize = sprintf((char*)pSendBuffer,"4 SOTP/2.1\nE");
	memcpy(pSendBuffer+pOutSize,&pRtpDataHead,SOTP_RTP_DATA_HEAD_SIZE);
	pOutSize += (SOTP_RTP_DATA_HEAD_SIZE);
	memcpy(pSendBuffer+pOutSize,(const void*)(const unsigned char *)pAttachment->getAttachData(),pAttachment->getAttachSize());
	pOutSize += (pAttachment->getAttachSize()+1);
	pSendBuffer[pOutSize-1] = '\n';
	return pSendBuffer;
}

void SotpCallTable2::setParameter(const cgcParameter::pointer & parameter, bool bSetForce)
{
	if (parameter.get() != NULL)
	{
		if (!bSetForce && parameter->getType() == cgcValueInfo::TYPE_STRING && parameter->empty())
			return;
		m_parameters.remove(parameter->getName());
		m_parameters.insert(parameter->getName(), parameter);
	}
}

void SotpCallTable2::addParameter(const cgcParameter::pointer & parameter, bool bAddForce)
{
	if (parameter.get() != NULL)
	{
		if (!bAddForce && parameter->getType() == cgcValueInfo::TYPE_STRING && parameter->empty())
			return;
		m_parameters.insert(parameter->getName(), parameter);
	}
}

void SotpCallTable2::addParameters(const std::vector<cgcParameter::pointer> & parameters, bool bAddForce)
{
	for (std::size_t i=0; i<parameters.size(); i++)
	{
		addParameter(parameters[i], bAddForce);
	}
}

unsigned long SotpCallTable2::getNextCallId(void)
{
	boost::mutex::scoped_lock lock(m_mutexCid);
	return (++m_nCurrentCallId)==0?1:m_nCurrentCallId;
}
unsigned short SotpCallTable2::getNextSeq(void)
{
	boost::mutex::scoped_lock lock(m_mutexReq);
	return ++m_nCurrentSeq;
}

std::string SotpCallTable2::GetParametersString(SOTP_PROTO_VERSION nVersion) const
{
	//boost::format fRV(_T("Pv: %s;pt=%s;pl=%d\n%s\n"));
	char lpszBuffer[1024];
	std::string result = _T("");

	// ???
	AUTO_CONST_RLOCK(m_parameters);
	//long index=0;
	cgcParameterMap::const_iterator pConstIter;
	for (pConstIter=m_parameters.begin(); pConstIter!=m_parameters.end(); pConstIter++)
	{
		const cgcParameter::pointer& parameter = pConstIter->second;
#ifdef WIN32
		cgcValueInfo::ValueType nValueType = parameter->getType();
#endif
		tstring paramType(parameter->typeString());
		tstring paramName(parameter->getName());
		tstring paramValue(parameter->toString());
#ifdef WIN32
		if (nValueType == cgcValueInfo::TYPE_STRING && m_sEncoding == _T("UTF8"))
		{
			paramValue = str_convert(paramValue.c_str(),CP_ACP,CP_UTF8);
		}
#endif

		// 2.0
		size_t paramValueLength = paramValue.size();
#ifdef _UNICODE
		const wchar_t * pTemp = paramValue.c_str();
		size_t targetLen = 0;
		errno_t err = wcsrtombs_s(&targetLen, NULL, 0, &pTemp, paramValue.size(), NULL);
		if (err == 0 && targetLen > 0)
		{
			paramValueLength = targetLen-1;
		}
#endif // _UNICODE
		if (nVersion==SOTP_PROTO_VERSION_21)
			sprintf(lpszBuffer,"8%s;pt=%s;pl=%d\n",paramName.c_str(),paramType.c_str(),paramValueLength);
		else
			sprintf(lpszBuffer,"Pv: %s;pt=%s;pl=%d\n",paramName.c_str(),paramType.c_str(),paramValueLength);
		tstring sParam(lpszBuffer);
		sParam.append(paramValue);
		sParam.append("\n");
		//tstring sParam((fRV%paramName.c_str()%paramType.c_str()%paramValueLength%paramValue.c_str()).str());
#ifdef _DEBUG
		// ???		tstring sParamValueUtf8 = cgcString::Convert(parameter.getValue(), CP_ACP, CP_UTF8);
//		tstring sT(parameter.getValue());
//		tstring sParamValueGb2312 = cgcString::Convert(sT, CP_UTF8, CP_ACP);
#endif
		result.append(sParam.c_str());
		//index++;
	}
	return result;
}

} // namespace mycp

