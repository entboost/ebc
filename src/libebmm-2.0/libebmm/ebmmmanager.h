#ifndef EBMMMANAGER_H
#define EBMMMANAGER_H

#include <QCamera>
#include <QCameraInfo>
//#include <QCameraImageCapture>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>

#include "../include/EBRtpManager.h"
#include "../ebaacenc/ebaacencoder.h"
#include "../ebaacdec/ebaacdecoder.h"
#include "../include/ebffmpegproxy.h"
#include "../include/ebaudioinputiodevice.h"
#include "../include/ebabstractvideosurface.h"
#include "ebmmhandle.h"
#include "ebrtproominfolist.h"
#include "ebp2ptaskinfo.h"
#include "ebvideostreammanager.h"
#include "ebaudiostreammanager.h"
//#include "ebvideodefine.h"
//#ifdef Q_OS_MACOS
//#define uint64 quint64
//#include "opencv2/opencv.hpp"
//#endif

//typedef enum {
//    VIDEO_RGB24=0,
//    VIDEO_RGB32,
//    VIDEO_I420,
//    VIDEO_H264,
////    VIDEO_WMV9,
////    VIDEO_RV40,
////    VIDEO_XVID,
//    VIDEO_YUY2,
//}VIDEO_RGB_TYPE;

namespace entboost {

#define MMVS_APP_NAME EB_APP_NAME_MMV_SERVER
#define MMAS_APP_NAME EB_APP_NAME_MMA_SERVER

static QList<QCameraInfo> theCameras;

class EbMMManager : public QObject
        , public CEBRtpHangler
        , public EbAacEncoderCallback
{
    Q_OBJECT
public:
    EbMMManager(int nVideoWidth, int nVideoHeight);
    virtual ~EbMMManager(void);

    CEBRtpManager::pointer m_pRtpAudioManager;
    CEBRtpManager::pointer m_pRtpVideoManager;

    void setCallback(FPMMCallBack cb, void *userData);
    void Run(void);

    static void func_video_cbs(mycp::bigint nId, const VIDEOFRAME *pFrame, void *cbParam, unsigned long userData);
    void func_video_cb2(eb::bigint nId, const VIDEOFRAME *pFrame, unsigned long userData);

//    static void func_WaveCapturecbs(INT64 nId, const unsigned char *buffer,  unsigned int bufferlen, unsigned int ts ,void *udata)
//    {
//        CMMManager *pManager = (CMMManager *)udata;
//        if(pManager == 0) return;
//        pManager->func_WaveCapturecb2(nId,buffer,bufferlen,ts);
//    }
//    void func_WaveCapturecb2(INT64 nId, const unsigned char *buffer,  unsigned int bufferlen, unsigned int ts)
//    {
//        if (m_bSendLocalAudio)
//        {
//            onLocalAudioData(buffer,bufferlen,ts);
//        }
//    }

public slots:
    void onCaptureVideoFrame(const uchar *frameData, int frameSize, int width, int height, unsigned long timestamp);
    void onCaptureAudioData(const char *data, qint64 size, unsigned long timestamp);
    /// EbAacEncoderCallback
    virtual void onEncodeInitOk(const unsigned char */*decoderSCData*/, int /*size*/) {}
    virtual void onEncodeOk(const unsigned char *data, int size, unsigned long param);

    bool IsStart(void) const;
    void Stop(void);
//	//void SetDefaultQuant(int v) {m_nDefaultQuant = v;}
//	//int GetDefaultQuant(void) const {return m_nDefaultQuant;}
//	//void SetDefaultFps(int v) {m_nDefaultFps = v;}
//	//int GetDefaultFps(void) const {return m_nDefaultFps;}

    void SetRoomId(mycp::bigint nRoomId, bool bIsGroup) {m_nRoomId = nRoomId; m_bIsGroup = bIsGroup;}
    mycp::bigint GetRoomId(void) const {return m_nRoomId;}
    void SetSourceId(mycp::bigint nSourceId) {m_nSourceId = nSourceId;}
    mycp::bigint GetSourceId(void) const {return m_nSourceId;}
    bool IsGroup(void) const {return m_bIsGroup;}
    //void SetLocalVideoWndParent(HWND hWndParent) {m_hLocalVideoWndParent = hWndParent;}
    void SetParam(mycp::bigint nParam) {m_nParam = nParam;}
    mycp::bigint GetParam(void) const {return m_nParam;}

    bool Register(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam, bool bIsGroup);
    void UnRegister(void);
    void DelAllSink(void);
    void DelAudioSink(mycp::bigint nDestId);
    void DelVideoSink(mycp::bigint nDestId);
    bool AddSink(mycp::bigint nDestId, bool bAddAudioStream, QWidget *hVideoStreamWndParent=NULL);
    bool IsAudioSink(mycp::bigint nDestId) const {return m_pAudioRoomWndList.IsExistSinkWnd(nDestId);}
    bool IsVideoSink(mycp::bigint nDestId) const {return m_pVideoRoomWndList.IsExistSinkWnd(nDestId);}
    void PlayLocalVideo(bool bPlay) {m_pVideoRoomWndList.SetPlay(this->GetSourceId(),bPlay);}
    bool PlayLocalVideo(void) const {return m_pVideoRoomWndList.IsExistSinkWnd(this->GetSourceId());}
    void PlayLocalAudio(bool bPlay) {m_pAudioRoomWndList.SetPlay(this->GetSourceId(),true);}
    bool isPlayLocalAudio(void) const {return m_pAudioRoomWndList.isPlay(this->GetSourceId());}
    void PlaySinkVideo(mycp::bigint nUserId, bool bPlay) {m_pVideoRoomWndList.SetPlay(nUserId, bPlay);}
    bool isPlaySinkVideo(mycp::bigint nUserId) const {return m_pVideoRoomWndList.isPlay(nUserId);}
    void PlaySinkAudio(mycp::bigint nUserId, bool bPlay) {m_pAudioRoomWndList.SetPlay(nUserId, bPlay);}
    bool isPlaySinkAudio(mycp::bigint nUserId) const {return m_pAudioRoomWndList.isPlay(nUserId);}

    /// 视频捕捉器
    static int getVideoDevices(std::vector<QString> &pOutList);
    int GetVideoDeviceSize(void) const;
//    void InitBitmapInfo(void);
    bool InitVideoCapture(int nIndex, int nQuality);
    void StartVideoCapture(QWidget *localVideoWndParent);
    void StopVideoCapture(bool bDelStream);
    void ExitVideoCapture(bool bDelStream);
    /// 音频捕捉器
    static void func_FFT_cbs(mycp::bigint nId, const char *data, int size, void* userData);
    void func_FFT_cb2(mycp::bigint nId, const char *data, int sizd);
    bool InitAudioCapture(void);
    void StartAudioCapture(void);
    void StopAudioCapture(void);
    void ExitAudioCapture(void);
    void SetSendLocalVideo(bool bSend){m_bSendLocalVideo = bSend;}
    bool GetSendLocalVideo(void) const {return m_bSendLocalVideo;}
    void SetSendLocalAudio(bool bSend){m_bSendLocalAudio = bSend;}
    bool GetSendLocalAudio(void) const {return m_bSendLocalAudio;}

    bool StartVideoGraph(mycp::bigint nUserId) {return m_pVideoRoomWndList.BuildGraph(nUserId,false,true);}
    bool MoveVideoWindow(mycp::bigint nUserId, int x, int y, int width, int height){return m_pVideoRoomWndList.MoveVideoWindow(nUserId, x, y, width, height);}
    bool ShowVideoWindow(mycp::bigint nUserId, bool bShow){return m_pVideoRoomWndList.ShowVideoWindow(nUserId, bShow);}
    bool saveImageFile(mycp::bigint nUserId, const QString &sSaveToFile) {return m_pVideoRoomWndList.saveImageFile(nUserId,sSaveToFile);}

    int GetAudioP2PStatus(void) const;
    int GetVideoP2PStatus(void) const;

protected:
    enum P2P_STATUS{
        P2P_NONE=0,
        P2P_WAIT,   /// ?
        P2P_OK
    };

    enum{
        TYPE_NOCOMPRESS=0,
        TYPE_XVID,
        TYPE_H264
    };
    unsigned int m_uTimeStampVideo;
    unsigned short m_nVideoSeq;
    bool m_bSendLocalVideo;
    bool m_bSendLocalAudio;

    EbRtpRoomInfoList m_pAudioRoomWndList;
    EbRtpRoomInfoList m_pVideoRoomWndList;
protected:
//    virtual void customEvent(QEvent *e);
//    void initVideoCapture(void);
    void GetLocalHostInfo(void);
    unsigned char* m_pSendDataBuffer;
    unsigned int m_nSendBufferSize;
    void onLocalAudioData(const unsigned char* pData, unsigned int nSize, unsigned int nTimestamp);
    void onLocalVideoData(const unsigned char* pData, unsigned int nSize, bool bKeyFrame, unsigned int nTimestamp);
    virtual void OnRtpFrame(mycp::bigint nSrcId, const CSotpRtpFrame::pointer& pRtpFrame, mycp::uint16 nLostCount, mycp::uint32 nUserData, CEBRtpManager* pLCOwner);
    virtual void OnRDOnResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner);

