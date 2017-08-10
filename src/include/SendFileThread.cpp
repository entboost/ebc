#ifdef WIN32
#pragma warning(disable:4819 4267)
#endif

#ifdef _QT_MAKE_
#include <QString>
#endif

#ifdef __MACH__
#include "mach_uses.h"
#endif

//#include "StdAfx.h"
#include "pop_sotpclient.h"
#include "SendFileThread.h"
#include "POPCChatManager.h"
#ifdef WIN32
#include <Mmsystem.h>
#else
#include <time.h>   
inline unsigned long timeGetTime()  
{  
	unsigned long uptime = 0;  
	struct timespec on;  
	if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)  
		uptime = on.tv_sec*1000 + on.tv_nsec/1000000;  
	return uptime;  
} 
#define _FILE_OFFSET_BITS 64
#endif

namespace entboost {

CSendFileThread::CSendFileThread(CPOPCChatManager * pChatManager, mycp::bigint ntotallen, mycp::bigint nMsgId, FILE * pfile, int nSendSize)
: m_bKilled(false), m_nMsgType(EB_MSG_FILE)
, m_pChatManager(pChatManager)
, m_ntotallen(ntotallen)
, m_nMsgId(nMsgId)
, m_file(pfile), m_fEbConfig(NULL)
, m_nSendSize(nSendSize)
, m_nAcceptEncoding(0)
, m_bDisableZip(false), m_bP2P(false), m_bLocalIpAddress(false)
, m_bMultiThreadSend(false)
{
	m_pSendBuffer = NULL;
	m_nUnSendDataSize = 0;
	m_nAttachEncoding = 0;
	m_nZipFileSize = 0;
	m_nSendZipDataSizeTemp = 0;
}
CSendFileThread::CSendFileThread(CPOPCChatManager * pChatManager,mycp::bigint ntotallen,int nSendSize,mycp::bigint nMsgId, const CPOPSotpRequestInfo::pointer & pRequestInfo)
: m_bKilled(false), m_nMsgType(EB_MSG_RICH)
, m_pChatManager(pChatManager)
, m_ntotallen(ntotallen)
, m_nMsgId(nMsgId)
, m_file(0),m_fEbConfig(NULL)
, m_nSendSize(nSendSize)
, m_pRequestInfo(pRequestInfo)
, m_nAcceptEncoding(0)
, m_bDisableZip(false), m_bP2P(false), m_bLocalIpAddress(false)
, m_bMultiThreadSend(false)
{
	m_pSendBuffer = NULL;
	m_nAttachEncoding = 0;
	m_nUnSendDataSize = 0;
	m_nZipFileSize = 0;
	m_nSendZipDataSizeTemp = 0;
}
CSendFileThread::CSendFileThread(CPOPCChatManager * pChatManager, const CChatMsgInfo::pointer& pChatMsgInfo)
: m_bKilled(false), m_nMsgType(EB_MSG_FILE), m_pChatMsgInfo(pChatMsgInfo)
, m_pChatManager(pChatManager)
, m_ntotallen(0)
, m_nMsgId(0)
, m_file(0),m_fEbConfig(NULL)
, m_nSendSize(0)
//, m_pRequestInfo(pRequestInfo)
, m_nAcceptEncoding(0)
, m_bDisableZip(false), m_bP2P(false), m_bLocalIpAddress(false)
, m_bMultiThreadSend(false)
{
	m_pSendBuffer = NULL;
	m_nAttachEncoding = 0;
	m_nUnSendDataSize = 0;
	m_nZipFileSize = 0;
	m_nSendZipDataSizeTemp = 0;
}

CSendFileThread::~CSendFileThread(void)
{
	if (m_fEbConfig!=NULL)
	{
		fclose(m_fEbConfig);
		m_fEbConfig = NULL;
	}

    if (!m_sRemoveFileName.empty()) {
		remove(m_sRemoveFileName.c_str());
    }
	if (m_pSendBuffer!=NULL)
	{
		delete m_pSendBuffer;
		m_pSendBuffer = NULL;
	}
	KillThread();
	//m_bKilled = true;
	//m_pResendIndex.clear();
	//m_pThreadSendFile.reset();

	////if (m_pThreadSendFile)
	////{
	////	delete m_pThreadSendFile;
	////}
	////for (size_t i=0;i<m_pTreadSendFileList.size();i++)
	////{
	////	delete m_pTreadSendFileList[i];
	////}
	//m_pTreadSendFileList.clear();
}
	
CSendFileThread::pointer CSendFileThread::create(CPOPCChatManager * pChatManager, mycp::bigint ntotallen, mycp::bigint nMsgId, FILE * pfile, int nSendSize)
{
	return CSendFileThread::pointer(new CSendFileThread(pChatManager, ntotallen, nMsgId, pfile, nSendSize));
}
CSendFileThread::pointer CSendFileThread::create(CPOPCChatManager * pChatManager, mycp::bigint ntotallen,int nSendSize,mycp::bigint nMsgId, const CPOPSotpRequestInfo::pointer & pRequestInfo)
{
	return CSendFileThread::pointer(new CSendFileThread(pChatManager, ntotallen,nSendSize,nMsgId, pRequestInfo));
}
CSendFileThread::pointer CSendFileThread::create(CPOPCChatManager * pChatManager, const CChatMsgInfo::pointer& pChatMsgInfo)
{
	return CSendFileThread::pointer(new CSendFileThread(pChatManager,pChatMsgInfo));
}
//void CSendFileThread::thread_send_file(void * lparam)
//{
//	CSendFileThread * pThread = (CSendFileThread*)lparam;
//	pThread->SendData();
//	//delete pThread;
//}

void CSendFileThread::SetThread(void)
{
	if (m_pThreadSendFile.get() == NULL)
	{
        m_pThreadSendFile = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CSendFileThread::SendData, this)));
