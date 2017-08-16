#include "ebmmmanager.h"

#include <QNetworkInterface>
#include <QHostAddress>
#include <QDesktopServices>
#include <QCoreApplication>
#include "ebtimegettime.h"
//#include <QMessageBox>

#define DEFAULT_CHANNELS   1
#define DEFAULT_SAMPLERATE 16000					// 11025 12000 16000（同步） 22050(会不同步)
#define DEFAULT_AUDIO_BITRATE DEFAULT_SAMPLERATE

namespace entboost {

EbMMManager::EbMMManager(int nVideoWidth, int nVideoHeight)
    : QObject(0)
    , m_callback(NULL), m_cbUserData(NULL), m_receiver(NULL)
    , m_nVideoWidth(nVideoWidth), m_nVideoHeight(nVideoHeight)
    , m_nEnDecodeType(TYPE_H264)
    , m_nVideoQuality(3)
    , m_nVideoIndex(0)
    , m_nDefaultFps(10)
    , m_imageFormat(QImage::Format_RGB888)
    , m_ffmpegFormat(AV_PIX_FMT_RGB24)
    , m_nRoomId(0), m_nSourceId(0), m_bIsGroup(false), m_hLocalVideoWndParent(NULL), m_nParam(0)
    , m_pSendDataBuffer(NULL), m_nSendBufferSize(0)
    , m_bSendLocalVideo(true), m_bSendLocalAudio(true)
    , m_nVideoLocalP2PFlag(false)

