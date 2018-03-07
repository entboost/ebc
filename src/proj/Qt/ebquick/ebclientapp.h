#ifndef EBCLIENTAPP_H
#define EBCLIENTAPP_H

#include <QObject>
#include <QQuickTextDocument>
//#include <QQuickPaintedItem>
//#include <QQuickImageProvider>
//#include <QQuickImageResponse>
//#include <QDeclarativeItem>
#include "../entboost/ebdefines.h"
#include "../entboost/ebclientappbase.h"
#include "../entboost/ebclocales.h"
#include "ebquickiteminfo.h"
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidActivityResultReceiver>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#endif
#include "ebccallinfo.h"
#include "ebvoicerecorder.h"

//class EbMyGroupInfoDataModel : public QAbstractListModel
//{
//    Q_OBJECT
//public:
//    enum EbMyGroupInfoDataRoles {
//        TypeRole = Qt::UserRole + 1,
//        SizeRole
//    };

//    EbMyGroupInfoDataModel(QObject *parent = 0)
//        : QAbstractListModel(parent)
//    {
//        EB_GroupInfo groupInfo;
//        groupInfo.m_sGroupName = "groupName1";
//        m_data.append(new EbQuickGroupInfo(&groupInfo));
//    }
//    void setMyGroupInfoModelData(const EB_GroupInfo *groupInfo)
//    {
//        for (int i=0; i<m_data.size(); i++) {
//            const EB_GroupInfo * quickGroupInfo = m_data.at(i);
//            if (quickGroupInfo->m_sGroupCode==groupInfo->m_sGroupCode) {
//                return;
//            }
//        }
//        m_data.append(new EB_GroupInfo(groupInfo));
////        QModelIndex topLeft;
////        QModelIndex bottomRight;
////        emit modelReset(QPrivateSignal());
//    }


//    virtual QHash<int, QByteArray> roleNames() const {
//         QHash<int, QByteArray> roles;
//         roles[TypeRole] = "groupInfo";
//         return roles;
//     }
////    Q_INVOKABLE virtual QModelIndex index(int row, int column,
////                              const QModelIndex &parent = QModelIndex()) {

////    }

////    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const = 0;

////    Q_INVOKABLE virtual QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
////    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const = 0;
////    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const = 0;
////    Q_INVOKABLE virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

////    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const = 0;

//    Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
//        const EB_GroupInfo *groupInfo = m_data.at(index.row());
//        if (groupInfo==0) {
//            return QVariant();
//        }
//        return QVariant::fromValue(new EbQuickGroupInfo(groupInfo));

////        QVariant value;

////        if (index.isValid()) {
////            if (role < Qt::UserRole) {
//////                value = QSqlQueryModel::data(index, role);
////            } else {
////                const int columnIdx = role - Qt::UserRole - 1;
//////                QModelIndex modelIndex = this->index(index.row(), columnIdx);
//////                value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
////            }
////        }
////        return value;

////        if (index.row() < 0 || index.row() >= m_data.size()) return QVariant();
//////        Data::const_iterator iter = m_data.constBegin() + index.row();

//////        switch (role) {
//////        case WordRole:
//////            return iter.key();
//////        case CountRole:
//////            return iter.value();
//////        }
////        return QVariant();
//    }

//    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent) const {
//        Q_UNUSED(parent)
//        return m_data.size();
//    }
////    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) {return 1;}
//    Q_INVOKABLE virtual int count() const {
//        return m_data.size();
//    }

//    QList<EB_GroupInfo*> m_data;
//};

