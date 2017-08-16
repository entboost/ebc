#ifndef EBDIALOGLOGIN_H
#define EBDIALOGLOGIN_H

#include "ebclientapp.h"
#include <QMouseEvent>
#include <QMenu>
#include <QListWidgetItem>
#include <ebdialogbase.h>
#include <eblabel.h>

namespace Ui {
class EbDialogLogin;
}

class EbLoginInfo
{
public:
    typedef boost::shared_ptr<EbLoginInfo> pointer;
    EbLoginInfo(void);
    EbLoginInfo(const mycp::tstring &account, const mycp::tstring &password, bool safePwd);
    static EbLoginInfo::pointer create(const mycp::tstring &account, const mycp::tstring &password, bool safePwd);

    mycp::tstring m_sAccount;
    mycp::tstring m_sRealAccount;
    mycp::tstring m_sPassword;
    bool m_bSafePwd;
    EB_USER_LINE_STATE m_nLineState;
    mycp::bigint m_nUserId;
    mycp::bigint m_nPhone;
    QListWidgetItem* m_item;
};

//#ifdef __MACH__
//#define EbLoginDialogType QMainWindow
//#else
//#define EbLoginDialogType EbDialogBase
//#endif

class EbDialogLogin : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogLogin(QWidget *parent = 0);
    ~EbDialogLogin(void);

    void updateLocaleInfo(void);
    void updateEntLogo(const QString &fileName);
    void updateProductName(void);
    void setErrorText(const QString &errorText, bool messageShowError);
    bool isDefaultEntLogo(void) const {return m_isDefaultEntLogo;}
public slots:
    void onClickedPushButtonSetting(void);
    void onClickedEntLogo(void);
    void setLogonCtrlEnabled(bool);
    void onClickedPushButtonLogon(void);
#ifdef WIN32
    void onClickedCheckBoxAutoRun(void);
#endif

    void onClickedSelectColor(void);
    void onClickedSelectLocale(void);

    void onTextChangedEditAccount(const QString &text);
    void onClickedLabelAccountIcon(void);
    void onItemClickedLoginRecords(QListWidgetItem *item);
    void onItemSelectionChangedLoginRecords(void);
    void onItemEnteredLoginRecords(QListWidgetItem *item);
//    void onCurrentItemChangedLoginRecords(QListWidgetItem *current, QListWidgetItem *previous);
//    void onCurrentRowChangedLoginRecords(int currentRow);

    void onClickedPushButtonDeleteAccount(void);
    void onClickedPushButtonVisitor(void);
    void onClickedPushButtonRegister(void);
    void onClickedPushButtonForgetPwd(void);
    void onClickedPushButtonConnectSetting(void);

    void processDatas(void);
protected:
    void setLoginInfo(QListWidgetItem *item, bool bAccountSelect);
    void setLoginInfo(const EbLoginInfo::pointer &pLoginInfo, bool bAccountSelect);
    void loadLoginData(void);
    void createMenuData(void);

    virtual void customEvent(QEvent *e);
    void keyPressEvent(QKeyEvent *);
    virtual bool eventFilter(QObject *obj, QEvent *e);
private:
    void onAppIdSuccess(QEvent *e); /// from ebclientapp
    void onAppIdError(QEvent *e);   /// from ebclientapp
    void onLogonSuccess(QEvent *e);
    void onLogonError(QEvent *e);
    void onOnlineAnother(QEvent *e);
private:
    Ui::EbDialogLogin *ui;
    QMenu* m_menuSetting;
    QMenu* m_menuLocale;
    EbLabel * m_labelEntLogo;
    EbLabel * m_labelAccountIcon;
    bool m_isDefaultEntLogo;
    bool m_inMessageBox;
    bool m_canSearch;
    bool m_searchFirst; /// true=查找开头 false=完全匹配
    CLockMap<mycp::bigint, EbLoginInfo::pointer> m_pLoginInfoList;
    mycp::tstring m_sOAuthKey;
};

#endif // EBDIALOGLOGIN_H