    , m_pX264Encoder(0), m_pX264Decoder(0)
    , m_audioInput(0), m_audioInputIODevice(0)
    , m_pAudioEncoder(0), m_pAudioDecoder(0)
    , m_camera(0), m_videoSurface(0)

{
//    g_pWebcam = new DirectXWebcamDriver(0, m_nVideoWidth, m_nVideoHeight);
    m_pVideoStreamManger = new EbVideoStreamManager(m_nVideoWidth, m_nVideoHeight, this);
    m_pVideoStreamManger->SetVideoStreamCallBack(func_video_cbs, (void*)this);
    m_pAudioStreamManager = new EbAudioStreamManager(DEFAULT_SAMPLERATE, DEFAULT_CHANNELS, this);

    m_uTimeStampVideo = 0;
    m_nVideoSeq = 1;
    //m_uTimeStampAudio = 0;
    //m_nStepVideo = 66;
    //m_nStepAudio = 20;

    m_pRtpAudioManager = CEBRtpManager::create(this,true);
    m_pRtpAudioManager->SetParameter(1);
    m_pRtpVideoManager = CEBRtpManager::create(this,true);
    m_pRtpVideoManager->SetParameter(2);
    //memset(&m_pDesktopData,0,RDPSTRUCT_SIZE);
}

EbMMManager::~EbMMManager()
{
    Stop();

    m_pClearChatRoom.clear();
    if (m_pVideoStreamManger!=0) {
        m_pVideoStreamManger->Stop();
        delete m_pVideoStreamManger;
        m_pVideoStreamManger = 0;
    }
    if (m_pAudioStreamManager!=0) {
        m_pAudioStreamManager->Stop();
        delete m_pAudioStreamManager;
        m_pAudioStreamManager = 0;
    }

    if (m_pSendDataBuffer!=0) {
        delete[] m_pSendDataBuffer;
        m_pSendDataBuffer = NULL;
    }
}

void EbMMManager::setCallback(FPMMCallBack cb, void *userData)
{
    m_callback = cb;
    m_cbUserData = userData;
}

void EbMMManager::Run()
{
    if (m_pVideoStreamManger!=0) {
        m_pVideoStreamManger->Run();
    }
    if (m_pAudioStreamManager!=0) {
        m_pAudioStreamManager->Run();
    }
}

void EbMMManager::func_video_cbs(mycp::bigint nId, const VIDEOFRAME *pFrame, void *cbParam, unsigned long userData)
{
    EbMMManager *pManager = (EbMMManager *)cbParam;
    if(pManager!=0) {
        pManager->func_video_cb2(nId, pFrame, userData);
    }
}

void EbMMManager::func_video_cb2(eb::bigint nId, const VIDEOFRAME *pFrame, unsigned long userData)
{
    /// 视频数据
    if (!m_pVideoRoomWndList.DrawVideo(nId, pFrame->data, pFrame->usedSize)) {
        return;
    }
    if (m_callback!=0) {
        if (m_pVideoRoomWndList.GetUserData(nId)==0) {
            m_pVideoRoomWndList.SetUserData(nId,1);
            m_callback(nId, EBMM_CALLBACK_FIRST_VIDEO_FPS, 0, 0, m_cbUserData);
        }
    }
}

void EbMMManager::onCaptureVideoFrame(const uchar *frameData, int frameSize, int width, int height, unsigned long timestamp)
{
    if (frameData==0 || frameSize<=0) {
        return;
    }
//    const int bytesPerLine = frameData.bytesPerLine();
//    const int size = bytesPerLine*height;

//    int ret = 0;
//    unsigned long dwSizeWrite = 0;
    /// timestamp
    if (m_uTimeStampVideo==0)
        m_uTimeStampVideo = timeGetTime();
    else
        m_uTimeStampVideo += 66;
    m_nVideoSeq++;

    /// 进行本地预览
    if (m_pVideoStreamManger!=0 &&
            this->m_pVideoRoomWndList.isPlay(this->GetSourceId())) {
        m_pVideoStreamManger->Write(
                    this->GetSourceId(),
                    frameSize,
                    frameData,
                    m_nVideoSeq,
                    m_uTimeStampVideo,
                    EB_RTP_PAYLOAD_TYPE_RGB, -1);
    }
//    return;
    /// 进行H264/Xvid编码和传输.
    if (m_bSendLocalVideo) {
        /// encode 视频编码
        if (m_pX264Encoder!=0) {
            bool isKeyframe = false;
            int videoDataSize = 0;
            try {
                if (m_nEnDecodeType == TYPE_XVID) {
                    /// XVID Encoder

                }
                else if(m_nEnDecodeType == TYPE_H264) {
                    try {
                        m_pX264Encoder->encodeVideo(
                                    frameData,
                                    m_videoFrameBuffer,
                                    EB_MAX_VIDEO_FRAME_SIZE,
                                    &videoDataSize,
                                    &isKeyframe);
                    }catch(const std::exception&)
                    {
                        return;
                    }catch(...)
                    {
                        return;
                    }
                }
                //return;
                if (videoDataSize > 0 ) {
                    /// 发送视频编码数据
                    onLocalVideoData(m_videoFrameBuffer, videoDataSize, isKeyframe, timeGetTime());
                }
            }
            catch(...)
            {
                //compress error.
                ///ASSERT(FALSE);
            }
        }
    }
}

void EbMMManager::onCaptureAudioData(const char *data, qint64 size, unsigned long timestamp)
{
    /// 显示本地音量条
    func_FFT_cb2(this->GetSourceId(), data, (int)size);
    if (m_bSendLocalAudio) {
        /// 网络传输
        if (m_pAudioEncoder==0) {
            m_pAudioEncoder = new EbAacEncoder(this);
            if (!m_pAudioEncoder->init(1, EB_DEFAULT_SAMPLERATE, EB_DEFAULT_AUDIO_BITRATE)) {
                delete m_pAudioEncoder;
                m_pAudioEncoder = 0;
                return;
            }
        }
        const unsigned long param = timestamp;
        m_pAudioEncoder->encode((const short*)data, (int)size, param);
    }
}

void EbMMManager::onEncodeOk(const unsigned char *data, int size, unsigned long param)
{
    if (m_bSendLocalAudio) {
        const unsigned long timestamp = param;
        onLocalAudioData((const unsigned char*)data, (unsigned int)size, timestamp);
    }
}

bool EbMMManager::IsStart() const
{
    return m_pRtpAudioManager->IsStart() || m_pRtpVideoManager->IsStart();
}

void EbMMManager::Stop()
{
    /// 停止音频捕捉器
    ExitAudioCapture();
    /// 停止视频捕捉器
    ExitVideoCapture(true);

    try{
        m_pVideoStreamManger->Stop();
    }catch(...){}
    try{
        m_pAudioStreamManager->Stop();
    }catch(...){}

    try{
        this->DelAllSink();
    }catch(...){}

    try{
        m_pRtpAudioManager->Stop();
    }catch(...){}

    try{
        m_pRtpVideoManager->Stop();
    }catch(...){}

    if (m_pX264Encoder !=0) {
        m_pX264Encoder->close();
        delete m_pX264Encoder;
        m_pX264Encoder=0;
    }
    if (m_pX264Decoder !=0) {
        m_pX264Decoder->close();
        delete m_pX264Decoder;
        m_pX264Decoder=0;
    }
    if (m_pAudioEncoder!=0) {
        m_pAudioEncoder->stop();
        delete m_pAudioEncoder;
        m_pAudioEncoder = 0;
    }
    if (m_pAudioDecoder!=0) {
        m_pAudioDecoder->stop();
        delete m_pAudioDecoder;
        m_pAudioDecoder = 0;
    }

    theCameras.clear();
}

bool EbMMManager::Register(bigint nRoomId, bigint nSourceId, bigint nParam, bool bIsGroup)
{
    SetRoomId(nRoomId, bIsGroup);
    SetSourceId(nSourceId);
    SetParam(nParam);
    bool result = false;
    if (m_pRtpAudioManager->IsStart())
    {
        m_pRtpAudioManager->SetSourceId(nSourceId);
        m_pRtpAudioManager->RegisterSource(nRoomId, nParam);	/// **需要先做一次注册，本地保存数据，否则会出错；
        if (m_pRtpAudioManager->SendRDOn(nRoomId,nSourceId,nParam,EB_LOGON_TYPE_PC)!=0)
            return false;
        result = true;
    }
    if (m_pRtpVideoManager->IsStart())
    {
        m_pRtpVideoManager->SetSourceId(nSourceId);
        m_pRtpVideoManager->RegisterSource(nRoomId, nParam);	/// **需要先做一次注册，本地保存数据，否则会出错；
        return m_pRtpVideoManager->SendRDOn(nRoomId,nSourceId,nParam,EB_LOGON_TYPE_PC)==0?true:false;
    }
    return result;
}

void EbMMManager::UnRegister()
{
    this->StopAudioCapture();
    this->StopVideoCapture(true);

    const mycp::bigint nRoomId = GetRoomId();
    const mycp::bigint nFromUserId = GetSourceId();
    m_pVideoStreamManger->DelAllUser();
    m_pAudioStreamManager->DelAllStream();

    this->m_pAudioRoomWndList.DelAllSinkWnd();
    this->m_pVideoRoomWndList.DelAllSinkWnd();

    if (m_pRtpAudioManager->IsStart())
    {
        m_pRtpAudioManager->SendRDOff(nRoomId, nFromUserId, this->GetParam());
        m_pRtpAudioManager->UnRegisterSource(nRoomId);
        m_pRtpAudioManager->UnRegisterAllSource();
    }
    if (m_pRtpVideoManager->IsStart())
    {
        m_pRtpVideoManager->SendRDOff(nRoomId, nFromUserId, this->GetParam());
        m_pRtpVideoManager->UnRegisterSource(nRoomId);
        m_pRtpVideoManager->UnRegisterAllSource();
    }
    m_pRDAudioManagerP2POk.reset();
    m_pRDVideoManagerP2POk.reset();
    m_pAudioP2PTaskInfo.reset();
    m_pVideoP2PTaskInfo.reset();
}

void EbMMManager::DelAllSink()
{
    m_pVideoStreamManger->DelAllUser();
    m_pAudioStreamManager->DelAllStream();

    m_pAudioRoomWndList.DelAllSinkWnd();
    m_pVideoRoomWndList.DelAllSinkWnd();

    m_pRtpAudioManager->UnRegisterAllSink();
    m_pRtpVideoManager->UnRegisterAllSink();
    m_pRDAudioManagerP2POk.reset();
    m_pRDVideoManagerP2POk.reset();
    m_pAudioP2PTaskInfo.reset();
    m_pVideoP2PTaskInfo.reset();
}

void EbMMManager::DelAudioSink(bigint nDestId)
{
    const mycp::bigint nRoomId = GetRoomId();
    m_pAudioStreamManager->DelStream(nDestId);

    m_pAudioRoomWndList.DelSinkWnd(nDestId);
    m_pRtpAudioManager->UnRegisterSink(nRoomId, nDestId);
    m_pRDAudioManagerP2POk.reset();
    m_pAudioP2PTaskInfo.reset();
}

void EbMMManager::DelVideoSink(bigint nDestId)
{
    const mycp::bigint nRoomId = GetRoomId();
    m_pVideoStreamManger->DelUser(nDestId);

    m_pVideoRoomWndList.DelSinkWnd(nDestId);
    m_pRtpVideoManager->UnRegisterSink(nRoomId, nDestId);
    m_pRDVideoManagerP2POk.reset();
    m_pVideoP2PTaskInfo.reset();
}

bool EbMMManager::AddSink(bigint nDestId, bool bAddAudioStream, QWidget *hVideoStreamWndParent)
{
    const mycp::bigint nRoomId = GetRoomId();
    bool ret = false;
    if (bAddAudioStream && m_pRtpAudioManager->IsStart())
    {
        ret = m_pRtpAudioManager->RegisterSink(nRoomId, nDestId)==0?true:false;
        if (!ret) return false;

        EbRtpSinkInfo::pointer pSinkWnd = m_pAudioRoomWndList.AddSinkWnd( nDestId, QImage::Format_Invalid,0,0,NULL);
        if (pSinkWnd.get()==NULL) return false;

        m_pAudioStreamManager->AddStream(nDestId);
        m_pAudioStreamManager->SetStreamFFTCallback(nDestId, func_FFT_cbs,this);
        GetLocalHostInfo();
        if (!IsGroup() && !m_pLocalHostIp.empty())
        {
            if (pSinkWnd->m_pRtpManager1.get()==NULL)
            {
                pSinkWnd->m_pRtpManager1 = CEBRtpManager::create(this);
                pSinkWnd->m_pRtpManager1->Start(m_pRtpAudioManager->GetAddress(),MMAS_APP_NAME,0,0,0);
                pSinkWnd->m_pRtpManager1->SetParameter(1);	/// 这是音频
            }

            if (!pSinkWnd->m_bLocalP2POk)	/// **本地未打通
            {
                EbP2PTaskInfo::pointer pP2PTaskInfo = m_pAudioP2PTaskInfo;
                if (pP2PTaskInfo.get()==NULL)
                {
                    pP2PTaskInfo = EbP2PTaskInfo::create();
                    m_pAudioP2PTaskInfo = pP2PTaskInfo;
                }
                pP2PTaskInfo->m_nCallId = this->GetRoomId();
                pP2PTaskInfo->m_nFromUserId = nDestId;
                pP2PTaskInfo->m_tP2PTry = time(0);
                pP2PTaskInfo->m_bP2PResponse = false;
            }
            /// **这里不用担心，对方 rd_on太迟，因为双方都会做这个动作；只要一方做请求成功就可以的；
            char lpszLocalIp[64];
            sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pSinkWnd->m_pRtpManager1->GetLocalPort());
            pSinkWnd->m_pRtpManager1->SetDestAddress(0,m_pRtpAudioManager->GetAddress().address());
            pSinkWnd->m_pRtpManager1->SendP2PRequest(this->GetSourceId(),this->GetRoomId(),nDestId,lpszLocalIp,0);	// 0 正常P2P；1：离线重试；
        }
    }
    if (hVideoStreamWndParent==0 || !m_pRtpVideoManager->IsStart()) {
        return ret;
    }
    ret = m_pRtpVideoManager->RegisterSink(nRoomId, nDestId)==0?true:false;
    if (!ret) return false;

    EbRtpSinkInfo::pointer pSinkWnd = m_pVideoRoomWndList.AddSinkWnd(
                nDestId, QImage::Format_RGB888, this->m_nVideoWidth,
                this->m_nVideoHeight, hVideoStreamWndParent);
    if (pSinkWnd.get()==0) {
        return false;
    }
    m_pVideoStreamManger->AddUser(nDestId, 800, EB_CODEC_TYPE_H264);
    //m_pVideoRoomWndList.BuildGraph(nDestId,false,false);