//		boost::thread_attributes attrs;
//        attrs.set_stack_size(CGC_THREAD_STACK_MIN);
//		m_pThreadSendFile = boost::shared_ptr<boost::thread>(new boost::thread(attrs,boost::bind(&CSendFileThread::SendData, this)));
	}
}
void CSendFileThread::KillThread(void)
{
	m_bKilled = true;
	m_pResendIndex.clear();
	if (m_pThreadSendFile.get() != NULL)
	{
		m_pThreadSendFile->join();
		m_pThreadSendFile.reset();
	}
	for (size_t i=0;i<m_pTreadSendFileList.size();i++)
	{
		m_pTreadSendFileList[i]->join();
	}
	m_pTreadSendFileList.clear();
}
void CSendFileThread::SendData(void)
{
	if (EB_MSG_FILE == m_nMsgType)
	{
		if (m_pChatMsgInfo.get()!=NULL)
			WriteFile();
		else
			SendFile();
	}else if (EB_MSG_RICH == m_nMsgType)
	{
		SendRich();
	}
}
void CSendFileThread::SendRich(void)
{
	cgcAttachment::pointer pAttach;
	const int const_size = m_nSendSize;
	const unsigned int nsendcount = (unsigned int)((m_ntotallen+const_size-1)/const_size);
	CSpeechInfo pSpeechInfo;
	int nfiles = 0;
	m_pChatManager->GetFileSpeechInfo(m_nMsgId,pSpeechInfo,nfiles);
	for (unsigned int i=0; i<nsendcount; i++)
	{
		if (m_bKilled)
			break;
		mycp::bigint nindex = i*const_size;
		if (m_pRequestInfo->m_pAttachList.find(nindex, pAttach))
		{
			m_pChatManager->SendDSSend(m_nMsgId, pAttach, m_bDisableZip);
			// 公网发太快，也是丢包，或者收不到
			const unsigned int nSleepValue = nfiles*((m_bLocalIpAddress||m_bP2P)?const_default_sleep_p2p:const_default_sleep_value1);
			//const unsigned int nSleepValue = const_default_sleep_value1+(nfiles-1)*const_default_sleep_value1;
#ifdef WIN32
			Sleep(nSleepValue);
#else
			usleep(nSleepValue*1000);
#endif
		}
	}

	unsigned long tSendTime = timeGetTime();
	while (!m_bKilled)
	{
		// 检查需要发送的补偿数据
		mycp::bigint nindex = 0;
		{
			BoostReadLock rdlock(m_pResendIndex.mutex());
			CLockMap<mycp::bigint,bool>::iterator pIter = m_pResendIndex.begin();
			if (pIter==m_pResendIndex.end())
			{
				rdlock.unlock();
				const unsigned long noffset = timeGetTime()-tSendTime;
				if (noffset >= 250)
				{
					tSendTime = timeGetTime();	// 复位，实现200ms秒后再请求DSCheck
					m_pChatManager->SendDSCheck(m_nMsgId,false,m_pRequestInfo);
				}
#ifdef WIN32
				Sleep(10);
#else
				usleep(10000);
#endif
				continue;
			}else
			{
				nindex = pIter->first;
				rdlock.unlock();
				m_pResendIndex.remove(nindex);
			}
		}
		if (m_pRequestInfo->m_pAttachList.find(nindex, pAttach))
		{
			m_pChatManager->SendDSSend(m_nMsgId, pAttach, m_bDisableZip);
#ifdef WIN32
			Sleep(const_default_sleep_value1);
#else
			usleep(const_default_sleep_value1*1000);
#endif
		}
	}

}

void CSendFileThread::WriteFile(void)
{
	if (!m_bMultiThreadSend)
	{
		m_bMultiThreadSend = true;
		int nThreadCount = 0;
		if (m_pChatMsgInfo->GetSize()>50*1024*1024)
			nThreadCount = 3;
		else if (m_pChatMsgInfo->GetSize()>20*1024*1024)
			nThreadCount = 2;
		else if (m_pChatMsgInfo->GetSize()>5*1024*1024)
			nThreadCount = 1;
		if (nThreadCount>0)
		{
//			boost::thread_attributes attrs;
//            attrs.set_stack_size(CGC_THREAD_STACK_MIN);
			for (int i=0;i<nThreadCount;i++)
			{
                boost::shared_ptr<boost::thread> pThreadSendFile(new boost::thread(boost::bind(&CSendFileThread::SendData, this)));
//				boost::shared_ptr<boost::thread> pThreadSendFile(new boost::thread(attrs,boost::bind(&CSendFileThread::SendData, this)));
				m_pTreadSendFileList.push_back(pThreadSendFile);
			}
		}
	}

	while (!m_bKilled)
	{
		cgcAttachment::pointer pAttachment;
		if (!m_pChatMsgInfo->m_pRecvAttachList.get_begin(NULL,&pAttachment,true))
		{
#ifdef WIN32
				Sleep(3);
#else
				usleep(3000);
#endif
				continue;				
		}
//		{
//			BoostWriteLock wtlock(m_pChatMsgInfo->m_pRecvAttachList.mutex());
//			CLockMap<mycp::bigint, cgcAttachment::pointer>::iterator pIter = m_pChatMsgInfo->m_pRecvAttachList.begin();
//			if (pIter==m_pChatMsgInfo->m_pRecvAttachList.end())
//			{
//				wtlock.unlock();
//#ifdef WIN32
//				Sleep(3);
//#else
//				usleep(3000);
//#endif
//				continue;				
//			}
//			pAttachment = pIter->second;
//			m_pChatMsgInfo->m_pRecvAttachList.erase(pIter);
//		}
		//m_pChatMsgInfo->m_pRecvAttachList.insert(pAttachment->getIndex(),pAttachment,false);
		boost::mutex::scoped_lock lockFile(m_pChatMsgInfo->m_mutexFile);
		if (m_pChatMsgInfo->m_pFileHandle==NULL)
			break;	// 已经退出
#ifdef WIN32
		_fseeki64(m_pChatMsgInfo->m_pFileHandle, pAttachment->getIndex(), SEEK_SET);
		//if (_fseeki64(m_pChatMsgInfo->m_pFileHandle, pAttachment->getIndex(), SEEK_SET)!=0)
		//{
		//	clearerr(m_pChatMsgInfo->m_pFileHandle);
		//	//fflush(m_pChatMsgInfo->m_pFileHandle);
		//	if (_fseeki64(m_pChatMsgInfo->m_pFileHandle, pAttachment->getIndex(), SEEK_SET)!=0)
		//	{
		//		const int nIndex = (int)(pAttachment->getIndex()/m_pChatMsgInfo->m_nDataStreamSize);
		//		m_pChatMsgInfo->m_pWaitStreamList.insert(nIndex,false,false);
		//		continue;
		//	}
		//}
#else
		if (fseeko(m_pChatMsgInfo->m_pFileHandle, pAttachment->getIndex(), SEEK_SET)!=0)
		{
			clearerr(m_pChatMsgInfo->m_pFileHandle);
			//fflush(m_pChatMsgInfo->m_pFileHandle);
			if (fseeko(m_pChatMsgInfo->m_pFileHandle, pAttachment->getIndex(), SEEK_SET)!=0)
			{
				const int nIndex = (int)(pAttachment->getIndex()/m_pChatMsgInfo->m_nDataStreamSize);
				m_pChatMsgInfo->m_pWaitStreamList.insert(nIndex,false,false);
				continue;
			}
		}
#endif
		fwrite(pAttachment->getAttachData(), 1, pAttachment->getAttachSize(), m_pChatMsgInfo->m_pFileHandle);
		//if (fwrite(pAttachment->getAttachData(), 1, pAttachment->getAttachSize(), m_pChatMsgInfo->m_pFileHandle)!=pAttachment->getAttachSize())
		//{
		//	clearerr(m_pChatMsgInfo->m_pFileHandle);
		//	//fflush(m_pChatMsgInfo->m_pFileHandle);
		//	if (fwrite(pAttachment->getAttachData(), 1, pAttachment->getAttachSize(), m_pChatMsgInfo->m_pFileHandle)!=pAttachment->getAttachSize())
		//	{
		//		const int nIndex = (int)(pAttachment->getIndex()/m_pChatMsgInfo->m_nDataStreamSize);
		//		m_pChatMsgInfo->m_pWaitStreamList.insert(nIndex,false,false);
		//		continue;
		//	}
		//}
	}
}
#define USES_ZIPFILE_TO_SEND

