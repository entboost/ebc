#ifndef EBQUICKITEMINFO_H
#define EBQUICKITEMINFO_H

#include <QtGui>
#include <QtQuick/QQuickItem>
#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
#include "../../include/eb_object.h"
#include "../../include/chatroomhandle.h"
using namespace entboost;

const QString theFileHead("file:///");

class EbQuickEnterpriseInfo : public QQuickItem
        , public EB_EnterpriseInfo
{
    Q_OBJECT
    Q_PROPERTY(QString enterpriseId READ enterpriseIdString)
//    Q_PROPERTY(qint64 enterpriseId READ enterpriseId WRITE setEnterpriseId)
    Q_PROPERTY(QString enterpriseName READ enterpriseName WRITE setEnterpriseName)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString phone READ phone WRITE setPhone)
    Q_PROPERTY(QString fax READ fax WRITE setFax)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString createTime READ createTime WRITE setCreateTime)
    Q_PROPERTY(QString createUserId READ createUserIdString)
//    Q_PROPERTY(qint64 createUserId READ createUserId WRITE setCreateUserId)
    Q_PROPERTY(int enterpriseExt READ enterpriseExt WRITE setEnterpriseExt)
public:
    EbQuickEnterpriseInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_EnterpriseInfo() {
    }
    EbQuickEnterpriseInfo(const EB_EnterpriseInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_EnterpriseInfo(other) {
    }

    void setEnterpriseId(qint64 v) {m_sEnterpriseCode=v;}
    qint64 enterpriseId() const {return m_sEnterpriseCode;}
    QString enterpriseIdString() const {return QString::number(m_sEnterpriseCode);}
    void setEnterpriseName(const QString &v) {m_sEnterpriseName = v.toStdString();}
    QString enterpriseName() const {return QString::fromStdString(m_sEnterpriseName.string());}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}
    void setPhone(const QString &v) {m_sPhone = v.toStdString();}
    QString phone() const {return QString::fromStdString(m_sPhone.string());}
    void setFax(const QString &v) {m_sFax = v.toStdString();}
    QString fax() const {return QString::fromStdString(m_sFax.string());}
    void setEmail(const QString &v) {m_sEmail = v.toStdString();}
    QString email() const {return QString::fromStdString(m_sEmail.string());}
    void setUrl(const QString &v) {m_sUrl = v.toStdString();}
    QString url() const {return QString::fromStdString(m_sUrl.string());}
    void setAddress(const QString &v) {m_sAddress = v.toStdString();}
    QString address() const {return QString::fromStdString(m_sAddress.string());}
    void setCreateTime(const QString &v) {m_sCreateTime = v.toStdString();}
    QString createTime() const {return QString::fromStdString(m_sCreateTime.string());}
    void setCreateUserId(qint64 v) {m_nCreateUserId=v;}
    qint64 createUserId() const {return m_nCreateUserId;}
    QString createUserIdString() const {return QString::number(m_nCreateUserId);}
    void setEnterpriseExt(int v) {m_nEntExt=v;}
    int enterpriseExt() const {return m_nEntExt;}
};

class EbSharedGroupInfo : public EB_GroupInfo
{
public:
    typedef boost::shared_ptr<EbSharedGroupInfo> pointer;
    EbSharedGroupInfo(const EB_GroupInfo *other)
        : EB_GroupInfo(other)
    {
    }
    static EbSharedGroupInfo::pointer create(const EB_GroupInfo *other) {
        return EbSharedGroupInfo::pointer(new EbSharedGroupInfo(other));
    }
};

class EbQuickGroupInfo : public QQuickItem
        , public EB_GroupInfo
{
    Q_OBJECT
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(QString groupFullName READ groupFullName WRITE setGroupFullName NOTIFY groupFullNameChanged)
    Q_PROPERTY(QString enterpriseId READ enterpriseIdString)
//    Q_PROPERTY(qint64 enterpriseId READ enterpriseId WRITE setEnterpriseId)
//    Q_PROPERTY(QString enterpriseName READ enterpriseName WRITE setEnterpriseName)
    Q_PROPERTY(qint64 groupId READ groupId WRITE setGroupId)
    Q_PROPERTY(qint64 parentId READ parentId WRITE setParentId)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString phone READ phone WRITE setPhone)
    Q_PROPERTY(QString fax READ fax WRITE setFax)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString createTime READ createTime WRITE setCreateTime)
    Q_PROPERTY(QString createUserId READ createUserIdString)
//    Q_PROPERTY(qint64 createUserId READ createUserId WRITE setCreateUserId)
    Q_PROPERTY(QString managerUserId READ managerUserIdString)
//    Q_PROPERTY(qint64 managerUserId READ managerUserId WRITE setManagerUserId)
//    Q_PROPERTY(QString managerUserName READ managerUserName)
    Q_PROPERTY(int displayIndex READ displayIndex WRITE setDisplayIndex)
    Q_PROPERTY(int groupType READ groupType WRITE setGroupType)
    Q_PROPERTY(QString myMemberId READ myMemberIdString)
//    Q_PROPERTY(qint64 myMemberId READ myMemberId WRITE setMyMemberId)
    Q_PROPERTY(int memberCount READ memberCount)
    Q_PROPERTY(QString groupVersion READ groupVersionString)
//    Q_PROPERTY(qint64 groupVersion READ groupVersion)
    Q_PROPERTY(int groupExtData READ groupExtData WRITE setGroupExtData)
    Q_PROPERTY(QString groupImagePath READ groupImagePath)
public:
    EbQuickGroupInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_GroupInfo() {
    }
    EbQuickGroupInfo(const EB_GroupInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_GroupInfo(other) {
    }

signals:
    void groupFullNameChanged();

public:
    void setEnterpriseId(qint64 v) {m_sEnterpriseCode=v;}
    qint64 enterpriseId() const {return m_sEnterpriseCode;}
    QString enterpriseIdString() const {return QString::number(m_sEnterpriseCode);}
//    void setEnterpriseName(const QString &v) {m_enterpriseName = v;}
//    QString enterpriseName() const {return m_enterpriseName;}
    void setGroupId(qint64 v) {m_sGroupCode=v;}
    qint64 groupId() const {return m_sGroupCode;}
    void setGroupName(const QString &v) {m_sGroupName = v.toStdString();}
    QString groupName() const {return QString::fromStdString(m_sGroupName.string());}
    void setGroupFullName(const QString &v) {
        if (m_groupFullName!=v) {
            m_groupFullName = v;
            emit groupFullNameChanged();
        }
    }
    QString groupFullName() const {return m_groupFullName;}
    void setParentId(qint64 v) {m_sParentCode=v;}
    qint64 parentId() const {return m_sParentCode;}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}
    void setPhone(const QString &v) {m_sPhone = v.toStdString();}
    QString phone() const {return QString::fromStdString(m_sPhone.string());}
    void setFax(const QString &v) {m_sFax = v.toStdString();}
    QString fax() const {return QString::fromStdString(m_sFax.string());}
    void setEmail(const QString &v) {m_sEmail = v.toStdString();}
    QString email() const {return QString::fromStdString(m_sEmail.string());}
    void setUrl(const QString &v) {m_sUrl = v.toStdString();}
    QString url() const {return QString::fromStdString(m_sUrl.string());}
    void setAddress(const QString &v) {m_sAddress = v.toStdString();}
    QString address() const {return QString::fromStdString(m_sAddress.string());}
    void setCreateTime(const QString &v) {m_sCreateTime = v.toStdString();}
    QString createTime() const {return QString::fromStdString(m_sCreateTime.string());}
    void setCreateUserId(qint64 v) {m_nCreateUserId=v;}
    qint64 createUserId() const {return m_nCreateUserId;}
    QString createUserIdString() const {return QString::number(m_nCreateUserId);}
    void setManagerUserId(qint64 v) {m_nManagerUserId=v;}
    qint64 managerUserId() const {return m_nManagerUserId;}
    QString managerUserIdString() const {return QString::number(m_nManagerUserId);}
//    void setManagerUserName(const QString &v) {m_managerUserName=v;}
//    QString managerUserName() const {return m_managerUserName;}
    void setDisplayIndex(int v) {m_nDisplayIndex=v;}
    int displayIndex() const {return m_nDisplayIndex;}
    void setGroupType(int v) {m_nGroupType=(EB_GROUP_TYPE)v;}
    int groupType() const {return (int)m_nGroupType;}
