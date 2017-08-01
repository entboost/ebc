#ifndef EBDIALOGMYCENTER_H
#define EBDIALOGMYCENTER_H

#include "ebclientapp.h"
#include <ebdialogbase.h>


namespace Ui {
class EbDialogMyCenter;
}
class EbWidgetAccountInfo;
class EbWidgetMySetting;
class EbWidgetModifyPassword;

class EbDialogMyCenter : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogMyCenter(QWidget *parent = 0);
    ~EbDialogMyCenter(void);

    void updateLocaleInfo(void);
    void onAreaInfo(const EB_AreaInfo *areaInfo,int parameter);
public slots:
    void onClickedPushButtonAccountInfo(void);
    void onClickedPushButtonMySetting(void);
    void onClickedPushButtonModifyPassword(void);
    void udpateClickedPushButton(const QObject *sender);
    void onClickedPushButtonDefaultMember(void);
protected:
    virtual void accept(void);
private:
    Ui::EbDialogMyCenter *ui;
    QLabel * m_labelAccountInfoIcon;
    QLabel * m_labelModifyPasswordIcon;
    EbWidgetAccountInfo *m_widgetAccountInfo;
    EbWidgetMySetting *m_widgetMySetting;
    EbWidgetModifyPassword *m_widgetModifyPassword;
};

#endif // EBDIALOGMYCENTER_H