    GetLocalHostInfo();
    if (!IsGroup() && !m_pLocalHostIp.empty())
    {
        if (pSinkWnd->m_pRtpManager1.get()==NULL)
        {
            pSinkWnd->m_pRtpManager1 = CEBRtpManager::create(this);
            pSinkWnd->m_pRtpManager1->Start(m_pRtpVideoManager->GetAddress(),MMVS_APP_NAME,0,0,0);
            pSinkWnd->m_pRtpManager1->SetParameter(2);	// 这是视频
        }

        if (!pSinkWnd->m_bLocalP2POk)	// **本地未打通
        {
            EbP2PTaskInfo::pointer pP2PTaskInfo = m_pVideoP2PTaskInfo;
            if (pP2PTaskInfo.get()==NULL)
            {
                pP2PTaskInfo = EbP2PTaskInfo::create();
                m_pVideoP2PTaskInfo = pP2PTaskInfo;
            }
            pP2PTaskInfo->m_nCallId = this->GetRoomId();
            pP2PTaskInfo->m_nFromUserId = nDestId;
            pP2PTaskInfo->m_tP2PTry = time(0);
            pP2PTaskInfo->m_bP2PResponse = false;
        }
        // **这里不用担心，对方 rd_on太迟，因为双方都会做这个动作；只要一方做请求成功就可以的；
        char lpszLocalIp[64];
        sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pSinkWnd->m_pRtpManager1->GetLocalPort());
        pSinkWnd->m_pRtpManager1->SetDestAddress(0,m_pRtpVideoManager->GetAddress().address());
        pSinkWnd->m_pRtpManager1->SendP2PRequest(this->GetSourceId(),this->GetRoomId(),nDestId,lpszLocalIp,0);	// 0 正常P2P；1：离线重试；
    }
    return true;
}

int EbMMManager::getVideoDevices(std::vector<QString> &pOutList)
{
    if (theCameras.isEmpty()) {
        theCameras = QCameraInfo::availableCameras();
    }
    foreach (const QCameraInfo &cameraInfo, theCameras) {
        pOutList.push_back(cameraInfo.description());
    }
    return theCameras.size();
}

//void EbMMManager::InitBitmapInfo()
//{
//    /// 初始化视频BITMAPINFO信息
////    SetCodecBitmapInfo(this->m_nVideoWidth, this->m_nVideoHeight, &g_bmiOrg, &g_bmiComp);
//}

int EbMMManager::GetVideoDeviceSize() const
{
    if (theCameras.isEmpty()) {
        theCameras = QCameraInfo::availableCameras();
    }
    return theCameras.size();
}

#ifdef Q_OS_MACOS
//QImage Mat2QImage(cv::Mat cvImg)
//{
//    QImage qImg;
//    if(cvImg.channels()==3) {                            //3 channels color image
//        cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
//        qImg =QImage((const unsigned char*)(cvImg.data),
//                    cvImg.cols, cvImg.rows,
//                    cvImg.cols*cvImg.channels(),
//                    QImage::Format_RGB888);
//    }
//    else if(cvImg.channels()==1) {                    //grayscale image
//        qImg =QImage((const unsigned char*)(cvImg.data),
//                    cvImg.cols,cvImg.rows,
//                    cvImg.cols*cvImg.channels(),
//                    QImage::Format_Indexed8);
//    }
//    else {
//        qImg =QImage((const unsigned char*)(cvImg.data),
//                    cvImg.cols,cvImg.rows,
//                    cvImg.cols*cvImg.channels(),
//                    QImage::Format_RGB888);
//    }

//    return qImg;

//}
#endif

bool EbMMManager::InitVideoCapture(int nIndex, int nQuality)
{
    if (m_camera!=0) {
        /// 已经初始过
        return true;
    }
    StopVideoCapture(false);
    m_nVideoQuality = nQuality;
    m_nVideoIndex = nIndex;

    /// 获取视频设备列表
    const int nSize = GetVideoDeviceSize();
    if (nSize==0)
        return false;
    else if (nIndex+1>nSize)
        return false;

    /// for test
//#ifdef Q_OS_MACOS
////    m_videoCapture.open(0);           /// open the default camera
//    if (m_videoCapture.isOpened()) {
//        /// CV_CAP_MODE_RGB  = 1, // RGB24
//        m_videoCapture.set(CV_CAP_PROP_MODE, 1.0);
//        /// 320,180
//        m_videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, 320.0);
//        m_videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 240.0);
//        m_videoCapture.set(CV_CAP_PROP_FPS, 10.0);
////        BRG = 16.0
////        double format = m_videoCapture.get(CV_CAP_PROP_FORMAT);
////        const double rate = m_videoCapture.get(CV_CAP_PROP_FPS);
//        m_videoCapture >> m_cvframe;
//        if (!m_cvframe.empty()) {

//            QImage image((const unsigned char*)(m_cvframe.data),
//                        m_cvframe.cols, m_cvframe.rows,
//                        m_cvframe.cols*m_cvframe.channels(),
//                        QImage::Format_RGB888);

////            QImage image = Mat2QImage(m_cvframe);
//            image.save("/Users/akee/Desktop/opencv_videocapture.png");

//            //                ui->label->setPixmap(QPixmap::fromImage(image));
//            //                timer = new QTimer(this);
//            //                timer->setInterval(1000/rate);   //set timer match with FPS
//            //                connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
//            //                timer->start();
//        }
//        m_videoCapture.release();
//    }
//#endif

    const QCameraInfo cameraInfo = theCameras.at(nIndex);
    m_camera = new QCamera(cameraInfo, this);
//    m_camera->setCaptureMode(QCamera::CaptureViewfinder);
    m_videoSurface = new EbAbstractVideoSurface(this);
    m_videoSurface->setFps(8);
    connect(m_videoSurface, SIGNAL(captureVideoFrame(const uchar*,int,int,int,unsigned long)),
            this, SLOT(onCaptureVideoFrame(const uchar*,int,int,int,unsigned long)) );
    QCameraViewfinderSettings viewfinderSettings = m_camera->viewfinderSettings();
    /// 设置320,240
    viewfinderSettings.setResolution(320, 240);
    m_camera->setViewfinderSettings(viewfinderSettings);
    m_videoSurface->setSource(m_camera, QSize(320, 240));
//    m_camera->setViewfinder(m_videoSurface);
    /// 启动，获取视频格式
    m_camera->start();
//    QList<QVideoFrame::PixelFormat> list = m_camera->supportedViewfinderPixelFormats();
//    for (int i=0; i<list.size(); i++) {
//        QMessageBox::about(0, "formst", QString::number(list.at(i)));
//    }
    while (!m_videoSurface->isStarted()) {
#ifdef WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
        QCoreApplication::processEvents();
    }
//    const QString errorString  = m_camera->errorString();
//    QMessageBox::about(0, "error", errorString);

    /// 获取视频格式
    m_imageFormat = m_videoSurface->imageFormat();
    const QVideoFrame::PixelFormat videoFormat = m_videoSurface->videoFormat();
    switch (videoFormat) {
    case QVideoFrame::Format_UYVY:  /// 已经转换成RGB24
    case QVideoFrame::Format_YUYV:  /// 已经转换成RGB24
    case QVideoFrame::Format_NV12:  /// 已经转换成RGB24
    case QVideoFrame::Format_RGB24: {
        m_ffmpegFormat = AV_PIX_FMT_RGB24;
        break;
    }
    case QVideoFrame::Format_BGRA32: {
        /// BGRA32 can change to RGB32
        m_ffmpegFormat = AV_PIX_FMT_RGBA;
        break;
    }
    case QVideoFrame::Format_ARGB32: {
        m_ffmpegFormat = AV_PIX_FMT_RGB32;   /// ok
//        m_ffmpegFormat = AV_PIX_FMT_ARGB;   /// 颜色有点失真
        break;
    }
    case QVideoFrame::Format_RGB32: {
        m_ffmpegFormat = AV_PIX_FMT_BGRA;
//        m_ffmpegFormat = AV_PIX_FMT_RGBA;
        break;
    }
    case QVideoFrame::Format_YUV420P: {
        m_ffmpegFormat = AV_PIX_FMT_YUV420P;  // I420
        break;
    }
    default: {
        /// 格式未支持
        StopVideoCapture(false);
        return false;
        break;
    }
    }

    if (m_nEnDecodeType==TYPE_XVID) {
        /// ?未支持
    }
    else if(m_nEnDecodeType == TYPE_H264) {
        if (m_pX264Encoder!=0) {
            m_pX264Encoder->close();
            delete m_pX264Encoder;
            m_pX264Encoder=0;
        }

        if (m_nVideoLocalP2PFlag)
            m_pX264Encoder = BuildH264Encoder(m_nVideoQuality+3, m_nDefaultFps, m_ffmpegFormat);
        else if (m_pRDVideoManagerP2POk.get()!=NULL)
            m_pX264Encoder = BuildH264Encoder(m_nVideoQuality+1, m_nDefaultFps, m_ffmpegFormat);
        else
            m_pX264Encoder = BuildH264Encoder(m_nVideoQuality, m_nDefaultFps, m_ffmpegFormat);
        if (m_pX264Encoder==0) {
            /// 视频编码器打开失败
            StopVideoCapture(false);
            return false;
        }
    }
    return true;
}

