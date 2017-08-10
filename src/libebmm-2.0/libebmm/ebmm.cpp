// ebmm.cpp : Defines the exported functions for the DLL application.
//
#include "../include/EBRtpManager.h"
#define EBMM_EXPORTS
#include "ebmm.h"
#include <QFileInfo>
#include <QCoreApplication>
#include <QImage>
#include <QDir>
#include <QFile>
#include "../include/boost_ini.h"
#include "ebmmmanager.h"

namespace entboost {

Cebmm::Cebmm(void)
    : m_handle(NULL)
{
}

Cebmm::~Cebmm(void)
{
    Stop();
}

Cebmm::pointer Cebmm::create()
{
    return Cebmm::pointer(new Cebmm());
}

int ParseString(const char * lpszString, const char * lpszInterval, std::vector<std::string> & pOut)
{
	std::string sIn(lpszString);
	pOut.clear();
	while (!sIn.empty())
	{
		std::string::size_type find = sIn.find(lpszInterval);
		if (find == std::string::npos)
		{
			pOut.push_back(sIn);
			break;
		}
		if (find==0)
			pOut.push_back("");	// 空
		else
			pOut.push_back(sIn.substr(0, find));
		sIn = sIn.substr(find+strlen(lpszInterval));
	}
	return (int)pOut.size();
}
std::string GetHostIp(const char * lpszHostName,const char* lpszDefaultName)
{
	if (lpszHostName==NULL || strlen(lpszHostName)==0)
		return lpszDefaultName;
	try
	{
		struct hostent *host_entry;
		//struct sockaddr_in addr;
		/* 即要解析的域名或主机名 */
		host_entry=gethostbyname(lpszHostName);
		//printf("%s\n", dn_or_ip);
		if(host_entry!=0)
		{
			//printf("解析IP地址: ");
			char lpszIpAddress[50];
			memset(lpszIpAddress, 0, sizeof(lpszIpAddress));
			sprintf(lpszIpAddress, "%d.%d.%d.%d",
				(host_entry->h_addr_list[0][0]&0x00ff),
				(host_entry->h_addr_list[0][1]&0x00ff),
				(host_entry->h_addr_list[0][2]&0x00ff),
				(host_entry->h_addr_list[0][3]&0x00ff));
			return lpszIpAddress;
		}
	}catch(std::exception&)
	{
	}catch(...)
	{}
	return lpszDefaultName;
}
tstring tehAppDataTempPath;
tstring GetAddressList(const tstring& sFullAddress)
{
	std::vector<std::string> pList;
	if (ParseString(sFullAddress.c_str(),":",pList)!=2)
		return "";

	const tstring sAddress = pList[0];
	char sTempIniFile[260];
    sprintf(sTempIniFile,"%s/temp.ini",tehAppDataTempPath.c_str());
	char lpszIp[101];
	memset(lpszIp,0,101);
    GetPrivateProfileStringABoost("address_list",sAddress.c_str(),"",lpszIp,100,sTempIniFile);
	if (strlen(lpszIp)==0)
	{
		return "";
	}
	tstring result = lpszIp;
	result.append(":");
	result.append(pList[1]);
	return result;
}
void SaveAddressList(const tstring& sFullAddress)
{
	std::vector<std::string> pList;
	if (ParseString(sFullAddress.c_str(),":",pList)==2)
	{
		const tstring sAddress(pList[0]);
		const std::string sIp = GetHostIp(sAddress.c_str(),sAddress.c_str());
		if (sAddress!=sIp)
		{
			char sTempIniFile[260];
            sprintf(sTempIniFile,"%s/temp.ini",tehAppDataTempPath.c_str());
            WritePrivateProfileStringABoost("address_list", sAddress.c_str(), sIp.c_str(), sTempIniFile);
		}
	}
}

//void Cebmm::Init(void)
//{
//	EbMMManager * pChatManager = (EbMMManager*)m_handle;
//	if (pChatManager == NULL)
//	{
//		int nVideoWidth = 320;
//		int nVideoHeight = 240;
//		pChatManager = new EbMMManager(nVideoWidth, nVideoHeight);
//		m_handle = pChatManager;
//	}
//}

int Cebmm::Start(const char* sAudioAddress, const char* sVideoAddress, unsigned int bindPort)
{
	int result = -1;
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager==0) {
		const int nVideoWidth = 320;
		const int nVideoHeight = 240;
        pChatManager = new EbMMManager(nVideoWidth, nVideoHeight);
		bool bRetry = false;
        if (sAudioAddress!=0 && strlen(sAudioAddress)>0)
		{
            /// 设置音频服务器
			//pChatManager->m_pRtpAudioManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
            result = pChatManager->m_pRtpAudioManager->Start(
                        CCgcAddress(sAudioAddress, CCgcAddress::ST_UDP), MMAS_APP_NAME, bindPort, 0);
			pChatManager->m_pRtpAudioManager->SetRtpCallbackData(1);
			if (result==-1)
			{
                /// 有可能域名解析问题，再试一次；
				const tstring sServerAddress = GetAddressList(sAudioAddress);
				if (!sServerAddress.empty())
				{
					delete pChatManager;
                    pChatManager = new EbMMManager(nVideoWidth, nVideoHeight);
					//pChatManager->m_chatManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
					result = pChatManager->m_pRtpAudioManager->Start(CCgcAddress(sAudioAddress, CCgcAddress::ST_UDP), MMAS_APP_NAME, bindPort, 0);
					pChatManager->m_pRtpAudioManager->SetRtpCallbackData(1);
					bRetry = true;
				}
				if (result==-1)
				{
					delete pChatManager;
					return result;
				}
			}
			if (!bRetry)
			{
				SaveAddressList(sAudioAddress);
			}
			bRetry = false;
		}
		if (sVideoAddress!=NULL && strlen(sVideoAddress)>0)
		{
			// 设置视频服务器
			//pChatManager->m_pRtpVideoManager->SetSslKey(theSslPublicKey,theSslPrivateKey,theSslPrivatePwd);
			result = pChatManager->m_pRtpVideoManager->Start(CCgcAddress(sVideoAddress, CCgcAddress::ST_UDP), MMVS_APP_NAME, bindPort, 0);
			pChatManager->m_pRtpVideoManager->SetRtpCallbackData(2);
			if (result==-1)
			{
				delete pChatManager;
				return result;
			}
			if (!bRetry)
			{
				SaveAddressList(sVideoAddress);
			}
		}
		pChatManager->Run();
		m_handle = pChatManager;
	}else
	{
		result = 1;
	}
	return result;
}