class EbClientApp : public EbClientAppBase
        #ifdef Q_OS_ANDROID
        , public QAndroidActivityResultReceiver
        #endif
        , public CEBSearchCallback
{
    Q_OBJECT
    Q_PROPERTY(QString currentChatGroupId READ currentChatGroupIdString WRITE setCurrentChatGroupIdString)
    Q_PROPERTY(QString currentChatUserId READ currentChatUserIdString WRITE setCurrentChatUserIdString)
    Q_PROPERTY(QString myHeadImage READ myHeadImage NOTIFY myHeadImageChanged)
public:
    explicit EbClientApp(QObject *parent = nullptr);
    virtual ~EbClientApp(void);

    void setLocales(const EbcLocales *v) {m_locales=v;}
    Q_INVOKABLE bool setDevAppId(void);
    Q_INVOKABLE bool login(const QString &account, const QString &password, const QString &savePassword);
    Q_INVOKABLE bool loginVisitor(void);
    Q_INVOKABLE void logout(bool clearSavePassword, bool acceptPush);
    Q_INVOKABLE int loadLoginData(void);
    Q_INVOKABLE QString deleteLoginData(const QString &accountInput, const QString &accountReal, qint64 userId);
    Q_INVOKABLE bool registerAccount(const QString &account,
                                     const QString &password,
                                     const QString &userName="",
                                     const QString &enterpriseName="");
    Q_INVOKABLE int loadOrgData(void);
    Q_INVOKABLE int loadContactData(void);
    Q_INVOKABLE bool isEnterpriseGroupType(int groupType) const {return groupType<=EB_GROUP_TYPE_PROJECT?true:false;}
    Q_INVOKABLE QString enterpriseName(qint64 enterpriseId) const;
    Q_INVOKABLE int groupType(qint64 groupId) const;
    Q_INVOKABLE QString groupName(qint64 groupId) const;
    Q_INVOKABLE QString groupFullName(qint64 groupId, const QString &groupName, int memberSize) const;
    Q_INVOKABLE QString groupFullName(qint64 groupId) const;
//    Q_INVOKABLE QString groupFullName(qint64 groupId, const QString &groupName) const;
    Q_INVOKABLE QString groupImage(qint64 groupId) const;
    Q_INVOKABLE bool hasSubGroup(qint64 groupId) const;
    Q_INVOKABLE int subGroupCount(qint64 groupId) const;
    Q_INVOKABLE bool isGroupAdminLevel(qint64 groupId) const;
    Q_INVOKABLE bool isGroupAdminLevel(qint64 groupId, qint64 memberUserId) const;
    Q_INVOKABLE bool isGroupForbidSpeech(qint64 groupId) const;
    Q_INVOKABLE int setGroupForbidSpeech(qint64 groupId, bool forbidSpeech);
    Q_INVOKABLE bool canEditGroupInfo(qint64 groupId) const;
    Q_INVOKABLE bool canNewGroupInfo(qint64 enterpriseId) const;
    Q_INVOKABLE int editGroupInfo(EbQuickGroupInfo *groupInfo);
    Q_INVOKABLE int newGroupInfo(int groupType, const QString &groupName, qint64 enterpriseId, qint64 parentId);
    Q_INVOKABLE bool canDeleteGroupInfo(qint64 groupId) const;
    Q_INVOKABLE int deleteGroupInfo(qint64 groupId);
    Q_INVOKABLE bool canExitGroupInfo(qint64 groupId, int groupType) const;
    Q_INVOKABLE int exitGroupInfo(qint64 groupId);
    Q_INVOKABLE bool canEditMemberAdminLevel(qint64 groupId, qint64 memberUserId) const;
    Q_INVOKABLE int addMemberAdminLevel(qint64 groupId, qint64 memberUserId);
    Q_INVOKABLE int deleteMemberAdminLevel(qint64 groupId, qint64 memberUserId);
    Q_INVOKABLE bool canEditMemberInfo(qint64 groupId, qint64 memberUserId) const;
    Q_INVOKABLE bool isGroupMember(qint64 groupId, int userId) const;
    Q_INVOKABLE bool canNewMemberInfo(qint64 groupId) const;
    Q_INVOKABLE int deleteMemberInfo(qint64 groupId, qint64 memberUserId);
    Q_INVOKABLE int editMemberInfo(EbQuickMemberInfo *memberInfo);
    Q_INVOKABLE EbQuickMemberInfo *memberInfo(qint64 groupId, qint64 userId);
    Q_INVOKABLE EbQuickMemberInfo *memberInfoByMemberId(qint64 memberId);
    Q_INVOKABLE EbQuickMemberInfo *defaultMemberInfo(qint64 userId);
    Q_INVOKABLE EbQuickMemberInfo *buildNewMemberInfo(qint64 groupId, const QString &account, const QString &userName);
    Q_INVOKABLE EbQuickGroupInfo *groupQuickInfo(qint64 groupId, bool loadMember);
    Q_INVOKABLE int requestLoadGroupMember(qint64 groupId);
    Q_INVOKABLE void loadGroupMemberInfo(qint64 groupId, int parameter=0);
    Q_INVOKABLE int loadSubGroupInfo(qint64 groupId, int parameter=0);
    Q_INVOKABLE QString userName(qint64 groupId, qint64 userId) const;
    Q_INVOKABLE QString userNameByUserId(qint64 userId, qint64 callId=0) const;
    Q_INVOKABLE QString userNameByAccount(const QString &account) const;
    Q_INVOKABLE bool hasUGContact(qint64 ugId) const;
    Q_INVOKABLE int ugContactCount(qint64 ugId) const;
    Q_INVOKABLE EbQuickUGInfo *buildUGInfo(qint64 ugId, const QString &groupName);
    Q_INVOKABLE QString ugFullName(qint64 ugId, const QString &groupName) const;
    Q_INVOKABLE QString memberHeadImage(EbQuickMemberInfo *memberInfo) const;
    Q_INVOKABLE QString contactHeadImage(EbQuickContactInfo *contactInfo) const;
    Q_INVOKABLE QString userHeadImage2(qint64 userId, const QString &resourceFile) const;
    Q_INVOKABLE QString userHeadImage(qint64 userId) const;
    Q_INVOKABLE int loadUGContact(qint64 ugId);
    Q_INVOKABLE bool compareContactInfo(EbQuickContactInfo *contactInfo1, EbQuickContactInfo *contactInfo2) const;
    Q_INVOKABLE int editUGInfo(qint64 ugId, const QString &groupName);
    Q_INVOKABLE int deleteUGInfo(qint64 ugId);
    Q_INVOKABLE int editContactInfo(EbQuickContactInfo *contactInfo);
    Q_INVOKABLE int deleteContactInfo(qint64 contactId);
    Q_INVOKABLE int moveContactToByContactId(qint64 contactId, qint64 ugId);
    Q_INVOKABLE EbQuickContactInfo *getContactInfoByUserId(qint64 userId);
    Q_INVOKABLE int loadAllUGInfo(int parameter=0);
    Q_INVOKABLE int loadSubscribeFuncInfo(int location, int parameter=0);
    Q_INVOKABLE QString subscribeFuncUrl(qint64 subId, qint64 groupId=0, const QString &sParameters="");
    Q_INVOKABLE bool isExistSubscribeFunc(qint64 subId) const;
    Q_INVOKABLE EbQuickSubscribeFuncInfo *subscribeFuncInfo(qint64 subId);
    int openSubId(const QString &url);
    Q_INVOKABLE int requestUrl(const QString &url);
    Q_INVOKABLE EbQuickAccountInfo *myAccountInfo(void) const;
    Q_INVOKABLE QString myHeadImage(void);
    Q_INVOKABLE int setPassword(const QString &currentPassword, const QString &newPassword);
    Q_INVOKABLE void playTone(void);
    Q_INVOKABLE void vibrate(int ms=500);
    Q_INVOKABLE QString accountHeadImage(EbQuickAccountInfo *account) const;
    Q_INVOKABLE int setMyAccountInfo(EbQuickAccountInfo *account);
    Q_INVOKABLE int loadAreaInfo(int parentId, int parameter=0);
    Q_INVOKABLE int loadHeadEmotionInfo(int parameter=0);
    Q_INVOKABLE int loadMyEmotionInfo(int parameter=0);
    Q_INVOKABLE int setMyGroupHeadImageResourceId(qint64 groupId, qint64 resourceId);
    Q_INVOKABLE int setMyGroupHeadImageFile(qint64 groupId, const QString &imageFile);
#ifdef Q_OS_ANDROID
    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data);
    QString getLocalPathFromUri(const QAndroidJniObject& uri);
