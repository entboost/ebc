#ifndef EBDIALOGMESSAGETIP_H
#define EBDIALOGMESSAGETIP_H

#include "ebclientapp.h"
#include <ebdialogbase.h>
#include <ebwidgetiteminfo.h>

namespace Ui {
class EbDialogMessageTip;
}

class EbDialogMessageTip : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogMessageTip(QWidget *parent = 0);
    ~EbDialogMessageTip(void);

    void addMsgTip(const QImage &image,mycp::bigint groupId, mycp::bigint fromUserId, const QString &msgTip);
    void addSubMsgTip(mycp::bigint nId, mycp::bigint nSubId, const QString &msgTip, const QString &param, mycp::bigint msgId);
    void addEmailMsgTip(mycp::bigint nId, mycp::bigint emailSubId, const QString &msgTip, const QString &param="");
    void delMsgTip(mycp::bigint groupId, mycp::bigint fromUserId);
    void delEmailMsgTip(mycp::bigint nId);
    bool isEmpty(void) const;

    void updateLocaleInfo(void);
signals:

public slots:
    void onItemEnteredMessageTips(QListWidgetItem *item);
    void deleteItem(QListWidgetItem *item);
    void callItem(QListWidgetItem *item);
    void onItemClickedMessageTips(QListWidgetItem *item);
    void onClickedIgnoreItem(bool);
    void onClickedIgnoreAll(bool);
    void onClickedViewAll(bool);
protected:
    virtual void resizeEvent(QResizeEvent *e);
private:
    void showMsgTip(void);
    void checkCursorInMsgArea(void);
private:
    Ui::EbDialogMessageTip *ui;
    CLockMap<mycp::bigint,EbWidgetItemInfo::pointer> m_pIdItemInfo;		/// id->
    CLockMap<mycp::bigint,EbWidgetItemInfo::pointer> m_pGroupItemInfo;	/// dep_code->
    CLockMap<mycp::bigint,EbWidgetItemInfo::pointer> m_pUserItemInfo;	/// user_id->
    EB_SubscribeFuncInfo m_emailSubscribeFuncInfo;
};

#endif // EBDIALOGMESSAGETIP_H
