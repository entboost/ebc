#ifndef EBWIDGETVIDEOWINDOW_H
#define EBWIDGETVIDEOWINDOW_H

#include <QWidget>
#include <CGCBase/cgcobject.h>

const int const_video_width = 320;
const int const_video_height = 240;

class EbWidgetVoiceBar;

/// 视频窗口
class EbWidgetVideoWindow : public QWidget
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbWidgetVideoWindow> pointer;
    explicit EbWidgetVideoWindow(mycp::bigint userId, QWidget *parent = nullptr);
    virtual ~EbWidgetVideoWindow(void);
    static EbWidgetVideoWindow::pointer create(mycp::bigint userId, QWidget *parent=0);

    mycp::bigint userId(void) const {return m_userId;}
    void setCallId(mycp::bigint callId) {m_callId=callId;}
    mycp::bigint callId(void) const {return m_callId;}
    void onFPMMCallBack(mycp::bigint nUserId, int nCallBackType, unsigned long lParam, unsigned int wParam);
signals:

public slots:

protected:
    virtual void resizeEvent(QResizeEvent *e);

private:
    mycp::bigint m_userId;
    mycp::bigint m_callId;
    EbWidgetVoiceBar *m_voiceBar;
};

#endif // EBWIDGETVIDEOWINDOW_H
