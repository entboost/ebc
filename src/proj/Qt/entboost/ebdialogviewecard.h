#ifndef EBDIALOGVIEWECARD_H
#define EBDIALOGVIEWECARD_H

#include <ebclientapp.h>
#include <ebdialogbase.h>
#include <ebwidgetiteminfo.h>

namespace Ui {
class EbDialogViewECard;
}

class EbDialogViewECard : public EbDialogBase
{
    Q_OBJECT
public:
    enum VIEW_TYPE {
        VIEW_UNKNOWE
        , VIEW_ENTERPRISE
        , VIEW_GROUP
        , VIEW_MEMBER
        , VIEW_CONTACT
    };
    explicit EbDialogViewECard(QWidget *parent = 0);
    virtual ~EbDialogViewECard(void);

    void updateLocaleInfo(void);
    bool setItemInfo(const EbWidgetItemInfo::pointer &itemInfo);
    void setEnterpriseInfo(const EB_EnterpriseInfo *enterpriseInfo);
    void setGroupInfo(const EB_GroupInfo *groupInfo, const EB_EnterpriseInfo *enterpriseInfo);
    void setGroupInfo(eb::bigint groupId);
    void setMemberInfo(const EB_MemberInfo *memberInfo, const EB_GroupInfo *groupInfo);
    void setMemberInfo(eb::bigint groupId, eb::bigint memberUserId);
    void setContactInfo(const EB_ContactInfo *contactInfo);
    void setContactInfo1(eb::bigint contactId);
    void setContactInfo2(eb::bigint userId);

    void setMouseEnter(const QWidget * widgetValid, const QRect &rectValid, bool showImmediate=false);
    void hideReset(const QWidget * widgetValid);
    void hideReset(void);
public slots:
    void onClieckedButtonAddContact(void);
    void onClieckedButtonSendMsg(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void timerEvent(QTimerEvent *e);
    void updateEnterpriseInfo(void);
    void updateGroupInfo(void);
    void updateMemberInfo(void);
    void updateContactInfo(void);
private:
    Ui::EbDialogViewECard *ui;
    const QWidget * m_widgetValid;
    QRect m_rectValid;
    int m_timerIdCheck2Show;
//    int m_checkShowCount;
    int m_timerIdCheck2Hide;
    VIEW_TYPE m_viewType;
    EB_EnterpriseInfo m_enterpriseInfo;
    EB_GroupInfo m_groupInfo;
    EB_MemberInfo m_memberInfo;
    EB_ContactInfo m_contactInfo;
};

#endif // EBDIALOGVIEWECARD_H
