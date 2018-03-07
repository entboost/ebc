#ifndef EBCCALLINFO_H
#define EBCCALLINFO_H

#include <QtGui>
#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
#include "../../../include/eb_object.h"
using namespace entboost;
#include <QtQuick/QQuickItem>
#include "ebfileinfo.h"

class EbcCallInfo
{
public:
    typedef boost::shared_ptr<EbcCallInfo> pointer;
    static EbcCallInfo::pointer create(void);

    quint64 callId(void) const {return m_pCallInfo.GetCallId();}
    qint64 groupId(void) const {return m_pCallInfo.m_sGroupCode;}
    bool isGroupCall(void) const {return m_pCallInfo.m_sGroupCode>0?true:false;}
    qint64 fromUserId(void) const {return m_pFromAccountInfo.GetUserId();}
    qint64 fromMemberId(void) const {return m_pFromAccountInfo.m_pFromCardInfo.m_sMemberCode;}
    const tstring &fromUserAccount(void) const {return m_pFromAccountInfo.GetAccount();}
    tstring fromName(void) const;
    EB_CallInfo m_pCallInfo;
    EB_AccountInfo m_pFromAccountInfo;
    bool m_bOffLineUser;
    time_t m_tLastTime;

    CLockMap<qint64, EbFileInfo::pointer> m_progressFiles;    /// msg-id->
    EbcCallInfo(const EbcCallInfo *other);
    EbcCallInfo(void);
};
const EbcCallInfo::pointer EbcCallInfoNull;

class EbQuickCallInfo : public QQuickItem
        , public EbcCallInfo
{
    Q_OBJECT
    Q_PROPERTY(QString callId READ callIdString)
    Q_PROPERTY(bool isGroupCall READ isGroupCall)
    Q_PROPERTY(QString groupId READ groupIdString)
    Q_PROPERTY(QString oldCallId READ oldCallIdString)
    Q_PROPERTY(QString fromUserId READ fromUserIdString)
    Q_PROPERTY(QString fromMemberId READ fromMemberIdString)
    Q_PROPERTY(QString fromAccount READ fromAccount)
    Q_PROPERTY(bool isOffLineUser READ isOffLineUser)
public:
    EbQuickCallInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EbcCallInfo() {
    }
    EbQuickCallInfo(const EbcCallInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EbcCallInfo(other) {
    }

    QString callIdString() const {return QString::number(callId());}
    QString groupIdString() const {return QString::number(groupId());}
    QString oldCallIdString() const {return QString::number(oldCallId());}
    QString fromUserIdString() const {return QString::number(fromUserId());}
    QString fromMemberIdString() const {return QString::number(fromMemberId());}
    qint64 oldCallId() const {return m_pCallInfo.m_sOldCallId;}
//    void setFromUserId(qint64 v) {m_pFromAccountInfo.SetUserId(v);}
//    void setFromAccount(const QString &v) {m_pFromAccountInfo.SetAccount(v.toStdString());}
    QString fromAccount() const {return QString::fromStdString(m_pFromAccountInfo.GetAccount().string());}
    bool isOffLineUser() const {return m_bOffLineUser;}
};


#endif // EBCCALLINFO_H
