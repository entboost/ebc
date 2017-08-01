#ifndef EBDIALOGREGISTER_H
#define EBDIALOGREGISTER_H

#include "ebclientapp.h"
#include <ebdialogbase.h>


namespace Ui {
class EbDialogRegister;
}

class EbDialogRegister : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogRegister(QWidget *parent = 0);
    ~EbDialogRegister(void);

    const QString &account(void) const {return m_sAccount;}
//    const QString &password(void) const {return m_sPassword;}
    const QString &userName(void) const {return m_sUserName;}
    const QString &enterpriseName(void) const {return m_sEnterpriseName;}
    mycp::bigint registerUserId(void) const {return m_nRegisterUserId;}
protected:
    virtual void accept(void);
    virtual void customEvent(QEvent *e);

    void onRegisterResponse(QEvent *e);
private:
    Ui::EbDialogRegister *ui;
    CEBAppClient m_pEBAppClient;
    QString m_sAccount;
    QString m_sPassword;
    QString m_sConfirmPwd;
    QString m_sUserName;
    QString m_sEnterpriseName;
    mycp::bigint m_nRegisterUserId;
};

#endif // EBDIALOGREGISTER_H