bool Cebmm::IsStart(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->IsStart();
	}
	return false;
}
void Cebmm::SetCallBack(FPMMCallBack pCallBack, void *pUserData)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0) {
        pChatManager->setCallback(pCallBack, pUserData);
	}
}
//void Cebmm::SetMsgHwnd(HWND pHwnd)
//{
//	EbMMManager * pChatManager = (EbMMManager*)m_handle;
//	if (pChatManager!=0)
//	{
//		pChatManager->m_pHwnd = pHwnd;
//	}
//}
////void Cchatroom::SetParam(unsigned long nParam)
////{
////	CChatManager * pChatManager = (CChatManager*)m_handle;
////	if (pChatManager!=0)
////	{
////		pChatManager->m_nParam = nParam;
////		//pChatManager->m_sParam = "";
////	}
////}
////
void Cebmm::Stop(void)
{
	//StopLocalDesktop();
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
	m_handle = NULL;
    if (pChatManager!=0)
	{
		pChatManager->UnRegister();
		pChatManager->DelAllSink();
		pChatManager->Stop();
		delete pChatManager;
	}
}

////void Cebrd::SetRDFPS(int fps)
////{
////	EbMMManager * pChatManager = (EbMMManager*)m_handle;
////	if (pChatManager!=0)
////	{
////		pChatManager->SetFPS(fps);
////	}
////}
////int Cebrd::GetRDFPS(void) const
////{
////	EbMMManager * pChatManager = (EbMMManager*)m_handle;
////	if (pChatManager!=0)
////	{
////		return pChatManager->GetFPS();
////	}
////	return 0;
////}
//

mycp::bigint Cebmm::GetRoomId(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->GetRoomId();
	}
	return 0;
}
mycp::bigint Cebmm::GetSourceId(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->GetSourceId();
	}
	return 0;
}

bool Cebmm::Register(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam, bool bIsGroup)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->Register(nRoomId, nSourceId, nParam, bIsGroup);
	}
	return false;
}
void Cebmm::UnRegister(void)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->UnRegister();
	}
}

bool Cebmm::AddAudioSink(mycp::bigint nDestId)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->AddSink(nDestId, true, NULL);
	}
	return false;
}
bool Cebmm::IsAudioSink(mycp::bigint nDestId) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->IsAudioSink(nDestId);
	}
	return false;
}
bool Cebmm::AddVideoSink(mycp::bigint nDestId, QWidget *hVideoStreamWndParent)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->AddSink(nDestId, false, hVideoStreamWndParent);
	}
	return false;
}
bool Cebmm::IsVideoSink(mycp::bigint nDestId) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->IsVideoSink(nDestId);
	}
	return false;
}