//    EB_GROUP_TYPE m_nGroupType;
    void setMyMemberId(qint64 v) {m_nMyEmpId=v;}
    qint64 myMemberId() const {return m_nMyEmpId;}
    QString myMemberIdString() const {return QString::number(m_nMyEmpId);}
    void setMemberCount(int v) {m_nEmpCount=v;}
    int memberCount() const {return m_nEmpCount;}
    void setGroupVersion(qint64 v) {m_nGroupVer=v;}
    qint64 groupVersion() const {return m_nGroupVer;}
    QString groupVersionString() const {return QString::number(m_nGroupVer);}
    void setGroupExtData(int v) {m_nGroupExtData=v;}
    int groupExtData() const {return m_nGroupExtData;}
    QString groupImagePath() {
        if (m_groupImagePath.isEmpty()) {
            if (m_nGroupType==EB_GROUP_TYPE_DEPARTMENT) {
                m_groupImagePath = "assets:/res/imgdefaultdepartment.png";
            }
            else if (m_nGroupType==EB_GROUP_TYPE_PROJECT) {
                m_groupImagePath = "assets:/res/imgdefaultproject.png";
            }
            else if (m_nGroupType==EB_GROUP_TYPE_GROUP) {
                m_groupImagePath = "assets:/res/imgdefaultgroup.png";
            }
            else {
                m_groupImagePath = "assets:/res/imgdefaulttempgroup.png";
            }
        }
        return m_groupImagePath;
    }
private:
    QString m_groupImagePath;
    QString m_groupFullName;
//    QString m_managerUserName;
//    QString m_enterpriseName;
};

class EbQuickMemberInfo : public QQuickItem
        , public EB_MemberInfo
{
    Q_OBJECT
    Q_PROPERTY(QString memberId READ memberIdString)
//    Q_PROPERTY(qint64 memberId READ memberId WRITE setMemberId)
    Q_PROPERTY(QString groupId READ groupIdString)
//    Q_PROPERTY(qint64 groupId READ groupId WRITE setGroupId)
    Q_PROPERTY(QString memberUserId READ memberUserIdString)
//    Q_PROPERTY(qint64 memberUserId READ memberUserId WRITE setMemberUserId)
    Q_PROPERTY(QString memberAccount READ memberAccount WRITE setMemberAccount)
    Q_PROPERTY(QString userName READ userName WRITE setUserName)
    Q_PROPERTY(QString jobTitle READ jobTitle WRITE setJobTitle)
    Q_PROPERTY(int jobPosition READ jobPosition WRITE setJobPosition)
    Q_PROPERTY(QString cellPhone READ cellPhone WRITE setCellPhone)
    Q_PROPERTY(QString fax READ fax WRITE setFax)
    Q_PROPERTY(QString workPhone READ workPhone WRITE setWorkPhone)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(int managerLevel READ managerLevel WRITE setManagerLevel)
    Q_PROPERTY(int gender READ gender WRITE setGender)
    Q_PROPERTY(int birthday READ birthday WRITE setBirthday)
    Q_PROPERTY(int lineState READ lineState WRITE setLineState)
    Q_PROPERTY(bool isOnlineState READ isOnlineState)
    Q_PROPERTY(QString headResourceId READ headResourceIdString)
//    Q_PROPERTY(qint64 headResourceId READ headResourceId WRITE setHeadResourceId)
    Q_PROPERTY(QString headResourceFile READ headResourceFile WRITE setHeadResourceFile)
    Q_PROPERTY(QString headImageFile READ headImageFile)
    Q_PROPERTY(QString headMd5 READ headMd5 WRITE setHeadMd5)
    Q_PROPERTY(QString clientServiceId READ clientServiceIdString)
//    Q_PROPERTY(qint64 clientServiceId READ clientServiceId WRITE setClientServiceId)
    Q_PROPERTY(int clientServiceExt READ clientServiceExt WRITE setClientServiceExt)
    Q_PROPERTY(int displayIndex READ displayIndex WRITE setDisplayIndex)
    Q_PROPERTY(int forbidMinutes READ forbidMinutes WRITE setForbidMinutes)
    Q_PROPERTY(int subType READ subType WRITE setSubType)
public:
    EbQuickMemberInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_MemberInfo() {
    }
    EbQuickMemberInfo(const EB_MemberInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_MemberInfo(other) {
    }

    void setMemberId(qint64 v) {m_sMemberCode=v;}
    qint64 memberId() const {return m_sMemberCode;}
    QString memberIdString() const {return QString::number(m_sMemberCode);}
    void setGroupId(qint64 v) {m_sGroupCode=v;}
    qint64 groupId() const {return m_sGroupCode;}
    QString groupIdString() const {return QString::number(m_sGroupCode);}
    void setMemberUserId(qint64 v) {m_nMemberUserId=v;}
    qint64 memberUserId() const {return m_nMemberUserId;}
    QString memberUserIdString() const {return QString::number(m_nMemberUserId);}
    void setMemberAccount(const QString &v) {m_sMemberAccount = v.toStdString();}
    QString memberAccount() const {return QString::fromStdString(m_sMemberAccount.string());}
    void setUserName(const QString &v) {m_sUserName = v.toStdString();}
    QString userName() const {return QString::fromStdString(m_sUserName.string());}
    void setJobTitle(const QString &v) {m_sJobTitle = v.toStdString();}
    QString jobTitle() const {return QString::fromStdString(m_sJobTitle.string());}
    void setJobPosition(int v) {m_nJobPosition=v;}
    int jobPosition() const {return m_nJobPosition;}
    void setCellPhone(const QString &v) {m_sCellPhone = v.toStdString();}
    QString cellPhone() const {return QString::fromStdString(m_sCellPhone.string());}
    void setFax(const QString &v) {m_sFax = v.toStdString();}
    QString fax() const {return QString::fromStdString(m_sFax.string());}
    void setWorkPhone(const QString &v) {m_sWorkPhone = v.toStdString();}
    QString workPhone() const {return QString::fromStdString(m_sWorkPhone.string());}
    void setEmail(const QString &v) {m_sEmail = v.toStdString();}
    QString email() const {return QString::fromStdString(m_sEmail.string());}
    void setAddress(const QString &v) {m_sAddress = v.toStdString();}
    QString address() const {return QString::fromStdString(m_sAddress.string());}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}
    void setManagerLevel(int v) {m_nManagerLevel=v;}
    int managerLevel() const {return m_nManagerLevel;}
    /// 0:unknown 1:M 2:F
    void setGender(int v) {m_nGender=(EB_GENDER_TYPE)v;}
    int gender() const {return (int)m_nGender;}
    /// format: 19790101
    void setBirthday(int v) {m_nBirthday=v;}
    int birthday() const {return m_nBirthday;}
    void setLineState(int v) {m_nLineState=(EB_USER_LINE_STATE)v;}
    int lineState() const {return (int)m_nLineState;}
    bool isOnlineState() const {return m_nLineState>=EB_LINE_STATE_BUSY?true:false;}
    void setHeadResourceId(qint64 v) {m_sHeadResourceId=v;}
    qint64 headResourceId() const {return m_sHeadResourceId;}
    QString headResourceIdString() const {return QString::number(m_sHeadResourceId);}
    void setHeadResourceFile(const QString &v) {m_sHeadResourceFile = v;}
    QString headResourceFile() const {return m_sHeadResourceFile;}
    QString headImageFile() const {
        if (!m_sHeadResourceFile.isEmpty() && QFile::exists(m_sHeadResourceFile)) {
            return theFileHead+m_sHeadResourceFile;
        }
        else {
            return "assets:/res/defaultmember.png";
        }
    }
    void setHeadMd5(const QString &v) {m_sHeadMd5 = v.toStdString();}
    QString headMd5() const {return QString::fromStdString(m_sHeadMd5.string());}
    void setClientServiceId(qint64 v) {m_nCSId=v;}
    qint64 clientServiceId() const {return m_nCSId;}
    QString clientServiceIdString() const {return QString::number(m_nCSId);}
    void setClientServiceExt(int v) {m_nCSExt=v;}
    int clientServiceExt() const {return m_nCSExt;}
    void setDisplayIndex(int v) {m_nDisplayIndex=v;}
    int displayIndex() const {return m_nDisplayIndex;}
    void setForbidMinutes(int v) {m_nForbidMinutes=v;}
    int forbidMinutes() const {return m_nForbidMinutes;}
    void setSubType(int v) {m_subType=v;}
    int subType() const {return m_subType;}
private:
    /// 主要用于排序用
    int m_subType;
};