#ifdef USES_ZIPFILE_TO_SEND
bool CSendFileThread::ZipFileToSendCallBack(uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned long nUserData)
{
	if (nSize>0 && nUserData!=0)
	{
		CSendFileThread * pOwner = (CSendFileThread*)nUserData;
		return pOwner->SendZipData(nSourceIndex, pData, nSize);
	}
	return true;
}

//inline int GetSpeechSleepValue(int nCurSpeed)	// 以 1100 数据包为计算
//{
//	//if (pNewSpeechInfo.m_nCurSpeed>128*1024)
//	//	nSleepValue = nfiles*(const_default_sleep_value1-5);
//	//if (pNewSpeechInfo.m_nCurSpeed>64*1024)
//	//	nSleepValue = nfiles*(const_default_sleep_value1-3);
//	//else
//	//	nSleepValue = nfiles*const_default_sleep_value1;
//
//	if (nCurSpeed>128*1024)
//	{
//		return 1;			// 1>256BK 左右
//	}else if (nCurSpeed>96*1024)
//	{
//		return 2;			// 4>128BK 左右
//	}else if (nCurSpeed>84*1024)
//	{
//		return 3;			// 8=100BK 左右
//	}else if (nCurSpeed>64*1024)
//	{
//		return 5;		// 12=100-128BK 左右
//	}else
//	{
//		return 15;		// 15=64-70KB 左右
//	}
//}