void Cebmm::DelAudioSink(mycp::bigint nDestId)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->DelAudioSink(nDestId);
	}
}
void Cebmm::DelVideoSink(mycp::bigint nDestId)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->DelVideoSink(nDestId);
	}
}
void Cebmm::DelDestSink(mycp::bigint nDestId)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->DelAudioSink(nDestId);
		pChatManager->DelVideoSink(nDestId);
	}
}

void Cebmm::DelAllSink(void)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->DelAllSink();
	}
}

void Cebmm::GetVideoDevices(std::vector<QString> &pOutDevices)
{
    EbMMManager::getVideoDevices(pOutDevices);
}
bool Cebmm::InitVideoCapture(int nIndex, int nQuality)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->InitVideoCapture(nIndex, nQuality);
	}
	return false;
}
int Cebmm::GetVideoDeviceSize(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->GetVideoDeviceSize();
	}
	return 0;
}

bool Cebmm::InitAudioCapture(void)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->InitAudioCapture();
	}
	return false;
}
void Cebmm::StartVideoCapture(QWidget *hLocalVideoWndParent)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
    {
        /// 图像翻转
//        QImage* imgRatate = new QImage;
//        QMatrix matrix;
//        matrix.rotate(270);
//        *imgRotate = img->transformed(matrix);
//        ui->label->setPixmap(QPixmap::fromImage(*imgRotate));
        pChatManager->StartVideoCapture(hLocalVideoWndParent);
	}
}
void Cebmm::StopVideoCapture(void)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->StopVideoCapture(true);
	}
}
void Cebmm::StartAudioCapture(void)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->StartAudioCapture();
	}
}
void Cebmm::StopAudioCapture(void)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->StopAudioCapture();
	}
}
void Cebmm::SetLocalVideoSend(bool bSend)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->SetSendLocalVideo(bSend);
	}
}
bool Cebmm::GetLocalVideoSend(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->GetSendLocalVideo();
	}
	return false;
}
void Cebmm::SetLocalAudioSend(bool bSend)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->SetSendLocalAudio(bSend);
	}
}
bool Cebmm::GetLocalAudioSend(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->GetSendLocalAudio();
	}
	return false;
}

bool Cebmm::StartVideoGraph(mycp::bigint nUserId)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->StartVideoGraph(nUserId);
	}
	return false;
}
bool Cebmm::MoveVideoWindow(mycp::bigint nUserId, int x, int y, int width, int height)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->MoveVideoWindow(nUserId, x, y, width, height);
	}
	return false;
}
bool Cebmm::ShowVideoWindow(mycp::bigint nUserId, bool bShow)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->ShowVideoWindow(nUserId, bShow);
	}
	return false;
}
bool Cebmm::SaveVideoBitmapFile(mycp::bigint nUserId, const QString &sSaveToFile)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
        return pChatManager->saveImageFile(nUserId, sSaveToFile);
	}
	return false;
}

int Cebmm::GetAudioP2PStatus(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->GetAudioP2PStatus();
	}
	return 0;
}
int Cebmm::GetVideoP2PStatus(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->GetVideoP2PStatus();
	}
	return 0;
}

void Cebmm::SetLocalVideoPlay(bool bPlay)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->PlayLocalVideo(bPlay);
	}
}
bool Cebmm::GetLocalVideoPlay(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		return pChatManager->PlayLocalVideo();
	}
	return false;
}

void Cebmm::SetLocalAudioPlay(bool bPlay)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->PlayLocalAudio(bPlay);
	}
}
bool Cebmm::GetLocalAudioPlay(void) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
        return pChatManager->isPlayLocalAudio();
	}
	return false;
}
void Cebmm::SetSinkVideoPlay(mycp::bigint nUserId, bool bPlay)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->PlaySinkVideo(nUserId, bPlay);
	}
}
bool Cebmm::GetSinkVideoPlay(mycp::bigint nUserId) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
        return pChatManager->isPlaySinkVideo(nUserId);
	}
	return false;
}

void Cebmm::SetSinkAudioPlay(mycp::bigint nUserId, bool bPlay)
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
		pChatManager->PlaySinkAudio(nUserId, bPlay);
	}
}
bool Cebmm::GetSinkAudioPlay(mycp::bigint nUserId) const
{
    EbMMManager * pChatManager = (EbMMManager*)m_handle;
    if (pChatManager!=0)
	{
        return pChatManager->isPlaySinkAudio(nUserId);
	}
	return false;
}

} // namespace entboost