class EbQuickUGInfo : public QQuickItem
        , public EB_UGInfo
{
    Q_OBJECT
    Q_PROPERTY(QString ugId READ ugIdString)
//    Q_PROPERTY(qint64 ugId READ ugId WRITE setUgId)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(QString groupFullName READ groupFullName WRITE setGroupFullName NOTIFY groupFullNameChanged)
public:
    EbQuickUGInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_UGInfo() {
    }
    EbQuickUGInfo(const EB_UGInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_UGInfo(other) {
    }

signals:
    void groupFullNameChanged();

public:
    void setUgId(qint64 v) {m_nUGId=v;}
    qint64 ugId() const {return m_nUGId;}
    QString ugIdString() const {return QString::number(m_nUGId);}
    void setGroupName(const QString &v) {m_sGroupName = v.toStdString();}
    QString groupName() const {return QString::fromStdString(m_sGroupName.string());}
    void setGroupFullName(const QString &v) {
        if (m_groupFullName!=v) {
            m_groupFullName = v;
            emit groupFullNameChanged();
        }
    }
    QString groupFullName() const {return m_groupFullName;}
private:
    QString m_groupFullName;
};


class EbQuickContactInfo : public QQuickItem
        , public EB_ContactInfo
{
    Q_OBJECT
    Q_PROPERTY(QString ugId READ ugIdString)
//    Q_PROPERTY(qint64 ugId READ ugId WRITE setUgId)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)

    Q_PROPERTY(QString contactId READ contactIdString)
//    Q_PROPERTY(qint64 contactId READ contactId WRITE setContactId)
    Q_PROPERTY(QString contactUserId READ contactUserIdString)
//    Q_PROPERTY(qint64 contactUserId READ contactUserId WRITE setContactUserId)
    Q_PROPERTY(int contactType READ contactType WRITE setContactType)
    Q_PROPERTY(QString contact READ contact WRITE setContact)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString phone READ phone WRITE setPhone)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString company READ company WRITE setCompany)
    Q_PROPERTY(QString jobTitle READ jobTitle WRITE setJobTitle)
    Q_PROPERTY(QString tel READ tel WRITE setTel)
    Q_PROPERTY(QString fax READ fax WRITE setFax)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(int lineState READ lineState WRITE setLineState)
    Q_PROPERTY(bool isOnlineState READ isOnlineState)
    Q_PROPERTY(QString headResourceId READ headResourceIdString)
//    Q_PROPERTY(qint64 headResourceId READ headResourceId WRITE setHeadResourceId)
    Q_PROPERTY(QString headResourceFile READ headResourceFile WRITE setHeadResourceFile)
    Q_PROPERTY(QString headMd5 READ headMd5 WRITE setHeadMd5)
public:
    EbQuickContactInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_ContactInfo() {
    }
    EbQuickContactInfo(const EB_ContactInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_ContactInfo(other) {
    }

    void setUgId(qint64 v) {m_nUGId=v;}
    qint64 ugId() const {return m_nUGId;}
    QString ugIdString() const {return QString::number(m_nUGId);}
    void setGroupName(const QString &v) {m_sGroupName = v.toStdString();}
    QString groupName() const {return QString::fromStdString(m_sGroupName.string());}

    void setContactId(qint64 v) {m_nContactId=v;}
    qint64 contactId() const {return m_nContactId;}
    QString contactIdString() const {return QString::number(m_nContactId);}
    void setContactUserId(qint64 v) {m_nContactUserId=v;}
    qint64 contactUserId() const {return m_nContactUserId;}
    QString contactUserIdString() const {return QString::number(m_nContactUserId);}
    void setContactType(int v) {m_nContactType=v;}
    int contactType() const {return (int)m_nContactType;}
    void setContact(const QString &v) {m_sContact = v.toStdString();}
    QString contact() const {return QString::fromStdString(m_sContact.string());}
    void setName(const QString &v) {m_sName = v.toStdString();}
    QString name() const {return QString::fromStdString(m_sName.string());}
    void setPhone(const QString &v) {m_sPhone = v.toStdString();}
    QString phone() const {return QString::fromStdString(m_sPhone.string());}
    void setEmail(const QString &v) {m_sEmail = v.toStdString();}
    QString email() const {return QString::fromStdString(m_sEmail.string());}
    void setAddress(const QString &v) {m_sAddress = v.toStdString();}
    QString address() const {return QString::fromStdString(m_sAddress.string());}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}
    void setCompany(const QString &v) {m_sCompany = v.toStdString();}
    QString company() const {return QString::fromStdString(m_sCompany.string());}
    void setJobTitle(const QString &v) {m_sJobTitle = v.toStdString();}
    QString jobTitle() const {return QString::fromStdString(m_sJobTitle.string());}
    void setTel(const QString &v) {m_sTel = v.toStdString();}
    QString tel() const {return QString::fromStdString(m_sTel.string());}
    void setFax(const QString &v) {m_sFax = v.toStdString();}
    QString fax() const {return QString::fromStdString(m_sFax.string());}
    void setUrl(const QString &v) {m_sUrl = v.toStdString();}
    QString url() const {return QString::fromStdString(m_sUrl.string());}
    void setLineState(int v) {m_nLineState=(EB_USER_LINE_STATE)v;}
    int lineState() const {return (int)m_nLineState;}
    bool isOnlineState() const {return m_nLineState>=EB_LINE_STATE_BUSY?true:false;}
    void setHeadResourceId(qint64 v) {m_sHeadResourceId=v;}
    qint64 headResourceId() const {return m_sHeadResourceId;}
    QString headResourceIdString() const {return QString::number(m_sHeadResourceId);}
    void setHeadResourceFile(const QString &v) {m_sHeadResourceFile = v;}
    QString headResourceFile() const {return m_sHeadResourceFile;}
    QString headImageFile() const {
        if (!m_sHeadResourceFile.isEmpty() && QFile::exists(m_sHeadResourceFile)) {
            return theFileHead+m_sHeadResourceFile;
        }
        else {
            return "assets:/res/defaultcontact.png";
        }
    }
//    void setHeadResourceFile(const QString &v) {m_sHeadResourceFile = v.toStdString();}
//    QString headResourceFile() const {return QString::fromStdString(m_sHeadResourceFile.string());}
    void setHeadMd5(const QString &v) {m_sHeadMd5 = v.toStdString();}
    QString headMd5() const {return QString::fromStdString(m_sHeadMd5.string());}
};

//class EbQuickCallInfo : public QQuickItem
//        , public EB_CallInfo
//{
//    Q_OBJECT
//    Q_PROPERTY(qint64 callId READ callId WRITE setCallId)
//    Q_PROPERTY(bool isGroupCall READ isGroupCall)
//    Q_PROPERTY(qint64 groupId READ groupId WRITE setGroupId)
//    Q_PROPERTY(qint64 oldCallId READ oldCallId WRITE setOldCallId)
//    Q_PROPERTY(qint64 fromUserId READ fromUserId WRITE setFromUserId)
//    Q_PROPERTY(QString fromAccount READ fromAccount WRITE setFromAccount)
//public:
//    EbQuickCallInfo(QQuickItem *parent = 0)
//        : QQuickItem(parent), EB_CallInfo() {
//    }
//    EbQuickCallInfo(const EB_CallInfo *other, QQuickItem *parent = 0)
//        : QQuickItem(parent), EB_CallInfo(other) {
//    }

//    void setCallId(qint64 v) {m_sCallId=v;}
//    qint64 callId() const {return m_sCallId;}
//    bool isGroupCall() const {return m_sGroupCode>0?true:false;}
//    void setGroupId(qint64 v) {m_sGroupCode=v;}
//    qint64 groupId() const {return m_sGroupCode;}
//    void setOldCallId(qint64 v) {m_sOldCallId=v;}
//    qint64 oldCallId() const {return m_sOldCallId;}
//    void setFromUserId(qint64 v) {m_nFromUserId=v;}
//    qint64 fromUserId() const {return m_nFromUserId;}
//    void setFromAccount(const QString &v) {m_sFromAccount = v.toStdString();}
//    QString fromAccount() const {return QString::fromStdString(m_sFromAccount.string());}
//};