#endif
//    void openGalleryFinished(const QString &uriFile);
    int m_openGalleryParameter;
    Q_INVOKABLE void openGallery(int parameter);
    Q_INVOKABLE int loadSessionInfo(int nLoadCount, int parameter);
    Q_INVOKABLE void setSessionIsRead(qint64 callId);
    Q_INVOKABLE int callAccount(const QString &account, qint64 existCallId=0);
    Q_INVOKABLE int callUserId(qint64 userId, qint64 existCallId=0);
    Q_INVOKABLE int callGroupId(qint64 groupId);
    Q_INVOKABLE EbQuickCallInfo *findCallInfo(qint64 groupId, qint64 userId, const QString &account);
    bool insertImage(QString &pOutRichText, const QString &filePath, const QString &alt, int maxWidth=320, int maxHeight=240) const;
    static bool ChangeTime(const char* sTimeString, time_t& pOutTime, int* pOutMS=0);
    void writeFileMessage(QString &richText, qint64 resourceId, const QString &filePath, eb::bigint fileSizeOrg);
    bool writeCardDataMessage(QString &richText, const char *cardData, QString *pOutUserName=0);
    Q_INVOKABLE int loadRichInfo(qint64 groupId, qint64 fromUserId);
    Q_INVOKABLE int sendText(qint64 callId, const QString &text, qint64 toUserId=0, bool bPrivate=false);
    bool formatInputMsg(QQuickTextDocument *textDocument, EB_ChatRoomRichMsg *pOutMsgFormat) const;
    Q_INVOKABLE void notifyMaxWidth(int maxWidth);