void EbMMManager::func_FFT_cbs(bigint nId, const char *data, int size, void *userData)
{
    EbMMManager * pMMManager = (EbMMManager*)userData;
    if (pMMManager!=0) {
        pMMManager->func_FFT_cb2(nId, data, size);
    }
}

void EbMMManager::func_FFT_cb2(mycp::bigint nId, const char *data, int size)
{
    const int nVoicePos = this->m_pAudioRoomWndList.AddFFTSampleData(nId, data, size);
    if (nVoicePos>=0) {
        if (m_callback!=0) {
            m_callback(nId, EBMM_CALLBACK_VOICE_VOLUME, nVoicePos, 0, m_cbUserData);
        }
    }
}

bool EbMMManager::InitAudioCapture()
{
    /// 音频捕捉器
    if (m_audioInput!=0) {
        return true;
    }
    QAudioFormat audioFormat;
//    audioFormat.setSampleRate(44100);
    audioFormat.setSampleRate(EB_DEFAULT_SAMPLERATE);
    audioFormat.setChannelCount(EB_DEFAULT_CHANNELS);
//    audioFormat.framesForBytes(1024);
    audioFormat.setSampleSize(16);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    m_audioInput = new QAudioInput(audioFormat, this);
    m_audioInput->setBufferSize(8*1024);    /// **8*1024比6*1024效果好一些
//    QAudioDeviceInfo d = QAudioDeviceInfo::defaultInputDevice();
//    QStringList supportedCodes = d.supportedCodecs();
//    QList<int> supportedSampleRates = d.supportedSampleRates();
//    QList<int> supportedSampleSizes = d.supportedSampleSizes();
    m_audioInputIODevice = new EbAudioInputIODevice(this);
    connect( m_audioInputIODevice, SIGNAL(captureAudioData(const char*,qint64,unsigned long)),
             this, SLOT(onCaptureAudioData(const char*,qint64,unsigned long)) );
    m_audioInputIODevice->open(QIODevice::WriteOnly);
    return true;
}

void EbMMManager::StartVideoCapture(QWidget *hLocalVideoWndParent)
{
    if (hLocalVideoWndParent!=0) {
        m_pVideoRoomWndList.AddSinkWnd(
                    this->GetSourceId(),
                    m_imageFormat,
                    this->m_nVideoWidth,
                    m_nVideoHeight,
                    hLocalVideoWndParent);
        m_pVideoStreamManger->AddUser(this->GetSourceId(), 800, EB_CODEC_TYPE_NOCPMPRESS);
    }

    if (m_camera!=0) {
        m_camera->start();
    }
}

void EbMMManager::StopVideoCapture(bool bDelStream)
{
    if (bDelStream) {
        m_pVideoRoomWndList.DelSinkWnd(this->GetSourceId());
        m_pVideoStreamManger->DelUser(this->GetSourceId());
    }

    if (m_camera!=0) {
        m_camera->stop();
    }
}

void EbMMManager::ExitVideoCapture(bool bDelStream)
{
    StopVideoCapture(bDelStream);
    /// 不需要 delete ，因为 EbMMManager 退出时会自动清除，预防外面不同线程调用，导致 delete 发生异常问题；
    /// *必须放在前面
    if (m_camera!=0) {
        m_camera->stop();
        delete m_camera;
        m_camera = 0;
    }
    if (m_videoSurface!=0) {
        m_videoSurface->stop();
        delete m_videoSurface;
        m_videoSurface = 0;
    }
}

void EbMMManager::StartAudioCapture()
{
    m_pAudioRoomWndList.AddSinkWnd(this->GetSourceId(), QImage::Format_Invalid, 0, 0, NULL);
    m_pAudioRoomWndList.SetPlay(this->GetSourceId(), true);
    m_pAudioStreamManager->AddStream(this->GetSourceId());

    if (m_audioInput!=0) {
        m_audioInput->start(m_audioInputIODevice);
    }
}

void EbMMManager::StopAudioCapture()
{
    if (m_audioInput!=0) {
        m_audioInput->reset();
        m_audioInput->stop();
    }

    m_pAudioRoomWndList.DelSinkWnd(this->GetSourceId());
    m_pAudioStreamManager->DelStream(this->GetSourceId());
}

void EbMMManager::ExitAudioCapture()
{
    StopAudioCapture();
    /// *必须放在前面
    if (m_audioInput!=0) {
        m_audioInput->reset();
        m_audioInput->stop();
        delete m_audioInput;
        m_audioInput = 0;
    }
    if (m_audioInputIODevice!=0) {
        m_audioInputIODevice->close();
        delete m_audioInputIODevice;
        m_audioInputIODevice = 0;
    }
}

int EbMMManager::GetAudioP2PStatus() const
{
    if (this->m_pRDAudioManagerP2POk.get()!=NULL)
        return P2P_OK;
    else if (this->m_pAudioP2PTaskInfo.get()!=NULL)
        return P2P_WAIT;
    return P2P_NONE;
}

int EbMMManager::GetVideoP2PStatus() const
{
    if (this->m_pRDVideoManagerP2POk.get()!=NULL)
        return P2P_OK;
    else if (this->m_pVideoP2PTaskInfo.get()!=NULL)
        return P2P_WAIT;
    return P2P_NONE;
}

//void EbMMManager::customEvent(QEvent *e)
//{
//    if (e->type()==QEvent::User) {
//        initVideoCapture();
//    }
//}

//void EbMMManager::initVideoCapture()
//{
//    if (m_camera!=0) {
//        return;
//    }
//    if (theCameras.empty()) {
//        return;
//    }
//    if (m_nVideoIndex>=theCameras.size()) {
//        m_nVideoIndex = 0;
//    }
//    const QCameraInfo cameraInfo = theCameras.at(m_nVideoIndex);
//    m_camera = new QCamera(cameraInfo, this);

//    m_videoSurface = new EbAbstractVideoSurface(this);
//    m_videoSurface->setFps(8);
//    connect(m_videoSurface, SIGNAL(captureVideoFrame(QVideoFrame,unsigned long)),
//            this, SLOT(onCaptureVideoFrame(QVideoFrame,unsigned long)) );
//    QCameraViewfinderSettings viewfinderSettings = m_camera->viewfinderSettings();
//    /// 设置320,240
//    viewfinderSettings.setResolution(320, 240);
//    m_camera->setViewfinderSettings(viewfinderSettings);
//    m_camera->setViewfinder(m_videoSurface);
//    /// 启动，获取视频格式
//    m_camera->start();
//}

typedef struct tagNetworkCfg
{
    char szIP[18];
#ifndef _QT_MAKE_
    char szNetmask[18];
    char szGateway[18];
    //char szDns1[18];
    //char szDns2[18];
#endif
}NetworkCfg;

bool GetNetworkCfg(std::vector<NetworkCfg>& pOutNetworkCfg,bool bGetEmptyGateway)
{
#ifdef _QT_MAKE_
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            //IPv4地址
            const QString sIp = address.toString();
            if (sIp.contains("127.0.") || sIp.contains("0.0.0.0"))
            {
                continue;
            }
            NetworkCfg cfg;
            strcpy(cfg.szIP, sIp.toStdString().c_str());
            pOutNetworkCfg.push_back(cfg);
        }
    }
#else