class EbQuickECardInfo : public QQuickItem
        , public EB_ECardInfo
{
    Q_OBJECT
    Q_PROPERTY(int accountType READ accountType WRITE setAccountType)
    Q_PROPERTY(QString memberUserId READ memberUserIdString)
//    Q_PROPERTY(qint64 memberUserId READ memberUserId WRITE setMemberUserId)
    Q_PROPERTY(QString memberId READ memberIdString)
//    Q_PROPERTY(qint64 memberId READ memberId WRITE setMemberId)
    Q_PROPERTY(QString accountName READ accountName WRITE setAccountName)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString phone READ phone WRITE setPhone)
    Q_PROPERTY(QString tel READ tel WRITE setTel)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(QString groupId READ groupIdString)
//    Q_PROPERTY(qint64 groupId READ groupId WRITE setGroupId)
    Q_PROPERTY(QString enterprise READ enterprise WRITE setEnterprise)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString headResourceId READ headResourceIdString)
//    Q_PROPERTY(qint64 headResourceId READ headResourceId WRITE setHeadResourceId)
    Q_PROPERTY(QString headResourceFile READ headResourceFile WRITE setHeadResourceFile)
    Q_PROPERTY(QString headMd5 READ headMd5 WRITE setHeadMd5)
public:
    EbQuickECardInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_ECardInfo() {
    }
    EbQuickECardInfo(const EB_ECardInfo &other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_ECardInfo(other) {
    }

    void setAccountType(int v) {m_nAccountType=(EB_ACCOUNT_TYPE)v;}
    int accountType() const {return (int)m_nAccountType;}
    void setMemberUserId(qint64 v) {m_nMemberUserId=v;}
    qint64 memberUserId() const {return m_nMemberUserId;}
    QString memberUserIdString() const {return QString::number(m_nMemberUserId);}
    void setMemberId(qint64 v) {m_sMemberCode=v;}
    qint64 memberId() const {return m_sMemberCode;}
    QString memberIdString() const {return QString::number(m_sMemberCode);}
    void setAccountName(const QString &v) {m_sAccountName = v.toStdString();}
    QString accountName() const {return QString::fromStdString(m_sAccountName.string());}
    void setName(const QString &v) {m_sName = v.toStdString();}
    QString name() const {return QString::fromStdString(m_sName.string());}
    void setPhone(const QString &v) {m_sPhone = v.toStdString();}
    QString phone() const {return QString::fromStdString(m_sPhone.string());}
    void setTel(const QString &v) {m_sTel = v.toStdString();}
    QString tel() const {return QString::fromStdString(m_sTel.string());}
    void setEmail(const QString &v) {m_sEmail = v.toStdString();}
    QString email() const {return QString::fromStdString(m_sEmail.string());}
    void setTitle(const QString &v) {m_sTitle = v.toStdString();}
    QString title() const {return QString::fromStdString(m_sTitle.string());}
    void setGroupName(const QString &v) {m_sGroupName = v.toStdString();}
    QString groupName() const {return QString::fromStdString(m_sGroupName.string());}
    void setGroupId(qint64 v) {m_sGroupCode=v;}
    qint64 groupId() const {return m_sGroupCode;}
    QString groupIdString() const {return QString::number(m_sGroupCode);}
    void setEnterprise(const QString &v) {m_sEnterprise = v.toStdString();}
    QString enterprise() const {return QString::fromStdString(m_sEnterprise.string());}
    void setAddress(const QString &v) {m_sAddress = v.toStdString();}
    QString address() const {return QString::fromStdString(m_sAddress.string());}

    void setHeadResourceId(qint64 v) {m_nHeadResId=v;}
    qint64 headResourceId() const {return m_nHeadResId;}
    QString headResourceIdString() const {return QString::number(m_nHeadResId);}
    void setHeadResourceFile(const QString &v) {m_sHeadResourceFile = v;}
    QString headResourceFile() const {return m_sHeadResourceFile;}
    void setHeadMd5(const QString &v) {m_sHeadMd5 = v.toStdString();}
    QString headMd5() const {return QString::fromStdString(m_sHeadMd5.string());}
//    eb::bigint	m_nUserSignId;
//    eb::bigint	m_nFromIp;
//    eb::bigint	m_nHeadVer;
//    CEBString	m_sHeadCmServer;
//    CEBString	m_sHeadHttpServer;
};

class EbQuickAccountInfo : public QQuickItem
        , public EB_AccountInfo
{
    Q_OBJECT
    Q_PROPERTY(QString userId READ userIdString)
//    Q_PROPERTY(qint64 userId READ userId WRITE setUserId)
    Q_PROPERTY(QString account READ account WRITE setAccount)
    Q_PROPERTY(QString phone READ phoneString)
//    Q_PROPERTY(qint64 phone READ phone WRITE setPhone)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(QString userName READ userName WRITE setUserName)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(int gender READ gender WRITE setGender)
    Q_PROPERTY(int birthday READ GetBirthday WRITE SetBirthday)
    Q_PROPERTY(QString mobile READ mobile WRITE setMobile)
    Q_PROPERTY(QString tel READ tel WRITE setTel)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(int area1 READ GetArea1 WRITE SetArea1)
    Q_PROPERTY(int area2 READ GetArea2 WRITE SetArea2)
    Q_PROPERTY(int area3 READ GetArea3 WRITE SetArea3)
    Q_PROPERTY(int area4 READ GetArea4 WRITE SetArea4)
    Q_PROPERTY(QString area1String READ area1String WRITE setArea1String)
    Q_PROPERTY(QString area2String READ area2String WRITE setArea2String)
    Q_PROPERTY(QString area3String READ area3String WRITE setArea3String)
    Q_PROPERTY(QString area4String READ area4String WRITE setArea4String)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString zipCode READ zipCode WRITE setZipCode)
    Q_PROPERTY(EbQuickECardInfo *eCardInfo READ eCardInfo WRITE setECardInfo)
public:
    EbQuickAccountInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_AccountInfo()
        , m_eCardInfo(0)
    {
    }
    EbQuickAccountInfo(const EB_AccountInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_AccountInfo(other)
        , m_eCardInfo(0)
    {
        m_eCardInfo = new EbQuickECardInfo(m_pFromCardInfo);
    }
    virtual ~EbQuickAccountInfo() {
        if (m_eCardInfo!=0) {
            delete m_eCardInfo;
        }
    }

    void setUserId(qint64 v) {m_nUserId=v;}
    qint64 userId() const {return m_nUserId;}
    QString userIdString() const {return QString::number(m_nUserId);}
    void setAccount(const QString &v) {m_sAccount = v.toStdString();}
    QString account() const {return QString::fromStdString(m_sAccount.string());}
    void setPhone(qint64 v) {m_nPhone=v;}
    qint64 phone() const {return m_nPhone;}
    QString phoneString() const {return QString::number(m_nPhone);}
    void setPassword(const QString &v) {m_sPasswd = v.toStdString();}
    QString password() const {return QString::fromStdString(m_sPasswd.string());}
    void setUserName(const QString &v) {m_sUserName = v.toStdString();}
    QString userName() const {return QString::fromStdString(m_sUserName.string());}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}

    /// 0:unknown 1:M 2:F
    void setGender(int v) {m_nGender=(EB_GENDER_TYPE)v;}
    int gender() const {return (int)m_nGender;}
    void setMobile(const QString &v) {m_sMobile = v.toStdString();}
    QString mobile() const {return QString::fromStdString(m_sMobile.string());}
    void setTel(const QString &v) {m_sTel = v.toStdString();}
    QString tel() const {return QString::fromStdString(m_sTel.string());}
    void setEmail(const QString &v) {m_sEmail = v.toStdString();}
    QString email() const {return QString::fromStdString(m_sEmail.string());}
    void setArea1String(const QString &v) {m_sArea1 = v.toStdString();}
    QString area1String() const {return QString::fromStdString(m_sArea1.string());}
    void setArea2String(const QString &v) {m_sArea2 = v.toStdString();}
    QString area2String() const {return QString::fromStdString(m_sArea2.string());}
    void setArea3String(const QString &v) {m_sArea3 = v.toStdString();}
    QString area3String() const {return QString::fromStdString(m_sArea3.string());}
    void setArea4String(const QString &v) {m_sArea4 = v.toStdString();}
    QString area4String() const {return QString::fromStdString(m_sArea4.string());}
    void setAddress(const QString &v) {m_sAddress = v.toStdString();}
    QString address() const {return QString::fromStdString(m_sAddress.string());}
    void setUrl(const QString &v) {m_sUrl = v.toStdString();}
    QString url() const {return QString::fromStdString(m_sUrl.string());}
    void setZipCode(const QString &v) {m_sZipCode = v.toStdString();}
    QString zipCode() const {return QString::fromStdString(m_sZipCode.string());}
    EbQuickECardInfo *eCardInfo(void) const {return m_eCardInfo;}
    void setECardInfo(EbQuickECardInfo *v) {
        if (m_eCardInfo!=0) {
            delete m_eCardInfo;
            m_eCardInfo = 0;
        }
        m_eCardInfo = new EbQuickECardInfo(v);
    }