//    Q_INVOKABLE void notifyCurrentChatInfo(qint64 currentChatGroupId, qint64 currentChatUserId);
    void setCurrentChatGroupId(qint64 v);
    void setCurrentChatGroupIdString(const QString &v);
    qint64 currentChatGroupId(void) {return m_currentChatGroupId;}
    QString currentChatGroupIdString(void) {return QString::number(m_currentChatGroupId);}
    void setCurrentChatUserId(qint64 v);
    void setCurrentChatUserIdString(const QString &v);
    qint64 currentChatUserId(void) {return m_currentChatUserId;}
    QString currentChatUserIdString(void) {return QString::number(m_currentChatUserId);}
    Q_INVOKABLE int sendRich(qint64 callId, QQuickTextDocument *textDocument, qint64 toUserId=0, bool bPrivate=false);
    Q_INVOKABLE int sendImage(qint64 callId, const QString &imageFile, qint64 toUserId=0, bool bPrivate=false);
    Q_INVOKABLE int sendVoice(qint64 callId, const QString &voiceFile, qint64 toUserId=0, bool bPrivate=false);
    Q_INVOKABLE int sendFile(qint64 callId, const QString &filePath, qint64 toUserId=0, bool bPrivate=false);
    Q_INVOKABLE int sendGroupTempFile(qint64 groupId, const QString &filePath);
    Q_INVOKABLE int sendUserIdEChard(qint64 callId, qint64 userId);
    Q_INVOKABLE int sendUserAccountEChard(qint64 callId, const QString &account);
    Q_INVOKABLE EbQuickECardInfo *parserECardInfo(const QString &cardData) const;
    Q_INVOKABLE int cancelFile(qint64 callId, qint64 msgId);
    Q_INVOKABLE int saveFile(qint64 callId, qint64 msgId, const QString &fileName);
    Q_INVOKABLE int call2Group(qint64 callId, const QString &account);
    Q_INVOKABLE bool isImageFile(const QString &filePath) const;
    Q_INVOKABLE void clearUnreadCount(qint64 groupId, qint64 fromUserId);
    Q_INVOKABLE bool insertEmotion(QQuickTextDocument *textDocument, EbQuickEmotionInfo *emotionInfo);
    Q_INVOKABLE bool voiceRecordStart(int maxSeconds=30);
    Q_INVOKABLE bool isVoiceRecordStarted(void) const;
    Q_INVOKABLE int voiceRecordStop(qint64 sendCallId);
    Q_INVOKABLE void playWavFile(const QString &filePath);
    Q_INVOKABLE void openFile(const QString &filePath);
//    Q_INVOKABLE QString getOpenFileUrl(const QString &filePath);
    Q_INVOKABLE bool isCanWithdrawMessage(qint64 msgId) const;
    Q_INVOKABLE int withdrawMessage(qint64 callId, qint64 msgId);
    Q_INVOKABLE bool isCanForwardMessage(qint64 msgId) const;
    Q_INVOKABLE void forwardMessageToGroupId(qint64 groupId, qint64 msgId);
    Q_INVOKABLE void forwardMessageToUserId(qint64 userId, qint64 msgId);
    Q_INVOKABLE void forwardMessageToAccount(const QString &account, qint64 msgId);
    Q_INVOKABLE int forwardMessage(qint64 callId, qint64 msgId);
    Q_INVOKABLE bool isCanCopyMessage(qint64 msgId) const;
    Q_INVOKABLE bool copyMessage(qint64 msgId);
    Q_INVOKABLE bool isCanCollectMessage(qint64 msgId) const;
    Q_INVOKABLE int collectMessage(qint64 callId, qint64 msgId);
    Q_INVOKABLE void ackMessage(qint64 msgId, int akcType);
    Q_INVOKABLE QString conversationsUrl(qint64 groupId, qint64 fromUserId);
    Q_INVOKABLE void deleteChat(qint64 callId, qint64 groupId, qint64 fromUserId);
    int m_searchMode;
    Q_INVOKABLE void search(const QString &searchKey, int searchMode, int parameter);
    /// CEBSearchCallback
    virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo, unsigned long dwParam);
    virtual void onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam);
    virtual void onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam);
    /// 检查版本
    EbHttpFileDownload m_versionFileDownload;
    Q_INVOKABLE void checkVersion(void);
    Q_INVOKABLE void sendNotify(int messageId, const QString &title, const QString &text);
    /// 消息推送
    Q_INVOKABLE void getManufacturerName();
    Q_INVOKABLE void getPushToken();

