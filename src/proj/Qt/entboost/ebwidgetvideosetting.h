#ifndef EBWIDGETVIDEOSETTING_H
#define EBWIDGETVIDEOSETTING_H

#include "ebclientapp.h"
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include "ebmm.h"
#include "ebwidgetvideowindow.h"

class EbWidgetVideoSetting : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetVideoSetting(QWidget *parent = nullptr);
    virtual ~EbWidgetVideoSetting(void);

    void closeVideo(bool fromDelete=false);
    void updateLocaleInfo(void);
signals:

public slots:
    void onClickedButtonOpen(void);
    void onCurrentIndexChangedComboBoxVideoDevices(int index);

    static void onFPMMCallBack(mycp::bigint nUserId, int nCallBackType, unsigned long lParam, unsigned int wParam, void* pUserData);

private:
    QComboBox *m_comboBoxVideoDevices;
    QPushButton *m_buttonOpen;
    Cebmm::pointer m_videoRoom;
    EbWidgetVideoWindow::pointer m_videoWindow;
};

#endif // EBWIDGETVIDEOSETTING_H