private:
    EbQuickECardInfo *m_eCardInfo;

};

class EbQuickResourceInfo : public QQuickItem
        , public EB_ResourceInfo
{
    Q_OBJECT
    Q_PROPERTY(QString resourceId READ resourceIdString)
//    Q_PROPERTY(qint64 resourceId READ resourceId WRITE setResourceId)
    Q_PROPERTY(QString parentResourceId READ parentResourceIdString)
//    Q_PROPERTY(qint64 parentResourceId READ parentResourceId WRITE setParentResourceId)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(int resourceType READ resourceType WRITE setResourceType)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString fromId READ fromIdString)
//    Q_PROPERTY(qint64 fromId READ fromId WRITE setFromId)
    Q_PROPERTY(int fromType READ fromType WRITE setFromType)
    Q_PROPERTY(QString createUserId READ createUserIdString)
//    Q_PROPERTY(qint64 createUserId READ createUserId WRITE setCreateUserId)
    Q_PROPERTY(QString time READ time WRITE setTime)
    Q_PROPERTY(QString deleteTime READ deleteTime WRITE setDeleteTime)
    Q_PROPERTY(unsigned int downloads READ downloads WRITE setDownloads)
    Q_PROPERTY(qint64 size READ size WRITE setSize)
    Q_PROPERTY(int shareType READ shareType WRITE setShareType)
    Q_PROPERTY(QString onlineViewUrl READ onlineViewUrl WRITE setOnlineViewUrl)
    Q_PROPERTY(int viewExtType READ viewExtType WRITE setViewExtType)
public:
    EbQuickResourceInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_ResourceInfo() {
    }
    EbQuickResourceInfo(const EB_ResourceInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_ResourceInfo(other) {
    }

    void setResourceId(qint64 v) {m_sResId=v;}
    qint64 resourceId() const {return m_sResId;}
    QString resourceIdString() const {return QString::number(m_sResId);}
    void setParentResourceId(qint64 v) {m_sParentResId=v;}
    qint64 parentResourceId() const {return m_sParentResId;}
    QString parentResourceIdString() const {return QString::number(m_sParentResId);}
    void setName(const QString &v) {m_sName = v.toStdString();}
    QString name() const {return QString::fromStdString(m_sName.string());}
    void setResourceType(int v) {m_nResType=(EB_RESOURCE_TYPE)v;}
    int resourceType() const {return (int)m_nResType;}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}
    void setFromId(qint64 v) {m_nFromId=v;}
    qint64 fromId() const {return m_nFromId;}
    QString fromIdString() const {return QString::number(m_nFromId);}
    void setFromType(int v) {m_nFromType=(EB_RESOURCE_FROM_TYPE)v;}
    int fromType() const {return (int)m_nFromType;}
    void setCreateUserId(qint64 v) {m_nCreateUserId=v;}
    qint64 createUserId() const {return m_nCreateUserId;}
    QString createUserIdString() const {return QString::number(m_nCreateUserId);}
    void setTime(const QString &v) {m_sTime = v.toStdString();}
    QString time() const {return QString::fromStdString(m_sTime.string());}
    void setDeleteTime(const QString &v) {m_sDeleteTime = v.toStdString();}
    QString deleteTime() const {return QString::fromStdString(m_sDeleteTime.string());}
    void setDownloads(unsigned int v) {m_nDownloads=v;}
    unsigned int downloads() const {return m_nDownloads;}
    void setSize(qint64 v) {m_nSize=v;}
    qint64 size() const {return m_nSize;}
    void setShareType(int v) {m_nShare=(EB_RESOURCE_SHARE_TYPE)v;}
    int shareType() const {return (int)m_nShare;}
    void setOnlineViewUrl(const QString &v) {m_sOnlineViewUrl = v.toStdString();}
    QString onlineViewUrl() const {return QString::fromStdString(m_sOnlineViewUrl.string());}
    void setViewExtType(int v) {m_nViewExtType=(EB_FILE_EXT_TYPE)v;}
    int viewExtType() const {return (int)m_nViewExtType;}
};


class EbQuickEmotionInfo : public QQuickItem
        , public EB_EmotionInfo
{
    Q_OBJECT
    Q_PROPERTY(QString resourceId READ resourceIdString)
//    Q_PROPERTY(qint64 resourceId READ resourceId WRITE setResourceId)
    Q_PROPERTY(QString resourceFile READ resourceFile WRITE setResourceFile)
    Q_PROPERTY(QString resourceImageFile READ resourceImageFile)
    Q_PROPERTY(int index READ index WRITE setIndex)
    Q_PROPERTY(int type READ type WRITE setType)
    Q_PROPERTY(QString userId READ userIdString)
//    Q_PROPERTY(qint64 userId READ userId WRITE setUserId)
    Q_PROPERTY(QString enterpriseId READ enterpriseIdString)
//    Q_PROPERTY(qint64 enterpriseId READ enterpriseId WRITE setEnterpriseId)
    Q_PROPERTY(int resourceType READ resourceType WRITE setResourceType)
    Q_PROPERTY(QString description READ description WRITE setDescription)
public:
    EbQuickEmotionInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_EmotionInfo() {
    }
    EbQuickEmotionInfo(const EB_EmotionInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_EmotionInfo(other) {
    }

    void setResourceId(qint64 v) {m_sResId=v;}
    qint64 resourceId() const {return m_sResId;}
    QString resourceIdString() const {return QString::number(m_sResId);}
    void setResourceFile(const QString &v) {m_sResFile = v;}
    QString resourceFile() const {return m_sResFile;}
    QString resourceImageFile() const {
        if (!m_sResFile.isEmpty() && QFileInfo::exists(m_sResFile) ) {
            return theFileHead + m_sResFile;
        }
        else {
            return "";
        }
    }
    void setIndex(int v) {m_nIndex=v;}
    int index() const {return m_nIndex;}
    void setType(int v) {m_nType=v;}
    int type() const {return m_nType;}
    void setUserId(qint64 v) {m_nUserId=v;}
    qint64 userId() const {return m_nUserId;}
    QString userIdString() const {return QString::number(m_nUserId);}
    void setEnterpriseId(qint64 v) {m_sEnterpriseCode=v;}
    qint64 enterpriseId() const {return m_sEnterpriseCode;}
    QString enterpriseIdString() const {return QString::number(m_sEnterpriseCode);}
    void setResourceType(int v) {m_nResType=(EB_RESOURCE_TYPE)v;}
    int resourceType() const {return (int)m_nResType;}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}
};