//    Q_INVOKABLE int loadAllUGInfo(int parameter=0);
//    Q_INVOKABLE EbQuickContactInfo *buildNewContactInfo(qint64 ugId);

//    Q_INVOKABLE QQmlListProperty<EbQuickMemberInfo> groupMemberInfoList(qint64 groupId);
//    Q_INVOKABLE QString groupImageFile(const EbQuickGroupInfo *groupInfo);

signals:
    /// 组织架构
    void receivedEnterpriseInfo(EbQuickEnterpriseInfo *enterpriseInfo);
    void receivedGroupInfo(EbQuickGroupInfo *groupInfo);
//    void receivedMyGroupInfo(EbQuickGroupInfo *groupInfo, bool newData, int index, int moveToIndex);
    void receivedSubGroupInfo(EbQuickGroupInfo *subGroupInfo, const QString &parentGroupId, int parameter);
    void deletedGroupInfo(EbQuickGroupInfo *groupInfo, const QString &tip);
    void deletedGroupId(const QString &groupId);
//    void deletedMyGroupInfo(qint64 groupId, int index);
    void receivedMemberInfo(EbQuickMemberInfo *memberInfo);
//    void receivedMyGroupMemberInfo(EbQuickMemberInfo *memberInfo, bool isChangeLineState);
    void memberExitGroup(EbQuickGroupInfo *groupInfo, EbQuickMemberInfo *memberInfo);
    void memberRemoveGroup(EbQuickGroupInfo *groupInfo, EbQuickMemberInfo *memberInfo);
    void receivedUGInfo(EbQuickUGInfo *ugInfo);
    void ugInfoDeleted(EbQuickUGInfo *ugInfo);
    void loadUGInfoResult(EbQuickUGInfo *ugInfo, int parameter);
    void receivedContactInfo(EbQuickContactInfo *contactInfo);
    void receivedUserHeadImage(const QString &userId, const QString &headImage);
    void contactLineStateChanged(EbQuickContactInfo *contactInfo);
    void contactDeleted(EbQuickContactInfo *contactInfo);
    void loadedGroupMemberInfo(EbQuickMemberInfo *memberInfo, int parameter);
    void groupEditResponse(EbQuickGroupInfo *groupInfo, int stateCode, const QString &stateString);
    void memberEditResponse(EbQuickMemberInfo *memberInfo, int stateCode, const QString &stateString);
    void loadSubscribeFuncInfoResult(EbQuickSubscribeFuncInfo *funcInfo, int parameter);
    void setPasswordResponse(int stateCode, const QString &stateString);
    void myHeadImageChanged(const QString &imageUrl);
    void setAccountInfoResponse(int stateCode, const QString &stateString);
    void loadAreaInfoResponse(EbQuickAreaInfo *areaInfo, int parameter);
    void loadHeadEmotionInfoResponse(EbQuickEmotionInfo *emotionInfo, int parameter);
    void loadMyEmotionInfoResponse(EbQuickEmotionInfo *emotionInfo, int parameter);
    void openGalleryResponse(const QString &uriFile, int parameter);
    void loadSessionInfoResponse(EbQuickSessionInfo *sessionInfo, int parameter);
    void receivedSessionLastInfo(const QString &groupId, const QString &fromUserId, QString lastInfo);
    void clearSessionUnreadCount(const QString &groupId, const QString &fromUserId);
    void receivedCallConnected(EbQuickCallInfo *callInfo);
    void receivedRichInfo(EbQuickRichInfo *richInfo, const QString &updateMsgId);
    void fileCancel(const QString &groupId, const QString &fromUserId, const QString &msgId);
    void fileSent(const QString &groupId, const QString &fromUserId, const QString &msgId);
    void fileReceived(const QString &groupId, const QString &fromUserId, const QString &msgId, const QString &richText, const QString &filePath);
    void filePercent(const QString &groupId, const QString &fromUserId, const QString &msgId, int percent, const QString &richDesc);
    void displayMessageTip(const QString &groupId, const QString &fromUserId, const QString &messageTip);
    void receivedMsgReceiptData(const QString &callId, const QString &msgId, const QString &fromUserId, int ackType, int stateCode);