    bool DoCheckP2PState(const tstring& sLocalAddress, const tstring& sRemoteAddress,const EbRtpSinkInfo::pointer& pDestSink);
    CLockList<CEBRtpManager::pointer> m_pClearChatRoom;
    bool DoP2PTask(bool bAudio, mycp::bigint sCallId, mycp::bigint sChatId,mycp::bigint nFromUserId,const tstring& sLocalAddress, const tstring& sRemoteAddress,bool& pOutP2POk);
    virtual void OnP2POk(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner);
    virtual void OnFCMP2PRequest(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner);
    EbP2PTaskInfo::pointer m_pAudioP2PTaskInfo;
    EbP2PTaskInfo::pointer m_pVideoP2PTaskInfo;
    virtual void OnFCMP2PResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner);
    virtual void OnCMP2PResponseRes(const CPOPSotpRequestInfo::pointer & pRequestInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CEBRtpManager* pCMOwner);
    void OnProcessTimer(const CEBRtpManager* pCMOwner);
private:
    static EbFfmpegProxy * BuildH264Encoder(int nVideoQuality, int nFPS, AVPixelFormat nVideoType);

protected:
    FPMMCallBack m_callback;
    void *m_cbUserData;
    QObject *m_receiver;

    int m_nVideoWidth;
    int m_nVideoHeight;
    int m_nEnDecodeType;
    int m_nVideoQuality;	/// 清晰度控制，0（节省带宽）-10（高清），默认3标清
    int m_nVideoIndex;	/// for initVideoCapture()
    int m_nDefaultFps;
    QImage::Format m_imageFormat;
    AVPixelFormat m_ffmpegFormat;
    unsigned char m_videoFrameBuffer[EB_MAX_VIDEO_FRAME_SIZE]; /// 用户本地编码的缓冲
    EbFfmpegProxy *m_pX264Encoder; /// x264编码器
    EbFfmpegProxy *m_pX264Decoder; /// x264解码器