class EbQuickSubscribeFuncInfo : public QQuickItem
        , public EB_SubscribeFuncInfo
{
    Q_OBJECT
    Q_PROPERTY(QString subscribeId READ subscribeIdString)
//    Q_PROPERTY(qint64 subscribeId READ subscribeId WRITE setSubscribeId)
    Q_PROPERTY(QString fromSubscribeId READ fromSubscribeIdString)
//    Q_PROPERTY(qint64 fromSubscribeId READ fromSubscribeId WRITE setFromSubscribeId)
    Q_PROPERTY(int location READ location WRITE setLocation)
    Q_PROPERTY(QString iconResourceId READ iconResourceIdString)
//    Q_PROPERTY(qint64 iconResourceId READ iconResourceId WRITE setIconResourceId)
    Q_PROPERTY(QString iconResourceFile READ iconResourceFile WRITE setIconResourceFile)
    Q_PROPERTY(QString iconImageFile READ iconImageFile)
    Q_PROPERTY(QString functionName READ functionName WRITE setFunctionName)
    Q_PROPERTY(QString functionUrl READ functionUrl WRITE setFunctionUrl)
    Q_PROPERTY(int functionMode READ functionMode WRITE setFunctionMode)
    Q_PROPERTY(int windowWidth READ windowWidth WRITE setWindowWidth)
    Q_PROPERTY(int windowHeight READ windowHeight WRITE setWindowHeight)
    Q_PROPERTY(bool isDisableContextMenu READ isDisableContextMenu WRITE setDisableContextMenu)
    Q_PROPERTY(bool isDisableScrollBar READ isDisableScrollBar WRITE setDisableScrollBar)
    Q_PROPERTY(bool isClosePrompt READ isClosePrompt WRITE setClosePrompt)
    Q_PROPERTY(bool isWinResizable READ isWinResizable WRITE setWinResizable)
    Q_PROPERTY(int displayIndex READ displayIndex WRITE setDisplayIndex)
    Q_PROPERTY(QString description READ description WRITE setDescription)
public:
    EbQuickSubscribeFuncInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_SubscribeFuncInfo() {
    }
    EbQuickSubscribeFuncInfo(const EB_SubscribeFuncInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_SubscribeFuncInfo(other) {
    }

//    EB_FUNC_BROWSER_TYPE m_nBrowserType;
    void setSubscribeId(qint64 v) {m_nSubscribeId=v;}
    qint64 subscribeId() const {return m_nSubscribeId;}
    QString subscribeIdString() const {return QString::number(m_nSubscribeId);}
    void setFromSubscribeId(qint64 v) {m_nFromSubscribeId=v;}
    qint64 fromSubscribeId() const {return m_nFromSubscribeId;}
    QString fromSubscribeIdString() const {return QString::number(m_nFromSubscribeId);}
    void setLocation(int v) {m_nLocation=v;}
    int location() const {return (int)m_nLocation;}
    void setIconResourceId(qint64 v) {m_nIconResId=v;}
    qint64 iconResourceId() const {return m_nIconResId;}
    QString iconResourceIdString() const {return QString::number(m_nIconResId);}
    void setIconResourceFile(const QString &v) {m_sResFile = v;}
    QString iconResourceFile() const {return m_sResFile;}
    QString iconImageFile() const {
        if (!m_sResFile.isEmpty() && QFile::exists(m_sResFile)) {
            return theFileHead+m_sResFile;
        }
        else {
            /// 返回空，由 QML 判断处理
            return "";
        }
    }
    void setFunctionName(const QString &v) {m_sFunctionName = v.toStdString();}
    QString functionName() const {return QString::fromStdString(m_sFunctionName.string());}
    void setFunctionUrl(const QString &v) {m_sFunctionUrl = v.toStdString();}
    QString functionUrl() const {return QString::fromStdString(m_sFunctionUrl.string());}
    void setFunctionMode(int v) {m_nFunctionMode=(EB_FUNC_MODE)v;}
    int functionMode() const {return (int)m_nFunctionMode;}
    void setWindowWidth(int v) {m_nWindowWidth=v;}
    int windowWidth() const {return (int)m_nWindowWidth;}
    void setWindowHeight(int v) {m_nWindowHeight=v;}
    int windowHeight() const {return (int)m_nWindowHeight;}
    void setDisableContextMenu(bool v) {m_bDisableContextMenu=v;}
    bool isDisableContextMenu() const {return m_bDisableContextMenu;}
    void setDisableScrollBar(bool v) {m_bDisableScrollBar=v;}
    bool isDisableScrollBar() const {return m_bDisableScrollBar;}
    void setClosePrompt(bool v) {m_bClosePrompt=v;}
    bool isClosePrompt() const {return m_bClosePrompt;}
    void setWinResizable(bool v) {m_bWinResizable=v;}
    bool isWinResizable() const {return m_bWinResizable;}
    void setDisplayIndex(int v) {m_nDisplayIndex=v;}
    int displayIndex() const {return (int)m_nDisplayIndex;}
    void setDescription(const QString &v) {m_sDescription = v.toStdString();}
    QString description() const {return QString::fromStdString(m_sDescription.string());}
};

class EbQuickAreaInfo : public QQuickItem
        , public EB_AreaInfo
{
    Q_OBJECT
    Q_PROPERTY(int areaCode READ areaCode WRITE setAreaCode)
    Q_PROPERTY(int parentCode READ parentCode WRITE setParentCode)
    Q_PROPERTY(QString areaName READ areaName WRITE setAreaName)
    Q_PROPERTY(QString areaDigit READ areaDigit WRITE setAreaDigit)
public:
    EbQuickAreaInfo(QQuickItem *parent = 0)
        : QQuickItem(parent), EB_AreaInfo() {
    }
    EbQuickAreaInfo(const EB_AreaInfo *other, QQuickItem *parent = 0)
        : QQuickItem(parent), EB_AreaInfo(other) {
    }

    void setAreaCode(int v) {m_nAreaCode=v;}
    int areaCode() const {return m_nAreaCode;}
    void setParentCode(int v) {m_nParentCode=v;}
    int parentCode() const {return m_nParentCode;}
    void setAreaName(const QString &v) {m_sAreaName = v.toStdString();}
    QString areaName() const {return QString::fromStdString(m_sAreaName.string());}
    void setAreaDigit(const QString &v) {m_sAreaDigit = v.toStdString();}
    QString areaDigit() const {return QString::fromStdString(m_sAreaDigit.string());}
};

class EbQuickSessionInfo : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool isMsgSession READ isMsgSession)
    Q_PROPERTY(qint64 time READ time WRITE setTime)
    Q_PROPERTY(QString timeString READ timeString WRITE setTimeString)
    Q_PROPERTY(QString callId READ callIdString)
//    Q_PROPERTY(qint64 callId READ callId WRITE setCallId)
    Q_PROPERTY(QString groupId READ groupIdString)
//    Q_PROPERTY(qint64 groupId READ groupId WRITE setGroupId)
    Q_PROPERTY(bool isGroupCall READ isGroupCall)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(QString groupFullName READ groupFullName WRITE setGroupFullName NOTIFY groupFullNameChanged)
    Q_PROPERTY(QString memberId READ memberIdString)
//    Q_PROPERTY(qint64 memberId READ memberId WRITE setMemberId)
    Q_PROPERTY(QString fromUserId READ fromUserIdString)
//    Q_PROPERTY(qint64 fromUserId READ fromUserId WRITE setFromUserId)
    Q_PROPERTY(QString fromPhone READ fromPhoneString)
//    Q_PROPERTY(qint64 fromPHone READ fromPhone WRITE setFromPhone)
    Q_PROPERTY(QString fromAccount READ fromAccount WRITE setFromAccount)
    Q_PROPERTY(QString fromName READ fromName WRITE setFromName)
    Q_PROPERTY(int fromType READ fromType WRITE setFromType)
    Q_PROPERTY(QString company READ company WRITE setCompany)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString tel READ tel WRITE setTel)
    Q_PROPERTY(QString email READ email WRITE setEmail)
    Q_PROPERTY(bool isRead READ isRead WRITE setRead NOTIFY isReadChanged)
    Q_PROPERTY(QString fromHeadImage READ fromHeadImage WRITE setFromHeadImage NOTIFY fromHeadImageChanged)
    Q_PROPERTY(QString lastInfo READ lastInfo WRITE setLastInfo NOTIFY lastInfoChanged)
    Q_PROPERTY(int unreadCount READ unreadCount WRITE setUnreadCount NOTIFY unreadCountChanged)
public:
    EbQuickSessionInfo(QQuickItem *parent = 0)
        : QQuickItem(parent)
        , m_time(0)
        , m_callId(0)
        , m_groupId(0)
        , m_memberId(0)
        , m_fromUserId(0)
        , m_fromPhone(0)
        , m_fromType(0)
        , m_isRead(true)
        , m_unreadCount(0)
    {
    }

signals:
    void fromHeadImageChanged();
    void lastInfoChanged();
    void unreadCountChanged();
    void groupFullNameChanged();
    void isReadChanged();
