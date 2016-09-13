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

typedef unsigned int			UINT;
typedef unsigned int      WPARAM;
typedef long							LPARAM;

#include "CGCClass.h"
#include "ParseCgcSotp2.h"
#include "../ThirdParty/stl/rsa.h"
#include "../ThirdParty/stl/aes.h"
#define USES_DATA_ENCODING
#ifdef USES_DATA_ENCODING
#include "../ThirdParty/stl/zlib.h"
#endif

namespace mycp {

#ifdef WIN32
#include "windows.h"
#pragma comment(lib, "Ws2_32.lib")
std::string ParseCgcSotp2::str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
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

ParseCgcSotp2::ParseCgcSotp2(void)
: m_pCallback(NULL)
, m_nSotpVersion(SOTP_PROTO_VERSION_20)
, /*m_nCgcProto(0),*/ m_nProtoType(SOTP_PROTO_TYPE_UNKNOWN),m_bHasSeq(false),m_seq(0), m_bNeedAck(false)
, m_sSid(_T("")), m_sApp(_T("")), m_sApi(_T("")), m_sSslPublicKey(""), m_nAcceptEncoding(SOTP_DATA_ENCODING_UNKNOWN),m_nDataEncoding(SOTP_DATA_ENCODING_UNKNOWN),m_nDataSize(0),/*m_pUnZipDataTBuffer(NULL),m_nUnZipDataBufferSize(0),m_nUnZipDataBufferIndex(0),*/m_pSslDecryptData(NULL),m_nSslDecrptyBufferSize(0)
, m_nCallId(0), m_nSign(0), m_bResulted(false), m_nResultValue(0)
, m_sAccount(_T("")), m_sPasswd(_T(""))
, m_et(ModuleItem::ET_NONE)
{
	m_bRtpCommand = false;
	m_bRtpData = false;
	m_attach = cgcAttachment::create();
}
ParseCgcSotp2::~ParseCgcSotp2(void)
{
	//if (m_pUnZipDataTBuffer!=NULL)
	//{
	//	free(m_pUnZipDataTBuffer);
	//	m_pUnZipDataTBuffer = NULL;
	//}
	if (m_pSslDecryptData!=NULL)
	{
		delete[] m_pSslDecryptData;
		m_pSslDecryptData = NULL;
	}
	m_parameterMap.clear(false);

	//Reset(false);
	m_attach.reset();
}

/*
cgcParameter::pointer ParseCgcSotp2::getParameter(const tstring & paramName) const
{
	cgcParameter::pointer result;
	m_parameterMap.find(paramName, result);
	return result;
}

bool ParseCgcSotp2::getParameter(const tstring & paramName, std::vector<cgcParameter::pointer>& outParams) const
{
	return m_parameterMap.find(paramName, outParams);
}

tstring ParseCgcSotp2::getParameterString(const tstring & paramName, const tstring & defaultValue) const
{
	cgcParameter::pointer result;
	if (!m_parameterMap.find(paramName, result))
		return defaultValue;
	return result->toString();
}*/
/*
int ParseCgcSotp2::getClusters(ClusterSvrList & listResult)
{
	// get clusters list, return get out count
	int result = m_custerSvrList.size();
	ClusterSvrList::iterator pIter;
	for (pIter=m_custerSvrList.begin(); pIter!=m_custerSvrList.end(); pIter++)
	{
		ClusterSvr * pClusterSvr = *pIter;
		listResult.push_back(new ClusterSvr(*pClusterSvr));
	}
	return result;
}
*/
void ParseCgcSotp2::Reset(bool bResetAttach)
{
	//m_nCgcProto = 0;
	m_pCallback = NULL;
	m_nSotpVersion = SOTP_PROTO_VERSION_20;
	m_nProtoType = SOTP_PROTO_TYPE_UNKNOWN;
	m_bHasSeq = false;
	m_seq = 0;
	m_bNeedAck = false;
	//m_sProtoValue.clear();
	m_sApp = _T("");
	m_sApi = _T("");
	m_sSid = _T("");
	m_sSslPublicKey.clear();
	m_nAcceptEncoding = SOTP_DATA_ENCODING_UNKNOWN;
	m_nDataEncoding = SOTP_DATA_ENCODING_UNKNOWN;
	m_nDataSize = 0;
	//if (m_pUnZipDataTBuffer!=NULL)
	//{
	//	free(m_pUnZipDataTBuffer);
	//	m_pUnZipDataTBuffer = NULL;
	//}
	//m_nUnZipDataBufferSize = 0
	//m_nUnZipDataBufferIndex = 0;
	//if (m_pSslDecryptData!=NULL)
	//{
	//	delete[] m_pSslDecryptData;
	//	m_pSslDecryptData = NULL;
	//}
	m_nCallId = 0;
	m_nSign = 0;
	m_bResulted = false;
	m_nResultValue = 0;
	m_sAccount = "";
	m_sPasswd = "";
	//m_et = ModuleItem::ET_NONE;

	m_parameterMap.clear();
	m_bRtpCommand = false;
	m_bRtpData = false;
	memset(&m_pSotpRtpCommand,0,SOTP_RTP_COMMAND_SIZE);
	memset(&m_pSotpRtpDataHead,0,SOTP_RTP_DATA_HEAD_SIZE);

	if (bResetAttach && m_attach->isHasAttach())
		m_attach = cgcAttachment::create();
	//m_attach->clear();

	//for_each(m_custerSvrList.begin(), m_custerSvrList.end(), DeletePtr());
	//m_custerSvrList.clear();
}
void ParseCgcSotp2::addParameter(const cgcParameter::pointer& parameter)
{
	if (parameter.get() == 0) return;
	m_parameterMap.insert(parameter->getName(), parameter);
}

//inline bool IsVersion21(const unsigned char* pBuffer)
//{
//	return memcmp(pBuffer+1," SOTP/2.1",9)==0;
//	//return memcmp(pBuffer+1,"21",2)==0;
//	//return pBuffer[1]=='2'&& pBuffer[2]=='1';
//}

bool ParseCgcSotp2::parseBuffer(const unsigned char * pBuffer,size_t nBufferSize,const char* sEncoding)
{
	if (pBuffer == 0) return false;
	if (m_attach.get() !=NULL && m_attach->isHasAttach())
	{
		m_attach = cgcAttachment::create();
	}
	m_sEncoding = sEncoding;
	// Get Proto Type.
	const char * pCgcProto = NULL;
	if (memcmp(pBuffer+1," SOTP/2.1",9)==0)
	{
		m_nSotpVersion = SOTP_PROTO_VERSION_21;
		pCgcProto = (const char*)(pBuffer + 2);
	}else
	{
		pCgcProto = strstr((const char*)pBuffer, "SOTP/2.0");
		if (pCgcProto == NULL)
			return false;
		m_nSotpVersion = SOTP_PROTO_VERSION_20;
	}

	if (m_nSotpVersion==SOTP_PROTO_VERSION_21)
	{
		// OPEN='1' CLOSE='2' ACTIVE='3' RTP='4' CALL='A' ACK='B' P2P='B'
		const int nProtoType = SotpChar2Int(pBuffer[0]);
		if (nProtoType<SOTP_PROTO_TYPE_OPEN || nProtoType>SOTP_PROTO_TYPE_P2P)
			return false;
		m_nProtoType = (SOTP_PROTO_TYPE)nProtoType;
	}else
	{
		int leftIndex = 0;
		if (sotpCompare((const char *)pBuffer, "ACK", leftIndex))
		{
			//m_nCgcProto = 3;
			m_nProtoType = SOTP_PROTO_TYPE_ACK;
		}else if (sotpCompare((const char *)pBuffer, "CALL", leftIndex))
		{
			//m_nCgcProto = 2;
			m_nProtoType = SOTP_PROTO_TYPE_CALL;
		}else if (sotpCompare((const char *)pBuffer, "OPEN", leftIndex))
		{
			//m_nCgcProto = 1;
			m_nProtoType = SOTP_PROTO_TYPE_OPEN;
		}else if (sotpCompare((const char *)pBuffer, "CLOSE", leftIndex))
		{
			//m_nCgcProto = 1;
			m_nProtoType = SOTP_PROTO_TYPE_CLOSE;
		}else if (sotpCompare((const char *)pBuffer, "ACTIVE", leftIndex))
		{
			//m_nCgcProto = 1;
			m_nProtoType = SOTP_PROTO_TYPE_ACTIVE;
		}else if (sotpCompare((const char *)pBuffer, "P2P", leftIndex))
		{
			//m_nCgcProto = 4;
			m_nProtoType = SOTP_PROTO_TYPE_P2P;
		}
		/*else if (sotpCompare(v, "QUERY", leftIndex))
		{
		m_nCgcProto = 3;
		m_nProtoType = 20;
		}else if (sotpCompare(pBuffer, "VERIFY", leftIndex))
		{
		m_nCgcProto = 3;
		m_nProtoType = 30;
		}*/else
		return false;
	}
	// Get SOTP/2.1 [CODE]
	// Get SOTP/2.0 [CODE]
	const char * pNextLineFind = strstr(pCgcProto, "\n");
	if (pNextLineFind == NULL)
		return false;
	if (pNextLineFind-pCgcProto-9 > 0)
	{
		m_bResulted = true;
		const std::string sTemp(pCgcProto+9, pNextLineFind-pCgcProto-9);
		try
		{
			m_nResultValue = atoi(sTemp.c_str());
		}catch(...)
		{
			m_nResultValue = 0;
		}
	}

	// ? 要调整并且需要测试！
	pNextLineFind = strstr(pNextLineFind, "\n");
	while (pNextLineFind != NULL && pNextLineFind[0] == '\n')
	{
		pNextLineFind = parseOneLine(pNextLineFind+1,nBufferSize-((const unsigned char*)pNextLineFind-pBuffer));
	}
	return m_nProtoType >= SOTP_PROTO_TYPE_OPEN;
	//return m_nCgcProto > 0;
}

//bool ParseCgcSotp2::MyUnZipDataCallBack(const unsigned char* pData, unsigned long nSize, unsigned int nUserData)
//{
//	if (nSize>0 && nUserData!=NULL)
//	{
//		ParseCgcSotp2* pOwner = (ParseCgcSotp2*)nUserData;
//		if (pOwner->m_nUnZipDataBufferIndex+nSize>=pOwner->m_nUnZipDataBufferSize)
//		{
//			const size_t nSizeTemp = pOwner->m_nUnZipDataBufferSize + (nSize*2) + 8*1024;
//			char * bufferTemp = (char*)realloc(pOwner->m_pUnZipDataTBuffer,nSizeTemp);
//			if (bufferTemp==NULL)
//			{
//				bufferTemp = (char*)realloc(pOwner->m_pUnZipDataTBuffer,nSizeTemp);
//				if (bufferTemp==NULL)
//					return false;
//			}
//			pOwner->m_pUnZipDataTBuffer = bufferTemp;
//			pOwner->m_nUnZipDataBufferSize = nSizeTemp;
//		}
//		memcpy(pOwner->m_pUnZipDataTBuffer+pOwner->m_nUnZipDataBufferIndex,pData,nSize);
//		pOwner->m_nUnZipDataBufferIndex += nSize;
//	}
//	return true;
//}

unsigned char* ParseCgcSotp2::rebuildBuffer(size_t bBufferSize)
{
	if (m_pSslDecryptData==NULL)
	{
		m_nSslDecrptyBufferSize = bBufferSize;
		m_pSslDecryptData = new unsigned char[m_nSslDecrptyBufferSize];
	}else if (m_nSslDecrptyBufferSize<bBufferSize)
	{
		m_nSslDecrptyBufferSize = bBufferSize;
		delete[] m_pSslDecryptData;
		m_pSslDecryptData = new unsigned char[m_nSslDecrptyBufferSize];
	}
	return m_pSslDecryptData;
}

