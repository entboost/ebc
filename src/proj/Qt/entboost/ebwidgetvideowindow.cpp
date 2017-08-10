#include "ebwidgetvideowindow.h"
#include "ebmmhandle.h"
using namespace entboost;
#include "ebwidgetvoicebar.h"

EbWidgetVideoWindow::EbWidgetVideoWindow(mycp::bigint userId, QWidget *parent) : QWidget(parent)
  , m_userId(userId)
  , m_callId(0)
{
    m_voiceBar = new EbWidgetVoiceBar(this);
    m_voiceBar->setVisible(true);
    m_voiceBar->raise();

}

EbWidgetVideoWindow::~EbWidgetVideoWindow()
{

}

EbWidgetVideoWindow::pointer EbWidgetVideoWindow::create(mycp::bigint userId, QWidget *parent)
{
    return EbWidgetVideoWindow::pointer(new EbWidgetVideoWindow(userId, parent));
}

void EbWidgetVideoWindow::onFPMMCallBack(mycp::bigint nUserId, int nCallBackType, unsigned long lParam, unsigned int wParam)
{
    switch (nCallBackType) {
    case EBMM_CALLBACK_FIRST_VIDEO_FPS:
        break;
    case EBMM_CALLBACK_VOICE_VOLUME:
        m_voiceBar->setValue(lParam);
        break;
    default:
        break;
    }

}

void EbWidgetVideoWindow::resizeEvent(QResizeEvent *e)
{
    const long const_VoiceBar_Width = 50;
    const long const_VoiceBar_Height = 40;
    m_voiceBar->setGeometry(6, height()-6-const_VoiceBar_Height, const_VoiceBar_Width, const_VoiceBar_Height);
    QWidget::resizeEvent(e);
}