public:
    bool isMsgSession(void) const {return m_memberId==-1?true:false;}
    void setTime(qint64 v) {m_time=v;}
    qint64 time() const {return m_time;}
    void setTimeString(const QString &v) {m_timeString = v;}
    const QString &timeString() const {return m_timeString;}
    void setCallId(qint64 v) {m_callId=v;}
    qint64 callId() const {return m_callId;}
    QString callIdString() const {return QString::number(m_callId);}
    void setGroupId(qint64 v) {m_groupId=v;}
    qint64 groupId() const {return m_groupId;}
    QString groupIdString() const {return QString::number(m_groupId);}
    bool isGroupCall() const {return m_groupId>0?true:false;}
    void setGroupName(const QString &v) {m_groupName = v;}
    const QString &groupName() const {return m_groupName;}
    void setGroupFullName(const QString &v) {
        if (m_groupFullName != v) {
            m_groupFullName = v;
            emit groupFullNameChanged();
        }
    }
    const QString &groupFullName() const {return m_groupFullName;}
    void setMemberId(qint64 v) {m_memberId=v;}
    qint64 memberId() const {return m_memberId;}
    QString memberIdString() const {return QString::number(m_memberId);}
    void setFromUserId(qint64 v) {m_fromUserId=v;}
    qint64 fromUserId() const {return m_fromUserId;}
    QString fromUserIdString() const {return QString::number(m_fromUserId);}
    void setFromPhone(qint64 v) {m_fromPhone=v;}
    qint64 fromPhone() const {return m_fromPhone;}
    QString fromPhoneString() const {return QString::number(m_fromPhone);}
    void setFromAccount(const QString &v) {m_fromAccount = v;}
    const QString &fromAccount() const {return m_fromAccount;}
    void setFromName(const QString &v) {m_fromName = v;}
    const QString &fromName() const {return m_fromName;}
    void setFromType(int v) {m_fromType=v;}
    int fromType() const {return m_fromType;}
    void setCompany(const QString &v) {m_company = v;}
    const QString &company() const {return m_company;}
    void setTitle(const QString &v) {m_title = v;}
    const QString &title() const {return m_title;}
    void setTel(const QString &v) {m_tel = v;}
    const QString &tel() const {return m_tel;}
    void setEmail(const QString &v) {m_email = v;}
    const QString &email() const {return m_email;}
    void setRead(bool v) {
        if (m_isRead!=v) {
            m_isRead = v;
            emit isReadChanged();
        }
    }
    bool isRead() const {return m_isRead;}
    void setFromHeadImage(const QString &v) {
        if (m_fromHeadImage!=v) {
            m_fromHeadImage = v;
            emit fromHeadImageChanged();
        }
    }
    const QString &fromHeadImage() const {return m_fromHeadImage;}
    void setLastInfo(const QString &v) {
        if (m_lastInfo!=v) {
            m_lastInfo = v;
            emit lastInfoChanged();
        }
    }
    const QString &lastInfo() const {return m_lastInfo;}
    void setUnreadCount(int v) {
        if (m_unreadCount!=v) {
            m_unreadCount = v;
            emit unreadCountChanged();
        }
    }
    int unreadCount() const {return m_unreadCount;}

private:
    qint64 m_time;
    QString m_timeString;
    qint64 m_callId;
    qint64 m_groupId;
    QString m_groupName;
    QString m_groupFullName;
    qint64 m_memberId;
    qint64 m_fromUserId;
    qint64 m_fromPhone;
    QString m_fromAccount;
    QString m_fromName;
    int m_fromType;
    QString m_company;
    QString m_title;
    QString m_tel;
    QString m_email;
    bool m_isRead;
    QString m_fromHeadImage;
    QString m_lastInfo;
    int m_unreadCount;
};

class EbQuickRichInfo : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString msgId READ msgIdString)
//    Q_PROPERTY(qint64 msgId READ msgId WRITE setMsgId)
    Q_PROPERTY(int msgType READ msgType WRITE setMsgType)
    Q_PROPERTY(QString msgName READ msgName WRITE setMsgName)
//    Q_PROPERTY(qint64 msgSize READ msgSize WRITE setMsgSize)
    Q_PROPERTY(QString sendFromUserId READ sendFromUserIdString)
//    Q_PROPERTY(qint64 sendFromUserId READ sendFromUserId WRITE setSendFromUserId)
    Q_PROPERTY(QString sendFromName READ sendFromName WRITE setSendFromName)
    Q_PROPERTY(QString sendToUserId READ sendToUserIdString)
//    Q_PROPERTY(qint64 sendToUserId READ sendToUserId WRITE setSendToUserId)
    Q_PROPERTY(QString sendToName READ sendToName WRITE setSendToName)
    Q_PROPERTY(bool isPrivate READ isPrivate WRITE setPrivate)
    Q_PROPERTY(bool isReceive READ isReceive WRITE setReceive)
    Q_PROPERTY(bool isRead READ isRead WRITE setRead NOTIFY isReadChanged)

    Q_PROPERTY(qint64 time READ time WRITE setTime)
    Q_PROPERTY(QString timeString READ timeString)
    Q_PROPERTY(QString callId READ callIdString)
//    Q_PROPERTY(qint64 callId READ callId WRITE setCallId)
    Q_PROPERTY(QString groupId READ groupIdString)
//    Q_PROPERTY(qint64 groupId READ groupId WRITE setGroupId)
    Q_PROPERTY(bool isGroupCall READ isGroupCall)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(QString memberId READ memberIdString)
//    Q_PROPERTY(qint64 memberId READ memberId WRITE setMemberId)
    Q_PROPERTY(QString fromPhone READ fromPhoneString)
//    Q_PROPERTY(qint64 fromPhone READ fromPhone WRITE setFromPhone)
    Q_PROPERTY(QString fromAccount READ fromAccount WRITE setFromAccount)
    Q_PROPERTY(int fromType READ fromType WRITE setFromType)
    Q_PROPERTY(QString richText READ richText WRITE setRichText NOTIFY richTextChanged)
    Q_PROPERTY(int progressValue READ progressValue WRITE setProgressValue NOTIFY progressValueChanged)
    Q_PROPERTY(QString richDesc READ richDesc WRITE setRichDesc NOTIFY richDescChanged)
public:
    EbQuickRichInfo(QQuickItem *parent = 0)
        : QQuickItem(parent)
        , m_msgId(0)
        , m_msgType(0)
//        , m_msgSize(0)
        , m_sendFromUserId(0)
        , m_sendToUserId(0)
        , m_isPrivate(false)
        , m_isReceive(false)
        , m_receiptData(0)
        , m_isRead(true)

        , m_time(0)
        , m_callId(0)
        , m_groupId(0)
        , m_memberId(0)
        , m_fromPhone(0)
        , m_fromType(0)
        , m_progressValue(-1)
    {
    }
    EbQuickRichInfo(const CCrFileInfo *fileInfo, QQuickItem *parent = 0)
        : QQuickItem(parent)
        , m_msgId(fileInfo->m_nMsgId)
        , m_msgType(1)
        , m_msgName(fileInfo->m_sFileName)
//        , m_msgSize(fileInfo->m_nFileSize)
        , m_sendFromUserId(fileInfo->m_sSendFrom)
        , m_sendToUserId(fileInfo->m_sSendTo)
        , m_isPrivate(fileInfo->m_bPrivate)
        , m_isReceive(false)
        , m_receiptData(0)
        , m_isRead(true)

        , m_time(0)
        , m_callId(fileInfo->GetCallId())
        , m_groupId(0)
        , m_memberId(0)
        , m_fromPhone(0)
        , m_fromType(0)
        , m_progressValue(-1)
    {
    }
signals:
    void isReadChanged();
    void progressValueChanged();
    void richTextChanged();
    void richDescChanged();

public:
    void setMsgId(qint64 v) {m_msgId=v;}
    qint64 msgId() const {return m_msgId;}
    QString msgIdString() const {return QString::number(m_msgId);}
    void setMsgType(int v) {m_msgType=v;}
    int msgType() const {return m_msgType;}
    void setMsgName(const QString &v) {m_msgName = v;}
    const QString &msgName() const {return m_msgName;}