	// 把SOTP协议，改到2.0版本
const char * ParseCgcSotp2::parseOneLine(const char * pLineBuffer,size_t nBufferSize)
{
	if (pLineBuffer == NULL || nBufferSize<=1) return NULL;

	const char * pNextLineFind = strstr(pLineBuffer, "\n");
	//if (pNextLineFind == NULL) return NULL;
	const short const_r_offset = (pNextLineFind!=0&&((pNextLineFind-1)[0])=='\r')?1:0;

	if (m_nSotpVersion==SOTP_PROTO_VERSION_21)
	{
		const int nSotpItemType = SotpChar2Int(pLineBuffer[0]);
		//printf("**** parseOneLine nBufferSize=%d, nSotpItemType=%d\n",(int)nBufferSize,nSotpItemType);
		switch (nSotpItemType)
		{
		case SOTP_PROTO_ITEM_TYPE_RTP_COMMAND:
			{
				if (!isRTPProto()) return NULL;
				memcpy(&m_pSotpRtpCommand,pLineBuffer+1,SOTP_RTP_COMMAND_SIZE);
				pNextLineFind = pLineBuffer+(1+SOTP_RTP_COMMAND_SIZE);
				// **** do ntoll,...
				if (m_pSotpRtpCommand.m_nCommand==SOTP_RTP_COMMAND_DATA_REQUEST)
				{
					m_pSotpRtpCommand.u.m_nDataRequest.m_nCount = ntohs(m_pSotpRtpCommand.u.m_nDataRequest.m_nCount);
					m_pSotpRtpCommand.u.m_nDataRequest.m_nSeq = ntohs(m_pSotpRtpCommand.u.m_nDataRequest.m_nSeq);
				}else
				{
					m_pSotpRtpCommand.u.m_nDestId = ntohll(m_pSotpRtpCommand.u.m_nDestId);
				}
				m_pSotpRtpCommand.m_nRoomId = ntohll(m_pSotpRtpCommand.m_nRoomId);
				m_pSotpRtpCommand.m_nSrcId = ntohll(m_pSotpRtpCommand.m_nSrcId);
				m_bRtpCommand = true;
			}break;
		case SOTP_PROTO_ITEM_TYPE_RTP_DATA:
			{
				if (!isRTPProto()) return NULL;
				memcpy(&m_pSotpRtpDataHead,pLineBuffer+1,SOTP_RTP_DATA_HEAD_SIZE);
				//m_pSotpRtpDataHead.m_nRoomId = ntohll(m_pSotpRtpDataHead.m_nRoomId);
				//m_pSotpRtpDataHead.m_nSeq = ntohs(m_pSotpRtpDataHead.m_nSeq);
				//m_pSotpRtpDataHead.m_nSrcId = ntohll(m_pSotpRtpDataHead.m_nSrcId);
				//m_pSotpRtpDataHead.m_nTimestamp = ntohl(m_pSotpRtpDataHead.m_nTimestamp);
				//m_pSotpRtpDataHead.m_nTotleLength = ntohl(m_pSotpRtpDataHead.m_nTotleLength);
				//m_pSotpRtpDataHead.m_nUnitLength = ntohs(m_pSotpRtpDataHead.m_nUnitLength);
				const uint16 nIndex = ntohs(m_pSotpRtpDataHead.m_nIndex);
				const uint16 nUnitLength = ntohs(m_pSotpRtpDataHead.m_nUnitLength);
				const uint32 nTotleLength = ntohl(m_pSotpRtpDataHead.m_nTotleLength);

				const uint32 nDataOffset = ((uint32)nIndex)*nUnitLength;
				const uint32 nDataLength = (nTotleLength-nDataOffset)>=nUnitLength?nUnitLength:(nTotleLength-nDataOffset);
				if (nDataOffset>=nTotleLength ||
					(nDataLength+SOTP_RTP_DATA_HEAD_SIZE+2)>(nBufferSize))
					return NULL;
				try
				{
					m_attach->setTotal(nTotleLength);
					m_attach->setIndex(nDataOffset);
					m_attach->setAttach((const unsigned char *)pLineBuffer+(1+SOTP_RTP_DATA_HEAD_SIZE),nDataLength);
				}catch(const std::exception&)
				{
				}catch(...)
				{
				}
				pNextLineFind = pLineBuffer+(1+SOTP_RTP_DATA_HEAD_SIZE+nDataLength);
				m_bRtpData = true;
			}break;
		case SOTP_PROTO_ITEM_TYPE_SID:
			{
				if (pNextLineFind == NULL) return NULL;
				m_sSid = std::string(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
			}break;
		case SOTP_PROTO_ITEM_TYPE_ACCEPT_ENCODING:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sDest(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				//printf("**** parseOneLine SOTP_PROTO_ITEM_TYPE_ACCEPT_ENCODING=%s\n",sDest.c_str());
				try
				{
					m_nAcceptEncoding = atoi(sDest.c_str());
				}catch(...)
				{
					m_nAcceptEncoding = SOTP_DATA_ENCODING_UNKNOWN;
				}
			}break;
		case SOTP_PROTO_ITEM_TYPE_DATA_ENCODING:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sDest(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				//printf("**** parseOneLine SOTP_PROTO_ITEM_TYPE_DATA_ENCODING=%s\n",sDest.c_str());
				if (sDest.size()<=1)  return NULL;
				try
				{
					// G1L\n
					char buf[2];
					memcpy(buf,sDest.c_str(),1);
					buf[1] = '\0';
					m_nDataEncoding = atoi(buf);
					m_nDataSize= atoi(sDest.c_str()+1);
				}catch(...)
				{
					m_nDataEncoding = SOTP_DATA_ENCODING_UNKNOWN;
				}
			}break;
		case SOTP_PROTO_ITEM_TYPE_SSLDATA:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sCurLineBuffer(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				const int nSslSize = atoi(sCurLineBuffer.c_str());
				const char * pSslEnd = pNextLineFind;
				if (nSslSize > 0)
				{
					try
					{
						pSslEnd = pNextLineFind + (nSslSize + 1);
						const unsigned char* pSslData = (const unsigned char*)(pNextLineFind+1);
						if (isOpenType())
						{
							if (m_pCallback==NULL)
								return pSslEnd;
							CRSA pRsa;
							pRsa.SetPrivateKey(m_pCallback->onGetSslPrivateKey());
							pRsa.SetPrivatePwd(m_pCallback->onGetSslPrivatePwd());
							if (!pRsa.rsa_open_private_mem())
								return pSslEnd;
							if (!pRsa.rsa_open_private_mem())
								return pSslEnd;
							unsigned char * pServerSslPwd = NULL;
							const int ret = pRsa.rsa_private_decrypt(pSslData,nSslSize,&pServerSslPwd);
							if (ret < 0)
								return pSslEnd;
							m_sSslPassword = tstring((const char*)pServerSslPwd,ret);
							delete[] pServerSslPwd;
						}else
						{
							// 
							const tstring sSslPassword = m_pCallback->onGetSslPassword(getSid());
							if (sSslPassword.empty())
								return pSslEnd;

#ifdef USES_DATA_ENCODING
							if (m_nDataEncoding!=SOTP_DATA_ENCODING_UNKNOWN)
							{
								//printf("**** parseOneLine m_nDataEncoding=%d,m_nDataSize=%d,nSslSize=%d\n",(int)m_nDataEncoding,(int)m_nDataSize,(int)nSslSize);
								if (rebuildBuffer(m_nDataSize+20)==NULL)
								{
									return NULL;
								}
								unsigned char * pSotpData = new unsigned char[nSslSize+20];
								pSotpData[nSslSize] = '\0';		// **
								const int ret = aes_cbc_decrypt_full((const unsigned char*)sSslPassword.c_str(),(int)sSslPassword.size(),pSslData,nSslSize,pSotpData);
								//printf("**** aes_cbc_decrypt_full ret=%d\n",(int)ret);
								if (ret != 0)
								{
									delete[] pSotpData;
									return NULL;
								}
								m_pSslDecryptData[0] = '\n';	// **
								if (m_nDataEncoding==SOTP_DATA_ENCODING_DEFLATE)
								{
									uLong nUnZipDestSize = m_nDataSize+20-1;
									const int nUnZipRet = UnZipData(pSotpData,nSslSize,m_pSslDecryptData+1,&nUnZipDestSize,0);
									if (nUnZipRet!=Z_OK)
									{
										delete[] pSotpData;
										return NULL;
									}
								}else if (m_nDataEncoding==SOTP_DATA_ENCODING_GZIP)
								{
									uLong nUnZipDestSize = m_nDataSize+20-1;
									const int nUnZipRet = UnGZipData(pSotpData,nSslSize,m_pSslDecryptData+1,&nUnZipDestSize);
									//printf("**** UnGZipData nUnZipRet=%d,nUnZipDestSize=%d\n",(int)nUnZipRet,(int)nUnZipDestSize);
									if (nUnZipRet!=Z_OK)
									{
										delete[] pSotpData;
										return NULL;
									}
								}
								delete[] pSotpData;
								m_pSslDecryptData[m_nDataSize+1] = '\0';		// **
								return (const char*)m_pSslDecryptData;
							}
#endif
							if (rebuildBuffer(nSslSize+20)==NULL)
							{
								return pSslEnd;
							}
							m_pSslDecryptData[0] = '\n';	// **
							m_pSslDecryptData[nSslSize+1] = '\0';		// **
							const int ret = aes_cbc_decrypt_full((const unsigned char*)sSslPassword.c_str(),(int)sSslPassword.size(),pSslData,nSslSize,m_pSslDecryptData+1);
							if (ret != 0)
							{
								//delete[] m_pSslDecryptData;
								//m_pSslDecryptData = NULL;
								return pSslEnd;
							}
							return (const char*)m_pSslDecryptData;
						}
					}catch(const std::exception&)
					{
					}catch(...)
					{
					}
				}
				return pSslEnd;
			}break;
		case SOTP_PROTO_ITEM_TYPE_CID:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sDest(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				try
				{
					m_nCallId = (unsigned long)cgc_atoi64(sDest.c_str());
				}catch(...)
				{
					m_nCallId = 0;
				}
			}break;
		case SOTP_PROTO_ITEM_TYPE_SEQ:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sDest(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				try
				{
					m_bHasSeq = true;
					m_seq = atoi(sDest.c_str());
				}catch(...)
				{
					m_seq = 0;
				}
			}break;
		case SOTP_PROTO_ITEM_TYPE_NACK:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sDest(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				try
				{
					m_bNeedAck = (sDest=="1")?true:false;
				}catch(...)
				{
					m_bNeedAck = false;
				}
			}break;
		case SOTP_PROTO_ITEM_TYPE_SIGN:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sDest(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				try
				{
					m_nSign = (unsigned long)cgc_atoi64(sDest.c_str());
				}catch(...)
				{
					m_nSign = 0;
				}
			}break;
		case SOTP_PROTO_ITEM_TYPE_API:
			{
				if (pNextLineFind == NULL) return NULL;
				//if (m_nProtoType==SOTP_PROTO_TYPE_SYNC)
				//	m_sApi = "sync_" + std::string(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				//else
					m_sApi = std::string(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
			}break;
		case SOTP_PROTO_ITEM_TYPE_PV:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sCurLineBuffer(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				// Get Parameter Name.
				std::string::size_type nFindPT = sCurLineBuffer.find(";pt=");
				if (nFindPT == std::string::npos)
				{
					return pNextLineFind;
				}
				std::string parameterName = sCurLineBuffer.substr(0, nFindPT);

				// Get Parameter Type.
				std::string::size_type nFindPL = sCurLineBuffer.find(";pl=", nFindPT+4);
				if (nFindPL == std::string::npos)
				{
					return pNextLineFind;
				}
				std::string parameterType = sCurLineBuffer.substr(nFindPT+4, nFindPL-nFindPT-4);

				// Get Parameter Value Length.
				tstring sTemp = sCurLineBuffer.substr(nFindPL+4);
				int length = 0;
				try
				{
					length = atoi(sTemp.c_str());
					//length = (long)cgc_atoi64(sTemp.c_str());
				}catch(...)
				{
					length = 0;
				}
				// Get Parameter Value.
				std::string sParamValue = "";
				const char * pPvEnd = pNextLineFind;
				try
				{
					pPvEnd = pNextLineFind + length + 1;
					sParamValue = std::string(pNextLineFind+1, length);
				}catch(...)
				{
					sParamValue = "";
				}

				// new Parameter Class.
				cgcParameter::pointer parameter = CGC_PARAMETER(parameterName, sParamValue);
				parameter->totype(cgcParameter::cgcGetValueType(parameterType));
#ifdef WIN32
				if (parameter->getType() == cgcValueInfo::TYPE_STRING && m_sEncoding=="UTF8")
				{
					parameter->setStr(str_convert(parameter->getStr().c_str(),CP_UTF8,CP_ACP));
				}
#endif
				addParameter(parameter);
				return pPvEnd;
			}break;
		case SOTP_PROTO_ITEM_TYPE_AT:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sCurLineBuffer(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);

				// Get Attach Name.
				std::string::size_type nFindAT = sCurLineBuffer.find(";at=");
				if (nFindAT == std::string::npos)
				{
					return pNextLineFind;
				}
				std::string sTemp = sCurLineBuffer.substr(0, nFindAT);
				m_attach->setName(sTemp);

				// Get Attach Total.
				std::string::size_type nFindAI = sCurLineBuffer.find(";ai=", nFindAT+4);
				if (nFindAI == std::string::npos)
				{
					m_attach->clear();
					return pNextLineFind;
				}
				sTemp = sCurLineBuffer.substr(nFindAT+4, nFindAI-nFindAT-4);
				mycp::bigint total = 0;
				try
				{
					total = cgc_atoi64(sTemp.c_str());
				}catch(...)
				{
					total = 0;
				}
				m_attach->setTotal(total);

				// Get Attach Index.
				std::string::size_type nFindAL = sCurLineBuffer.find(";al=", nFindAI+4);
				if (nFindAL == std::string::npos)
				{
					m_attach->clear();
					return pNextLineFind;
				}
				sTemp = sCurLineBuffer.substr(nFindAI+4, nFindAL-nFindAI-4);
				mycp::bigint index = 0;
				try
				{
					index = cgc_atoi64(sTemp.c_str());
				}catch(...)
				{
					index = 0;
				}
				m_attach->setIndex(index);

				// Get Attach Length.
				sTemp = sCurLineBuffer.substr(nFindAL+4);
				unsigned int attachSize = 0;
				try
				{
					attachSize = (unsigned int)cgc_atoi64(sTemp.c_str());
				}catch(...)
				{
					attachSize = 0;
				}
				const char * pAttachEnd = pNextLineFind;
				if (attachSize > 0)
				{
					// Get Attach Data.
					//std::string sAttachData = "";
					try
					{
						pAttachEnd = pNextLineFind + attachSize + 1;
						//sAttachData = std::string(pNextLineFind+1, attachSize);
						m_attach->setAttach((const unsigned char *)pNextLineFind+1, attachSize);
					}catch(const std::exception&)
					{
						//sAttachData = "";
					}catch(...)
					{
						//sAttachData = "";
					}
					//m_attach->setAttach((const unsigned char *)sAttachData.c_str(), attachSize);
				}
				return pAttachEnd;
			}break;
		case SOTP_PROTO_ITEM_TYPE_APP:
			{
				if (pNextLineFind == NULL) return NULL;
				m_sApp = std::string(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
			}break;
		case SOTP_PROTO_ITEM_TYPE_SSL:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sCurLineBuffer(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);
				const int nSslSize = atoi(sCurLineBuffer.c_str());
				const char * pSslEnd = pNextLineFind;
				if (nSslSize > 0)
				{
					try
					{
						pSslEnd = pNextLineFind + nSslSize + 1;
						m_sSslPublicKey = tstring((const char *)pNextLineFind+1, nSslSize);
					}catch(const std::exception&)
					{
					}catch(...)
					{
					}
				}
				return pSslEnd;
			}break;
		case SOTP_PROTO_ITEM_TYPE_UA:
			{
				if (pNextLineFind == NULL) return NULL;
				const std::string sCurLineBuffer(pLineBuffer+1, pNextLineFind-pLineBuffer-1-const_r_offset);

				// Get Account.
				std::string::size_type nFindPwd = sCurLineBuffer.find(";pwd=");
				if (nFindPwd == std::string::npos)
				{
					return pNextLineFind;
				}
				std::string sTemp = sCurLineBuffer.substr(0, nFindPwd);
				//#ifdef _UNICODE
				//		m_sAccount = cgcString::Char2W(sTemp);
				//#else
				m_sAccount = sTemp;
				//#endif // _UNICODE

				// Get Pwd.
				std::string::size_type nFindEnc = sCurLineBuffer.find(";enc=", nFindPwd+5);
				if (nFindEnc == std::string::npos)
				{
					return pNextLineFind;
				}
				//#ifdef _UNICODE
				//		sTemp = sCurLineBuffer.substr(nFindPwd+5, nFindEnc-nFindPwd-5);
				//		m_sPasswd = cgcString::Char2W(sTemp);
				//#else
				m_sPasswd = sCurLineBuffer.substr(nFindPwd+5, nFindEnc-nFindPwd-5);
				//#endif // _UNICODE

				// Get Enc
				sTemp = sCurLineBuffer.substr(nFindEnc+5);
				//#ifdef _UNICODE
				//		m_et = ModuleItem::getEncryption(cgcString::Char2W(sTemp));
				//#else
				m_et = ModuleItem::getEncryption(sTemp);
				//#endif // _UNICODE
			}break;
		default:
			break;
		}
	}else
	{
		if (pNextLineFind == NULL) return NULL;
		int leftIndex = 0;
		if (sotpCompare(pLineBuffer, "Sid: ", leftIndex))
		{
			leftIndex += 5;
			//#ifdef _UNICODE
			//		std::string sTemp(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex);
			//		m_sSid = cgcString::Char2W(sTemp);
			//#else
			m_sSid = std::string(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			//#endif // _UNICODE
			return pNextLineFind;
		}else if (sotpCompare(pLineBuffer, "Sd: ", leftIndex))
		{
			leftIndex += 4;
			const std::string sCurLineBuffer(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			const int nSslSize = atoi(sCurLineBuffer.c_str());
			const char * pSslEnd = pNextLineFind;
			if (nSslSize > 0)
			{
				try
				{
					pSslEnd = pNextLineFind + nSslSize + 1;
					const unsigned char* pSslData = (const unsigned char*)(pNextLineFind+1);
					if (isOpenType())
					{
						if (m_pCallback==NULL)
							return pSslEnd;
						CRSA pRsa;
						pRsa.SetPrivateKey(m_pCallback->onGetSslPrivateKey());
						pRsa.SetPrivatePwd(m_pCallback->onGetSslPrivatePwd());
						if (!pRsa.rsa_open_private_mem())
							return pSslEnd;
						if (!pRsa.rsa_open_private_mem())
							return pSslEnd;
						unsigned char * pServerSslPwd = NULL;
						const int ret = pRsa.rsa_private_decrypt(pSslData,nSslSize,&pServerSslPwd);
						if (ret < 0)
							return pSslEnd;
						m_sSslPassword = tstring((const char*)pServerSslPwd,ret);
						delete[] pServerSslPwd;
					}else
					{
						// 
						const tstring sSslPassword = m_pCallback->onGetSslPassword(getSid());
						if (sSslPassword.empty())
							return pSslEnd;

						// new version
						if (rebuildBuffer(nSslSize+20)==NULL)
						{
							return pSslEnd;
						}
						m_pSslDecryptData[0] = '\n';	// **
						m_pSslDecryptData[nSslSize+1] = '\0';		// **
						const int ret = aes_cbc_decrypt_full((const unsigned char*)sSslPassword.c_str(),(int)sSslPassword.size(),pSslData,nSslSize,m_pSslDecryptData+1);
						if (ret != 0)
						{
							//delete[] m_pSslDecryptData;
							//m_pSslDecryptData = NULL;
							return pSslEnd;
						}
						return (const char*)m_pSslDecryptData;
						// old version
						//unsigned char * pSotpData = new unsigned char[nSslSize+20];	// 2
						//pSotpData[0] = '\n';	// **
						//pSotpData[nSslSize+1] = '\0';		// **
						//const int ret = aes_cbc_decrypt_full((const unsigned char*)sSslPassword.c_str(),(int)sSslPassword.size(),pSslData,nSslSize,pSotpData+1);
						//if (ret != 0)
						//{
						//	delete[] pSotpData;
						//	return pSslEnd;
						//}
						//if (m_pSslDecryptData!=NULL)
						//{
						//	delete[] m_pSslDecryptData;
						//}
						//m_pSslDecryptData = pSotpData;
						//return (const char*)m_pSslDecryptData;
					}
				}catch(const std::exception&)
				{
				}catch(...)
				{
				}
			}
			return pSslEnd;
		}else if (sotpCompare(pLineBuffer, "Cid: ", leftIndex))
		{
			leftIndex += 5;
			std::string sDest(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			try
			{
				m_nCallId = (unsigned long)cgc_atoi64(sDest.c_str());
			}catch(...)
			{
				m_nCallId = 0;
			}
			return pNextLineFind;
		}else if (sotpCompare(pLineBuffer, "Seq: ", leftIndex))
		{
			leftIndex += 5;
			std::string sDest(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			try
			{
				m_bHasSeq = true;
				m_seq = atoi(sDest.c_str());
			}catch(...)
			{
				m_seq = 0;
			}
			return pNextLineFind;
		}else if (sotpCompare(pLineBuffer, "NAck: ", leftIndex))
		{
			leftIndex += 6;
			std::string sDest(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			try
			{
				m_bNeedAck = bool(atoi(sDest.c_str()) == 1);
			}catch(...)
			{
				m_bNeedAck = false;
			}
			return pNextLineFind;
		}else if (sotpCompare(pLineBuffer, "Sign: ", leftIndex))
		{
			leftIndex += 6;
			std::string sDest(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			try
			{
				m_nSign = (unsigned long)cgc_atoi64(sDest.c_str());
			}catch(...)
			{
				m_nSign = 0;
			}
			return pNextLineFind;
		}else if (sotpCompare(pLineBuffer, "Api: ", leftIndex))
		{
			leftIndex += 5;
			//#ifdef _UNICODE
			//		std::string sTemp(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex);
			//		m_sApi = cgcString::Char2W(sTemp);
			//#else
			m_sApi = std::string(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			//#endif // _UNICODE
			return pNextLineFind;
		}else if (sotpCompare(pLineBuffer, "Pv: ", leftIndex))
		{
			leftIndex += 4;
			std::string sCurLineBuffer(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);


			// Get Parameter Name.
			std::string::size_type nFindPT = sCurLineBuffer.find(";pt=");
			if (nFindPT == std::string::npos)
			{
				return pNextLineFind;
			}
			std::string parameterName = sCurLineBuffer.substr(0, nFindPT);

			// Get Parameter Type.
			std::string::size_type nFindPL = sCurLineBuffer.find(";pl=", nFindPT+4);
			if (nFindPL == std::string::npos)
			{
				return pNextLineFind;
			}
			std::string parameterType = sCurLineBuffer.substr(nFindPT+4, nFindPL-nFindPT-4);

			// Get Parameter Value Length.
			tstring sTemp = sCurLineBuffer.substr(nFindPL+4);
			int length = 0;
			try
			{
				length = atoi(sTemp.c_str());
				//length = cgc_atoi64(sTemp.c_str());
			}catch(...)
			{
				length = 0;
			}
			// Get Parameter Value.
			std::string sParamValue = "";
			const char * pPvEnd = pNextLineFind;
			try
			{
				pPvEnd = pNextLineFind + length + 1;
				sParamValue = std::string(pNextLineFind+1, length);
			}catch(...)
			{
				sParamValue = "";
			}

			// new Parameter Class.
			cgcParameter::pointer parameter = CGC_PARAMETER(parameterName, sParamValue);
			parameter->totype(cgcParameter::cgcGetValueType(parameterType));
#ifdef WIN32
			if (parameter->getType() == cgcValueInfo::TYPE_STRING && m_sEncoding=="UTF8")
			{
				parameter->setStr(str_convert(parameter->getStr().c_str(),CP_UTF8,CP_ACP));
			}
#endif
			addParameter(parameter);
			return pPvEnd;
		}else if (sotpCompare(pLineBuffer, "At: ", leftIndex))
		{
			leftIndex += 4;
			std::string sCurLineBuffer(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);

			// Get Attach Name.
			std::string::size_type nFindAT = sCurLineBuffer.find(";at=");
			if (nFindAT == std::string::npos)
			{
				return pNextLineFind;
			}
			std::string sTemp = sCurLineBuffer.substr(0, nFindAT);
			m_attach->setName(sTemp);

			// Get Attach Total.
			std::string::size_type nFindAI = sCurLineBuffer.find(";ai=", nFindAT+4);
			if (nFindAI == std::string::npos)
			{
				m_attach->clear();
				return pNextLineFind;
			}
			sTemp = sCurLineBuffer.substr(nFindAT+4, nFindAI-nFindAT-4);
			mycp::bigint total = 0;
			try
			{
				total = cgc_atoi64(sTemp.c_str());
			}catch(...)
			{
				total = 0;
			}
			m_attach->setTotal(total);

			// Get Attach Index.
			std::string::size_type nFindAL = sCurLineBuffer.find(";al=", nFindAI+4);
			if (nFindAL == std::string::npos)
			{
				m_attach->clear();
				return pNextLineFind;
			}
			sTemp = sCurLineBuffer.substr(nFindAI+4, nFindAL-nFindAI-4);
			mycp::bigint index = 0;
			try
			{
				index = cgc_atoi64(sTemp.c_str());
			}catch(...)
			{
				index = 0;
			}
			m_attach->setIndex(index);

			// Get Attach Length.
			sTemp = sCurLineBuffer.substr(nFindAL+4);
			unsigned int attachSize = 0;
			try
			{
				attachSize = (unsigned int)cgc_atoi64(sTemp.c_str());
			}catch(...)
			{
				attachSize = 0;
			}
			const char * pAttachEnd = pNextLineFind;
			if (attachSize > 0)
			{
				// Get Attach Data.
				//std::string sAttachData = "";
				try
				{
					pAttachEnd = pNextLineFind + attachSize + 1;
					//sAttachData = std::string(pNextLineFind+1, attachSize);
					m_attach->setAttach((const unsigned char *)pNextLineFind+1, attachSize);
				}catch(const std::exception&)
				{
					//sAttachData = "";
				}catch(...)
				{
					//sAttachData = "";
				}
				//m_attach->setAttach((const unsigned char *)sAttachData.c_str(), attachSize);
			}

			return pAttachEnd;
		}else if (sotpCompare(pLineBuffer, "App: ", leftIndex))
		{
			leftIndex += 5;
			//#ifdef _UNICODE
			//		std::string sTemp(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex);
			//		m_sApp = cgcString::Char2W(sTemp);
			//#else
			m_sApp = std::string(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			//#endif // _UNICODE
			return pNextLineFind;
		}else if (sotpCompare(pLineBuffer, "Ssl: ", leftIndex))
		{
			leftIndex += 5;
			const std::string sCurLineBuffer(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);
			const int nSslSize = atoi(sCurLineBuffer.c_str());
			const char * pSslEnd = pNextLineFind;
			if (nSslSize > 0)
			{
				try
				{
					pSslEnd = pNextLineFind + nSslSize + 1;
					m_sSslPublicKey = tstring((const char *)pNextLineFind+1, nSslSize);
				}catch(const std::exception&)
				{
				}catch(...)
				{
				}
			}
			return pSslEnd;
		}else if (sotpCompare(pLineBuffer, "Ua: ", leftIndex))
		{
			leftIndex += 4;
			std::string sCurLineBuffer(pLineBuffer+leftIndex, pNextLineFind-pLineBuffer-leftIndex-const_r_offset);

			// Get Account.
			std::string::size_type nFindPwd = sCurLineBuffer.find(";pwd=");
			if (nFindPwd == std::string::npos)
			{
				return pNextLineFind;
			}
			std::string sTemp = sCurLineBuffer.substr(0, nFindPwd);
			//#ifdef _UNICODE
			//		m_sAccount = cgcString::Char2W(sTemp);
			//#else
			m_sAccount = sTemp;
			//#endif // _UNICODE

			// Get Pwd.
			std::string::size_type nFindEnc = sCurLineBuffer.find(";enc=", nFindPwd+5);
			if (nFindEnc == std::string::npos)
			{
				return pNextLineFind;
			}
			//#ifdef _UNICODE
			//		sTemp = sCurLineBuffer.substr(nFindPwd+5, nFindEnc-nFindPwd-5);
			//		m_sPasswd = cgcString::Char2W(sTemp);
			//#else
			m_sPasswd = sCurLineBuffer.substr(nFindPwd+5, nFindEnc-nFindPwd-5);
			//#endif // _UNICODE

			// Get Enc
			sTemp = sCurLineBuffer.substr(nFindEnc+5);
			//#ifdef _UNICODE
			//		m_et = ModuleItem::getEncryption(cgcString::Char2W(sTemp));
			//#else
			m_et = ModuleItem::getEncryption(sTemp);
			//#endif // _UNICODE

			return pNextLineFind;
		}
	}
	return pNextLineFind;
	//return parsePv(pLineBuffer);
}
bool ParseCgcSotp2::sotpCompare(const char * pBuffer, const char * pCompare, int & leftIndex)
{
	int i1 = 0, i2 = 0;
	leftIndex = 0;
	// 判断前面空格或者‘TAB’键；
	//while (' ' == pBuffer[leftIndex] || '\t' == pBuffer[leftIndex] || '\r' == pBuffer[leftIndex])
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
bool ParseCgcSotp2::getProtoItem(int nItemType, unsigned long * pOutItemValue) const
{
	switch(nItemType)
	{
	case SOTP_PROTO_ITEM_TYPE_ACCEPT_ENCODING:
		*pOutItemValue = m_nAcceptEncoding;
		return true;
	case SOTP_PROTO_ITEM_TYPE_DATA_ENCODING:
		*pOutItemValue = m_nDataEncoding;
		return true;
	default:
		break;
	}
	return false;
}

}	// namespace mycp
