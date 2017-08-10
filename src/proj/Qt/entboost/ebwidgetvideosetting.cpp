#include "ebwidgetvideosetting.h"
#include "ebmessagebox.h"

EbWidgetVideoSetting::EbWidgetVideoSetting(QWidget *parent) : QWidget(parent)
{
    const int nLocalVideoIndex = (int)theApp->localVideoIndex();
    m_comboBoxVideoDevices = new QComboBox(this);
    connect( m_comboBoxVideoDevices, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChangedComboBoxVideoDevices(int)) );
    std::vector<QString> pVideoDevices;
    Cebmm::GetVideoDevices(pVideoDevices);
    for (size_t i=0; i<pVideoDevices.size(); i++) {
        m_comboBoxVideoDevices->addItem(pVideoDevices[i]);
    }
    if (nLocalVideoIndex>0 && pVideoDevices.size()>nLocalVideoIndex) {
        m_comboBoxVideoDevices->setCurrentIndex(nLocalVideoIndex);
    }
    m_buttonOpen = new QPushButton(this);
    m_buttonOpen->setObjectName("RequestAddContact");
    connect( m_buttonOpen, SIGNAL(clicked()), this, SLOT(onClickedButtonOpen()) );

    const int const_y	= 18;
    const int const_x1	= 45-28;	/// 28是父窗口左边间隔
    const int const_x2	= 272-28;
    int x = const_x1;
    int y = const_y;
    m_comboBoxVideoDevices->setGeometry(x, y, 210, 22);
    x = const_x2;
    m_buttonOpen->setGeometry(x, y, 60, 25);

    updateLocaleInfo();
}

EbWidgetVideoSetting::~EbWidgetVideoSetting()
{
    closeVideo(true);
}

void EbWidgetVideoSetting::closeVideo(bool fromDelete)
{
    /// * 这个必须放前面
    if (m_videoRoom.get()!=0) {
        m_videoRoom.reset();
        if (!fromDelete) {
            updateLocaleInfo();
        }
    }
    m_videoWindow.reset();
}

void EbWidgetVideoSetting::updateLocaleInfo()
{
    m_comboBoxVideoDevices->setToolTip( theLocales.getLocalText("my-center-dialog.video-setting-widget.combo-video-devices.tooltip","") );
    if (m_videoRoom.get()==0) {
        m_buttonOpen->setText( theLocales.getLocalText("my-center-dialog.video-setting-widget.button-open.text","Open") );
        m_buttonOpen->setToolTip( theLocales.getLocalText("my-center-dialog.video-setting-widget.button-open.tooltip","") );
    }
    else  {
        m_buttonOpen->setText( theLocales.getLocalText("my-center-dialog.video-setting-widget.button-close.text","Close") );
        m_buttonOpen->setToolTip( theLocales.getLocalText("my-center-dialog.video-setting-widget.button-close.tooltip","") );
    }
}

void EbWidgetVideoSetting::onClickedButtonOpen()
{
    if (m_videoWindow.get()==0) {

        if (m_comboBoxVideoDevices->count()==0) {
            /// 本地找不到视频设备：\r\n请检查设备后重试一次！
            const QString text = theLocales.getLocalText("message-show.video-devices-empty-tip", "Local video devices empty");
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            return;
        }

        m_videoRoom = Cebmm::create();
        m_videoRoom->Start("", "", 0);
        m_videoRoom->Register(0, theApp->logonUserId(), 0, false);
        m_videoRoom->SetLocalAudioSend(false);
        m_videoRoom->SetLocalVideoSend(false);

        /// 打开本地视频
        const int nvideoindex = m_comboBoxVideoDevices->currentIndex();
        if (!m_videoRoom->InitVideoCapture(nvideoindex, 10)) {
            m_videoRoom.reset();
            /// 打开视频摄像头失败：\r\n请检查摄像头是否被占用或其他硬件故障！
            const QString text = theLocales.getLocalText("message-show.open-video-device-error", "Open local video device error");
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            return;
        }
        if (!m_videoRoom->InitAudioCapture()) {
            /// 打开麦克风失败：\r\n请检查麦克风是否被占用或其他硬件故障！
            const QString text = theLocales.getLocalText("message-show.open-mic-device-error", "Open local mic device error");
            EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        }

        const QRect &rect = m_comboBoxVideoDevices->geometry();
        m_videoWindow = EbWidgetVideoWindow::create(theApp->logonUserId(), this);
        m_videoRoom->SetCallBack(onFPMMCallBack, m_videoWindow.get());
        m_videoWindow->setGeometry(rect.x(), rect.y()+28, const_video_width, const_video_height);
        m_videoWindow->setVisible(true);

        m_videoRoom->StartVideoCapture(m_videoWindow.get());
        m_videoRoom->StartAudioCapture();
        m_videoRoom->StartVideoGraph(theApp->logonUserId());
//        m_videoRoom->SetUserData(theApp->logonUserId(), (ULONG)m_pMyVideo);
        m_comboBoxVideoDevices->setEnabled(false);
    }
    else {
        /// 关闭本地视频
        closeVideo();
        m_comboBoxVideoDevices->setEnabled(true);
    }
    updateLocaleInfo();
}

void EbWidgetVideoSetting::onCurrentIndexChangedComboBoxVideoDevices(int index)
{
    theApp->setLocalVideoIndex(index);
}

void EbWidgetVideoSetting::onFPMMCallBack(mycp::bigint nUserId, int nCallBackType, unsigned long lParam, unsigned int wParam, void *pUserData)
{
    EbWidgetVideoWindow * videoWindow = (EbWidgetVideoWindow*)pUserData;
    if (videoWindow!=0) {
        videoWindow->onFPMMCallBack(nUserId, nCallBackType, lParam, wParam);
    }

}
