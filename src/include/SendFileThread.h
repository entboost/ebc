#pragma once
#ifndef __SendFileThread_h__
#define __SendFileThread_h__

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <stl/locklist.h>
#include "eb_objects.h"

//const int const_send_data_size = 1600;//1024*2;
const int const_default_sleep_p2p	 = 3;//4;	// 4外网1100：250KB－380KB；内网10KB：2.4MB-3.6MB
const int const_default_sleep_value1 = 15;
const int const_default_sleep_value2 = 100;

namespace entboost {


//class CSourceIndexInfo
//{
//public:
//	typedef boost::shared_ptr<CSourceIndexInfo> pointer;
//	static CSourceIndexInfo::pointer create(uLong nSourceIndex, uLong nSendDataSourceIndexOffset)
//	{
//		return CSourceIndexInfo::pointer(new CSourceIndexInfo(nSourceIndex, nSendDataSourceIndexOffset));
//	}
//	//mycp::bigint m_nFileIndex;
//	uLong m_nSourceIndex;								// * file index
//	uLong m_nSendDataSourceIndexOffset;
//	CSourceIndexInfo(uLong nSourceIndex, uLong nSendDataSourceIndexOffset)
//		: m_nSourceIndex(nSourceIndex), m_nSendDataSourceIndexOffset(nSendDataSourceIndexOffset)
//	{}
//	CSourceIndexInfo(void)
//		: m_nSourceIndex(0), m_nSendDataSourceIndexOffset(0)
//		//: m_nFileIndex(0), m_nSourceIndex(0), m_nSendDataSourceIndexOffset(0);
//	{}
//};

class CPOPCChatManager;
class CSendFileThread
{
public:
	typedef boost::shared_ptr<CSendFileThread> pointer;
	static CSendFileThread::pointer create(CPOPCChatManager * pChatManager, mycp::bigint ntotallen, mycp::bigint nMsgId, FILE * pfile, int nSendSize);
	static CSendFileThread::pointer create(CPOPCChatManager * pChatManager, mycp::bigint ntotallen,int nSendSize,mycp::bigint nMsgId, const CPOPSotpRequestInfo::pointer & pRequestInfo);
	static CSendFileThread::pointer create(CPOPCChatManager * pChatManager, const CChatMsgInfo::pointer& pChatMsgInfo);
	CSendFileThread(CPOPCChatManager * pChatManager, mycp::bigint ntotallen, mycp::bigint nMsgId, FILE * pfile, int nSendSize);
	CSendFileThread(CPOPCChatManager * pChatManager, mycp::bigint ntotallen,int nSendSize,mycp::bigint nMsgId, const CPOPSotpRequestInfo::pointer & pRequestInfo);
	CSendFileThread(CPOPCChatManager * pChatManager, const CChatMsgInfo::pointer& pChatMsgInfo);
	virtual ~CSendFileThread(void);

	//static void thread_send_file(void * lparam);

	void SetThread(void);
	void KillThread(void);
	void SendData(void);
	void SetAcceptEncoding(int v) {m_nAcceptEncoding = v;}
	void SetFileName(const tstring& v) {m_sFileName = v;}
	void SetDisableZip(bool v) {m_bDisableZip = v;}
	void SetP2P(bool v) {m_bP2P = v;}
	void SetLocalIpAddress(bool v) {m_bLocalIpAddress = v;}

	CLockMap<mycp::bigint,bool> m_pResendIndex;	// 需要补偿数据
	//CLockList<mycp::bigint> m_pResendIndex;	// 需要补偿数据

private:
	void SendFile(void);
	void WriteFile(void);
	void SendRich(void);

	CZIB m_pZib;
	//CLockMap<mycp::bigint,CSourceIndexInfo::pointer> m_pSendIndexList;	// send_index->*
	static bool ZipFileToSendCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned long nUserData);
	unsigned char * m_pSendBuffer;
	int m_nAttachEncoding;
	size_t m_nUnSendDataSize;
	uLong m_nZipFileSize;
	mycp::bigint m_nSendZipDataSizeTemp;
	//CSourceIndexInfo::pointer m_pUnSendDataSourceIndex;
	//CSourceIndexInfo::pointer m_pLastSourceIndex;
	cgcAttachment::pointer m_pSendZipAttach;
	bool SendZipData(uLong nSourceIndex, const unsigned char* pData, uLong nSize);
	//CSourceIndexInfo::pointer m_pWaitToSendDataSourceIndex;	// nWaitToSendSize=m_nSourceIndex

private:
	bool m_bKilled;
	int m_nMsgType;
	CChatMsgInfo::pointer m_pChatMsgInfo;
	CLockList<unsigned int> m_pSendFileIndex;
	std::vector<boost::shared_ptr<boost::thread> > m_pTreadSendFileList;
	boost::shared_ptr<boost::thread> m_pThreadSendFile;
	CPOPCChatManager * m_pChatManager;
	mycp::bigint m_ntotallen;
	mycp::bigint m_nMsgId;
	boost::mutex m_mutex;
	FILE * m_file;
	FILE * m_fEbConfig;
    EBFileString m_sFileName;
    tstring m_sRemoveFileName;
	int m_nSendSize;
	CPOPSotpRequestInfo::pointer m_pRequestInfo;	// for rich
	int m_nAcceptEncoding;
	bool m_bDisableZip;
	bool m_bP2P;
	bool m_bLocalIpAddress;
	bool m_bMultiThreadSend;
};

} // namespace entboost

#endif // __SendFileThread_h__
