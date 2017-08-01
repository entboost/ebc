#ifndef EBWIDGETUSERINFO_H
#define EBWIDGETUSERINFO_H

#include <ebclientapp.h>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <eblineedit.h>

class EbWidgetUserInfo : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetUserInfo(const EbcCallInfo::pointer &callInfo, QWidget *parent = nullptr);

    void updateLocaleInfo(void);
signals:

public slots:
    void onClickedButtonAddContact(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
private:
    EbcCallInfo::pointer m_callInfo;
    QLabel * m_labelImage;
    QLabel * m_labelUserName;
    QLineEdit * m_lineEditUserName;
    QLabel * m_labelAccount;
    QLineEdit * m_lineEditAccount;
    QLabel * m_labelTel;
    QLineEdit * m_lineEditTel;
    QLabel * m_labelPhone;
    QLineEdit * m_lineEditPhone;
    QLabel * m_labelEmail;
    QLineEdit * m_lineEditEmail;
    QLabel * m_labelJobTitle;
    QLineEdit * m_lineEditJobTitle;
    QLabel * m_labelGroupName;
    QLineEdit * m_lineEditGroupName;
    QLabel * m_labelEnterprise;
    QLineEdit * m_lineEditEnterprise;
    QLabel * m_labelAddress;
    QLineEdit * m_lineEditAddress;
    QPushButton * m_buttonReqeustAddContact;
};

#endif // EBWIDGETUSERINFO_H