//    void sendingFile(EbQuickFileInfo *fileInfo);
    void memberLineStateChanged(EbQuickMemberInfo *memberInfo);
    void searchResultFuncInfo(EbQuickSubscribeFuncInfo *funcInfo, int parameter);
    void searchResultGroupInfo(EbQuickGroupInfo *groupInfo, int parameter);
    void searchResultMemberInfo(EbQuickGroupInfo *groupInfo, EbQuickMemberInfo *memberInfo, int parameter);
    void searchResultContactInfo(EbQuickContactInfo *contactInfo, int parameter);
    /// 消息
    void receivedSubMsg(const QString &msgId, const QString &msgSubId, const QString &msgTitle, const QString &msgContent, const QString &parameter);
    void receivedBcMsg(const QString &msgId, const QString &msgSubId, const QString &msgTitle, const QString &msgContent, const QString &parameter);
    /// Url request
    void requestUrlCallUserId(const QString &userId);
    void requestUrlCallUserAccount(const QString &account);
    void requestUrlCallGroup(const QString &groupId);
    void requestUrlRedirect(const QString &url);
    void requestUrlOpenSubId(const QString &subId, const QString &parameter);

    /// APPID
    void appidSuccess(void);
    void appidError(int errorCode, const QString &errorString);
    /// LOGIN 登录
    void loginSuccess(EbQuickAccountInfo *accountInfo);
    void loginError(int errorCode, const QString &errorString);
    void registerSuccess(const QString &account, const QString &userId, const QString &tip);
    void registerError(int errorCode, const QString &errorString);
    /// 0=已经在其他地方登录，退出当前连接；
    /// 1=修改密码，退出当前连接；
    void logonAnother(int type);
    void loginRecordInfo(const QString &accountInput, const QString &accountReal, qint64 userId, const QString &savePassword);

public slots:
    void onForwardMessage(void);

protected:
    virtual void timerEvent(QTimerEvent *e);
    virtual void customEvent(QEvent *e);
    void onAppIdSuccess(QEvent *e);
    void onAppIdError(QEvent *e);
    void onRegisterResponse(QEvent *e);

    void onBroadcastMsg(QEvent *e);
    void onAreaInfo(QEvent *e);

    void onUserStateChange(QEvent *e);
    void checkMyHeadImageChange(const EB_MemberInfo *memberInfo);
    void onMemberHeadChange(QEvent *e);
    void onContactHeadChange(QEvent *e);
    /// 视频聊天
    void onVRequestResponse(QEvent *e);
    void onVAckResponse(QEvent *e);
    void onVideoRequest(QEvent *e);
    void onVideoAccept(QEvent *e);
    void onVideoReject(QEvent *e);
    void onVideoTimeout(QEvent *e);
    void onVideoClose(QEvent *e);
    /// 聊天消息
    void onMsgReceipt(QEvent *e);
    bool isCurrentCallInfo(const EbcCallInfo::pointer &pEbCallInfo) const;
    void sendMessageTip(const EbcCallInfo::pointer &pEbCallInfo, const QString &messageTip);
    void sendRichInfo(EbQuickRichInfo *richInfo);
    void writeVoiceMessage(QString &richText, const QString &voiceFile);
    void checkSaveCallInfo(const EbcCallInfo::pointer &pEbCallInfo);
    void processRichInfo(const EbcCallInfo::pointer &pEbCallInfo, const CCrRichInfo *pCrMsgInfo, bool receive, bool saveHistory);
    void onSendRich(QEvent *e);
    /// 0=不处理
    /// 1=切换后台，需要 sendNotify
    int checkMessageNotify(void);
    bool onReceiveRich(QEvent *e);
    tstring GetFileName(const tstring & sPathName);
    QString getFileText(const CCrFileInfo *fileInfo);
    void onSendingFile(QEvent *e);
    void onSentFile(QEvent *e);
    void onCancelFile(QEvent *e);
    bool onReceivingFile(QEvent *e);
    void onReceivedFile(QEvent *e);
    void onFilePercent(QEvent *e);
    void onSave2Cloud(QEvent *e);
    /// 聊天会话
    void onCallConnected(QEvent *e);
    void onCallError(QEvent *e);
    void onCallHangup(QEvent *e);
    void onCallAlerting(QEvent *e);
    void onCallIncoming(QEvent *e);
    /// 联系人
    void onUGInfo(QEvent *e);
    void onUGDelete(QEvent *e);
    void onContactDelete(QEvent *e);
    void onContactInfo(QEvent *e);
    void onContactStateChanged(QEvent *e);
    void onAcceptAddContact(QEvent *e);
    void onRejectAddContact(QEvent *e);
    void onRequestAddContact(QEvent *e);
    /// 组织结构
    void onEditInfoResponse(QEvent *e);
    void onMemberEditResponse(QEvent *e);
    void onMemberDelete(QEvent *e);
    void setWindowTitleAndTrayInfo(void);
    void onMemberInfo(QEvent *e);
    void onRejectAdd2Group(QEvent *e);
    void onInviteAdd2Group(QEvent *e);
    void onRequestAdd2Group(QEvent *e);
    void onExitGroup(QEvent *e);
    void onRemoveGroup(QEvent *e);
    void onGroupEditResponse(QEvent *e);
    void onGroupDelete(QEvent *e);