bool CSendFileThread::SendZipData(uLong nSourceIndex, const unsigned char* pData, uLong nSize)
{
	const int const_size = m_nSendSize;
	m_nSendZipDataSizeTemp += (mycp::bigint)nSize;

	//if (m_pLastSourceIndex.get()==NULL)
	//{
	//	m_pLastSourceIndex = CSourceIndexInfo::create(nSourceIndex,0);
	//}else if (/*m_pWaitToSendDataSourceIndex.get()==NULL && */m_pLastSourceIndex->m_nSourceIndex != nSourceIndex)	// ** m_pWaitToSendDataSourceIndex.get()!=NULL，不重置offset，保证重发数据时，offset一直叠加
	//{
	//	m_pLastSourceIndex->m_nSourceIndex = nSourceIndex;
	//	m_pLastSourceIndex->m_nSendDataSourceIndexOffset = 0;
	//}
	//m_pLastSourceIndex->m_nSendDataSourceIndexOffset += nSize;

	//// ** 判断重发数据
	//if (m_pWaitToSendDataSourceIndex.get()!=NULL)
	//{
	//	const uLong nWaitToSendSize = m_pWaitToSendDataSourceIndex->m_nSourceIndex;
	//	const uLong nWaitToSendOffset = m_pWaitToSendDataSourceIndex->m_nSendDataSourceIndexOffset;
	//	if ((m_pLastSourceIndex->m_nSendDataSourceIndexOffset-nSize)>=nWaitToSendOffset)
	//	{
	//		// 
	//		const int nToCopySize = min((int)nSize,(int)(const_size-m_nUnSendDataSize));
	//		if (nToCopySize>0)
	//		{
	//			memcpy(m_pSendBuffer+m_nUnSendDataSize,pData,nToCopySize);
	//			m_nUnSendDataSize += nToCopySize;
	//		}
	//	}
	//	return m_nUnSendDataSize>=(size_t)const_size?false:true;
	//}
	fwrite(pData,1,nSize,m_fEbConfig);

	const int const_zip_pack_size = const_size*8;
	const bool bLastPackFlag = ((m_pSendZipAttach->getTotal()-nSourceIndex)<=const_zip_pack_size)?true:false;	// ** 判断是否最后一个 PACK
	int nUnSendDataToCopySize = 0;
	//mycp::bigint nUnSendIndex = -1;
	if (m_nUnSendDataSize>0)
	{
		if (m_nUnSendDataSize+(size_t)nSize<(size_t)const_size)
		{
			memcpy(m_pSendBuffer+m_nUnSendDataSize,pData,nSize);
			m_nUnSendDataSize += nSize;
			//m_nZipFileSize +=
			return true;
		}

		// **
		nUnSendDataToCopySize = (int)(const_size-m_nUnSendDataSize);
		memcpy(m_pSendBuffer+m_nUnSendDataSize,pData,nUnSendDataToCopySize);
		//const mycp::bigint nSourceSendIndex = (mycp::bigint)(m_pSendIndexList.size()*const_size);
		//nUnSendIndex = ((m_pUnSendDataSourceIndex->m_nSourceIndex+m_pUnSendDataSourceIndex->m_nSendDataSourceIndexOffset+(const_size-1))/const_size)*const_size;
		// 发送之前未发送保留数据
		//m_pSendZipAttach->setIndex(nSourceSendIndex);
		m_pSendZipAttach->setIndex(m_nZipFileSize);
		m_pSendZipAttach->setAttach(m_pSendBuffer, (unsigned int)const_size);
		m_nZipFileSize += const_size;
		if (bLastPackFlag && (nSize-nUnSendDataToCopySize)==0)
			m_pChatManager->SendDSSend(m_nMsgId, m_pSendZipAttach, m_bDisableZip, m_nAttachEncoding, m_nSendZipDataSizeTemp);
		else
			m_pChatManager->SendDSSend(m_nMsgId, m_pSendZipAttach, m_bDisableZip, m_nAttachEncoding);
		//m_pSendIndexList.insert(nSourceSendIndex,CSourceIndexInfo::create(m_pUnSendDataSourceIndex->m_nSourceIndex,m_pUnSendDataSourceIndex->m_nSendDataSourceIndexOffset),false);
		m_nUnSendDataSize = 0;
	}
	// 发送后面压缩数据
	//mycp::bigint nSourceSendIndex = nUnSendIndex==-1?(((nSourceIndex+(const_size-1))/const_size)*const_size):(nUnSendIndex+const_size);
	const unsigned int nsendcount = (unsigned int)((nSize-nUnSendDataToCopySize)/const_size);
	CSpeechInfo pNewSpeechInfo;
	int nfiles = -1;
	for (unsigned int i=0;i<nsendcount; i++)
	{
		//const mycp::bigint nSourceSendIndex = (mycp::bigint)(m_pSendIndexList.size()*const_size);
		m_pSendZipAttach->setIndex(m_nZipFileSize);
		//m_pSendZipAttach->setIndex(nSourceSendIndex);
		m_pSendZipAttach->setAttach(pData+(nUnSendDataToCopySize+i*const_size), (unsigned int)const_size);
		m_nZipFileSize += const_size;
		if (bLastPackFlag && i==nsendcount && (nSize-nUnSendDataToCopySize-(nsendcount*const_size))==0)
			m_pChatManager->SendDSSend(m_nMsgId, m_pSendZipAttach, m_bDisableZip, m_nAttachEncoding, m_nSendZipDataSizeTemp);
		else
			m_pChatManager->SendDSSend(m_nMsgId, m_pSendZipAttach, m_bDisableZip, m_nAttachEncoding);
		//const uLong nSourceIndexOffset = m_pLastSourceIndex->m_nSendDataSourceIndexOffset-nSize+(i*const_size);	// * nSize 不需要 - nUnSendDataToCopySize
		//m_pSendIndexList.insert(nSourceSendIndex,CSourceIndexInfo::create(nSourceIndex,nSourceIndexOffset),false);
		////nSourceSendIndex += const_size;

		if (nfiles==-1)
			m_pChatManager->GetFileSpeechInfo(m_nMsgId,pNewSpeechInfo,nfiles);
		unsigned int nSleepValue = 0;
		if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>128*1024)
		{
			if ((i%7)==1)	// 6
				nSleepValue = nfiles+1;
			else
				continue;
		}else if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>64*1024)
		{
			if (i%4==1)	// 3
				nSleepValue = nfiles+1;
			else
				continue;
		}else if (m_bLocalIpAddress||m_bP2P)
		{
			// 如果限制64KB，等等时间*4，发太快也没用；
			//if (pNewSpeechInfo.m_nCurSpeed>128*1024)
			//{
			//	if (i%6==1)
			//		nSleepValue = 1;
			//	else
			//		continue;
			//}else
			{
				nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?2:const_default_sleep_p2p;
			}
		}else
		{
			nSleepValue = nfiles*5;
			//nSleepValue = nfiles*GetSpeechSleepValue(pNewSpeechInfo.m_nCurSpeed);
		}
		//if (m_bLocalIpAddress||m_bP2P)
		//{
		//	// 如果限制64KB，等等时间*4，发太快也没用；
		//	if (pNewSpeechInfo.m_nCurSpeed>128*1024)
		//	{
		//		if (i%3==1)
		//			nSleepValue = 1;
		//		else
		//			continue;
		//	}else
		//		nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?const_default_sleep_p2p:const_default_sleep_p2p*4;
		//}else
		//{
		//	if (i%2==1)	// *压缩已经花费时间，所以调快一倍速度
		//		continue;
		//	nSleepValue = nfiles*GetSpeechSleepValue(pNewSpeechInfo.m_nCurSpeed);
		//	//nSleepValue = nfiles*5;	// 压缩已经花费时间，调快一些；（内网大约110-150KB左右）
		//	//nSleepValue = nfiles*const_default_sleep_value1;
		//}
#ifdef WIN32
		Sleep(nSleepValue);
#else
		usleep(nSleepValue*1000);