//#ifdef _MSC_VER
    //log_printf("Get network config");
    //获取网卡名称 网卡名称，网卡别名
    std::vector<tstring> strAdapterAliasList;
    //tstring strAdapterName;
    HKEY hKey, hSubKey, hNdiIntKey;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        "System\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}",
        0,
        KEY_READ,
        &hKey) != ERROR_SUCCESS)
    {
        return false;
    }
    DWORD dwIndex = 0;
    DWORD dwBufSize = 256;
    DWORD dwDataType;
    char szSubKey[256];
    unsigned char szData[256];

    while(RegEnumKeyEx(hKey, dwIndex++, szSubKey, &dwBufSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
    {
        if(RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
        {
            if(RegOpenKeyEx(hSubKey, "Ndi\\Interfaces", 0, KEY_READ, &hNdiIntKey) == ERROR_SUCCESS)
            {
                dwBufSize = 256;
                if(RegQueryValueEx(hNdiIntKey, "LowerRange", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
                {
                    if(strstr((char*)szData, "ethernet") != NULL)//  判断是不是以太网卡
                    {
                        dwBufSize = 256;
                        if(RegQueryValueEx(hSubKey, "DriverDesc", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
                        {
                            //strAdapterName = (LPCTSTR)szData;
                            dwBufSize = 256;
                            if(RegQueryValueEx(hSubKey, "NetCfgInstanceID", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
                            {
                                strAdapterAliasList.push_back((LPCTSTR)szData);
                                //break;
                            }
                        }
                    }
                }
                RegCloseKey(hNdiIntKey);
            }
            RegCloseKey(hSubKey);
        }

        dwBufSize = 256;
    }  /* end of while */

    RegCloseKey(hKey);
    if (strAdapterAliasList.empty())
    {
        //log_printf("failed to get network config");
        return false;
    }
    for (size_t i=0;i<strAdapterAliasList.size();i++)
    {
        tstring strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
        strKeyName += strAdapterAliasList[i];
        if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            strKeyName.c_str(),
            0,
            KEY_READ,
            &hKey) != ERROR_SUCCESS)
        {
            continue;
        }
        NetworkCfg cfg;
        memset(&cfg,0,sizeof(cfg));
        dwBufSize = 256;
        if(RegQueryValueEx(hKey, "DhcpIPAddress", 0,&dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
            strcpy(cfg.szIP,(LPCTSTR)szData);
        else{
            if(RegQueryValueEx(hKey, "IPAddress", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
                strcpy(cfg.szIP,(LPCTSTR)szData);
        }
        if (strstr(cfg.szIP,"0.0.0.0")!=NULL)
        {
            RegCloseKey(hKey);
            continue;
        }

        dwBufSize = 256;
        if(RegQueryValueEx(hKey, "DhcpSubnetMask", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
            strcpy(cfg.szNetmask,(LPCTSTR)szData);
        else
        {
            if(RegQueryValueEx(hKey, "SubnetMask", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
                strcpy(cfg.szNetmask,(LPCTSTR)szData);
        }

        dwBufSize = 256;
        if(RegQueryValueEx(hKey, "DhcpDefaultGateway", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
            strcpy(cfg.szGateway,(LPCTSTR)szData);
        else
        {
            if(RegQueryValueEx(hKey, "DefaultGateway", 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
                strcpy(cfg.szGateway,(LPCSTR)szData);
        }
        RegCloseKey(hKey);

        if (!bGetEmptyGateway && strlen(cfg.szGateway)==0)
        {
            continue;
        }

        ////获取DNS服务器信息
        //FIXED_INFO *fi = (FIXED_INFO *)GlobalAlloc(GPTR,sizeof( FIXED_INFO));
        //ULONG ulOutBufLen = sizeof(FIXED_INFO);
        //DWORD ret = ::GetNetworkParams(fi, &ulOutBufLen);
        //if(ret != ERROR_SUCCESS)
        //{
        //	GlobalFree(fi);
        //	fi = (FIXED_INFO *) GlobalAlloc( GPTR, ulOutBufLen );
        //	ret = ::GetNetworkParams(fi, &ulOutBufLen);
        //	if(ret != ERROR_SUCCESS)
        //	{
        //		//log_printf("Get Dns server failed");
        //		GlobalFree(fi);
        //		continue;
        //	}
        //}
        //strcpy(cfg.szDns1,fi->DnsServerList.IpAddress.String);
        //IP_ADDR_STRING *pIPAddr = fi->DnsServerList.Next;
        //if(pIPAddr != NULL)
        //	strcpy(cfg.szDns2, pIPAddr->IpAddress.String);
        //GlobalFree(fi);
        pOutNetworkCfg.push_back(cfg);
    }
//#endif

#endif //_QT_MAKE_
    return true;
}
int GetLocalHostIp(std::vector<std::string> & pOutIps)
{
    int count=0;
#ifdef _WIN32
    char host_name[255];
    /// 获取本地主机名称
    if (gethostname(host_name, sizeof(host_name)) == -1) {
        printf("Error when getting local host name.n\n" );
        return 1;
    }
    //printf("Host name is: %s\n", host_name);
    //从主机名数据库中得到对应的“主机”
    struct hostent *phe = gethostbyname(host_name);
    if (phe == 0) {
        printf("Yow! Bad host lookup.");
        return 1;
    }
    //循环得出本地机器所有IP地址
    for (int i = 0; phe->h_addr_list[i] != 0; ++i)
    {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        pOutIps.push_back(inet_ntoa(addr));
        printf("Address %d : %s\n" , i, inet_ntoa(addr));
    }
#else
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock>=0)//!<0
    {
        for(int i=0;i<10;i++)
        {
            memset(&ifr, 0, sizeof(ifr));
            sprintf(ifr.ifr_name,"eth%d",i);
            if(ioctl(sock,SIOCGIFADDR,&ifr)<0) break;
            ::memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
            pOutIps.push_back((const char*)inet_ntoa(sin.sin_addr));
            printf("Address %d : %s\n", i, (const char*)inet_ntoa(sin.sin_addr));

            memset(&ifr, 0, sizeof(ifr));
            sprintf(ifr.ifr_name,"eth%d:1",i);
            if(ioctl(sock,SIOCGIFADDR,&ifr)<0) continue;
            ::memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
            pOutIps.push_back((const char*)inet_ntoa(sin.sin_addr));
            printf("Address %d : %s\n", i, (const char*)inet_ntoa(sin.sin_addr));
        }
        close(sock);
    }else
    {
        printf("socket error\n");
    }
#endif
    return count;
}

void EbMMManager::GetLocalHostInfo()
{
    if (m_pLocalHostIp.empty())
    {
        std::vector<tagNetworkCfg> pNetworkCfg;
        GetNetworkCfg(pNetworkCfg,false);	// **有可能取到二个以上，其中有一个是不对的
        if (pNetworkCfg.size()>1)
        {
            // 做交叉比较，去掉没用的IP地址，取出真实IP地址；
            CLockMap<std::string,bool> pIpList;
            for (size_t i=0;i<pNetworkCfg.size();i++)
                pIpList.insert(pNetworkCfg[i].szIP,true);
            std::vector<std::string> pLocalHostIp;
            GetLocalHostIp(pLocalHostIp);
            for (size_t i=0;i<pLocalHostIp.size();i++)
            {
                const tstring sIp = pLocalHostIp[i];
                if (pIpList.exist(sIp))
                {
                    m_pLocalHostIp.push_back(sIp);
                    break;
                }
            }
            if (m_pLocalHostIp.empty())
                GetLocalHostIp(m_pLocalHostIp);
        }else if (pNetworkCfg.size()==1)
        {
            m_pLocalHostIp.push_back(pNetworkCfg[0].szIP);
        }else
        {
            GetLocalHostIp(m_pLocalHostIp);
        }
    }
}

void EbMMManager::onLocalAudioData(const unsigned char *pData, unsigned int nSize, unsigned int nTimestamp)
{
    if (m_pRtpAudioManager.get()==NULL) return;
    try
    {
        if (m_pRDAudioManagerP2POk.get()!=NULL)
            m_pRDAudioManagerP2POk->SendRtpData(m_nRoomId,pData,nSize,nTimestamp,SOTP_RTP_NAK_DATA_AUDIO,SOTP_RTP_NAK_REQUEST_1);
        else if (m_pAudioP2PTaskInfo.get()==NULL)	/// m_pAudioP2PTaskInfo.get()!=NULL：表示正在做P2P尝试；
            m_pRtpAudioManager->SendRtpData(m_nRoomId,pData,nSize,nTimestamp,SOTP_RTP_NAK_DATA_AUDIO,SOTP_RTP_NAK_REQUEST_1);
    }catch(std::exception&)
    {}catch(...)
    {}
}

void EbMMManager::onLocalVideoData(const unsigned char *pData, unsigned int nSize, bool bKeyFrame, unsigned int nTimestamp)
{
    if (m_pRtpVideoManager.get()==NULL) return;
    const mycp::uint8 nDataType = bKeyFrame?SOTP_RTP_NAK_DATA_VIDEO_I:SOTP_RTP_NAK_DATA_VIDEO;
    try
    {
        if (m_pRDVideoManagerP2POk.get()!=NULL)
            m_pRDVideoManagerP2POk->SendRtpData(m_nRoomId,pData,nSize,nTimestamp,nDataType,SOTP_RTP_NAK_REQUEST_1);
        else if (m_pVideoP2PTaskInfo.get()==NULL)	/// m_pVideoP2PTaskInfo.get()!=NULL：表示正在做P2P尝试；
            m_pRtpVideoManager->SendRtpData(m_nRoomId,pData,nSize,nTimestamp,nDataType,SOTP_RTP_NAK_REQUEST_1);
    }catch(std::exception&)
    {}catch(...)
    {}
}

void EbMMManager::OnRtpFrame(bigint nSrcId, const CSotpRtpFrame::pointer &pRtpFrame, uint16 nLostCount, uint32 nUserData, CEBRtpManager *pLCOwner)
{
    if (pRtpFrame->m_pRtpHead.m_nRoomId!=this->GetRoomId()) return;

    if (nUserData==1 && m_pAudioStreamManager!=0) {
        /// audio
        m_pAudioStreamManager->WritestreamData(nSrcId,(unsigned char*)pRtpFrame->m_pPayload,pRtpFrame->m_pRtpHead.m_nTotleLength,pRtpFrame->m_nFirstSeq,pRtpFrame->m_pRtpHead.m_nTimestamp);
    }
    else if (nUserData==2 && m_pVideoStreamManger!=0) {
        /// video
        const int pt = pRtpFrame->m_pRtpHead.m_nDataType==SOTP_RTP_NAK_DATA_VIDEO_I?11:1;
        m_pVideoStreamManger->Write(nSrcId, pRtpFrame->m_pRtpHead.m_nTotleLength,
                                    (unsigned char*)pRtpFrame->m_pPayload,
                                    pRtpFrame->m_nFirstSeq, pRtpFrame->m_pRtpHead.m_nTimestamp,
                                    EB_RTP_PAYLOAD_TYPE_H264,pt);
    }
}

void EbMMManager::OnRDOnResponse(const CPOPSotpRequestInfo::pointer &pReqeustInfo, const CPOPSotpResponseInfo::pointer &pResponseInfo, const CEBRtpManager *pCMOwner)
{
    const int nResultValue = pResponseInfo->GetResultValue();
    if (nResultValue==EB_STATE_OK)
    {
        if (pCMOwner==this->m_pRtpAudioManager.get())
            m_pRtpAudioManager->RegisterSource(this->GetRoomId(),this->GetParam());
        else if (pCMOwner==this->m_pRtpVideoManager.get())
            m_pRtpVideoManager->RegisterSource(this->GetRoomId(),this->GetParam());
    }
}

bool EbMMManager::DoCheckP2PState(const cgcSmartString &sLocalAddress, const cgcSmartString &sRemoteAddress, const EbRtpSinkInfo::pointer &pDestSink)
{
    if (pDestSink->m_bLocalP2POk || pDestSink->m_bRemoteP2POk)
    {
        if (pDestSink->m_pRtpManager1.get()!=NULL)
        {
            pDestSink->m_pRtpManager1->SetDestAddress(1,pDestSink->GetLocalAddress());
            pDestSink->m_pRtpManager1->SendP2PTest(3);
        }else if (pDestSink->m_pRtpManager2.get()!=NULL)
        {
            pDestSink->m_pRtpManager2->SetDestAddress(1,pDestSink->GetRemoteAddress());
            pDestSink->m_pRtpManager2->SendP2PTest(3);
        }
        return true;
    }
    if (!sLocalAddress.empty())
        pDestSink->SetLocalAddress(sLocalAddress);		// 这个内网地址，可以打通P2P；connect&send也可以打通
    if (!sRemoteAddress.empty())
        pDestSink->SetRemoteAddress(sRemoteAddress);		// 这个外网地址，可以打通P2P；（大部分路由器支持）
    return false;
}

bool EbMMManager::DoP2PTask(bool bAudio, bigint sCallId, bigint sChatId, bigint nFromUserId, const cgcSmartString &sLocalAddress, const cgcSmartString &sRemoteAddress, bool &pOutP2POk)
{
    //const int nP2PData = pP2PInfo.m_nP2PData;
    EbRtpSinkInfo::pointer pSinkWnd = bAudio?this->m_pAudioRoomWndList.GetSinkWnd(nFromUserId):this->m_pVideoRoomWndList.GetSinkWnd(nFromUserId);
    if (pSinkWnd.get()==NULL) return false;

    if (DoCheckP2PState(sLocalAddress,sRemoteAddress,pSinkWnd))
    {
        pOutP2POk = true;
        return false;	// 已经打通P2P
    }

    // LOCAL P2P
    if (!pSinkWnd->GetLocalAddress().empty() && pSinkWnd->m_pRtpManager1.get()!=NULL)
    {
        CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRtpManager1;	// 使用这个，不会导致OnP2POk清空异常
        pRDManager->SetDestAddress(1,pSinkWnd->GetLocalAddress());
        pRDManager->SendP2PTry(sCallId,sChatId,this->GetSourceId(),0,1);	// 1=内网地址
    }

    // REMOTE P2P
    if (!pSinkWnd->GetRemoteAddress().empty() && pSinkWnd->m_pRtpManager2.get()!=NULL)
    {
        CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRtpManager2;	// 使用这个，不会导致OnP2POk清空异常
        pRDManager->SetDestAddress(1,pSinkWnd->GetRemoteAddress());
        pRDManager->SendP2PTry(sCallId,sChatId,this->GetSourceId(),0,2);	// 2=NAT公网地址
    }
    return true;
}

void EbMMManager::OnP2POk(const CPOPSotpRequestInfo::pointer &pReqeustInfo, const CPOPSotpResponseInfo::pointer &pResponseInfo, const CEBRtpManager *pCMOwner)
{
    // 已经打通P2P通道
    const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
    const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    const mycp::bigint sCallId = pResponseInfo->m_pResponseList.getParameterValue("call-id", (mycp::bigint)0);
    const int nP2PData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
    //CCrP2PInfo pP2PInfo(m_nParam,sCallId,sChatId);
    //pP2PInfo.m_sFromAccount = sFromAccount;
    //pP2PInfo.m_nP2PData = nP2PData;
    //if (m_callback)
    //	m_callback->OnP2POk(pP2PInfo);
    //if (m_receiver)
    //	::SendMessage(m_receiver, CR_WM_P2P_OK, (WPARAM)&pP2PInfo, 0);

    //// 已经打通P2P通道；
    //const mycp::bigint sCallId = pP2PInfo.GetCallId();
    //const mycp::bigint sChatId = pP2PInfo.GetChatId();
    //const int nP2PData = pP2PInfo.m_nP2PData;	// 1：是内网；2：公网

    if (this->IsGroup()) {
        return;
    }

    const bool bAudio = pCMOwner->GetParameter()==1?true:false;
    EbRtpSinkInfo::pointer pSinkWnd = bAudio?this->m_pAudioRoomWndList.GetSinkWnd(sFromAccount):this->m_pVideoRoomWndList.GetSinkWnd(sFromAccount);
    if (pSinkWnd.get()==0) {
        return;
    }

    CEBRtpManager::pointer pRtpManagerP2POk;
    if (nP2PData==1 && pSinkWnd->m_pRtpManager1.get()!=0 && !pSinkWnd->m_bLocalP2POk) {
        /// 找到对方；
        pSinkWnd->m_bLocalP2POk = true;
        pSinkWnd->m_pRtpManager1->SetDestAddress(1,pSinkWnd->GetLocalAddress());

        pRtpManagerP2POk = pSinkWnd->m_pRtpManager1;
        pRtpManagerP2POk->SetTranSpeedLimit(256);
        if (bAudio) {
            m_pRDAudioManagerP2POk = pRtpManagerP2POk;
            m_pRDAudioManagerP2POk->SetRtpCallbackData(1);
        }
        else {
            m_pRDVideoManagerP2POk = pRtpManagerP2POk;
            m_pRDVideoManagerP2POk->SetRtpCallbackData(2);
            m_nVideoLocalP2PFlag = true;

            EbFfmpegProxy *pX264EncoderNew = BuildH264Encoder(m_nVideoQuality+3, m_nDefaultFps, m_ffmpegFormat);
            if (pX264EncoderNew!=0) {
                EbFfmpegProxy * pX264EncoderOld = m_pX264Encoder;
                m_pX264Encoder = pX264EncoderNew;
                if (pX264EncoderOld!=0) {
#ifdef WIN32
                    Sleep(100);
#else
                    usleep(100000);
#endif
                    pX264EncoderOld->close();
                    delete pX264EncoderOld;
                    pX264EncoderOld=0;
                }
            }
        }

        /// 清空外网P2P，否则会有问题
        pSinkWnd->m_bRemoteP2POk = false;
        if (pSinkWnd->m_pRtpManager2.get()!=0) {
            m_pClearChatRoom.add(pSinkWnd->m_pRtpManager2);
            pSinkWnd->m_pRtpManager2.reset();
        }
    }
    else if (nP2PData==2 && pSinkWnd->m_pRtpManager2.get()!=0 &&
             !pSinkWnd->m_bRemoteP2POk && !pSinkWnd->m_bLocalP2POk) {
        /// 找到对方；
        pSinkWnd->m_bRemoteP2POk = true;
        pSinkWnd->m_pRtpManager2->SetDestAddress(1,pSinkWnd->GetRemoteAddress());

        pRtpManagerP2POk = pSinkWnd->m_pRtpManager2;
        if (bAudio) {
            m_pRDAudioManagerP2POk = pRtpManagerP2POk;
            m_pRDAudioManagerP2POk->SetRtpCallbackData(1);
        }
        else {
            m_pRDVideoManagerP2POk = pRtpManagerP2POk;
            m_pRDVideoManagerP2POk->SetRtpCallbackData(2);

            m_nVideoLocalP2PFlag = false;
            EbFfmpegProxy * pX264EncoderNew = BuildH264Encoder(m_nVideoQuality+1, m_nDefaultFps, m_ffmpegFormat);
            if (pX264EncoderNew!=0) {
                EbFfmpegProxy * pX264EncoderOld = m_pX264Encoder;
                m_pX264Encoder = pX264EncoderNew;
                if (pX264EncoderOld!=0) {
#ifdef WIN32
                    Sleep(100);
#else
                    usleep(100000);
#endif
                    pX264EncoderOld->close();
                    delete pX264EncoderOld;
                    pX264EncoderOld=0;
                }
            }
        }

        /// 清空内网P2P，否则会有问题
        pSinkWnd->m_bLocalP2POk = false;
        if (pSinkWnd->m_pRtpManager1.get()!=0) {
            m_pClearChatRoom.add(pSinkWnd->m_pRtpManager1);
            pSinkWnd->m_pRtpManager1.reset();
        }
    }
    if (pRtpManagerP2POk.get()!=0) {
        pRtpManagerP2POk->SetSourceId(this->GetSourceId());
        pRtpManagerP2POk->RegisterSource(this->GetRoomId(), this->GetParam());
        pRtpManagerP2POk->RegisterSink(this->GetRoomId(), sFromAccount);
    }
    ///*必须放后面
    if (bAudio)
        m_pAudioP2PTaskInfo.reset();
    else
        m_pVideoP2PTaskInfo.reset();
}

void EbMMManager::OnFCMP2PRequest(const CPOPSotpRequestInfo::pointer &pReqeustInfo, const CPOPSotpResponseInfo::pointer &pResponseInfo, const CEBRtpManager *pCMOwner)
{
    const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
    //const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    const tstring sLocalIp = pResponseInfo->m_pResponseList.getParameterValue("local-ip", "");
    const tstring sRemoteIp = pResponseInfo->m_pResponseList.getParameterValue("remote-ip", "");
    const int nRequestData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
    //CCrP2PInfo pP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId());
    //pP2PInfo.m_sFromAccount = sFromAccount;
    //pP2PInfo.m_sFromLocalAddress = sLocalIp;
    //pP2PInfo.m_sFromRemoteAddress = sRemoteIp;
    //pP2PInfo.m_nP2PData = nRequestData;
    //if (m_callback)
    //	m_callback->OnP2PRequest(pP2PInfo);
    //if (m_receiver)
    //	::SendMessage(m_receiver, CR_WM_P2P_REQUEST, (WPARAM)&pP2PInfo, 0);

    // 服务端转发P2P请求；
    const bool bAudio = pCMOwner->GetParameter()==1?true:false;
    const mycp::bigint sCallId = this->GetRoomId();
    const mycp::bigint sChatId = this->GetRoomId();
    const int nP2PData = nRequestData;	// nP2PData=1：表示对方离线超时，重试
    if (this->IsGroup()) return;

    EbRtpSinkInfo::pointer pSinkWnd = bAudio?this->m_pAudioRoomWndList.GetSinkWnd(sFromAccount):this->m_pVideoRoomWndList.GetSinkWnd(sFromAccount);
    if (pSinkWnd.get()==NULL) return;

    // 找到对方；
    if (nP2PData==1)	// 对方发超时离线，请求重连
    {
        pSinkWnd->m_bLocalP2POk = false;
        pSinkWnd->m_bRemoteP2POk = false;
        //MessageBox(NULL,"nP2PData==1","",MB_OK);
    }
    DoCheckP2PState(sLocalIp, sRemoteIp, pSinkWnd);			// **已经打通也不返回，可以再处理一次；
    //if (DoCheckP2PState(sLocalIp, sRemoteIp, pSinkWnd))	// 已经打通，直接返回
    //{
    //	return;
    //}

    // LOCAL P2P，这个可以打通，connect&send也可以打通
    const CEBRtpManager::pointer& pDoRtpManager = bAudio?m_pRtpAudioManager:m_pRtpVideoManager;
    if (!sLocalIp.empty())
    {
        CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRtpManager1;
        if (pRDManager.get()==NULL)
        {
            pRDManager = CEBRtpManager::create(this);
            pRDManager->Start(pDoRtpManager->GetAddress(),bAudio?MMAS_APP_NAME:MMVS_APP_NAME,0,0,0);
            pRDManager->SetParameter(pDoRtpManager->GetParameter());
            pSinkWnd->m_pRtpManager1 = pRDManager;

            GetLocalHostInfo();
            if (!m_pLocalHostIp.empty())
            {
                // *这边也主动请求一次；
                char lpszLocalIp[64];
                sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pRDManager->GetLocalPort());
                pRDManager->SetDestAddress(0,pDoRtpManager->GetAddress().address());
                pRDManager->SendP2PRequest(this->GetSourceId(),this->GetRoomId(),sFromAccount,lpszLocalIp,0);
            }
        }
        if (!m_pLocalHostIp.empty())
        {
            CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
            pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", sFromAccount));

            char lpszLocalIp[64];
            sprintf(lpszLocalIp,"%s:%d",m_pLocalHostIp[0].c_str(),pRDManager->GetLocalPort());
            pRDManager->SetDestAddress(0,pDoRtpManager->GetAddress().address());
            pRDManager->SendP2PResponse(this->GetSourceId(),this->GetRoomId(),sFromAccount,lpszLocalIp,0,pRequestInfo);
        }
    }

    // REMOTE P2P
    {
        CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRtpManager2;
        if (pRDManager.get()==NULL)
        {
            pRDManager = CEBRtpManager::create(this);
            pRDManager->Start(pDoRtpManager->GetAddress(),bAudio?MMAS_APP_NAME:MMVS_APP_NAME,0,0,0);
            pRDManager->SetParameter(pDoRtpManager->GetParameter());
            pSinkWnd->m_pRtpManager2 = pRDManager;

            // 这里主动请求一次；
            pRDManager->SetDestAddress(0,pDoRtpManager->GetAddress().address());
            pRDManager->SendP2PRequest(this->GetSourceId(),this->GetRoomId(),sFromAccount,"",0);
        }
        // 响应
        pRDManager->SetDestAddress(0,pDoRtpManager->GetAddress().address());
            CPOPSotpRequestInfo::pointer pRequestInfo = CPOPSotpRequestInfo::create(0);
            pRequestInfo->m_pRequestList.SetParameter(CGC_PARAMETER("to-uid", sFromAccount));
        pRDManager->SendP2PResponse(this->GetSourceId(),this->GetRoomId(),sFromAccount,"",0,pRequestInfo);
    }
}

void EbMMManager::OnFCMP2PResponse(const CPOPSotpRequestInfo::pointer &pReqeustInfo, const CPOPSotpResponseInfo::pointer &pResponseInfo, const CEBRtpManager *pCMOwner)
{
    const mycp::bigint sFromAccount = pResponseInfo->m_pResponseList.getParameterValue("from-uid", (mycp::bigint)0);
    //const mycp::bigint sChatId = pResponseInfo->m_pResponseList.getParameterValue("chat-id", (mycp::bigint)0);
    const tstring sLocalIp = pResponseInfo->m_pResponseList.getParameterValue("local-ip", "");
    const tstring sRemoteIp = pResponseInfo->m_pResponseList.getParameterValue("remote-ip", "");
    const int nRequestData = pResponseInfo->m_pResponseList.getParameterValue("p2p-data", (int)0);
    //CCrP2PInfo pP2PInfo(m_nParam,m_chatManager->GetChatInfo()->GetCallId(),m_chatManager->GetChatInfo()->GetChatId());
    //pP2PInfo.m_sFromAccount = sFromAccount;
    //pP2PInfo.m_sFromLocalAddress = sLocalIp;
    //pP2PInfo.m_sFromRemoteAddress = sRemoteIp;
    //pP2PInfo.m_nP2PData = nRequestData;
    //if (m_callback)
    //	m_callback->OnP2PResponse(pP2PInfo,0);
    //if (m_receiver)
    //	::SendMessage(m_receiver, CR_WM_P2P_RESPONSE, (WPARAM)&pP2PInfo, 0);

    // nFlag==0服务端转发P2P请求；
    const bool bAudio = pCMOwner->GetParameter()==1?true:false;
    const mycp::bigint sCallId = this->GetRoomId();
    const mycp::bigint sChatId = this->GetRoomId();
    const int nP2PData = nRequestData;
    bool bP2POk = false;
    if (DoP2PTask(bAudio,sCallId,sChatId,sFromAccount,sLocalIp,sRemoteIp,bP2POk))
    {
        // 未打通P2P，或其他错误，需要保存做超时检查
        EbP2PTaskInfo::pointer pP2PTaskInfo = bAudio?m_pAudioP2PTaskInfo:m_pVideoP2PTaskInfo;
        if (pP2PTaskInfo.get()==NULL)
        {
            pP2PTaskInfo = EbP2PTaskInfo::create();
            if (bAudio)
                m_pAudioP2PTaskInfo = pP2PTaskInfo;
            else
                m_pVideoP2PTaskInfo = pP2PTaskInfo;
        }
        pP2PTaskInfo->m_nCallId = sCallId;
        pP2PTaskInfo->m_nFromUserId = sFromAccount;
        pP2PTaskInfo->m_sFromLocalIp = sLocalIp;
        pP2PTaskInfo->m_sFromRemoteIp = sRemoteIp;
        pP2PTaskInfo->m_tP2PTry = time(0);
        pP2PTaskInfo->m_bP2PResponse = true;
    }else
    {
        // 已经打通P2P
        if (bAudio)
            m_pAudioP2PTaskInfo.reset();
        else
            m_pVideoP2PTaskInfo.reset();
    }
}

void EbMMManager::OnCMP2PResponseRes(const CPOPSotpRequestInfo::pointer &pRequestInfo, const CPOPSotpResponseInfo::pointer &pResponseInfo, const CEBRtpManager *pCMOwner)
{
    if (pRequestInfo.get()==NULL) return;
    const mycp::bigint sFromAccount = pRequestInfo->m_pRequestList.getParameterValue("to-uid", (mycp::bigint)0);

    if (this->IsGroup()) return;
    // nFlag==1:响应返回；
    const bool bAudio = pCMOwner->GetParameter()==1?true:false;
    const mycp::bigint sCallId = this->GetRoomId();
    const mycp::bigint sChatId = this->GetRoomId();

    EbRtpSinkInfo::pointer pSinkWnd = bAudio?this->m_pAudioRoomWndList.GetSinkWnd(sFromAccount):this->m_pVideoRoomWndList.GetSinkWnd(sFromAccount);
    if (pSinkWnd.get()==NULL) return;

    if (!pSinkWnd->GetLocalAddress().empty())
    {
        CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRtpManager1;	// 使用这个，不会导致OnP2POk清空异常
        if (pRDManager.get()!=NULL)
        {
            pRDManager->SetDestAddress(1,pSinkWnd->GetLocalAddress());
            pRDManager->SendP2PTry(sCallId,sChatId,this->GetSourceId(),0,1);	// 1=内网地址
        }
    }
    if (!pSinkWnd->GetRemoteAddress().empty())
    {
        CEBRtpManager::pointer pRDManager = pSinkWnd->m_pRtpManager2;	// 使用这个，不会导致OnP2POk清空异常
        if (pRDManager.get()!=NULL)
        {
            pRDManager->SetDestAddress(1,pSinkWnd->GetRemoteAddress());
            pRDManager->SendP2PTry(sCallId,sChatId,this->GetSourceId(),0,2);	// 2=NAT公网地址
        }
    }
}

void EbMMManager::OnProcessTimer(const CEBRtpManager *pCMOwner)
{
    const time_t tNow = time(0);
    const bool bAudio = pCMOwner->GetParameter()==1?true:false;
    EbP2PTaskInfo::pointer pP2PTaskInfo = bAudio?m_pAudioP2PTaskInfo:m_pVideoP2PTaskInfo;
    if (pP2PTaskInfo.get()!=NULL)
    {
        // 检查P2P穿透任务；
        const mycp::bigint sCallId = pP2PTaskInfo->m_nCallId;
        const mycp::bigint sChatId = pP2PTaskInfo->m_nCallId;
        const mycp::bigint sFromUserId = pP2PTaskInfo->m_nFromUserId;
        bool bP2POk = false;
        if (pP2PTaskInfo->m_bP2PResponse && !DoP2PTask(bAudio,sCallId,sChatId,sFromUserId,pP2PTaskInfo->m_sFromLocalIp,pP2PTaskInfo->m_sFromRemoteIp,bP2POk))
        {
            // 打通成功，或其他失败
            if (bAudio)
            {
                m_pRtpAudioManager->RegisterSink(this->GetRoomId(), sFromUserId);
                m_pAudioP2PTaskInfo.reset();
            }else
            {
                m_pRtpVideoManager->RegisterSink(this->GetRoomId(), sFromUserId);
                m_pVideoP2PTaskInfo.reset();
            }
        }else if (pP2PTaskInfo->m_tP2PTry>0 && (tNow-pP2PTaskInfo->m_tP2PTry)>=10)
        {
            // 超过8秒，当超时处理；

            // 清空P2P连接资源
            EbRtpSinkInfo::pointer pSinkWnd = bAudio?this->m_pAudioRoomWndList.GetSinkWnd(sFromUserId):this->m_pVideoRoomWndList.GetSinkWnd(sFromUserId);
            if (pSinkWnd.get()!=NULL)
            {
                //MessageBox(NULL,"OnProcessTimer","",MB_OK);
                pSinkWnd->m_bLocalP2POk = false;
                if (pSinkWnd->m_pRtpManager1.get()!=NULL)
                {
                    m_pClearChatRoom.add(pSinkWnd->m_pRtpManager1);
                    pSinkWnd->m_pRtpManager1.reset();
                }
                pSinkWnd->m_bRemoteP2POk = false;
                if (pSinkWnd->m_pRtpManager2.get()!=NULL)
                {
                    m_pClearChatRoom.add(pSinkWnd->m_pRtpManager2);
                    pSinkWnd->m_pRtpManager2.reset();
                }
            }
            if (bAudio)
            {
                m_pRtpAudioManager->RegisterSink(this->GetRoomId(), sFromUserId);
                m_pAudioP2PTaskInfo.reset();
            }else
            {
                m_pRtpVideoManager->RegisterSink(this->GetRoomId(), sFromUserId);
                m_pVideoP2PTaskInfo.reset();
            }
        }
    }

    static unsigned int theIndex = 0;
    theIndex++;
    if ((theIndex%5)==4)
    {
        CEBRtpManager::pointer pChatRoom;
        if (m_pClearChatRoom.front(pChatRoom))
        {
            //pChatRoom->Stop();
        }
    }
}

EbFfmpegProxy *EbMMManager::BuildH264Encoder(int nVideoQuality, int nFPS, AVPixelFormat nVideoType)
{
    EbFfmpegProxy * result = new EbFfmpegProxy();
    result->setVideoSize(320, 240);
    result->setKeyFrameInterval(10);
    result->setBitRate(128000);
    result->setFormat(nVideoType);
    if (!result->open(AV_CODEC_ID_H264, true)) {
        delete result;
        return 0;
    }
    return result;

//    CVideoEnDecodeItem Item;
//    //Item.m_stSize = VIDEO_SIZE_640X480;
//    Item.m_stSize = VIDEO_SIZE_320X240;
//    Item.m_nQuality = max(8,(38-nVideoQuality*2));	// 38（节省带宽）-18（高清），默认
//    Item.m_nKeyInterval = 10;	// 5; max=min*3
//    Item.m_nFps = nFPS;
//    Item.m_nBitRate = 128000;
//    CVideoEncoderX264* pX264Encoder = new CVideoEncoderX264();
//    if (!pX264Encoder->Connect(0, Item , nVideoType))
//    {
//        delete pX264Encoder;
//        pX264Encoder = 0;
//        return 0;
//    }
//    return pX264Encoder;
}

} ///namespace entboost