//    CVideoEncoderX264 *m_pX264Encoder; /// x264编码器
    ///音频捕捉
    QAudioInput *m_audioInput;
    EbAudioInputIODevice *m_audioInputIODevice;
//    CWaveInCapture    *m_pWaveCapture;
    EbAacEncoder *m_pAudioEncoder;   /// aac 音频编码器
    EbAacEncoder *m_pAudioDecoder;   /// aac 音频解码器
//    CAudioEncoderFaac *m_pAudioEncoder;
    /// 视频捕捉
    QCamera *m_camera;
//#ifdef Q_OS_MACOS
//    cv::VideoCapture  m_videoCapture;
//    cv::Mat m_cvframe;
//#endif
    EbAbstractVideoSurface *m_videoSurface;
    EbVideoStreamManager *m_pVideoStreamManger;
    EbAudioStreamManager *m_pAudioStreamManager;

    /// ???未初始化
//    BITMAPINFO        g_bmiOrg;
//    BITMAPINFO        g_bmiComp;
//    unsigned char g_compVopbuffer[MAX_VOP_SIZE]; //用户本地编码的缓冲
    //unsigned int  g_compVopBufferLength;

    mycp::bigint m_nRoomId;
    mycp::bigint m_nSourceId;
    bool m_bIsGroup;
    QWidget *m_hLocalVideoWndParent;	// 本地视频HWND
//    HWND m_hLocalVideoWndParent;	// 本地视频HWND
    mycp::bigint m_nParam;
    std::vector<std::string> m_pLocalHostIp;
    CEBRtpManager::pointer m_pRDAudioManagerP2POk;
    CEBRtpManager::pointer m_pRDVideoManagerP2POk;
    bool m_nVideoLocalP2PFlag;		// true:内网；false:公网
};

} // namespace entboost

#endif // EBMMMANAGER_H