#endif
	}
	m_nUnSendDataSize = (nSize-nUnSendDataToCopySize)-(nsendcount*const_size);
	if (m_nUnSendDataSize>0)
	{
		memcpy(m_pSendBuffer,pData+(nUnSendDataToCopySize+nsendcount*const_size),m_nUnSendDataSize);
		//const uLong nSourceIndexOffset = m_pLastSourceIndex->m_nSendDataSourceIndexOffset-nSize+(nsendcount*const_size);	// * nSize 不需要 - nUnSendDataToCopySize
		//if (m_pUnSendDataSourceIndex.get()==NULL)
		//{
		//	m_pUnSendDataSourceIndex = CSourceIndexInfo::create(nSourceIndex,nSourceIndexOffset);
		//}else
		//{
		//	m_pUnSendDataSourceIndex->m_nSourceIndex = nSourceIndex;
		//	m_pUnSendDataSourceIndex->m_nSendDataSourceIndexOffset = nSourceIndexOffset;
		//}
	}

	return m_bKilled?false:true;
}
#endif // USES_ZIPFILE_TO_SEND


void CSendFileThread::SendFile(void)
{
	// send data
	const int const_size = m_nSendSize;
	const int const_zip_pack_size = const_size*8;
	cgcAttachment::pointer pAttach = cgcAttachment::create();
	pAttach->setTotal(m_ntotallen);
	const unsigned int nsendcount = (unsigned int)((m_ntotallen+const_size-1)/const_size);
	char unsigned * lpBuffer = new unsigned char[const_size+1];
	//bool bAddCharFormat = false;
	//CSpeechInfo pOldSpeechInfo;
	//int nsleepvalue = const_default_sleep_value1;
	int nfiles = 1;
	bool bFirstThread = false;
	bool bMove1MB = false;
	//char lpszBuffer[128];
	//sprintf(lpszBuffer,"m_ntotallen=%lld P2P=%d,m_bMultiThreadSend=%d",m_ntotallen,(int)(m_bP2P?1:0),(int)(m_bMultiThreadSend?1:0));
	//MessageBox(NULL,lpszBuffer,"P2P",MB_OK);
	if (m_bP2P && m_ntotallen>=100*1024 && !m_bMultiThreadSend)
	{
		// 使用多线程；
		//MessageBox(NULL,"使用多线程","使用多线程",MB_OK);
		m_bMultiThreadSend = true;
		bFirstThread = true;
		for (unsigned int i=0; i<nsendcount; i++)
		{
			m_pSendFileIndex.add(i);
		}

		// 一个线程 + if ((nSendIndex%4)==1) ，正常发送流量为2.5-3MB
		// 二个线程 + if ((nSendIndex%4)==1) ，正常发送流量为3-3.3MB（本机达到20MB以上）
		// 三个线程 + if ((nSendIndex%3)==1) ，正常发送流量为7-9MB
//		boost::thread_attributes attrs;
//        attrs.set_stack_size(CGC_THREAD_STACK_MIN);
		for (int i=0;i<1;i++)
		{
            boost::shared_ptr<boost::thread> pThreadSendFile(new boost::thread(boost::bind(&CSendFileThread::SendData, this)));
//			boost::shared_ptr<boost::thread> pThreadSendFile(new boost::thread(attrs,boost::bind(&CSendFileThread::SendData, this)));
			m_pTreadSendFileList.push_back(pThreadSendFile);
		}
	}
	bool bSendByZipData = false;
	CSpeechInfo pNewSpeechInfo;
	if (m_bMultiThreadSend)
	{
		unsigned int nSendIndex = 0;
		unsigned int i = 0;
		while (!m_bKilled && m_pSendFileIndex.front(i))
		{
			// 苹果虚拟机，线程数少会低于512；
			if (bFirstThread && !bMove1MB && pNewSpeechInfo.m_nCurSpeed>1024*1024)
				bMove1MB = true;
			else if (bFirstThread && !bMove1MB && nSendIndex>800 && (nSendIndex%80)==50 && pNewSpeechInfo.m_nCurSpeed>128*1024 && pNewSpeechInfo.m_nCurSpeed<512*1024 && m_pTreadSendFileList.size()<5)
			{
                boost::shared_ptr<boost::thread> pThreadSendFile(new boost::thread(boost::bind(&CSendFileThread::SendData, this)));
//				boost::thread_attributes attrs;
//                attrs.set_stack_size(CGC_THREAD_STACK_MIN);
//				boost::shared_ptr<boost::thread> pThreadSendFile(new boost::thread(attrs,boost::bind(&CSendFileThread::SendData, this)));
				m_pTreadSendFileList.push_back(pThreadSendFile);
				//char lpszBuffer[128];
				//sprintf(lpszBuffer,"1 SendThreadSize=%d",m_pTreadSendFileList.size());
				//MessageBox(NULL,lpszBuffer,"P2P",MB_OK);
			}

			const mycp::bigint nindex = i*const_size;
			size_t readed = 0;
			{
				// 读文件
				boost::mutex::scoped_lock lockFile(m_mutex);
#ifdef WIN32
				_fseeki64(m_file, nindex, SEEK_SET);
#else
				fseeko(m_file, nindex, SEEK_SET);
#endif
				readed = fread(lpBuffer, 1, const_size, m_file);
				if (readed <= 0)
				{
					const int ret = ferror(m_file);
					if (ret)
					{
						clearerr(m_file);
						m_pSendFileIndex.add(i);
						continue;
					}
					break;
				}
			}
			// 发送文件
			pAttach->setIndex(nindex);
			pAttach->setAttach(lpBuffer, (unsigned int)readed);
			m_pChatManager->SendDSSend(m_nMsgId, pAttach, m_bDisableZip);

			//const int const_check_date_count = pNewSpeechInfo.m_nCurSpeed==0?100:(m_bP2P?500:300);
			nSendIndex++;
			//if ((nSendIndex % const_check_date_count) == 0)
			if ((nSendIndex % 50) == 20)	// 50+20=大70个包后，开始获取第一次速度；
			{
				// 每发送500个，判断一次速度
				m_pChatManager->GetFileSpeechInfo(m_nMsgId,pNewSpeechInfo,nfiles);
			}
			if (nsendcount > 0 && i == 0)
			{
				// 用于对方第一次处理数据
#ifdef WIN32
				Sleep(1000);
#else
				usleep(1000000);
#endif
			}else
			{
				if ((m_bLocalIpAddress||m_bP2P) && i<30)
				{
					// 开始30个包，发送慢一点
#ifdef WIN32
					Sleep(5);
#else
					usleep(5000);
#endif
					continue;
				}
				// 公网发太快，也是丢包，或者收不到
				unsigned int nSleepValue = 0;
				if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>256*1024)
				{
					if ((nSendIndex%3)==1)		// 4
						nSleepValue = nfiles+1;
					else
						continue;
				}else if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>64*1024)
				{
					nSleepValue = nfiles+1;
				}else if (m_bLocalIpAddress || m_bP2P)
				{
					// 如果限制64KB，等等时间*4，发太快也没用；
					//if (pNewSpeechInfo.m_nCurSpeed>128*1024)
					//{
					//	if ((nSendIndex%3)==1)		// 4
					//		nSleepValue = 1;
					//	else
					//		continue;
					//}else
					{
						//nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?2:const_default_sleep_p2p*3;
						nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?2:const_default_sleep_p2p*5;
						//if (pNewSpeechInfo.m_nCurSpeed==0)
						//	nSleepValue = 2;
						//else if (pNewSpeechInfo.m_nCurSpeed<64*1024)	// 正常情况下，传输速度慢，说明丢包严重，需要调慢发送速度；
						//	nSleepValue = const_default_sleep_p2p*5;
						//else if (pNewSpeechInfo.m_nCurSpeed<128*1024)	// 正常情况下，传输速度慢，说明丢包严重，需要调慢发送速度；
						//	nSleepValue = const_default_sleep_p2p*4;
						//else
						//	nSleepValue = const_default_sleep_p2p*3;		// 正常使用*3
					}
				}else
				{
					nSleepValue = nfiles*const_default_sleep_value1;
				}
#ifdef WIN32
				Sleep(nSleepValue);
#else
				usleep(nSleepValue*1000);
#endif
			}
		}

		//if (!bFirstThread)	// 其他线程全部返回，留下第一个线程做数据补偿
		//{
		//	delete[] lpBuffer;
		//	return;
		//}
	}else
	{
		// ***
#ifdef USES_ZIPFILE_TO_SEND
		if (!m_bP2P && !m_bDisableZip && (m_nAcceptEncoding&2)==2)	// * 1=gzip,2=deflate(default),3=both
		{
			// deflate
			// 压缩发送
			if (m_pZib.OpenZib())
			{
				bSendByZipData = true;
				m_nAttachEncoding = 2;	// ** deflate
#ifdef WIN32
				_fseeki64(m_file, 0, SEEK_SET);
#else
				fseeko(m_file, 0, SEEK_SET);
#endif

				if (m_fEbConfig ==NULL)
				{
					char lpszBuffer[260];
#ifdef _QT_MAKE_
                    sprintf(lpszBuffer,"%s.ebconfig",m_sFileName.toLocal8Bit().constData());
#else
                    sprintf(lpszBuffer,"%s.ebconfig",m_sFileName.c_str());
#endif
                    m_fEbConfig = fopen(lpszBuffer,"w+b");
					if (m_fEbConfig==NULL)
						return;
					m_sRemoveFileName = lpszBuffer;
				}
				m_nUnSendDataSize = 0;
				//m_pUnSendDataSourceIndex.reset();
				if (m_pSendBuffer==NULL)
					m_pSendBuffer = new unsigned char[const_size+1];
				if (m_pSendZipAttach.get()==NULL)
					m_pSendZipAttach = cgcAttachment::create();
				m_pSendZipAttach->setTotal(m_ntotallen);
				m_nSendZipDataSizeTemp = 0;
				m_nZipFileSize = 0;
                m_pZib.File2Cb(m_file,(uLong)m_ntotallen,ZipFileToSendCallBack,(unsigned long)this,(uLong)min(m_ntotallen,const_zip_pack_size));
				fflush(m_fEbConfig);
				if (m_nUnSendDataSize>0)
				{
					// 发送 之前未发送数据
					//const mycp::bigint nSourceSendIndex = (mycp::bigint)(m_pSendIndexList.size()*const_size);
					//const mycp::bigint nUnSendIndex = ((m_pUnSendDataSourceIndex->m_nSourceIndex+m_pUnSendDataSourceIndex->m_nSendDataSourceIndexOffset+(const_size-1))/const_size)*const_size;
					m_pSendZipAttach->setIndex(m_nZipFileSize);
					//m_pSendZipAttach->setIndex(nSourceSendIndex);
					m_pSendZipAttach->setAttach(m_pSendBuffer, (unsigned int)m_nUnSendDataSize);
					m_nZipFileSize += (uLong)m_nUnSendDataSize;
					m_pChatManager->SendDSSend(m_nMsgId, m_pSendZipAttach, m_bDisableZip, m_nAttachEncoding, (mycp::bigint)m_nZipFileSize);
					//m_pSendIndexList.insert(nSourceSendIndex,CSourceIndexInfo::create(m_pUnSendDataSourceIndex->m_nSourceIndex,m_pUnSendDataSourceIndex->m_nSendDataSourceIndexOffset),false);
					m_nUnSendDataSize = 0;
				}
			}
		}
#endif // USES_ZIPFILE_TO_SEND

		if (!bSendByZipData)
		{
			for (unsigned int i=0; !m_bKilled && i<nsendcount; i++)
			{
				mycp::bigint nindex = i*const_size;
				pAttach->setIndex(nindex);
				const size_t readed = fread(lpBuffer, 1, const_size, m_file);
				if (readed <= 0)
				{
					int ret = ferror(m_file);
					if (ret)
					{
						clearerr(m_file);
						continue;
					}
					break;
				}
				pAttach->setAttach(lpBuffer, (unsigned int)readed);
				m_pChatManager->SendDSSend(m_nMsgId, pAttach, m_bDisableZip);
				const int const_check_date_count = pNewSpeechInfo.m_nCurSpeed==0?50:((m_bLocalIpAddress||m_bP2P)?50:30);
				if ((i % const_check_date_count) == 0)
				{
					// 每发送500个，判断一次速度
					m_pChatManager->GetFileSpeechInfo(m_nMsgId,pNewSpeechInfo,nfiles);
					// ?下面算法不好用，以后再优化。
					//if (pNewSpeechInfo.m_percent = 0.0 ||
					//	(pNewSpeechInfo.m_time>0&&(time(0)-pNewSpeechInfo.m_time)>=8))	// 过期数据，可能收不到数据
					//{
					//	nsleepvalue = min(const_default_sleep_value2,nsleepvalue+20);
					//}else if (pNewSpeechInfo.m_nCurSpeed>pOldSpeechInfo.m_nCurSpeed+5*1024)
					//{
					//	// 快10 BYTE
					//	nsleepvalue = max(const_default_sleep_value1,nsleepvalue-3);
					//}else if (pNewSpeechInfo.m_nCurSpeed>pOldSpeechInfo.m_nCurSpeed+3*1024)
					//{
					//	// 快10 BYTE
					//	nsleepvalue = max(const_default_sleep_value1,nsleepvalue-2);
					//}else if (pOldSpeechInfo.m_nCurSpeed>pNewSpeechInfo.m_nCurSpeed+5*1024)
					//{
					//	// 慢10 BYTE
					//	nsleepvalue = min(const_default_sleep_value2,nsleepvalue+10);
					//}else if (pOldSpeechInfo.m_nCurSpeed>pNewSpeechInfo.m_nCurSpeed+3*1024)
					//{
					//	// 慢10 BYTE
					//	nsleepvalue = min(const_default_sleep_value2,nsleepvalue+5);
					//}
					//pOldSpeechInfo=pNewSpeechInfo;
				}
				if (nsendcount > 0 && i == 0)
				{
					// 用于对方第一次处理数据
#ifdef WIN32
					Sleep(1000);
#else
					usleep(1000000);
#endif
				}else
				{
					// 公网发太快，也是丢包，或者收不到
					unsigned int nSleepValue = 0;
					if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>256*1024)
					{
						if ((i%6)==1)
							nSleepValue = nfiles+1;
						else
							continue;
					}else if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>64*1024)
					{
						nSleepValue = nfiles+1;
						//if (i%2==1)
						//	nSleepValue = nfiles+1;
						//else
						//	continue;
					}else if (m_bLocalIpAddress||m_bP2P)
					{
						// 如果限制64KB，等等时间*4，发太快也没用；
						//if (pNewSpeechInfo.m_nCurSpeed>128*1024)
						//{
						//	if (i%6==1)
						//		nSleepValue = 1;
						//	else
						//		continue;
						//}else
						{
							//nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?2:const_default_sleep_p2p*3;
							nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?2:const_default_sleep_p2p*5;
							//if (pNewSpeechInfo.m_nCurSpeed==0)
							//	nSleepValue = 2;
							//else if (pNewSpeechInfo.m_nCurSpeed<64*1024)	// 正常情况下，传输速度慢，说明丢包严重，需要调慢发送速度；
							//	nSleepValue = const_default_sleep_p2p*5;
							//else if (pNewSpeechInfo.m_nCurSpeed<128*1024)	// 正常情况下，传输速度慢，说明丢包严重，需要调慢发送速度；
							//	nSleepValue = const_default_sleep_p2p*4;
							//else
							//	nSleepValue = const_default_sleep_p2p*3;		// 正常使用*3
						}
					}else
					{
						nSleepValue = nfiles*const_default_sleep_value1;
						//nSleepValue = nfiles*GetSpeechSleepValue(pNewSpeechInfo.m_nCurSpeed);
					}
#ifdef WIN32
					Sleep(nSleepValue);
#else
					usleep(nSleepValue*1000);
#endif
				}

				if ((m_bLocalIpAddress||m_bP2P) && i<20)
				{
					// 开始50个包，发送慢一点
#ifdef WIN32
					Sleep(5);
#else
					usleep(5000);
#endif
				}
			}
		}
	}

	if (m_bMultiThreadSend && !bFirstThread)
	{
		// *数据补偿，只保留一个默认线程即可，其他线程全部返回退出
		return;
	}

	const int const_check_date_count = (m_bLocalIpAddress||m_bP2P)?80:50;
	unsigned long tSendTime = timeGetTime();
	int i=0;
	while (!m_bKilled)
	{
		// 检查需要发送的补偿数据
		mycp::bigint nindex = 0;
		{
			BoostWriteLock wtLock(m_pResendIndex.mutex());
			CLockMap<mycp::bigint,bool>::iterator pIter = m_pResendIndex.begin();
			if (pIter==m_pResendIndex.end())
			{
				wtLock.unlock();
				if (!m_bMultiThreadSend || bFirstThread)
				{
					const unsigned long noffset = timeGetTime()-tSendTime;
					const unsigned long const_wait_time = (m_bLocalIpAddress||m_bP2P)?2500:250;	// P2P主要由对方ds_ack，本端等待1500，主要是为了预防对方请求丢包，本端主动检查一次；
					if (noffset >= const_wait_time)
					{
						tSendTime = timeGetTime();	// 复位，实现200ms后再请求DSCheck
						m_pChatManager->SendDSCheck(m_nMsgId,false,NullPOPSotpRequestInfo);
					}
				}
#ifdef WIN32
				Sleep(10);
#else
				usleep(10000);
#endif
				continue;
			}else
			{
				nindex = pIter->first;
				m_pResendIndex.erase(pIter);

				//if (bFirstThread && (i%800)==1 && pNewSpeechInfo.m_nCurSpeed>128*1024 && pNewSpeechInfo.m_nCurSpeed<2*1024*1024 && m_pTreadSendFileList.size()<10)
				//{
				//	boost::thread_attributes attrs;
				//	attrs.set_stack_size(CGC_THREAD_STACK_MIN);
				//	boost::thread* pThreadSendFile = new boost::thread(attrs,boost::bind(thread_send_file, (void*)this));
				//	m_pTreadSendFileList.push_back(pThreadSendFile);

				//	char lpszBuffer[128];
				//	sprintf(lpszBuffer,"2 SendThreadSize=%d",m_pTreadSendFileList.size());
				//	MessageBox(NULL,lpszBuffer,"P2P",MB_OK);
				//}
			}
		}
		if (bSendByZipData)
		{
			if (nindex>=(mycp::bigint)m_nZipFileSize)
				continue;
			const uLong nWaitToSendSize = (uLong)min(m_nZipFileSize-nindex,const_size);
#ifdef WIN32
			_fseeki64(m_fEbConfig, nindex, SEEK_SET);
#else
			fseeko(m_fEbConfig, nindex, SEEK_SET);
#endif
			m_nUnSendDataSize = fread(m_pSendBuffer,1,nWaitToSendSize,m_fEbConfig);

			//CSourceIndexInfo::pointer pWaitToSendSourceInfo;
			//if (!m_pSendIndexList.find(nindex,pWaitToSendSourceInfo))
			//{
			//	continue;
			//}
//			if (m_pWaitToSendDataSourceIndex.get()==NULL)
//			{
//				m_pWaitToSendDataSourceIndex = CSourceIndexInfo::create(nWaitToSendSize,pWaitToSendSourceInfo->m_nSendDataSourceIndexOffset);
//			}else
//			{
//				m_pWaitToSendDataSourceIndex->m_nSourceIndex = nWaitToSendSize;
//				m_pWaitToSendDataSourceIndex->m_nSendDataSourceIndexOffset = pWaitToSendSourceInfo->m_nSendDataSourceIndexOffset;
//			}
//			boost::mutex::scoped_lock lockFile(m_mutex);
//#ifdef WIN32
//			_fseeki64(m_file, pWaitToSendSourceInfo->m_nSourceIndex, SEEK_SET);
//#else
//			fseeko(m_file, pWaitToSendSourceInfo->m_nSourceIndex, SEEK_SET);
//#endif
//			uLong nOutSize = 0;
//			m_nUnSendDataSize = 0;
//			if (m_pLastSourceIndex.get()!=NULL)
//				m_pLastSourceIndex->m_nSendDataSourceIndexOffset = 0;
//			m_pZib.File2Cb(m_file,(uLong)(m_ntotallen-nindex),ZipFileToSendCallBack,(unsigned int)this,(uLong)min(m_ntotallen-nindex,const_zip_pack_size),&nOutSize);
			if (m_nUnSendDataSize>0)
			{
				pAttach->setIndex(nindex);
				pAttach->setAttach(m_pSendBuffer, (unsigned int)m_nUnSendDataSize);
				m_pChatManager->SendDSSend(m_nMsgId, pAttach, m_bDisableZip, m_nAttachEncoding, (mycp::bigint)m_nZipFileSize);
				m_nUnSendDataSize = 0;
			}
		}else
		{
			size_t readed = 0;
			{
				boost::mutex::scoped_lock lockFile(m_mutex);
#ifdef WIN32
				_fseeki64(m_file, nindex, SEEK_SET);
#else
				fseeko(m_file, nindex, SEEK_SET);
#endif
				readed = fread(lpBuffer, 1, const_size, m_file);
				if (readed <= 0)
				{
					const int ret = ferror(m_file);
					if (ret)
					{
						clearerr(m_file);
						m_pResendIndex.insert(nindex,true,false);
						continue;
					}
					break;
				}
			}
			pAttach->setIndex(nindex);
			pAttach->setAttach(lpBuffer, (unsigned int)readed);
			m_pChatManager->SendDSSend(m_nMsgId, pAttach, m_bDisableZip);
		}
		if (!m_bMultiThreadSend || bFirstThread)
			tSendTime = timeGetTime();
		if (((++i)%const_check_date_count)==0)	// 定期检查一次，剩下多少文件在传送
			m_pChatManager->GetFileSpeechInfo(m_nMsgId,pNewSpeechInfo,nfiles);
		// 公网发太快，也是丢包，或者收不到
		unsigned int nSleepValue = 0;
		if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>256*1024)
		{
			if (i%3==1)
				nSleepValue = nfiles+1;
			else
				continue;
		}else if ((nfiles*pNewSpeechInfo.m_nCurSpeed)>64*1024)
		{
			nSleepValue = nfiles+3;
		}else if (m_bLocalIpAddress||m_bP2P)
		{
			// 如果限制64KB，等等时间*4，发太快也没用；
			//if (pNewSpeechInfo.m_nCurSpeed>128*1024)
			//{
			//	if (i%3==1)
			//		nSleepValue = 1;
			//	else
			//		continue;
			//}else
				//nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?const_default_sleep_p2p:const_default_sleep_p2p*4;
			nSleepValue = (pNewSpeechInfo.m_nCurSpeed==0)?const_default_sleep_p2p:const_default_sleep_p2p*6;
			//if (pNewSpeechInfo.m_nCurSpeed==0)
			//	nSleepValue = const_default_sleep_p2p;
			//else if (pNewSpeechInfo.m_nCurSpeed<64*1024)	// 正常情况下，传输速度慢，说明丢包严重，需要调慢发送速度；
			//	nSleepValue = const_default_sleep_p2p*6;
			//else if (pNewSpeechInfo.m_nCurSpeed<128*1024)	// 正常情况下，传输速度慢，说明丢包严重，需要调慢发送速度；
			//	nSleepValue = const_default_sleep_p2p*5;
			//else
			//	nSleepValue = const_default_sleep_p2p*4;		// 正常使用*3
		}else
		{
			nSleepValue = nfiles*const_default_sleep_value1;
		}
		//const unsigned int nSleepValue = (nfiles)*(m_bP2P?const_default_sleep_p2p:const_default_sleep_value1);
		// 补偿数据，发慢一些
#ifdef WIN32
		Sleep(nSleepValue+3);
#else
		usleep((nSleepValue+3)*1000);
#endif
	}
	delete []lpBuffer;
}

} // namespace entboost