//    void setMsgSize(qint64 v) {m_msgSize=v;}
//    qint64 msgSize() const {return m_msgSize;}
    void setSendFromUserId(qint64 v) {m_sendFromUserId=v;}
    qint64 sendFromUserId() const {return m_sendFromUserId;}
    QString sendFromUserIdString() const {return QString::number(m_sendFromUserId);}
    void setSendFromName(const QString &v) {m_sendFromName = v;}
    const QString &sendFromName() const {return m_sendFromName;}
    void setSendToUserId(qint64 v) {m_sendToUserId=v;}
    qint64 sendToUserId() const {return m_sendToUserId;}
    QString sendToUserIdString() const {return QString::number(m_sendToUserId);}
    void setSendToName(const QString &v) {m_sendToName = v;}
    const QString &sendToName() const {return m_sendToName;}
    void setPrivate(bool v) {m_isPrivate=v;}
    bool isPrivate() const {return m_isPrivate;}
    void setReceive(bool v) {m_isReceive=v;}
    bool isReceive() const {return m_isReceive;}

    void setTime(qint64 v) {
        m_time = v;
        const QDateTime pTime = QDateTime::fromTime_t(m_time);
        m_timeString = pTime.toString("HH:mm");
    }
    qint64 time() const {return m_time;}
    const QString &timeString() {return m_timeString;}

    void setCallId(qint64 v) {m_callId=v;}
    qint64 callId() const {return m_callId;}
    QString callIdString() const {return QString::number(m_callId);}
    void setGroupId(qint64 v) {m_groupId=v;}
    qint64 groupId() const {return m_groupId;}
    QString groupIdString() const {return QString::number(m_groupId);}
    bool isGroupCall() const {return m_groupId>0;}
    void setGroupName(const QString &v) {m_groupName = v;}
    const QString &groupName() const {return m_groupName;}
    void setMemberId(qint64 v) {m_memberId=v;}
    qint64 memberId() const {return m_memberId;}
    QString memberIdString() const {return QString::number(m_memberId);}
    void setFromPhone(qint64 v) {m_fromPhone=v;}
    qint64 fromPhone() const {return m_fromPhone;}
    QString fromPhoneString() const {return QString::number(m_fromPhone);}
    void setFromAccount(const QString &v) {m_fromAccount = v;}
    const QString &fromAccount() const {return m_fromAccount;}
    void setFromType(int v) {m_fromType=v;}
    int fromType() const {return m_fromType;}
    void setRead(bool v) {
        if (m_isRead!=v) {
            m_isRead=v;
            emit isReadChanged();
        }
    }
    bool isRead() const {return m_isRead;}
    void setRichText(const QString &v) {
        if (m_richText != v) {
            m_richText = v;
            emit richTextChanged();
        }
    }
    const QString &richText() const {return m_richText;}
    void setProgressValue(int v) {
        if (m_progressValue!=v) {
            m_progressValue=v;
            emit progressValueChanged();
        }
    }
    int progressValue() const {return m_progressValue;}
    void setRichDesc(const QString &v) {
        if (m_richDesc!=v) {
            m_richDesc = v;
            emit richDescChanged();
        }
    }
    const QString &richDesc() const {return m_richDesc;}
private:
    qint64 m_msgId;
     /// 0=rich,1=file,2=image(?),3=wav-audio
     /// 4=个人名片
    int m_msgType;
    /// 配合 m_msgType使用，如 1=filename
    QString m_msgName;
//    qint64 m_msgSize;   /// filesize
    qint64 m_sendFromUserId;
    QString m_sendFromName;
    qint64 m_sendToUserId;
    QString m_sendToName;
    bool m_isPrivate;
    bool m_isReceive;
    int m_receiptData;

    qint64 m_time;
    QString m_timeString;
    qint64 m_callId;
    qint64 m_groupId;
    QString m_groupName;
    qint64 m_memberId;
    qint64 m_fromPhone;
    QString m_fromAccount;
    int m_fromType;
    bool m_isRead;
    QString m_richText;
    int m_progressValue; /// 进度条， -1,0-100 -1=不显示
    QString m_richDesc;
};


//class EbQuickFileInfo : public QQuickItem
//{
//    Q_OBJECT
//    Q_PROPERTY(qint64 resourceId READ resourceId WRITE setResourceId)
//    Q_PROPERTY(qint64 msgId READ msgId WRITE setMsgId)
//    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
//    Q_PROPERTY(qint64 fileSize READ fileSize WRITE setFileSize)
//    Q_PROPERTY(QString fileTime READ fileTime WRITE setFileTime)
//    Q_PROPERTY(bool isOffFile READ isOffFile WRITE setOffFile)
//    Q_PROPERTY(qint64 sendFromUserId READ sendFromUserId WRITE setSendFromUserId)
//    Q_PROPERTY(QString sendFromName READ sendFromName WRITE setSendFromName)
//    Q_PROPERTY(qint64 sendToUserId READ sendToUserId WRITE setSendToUserId)
//    Q_PROPERTY(QString sendToName READ sendToName WRITE setSendToName)
//    Q_PROPERTY(bool isPrivate READ isPrivate WRITE setPrivate)
//    Q_PROPERTY(bool isReceive READ isReceive WRITE setReceive)
//    Q_PROPERTY(qint64 callId READ callId WRITE setCallId)
//    Q_PROPERTY(qint64 groupId READ groupId WRITE setGroupId)
//    Q_PROPERTY(bool isGroupCall READ isGroupCall)
//public:
//    EbQuickFileInfo(QQuickItem *parent = 0)
//        : QQuickItem(parent)
//        , m_resourceId(0)
//        , m_msgId(0)
//        , m_fileSize(0)
//        , m_offFile(false)
//        , m_sendFromUserId(0)
//        , m_sendToUserId(0)
//        , m_isPrivate(false)
//        , m_isReceive(false)
//        , m_callId(0)
//        , m_groupId(0)
//    {
//    }
//    EbQuickFileInfo(const CCrFileInfo *fileInfo, QQuickItem *parent = 0)
//        : QQuickItem(parent)
//        , m_resourceId(fileInfo->m_sResId)
//        , m_msgId(fileInfo->m_nMsgId)
//        , m_fileName(fileInfo->m_sFileName)
//        , m_fileSize(fileInfo->m_nFileSize)
//        , m_offFile(fileInfo->m_bOffFile)
//        , m_sendFromUserId(fileInfo->m_sSendFrom)
//        , m_sendToUserId(fileInfo->m_sSendTo)
//        , m_isPrivate(fileInfo->m_bPrivate)
//        , m_isReceive(false)
//        , m_callId(fileInfo->GetCallId())
//        , m_groupId(0)
//    {
//    }
//signals:

//public:
////    CEBString m_sResCmServer;
////    CEBString m_sResCmAppName;
////    cr::bigint m_sReceiveAccount;
////    CEBString m_sMD5;

//    void setResourceId(qint64 v) {m_resourceId=v;}
//    qint64 resourceId() const {return m_resourceId;}
//    void setMsgId(qint64 v) {m_msgId=v;}
//    qint64 msgId() const {return m_msgId;}
//    void setFileName(const QString &v) {m_fileName = v;}
//    const QString &fileName() const {return m_fileName;}
//    void setFileSize(qint64 v) {m_fileSize=v;}
//    qint64 fileSize() const {return m_fileSize;}
//    void setFileTime(const QString &v) {m_fileTime = v;}
//    const QString &fileTime() const {return m_fileTime;}
//    void setOffFile(bool v) {m_offFile=v;}
//    bool isOffFile() const {return m_offFile;}
//    void setSendFromUserId(qint64 v) {m_sendFromUserId=v;}
//    qint64 sendFromUserId() const {return m_sendFromUserId;}
//    void setSendFromName(const QString &v) {m_sendFromName = v;}
//    const QString &sendFromName() const {return m_sendFromName;}
//    void setSendToUserId(qint64 v) {m_sendToUserId=v;}
//    qint64 sendToUserId() const {return m_sendToUserId;}
//    void setSendToName(const QString &v) {m_sendToName = v;}
//    const QString &sendToName() const {return m_sendToName;}
//    void setPrivate(bool v) {m_isPrivate=v;}
//    bool isPrivate() const {return m_isPrivate;}
//    void setReceive(bool v) {m_isReceive=v;}
//    bool isReceive() const {return m_isReceive;}

////    void setTime(qint64 v) {
////        m_time = v;
////        const QDateTime pTime = QDateTime::fromTime_t(m_time);
////        m_timeString = pTime.toString("HH:mm");
////    }
////    qint64 time() const {return m_time;}
////    const QString &timeString() {return m_timeString;}

//    void setCallId(qint64 v) {m_callId=v;}
//    qint64 callId() const {return m_callId;}
//    void setGroupId(qint64 v) {m_groupId=v;}
//    qint64 groupId() const {return m_groupId;}
//    bool isGroupCall() const {return m_groupId>0;}

//private:
//    qint64 m_resourceId;
//    qint64 m_msgId;
//    QString m_fileName;
//    qint64 m_fileSize;
//    QString m_fileTime;
//    bool m_offFile;
//    qint64 m_sendFromUserId;
//    QString m_sendFromName;
//    qint64 m_sendToUserId;
//    QString m_sendToName;
//    bool m_isPrivate;
//    bool m_isReceive;
////    int m_receiptData;
////    qint64 m_time;
////    QString m_timeString;
//    qint64 m_callId;
//    qint64 m_groupId;

//};

#endif // EBQUICKITEMINFO_H