//    void deleteMyGroupInfoModelData(qint64 groupId);
//    void setMyGroupInfoModelData(const EB_GroupInfo *groupInfo);
    void onGroupInfo(QEvent *e);
    void onEnterpriseInfo(QEvent *e);
    /// 登录
    void onLogonSuccess(QEvent *e);
    void onLogonError(QEvent *e);
    void onOnlineAnother(QEvent *e);

    void setTimeString(EbQuickSessionInfo *sessionInfo);
    QString getSessionLastInfo(eb::bigint groupId, eb::bigint fromUserId);
    int getSessionUnreadCount(eb::bigint groupId, eb::bigint fromUserId);
    void sendSessionInfo(EbQuickSessionInfo *sessionInfo, int parameter=0);
    tstring getFromAccountName(const EB_AccountInfo &fromAccountInfo) const;
    void saveCallRecord(eb::bigint callId, eb::bigint groupId, const EB_AccountInfo &fromAccountInfo, bool bSendSessionInfo);

private:
    EB_STATE_CODE m_errorCode;
    QString m_errorString;
    /// 请求登录
    QString m_sRequestAccount;
    QString m_sRequestPassword;
    /// for register 请求注册
    CEBAppClient m_registerEbUm;
    QString m_registerAccount;
    eb::bigint m_registerUserId;
    const EbcLocales *m_locales;
    QString m_myHeadImageFile;
    mycp::tstring m_myHeadImageMd5;
    qint64 m_myHeadResourceId;
    QString m_previewImage;
    /// 数据模型 model
    int m_timerIdCheckState;
    int m_timerIdGetPushToken;
    QString m_pushToken;
//    CLockList<EbSharedGroupInfo::pointer> m_myGroupInfoDataList;
    CLockMap<eb::bigint, EbcCallInfo::pointer> m_pCallList;
    CLockMap<eb::bigint, bool> m_pAutoCallGroupIdList;      /// groupId->
    CLockMap<eb::bigint, bool> m_pAutoCallFromUserIdList;   /// userId->
    ///
    int m_maxWidth;
    /// 记录当前聊天窗口
    qint64 m_currentChatGroupId;
    qint64 m_currentChatUserId;
    QDateTime m_tLastMsgDayTime;
    ///
    EbVoiceRecorder m_voiceRecorder;
    time_t m_tWaitCall2Group;
    /// 转发消息
    qint64 m_forwardMsgId;
    time_t m_forwardTime;
    qint64 m_forwardCallId;
    qint64 m_forwardUserId;
    QString m_forwardAccount;
    qint64 m_forwardGroupId;
};

#endif // EBCLIENTAPP_H
