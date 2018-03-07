#include <QHostAddress> /// **必须放在最前面
#include "ebclientapp.h"
//#include <socket.h>
#include <QSound>
#include <QMovie>
#include <QGuiApplication>
//#ifdef Q_OS_ANDROID
//#include <jni.h>
//#endif

const int theMaxDatabaseTextSize = 6000;

EbClientApp::EbClientApp(QObject *parent) : EbClientAppBase(parent)
  , m_errorCode(EB_STATE_OK)
  , m_registerUserId(0)
  , m_locales(0)
  , m_myHeadResourceId(0)
  , m_timerIdCheckState(0)
  , m_timerIdGetPushToken(0)
  , m_maxWidth(320)
  , m_currentChatGroupId(0)
  , m_currentChatUserId(0)
  , m_tWaitCall2Group(0)
  , m_forwardMsgId(0)
  , m_forwardTime(0)
  , m_forwardCallId(0)
  , m_forwardUserId(0)
  , m_forwardGroupId(0)
{
    m_searchMode = 0;
}
EbClientApp::~EbClientApp(void)
{
    exitApp(false);

//    m_myGroupInfoDataList.clear();
}

bool EbClientApp::setDevAppId()
{
    return EbClientAppBase::setDevAppId(0);
}

bool EbClientApp::login(const QString &account,
                        const QString &password,
                        const QString &savePassword)
{
    /// for test
//    EbQuickGroupInfo pGroupInfo;
//    pGroupInfo.m_sEnterpriseCode = 25897456325;
//    pGroupInfo.m_sGroupName = "group name";
//    emit receivedGroupInfo(&pGroupInfo);
//    return false;

    if (!m_ebum.EB_IsInited()) {
        return false;
    }
    m_ebum.EB_SetMsgReceiver(this);
#ifdef Q_OS_ANDROID
    const EB_LOGON_TYPE logonType = EB_LOGON_TYPE_ANDROID;
#elif defined(Q_OS_WIN32)
    const EB_LOGON_TYPE logonType = EB_LOGON_TYPE_PC;
#else
    const EB_LOGON_TYPE logonType = EB_LOGON_TYPE_IOS;
#endif
    const EB_USER_LINE_STATE lineState = EB_LINE_STATE_ONLINE_NEW;
    m_sRequestAccount = account;
    m_sRequestPassword = password;

    tstring sSavePassword = savePassword.toStdString();
    if (!password.isEmpty() && savePassword.isEmpty()) {
        /// 获取本地硬件信息，保存自动登录KEY
        entboost::GetLocalHostOAuthKey(sSavePassword);
    }
    m_ebum.EB_LogonByAccount(
                account.toLocal8Bit().constData(),
                password.toLocal8Bit().constData(),
                "",
                sSavePassword.c_str(),
                lineState,
                logonType);
//    emit loginError(1, "Login Error String...");
//    emit loginSuccess();
    return true;
}

bool EbClientApp::loginVisitor()
{
#ifdef Q_OS_ANDROID
    const EB_LOGON_TYPE logonType = EB_LOGON_TYPE_ANDROID;
#elif defined(Q_OS_WIN32)
    const EB_LOGON_TYPE logonType = EB_LOGON_TYPE_PC;
#else
    const EB_LOGON_TYPE logonType = EB_LOGON_TYPE_IOS;
#endif
    return m_ebum.EB_LogonByVisitor("", logonType);
}

void EbClientApp::logout(bool clearSavePassword, bool acceptPush)
{
    if (clearSavePassword &&
            m_sqliteEbc.get()!=0) {
        /// 清空保存密码
        char sql[128];
        sprintf(sql, "update user_login_record_t set password='' where user_id=%lld",
                this->logonUserId());
        m_sqliteEbc->execute(sql);
    }
    {
        AUTO_RLOCK(m_pCallList);
        CLockMap<eb::bigint, EbcCallInfo::pointer>::iterator iter = m_pCallList.begin();
        for (; iter!=m_pCallList.end(); iter++) {
            const EbcCallInfo::pointer &callInfo = iter->second;
            qint64 msgId = 0;
            EbFileInfo::pointer temp;
            while (callInfo->m_progressFiles.get_begin(&msgId, &temp, true)) {
                this->m_ebum.EB_CancelFile(callInfo->callId(), msgId);
            }
        }
        m_pCallList.clear(false);
    }
    m_ebum.EB_Logout(acceptPush);
    this->exitApp(true);
    m_sqliteUser.reset();
}

int EbClientApp::loadLoginData()
{
    if (m_sqliteEbc.get()==0) {
        return -1;
    }
    /// for test
//    emit loginRecordInfo("account", "account", 80);
//    return 0;

    std::vector<eb::bigint> deleteDataList;
    char sql[260];
    int nCookie = 0;
    const int ret = (int)m_sqliteEbc->select("SELECT user_id,account,account_r,password,safepwd,linestate,phone FROM user_login_record_t ORDER BY recordtime DESC", nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
    while (pRecord.get()!=NULL) {
        const mycp::bigint nUserId = pRecord->getVector()[0]->getBigIntValue();
        const mycp::tstring sAccount( pRecord->getVector()[1]->getStr() );
        const mycp::tstring sRealAccount( pRecord->getVector()[2]->getStr() );
        if (sAccount.empty() && sRealAccount.empty()) {
//        if (sAccount.empty() || sRealAccount.empty() || m_pLoginInfoList.exist(nUserId)) {
            if (nUserId>0) {
                deleteDataList.push_back(nUserId);
            }
            pRecord = m_sqliteEbc->next(nCookie);
            continue;
        }
        const mycp::tstring sPassword( pRecord->getVector()[3]->getStr() );
//        const bool bSafePwd = pRecord->getVector()[4]->getIntValue()==1?true:false;
//        const EB_USER_LINE_STATE nLineState = (EB_USER_LINE_STATE)pRecord->getVector()[5]->getIntValue();
        emit loginRecordInfo(
                    QString::fromStdString(sAccount.string()),
                    QString::fromStdString(sRealAccount.string()),
                    nUserId,
                    QString::fromStdString(sPassword.string()));
        pRecord = m_sqliteEbc->next(nCookie);
    }
    m_sqliteEbc->reset(nCookie);
    /// 清除没用数据
    for (size_t i=0; i<deleteDataList.size(); i++) {
        sprintf(sql, "DELETE FROM user_login_record_t where user_id=%lld AND (length(account)=0 OR length(account_r)=0)", deleteDataList[i]);
        m_sqliteEbc->execute(sql);
    }
    return ret;
}

inline bool deleteDirectory(const QString &path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            if (!fi.dir().remove(fi.fileName())) return false;
        }
        else {
            if (!deleteDirectory(fi.absoluteFilePath())) return false;
        }
    }
    return dir.rmdir(dir.absolutePath());
//    return dir.rmpath(dir.absolutePath());
}

QString EbClientApp::deleteLoginData(const QString &accountInput, const QString &accountReal, qint64 userId)
{
    /// 安卓使用 UID 目录
    QString sUserDirectory;
    if (userId>0) {
        sUserDirectory = appUsersPath() + "/" + QString::number(userId);
    }
    else {
        sUserDirectory = appUsersPath() + "/" + accountReal;
    }
//    const mycp::tstring sRealUserAccount(loginInfo->m_sRealAccount);
    if (QFile::exists(sUserDirectory)) {
        if (!deleteDirectory(sUserDirectory)) {
            /// 删除用户数据失败：<br>请重试！
//            setErrorText( m_locales->getLocalText("message-show.delete-logon-account-error", "删除用户数据失败：<br>请重试！"),true );
            return m_locales->getLocalText("message-show.delete-logon-account-error", "删除用户数据失败：<br>请重试！");
        }
    }
    if (userId>0) {
        // ??
        /// ** 删除对应 chrome 浏览器缓存
        /// C:/Users/pc/AppData/Roaming/ebcd
        const QString sStringTemp = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        const QString sCefCachePath = QString("%1/cef_cache_temp/%2").arg(sStringTemp).arg(userId);
        deleteDirectory(sCefCachePath);
    }
    if (!accountReal.isEmpty()) {
        char sSql[260];
        if (userId>0) {
            sprintf(sSql, "delete from user_login_record_t where account='%s' OR account_r='%s' OR user_id=%lld",
                    accountInput.toStdString().c_str(), accountReal.toStdString().c_str(), userId);
        }
        else {
            sprintf(sSql,"delete from user_login_record_t where account='%s' OR account_r='%s'",
                    accountInput.toStdString().c_str(), accountReal.toStdString().c_str());
        }
        m_sqliteEbc->execute(sSql);
    }
    else if (userId>0) {
        char sSql[260];
        sprintf(sSql, "delete from user_login_record_t where user_id=%lld", userId);
        m_sqliteEbc->execute(sSql);
    }
    return "";
}

bool EbClientApp::registerAccount(const QString &account, const QString &password,
                                  const QString &userName, const QString &enterpriseName)
{
    if (!m_registerEbUm.EB_IsInited()) {
        const std::string sAddress = m_setting.GetServerAddress();
        m_registerEbUm.EB_Init(sAddress.c_str());
        m_registerEbUm.EB_SetMsgReceiver(this);
        if (!m_registerEbUm.EB_IsInited()) {
            /// 本地服务启动失败：<br>请重试！
            m_errorCode = EB_STATE_ERROR;
            m_errorString = m_locales->getLocalText("message-show.ebum-start-error", "EBUM Start Error");
            emit registerError(m_errorCode, m_errorString);
            return false;
        }
    }
    m_registerAccount = account;

//    m_sUserName = ui->lineEditUserName->text();
//    m_sEnterpriseName = ui->lineEditEntName->text();
    const eb::bigint sAppId = m_ebum.EB_GetDevAppId();
    const tstring sAppOnlineKey = m_ebum.EB_GetDevAppOnlineKey();
    m_registerEbUm.EB_SetDevAppOnlineKey(sAppId, sAppOnlineKey.c_str());
    m_registerEbUm.EB_Register(account.toStdString().c_str(),
                               password.toStdString().c_str(),
                               userName.toStdString().c_str(),
                               enterpriseName.toStdString().c_str());
    return true;
}

int EbClientApp::loadOrgData()
{
    return this->m_ebum.EB_LoadOrg();
}

int EbClientApp::loadContactData()
{
    return this->m_ebum.EB_LoadContact();
}

QString EbClientApp::enterpriseName(qint64 enterpriseId) const
{
    mycp::tstring name;
    if (m_ebum.EB_GetEnterpriseName(name, enterpriseId)) {
        return QString::fromStdString(name.string());
    }
    else {
        return "";
    }
}

int EbClientApp::groupType(qint64 groupId) const
{
    EB_GROUP_TYPE groupType = EB_GROUP_TYPE_DEPARTMENT;
    m_ebum.EB_GetGroupType(groupId, groupType);
    return (int)groupType;
}

QString EbClientApp::groupName(qint64 groupId) const
{
    mycp::tstring name;
    if (m_ebum.EB_GetGroupName(groupId, name)) {
        return QString::fromStdString(name.string());
    }
    else {
        return QString::number(groupId);
    }
}

QString EbClientApp::groupFullName(qint64 groupId,
                                   const QString &groupName,
                                   int memberSize) const
{
    const int onlineSize = m_ebum.EB_GetGroupOnlineSize(groupId, 1);
    if (onlineSize>=0 && memberSize>0)
        return QString("%1 [%2/%3]").arg(groupName).arg(onlineSize).arg(memberSize);
    else if (memberSize>0)
        return QString("%1 [%2]").arg(groupName).arg(memberSize);
    else
        return groupName;
}

QString EbClientApp::groupFullName(qint64 groupId) const
{
    EB_GroupInfo groupInfo;
    if (m_ebum.EB_GetGroupInfo(groupId, &groupInfo)) {
        return QString("%1 (%2)")
                .arg(groupInfo.m_sGroupName.c_str())
                .arg(groupInfo.m_nEmpCount);
    }
    else {
        return QString::number(groupId);
    }
}

//QString EbClientApp::groupFullName(qint64 groupId, const QString &groupName) const
//{
//    int memberSize = 0;
//    int onlineSize = 0;
//    m_ebum.EB_GetGroupMemberSize(groupId, 1, memberSize, onlineSize);
//    if (onlineSize>=0 && memberSize>0)
//        return QString("%1 [%2/%3]").arg(groupName).arg(onlineSize).arg(memberSize);
//    else if (memberSize>0)
//        return QString("%1 [%2]").arg(groupName).arg(memberSize);
//    else
//        return groupName;
//}

QString EbClientApp::groupImage(qint64 groupId) const
{
    EB_GROUP_TYPE groupType = EB_GROUP_TYPE_DEPARTMENT;
    m_ebum.EB_GetGroupType(groupId, groupType);
    if (groupType==EB_GROUP_TYPE_DEPARTMENT) {
        return "assets:/res/imgdefaultdepartment.png";
    }
    else if (groupType==EB_GROUP_TYPE_PROJECT) {
        return "assets:/res/imgdefaultproject.png";
    }
    else if (groupType==EB_GROUP_TYPE_GROUP) {
        return "assets:/res/imgdefaultgroup.png";
    }
    else {
        return "assets:/res/imgdefaulttempgroup.png";
    }
}

bool EbClientApp::hasSubGroup(qint64 groupId) const
{
    return m_ebum.EB_HasSubGroup(groupId);
}

int EbClientApp::subGroupCount(qint64 groupId) const
{
    return m_ebum.EB_SubGroupCount(groupId);
}

bool EbClientApp::isGroupAdminLevel(qint64 groupId) const
{
    return m_ebum.EB_IsGroupAdminLevel(groupId);
}

bool EbClientApp::isGroupAdminLevel(qint64 groupId, qint64 memberUserId) const
{
    return m_ebum.EB_IsGroupAdminLevel(groupId, memberUserId);
}

bool EbClientApp::isGroupForbidSpeech(qint64 groupId) const
{
    return m_ebum.EB_IsGroupForbidSpeech(groupId);
}

int EbClientApp::setGroupForbidSpeech(qint64 groupId, bool forbidSpeech)
{
    return m_ebum.EB_SetGroupForbidSpeech(groupId, forbidSpeech);
}

bool EbClientApp::canEditGroupInfo(qint64 groupId) const
{
    return m_ebum.EB_IsGroupAdminLevel(groupId);
}

bool EbClientApp::canNewGroupInfo(qint64 enterpriseId) const
{
    if (enterpriseId<0) {
        enterpriseId = this->m_ebum.EB_GetEnterpriseId();
    }
    return m_ebum.EB_CanEditGroupInfo(enterpriseId, 0);
}

int EbClientApp::editGroupInfo(EbQuickGroupInfo *groupInfo)
{
    return m_ebum.EB_EditGroup(groupInfo);
}

int EbClientApp::newGroupInfo(int groupType, const QString &groupName,
                              qint64 enterpriseId, qint64 parentId)
{
    EB_GroupInfo groupInfo;
    groupInfo.m_nGroupType = (EB_GROUP_TYPE)groupType;
    groupInfo.m_sEnterpriseCode = enterpriseId;
    groupInfo.m_sParentCode = parentId;
    groupInfo.m_sGroupName = groupName.toStdString();
    groupInfo.m_nCreateUserId = logonUserId();
    return m_ebum.EB_EditGroup(&groupInfo);
}

bool EbClientApp::canDeleteGroupInfo(qint64 groupId) const
{
    if (!m_ebum.EB_CanDeleteGroupInfo(groupId)) {
        return false;
    }
    if (m_ebum.EB_HasSubGroup(groupId)) {
        /// 有下级单位，不能直接删除该部门
        return false;
    }
    return true;

}

int EbClientApp::deleteGroupInfo(qint64 groupId)
{
    return m_ebum.EB_DeleteGroup(groupId);
}

bool EbClientApp::canExitGroupInfo(qint64 groupId, int groupType) const
{
    if (!m_ebum.EB_IsGroupCreator(groupId) &&
            groupType != EB_GROUP_TYPE_DEPARTMENT &&
            groupType != EB_GROUP_TYPE_PROJECT) {
        return true;
    }
    return false;
}

int EbClientApp::exitGroupInfo(qint64 groupId)
{
    return m_ebum.EB_ExitGroup(groupId);
}

bool EbClientApp::canEditMemberAdminLevel(qint64 groupId, qint64 memberUserId) const
{
    /// 我是群主，选择对象不是群主
    return m_ebum.EB_IsGroupCreator(groupId) &&
            !m_ebum.EB_IsGroupCreator(groupId, memberUserId);
//    if (theApp->m_ebum.EB_IsGroupCreator(m_itemInfo->m_sGroupCode) && !theApp->m_ebum.EB_IsGroupCreator(m_itemInfo->m_sGroupCode,m_itemInfo->m_nUserId)) {
//        //            m_adminSeparatorAction->setVisible(true);
//        if (theApp->m_ebum.EB_IsGroupAdminLevel(m_itemInfo->m_sGroupCode,m_itemInfo->m_nUserId))
//            this->setMenuActionVisible( EB_COMMAND_MEMBER_DEL_ADMIN,true );
//        else
//            this->setMenuActionVisible( EB_COMMAND_MEMBER_ADD_ADMIN,true );
    //    }
}

int EbClientApp::addMemberAdminLevel(qint64 groupId, qint64 memberUserId)
{
    return m_ebum.EB_AddGroupAdminLevel(groupId, memberUserId);
}

int EbClientApp::deleteMemberAdminLevel(qint64 groupId, qint64 memberUserId)
{
    return m_ebum.EB_DelGroupAdminLevel(groupId, memberUserId);
}

bool EbClientApp::canEditMemberInfo(qint64 groupId, qint64 memberUserId) const
{
    if ( (m_ebum.EB_IsGroupCreator(groupId) && !m_ebum.EB_IsGroupCreator(groupId, memberUserId)) ||
         (m_ebum.EB_IsGroupAdminLevel(groupId) && !m_ebum.EB_IsGroupAdminLevel(groupId, memberUserId)) ) {
        /// 选择用户不是群管理员
        return true;
    }
    return false;
}

bool EbClientApp::isGroupMember(qint64 groupId, int userId) const
{
    return m_ebum.EB_IsGroupMember(groupId, userId);
}

bool EbClientApp::canNewMemberInfo(qint64 groupId) const
{
    return m_ebum.EB_CanEditMemberInfo(groupId, 0);
//    return m_ebum.EB_CanEditMemberInfo(groupId, this->logonUserId());
}

int EbClientApp::deleteMemberInfo(qint64 groupId, qint64 memberUserId)
{
    return m_ebum.EB_DeleteMember(groupId, memberUserId, true);
}

int EbClientApp::editMemberInfo(EbQuickMemberInfo *memberInfo)
{
    return m_ebum.EB_EditMember(memberInfo);
}

EbQuickMemberInfo *EbClientApp::memberInfo(qint64 groupId, qint64 userId)
{
    EB_MemberInfo memberInfo;
    if (this->m_ebum.EB_GetMemberInfoByUserId(&memberInfo, groupId, userId)) {
        return new EbQuickMemberInfo(&memberInfo);
    }
    return 0;
}

EbQuickMemberInfo *EbClientApp::memberInfoByMemberId(qint64 memberId)
{
    EB_MemberInfo memberInfo;
    if (this->m_ebum.EB_GetMemberInfoByMemberCode(&memberInfo, memberId)) {
        return new EbQuickMemberInfo(&memberInfo);
    }
    return 0;
}

EbQuickMemberInfo *EbClientApp::defaultMemberInfo(qint64 userId)
{
    EB_MemberInfo memberInfo;
    if (userId==this->logonUserId()) {
        if (this->m_ebum.EB_GetMyDefaultMemberInfo(&memberInfo)) {
            return new EbQuickMemberInfo(&memberInfo);
        }
    }
    this->m_ebum.EB_GetMemberInfoByUserId2(&memberInfo, userId);
    return new EbQuickMemberInfo(&memberInfo);
}

EbQuickMemberInfo *EbClientApp::buildNewMemberInfo(qint64 groupId, const QString &account, const QString &userName)
{
    EbQuickMemberInfo * result = new EbQuickMemberInfo();
    result->setGroupId(groupId);
    result->setMemberAccount(account);
    result->setUserName(userName);
    return result;
}

EbQuickGroupInfo *EbClientApp::groupQuickInfo(qint64 groupId, bool loadMember)
{
    EB_GroupInfo groupInfo;
    if (m_ebum.EB_GetGroupInfo(groupId, &groupInfo)) {
        if (loadMember) {
            m_ebum.EB_LoadGroup(groupId, true);
        }
        return new EbQuickGroupInfo(&groupInfo);
    }
    return 0;
}

int EbClientApp::requestLoadGroupMember(qint64 groupId)
{
    return m_ebum.EB_LoadGroup(groupId, true);
}

bool compare_group_member_list(const EB_MemberInfo& t1,const EB_MemberInfo& t2){
    const int nSubType1 = (int)t1.GetEventParameter();
    const int nSubType2 = (int)t2.GetEventParameter();
    if (t1.m_nDisplayIndex!=t2.m_nDisplayIndex)
        return t1.m_nDisplayIndex>t2.m_nDisplayIndex;
    else if (nSubType1 != nSubType2)
        return nSubType1 > nSubType2;
    else
        return t1.m_sUserName < t2.m_sUserName;
}
void EbClientApp::loadGroupMemberInfo(qint64 groupId, int parameter)
{
    std::vector<EB_MemberInfo> memberInfoList;
    if (m_ebum.EB_GetGroupMemberInfoList(groupId, memberInfoList) && memberInfoList.size()>0) {
        /// 先把自己排在最前面
        std::vector<EB_MemberInfo>::iterator iter = memberInfoList.begin();
        for (; iter!=memberInfoList.end(); iter++) {
            const EB_MemberInfo &memberInfo = *iter;
            if (memberInfo.m_nMemberUserId==this->logonUserId()) {
                EB_MemberInfo memberInfoTemp = memberInfo;
                memberInfoList.erase(iter);
                memberInfoList.insert(memberInfoList.begin(), memberInfoTemp);
                break;
            }
        }
        /// 计算出所有权限值
        for (iter=memberInfoList.begin(); iter!=memberInfoList.end(); iter++) {
            const EB_MemberInfo &memberInfo = *iter;
            (*iter).SetEventParameter((long)memberSubType(&memberInfo));
        }
        /// 再按照权限排序
        iter = memberInfoList.begin();
        std::sort(++iter, memberInfoList.end(), compare_group_member_list);

        for (size_t i=0; i<memberInfoList.size(); i++) {
            const EB_MemberInfo &memberInfo = memberInfoList[i];
            EbQuickMemberInfo *quickMemberInfo = new EbQuickMemberInfo(&memberInfo);
            quickMemberInfo->setSubType((int)memberInfo.GetEventParameter());
            emit loadedGroupMemberInfo(quickMemberInfo, parameter);
        }
    }
    else {
        m_ebum.EB_LoadGroup(groupId, true);
    }
}

int EbClientApp::loadSubGroupInfo(qint64 groupId, int parameter)
{
    std::vector<EB_GroupInfo> subGroupInfoList;
    m_ebum.EB_GetSubGroupInfoList(groupId, subGroupInfoList);
    for (size_t i=0; i<subGroupInfoList.size(); i++) {
        const EB_GroupInfo &groupInfo = subGroupInfoList[i];
        EbQuickGroupInfo * quickGroupInfo = new EbQuickGroupInfo(&groupInfo);
        const QString fullGroupName = this->groupFullName(quickGroupInfo->groupId(),
                                                          quickGroupInfo->groupName(),
                                                          quickGroupInfo->m_nEmpCount);
        quickGroupInfo->setGroupFullName(fullGroupName);
        emit receivedSubGroupInfo(quickGroupInfo,
                                  QString::number(groupId),
                                  parameter);
    }
    return (int)subGroupInfoList.size();
}

QString EbClientApp::userName(qint64 groupId, qint64 userId) const
{
    tstring userName;
    m_ebum.EB_GetMemberNameByUserId(groupId, userId, userName);
    return QString::fromStdString(userName.string());
}

QString EbClientApp::userNameByUserId(qint64 userId, qint64 callId) const
{
    tstring userName;
    if (m_ebum.EB_GetContactName2(userId, userName)) {
        return QString::fromStdString(userName.string());
    }
    else if (m_ebum.EB_GetMemberNameByUserId2(userId, userName)) {
        return QString::fromStdString(userName.string());
    }
    else {
        EbcCallInfo::pointer callInfo;
        if (callId>0 && m_pCallList.find(callId, callInfo) && callInfo->fromUserId()==userId) {
            return getFromAccountName(callInfo->m_pFromAccountInfo).c_str();
        }
        else {
            return QString::number(userId);
        }
    }
}

inline bool isFullNumber(const char* pString, size_t nLen)
{
    for (size_t i=0;i<nLen; i++)
    {
        const char pChar = pString[i];
        if (pChar<'0' || pChar>'9')
            return false;
    }
    return true;
}
QString EbClientApp::userNameByAccount(const QString &account) const
{
    const std::string aAccount = account.toStdString();
    if (isFullNumber(aAccount.c_str(), aAccount.size())) {
        return userNameByUserId(eb_atoi64(aAccount.c_str()));
    }
    else {
        /// ?
        return account;
    }
}

bool EbClientApp::hasUGContact(qint64 ugId) const
{
    return m_ebum.EB_HasUGContact(ugId);
}

int EbClientApp::ugContactCount(qint64 ugId) const
{
    int nContactCount = 0;
    int nOnlineCount = 0;
    m_ebum.EB_GetUGContactSize(ugId, nContactCount, nOnlineCount);
    return nContactCount;
}

EbQuickUGInfo *EbClientApp::buildUGInfo(qint64 ugId, const QString &groupName)
{
    EbQuickUGInfo * quickUGInfo = new EbQuickUGInfo();
    quickUGInfo->setUgId(ugId);
    quickUGInfo->setGroupName(groupName);
    quickUGInfo->setGroupFullName(ugFullName(ugId, groupName));
    return quickUGInfo;
}

QString EbClientApp::ugFullName(qint64 ugId, const QString &groupName) const
{
    int nContactCount = 0;
    int nOnlineCount = 0;
    m_ebum.EB_GetUGContactSize(ugId, nContactCount, nOnlineCount);
    return QString("%1 [%2/%3]").arg(groupName).arg(nOnlineCount).arg(nContactCount);
}

QString EbClientApp::memberHeadImage(EbQuickMemberInfo *memberInfo) const
{
    if (!memberInfo->m_sHeadResourceFile.isEmpty() &&
            QFile::exists(memberInfo->m_sHeadResourceFile)) {
        return theFileHead+memberInfo->m_sHeadResourceFile;
    }

    eb::bigint pOutResourceId = 0;
    EBFileString pOutHeadPath;
    mycp::tstring pOutFileMd5;
    this->m_ebum.EB_GetMemberHeadFile2(memberInfo->m_nMemberUserId, pOutResourceId, pOutHeadPath, pOutFileMd5);
    if (!pOutHeadPath.isEmpty() && QFile::exists(pOutHeadPath)) {
        return theFileHead+pOutHeadPath;
    }
    /// 获取联系人
    EB_USER_LINE_STATE lineState = EB_LINE_STATE_OFFLINE;
    this->m_ebum.EB_GetContactHeadInfoByUserId(memberInfo->m_nMemberUserId, pOutHeadPath, pOutFileMd5, lineState);
    if (!pOutHeadPath.isEmpty() && QFile::exists(pOutHeadPath)) {
        return theFileHead+pOutHeadPath;
    }
    return "assets:/res/defaultmember.png";
}

QString EbClientApp::contactHeadImage(EbQuickContactInfo *contactInfo) const
{
    if (!contactInfo->m_sHeadResourceFile.isEmpty() && QFile::exists(contactInfo->m_sHeadResourceFile)) {
        return theFileHead+contactInfo->m_sHeadResourceFile;
    }

    if (contactInfo->m_nContactUserId>0) {
        eb::bigint pOutResourceId = 0;
        EBFileString pOutHeadPath;
        mycp::tstring pOutFileMd5;
        this->m_ebum.EB_GetMemberHeadFile2(contactInfo->m_nContactUserId, pOutResourceId, pOutHeadPath, pOutFileMd5);
        if (!pOutHeadPath.isEmpty() && QFile::exists(pOutHeadPath)) {
            return theFileHead+pOutHeadPath;
        }
    }
    return "assets:/res/defaultcontact.png";
}

QString EbClientApp::userHeadImage2(qint64 userId, const QString &resourceFile) const
{
    if (!resourceFile.isEmpty() && QFile::exists(resourceFile)) {
        return theFileHead+resourceFile;
    }
    else {
        return userHeadImage(userId);
    }
}

QString EbClientApp::userHeadImage(qint64 userId) const
{
    eb::bigint pOutResourceId = 0;
    EBFileString pOutHeadPath;
    mycp::tstring pOutFileMd5;
    EB_USER_LINE_STATE lineState = EB_LINE_STATE_OFFLINE;
    this->m_ebum.EB_GetContactHeadInfoByUserId(userId, pOutHeadPath, pOutFileMd5, lineState);
    if (!pOutHeadPath.isEmpty() && QFile::exists(pOutHeadPath)) {
        return theFileHead+pOutHeadPath;
    }
    this->m_ebum.EB_GetMemberHeadFile2(userId, pOutResourceId, pOutHeadPath, pOutFileMd5);
    if (!pOutHeadPath.isEmpty() && QFile::exists(pOutHeadPath)) {
        return theFileHead+pOutHeadPath;
    }
    return "assets:/res/defaultmember.png";
}

int EbClientApp::loadUGContact(qint64 ugId)
{
    std::vector<EB_ContactInfo> ugContactList;
    this->m_ebum.EB_GetUGContactList(ugId, ugContactList);
    for (size_t i=0; i<ugContactList.size(); i++) {
        const EB_ContactInfo &contactInfo = ugContactList[i];
        emit receivedContactInfo(new EbQuickContactInfo(&contactInfo));
    }
    return (int)ugContactList.size();
}

bool EbClientApp::compareContactInfo(EbQuickContactInfo *contactInfo1, EbQuickContactInfo *contactInfo2) const
{
    if (contactInfo1->contactUserId()>0 && contactInfo2->contactUserId()==0) return false; /// 第1个有UID，不需要对换位置
    else if (contactInfo2->contactUserId()>0 && contactInfo1->contactUserId()==0) return true;	/// 第2个0，需要对换位置
    /// 先比较在线状态
    if ((contactInfo1->m_nLineState==EB_LINE_STATE_ONLINE_NEW && contactInfo2->m_nLineState<EB_LINE_STATE_ONLINE_NEW) ||
        (contactInfo1->m_nLineState>=EB_LINE_STATE_BUSY && contactInfo2->m_nLineState<EB_LINE_STATE_BUSY)) {
        /// a 第一个在线，第二个其他 不需要对换位置
        /// b 第一个离开/忙，第二个离线 不需要换位置
        return false;
    }
    else if ((contactInfo2->m_nLineState==EB_LINE_STATE_ONLINE_NEW && contactInfo1->m_nLineState<EB_LINE_STATE_ONLINE_NEW) ||
        (contactInfo2->m_nLineState>=EB_LINE_STATE_BUSY && contactInfo1->m_nLineState<EB_LINE_STATE_BUSY)) {
        /// a 第2个在线，第1个其他 对调位置
        /// b 第2个离开/忙，第1个离线 对调位置
        return true;
    }
    return contactInfo2->m_sName.string()<contactInfo1->m_sName.string();
}

int EbClientApp::editUGInfo(qint64 ugId, const QString &groupName)
{
    return m_ebum.EB_EditUGInfo(ugId, groupName.toStdString().c_str());
}

int EbClientApp::deleteUGInfo(qint64 ugId)
{
    return m_ebum.EB_DeleteUGInfo(ugId);
}

int EbClientApp::editContactInfo(EbQuickContactInfo *contactInfo)
{
    return this->m_ebum.EB_EditContact(contactInfo);
}

int EbClientApp::deleteContactInfo(qint64 contactId)
{
    if (this->isAuthContact())
        return m_ebum.EB_DeleteContact(contactId, true);
    else
        return m_ebum.EB_DeleteContact(contactId, false);
}

int EbClientApp::moveContactToByContactId(qint64 contactId, qint64 ugId)
{
    return m_ebum.EB_MoveContactTo1(contactId, ugId);
}

EbQuickContactInfo *EbClientApp::getContactInfoByUserId(qint64 userId)
{
    EB_ContactInfo contactInfo;
    if (m_ebum.EB_GetContactInfo2(userId, &contactInfo)) {
        return new EbQuickContactInfo(&contactInfo);
    }
    return 0;
}

int EbClientApp::loadAllUGInfo(int parameter)
{
    std::vector<EB_UGInfo> pUGInfoList;
    this->m_ebum.EB_GetUGInfoList(pUGInfoList);
    for (size_t i=0; i<pUGInfoList.size(); i++) {
        const  EB_UGInfo &ugInfo = pUGInfoList[i];
        EbQuickUGInfo * quickUGInfo = new EbQuickUGInfo(&ugInfo);
        quickUGInfo->setGroupFullName(ugFullName(quickUGInfo->ugId(),
                                                 quickUGInfo->groupName()));
        emit loadUGInfoResult(quickUGInfo, parameter);
    }
    return (int)pUGInfoList.size();
}

int EbClientApp::loadSubscribeFuncInfo(int location, int parameter)
{
    std::vector<EB_SubscribeFuncInfo> pFuncInfoList;
    this->m_ebum.EB_GetSubscribeFuncList((EB_FUNC_LOCATION)location, pFuncInfoList);
    for (size_t i=0; i<pFuncInfoList.size(); i++) {
        const EB_SubscribeFuncInfo &funcInfo = pFuncInfoList[i];
        emit loadSubscribeFuncInfoResult(new EbQuickSubscribeFuncInfo(&funcInfo), parameter);
    }
    return (int)pFuncInfoList.size();
}

QString EbClientApp::subscribeFuncUrl(qint64 subId, qint64 groupId, const QString &sParameters)
{
    mycp::bigint m_nSelectCallId = 0;
    mycp::bigint m_nSelectUserId = 0;
    mycp::bigint m_nSelectGroupId = groupId;
    tstring m_sSelectAccount;

    const tstring sFuncUrl = m_ebum.EB_GetSubscribeFuncUrl(
                subId,
                m_nSelectCallId,
                m_nSelectUserId,
                m_sSelectAccount,
                m_nSelectGroupId);
    if ( sFuncUrl.empty() ) {
        return "";
    }

    char buffer[1024];
    sprintf( buffer, "%s&color=%02x%02x%02x", sFuncUrl.c_str(), m_mainColor.red(), m_mainColor.green(), m_mainColor.blue() );
    QString sFullFuncurl(buffer);
    if (!sParameters.isEmpty()) {
        sFullFuncurl.append("&");
        sFullFuncurl.append(sParameters);
    }
    return sFullFuncurl;
}

bool EbClientApp::isExistSubscribeFunc(qint64 subId) const
{
    return m_ebum.EB_IsExistSubscribeFuncInfo(subId);
}

EbQuickSubscribeFuncInfo *EbClientApp::subscribeFuncInfo(qint64 subId)
{
    EB_SubscribeFuncInfo subFuncInfo;
    if (m_ebum.EB_GetSubscribeFuncInfo(subId, &subFuncInfo)) {
        return new EbQuickSubscribeFuncInfo(&subFuncInfo);
    }
    return 0;
}

int ParseString(const char * lpszString, const char * lpszInterval, std::vector<tstring> & pOut)
{
    const tstring sIn(lpszString);
    const size_t nInLen = sIn.size();
    const size_t nIntervalLen = strlen(lpszInterval);
    pOut.clear();
    std::string::size_type nFindBegin = 0;
    while (nFindBegin<nInLen)
    {
        std::string::size_type find = sIn.find(lpszInterval,nFindBegin);
        if (find == std::string::npos)
        {
            pOut.push_back(sIn.substr(nFindBegin));
            break;
        }
        if (find==nFindBegin)
        {
            pOut.push_back("");	// 空
        }else
        {
            pOut.push_back(sIn.substr(nFindBegin, (find-nFindBegin)));
        }
        nFindBegin = find+nIntervalLen;
    }
    return (int)pOut.size();
}

int EbClientApp::openSubId(const QString &info)
{
    const eb::bigint nSubId = eb_atoi64(info.toStdString().c_str());
    if (nSubId==0) {
        return 0;
    }
    bool bOpenNewWindows = true;
    QString sParameters;
    int pos = info.indexOf(",", 1);
    if( pos > 1 ) {
        sParameters = info.mid(pos+1);
        const QString sOpenFlag = sParameters.isEmpty()?"":sParameters.left(1);
        if (sOpenFlag=="0") {
            bOpenNewWindows = false;
            sParameters = sParameters.mid(11);
        }
        else if (sOpenFlag=="1") {
            bOpenNewWindows = true;
            sParameters = sParameters.mid(1);
        }
        if (!sParameters.isEmpty() && sParameters.left(1)==",") {
            sParameters = sParameters.mid(1);
        }
    }

    if (!bOpenNewWindows) {
        const QString sUrl = this->subscribeFuncUrl(nSubId, 0, sParameters);
        if (!sUrl.isEmpty()) {
            emit requestUrlRedirect(sUrl);
        }
        return 1;
    }
    emit requestUrlOpenSubId(QString::number(nSubId), sParameters);
    return 1;
}

int EbClientApp::requestUrl(const QString &url)
{
    /// 返回描述
    /// 0=默认不处理
    /// 1=IM业务处理，QML不需要处理
    /// 2=跳转链接？
    QUrl pUrl(url);
    const QString scheme = pUrl.scheme();
    /// 80->"0.0.0.80"
    /// 898542->"0.13.181.238"
    const QString host = pUrl.host();
    if (scheme==theDownloadResource) {
//        QString resourceInfo = pUrl.url().mid(scheme.length()+3);
//        if (resourceInfo.left(1)=="/") {
//            /// 新版本 xxx:///
//            resourceInfo = resourceInfo.mid(1);
//        }
//        const int nFind1 = resourceInfo.indexOf(',');
//        if (nFind1<=0) return 0;
////        const int nType = resourceInfo.left(nFind1).toInt();
//        const int nFind2 = resourceInfo.indexOf(',',nFind1+1);
//        eb::bigint resourceId = 0;
//        tstring defaultFileName;
//        if (nFind2>0)
//        {
//            const QString temp = resourceInfo.mid(nFind1+1,nFind2-nFind1-1);
//            resourceId = resourceInfo.mid(nFind1+1,nFind2-nFind1-1).toLongLong();
//            defaultFileName = libEbc::URLDecode(resourceInfo.mid(nFind2+1).toStdString().c_str(),true);
//        }
//        else {
//            resourceId = resourceInfo.mid(nFind1+1).toLongLong();
//            defaultFileName = QString("%1").arg(resourceId).toStdString();
//        }
//        const QString newPath = QFileDialog::getSaveFileName(0, QString(), defaultFileName.c_str() );
//        if (!newPath.isEmpty()) {
//            theApp->m_ebum.EB_DownloadFileRes(resourceId, newPath.toStdString().c_str());
//        }
    }
    else if (scheme==theReqAddContact) {
        QString requestInfo = pUrl.url().mid(scheme.length()+3);
        if (requestInfo.left(1)=="/") {
            /// 新版本 xxx:///
            requestInfo = requestInfo.mid(1);
        }
        const int nFind1 = requestInfo.indexOf(',');
        if (nFind1<=0) return 0;
        const qint64 nContactUserId = requestInfo.left(nFind1).toLongLong();
        if (nContactUserId==0) {
            return 0;
        }
        /// 最长取100个字符
        const QString sDescription = requestInfo.mid(nFind1+1).left(100);
//        theApp->m_ebum.EB_ReqAddContact(nContactUserId,sDescription.toStdString().c_str());
    }
    else if ( scheme==theCallAccount ) {
        const std::string callAccount( host.toStdString() );
        std::vector<tstring> list;
        if ( ParseString( callAccount.c_str(), ".", list) == 4) {
            const unsigned int userId = QHostAddress(host).toIPv4Address();
            emit requestUrlCallUserId(QString::number(userId));
        }
        else if (isFullNumber(callAccount.c_str(), callAccount.size())){
            emit requestUrlCallUserId(QString::fromStdString(callAccount));
//            emit requestUrlCallUserId(QString::number(eb_atoi64(callAccount.c_str())));
        }
        else {
            emit requestUrlCallUserAccount(callAccount.c_str());
        }
        return 1;
    }
    else if ( scheme==theCallGroup ) {
        const std::string sGroupId( host.toStdString() );
        std::vector<tstring> list;
        if ( ParseString( sGroupId.c_str(), ".", list) == 4) {
            const unsigned int nGroupId = QHostAddress(host).toIPv4Address();
            emit requestUrlCallGroup(QString::number((qint64)nGroupId));
        }
        else {
            emit requestUrlCallGroup(QString::number((qint64)eb_atoi64(sGroupId.c_str())));
        }
    }
    else if (scheme==theOpenSubId) {
        QString info = pUrl.url().mid(scheme.length()+3);
        if (info.left(1)=="/") {
            /// 新版本 xxx:///
            info = info.mid(1);
        }
        /// 1002300105=群共享应用
//        if (info.left(10).toLongLong()==constGroupShareSubId) {
//            emit openSubId(constGroupShareSubId);
//        }
//        else {
//            EbWebEngineUrlSchemeHandler::instance()->requestUrl(url);
//        }
        openSubId(info);
    }
    else if ( scheme==theOpenFile ) {
        /// 打开文件
        QString fileName = pUrl.url().mid(scheme.length()+3);
#ifdef WIN32
        if (fileName.left(1)=="/") {
#else
        if (fileName.left(2)=="//") {

#endif
            fileName = fileName.mid(1);
        }
        QDesktopServices::openUrl( QUrl::fromLocalFile(fileName) );
    }
//    else if ( scheme==thePlayVoice ) {
//        /// 播放语音消息
//        const QString fileName = pUrl.url().mid(scheme.length()+4);
//        QSound::play( fileName );
//    }
//    else if (scheme==theClose) {

//    }
//    else if (scheme==theExitApp) {
//        theApp->mainWnd()->onTriggeredActionExitApp();
//    }
//    else if (scheme==theLogout) {
//        theApp->mainWnd()->onTriggeredActionLogout();
//    }
//    else if (scheme==theShowMainFrame) {
//        theApp->mainWnd()->showNormal();
//    }
//    else if (scheme==theHideMainFrame) {
//        theApp->mainWnd()->onClickedPushButtonSysMin();
//    }
//    else if (scheme==theOpenFileManager) {
//        theApp->mainWnd()->onClickedPushButtonFileManager();
//    }
    else {
        /// ??? 是否使用工作台打开？
//        QDesktopServices::openUrl( pUrl );
    }
    return 0;
}

EbQuickAccountInfo *EbClientApp::myAccountInfo() const
{
    EB_AccountInfo accountInfo;
    if (this->m_ebum.EB_GetMyAccountInfo(&accountInfo)) {
        return new EbQuickAccountInfo(&accountInfo);
    }
    return 0;
}

QString EbClientApp::myHeadImage()
{
    if (m_myHeadImageFile.isEmpty()) {
        eb::bigint pOutResourceId = 0;
        EBFileString pOutHeadPath;
        mycp::tstring pOutFileMd5;
        this->m_ebum.EB_GetMemberHeadFile2(this->logonUserId(), pOutResourceId, pOutHeadPath, pOutFileMd5);
        if (!pOutHeadPath.isEmpty() && QFile::exists(pOutHeadPath)) {
            m_myHeadImageFile = theFileHead+pOutHeadPath;
        }
        else {
            m_myHeadImageFile = "assets:/res/defaultmember.png";
        }
    }
    return m_myHeadImageFile;
}

int EbClientApp::setPassword(const QString &currentPassword, const QString &newPassword)
{
    if (!m_ebum.EB_SetPassword(currentPassword.toStdString().c_str(),
                          newPassword.toStdString().c_str() )) {
        return -1;
    }
    /// *** 不保存用户数据，保存OAuthKey实现自动登录
    /// *** 兼容旧服务端版本 1.24.2 以前版本； v1.25.0.544, 2017-04-11
    /// *** v1.25.0 以上版本，客户端不保存密码，只保存本地硬件唯一标识，实现自动登录，提高安全性
    if (ebServerVersion()==0) {
        const QString sSql = QString("update user_login_record_t set password='%1' where (account='%2' OR user_id=%3) AND password='%4'")
                .arg(newPassword).arg(logonUserAccount().c_str()).arg(logonUserId()).arg(currentPassword);
        m_sqliteEbc->execute(sSql.toStdString().c_str());
    }
    return 1;
}

void EbClientApp::playTone()
{
    QSound::play(":/wav/msg.wav");
#ifdef Q_OS_ANDROID
//    assets\datas\wav
//    Uri uri = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
//    Ringtone rt = RingtoneManager.getRingtone(getApplicationContext(), uri);
//    rt.play();
#endif
}

void EbClientApp::vibrate(int ms)
{
#ifdef Q_OS_ANDROID
    //        QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
                "android/content/Context",
                "VIBRATOR_SERVICE",
                "Ljava/lang/String;"
                );
    //        CHECK_EXCEPTION();
    QAndroidJniObject vibrateService = activity.callObjectMethod(
                "getSystemService",
                "(Ljava/lang/String;)Ljava/lang/Object;",
                name.object<jstring>());
    //        CHECK_EXCEPTION();
    jlong duration = ms;
    vibrateService.callMethod<void>("vibrate", "(J)V", duration);
    //        CHECK_EXCEPTION();
#endif  /// Q_OS_ANDROID
}

QString EbClientApp::accountHeadImage(EbQuickAccountInfo *account) const
{
    eb::bigint pOutResourceId = 0;
    EBFileString pOutHeadPath;
    mycp::tstring pOutFileMd5;
    this->m_ebum.EB_GetMemberHeadFile2(account->userId(), pOutResourceId, pOutHeadPath, pOutFileMd5);
    if (!pOutHeadPath.isEmpty() && QFile::exists(pOutHeadPath)) {
        return theFileHead+pOutHeadPath;
    }
    return "assets:/res/defaultmember.png";
}

int EbClientApp::setMyAccountInfo(EbQuickAccountInfo *account)
{
    account->SetAccount(this->logonUserAccount());
    account->SetUserId(this->logonUserId());
    return m_ebum.EB_SetMyAccountInfo(account);
}

int EbClientApp::loadAreaInfo(int parentId, int parameter)
{
    return m_ebum.EB_GetAreaInfo(parentId, (unsigned int)parameter);
}

int EbClientApp::loadHeadEmotionInfo(int parameter)
{
    int result = 0;
    std::vector<EB_EmotionInfo> pEnterpriseImageList;
    m_ebum.EB_GetDefaultHeadList(pEnterpriseImageList);
    for (size_t i=0; i<pEnterpriseImageList.size(); i++) {
        const EB_EmotionInfo &pEmotionInfo = pEnterpriseImageList[i];
        if ( pEmotionInfo.m_sResFile.isEmpty() || !QFileInfo::exists(pEmotionInfo.m_sResFile) ) {
            continue;
        }
        result++;
        emit loadHeadEmotionInfoResponse(new EbQuickEmotionInfo(&pEmotionInfo), parameter);
    }
    return result;
}

int EbClientApp::loadMyEmotionInfo(int parameter)
{
    std::vector<EB_EmotionInfo> pEnterpriseImageList;
    m_ebum.EB_GetMyEmotionList(pEnterpriseImageList);
//    m_hasEmptyFile = false;
    for (size_t i=0; i<pEnterpriseImageList.size(); i++) {
        const EB_EmotionInfo &pEmotionInfo = pEnterpriseImageList[i];
        if (pEmotionInfo.m_sResFile.isEmpty()) {
//            m_hasEmptyFile = true;
            break;
        }
        if ( !QFileInfo::exists(pEmotionInfo.m_sResFile) ) {
//            m_hasEmptyFile = true;
            break;
        }
//        if (!m_loadedResourceIdList.insert(pEmotionInfo.m_sResId,true,false)) {
//            /// 前面已经添加，取下一个
//            continue;
//        }
        emit loadMyEmotionInfoResponse(new EbQuickEmotionInfo(&pEmotionInfo), parameter);
    }
    return (int)pEnterpriseImageList.size();
}

int EbClientApp::setMyGroupHeadImageResourceId(qint64 groupId, qint64 resourceId)
{
    return this->m_ebum.EB_SetMyGroupHeadRes(groupId, resourceId);
}

int EbClientApp::setMyGroupHeadImageFile(qint64 groupId, const QString &imageFile)
{
    QString imagePath(imageFile);
    if (imagePath.left(3)=="qrc") {
        imagePath = imagePath.mid(3);
    }
    else if (imagePath.left(theFileHead.size())==theFileHead) {
        imagePath = imagePath.mid(theFileHead.size());
    }

    QImage image(imagePath);
    if (image.isNull()) {
        return -10;
    }
    QString tempImageFile(imagePath);
    /// to MAX 320*320
    if (image.width()>image.height()) {
        const int min_height = MIN(image.height(), 320);
        if (image.height()>min_height) {
            image = image.scaled((image.width()*min_height)/image.height(), min_height,
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        /// 取中间四方形
        image = image.copy((image.width()-min_height)/2, 0, min_height, min_height);
    }
    else {
        const int min_width = MIN(image.width(), 320);
        if (image.width()>min_width) {
            image = image.scaled(min_width, (image.height()*min_width)/image.width(),
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        /// 取中间四方形
        image = image.copy(0, (image.height()-min_width)/2, min_width, min_width);
    }
    for (int i=0; i<100; i++) {
        tempImageFile = QString("%1/default_head_image_temp_%2.jpg").arg(userImagePath()).arg(i);
        if (image.save(tempImageFile)) {
            break;
        }
    }
    if (!QFile::exists(tempImageFile)) {
        return -11;
    }
    if (imagePath.indexOf(this->userImagePath())==0) {
        /// 删除临时图片文件
        QFile::remove(imagePath);
    }
    return this->m_ebum.EB_SetMyGroupHeadFile(groupId, tempImageFile);
}

//void EbClientApp::openGalleryFinished(const QString &uriFile)
//{
//    emit openGalleryResponse(uriFile, m_openGalleryParameter);
//}

#ifdef  Q_OS_ANDROID
void EbClientApp::handleActivityResult(int receiverRequestCode, int resultCode,
                                       const QAndroidJniObject &data)
{
    if(resultCode == -1 && 1 == receiverRequestCode) {
        if(data.isValid()) {
            // android/net/Uri
            // android/net/Uri getData()
            QAndroidJniObject uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
            const QString uriFile = getLocalPathFromUri(uri);

            emit openGalleryResponse(uriFile, m_openGalleryParameter);
//                notify->finished(getLocalPathFromUri(uri));
        } else {
//                notify->error("Data isn't valid");
        }
    } else {
//            notify->error(QString("resultCode:%1, Data: %2").arg(resultCode).arg(data.toString()));
    }
}

#define Q_SAFE_CALL_JAVA {                  \
    QAndroidJniEnvironment env;             \
    if(env->ExceptionCheck()) {             \
    qDebug() << "have a java exception";    \
    env->ExceptionDescribe();               \
    env->ExceptionClear();                  \
    }                                       \
    }



QString EbClientApp::getLocalPathFromUri(const QAndroidJniObject &uri)
{
//    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
//        super.onActivityResult(requestCode, resultCode, data);

//        if (requestCode == REQUEST_CODE&& resultCode == RESULT_OK && null != data) {
//            Uri selectedVideo = data.getData();
//            String[] filePathColumn = { MediaStore.Video.Media.DATA };

//            Cursor cursor = getContentResolver().query(selectedVideo ,
//                                                       filePathColumn, null, null, null);
//            cursor.moveToFirst();

//            int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
//            String videoPath = cursor.getString(columnIndex);
//            cursor.close();
//        }

//    Uri uri = data.getData();
//    String[] proj = { MediaStore.Images.Media.DATA };
//    Cursor actualimagecursor = managedQuery(uri,proj,null,null,null);
//    int actual_image_column_index = actualimagecursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
//    actualimagecursor.moveToFirst();
//    String img_path = actualimagecursor.getString(actual_image_column_index);
//    File file = new File(img_path);

    // uri: Landroid/net/Uri;
    // [url](http://stackoverflow.com/questions/34163437/qt-and-android-gallery-mediastore-using-qandroidjniobject)

    /// image=url	"content://media/external/images/media/287460"	QString
    /// video=url	"file:///storage/emulated/0/DCIM/Camera/VID_20170827_210548.mp4"	QString
    QString url = uri.toString();
    if (url.left(7)=="file://") {
        if (theFileHead.size()==8) {
            url.insert(8, "/");
        }
        return url;
    }
    QAndroidJniObject dadosAndroid
            = QAndroidJniObject::getStaticObjectField(
                "android/provider/MediaStore$MediaColumns",
                "DATA",
                "Ljava/lang/String;");

    QAndroidJniEnvironment env;
    jobjectArray projecao
            = (jobjectArray)env->NewObjectArray(1,
                                                env->FindClass("java/lang/String"),
                                                NULL);

    jobject projacaoDadosAndroid
            = env->NewStringUTF(dadosAndroid.toString().toStdString().c_str());

    env->SetObjectArrayElement(projecao, 0, projacaoDadosAndroid);

    QAndroidJniObject contentResolver
            = QtAndroid::androidActivity()
            .callObjectMethod("getContentResolver",
                              "()Landroid/content/ContentResolver;");

    QAndroidJniObject cursor = contentResolver.callObjectMethod(
                "query",
                "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;",
                uri.object<jobject>(), projecao, NULL, NULL, NULL);
//    Q_SAFE_CALL_JAVA
    /// 获取视频，这里异常
    jint columnIndex = cursor.callMethod<jint>("getColumnIndex",
                                               "(Ljava/lang/String;)I",
                                               dadosAndroid.object<jstring>());

    cursor.callMethod<jboolean>("moveToFirst", "()Z");

    QAndroidJniObject resultado
            = cursor.callObjectMethod("getString",
                                      "(I)Ljava/lang/String;",
                                      columnIndex);

    QString imagemCaminho = theFileHead + resultado.toString();
    return imagemCaminho;
}

#endif

void EbClientApp::openGallery(int parameter)
{
    m_openGalleryParameter = parameter;
    /// for test
//    emit openGalleryResponse("qrc:/res/entlogo.png", 0);
#ifdef  Q_OS_ANDROID
    // android.content.Intent.ACTION_GET_CONTENT
    QAndroidJniObject action = QAndroidJniObject::getStaticObjectField(
                "android/content/Intent",
                "ACTION_GET_CONTENT",
                "Ljava/lang/String;");

    QAndroidJniObject intent("android/content/Intent",
                             "(Ljava/lang/String;)V",
                             action.object<jstring>());
    Q_SAFE_CALL_JAVA
    // Intent setType(String type)
    //intent.setType(“video/;image/”);//同时选择视频和图片
    /// video 有异常
    const QString fileType = parameter==2?"video/*;image/*":"image/*";
    QAndroidJniObject type = QAndroidJniObject::fromString(fileType);
//    QAndroidJniObject type = QAndroidJniObject::fromString("image/*");
//    QAndroidJniObject type = QAndroidJniObject::fromString("video/*");
    intent.callObjectMethod("setType",
                            "(Ljava/lang/String;)Landroid/content/Intent;",
                            type.object<jstring>());

    Q_SAFE_CALL_JAVA
    QtAndroid::startActivity(intent, 1, this);
#endif
}

int EbClientApp::loadSessionInfo(int nLoadCount, int parameter)
{
//    /// for test data
//    EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
//    sessionInfo->setGroupId(999114);
//    sessionInfo->setFromName("group name");
//    emit loadSessionInfoResponse(sessionInfo, parameter);
//    for (int i=0; i<10; i++) {
//        sessionInfo = new EbQuickSessionInfo();
//        sessionInfo->setFromUserId(80+i);
//        sessionInfo->setFromName("abcd abc");
//        emit loadSessionInfoResponse(sessionInfo, parameter);
//    }
    char sql[256];
    sprintf( sql, "SELECT call_id,Datetime(call_time,'localtime'),dep_code,dep_name,emp_code,from_uid,"
             "from_phone,from_account,from_name,from_type,company,title,tel,email "
             "FROM call_record_t ORDER BY call_time desc LIMIT %d", nLoadCount);
    int nCookie = 0;
    const int nCount = (int)m_sqliteUser->select(sql, nCookie);
//    m_bReloadRecordData = nCount==50?true:false;
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
        sessionInfo->setCallId(pRecord->getVector()[0]->getBigIntValue());
        const tstring sCallTime = pRecord->getVector()[1]->getStrValue();
        time_t nCallTime = 0;
        ChangeTime(sCallTime.c_str(),nCallTime);
        sessionInfo->setTime(nCallTime);
        sessionInfo->setGroupId(pRecord->getVector()[2]->getBigIntValue());
        sessionInfo->setGroupName(pRecord->getVector()[3]->getStrValue().c_str());
        sessionInfo->setMemberId(pRecord->getVector()[4]->getBigIntValue());
        sessionInfo->setFromUserId(pRecord->getVector()[5]->getBigIntValue());
        sessionInfo->setFromPhone(pRecord->getVector()[6]->getBigIntValue());
        sessionInfo->setFromAccount(pRecord->getVector()[7]->getStrValue().c_str());
        sessionInfo->setFromName(pRecord->getVector()[8]->getStrValue().c_str());
//        const QString fromName(pRecord->getVector()[8]->getStrValue().c_str());
        sessionInfo->setFromType(pRecord->getVector()[9]->getIntValue());
        sessionInfo->setCompany(pRecord->getVector()[10]->getStrValue().c_str());
        sessionInfo->setTitle(pRecord->getVector()[11]->getStrValue().c_str());
        sessionInfo->setTel(pRecord->getVector()[12]->getStrValue().c_str());
        sessionInfo->setEmail(pRecord->getVector()[13]->getStrValue().c_str());
//        if (sessionInfo->isGroupCall() || sessionInfo->isMsgSession()) {
//            sessionInfo->setFromName(fromName);
//        }
//        else if (sessionInfo->fromUserId()>0) {
//            sessionInfo->setFromName(userName(sessionInfo->fromUserId()));
//        }
//        else if (!sessionInfo->fromAccount().isEmpty()) {
//            sessionInfo->setFromName(userName(sessionInfo->fromAccount()));
//        }
//        if (sessionInfo->fromName().isEmpty()) {
//            sessionInfo->setFromName(fromName);
//        }
        if (sessionInfo->isMsgSession()) {
            const bool isRead = (sessionInfo->fromPhone()&EBC_READ_FLAG_TRUE)==0?false:true;
            sessionInfo->setRead(isRead);
        }
//        insertCallRecord(pCallRecordInfo,false,(m_listWidget->count()+1)==nCount?true:false);
        pRecord = m_sqliteUser->next(nCookie);
        sendSessionInfo(sessionInfo, parameter);
    }
    m_sqliteUser->reset(nCookie);
    return nCount;
}

void EbClientApp::setSessionIsRead(qint64 callId)
{
    /// ** 用于消息广播，消息通知==
    char sql[256];
    sprintf(sql,"UPDATE call_record_t SET from_phone=%d WHERE call_id=%lld AND emp_code=-1 AND from_phone=0",
            (int)EBC_READ_FLAG_TRUE, callId);
    m_sqliteUser->execute(sql);
}

int EbClientApp::callAccount(const QString &account, qint64 existCallId)
{
    return m_ebum.EB_CallAccount(account.toStdString().c_str(), existCallId);
}

int EbClientApp::callUserId(qint64 userId, qint64 existCallId)
{
    return m_ebum.EB_CallUserId(userId, existCallId);
}

int EbClientApp::callGroupId(qint64 groupId)
{
    return m_ebum.EB_CallGroup(groupId);
}

EbQuickCallInfo *EbClientApp::findCallInfo(qint64 groupId,
                                           qint64 fromUserId,
                                           const QString &fromAccount)
{
    if (groupId==0 && fromUserId==0 && fromAccount.isEmpty()) {
        return 0;
    }
    AUTO_RLOCK(m_pCallList);
    CLockMap<eb::bigint, EbcCallInfo::pointer>::iterator iter = m_pCallList.begin();
    for (; iter!=m_pCallList.end(); iter++) {
        const EbcCallInfo::pointer &callInfo = iter->second;
        if ((groupId>0 && groupId==callInfo->groupId()) ||
                (groupId==0 && !callInfo->isGroupCall() &&
                 ((fromUserId>0 && fromUserId==callInfo->fromUserId()) ||
                  (fromAccount.isEmpty() &&
                   fromAccount.toStdString()==callInfo->fromUserAccount().string())))){
            /// 当前会话
            return new EbQuickCallInfo(callInfo.get());
        }
    }
    return 0;
}


void insertUrl(QString &pOutRichText, const QString &url)
{
    /// 后面增加一个空格，避免后面字符同样是链接点击问题
    if (url.left(4).toLower()=="http") {
        pOutRichText += QString("<a href=\"%1\">%2</a> ").arg(url).arg(url);
    }
    else {
        pOutRichText += QString("<a href=\"http://%1\">%2</a> ").arg(url).arg(url);
    }
}
void insertPlainTextEb(QString &pOutRichText, const QString &text,const QColor & color)
{
    QString sString1(text);
    int nfind1 = sString1.indexOf("http://",Qt::CaseInsensitive);
    if (nfind1 < 0)
        nfind1 = sString1.indexOf("https://",Qt::CaseInsensitive);
    if (nfind1 < 0)
        nfind1 = sString1.indexOf("www.",Qt::CaseInsensitive);
    if (nfind1>=0) {
        if (nfind1 > 0) {
            pOutRichText += sString1.left(nfind1);
//            setTextColor(color);
//            QTextBrowser::insertPlainText( sString1.left(nfind1) );
            sString1 = sString1.mid( nfind1 );
        }
        nfind1 = -1;
        //nfind1 = sString1.FindOneOf(L" \t\r\n;,；，。");
        //bool bFindBigChar = false;
        //if (nfind1 < 0)
        {
            /// 判断中文和 HTML 链接结束符
            const int nCount = sString1.count();
            for(int i=0; i<nCount ; i++)
            {
                QChar cha = sString1.at(i);
                ushort uni = cha.unicode();
                if(uni>=0x4E00 && uni<=0x9FA5) {
                    /// 这个字符是中文
                    nfind1 = i;
                    break;
                }
                else if (cha==160 || cha=='\t' || cha=='\r' || cha=='\n' || cha==';' || cha==',' || cha==' ') {
                    /// HTML 链接结束符
                    /// 160=' '
                    nfind1 = i;
                    break;
                }
            }
        }

        if (nfind1 > 0) {
            insertUrl(pOutRichText, sString1.left(nfind1));
            sString1 = sString1.mid(nfind1);
            insertPlainTextEb(pOutRichText, sString1, color);
            return;
        }
        else {
            insertUrl(pOutRichText, sString1);
            return;
        }
    }
    else {
        pOutRichText += text;
//        setTextColor(color);
//        QTextBrowser::insertPlainText(text);
    }
}

bool EbClientApp::insertImage(QString &pOutRichText, const QString &filePath, const QString &alt,
                 int maxWidth, int maxHeight) const
{
    QImage image(filePath);
    int imageWidth = image.width();
    int imageHeight = image.height();
    if (imageWidth==0 || imageHeight==0) {
        /// 判断是否 gif 图片
        const QMovie gif(filePath);
        if (!gif.isValid()) {
            return false;
        }
        image = gif.currentImage();
        if (!QFile::exists(filePath)) {
            return false;
        }
        imageWidth = 24;//image.width();
        imageHeight = 24;//image.height();
    }
    if (imageWidth>=46 && imageWidth<=82 && imageHeight>=46 && imageHeight<=82) {
        /// ?应该是表情
        maxWidth = 24;
        maxHeight = 24;
    }

    const int const_max_image_width = MIN(m_maxWidth, maxWidth);///640;
    const int const_max_image_height = maxHeight;///480;
    if (imageWidth>const_max_image_width) {
        const int width = const_max_image_width;
        const int height = (imageHeight*const_max_image_width)/imageWidth;
        const QString html = QString("<img src=\"%1%2\" width=\"%3\" height=\"%4\" alt=\"%5\"></img>")
                .arg(theFileHead).arg(filePath).arg(width).arg(height).arg(alt);
        pOutRichText += html;
    }
    else if (imageHeight>const_max_image_height) {
        const int height = const_max_image_height;
        const int width = (imageWidth*const_max_image_height)/imageHeight;
        const QString html = QString("<img src=\"%1%2\" width=\"%3\" height=\"%4\" alt=\"%5\"></img>")
                .arg(theFileHead).arg(filePath).arg(width).arg(height).arg(filePath);
        pOutRichText += html;
    }
    else {
        const QString html = QString("<img src=\"%1%2\" alt=\"%3\"></img>")
                .arg(theFileHead).arg(filePath).arg(filePath);
        pOutRichText += html;
    }
    return true;
}

bool EbClientApp::ChangeTime(const char* sTimeString, time_t& pOutTime, int* pOutMS)
{
    int nMS = 0;
    tm pTm;
    memset(&pTm,0,sizeof(pTm));
    int nRet = ::sscanf(sTimeString,"%d-%d-%d %d:%d:%d.%d",&pTm.tm_year,&pTm.tm_mon,&pTm.tm_mday,&pTm.tm_hour,&pTm.tm_min,&pTm.tm_sec,&nMS);
    if (nRet != EOF)
    {
        pTm.tm_year -= 1900;
        pTm.tm_mon -= 1;
        pOutTime = mktime(&pTm);
        if (pOutMS != NULL)
            *pOutMS = nMS;
        return true;
    }
    return false;
}

qint64 GetFileSize(const QString &lpszFile)
{
    QFile file(lpszFile);
    if ( !file.open(QFile::ReadOnly) ) {
        return -1;
    }
    const qint64 result = file.size();
    file.close();
    return result;
}


void EbClientApp::writeFileMessage(QString &richText,
                                   qint64 resourceId,
                                   const QString &filePath,
                                   eb::bigint fileSizeOrg)
{
    const eb::bigint fileSize = fileSizeOrg>0?fileSizeOrg:(eb::bigint)GetFileSize(filePath);
    if (fileSize>0) {
        if (insertImage(richText, filePath, filePath)) {
            richText += "<br>";
        }
    }
//    else if (resourceId==0){
//        QFileIconProvider icon_provider;
//        QIcon icon = icon_provider.icon( QFileInfo(filePath) );
//        this->document()->addResource(QTextDocument::ImageResource, QUrl(filePath), icon.pixmap(24,24));
//        this->textCursor().insertImage(filePath);
////        QString sTemp = QString("<img src=\"%1\" alt=\"%2\"/><br>").arg(filePath).arg(filePath);
////        this->insertHtml(sTemp);
//    }
    const tstring fileName = GetFileName(filePath.toStdString());
//    if (pOutMsgText!=0) {
//        *pOutMsgText = fileName.c_str();
//    }

    /// "上传群共享文件："
    char textTemp[128];
    memset(textTemp, 0, sizeof(textTemp));
//    if (showNameOnly) {
//        strcpy(textTemp, " : ");
//    }
//    else if (m_callInfo->isGroupCall() && resourceId>0) {
//        sprintf( textTemp, "%s: ", theLocales.getLocalStdString("chat-msg-text.send-group-file","Send Group File").c_str() );
//    }
//    EbTextBlockUserData * userData = updateBlockMsgId(msgId);
//    userData->setFlags( userData->flags()|EbTextBlockUserData::FileMessage );
//    userData->setFilePath(filePath);
    char fileText[260];

    if (resourceId==0 && fileSize == -1)    /// 文件不存在
        sprintf(fileText,"%s%s (%s)",textTemp,fileName.c_str(),m_locales->getLocalStdString("chat-msg-text.file-not-exist","File not exist").c_str());
    else if (fileSize >= const_gb_size)
        sprintf(fileText,"%s%s (%.02fGB)",textTemp,fileName.c_str(),(double)fileSize/const_gb_size);
    else if (fileSize >= const_mb_size)
        sprintf(fileText,"%s%s (%.02fMB)",textTemp,fileName.c_str(),(double)fileSize/const_mb_size);
    else if (fileSize >= const_kb_size)
        sprintf(fileText,"%s%s (%.02fKB)",textTemp,fileName.c_str(),(double)fileSize/const_kb_size);
    else if (fileSize>=0)
        sprintf(fileText,"%s%s (%lldByte)",textTemp,fileName.c_str(),fileSize);
    else if (resourceId>0)
        sprintf(fileText,"%s%s",textTemp,fileName.c_str());
    richText += fileText;
//    if (!receive && m_callInfo->isGroupCall() && resourceId>0) {
//        this->insertPlainText(fileText);
//    }
//    else {
//        const QString text = QString(" <a href=\"%1:///%2\">%3</a> ").arg(theOpenFile).arg(filePath).arg(fileText);
//        this->insertHtml(text);
//    }

//    if (!receive && m_callInfo->isGroupCall() && resourceId>0) {
//        /// 下载
//        const QString text = QString(" <a href=\"%1:///0,%2,%3\">%4</a> ")
//                .arg(theDownloadResource).arg(resourceId).arg(fileName.c_str())
//                .arg(m_locales->getLocalText("chat-msg-text.download","Download"));
//        this->insertHtml(text);
//        if (m_callInfo->isGroupCall() && !theApp->isDisableGroupShareCloud()) {
//            /// 群共享
//            const QString text = QString(" <a href=\"%1:///%2\">%3</a>")
//                    .arg(theOpenSubId).arg(constGroupShareSubId)
//                    .arg(m_locales->getLocalText("chat-msg-text.group-share","Group share"));
//            this->insertHtml(text);
//        }
//    }

}

bool EbClientApp::writeCardDataMessage(QString &richText, const char *cardData, QString *pOutUserName)
{
    int nCardType = 0;
    tstring sCardData;
    m_ebum.EB_ParseCardInfo(cardData,nCardType,sCardData);
    EB_ECardInfo pUserECard;
    if (nCardType==1 && m_ebum.EB_GetUserECardByFromInfo(sCardData,&pUserECard)) {
        /// ** 用户名片（个人名片）
        if (pOutUserName!=0) {
            *pOutUserName = pUserECard.m_sAccountName.c_str();
        }
        const QString sFilePath = userHeadImage2(pUserECard.m_nMemberUserId, pUserECard.m_sHeadResourceFile);
        richText = QString("<img src=\"%1\" width=\"48\" height=\"48\" />%2")
                .arg(sFilePath)
                .arg(pUserECard.m_sAccountName.c_str());
        return true;
    }
    return false;
}

int EbClientApp::loadRichInfo(qint64 groupId, qint64 fromUserId)
{
    /// ORDER BY msg_time DESC LIMIT
    const bool desc = true;
    const int loadLastCount = 50;
    char sql[512];
    if (groupId>0) {
        sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t "
                      "WHERE dep_code=%lld ORDER BY msg_time DESC LIMIT %d",
                 groupId, loadLastCount);
    }
    else if (fromUserId>0) {
        sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t "
                      "WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time DESC LIMIT %d",
                 fromUserId, fromUserId, loadLastCount);
    }
    else {
        return 0;
    }
    EbQuickRichInfo * richInfo = NULL;
    QString richText;
//    tstring sToWriteString;
    bool receive = false;
    int nCookie = 0;
    mycp::bigint nLastWithdrawMsgId = 0;
    const mycp::bigint nRet = m_sqliteUser->select(sql, nCookie);
    cgcValueInfo::pointer pRecord = desc?m_sqliteUser->last(nCookie):m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        const tstring sMsgTime(pRecord->getVector()[0]->getStrValue());
        const eb::bigint msgId = pRecord->getVector()[1]->getBigIntValue();
//        const tstring soffTime(pRecord->getVector()[2]->getStrValue());
        const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
        const tstring sFromName(pRecord->getVector()[4]->getStrValue());
        const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
        const tstring sToName(pRecord->getVector()[6]->getStrValue());
        const int nPrivate = pRecord->getVector()[7]->getIntValue();
        const int nMsgType = pRecord->getVector()[8]->getIntValue();
        const tstring sMsgName(pRecord->getVector()[9]->getStrValue() );
        const tstring sMsgText(pRecord->getVector()[10]->getStrValue() );
        //theApp.m_pBoUsers->escape_string_out(sMsgName);
        //theApp.m_pBoUsers->escape_string_out(sMsgText);
        const unsigned int nMsgSize = sMsgText.size();
        const int nReadFlag = pRecord->getVector()[11]->getIntValue();
        pRecord = desc?m_sqliteUser->previous(nCookie):m_sqliteUser->next(nCookie);

        /// 前面未显示，并且当前文本不是最长文本，显示前面内容。
//        if (!richText.isEmpty() && msgId!=richInfo->msgId()) {
//            insertPlainTextEb(richText, richText, QColor(64,64,64) );
////        if (!sToWriteString.empty() && nMsgSize!=theMaxDatabaseTextSize) {
////            insertPlainTextEb(richText, QString::fromStdString(sToWriteString.string()),QColor(64,64,64) );
//            richInfo->setRichText(richText);
//            sendRichInfo(richInfo);
//            richInfo = 0;
//            richText.clear();
////            sToWriteString.clear();
//        }
//        else if (richInfo!=0 && !richText.isEmpty() && msgId!=richInfo->msgId()) {
//            richInfo->setRichText(richText);
//            sendRichInfo(richInfo);
//            richInfo = 0;
//            richText.clear();
//        }
        if (richInfo!=0 && !richText.isEmpty() && msgId!=richInfo->msgId()) {
            richInfo->setRichText(richText);
            sendRichInfo(richInfo);
            richInfo = 0;
            richText.clear();
        }
        receive = sFromAccount!=logonUserId()?true:false;
        time_t nMsgTime = 0;
        ChangeTime(sMsgTime.c_str(),nMsgTime);
//        const bool writeLeft = receive;
//        writeTitle( writeLeft,msgId,nPrivate==1,sFromAccount,sFromName,sToAccount,sToName,nMsgTime,nReadFlag );
//        addChatMsgBlock( msgId,receive );
        if (richInfo==0) {
            richInfo = new EbQuickRichInfo();
            richInfo->setGroupId(groupId);
            richInfo->setTime(nMsgTime);
            richInfo->setMsgId(msgId);
            richInfo->setSendFromUserId(sFromAccount);
            richInfo->setSendToUserId(sToAccount);
//            const QString crTextColor = writeLeft?"#42b475":( (!m_callInfo->isGroupCall() && (nReadFlag&EBC_READ_FLAG_RECEIPT
            const bool isRead = (nReadFlag&EBC_READ_FLAG_RECEIPT)==0?false:true;
            richInfo->setRead(isRead);
            richInfo->setSendFromName(sFromName.c_str());
            richInfo->setSendToName(sToName.c_str());
            richInfo->setPrivate(nPrivate==1?true:false);
            richInfo->setReceive(receive);
        }
        if ((nReadFlag&EBC_READ_FLAG_WITHDRAW)==EBC_READ_FLAG_WITHDRAW) {
            if (nLastWithdrawMsgId!=msgId) {
                nLastWithdrawMsgId = msgId;
                /// [撤回一条消息]
                const QString text = m_locales->getLocalText("withdraw-msg-response.state-ok-update","");
                const QString newText = QString("<font color=#808080>%1</font>").arg(text);
                richText = newText;
                richInfo->setRichText(richText);
                sendRichInfo(richInfo);
                richInfo = 0;
                richText.clear();
//                insertHtml(newText);
            }
            continue;
        }
        nLastWithdrawMsgId = 0;
//        QString sWindowText;
        switch (nMsgType) {
        case MRT_UNKNOWN: {
            break;
        }
        case MRT_TEXT: {
            /// 先临时保存，后面显示；处理分段保存长文本；
            const QString sRichTextTemp(sMsgText.c_str());
            richText = sRichTextTemp + richText;
//            if (sToWriteString.empty())
//                sToWriteString = sMsgText;
//            else
//                sToWriteString = sMsgText+sToWriteString;
            break;
        }
        case MRT_JPG: {
            /// 图片
            eb::bigint resourceId = 0;
            tstring resourceDesc;
            std::vector<tstring> pList;
            const tstring &sResourceInfo = sMsgText;
            if (ParseString(sResourceInfo.c_str(),";",pList)>=3) {
                resourceId = cgc_atoi64(pList[0].c_str());
                resourceDesc = pList.size()>3?pList[3]:"";
            }
            bool ret = false;
            QString sRichTextTemp;
            if (resourceId>0)
                ret = insertImage(sRichTextTemp, sMsgName.c_str(), resourceDesc.c_str());
            else
                ret = insertImage(sRichTextTemp, sMsgName.c_str(), sMsgText.c_str());
            richText = sRichTextTemp + richText;
            if (!ret) {
                const QString fileNotExist = "assets:/res/file_not_exist.png";
                const QString html = QString("<img src=\"%1\" width=\"24\" height=\"24\"></img>")
                        .arg(fileNotExist);
                richText = html + richText;
//                richInfo->setRichDesc(m_locales->getLocalStdString("chat-msg-text.file-not-exist","File not exist").c_str());
            }
            else {
                richInfo->setMsgType(2);
                richInfo->setMsgName(sMsgName.c_str());
            }
            break;
        }
        case MRT_MAP_POS:
            /// 地图位置
            break;
        case MRT_CARD_INFO:
            /// 名片信息
            writeCardDataMessage(richText, sMsgText.c_str());
            richInfo->setMsgType(4);
            richInfo->setMsgName(sMsgText.c_str());
            richInfo->setRichDesc(m_locales->getLocalText("chat-msg-text.user-ecard","User Ecard"));
            break;
        case MRT_USER_DATA:
            /// 用户自定义数据
            break;
        case MRT_WAV:
            /// 语音消息
            writeVoiceMessage(richText, sMsgName.c_str());
            richInfo->setMsgType(3);
            richInfo->setMsgName(sMsgName.c_str());
            break;
        case MRT_RESOURCE: {
            /// 资源文件
            const std::string::size_type nFind = sMsgText.find(",");
            if (nFind == std::string::npos) {
                break;
            }
            const QString sFileName = sMsgName.c_str();
            const mycp::bigint nResourceId = cgc_atoi64(sMsgText.substr(0,nFind).c_str());
            const mycp::bigint nFileSize = cgc_atoi64(sMsgText.substr(nFind+1).c_str());
            QString sRichTextTemp;
            writeFileMessage(sRichTextTemp, nResourceId, sFileName, nFileSize);
            richText = sRichTextTemp+richText;
            richInfo->setMsgType(1);
            richInfo->setMsgName(sFileName);
            break;
        }
        case MRT_FILE: {
            /// 文件
            const QString sFileName = sMsgText.c_str();
            writeFileMessage(richText, 0, sFileName, 0);
            richInfo->setMsgType(1);
            richInfo->setMsgName(sFileName);
            break;
        }
        default:
            break;
        }
    }
    m_sqliteUser->reset(nCookie);

    if (richInfo!=0 && !richText.isEmpty()) {
        richInfo->setRichText(richText);
        sendRichInfo(richInfo);
        richInfo = 0;
        richText.clear();
    }
//    else if (!sToWriteString.empty()) {
//        insertPlainTextEb(richText, QString::fromStdString(sToWriteString.string()),QColor(64,64,64) );
//        richInfo->setRichText(richText);
//        sendRichInfo(richInfo);
//        richInfo = 0;
//        richText.clear();
//        sToWriteString.clear();
//    }

    AUTO_RLOCK(m_pCallList);
    CLockMap<eb::bigint, EbcCallInfo::pointer>::iterator iter = m_pCallList.begin();
    for (; iter!=m_pCallList.end(); iter++) {
        const EbcCallInfo::pointer &callInfo = iter->second;
        if ((groupId>0 && groupId==callInfo->groupId()) ||
                (groupId==0 && !callInfo->isGroupCall()
                 && fromUserId==callInfo->fromUserId())){
            /// 当前会话
            AUTO_RLOCK(callInfo->m_progressFiles);
            CLockMap<qint64, EbFileInfo::pointer>::iterator iterFiles = callInfo->m_progressFiles.begin();
            for (; iterFiles!=callInfo->m_progressFiles.end(); iterFiles++) {
                const EbFileInfo::pointer &ebFileInfo = iterFiles->second;

                QString richText = getFileText(ebFileInfo.get());
                EbQuickRichInfo *richInfo = new EbQuickRichInfo(ebFileInfo.get());
                richInfo->setProgressValue(ebFileInfo->progressValue());
                richInfo->setRichText(richText);
            //    richInfo->setGroupId(groupId);
            //    richInfo->setTime(nMsgTime);
            //    richInfo->setSendFromName(sFromName.c_str());
            //    richInfo->setSendToName(sToName.c_str());
                richInfo->setReceive(ebFileInfo->isReceive());
                richInfo->setRead(false);
                emit receivedRichInfo(richInfo, "0");
            }
            break;
        }
    }

//    /// for test
//    richInfo = new EbQuickRichInfo();
//    richInfo->setMsgType(1);
//    richInfo->setSendFromUserId(80);
//    richInfo->setRichText("小孩文档asdfj;asdjf;adskf.doc");
//    richInfo->setProgressValue(100);
//    richInfo->setRichDesc("完成");
//    richInfo->setReceive(true);
//    emit receivedRichInfo(richInfo, 0);
    return 0;
}

int EbClientApp::sendText(qint64 callId, const QString &text, qint64 toUserId, bool bPrivate)
{
    if (text.isEmpty()) {
        return -1;
    }
    EB_ChatRoomRichMsg richMsg;
    richMsg.AddText(text.toStdString().c_str());
    return m_ebum.EB_SendRich(callId, &richMsg, toUserId, bPrivate);
}

bool EbClientApp::formatInputMsg(QQuickTextDocument *textDocument,
                                 EB_ChatRoomRichMsg *pOutMsgFormat) const
{
    QTextDocument *document = textDocument->textDocument();
    if (document->isEmpty()) {
        return false;
    }

    QTextBlock currentBlock = document->begin();
    QTextBlock::iterator it;
//    QTextCursor cursor = ui.textEditInput->textCursor();

    std::string sFindText;
    while( true) {
        /// 在修改chatformat时会改变当前Block的fragment
        for (it = currentBlock.begin(); !(it.atEnd()); ) {
            QTextFragment currentFragment = it.fragment();
            QTextImageFormat newImageFormat = currentFragment.charFormat().toImageFormat();

            if (newImageFormat.isValid()) {
                /// 判断出这个fragment为image
                if ( !sFindText.empty() ) {
                    pOutMsgFormat->AddText( sFindText.c_str() );
                    sFindText.clear();
                }
                QString resourceName = newImageFormat.name();
                if (resourceName.left(theFileHead.size())==theFileHead) {
                    resourceName = resourceName.mid(theFileHead.size());
                }
                QFile fileImage(resourceName);
                if (fileImage.open(QFile::ReadOnly)) {
                    /// 是本地图片文件
                    /// 判断是否本地资源文件
                    qint64 resourceId = 0;
                    if (resourceName.indexOf(this->appResPath())==0) {
                        resourceId = resourceName.mid(this->appResPath().size()+1).toLongLong();
                    }
                    ///
                    tstring resourceCmInfo;
                    if (resourceId>0 && m_ebum.EB_GetResourceCmInfo(resourceId, resourceCmInfo)) {
                        fileImage.close();
                        pOutMsgFormat->AddResource(resourceCmInfo.c_str());
                    }
                    else {
                        QByteArray data = fileImage.readAll();
                        fileImage.close();
                        const int dataSize = data.size();
                        if (dataSize>0) {
                            char * imageData = new char[dataSize];
                            memcpy(imageData, data.constData(), dataSize);
                            EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)pOutMsgFormat->AddObject( imageData,(unsigned long)dataSize );
                            /// 用于本端显示
                            pMsgItem->SetText(resourceName.toStdString().c_str());
                        }
                    }
                }
                else if (resourceName.indexOf(theMimeDataResourceName)==0) {
                    /// 截图
                    const QVariant imageResource = document->resource( QTextDocument::ImageResource, QUrl(resourceName) );
                    if ( imageResource.isValid() ) {

                        char lpszBuffer[260];
                        sprintf(lpszBuffer,"%s/%08d%06d.jpg",userImagePath().toStdString().c_str(),
                                (int)time(0)%100000000,rand()%1000000);
                        const QString jpgPhotoFile(lpszBuffer);

                        const QVariant::Type imageResourceType = imageResource.type();
                        switch (imageResourceType) {
                        case QVariant::Pixmap: {
                            const QPixmap pixmap = imageResource.value<QPixmap>();
                            const int size = pixmap.width()*pixmap.height();
                            pixmap.save(jpgPhotoFile,0,getImageQuality(size));
                            break;
                        }
                        case QVariant::Image: {
                            const QImage image = imageResource.value<QImage>();
                            const int size = image.width()*image.height();
                            image.save(jpgPhotoFile,0,getImageQuality(size));
                            break;
                        }
//                        case QVariant::Icon: {
//                            const QIcon image = imageResource.value<QIcon>();
//                            rect.setWidth( image.width() );
//                            rect.setHeight( image.height() );
//                            break;
//                        }
                        case QVariant::Bitmap: {
                            const QBitmap bitmap = imageResource.value<QBitmap>();
                            const int size = bitmap.width()*bitmap.height();
                            bitmap.save(jpgPhotoFile,0,getImageQuality(size));
                            break;
                        }
                        default:
                            break;
                        }

                        QFile fileJpg(jpgPhotoFile);
                        if (fileJpg.open(QFile::ReadOnly)) {
                            QByteArray data = fileJpg.readAll();
                            fileJpg.close();
                            const int dataSize = data.size();
                            if (dataSize>0) {
                                char * imageData = new char[dataSize];
                                memcpy(imageData, data.constData(), dataSize);
                                EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)pOutMsgFormat->AddObject( imageData,(unsigned long)dataSize );
                                /// 用于本端显示
                                pMsgItem->SetText(jpgPhotoFile.toStdString().c_str());
                            }

                        }
                    }
                }
                else {
                    pOutMsgFormat->AddResource( resourceName.toStdString().c_str());
                }
                ++it;
                continue;
            }

            if (currentFragment.isValid()) {
                const QString strText = currentFragment.text();
                if ( !sFindText.empty() ) {
                    sFindText.append("\n");
                }
                sFindText.append( strText.toStdString() );
                ++it;
            }
            else {
                // ???
                break;
            }
        }

        currentBlock = currentBlock.next();
        if(!currentBlock.isValid())
            break;
    }
    if ( !sFindText.empty() ) {
        pOutMsgFormat->AddText( sFindText.c_str() );
    }
    return true;
}

void EbClientApp::notifyMaxWidth(int maxWidth)
{
    m_maxWidth = maxWidth;
}

//void EbClientApp::notifyCurrentChatInfo(qint64 currentChatGroupId, qint64 currentChatUserId)
//{
//    m_currentChatGroupId = currentChatGroupId;
//    m_currentChatUserId = currentChatUserId;
//}

void EbClientApp::setCurrentChatGroupId(qint64 v)
{
    if (m_currentChatGroupId!=v) {
        m_currentChatGroupId = v;
        if (m_currentChatGroupId>0) {
            m_currentChatUserId = 0;
        }
//        m_tLastMsgDayTime.setDate();
    }
}

void EbClientApp::setCurrentChatGroupIdString(const QString &v)
{
    const qint64 id = v.toLongLong();
    if (m_currentChatGroupId!=id) {
        m_currentChatGroupId = id;
        if (m_currentChatGroupId>0) {
            m_currentChatUserId = 0;
        }
//        m_tLastMsgDayTime.setDate();
    }
}

void EbClientApp::setCurrentChatUserId(qint64 v)
{
    if (m_currentChatUserId!=v) {
        m_currentChatUserId = v;
        if (m_currentChatUserId>0) {
            m_currentChatGroupId = 0;
        }
    }
}

void EbClientApp::setCurrentChatUserIdString(const QString &v)
{
    const qint64 id = v.toLongLong();
    if (m_currentChatUserId!=id) {
        m_currentChatUserId = id;
        if (m_currentChatUserId>0) {
            m_currentChatGroupId = 0;
        }
    }
}

int EbClientApp::sendRich(qint64 callId, QQuickTextDocument *textDocument, qint64 toUserId, bool bPrivate)
{
    EB_ChatRoomRichMsg richMsg;
    if ( !formatInputMsg(textDocument, &richMsg)) {
        return 1;
    }
    return m_ebum.EB_SendRich(callId, &richMsg, toUserId, bPrivate);
}

int EbClientApp::sendImage(qint64 callId, const QString &imageFile, qint64 toUserId, bool bPrivate)
{
    QString imagePath(imageFile);
    if (imagePath.left(3)=="qrc") {
        imagePath = imagePath.mid(3);
    }
    else if (imagePath.left(theFileHead.size())==theFileHead) {
        imagePath = imagePath.mid(theFileHead.size());
    }

    QImage image(imagePath);
    if (image.isNull()) {
        return -10;
    }
    QString tempImageFile(imagePath);
    /// to MAX 640*480
    if (image.width()>image.height()) {
        const int min_height = MIN(image.height(), 480);
        if (image.height()>min_height) {
            image = image.scaled((image.width()*min_height)/image.height(), min_height,
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }
    else {
        const int min_width = MIN(image.width(), 640);
        if (image.width()>min_width) {
            image = image.scaled(min_width, (image.height()*min_width)/image.width(),
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }
    static unsigned int theIndex = 0;
    tempImageFile = QString("%1/%2_%3_%4.jpg")
            .arg(userImagePath())
            .arg(callId)
            .arg(theIndex++)
            .arg(time(0));
    image.save(tempImageFile);
    QFile fileImage(tempImageFile);
    if (!fileImage.open(QFile::ReadOnly)) {
        return -11;
    }
    /// 是本地图片文件
    EB_ChatRoomRichMsg richMsg;
    QByteArray data = fileImage.readAll();
    fileImage.close();
    const int dataSize = data.size();
    if (dataSize>0) {
        char * imageData = new char[dataSize];
        memcpy(imageData, data.constData(), dataSize);
        EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)richMsg.AddObject(imageData, (unsigned long)dataSize);
        /// 用于本端显示
        pMsgItem->SetText(tempImageFile.toStdString().c_str());
    }
    if (imagePath.indexOf(this->userImagePath())==0) {
        /// 删除临时图片文件
        QFile::remove(imagePath);
    }
    return m_ebum.EB_SendRich(callId, &richMsg, toUserId, bPrivate);
}

int EbClientApp::sendVoice(qint64 callId, const QString &voiceFile, qint64 toUserId, bool bPrivate)
{
    QString imagePath(voiceFile);
    if (imagePath.left(3)=="qrc") {
        imagePath = imagePath.mid(3);
    }
    else if (imagePath.left(theFileHead.size())==theFileHead) {
        imagePath = imagePath.mid(theFileHead.size());
    }

    QFile fileImage(imagePath);
    if (!fileImage.open(QFile::ReadOnly)) {
        return -11;
    }
    /// 是本地图片文件
    EB_ChatRoomRichMsg richMsg;
    richMsg.SetSubType(EB_RICH_SUB_TYPE_AUDIO);
    QByteArray data = fileImage.readAll();
    fileImage.close();
    const int dataSize = data.size();
    if (dataSize>0) {
        char * imageData = new char[dataSize];
        memcpy(imageData, data.constData(), dataSize);
        EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)richMsg.AddObject(imageData, (unsigned long)dataSize);
        /// 用于本端显示
//        pMsgItem->SetText(tempImageFile.toStdString().c_str());
    }
    if (imagePath.indexOf(this->userFilePath())==0) {
        /// 删除临时图片文件
        QFile::remove(imagePath);
    }
    return m_ebum.EB_SendRich(callId, &richMsg, toUserId, bPrivate);
}

int EbClientApp::sendFile(qint64 callId, const QString &filePath, qint64 toUserId, bool bPrivate)
{
    QString filePathTemp(filePath);
    if (filePathTemp.left(3)=="qrc") {
        filePathTemp = filePathTemp.mid(3);
    }
    else if (filePathTemp.left(theFileHead.size())==theFileHead) {
        filePathTemp = filePathTemp.mid(theFileHead.size());
    }

    QFile file(filePathTemp);
    if (!file.open(QFile::ReadOnly)) {
        return -11;
    }
    file.close();
    return m_ebum.EB_SendFile(callId, filePathTemp, toUserId, bPrivate);
//    return m_ebum.EB_SendFile(callId, filePathTemp.toLocal8Bit().constData(), toUserId, bPrivate);
}

int EbClientApp::sendGroupTempFile(qint64 groupId, const QString &filePath)
{
    QString filePathTemp(filePath);
    if (filePathTemp.left(3)=="qrc") {
        filePathTemp = filePathTemp.mid(3);
    }
    else if (filePathTemp.left(theFileHead.size())==theFileHead) {
        filePathTemp = filePathTemp.mid(theFileHead.size());
    }

    QFile file(filePathTemp);
    if (!file.open(QFile::ReadOnly)) {
        return -11;
    }
    file.close();
    return m_ebum.EB_AddTempGroupFileRes(filePathTemp.toStdString().c_str(), "", groupId);
}

int EbClientApp::sendUserIdEChard(qint64 callId, qint64 userId)
{
    return m_ebum.EB_SendUserCard(callId, userId);
}

int EbClientApp::sendUserAccountEChard(qint64 callId, const QString &account)
{
    return m_ebum.EB_SendUserCard(callId, account.toStdString().c_str());
}

EbQuickECardInfo *EbClientApp::parserECardInfo(const QString &cardData) const
{
    ///
    int nCardType = 0;
    tstring sCardData;
    m_ebum.EB_ParseCardInfo(cardData.toStdString().c_str(), nCardType, sCardData);
    EB_ECardInfo pUserECard;
    if (nCardType==1 && m_ebum.EB_GetUserECardByFromInfo(sCardData, &pUserECard)) {
        return new EbQuickECardInfo(pUserECard);
    }
    else {
        return 0;
    }
}

int EbClientApp::cancelFile(qint64 callId, qint64 msgId)
{
    return m_ebum.EB_CancelFile(callId, msgId);
}

int EbClientApp::saveFile(qint64 callId, qint64 msgId, const QString &fileName)
{
    QString saveTo = QString("%1/%2").arg(this->userFilePath()).arg(fileName);
    int index = 1;
    while (QFile::exists(saveTo)) {
        saveTo = QString("%1/(%2)%3")
                .arg(this->userFilePath())
                .arg(index++)
                .arg(fileName);
    }
    return m_ebum.EB_AcceptFile(callId, msgId, saveTo);
}

int EbClientApp::call2Group(qint64 callId, const QString &account)
{
    m_tWaitCall2Group = time(0);
    return m_ebum.EB_Call2Group(callId, account.toStdString().c_str());
}

bool EbClientApp::isImageFile(const QString &filePath) const
{
    QString imagePath(filePath);
    if (imagePath.left(3)=="qrc") {
        imagePath = imagePath.mid(3);
    }
    else if (imagePath.left(theFileHead.size())==theFileHead) {
        imagePath = imagePath.mid(theFileHead.size());
    }

    QImage image(imagePath);
    if (image.isNull()) {
        return false;
    }
    return (image.width()>0 && image.height()>0)?true:false;
}

void EbClientApp::clearUnreadCount(qint64 groupId, qint64 fromUserId)
{
//    char sql[256];
//    if (m_callInfo->isGroupCall()) {
//        if (fromUserClick)
//            theApp->m_pAutoCallGroupIdList.remove(m_callInfo->m_pCallInfo.m_sGroupCode);
//        else if (theApp->m_pAutoCallGroupIdList.exist(m_callInfo->m_pCallInfo.m_sGroupCode)) return false;
//        sprintf(sql,"UPDATE msg_record_t SET read_flag=read_flag|1 WHERE dep_code=%lld AND (read_flag&1)=0",m_callInfo->m_pCallInfo.m_sGroupCode);
//    }
//    else {
//        if (fromUserClick)
//            theApp->m_pAutoCallFromUserIdList.remove(m_callInfo->m_pFromAccountInfo.GetUserId());
//        else if (theApp->m_pAutoCallFromUserIdList.exist(m_callInfo->m_pFromAccountInfo.GetUserId())) return false;
//        sprintf(sql,"UPDATE msg_record_t SET read_flag=read_flag|1 WHERE from_uid=%lld AND dep_code=0 AND (read_flag&1)=0",m_callInfo->m_pFromAccountInfo.GetUserId());
//    }
//    theApp->m_sqliteUser->execute(sql);

    char sql[256];
    if (groupId>0) {
        sprintf( sql, "UPDATE msg_record_t SET read_flag=read_flag|1 "
                      "WHERE dep_code=%lld AND (read_flag&1)=0",
                 groupId);
    }
    else if (fromUserId>0) {
        sprintf( sql, "UPDATE msg_record_t SET read_flag=read_flag|1 "
                      "WHERE dep_code=0 AND from_uid=%lld AND (read_flag&1)=0",
                 fromUserId);
    }
    else {
        return;
    }

    const int ret = (int)m_sqliteUser->execute(sql);
    if (ret>0) {
        emit clearSessionUnreadCount(QString::number(groupId),
                                     QString::number(fromUserId));
    }
}

bool EbClientApp::insertEmotion(QQuickTextDocument *textDocument, EbQuickEmotionInfo *emotionInfo)
{
//    QQuickTextEdit
    QTextDocument *document = textDocument->textDocument();
    const QString resourceFilePath = emotionInfo->resourceFile();
    QImage image;
    if (!resourceFilePath.isEmpty() && QFileInfo::exists(resourceFilePath)) {
        ///
        image = QImage(resourceFilePath);
    }
    if (image.isNull()) {
        return false;
    }
    /// 下面的addResource代码非常重要, 内存增加很少
    const QString resourceCmInfo = "abc";
    document->addResource(QTextDocument::ImageResource, QUrl(resourceCmInfo), image);
//    QSyntaxHighlighter a(document);
//    a.currentBlock()
//    a.currentBlock().te
//    textDocument-
//    m_textEditInput->textCursor().insertImage(resourceCmInfo);
    return true;
}

bool EbClientApp::voiceRecordStart(int maxSeconds)
{
    static unsigned int theIndex = 0;
    const QString wavFilePath = QString("%1/default_recorder_%2_%3.wav")
            .arg(this->userFilePath())
            .arg(time(0))
            .arg(theIndex++);
    return m_voiceRecorder.startRecorder(wavFilePath);
}

bool EbClientApp::isVoiceRecordStarted() const
{
    return m_voiceRecorder.isStarted();
}

int EbClientApp::voiceRecordStop(qint64 sendCallId)
{
    const unsigned int nDataSeconds = m_voiceRecorder.dataSeconds();
    const QString wavFilePath = m_voiceRecorder.filePath();
    m_voiceRecorder.stopRecorder();
    if (sendCallId==0 || nDataSeconds<1) {
        /// 取消发送，或者低于1秒语音也不发送
        QFile::remove(wavFilePath);
        return 0;
    }
    QFile fileVoice(wavFilePath);
    if (!fileVoice.open(QFile::ReadOnly)) {
        return -11;
    }

    EB_ChatRoomRichMsg richMsg;
    richMsg.SetSubType(EB_RICH_SUB_TYPE_AUDIO);
    QByteArray data = fileVoice.readAll();
    fileVoice.close();
    const int dataSize = data.size();
    char * imageData = new char[dataSize];
    memcpy(imageData, data.constData(), dataSize);
    EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)richMsg.AddObject(imageData, (unsigned long)dataSize);
    /// 用于本端显示
    pMsgItem->SetText(wavFilePath.toStdString().c_str());
    return m_ebum.EB_SendRich(sendCallId, &richMsg, 0, false);
}

void EbClientApp::playWavFile(const QString &filePath)
{
    QString filePathTemp(filePath);
    if (filePathTemp.left(3)=="qrc") {
        filePathTemp = filePathTemp.mid(3);
    }
    else if (filePathTemp.left(theFileHead.size())==theFileHead) {
        filePathTemp = filePathTemp.mid(theFileHead.size());
    }
    QSound::play(filePathTemp);
}

//static JavaVM* g_javaVM = 0;
//static jclass g_testClassID   = 0;          //类ID
//static jmethodID g_poenFileID = 0;        //静态方法
//static jmethodID g_mystaticID = 0;        //静态方法
//static jmethodID g_getNumID = 0;          //无参方法
//static jmethodID g_setNumID = 0;          //有参方法
//static jmethodID g_getStrID = 0;          //参数和返回值为类对象
//static jmethodID g_boolTestID = 0;        //返回值为bool型
//static jmethodID g_constMethodID = 0;   //构造函数ID
//JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *)
//{
//    JNIEnv *env;
//    //[1] 指定JNI 版本，利用VM获得 env指针
//    if(vm->GetEnv(reinterpret_cast<void **>(&env),JNI_VERSION_1_6) != JNI_OK)
//    {
//        qCritical() << "Can't get the enviroument";
//        return -1;
//    }
//    //保存获得指针以便在任意上下文中使用
//    g_javaVM = vm;

//    //[2] 获取类ID
//    jclass clazz = env->FindClass("org/qtproject/entboost");
//    if(!clazz)
//    {
//        qCritical() << "Can't find class MyJavaClass";
//        return -1;
//    }

//    g_testClassID = (jclass)env->NewGlobalRef(clazz);

//     // search for its contructor

////    g_constMethodID = env->GetMethodID(g_testClassID, "<init>", "()V");
////    if (!g_constMethodID)
////    {
////        qCritical()<<"Can't find  class contructor";
////        return -1;
////    }

////    g_mystaticID = env->GetStaticMethodID(g_testClassID,"mystatic","(I)I");
////    if(!g_mystaticID)
////    {
////        qCritical() << "Can't find static method "<< endl;
////        return -1;
////    }


////    g_getNumID = env->GetMethodID(g_testClassID,"getNum","()I");
////    if(!g_getNumID)
////    {
////        qCritical() << "Can't find getNum method "<< endl;
////        return -1;
////    }

////    g_boolTestID = env->GetMethodID(g_testClassID,"boolTest","()Z");
////    if(!g_boolTestID)
////    {
////        qCritical() << "Can't find booltest method "<< endl;
////        return -1;
////    }

////    g_setNumID = env->GetMethodID(g_testClassID,"setNum","(I)I");
////    if(!g_setNumID)
////    {
////        qCritical() << "Can't find setNUM method "<< endl;
////        return -1;
////    }

////    g_getStrID = env->GetMethodID(g_testClassID,"getStr", "(Ljava/lang/String;)Ljava/lang/String;");
////    if(!g_getStrID)
////    {
////        qCritical() << "Can't find GetStr Method "<< endl;
////        return -1;
////    }
////    g_poenFileID = env->GetMethodID(g_testClassID,"openFile", "(Ljava/lang/String;)Z");
////    if(!g_poenFileID)
////    {
////        qCritical() << "Can't find GetStr Method "<< endl;
////        return -1;
////    }

//    qDebug() << "JNI Load Success !";
//    return JNI_VERSION_1_6;
//}
void EbClientApp::openFile(const QString &filePath)
{
    QString filePathTemp(filePath);
    if (filePathTemp.left(3)=="qrc") {
        filePathTemp = filePathTemp.mid(3);
    }
    else if (filePathTemp.left(theFileHead.size())==theFileHead) {
        filePathTemp = filePathTemp.mid(theFileHead.size());
    }
#ifdef Q_OS_ANDROID
//    if (g_javaVM==0) {
//        ///
//    }

//    int a2 = QAndroidJniObject::callStaticMethod<jint>
//            ("org/qtproject/entboost/OpenFiles" // class name
//             , "fibonacci" // method name
//             , "(I)I" // signature
//             , 12);

//    int a3 = QAndroidJniObject::callStaticMethod<jint>
//            ("org/qtproject/entboost/OpenFiles" // class name
//             , "getInt1" // method name
//             , "(I)I" // signature
//             , 12);
    QAndroidJniObject param1 = QAndroidJniObject::fromString(filePathTemp);
//    int a3 = QAndroidJniObject::callStaticMethod<jint>
//            ("org/qtproject/entboost/OpenFiles" // class name
//             , "setString" // method name
//             , "(Ljava/lang/String;)I" // signature
//             , param1.object<jstring>());
//    QAndroidJniObject a4 = QAndroidJniObject::callStaticObjectMethod
//            ("org/qtproject/entboost/OpenFiles" // class name
//             , "getString" // method name
//             , "(Ljava/lang/String;)Ljava/lang/String;" // signature
//             , param1.object<jstring>());
//    QString str = a4.toString();
    QAndroidJniObject Intent = QAndroidJniObject::callStaticObjectMethod
            ("org/qtproject/entboost/OpenFiles" // class name
             , "getImageFileIntent" // method name
             , "(Ljava/lang/String;)Landroid/content/Intent;" // signature
             , param1.object<jstring>());
    QtAndroid::startActivity(Intent, 0);

    //        QAndroidJniObject intent("android/content/Intent","()V");
    //        intent.callObjectMethod("setClassName",
    //                                "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
    //                                param1.object<jstring>(), param2.object<jstring>());
    //        QtAndroid::startActivity(intent, 0);
/*    QAndroidJniEnvironment env;
    QAndroidJniObject param1 = QAndroidJniObject::fromString(filePath);
    QAndroidJniObject VideoCaptureAndroid_JavaObj("org/qtproject/entboost/OpenFiles");
    int a1 = VideoCaptureAndroid_JavaObj.callObjectMethod<jint>
            ("getInt1",
             "(I)I", // signature
             50);
//    QString str = stringJinObj.toString();
*/

//    QAndroidJniObject param1 = QAndroidJniObject::fromString(filePath);
//    QAndroidJniObject a = QAndroidJniObject::callStaticMethod<jstring>
//            ("org/qtproject/entboost/OpenFiles" // class name
//             , "getString" // method name
//             , "(Ljava/lang/String;)Ljava/lang/String;" // signature
//             , param1.object<jstring>());
    int i=0;
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
#endif
}

//QString EbClientApp::getOpenFileUrl(const QString &filePath)
//{
//    return QUrl::fromLocalFile(filePath).toString();
//}

bool EbClientApp::isCanWithdrawMessage(qint64 msgId) const
{
    if (msgId==0 || m_sqliteUser.get()==0) {
        return false;
    }
    char sSql[256];
    sprintf( sSql, "select from_uid,read_flag,msg_type FROM msg_record_t WHERE msg_id=%lld LIMIT 1", msgId);
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->last(nCookie);
    if (pRecord.get()==0) {
        return false;
    }
    const eb::bigint m_msgFromUserId = pRecord->getVector()[0]->getBigIntValue();
    const int m_msgReadFlag = pRecord->getVector()[1]->getIntValue();
    const int m_msgType = pRecord->getVector()[2]->getIntValue();
    m_sqliteUser->reset(nCookie);
    if (m_msgFromUserId==logonUserId() &&
            m_msgType!=(int)MRT_RESOURCE &&
            (m_msgReadFlag&EBC_READ_FLAG_WITHDRAW)==0) {
        return true;
    }
    else {
        return false;
    }
}

int EbClientApp::withdrawMessage(qint64 callId, qint64 msgId)
{
    return m_ebum.EB_RequestWithdrawMsg(callId, msgId);
}

bool EbClientApp::isCanForwardMessage(qint64 msgId) const
{
    if (msgId==0 || m_sqliteUser.get()==0) {
        return false;
    }
    char sSql[256];
    sprintf(sSql, "select msg_type,msg_name,msg_text FROM msg_record_t " \
            "WHERE msg_id=%lld AND (read_flag&%d)=0 LIMIT 1",
            msgId,
            (int)EBC_READ_FLAG_WITHDRAW);
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    if (pRecord.get()==0) {
        return false;
    }
    const int nMsgType = pRecord->getVector()[0]->getIntValue();
    const tstring sMsgName(pRecord->getVector()[1]->getStrValue() );
    const tstring sMsgText(pRecord->getVector()[2]->getStrValue() );
    m_sqliteUser->reset(nCookie);
    switch (nMsgType) {
    case MRT_UNKNOWN: {
        break;
    }
    case MRT_TEXT: {
        /// 先临时保存，后面显示；处理分段保存长文本；
        return true;
    }
    case MRT_JPG: {
        /// 图片/表情
        eb::bigint resourceId = 0;
        tstring resourceDesc;
        std::vector<tstring> pList;
        const tstring &sResourceInfo = sMsgText;
        if (ParseString(sResourceInfo.c_str(),";",pList)>=3) {
            resourceId = cgc_atoi64(pList[0].c_str());
            resourceDesc = pList.size()>3?pList[3]:"";
        }
        if (resourceId>0) {
            tstring resourceCmInfo;
            return this->m_ebum.EB_GetResourceCmInfo(resourceId, resourceCmInfo);
        }
        else {
            /// filepath=sMsgName
            return QFileInfo::exists(QString::fromStdString(sMsgName.string()));
        }
        break;
    }
    case MRT_MAP_POS:
        /// 地图位置
        break;
    case MRT_CARD_INFO: {
        /// 名片信息
        int nCardType = 0;
        tstring sCardData;
        m_ebum.EB_ParseCardInfo(sMsgText.c_str(),nCardType,sCardData);
        EB_ECardInfo pUserECard;
        return (nCardType==1 && m_ebum.EB_GetUserECardByFromInfo(sCardData, &pUserECard));
    }
    case MRT_USER_DATA:
        /// 用户自定义数据
        break;
    case MRT_WAV:
        /// 语音消息
        /// filepath=sMsgName
        return QFileInfo::exists(QString::fromStdString(sMsgName.string()));
    case MRT_RESOURCE: {
        /// 资源文件
        const std::string::size_type nFind = sMsgText.find(",");
        if (nFind == std::string::npos) {
            break;
        }
//        const QString sFileName = sMsgName.c_str();
//        const mycp::bigint nResourceId = cgc_atoi64(sMsgText.substr(0,nFind).c_str());
//        const mycp::bigint nFileSize = cgc_atoi64(sMsgText.substr(nFind+1).c_str());
        break;
    }
    case MRT_FILE: {
        /// 文件
        return QFileInfo::exists(QString::fromStdString(sMsgText.string()));
    }
    default:
        break;
    }
    return false;
}

void EbClientApp::forwardMessageToGroupId(qint64 groupId, qint64 msgId)
{
    m_forwardMsgId = msgId;
    m_forwardTime = time(0);
    m_forwardUserId = 0;
    m_forwardAccount.clear();
    m_forwardGroupId = groupId;
    emit requestUrlCallGroup(QString::number(groupId));
}

void EbClientApp::forwardMessageToUserId(qint64 userId, qint64 msgId)
{
    m_forwardMsgId = msgId;
    m_forwardTime = time(0);
    m_forwardUserId = userId;
    m_forwardAccount.clear();
    m_forwardGroupId = 0;
    emit requestUrlCallUserId(QString::number(userId));
    //    return m_ebum.EB_CallUserId(userId);
}

void EbClientApp::forwardMessageToAccount(const QString &account, qint64 msgId)
{
    m_forwardMsgId = msgId;
    m_forwardTime = time(0);
    m_forwardUserId = 0;
    m_forwardAccount = account;
    m_forwardGroupId = 0;
    emit requestUrlCallUserAccount(account);
}

int EbClientApp::forwardMessage(qint64 callId, qint64 msgId)
{
    if (msgId==0 || m_sqliteUser.get()==0) {
        return -1;
    }
    char sSql[256];
    sprintf(sSql, "select msg_type,msg_name,msg_text FROM msg_record_t " \
            "WHERE msg_id=%lld AND (read_flag&%d)=0 ORDER BY msg_time,display_index",
            msgId,
            (int)EBC_READ_FLAG_WITHDRAW);
    EB_ChatRoomRichMsg richMsg;
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        const int nMsgType = pRecord->getVector()[0]->getIntValue();
        const tstring sMsgName(pRecord->getVector()[1]->getStrValue() );
        const tstring sMsgText(pRecord->getVector()[2]->getStrValue() );
        pRecord = m_sqliteUser->next(nCookie);
        switch (nMsgType) {
        case MRT_UNKNOWN: {
            m_sqliteUser->reset(nCookie);
            return -1;
        }
        case MRT_TEXT: {
            /// 先临时保存，后面显示；处理分段保存长文本；
            richMsg.AddText(sMsgText.c_str());
            break;
        }
        case MRT_JPG: {
            /// 图片/表情
            eb::bigint resourceId = 0;
            tstring resourceDesc;
            std::vector<tstring> pList;
            const tstring &sResourceInfo = sMsgText;
            if (ParseString(sResourceInfo.c_str(),";",pList)>=3) {
                resourceId = cgc_atoi64(pList[0].c_str());
                resourceDesc = pList.size()>3?pList[3]:"";
            }
            if (resourceId>0) {
                tstring resourceCmInfo;
                if (this->m_ebum.EB_GetResourceCmInfo(resourceId, resourceCmInfo)) {
                    richMsg.AddResource(resourceCmInfo.c_str());
                }
            }
            else {
                /// filepath=sMsgName
                const QString jpgFilePath = QString::fromStdString(sMsgName.string());
                QFile fileJpg(jpgFilePath);
                if (fileJpg.open(QFile::ReadOnly)) {
                    QByteArray data = fileJpg.readAll();
                    fileJpg.close();
                    const int dataSize = data.size();
                    if (dataSize>0) {
                        char * imageData = new char[dataSize];
                        memcpy(imageData, data.constData(), dataSize);
                        EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)richMsg.AddObject(imageData, (unsigned long)dataSize);
                        /// 用于本端显示
                        pMsgItem->SetText(jpgFilePath.toStdString().c_str());
                    }
                }
            }
            break;
        }
        case MRT_MAP_POS:
            /// 地图位置
            break;
        case MRT_CARD_INFO: {
            /// 名片信息
            m_sqliteUser->reset(nCookie);
            int nCardType = 0;
            tstring sCardData;
            m_ebum.EB_ParseCardInfo(sMsgText.c_str(),nCardType,sCardData);
            EB_ECardInfo pUserECard;
            if (nCardType==1 && m_ebum.EB_GetUserECardByFromInfo(sCardData, &pUserECard)) {
                /// ** 用户名片（个人名片）
                return this->sendUserIdEChard(callId, pUserECard.m_nMemberUserId);
            }
            return -1;
        }
        case MRT_USER_DATA:
            /// 用户自定义数据
            break;
        case MRT_WAV: {
            /// 语音消息
            /// filepath=sMsgName
            m_sqliteUser->reset(nCookie);

            richMsg.SetSubType(EB_RICH_SUB_TYPE_AUDIO);
            const QString voiceFilePath = QString::fromStdString(sMsgName.string());
            QFile fileJpg(voiceFilePath);
            if (fileJpg.open(QFile::ReadOnly)) {
                QByteArray data = fileJpg.readAll();
                fileJpg.close();
                const int dataSize = data.size();
                if (dataSize>0) {
                    char * imageData = new char[dataSize];
                    memcpy(imageData, data.constData(), dataSize);
                    EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)richMsg.AddObject(imageData, (unsigned long)dataSize);
                    /// 用于本端显示
                    pMsgItem->SetText(voiceFilePath.toStdString().c_str());
                    return m_ebum.EB_SendRich(callId, &richMsg);
                }
            }
            else {
                return -1;
            }
        }
        case MRT_RESOURCE: {
            /// 资源文件
            const std::string::size_type nFind = sMsgText.find(",");
            if (nFind == std::string::npos) {
                break;
            }
//            const QString sFileName = sMsgName.c_str();
//            const mycp::bigint nResourceId = cgc_atoi64(sMsgText.substr(0,nFind).c_str());
//            const mycp::bigint nFileSize = cgc_atoi64(sMsgText.substr(nFind+1).c_str());

            break;
        }
        case MRT_FILE: {
            /// 文件
            m_sqliteUser->reset(nCookie);
            const QString filePath(QString::fromStdString(sMsgText.string()));
            if (QFileInfo::exists(filePath)) {
                return this->sendFile(callId, filePath);
            }
            return -1;
        }
        default:
            break;
        }
    }
    m_sqliteUser->reset(nCookie);
    return m_ebum.EB_SendRich(callId, &richMsg);
}

bool EbClientApp::isCanCopyMessage(qint64 msgId) const
{
    if (msgId==0 || m_sqliteUser.get()==0) {
        return false;
    }
    char sSql[256];
    sprintf(sSql, "select from_uid,read_flag FROM msg_record_t " \
            "WHERE msg_id=%lld AND msg_type=%d LIMIT 1",
            msgId,
            (int)MRT_TEXT);
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    if (pRecord.get()==0) {
        return false;
    }
    const eb::bigint m_msgFromUserId = pRecord->getVector()[0]->getBigIntValue();
    const int m_msgReadFlag = pRecord->getVector()[1]->getIntValue();
    m_sqliteUser->reset(nCookie);
    if ((m_msgReadFlag&EBC_READ_FLAG_WITHDRAW)==0) {
        return true;
    }
    else {
        return false;
    }
}

bool EbClientApp::copyMessage(qint64 msgId)
{
    if (msgId==0 || m_sqliteUser.get()==0) {
        return false;
    }
    QString sRichText;
    char sSql[256];
    sprintf(sSql, "select msg_text FROM msg_record_t " \
            "WHERE msg_id=%lld AND msg_type=%d AND (read_flag&%d)=0 ORDER BY msg_time,display_index",
            msgId,
            (int)MRT_TEXT,
            (int)EBC_READ_FLAG_WITHDRAW);
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
    while (pRecord.get()!=0) {
        const tstring sMsgText(pRecord->getVector()[0]->getStrValue() );
        sRichText += QString::fromStdString(sMsgText.string());
        pRecord = m_sqliteUser->next(nCookie);
    }
    m_sqliteUser->reset(nCookie);
    if (!sRichText.isEmpty()) {
        QClipboard *board = QGuiApplication::clipboard();
        board->setText( sRichText );
    }
    return true;
}

bool EbClientApp::isCanCollectMessage(qint64 msgId) const
{
    if (msgId==0 || m_sqliteUser.get()==0) {
        return false;
    }
    char sSql[256];
    sprintf( sSql, "select from_uid,read_flag,msg_type FROM msg_record_t WHERE msg_id=%lld LIMIT 1", msgId);
    int nCookie = 0;
    m_sqliteUser->select(sSql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->last(nCookie);
    if (pRecord.get()==0) {
        return false;
    }
    const eb::bigint m_msgFromUserId = pRecord->getVector()[0]->getBigIntValue();
    const int m_msgReadFlag = pRecord->getVector()[1]->getIntValue();
    const int m_msgType = pRecord->getVector()[2]->getIntValue();
    m_sqliteUser->reset(nCookie);
    if (isCanCollectRecordType((EB_MSG_RECORD_TYPE)m_msgType) &&
            (m_msgReadFlag&EBC_READ_FLAG_WITHDRAW)==0) {
        return true;
    }
    else {
        return false;
    }
}

int EbClientApp::collectMessage(qint64 callId, qint64 msgId)
{
    return m_ebum.EB_RequestCollectMsg(callId, msgId, false);
}

void EbClientApp::ackMessage(qint64 msgId, int akcType)
{
    m_ebum.EB_AckMsg(msgId, akcType);
}

QString EbClientApp::conversationsUrl(qint64 groupId, qint64 fromUserId)
{
    const tstring funcUrl = m_ebum.EB_GetConversationsUrl(fromUserId, groupId);
    return QString::fromStdString(funcUrl.string());
}

void EbClientApp::deleteChat(qint64 callId, qint64 groupId, qint64 fromUserId)
{
    char sql[256];
    sprintf( sql, "DELETE FROM call_record_t WHERE call_id=%lld", callId);
    m_sqliteUser->execute(sql);
    if (groupId>0) {
        this->deleteDbRecord(groupId, false);
    }
    else if (fromUserId>0){
        this->deleteDbRecord(fromUserId, true);
    }
}

void EbClientApp::search(const QString &searchKey, int searchMode, int parameter)
{
    /// searchMode=0, 普通搜索（搜索所有）
    /// searchMode=1, 邀请好友搜索
    /// searchMode=2, 发送名片
    /// searchMode=3, 转发消息
    m_searchMode = searchMode;
    std::string sSearchKey = searchKey.toStdString();
    m_ebum.EB_SearchUserInfo(this, sSearchKey.c_str(), (unsigned long)parameter);

    if (searchMode==0 || searchMode==3) {
        m_ebum.EB_SearchGroupInfo(this, sSearchKey.c_str(), (unsigned long)parameter);
    }

    if (searchMode==0) {
        std::transform(sSearchKey.begin(), sSearchKey.end(), sSearchKey.begin(), tolower);
        std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;	/// for menu
        m_ebum.EB_GetSubscribeFuncList((EB_FUNC_LOCATION)0xffffff,m_pSubscribeFuncList);
        int count = 0;
        for (size_t i=0; i<m_pSubscribeFuncList.size(); i++) {
            const EB_SubscribeFuncInfo &funcInfo = m_pSubscribeFuncList[i];
            tstring sFunctionName(funcInfo.m_sFunctionName);
            tstring sDescription(funcInfo.m_sDescription);
            std::transform(sFunctionName.begin(), sFunctionName.end(), sFunctionName.begin(), tolower);
            std::transform(sDescription.begin(), sDescription.end(), sDescription.begin(), tolower);

            if (sFunctionName.find(sSearchKey)==std::string::npos &&
                    sDescription.find(sSearchKey)==std::string::npos) {
                continue;
            }
            emit searchResultFuncInfo(new EbQuickSubscribeFuncInfo(&funcInfo), parameter);
            if ((++count)>=5) {
                break;
            }
        }
    }
}

void EbClientApp::onGroupInfo(const EB_GroupInfo *pGroupInfo, const EB_EnterpriseInfo *pEnterpriseInfo, unsigned long dwParam)
{
    if (m_searchMode==3 && pGroupInfo->m_nMyEmpId==0) {
        /// 转发消息，非本部门信息返回不显示
        return;
    }
    EbQuickGroupInfo *quickGroupInfo = new EbQuickGroupInfo(pGroupInfo);
    const QString fullGroupName = this->groupFullName(quickGroupInfo->groupId(),
                                                      quickGroupInfo->groupName(),
                                                      quickGroupInfo->m_nEmpCount);
    quickGroupInfo->setGroupFullName(fullGroupName);
    emit searchResultGroupInfo(quickGroupInfo, (int)dwParam);
}

void EbClientApp::onMemberInfo(const EB_GroupInfo *pGroupInfo,
                               const EB_MemberInfo *pMemberInfo,
                               unsigned long dwParam)
{
    EbQuickGroupInfo *quickGroupInfo = new EbQuickGroupInfo(pGroupInfo);
    EbQuickMemberInfo *quickMemberInfo = new EbQuickMemberInfo(pMemberInfo);
    emit searchResultMemberInfo(quickGroupInfo, quickMemberInfo, (int)dwParam);
}

void EbClientApp::onContactInfo(const EB_ContactInfo *pContactInfo,
                                unsigned long dwParam)
{
    if (this->m_searchMode==3 && pContactInfo->m_nContactUserId==0) {
        /// 转发消息， uid为0不处理
        return;
    }
    EbQuickContactInfo *quickContactInfo = new EbQuickContactInfo(pContactInfo);
    emit searchResultContactInfo(quickContactInfo, (int)dwParam);
}

void EbClientApp::checkVersion()
{
    const std::string clientVersion = "2.0.0.509";
//    const std::string clientVersion = this->m_setting.GetVersion();
    this->m_ebum.EB_CheckVersion(clientVersion.c_str());
}

void EbClientApp::sendNotify(int messageId, const QString &title, const QString &text)
{
#ifdef Q_OS_ANDROID
//数据类型签名对照表:
//jobject //Ljava/lang/Object;
//jclass //Ljava/lang/Class;
//jstring //Ljava/lang/String;
//jthrowable //Ljava/lang/Throwable;
//jobjectArray //[Ljava/lang/Object;
//jarray //[<type>
//jbooleanArray //[Z
//jbyteArray //[B
//jcharArray //[C
//jshortArray //[S
//jintArray //[I
//jlongArray //[J
//jfloatArray //[F
//jdoubleArray //[D
//Primitive Types
//jboolean //Z
//jbyte //B
//jchar //C
//jshort //S
//jint//I
//jlong//J
//jfloat//F
//jdouble//D
//void //V
    if (!this->newMessageNotify()) {
        return;
    }
    const bool detailMessage = this->detailMessageNotify();
    const bool enableSound = this->voiceNotify();
    const bool enableVibrate = this->vibrateNotify();
    QAndroidJniObject notificationTitle = detailMessage?
                QAndroidJniObject::fromString(title):
                QAndroidJniObject::fromString(this->productName());
    QAndroidJniObject notificationText = detailMessage?
                QAndroidJniObject::fromString(text):
                QAndroidJniObject::fromString(m_locales->getLocalText("chat-msg-text.new-message", "New Message"));
    QAndroidJniObject::callStaticMethod<void>(
                "org/qtproject/entboost/NotificationClient",
                "notify",
                "(ILjava/lang/String;Ljava/lang/String;ZZ)V",
                messageId,
                notificationText.object<jstring>(),
                notificationTitle.object<jstring>(),
                enableSound,
                enableVibrate);
#endif  /// Q_OS_ANDROID
}

void EbClientApp::getManufacturerName()
{
#ifdef Q_OS_ANDROID
    /// 2.0 新版本 org.qtproject.entboost
    /// 小米推送平台申请的APPID
    const QString XIAOMI_PushAppId = "2882303761517619842"; ///注意：填入你自己应用所申请的APPID
    /// 小米推送平台申请的APPKey
    const QString XIAOMI_PushAppKey = "5201761947842"; ///注意：填入你自己应用所申请的APPKey
//    QString XIAOMI_PushAppId = "2882303761517548373"; ///注意：填入你自己应用所申请的APPID
//    /// 小米推送平台申请的APPKey
//    QString XIAOMI_PushAppKey = "5581754857373"; ///注意：填入你自己应用所申请的APPKey

    QAndroidJniObject context = QtAndroid::androidContext();
    QAndroidJniObject pushAppId = QAndroidJniObject::fromString(XIAOMI_PushAppId);
    QAndroidJniObject pushAppKey = QAndroidJniObject::fromString(XIAOMI_PushAppKey);

//    QAndroidJniObject entboostPushClientJavaObj("com/entboost/im/push/EntboostPushClient");
//    int ret = entboostPushClientJavaObj.callMethod<jint>(
//                "registerPush2",
//                "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)I",
//                context.object<jobject>(),
//                pushAppId.object<jstring>(),
//                pushAppKey.object<jstring>()
//                );
    /// ok
    int ret = QAndroidJniObject::callStaticMethod<jint>(
                "com/entboost/im/push/EntboostPushClient",
                "registerPush",
                "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)I",
                context.object<jobject>(),
                pushAppId.object<jstring>(),
                pushAppKey.object<jstring>()
                );
    if (ret==12) {
        m_timerIdGetPushToken = this->startTimer(3000);
    }
    return;

//    QAndroidJniObject testName = QAndroidJniObject::callStaticObjectMethod
//            ("com/entboost/utils/PhoneInfo" // class name
//             , "getContentTest" // method name
//             , "(Ljava/lang/Content;)Ljava/lang/String;");
//    /// manufacturer	"Xiaomi"	QString
//    /// HUAWEI
//    const QString test = name.toString();
//    QAndroidJniObject name = QAndroidJniObject::callStaticObjectMethod
//            ("com/entboost/utils/PhoneInfo" // class name
//             , "getManufacturerName" // method name
//             , "()Ljava/lang/String;");
    /// manufacturer	"Xiaomi"	QString
    /// HUAWEI
//    const QString manufacturer = name.toString();


    int i=0;
#endif
}

void EbClientApp::getPushToken()
{
#ifdef Q_OS_ANDROID
    /// 	pushToken	"ret: appid does not match packagename!"	QString
    /// 	pushToken	"ret: Invalid package name:org.qtproject.entboost"	QString
    /// 	pushToken	"8kCHnqXdPnQndbwNvTD0eYMDS2J/eVBz4fTB4IRUylE="	QString
    /// 	m_pushToken	"B6+LJwn+4kjGWvzIC1Tm/hjMu4Ax1Saz1RN/vkF99Kc="	QString
    if (m_pushToken.isEmpty()) {
        QAndroidJniObject name = QAndroidJniObject::callStaticObjectMethod(
                    "com/entboost/im/push/EntboostPushClient",
                    "getPushToken",
                    "()Ljava/lang/String;");
        m_pushToken = name.toString();
    }
    if (!m_pushToken.isEmpty()) {
        if (m_timerIdGetPushToken>0) {
            this->killTimer(m_timerIdGetPushToken);
            m_timerIdGetPushToken = 0;
        }
        /// ** 这里上传 token 到 IM 服务器
    }
#endif
}

void EbClientApp::onForwardMessage(void)
{
    if (m_forwardCallId==0 || !m_pCallList.exist(m_forwardCallId)) {
        return;
    }
    if (m_forwardMsgId>0) {
        this->forwardMessage(m_forwardCallId, m_forwardMsgId);
        m_forwardMsgId = 0;
    }
    m_forwardCallId = 0;
}

void EbClientApp::timerEvent(QTimerEvent *e)
{
    if (m_timerIdGetPushToken!=0 && m_timerIdGetPushToken==e->timerId()) {
        this->getPushToken();
    }
    else if (m_timerIdCheckState!=0 && m_timerIdCheckState==e->timerId()) {
        /// 定期检查会话状态，清空 超时无用 callinfo
        const time_t tNow = time(0);
        AUTO_RLOCK(m_pCallList);
        CLockMap<eb::bigint, EbcCallInfo::pointer>::iterator iter = m_pCallList.begin();
        for (; iter!=m_pCallList.end(); iter++) {
            const EbcCallInfo::pointer &callInfo = iter->second;
            if ((tNow-callInfo->m_tLastTime)<=5*60) continue;
            if (!isCurrentCallInfo(callInfo)) {
                /// callId 必须放在前面
                const eb::bigint callId = callInfo->callId();
                m_pCallList.erase(iter);
                m_ebum.EB_CallExit(callId);
                break;
            }
        }
    }

    EbClientAppBase::timerEvent(e);
}

//QQmlListProperty<EbQuickMemberInfo> EbClientApp::groupMemberInfoList(qint64 groupId)
//{
////    QQmlListProperty<EbQuickMemberInfo> result;
//    QList<EbQuickMemberInfo*> list;
//    std::vector<EB_MemberInfo> memberInfoList;
//    if (m_ebum.EB_GetGroupMemberInfoList(groupId, memberInfoList)) {

//        for (size_t i=0; i<memberInfoList.size(); i++) {
//            const EB_MemberInfo &memberInfo = memberInfoList[i];
//            list.append(&EbQuickMemberInfo(&memberInfo));
////            QQmlListProperty<EbQuickMemberInfo>::qlist_append(&result, new EbQuickMemberInfo(&memberInfo));
////            QQmlListProperty<EbQuickMemberInfo>::qlist_append(&result, new EbQuickMemberInfo(&memberInfo));
//        }
//    }
//    QObject * obj = this;
//    return QQmlListProperty<EbQuickMemberInfo>(obj, list);
//}

//QString EbClientApp::groupImageFile(const EbQuickGroupInfo *groupInfo)
//{

//}

void EbClientApp::customEvent(QEvent *e)
{
    const QEvent::Type eventType = e->type();
    bool result = true;
    const bool m_exited = false;
    if (m_exited) {
        result = false;
    }
    else {

        switch ((int)eventType) {
        case EB_WM_APPID_SUCCESS:
            onAppIdSuccess(e);
            break;
        case EB_WM_APPID_ERROR:
            onAppIdError(e);
            break;

        case EB_WM_BROADCAST_MSG:
            onBroadcastMsg(e);
            break;
        case EB_WM_AREA_INFO:
            onAreaInfo(e);
            break;
        case EB_WM_USER_STATE_CHANGE:
            onUserStateChange(e);
            break;
        case EB_WM_USER_HEAD_CHANGE:
            onMemberHeadChange(e);
            break;
        case EB_WM_CONTACT_HEAD_CHANGE:
            onContactHeadChange(e);
            break;
            /// 视频聊天
        case EB_WM_V_REQUEST_RESPONSE:
            onVRequestResponse(e);
            break;
        case EB_WM_V_ACK_RESPONSE:
            onVAckResponse(e);
            break;
        case EB_WM_VIDEO_REQUEST:
            onVideoRequest(e);
            break;
        case EB_WM_VIDEO_ACCEPT:
            onVideoAccept(e);
            break;
        case EB_WM_VIDEO_REJECT:
            onVideoReject(e);
            break;
        case EB_WM_VIDEO_TIMEOUT:
            onVideoTimeout(e);
            break;
        case EB_WM_VIDEO_CLOSE:
            onVideoClose(e);
            break;
            /// 聊天消息
        case CR_WM_MSG_RECEIPT:
            onMsgReceipt(e);
            break;
        case CR_WM_SEND_RICH:
            onSendRich(e);
            break;
        case CR_WM_RECEIVE_RICH:
            result = onReceiveRich(e);
            break;
        case CR_WM_SENDING_FILE:
            onSendingFile(e);
            break;
        case CR_WM_SENT_FILE:
            onSentFile(e);
            break;
        case CR_WM_CANCEL_FILE:
            onCancelFile(e);
            break;
        case CR_WM_RECEIVING_FILE:
            result = onReceivingFile(e);
            break;
        case CR_WM_RECEIVED_FILE:
            onReceivedFile(e);
            break;
        case CR_WM_FILE_PERCENT:
            onFilePercent(e);
            break;
        case CR_WM_SAVE2CLOUD_DRIVE:
            onSave2Cloud(e);
            break;
            /// 聊天会话
        case EB_WM_CALL_CONNECTED:
            onCallConnected(e);
            break;
        case EB_WM_CALL_ERROR:
            onCallError(e);
            break;
        case EB_WM_CALL_HANGUP:
            onCallHangup(e);
            break;
        case EB_WM_CALL_ALERTING:
            onCallAlerting(e);
            break;
        case EB_WM_CALL_INCOMING:
            onCallIncoming(e);
            break;
            ///  联系人
        case EB_WM_UG_INFO:
            onUGInfo(e);
            break;
        case EB_WM_UG_DELETE:
            onUGDelete(e);
            break;
        case EB_WM_CONTACT_DELETE:
            onContactDelete(e);
            break;
        case EB_WM_CONTACT_INFO:
            onContactInfo(e);
            break;
        case EB_WM_CONTACT_STATE_CHANGE:
            onContactStateChanged(e);
            break;
        case EB_WM_ACCEPT_ADDCONTACT:
            onAcceptAddContact(e);
            break;
        case EB_WM_REJECT_ADDCONTACT:
            onRejectAddContact(e);
            break;
        case EB_WM_REQUEST_ADDCONTACT:
            onRequestAddContact(e);
            break;
            //// 组织结构
        case EB_WM_EDITINFO_RESPONSE:
            onEditInfoResponse(e);
            break;
        case EB_WM_MEMBER_EDIT_RESPONSE:
            onMemberEditResponse(e);
            break;
        case EB_WM_MEMBER_DELETE:
            onMemberDelete(e);
            break;
        case EB_WM_MEMBER_INFO:
            onMemberInfo(e);
            break;
        case EB_WM_REJECT_ADD2GROUP:
            onRejectAdd2Group(e);
            break;
        case EB_WM_INVITE_ADD2GROUP:
            onInviteAdd2Group(e);
            break;
        case EB_WM_REQUEST_ADD2GROUP:
            onRequestAdd2Group(e);
            break;
        case EB_WM_EXIT_GROUP:
            onExitGroup(e);
            break;
        case EB_WM_REMOVE_GROUP:
            onRemoveGroup(e);
            break;
        case EB_WM_GROUP_EDIT_RESPONSE:
            onGroupEditResponse(e);
            break;
        case EB_WM_GROUP_DELETE:
            onGroupDelete(e);
            break;
        case EB_WM_GROUP_INFO:
            onGroupInfo(e);
            break;
        case EB_WM_ENTERPRISE_INFO:
            onEnterpriseInfo(e);
            break;
            /// 注册
        case EB_WM_REGISTER_RESPONSE:
            onRegisterResponse(e);
            break;
            //// 登录
        case EB_WM_LOGON_SUCCESS:
            onLogonSuccess(e);
            break;
//        case EB_WM_LOGON_TIMEOUT:
//            onLogonTimeout(e);
//            break;
        case EB_WM_LOGON_ERROR:
            onLogonError(e);
            break;
        case EB_WM_ONLINE_ANOTHER:
            onOnlineAnother(e);
            break;
        default:
            break;
        }
    }
    /// 返回结果
    if (eventType>=CR_WM_ENTER_ROOM && eventType<=CR_WM_EVENT_RESULT) {
        CCrInfo *aCrInfo = (CCrInfo*)e;
        QObject *receiver = aCrInfo->receiver();
        if (receiver!=0) {
            CCrInfo *event = new CCrInfo();
            event->SetQEventType((QEvent::Type)CR_WM_EVENT_RESULT);
            event->setReceiveResult(aCrInfo->receiveKey(),result?0:-1);
            QCoreApplication::postEvent(receiver,event);
        }
    }
    else if (eventType>=EB_WM_APPID_SUCCESS && eventType<=EB_WM_EVENT_RESULT) {
        EB_Event *aEvent = (EB_Event*)e;
        QObject *receiver = aEvent->receiver();
        if (receiver!=0) {
            EB_Event *event = new EB_Event((QEvent::Type)EB_WM_EVENT_RESULT);
            event->setReceiveResult(aEvent->receiveKey(),result?0:-1);
            QCoreApplication::postEvent(receiver,event);
        }
    }
}

void EbClientApp::onAppIdSuccess(QEvent *e)
{
    EbClientAppBase::onAppIdSuccess(e);
    emit appidSuccess();

//    ui->pushButtonLogon->setEnabled(true);
//    ui->pushButtonVisitorLogon->setVisible(theApp->isOpenVisitor()?true:false);
//    ui->pushButtonRegister->setVisible(theApp->openRegister()==0?false:true);
//    if (!theApp->isOpenVisitor() && theApp->openRegister()) {
//        /// 优化 “我的注册” 按钮，自动显示在最左边
//        ui->pushButtonRegister->move(ui->pushButtonVisitorLogon->geometry().topLeft());
//    }
//    else if (theApp->isOpenVisitor() && theApp->openRegister()) {
//        /// 优化 “我的注册” 按钮，自动显示在第二个
//        const int x = ui->pushButtonVisitorLogon->geometry().left()+const_button_interval;
//        const int y = ui->pushButtonVisitorLogon->geometry().y();
//        ui->pushButtonRegister->move(x, y);
//    }
//    ui->pushButtonForgetPwd->setVisible(theApp->forgetPwdUrl().isEmpty()?false:true);
//    theApp->m_ebum.EB_SetMsgReceiver(this);


}
void EbClientApp::onAppIdError(QEvent *e)
{
    EbClientAppBase::onAppIdError(e);
    const EB_Event * pEvent = (EB_Event*)e;
    m_errorCode = (EB_STATE_CODE)pEvent->GetEventParameter();
    switch (m_errorCode) {
    case EB_STATE_NOT_SUPPORT_VERSION_ERROR:
        /// 系统不支持当前版本，请更新客户端后重试！
        m_errorString = m_locales->getLocalText("on-appid-response.not-support-version.text", "EB_STATE_NOT_SUPPORT_VERSION_ERROR");
        break;
    case EB_STATE_TIMEOUT_ERROR: {
        m_errorString = m_locales->getLocalText("on-appid-response.timeout-error.text", "Timeout error");
        m_errorString.replace( "[STATE_CODE]", QString::number( (int)m_errorCode ) );
        break;
    }
//    case EB_STATE_APPID_KEY_ERROR:
//        break;
    default:
        /// 系统APPID验证失败，请退出程序后重新进入！<br>错误代码:[STATE_CODE]
        m_errorString = m_locales->getLocalText("on-appid-response.other-error.text", "APPID Error");
        m_errorString.replace( "[STATE_CODE]", QString::number( (int)m_errorCode ) );
        break;
    }   // switch
    emit appidError(m_errorCode, m_errorString);
}

void EbClientApp::onRegisterResponse(QEvent *e)
{
    const EB_Event * pEvent = (EB_Event*)e;
    m_errorCode = (EB_STATE_CODE)pEvent->GetEventParameter();
    QString sMessageText;
    switch(m_errorCode) {
    case EB_STATE_NOT_AUTH_ERROR: {
        const std::string sAccount = m_registerAccount.toStdString();
        if (isFullNumber(sAccount.c_str(), sAccount.size()))
            /// 不支持全数字帐号注册：<br>请更换帐号后重试！
            sMessageText = m_locales->getLocalText( "on-register-response.number-not-auth.text","EB_STATE_NOT_AUTH_ERROR" );
        else
            sMessageText = m_locales->getLocalText( "on-register-response.not-auth-error.text","EB_STATE_NOT_AUTH_ERROR" );
        break;
    }
    case EB_STATE_CONTENT_BIG_LENGTH:
        /// 字段内容超过最大长度：<br>请重新输入！
        sMessageText = m_locales->getLocalText( "on-register-response.content-big-length.text","EB_STATE_CONTENT_BIG_LENGTH" );
        break;
    case EB_STATE_OK: {
        m_registerUserId = pEvent->GetEventBigParameter();
        if (isSendRegMail()) {
            /// 注册成功：<br>请及时查阅邮件，完成帐号验证！
            sMessageText = m_locales->getLocalText( "on-register-response.ok-send-mail.text","EB_STATE_OK" );
        }
        else {
            sMessageText = m_locales->getLocalText( "on-register-response.ok-login.text","EB_STATE_OK" );
        }
        sMessageText.replace("[ACCOUNT]", m_registerAccount);
        sMessageText.replace("[USERID]", QString::number(m_registerUserId));
        break;
    }
    case EB_STATE_ACCOUNT_ALREADY_EXIST:
        // 帐号已经存在：<br>请更换帐号后重试！
        sMessageText = m_locales->getLocalText( "on-register-response.account-already-exist.text","EB_STATE_ACCOUNT_ALREADY_EXIST" );
        break;
    case EB_STATE_ENTERPRISE_ALREADY_EXIST:
        // 公司名称已经被注册：<br>请确认后重试！
        sMessageText = m_locales->getLocalText( "on-register-response.enterprise-already-exist.text","EB_STATE_ENTERPRISE_ALREADY_EXIST" );
        break;
    case EB_STATE_APPID_KEY_ERROR:
        // APPID KEY错误：<br>请重试或联系公司客服！
        sMessageText = m_locales->getLocalText( "on-register-response.appid-key-error.text","EB_STATE_APPID_KEY_ERROR" );
        break;
    case EB_STATE_DISABLE_REGISTER_USER:
        // 禁止用户注册功能：<br>请联系公司客服！
        sMessageText = m_locales->getLocalText( "on-register-response.disable-register-account.text","EB_STATE_DISABLE_REGISTER_USER" );
        break;
    case EB_STATE_DISABLE_REGISTER_ENT:
        sMessageText = m_locales->getLocalText( "on-register-response.disable-register-ent.text","EB_STATE_DISABLE_REGISTER_ENT" );
        break;
    case EB_STATE_ERROR:
        // 注册失败：<br>请重试！
        sMessageText = m_locales->getLocalText( "on-register-response.error.text","EB_STATE_ERROR" );
        break;
    default:
        // 注册失败，请重试或联系公司客服：<br>错误代码：[STATE_CODE]
        sMessageText = m_locales->getLocalText( "on-register-response.other-error.text","Other Error" );
        break;
    }
    sMessageText.replace( "[STATE_CODE]",QString::number((int)m_errorCode));
    m_errorString = sMessageText;
    if (m_errorCode==EB_STATE_OK) {
        emit registerSuccess(m_registerAccount, QString::number(m_registerUserId), m_errorString);
    }
    else {
        emit registerError(m_errorCode, m_errorString);
    }
}

const tstring & replace(tstring & strSource, const tstring & strFind, const tstring &strReplace)
{
    std::string::size_type pos=0;
    std::string::size_type findlen=strFind.size();
    std::string::size_type replacelen=strReplace.size();
    while ((pos=strSource.find(strFind, pos)) != std::string::npos)
    {
        strSource.replace(pos, findlen, strReplace);
        pos += replacelen;
    }
    return strSource;
}

void EbClientApp::onBroadcastMsg(QEvent *e)
{
    const EB_AccountInfo *pAccountInfo = (const EB_AccountInfo*)e;
    const eb::bigint nFromUserid = pAccountInfo->GetUserId();
    const tstring sFromAccount(pAccountInfo->GetAccount());
    const EB_APMsgInfo *pApMsgInfo = (const EB_APMsgInfo*)pAccountInfo->GetEventData();
    const eb::bigint nMsgId = pApMsgInfo->m_nMsgId;
    const EB_BROADCAST_SUB_TYPE nBCMsgSubType = (EB_BROADCAST_SUB_TYPE)pApMsgInfo->m_nMsgType;
    const tstring sMsgName(pApMsgInfo->m_sMsgName);
    const tstring sMsgContent(pApMsgInfo->m_sMsgContent);
//    char lpszBuffer[12];
//    sprintf(lpszBuffer, "subtype=%d", (int)nBCMsgSubType);
//    const tstring sMsgContent(lpszBuffer);

    const int notifyRet = checkMessageNotify();
    switch (nBCMsgSubType) {
    case EB_BROADCAST_SUB_TYPE_NEW_EMAIL: {
        /// 邮件消息通知；
        const tstring theEBSParseString0_from(" ");
        const tstring theEBSParseString0_to("&nbsp;");
        std::vector<tstring> pList;
        if (ParseString(sMsgContent.c_str(),theEBSParseString0_from.c_str(),pList)<10) {
            return;
        }
        //            const eb::bigint nUserId = eb_atoi64(pList[0].c_str());
        //            const eb::bigint nMailAddressId = eb_atoi64(pList[1].c_str());
        const eb::bigint nMailContentId = eb_atoi64(pList[2].c_str());
        //            const int nMailSize = atoi(pList[3].c_str());
        //            const int nAttachCount = atoi(pList[4].c_str());
        tstring sMailDate(pList[5]);
        replace(sMailDate,theEBSParseString0_to,theEBSParseString0_from);
        tstring sFromName(pList[6]);
        replace(sFromName,theEBSParseString0_to,theEBSParseString0_from);
        const tstring sFromMail(pList[7]);
        tstring sSubject(pList[8]);
        replace(sSubject,theEBSParseString0_to,theEBSParseString0_from);
        const tstring sParam(pList[9]);

        const eb::bigint nEmailSubId = eb_atoi64(sMsgName.c_str());
        /// 新邮件:%s<%s> %s:\n%s
        const QString newEmailTip = m_locales->getLocalText( "message-show.new-email-tip", "New Email:" );
        const QString msgTitle = QString("%1%2<%3> %4:")
                .arg(newEmailTip)
                .arg(sFromName.c_str())
                .arg(sFromMail.c_str())
                .arg(sMailDate.c_str());
        emit receivedSubMsg(QString::number(nMailContentId),
                            QString::number(nEmailSubId),
                            msgTitle,
                            sSubject.c_str(),
                            sParam.c_str());
        //            addSubUnreadMsg(nEmailSubId, true);
        if (notifyRet==1) {
            this->sendNotify(1, msgTitle, sSubject.c_str());
        }
        return;
    }
    case EB_BROADCAST_SUB_TYPE_UNREAD_EMAIL: {
        const int unreadEmailCount = atoi(sMsgContent.c_str());
        const eb::bigint emailSubId = eb_atoi64(sMsgName.c_str());
        /// 你一共有 %s 封未读邮件！\n点击查看！
        QString msgTip = m_locales->getLocalText( "message-show.unread-email-tip", "Unread Email" );
        msgTip.replace("[%1]", QString("<font color=\"red\">%1</font>").arg(unreadEmailCount) );
        msgTip.replace("\n", "<br>");
        emit receivedSubMsg("0", QString::number(emailSubId), msgTip, "", "");
//        setSubUnreadMsg( emailSubId, unreadEmailCount, true );
        return;
        }
    case EB_BROADCAST_SUB_TYPE_TW_MSG: {
        /// 应用消息通知；
        //const tstring sMsgTip(sMsgName);
        const tstring theEBSParseString0_from(" ");
        //const tstring theEBSParseString0_to("&nbsp;");
        std::vector<tstring> pList;
        if (ParseString(sMsgContent.c_str(),theEBSParseString0_from.c_str(),pList)<2) {
            m_ebum.EB_AckMsg(nMsgId,5);	/// *删除无用数据
            return;
        }
        const eb::bigint nId = eb_atoi64(pList[0].c_str());			/// id->planid,taskid,...
        const eb::bigint subId = eb_atoi64(pList[1].c_str());		/// subid
        const tstring sParam = pList.size()>=3?pList[2]:"";
        if (!sMsgName.empty()) {
            //CString sMsgTip;
            //sMsgTip.Format(_T("新消息:%s<%s> %s:\n%s"),sFromName.c_str(),sFromMail.c_str(),sMailDate.c_str(),sSubject.c_str());
            QString msgTitle(sMsgName.c_str());
            msgTitle.replace("\n", "<br>");
            emit receivedSubMsg(QString::number(nMsgId),
                                QString::number(subId),
                                msgTitle, "", sParam.c_str());
            if (notifyRet==1) {
                this->sendNotify(1, msgTitle, "");
            }
        }
        //            addSubUnreadMsg(subId,true);
        return;
    }
    case EB_BROADCAST_SUB_TYPE_SUBID_UNREAD_MSG: {
        std::vector<tstring> pList;
        if (ParseString(sMsgContent.c_str(),",",pList)<2)
        {
            m_ebum.EB_AckMsg(nMsgId,5);     /// *删除无用数据
            return;
        }
        const eb::bigint subId = eb_atoi64(pList[0].c_str());
        const size_t unreadMsg = atoi(pList[1].c_str());
        //            setSubUnreadMsg( subId,unreadMsg,true );
        return;
    }
    default:
        break;
    }

    /// emp_code=-1,from_type=msg-type
    const int nMsgType = EBC_MSG_TYPE_BC_MSG;
    char sql[1024];
    sprintf(sql, "INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type,dep_name) "
                 "VALUES(%lld,0,-1,%lld,'%s',%d,'%s')",
            pApMsgInfo->m_nMsgId,
            nFromUserid,
            sFromAccount.c_str(),
            nMsgType,
            sMsgName.c_str() );
    m_sqliteUser->execute(sql);

    EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
    sessionInfo->setCallId(pApMsgInfo->m_nMsgId);
    const tstring sCallTime = pApMsgInfo->m_sMsgTime;
    time_t nCallTime = time(0);
    ChangeTime(sCallTime.c_str(),nCallTime);
    sessionInfo->setTime(nCallTime);
    sessionInfo->setGroupName(sMsgName.c_str());
    sessionInfo->setMemberId(-1);
    sessionInfo->setFromUserId(nFromUserid);
    sessionInfo->setFromAccount(sFromAccount.c_str());
    sessionInfo->setFromType(nMsgType);
    sessionInfo->setRead(false);
    sendSessionInfo(sessionInfo);
    emit receivedBcMsg(QString::number(nMsgId),
                       "1002300103",
                       sMsgName.c_str(),
                       sMsgContent.c_str(),
                       "tab_type=bc_msg");
    if (notifyRet==1) {
        this->sendNotify(1, sMsgName.c_str(), sMsgContent.c_str());
    }

//    m_dialogBroadcaseMsg->setPopTipMessage(sMsgContent.c_str(), nMsgId, QVariant("tab_type=bc_msg"), QVariant(sMsgName.c_str()));
//    m_dialogBroadcaseMsg->show();
}

void EbClientApp::onAreaInfo(QEvent *e)
{
    const EB_AreaInfo *pAreaInfo = (const EB_AreaInfo*)e;
    const int nParameter = (int)pAreaInfo->GetEventParameter();
    emit loadAreaInfoResponse(new EbQuickAreaInfo(pAreaInfo), nParameter);
//    switch (nParameter) {
//    case 1:
//    case 2:
//    case 3:
//    case 4: {
//        if (m_pDlgMyCenter!=0) {
//            m_pDlgMyCenter->onAreaInfo(pAreaInfo,nParameter);
//        }
//        break;
//    }
//    default:
//        break;
//    }
}

void EbClientApp::onUserStateChange(QEvent *e)
{
    const EB_MemberInfo *memberInfo = (EB_MemberInfo*)e;
//    const bool bIsOwnerMember = pEvent->GetEventParameter()==1?true:false;

    emit memberLineStateChanged(new EbQuickMemberInfo(memberInfo));

//    /// 更新界面用户状况改变
//    if (m_widgetMyEnterprise!=0) {
//        m_widgetMyEnterprise->onMemberInfo(memberInfo,true);
//    }
//    if ( m_widgetMyGroup!=0 ) {
//        m_widgetMyGroup->onMemberInfo(memberInfo,true);
//    }
//    if ( m_widgetMySession!=0 ) {
//        m_widgetMySession->onMemberInfo(memberInfo,true);
//    }

//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//        /// **跑下面
//    }
//    else if (!theApp->isHideMainFrame()) {
////        CFrameWndInfoProxy::UserLineStateChange(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId, pMemberInfo->m_nLineState);
////        return 0;
//    }
//    if (m_pDlgFrameList!=0) {
//        m_pDlgFrameList->onMemberInfo(memberInfo, true);
////        m_pDlgFrameList->onUserLineStateChange(memberInfo->m_sGroupCode, memberInfo->m_nMemberUserId, memberInfo->m_nLineState);
    //    }
}

void EbClientApp::checkMyHeadImageChange(const EB_MemberInfo *memberInfo)
{
    if (memberInfo->m_nMemberUserId==logonUserId()) {
        if (!memberInfo->m_sHeadResourceFile.isEmpty() &&
                QFile::exists(memberInfo->m_sHeadResourceFile) &&
                (memberInfo->m_sHeadMd5!=m_myHeadImageMd5 || m_myHeadResourceId!=memberInfo->m_sHeadResourceId)) {
            m_myHeadResourceId = memberInfo->m_sHeadResourceId;
            m_myHeadImageMd5 = memberInfo->m_sHeadMd5;
            m_myHeadImageFile = theFileHead+memberInfo->m_sHeadResourceFile;
            emit myHeadImageChanged(m_myHeadImageFile);

            QString tempImageFile;
            for (int i=0; i<100; i++) {
                tempImageFile = QString("%1/default_head_image_temp_%2.jpg").arg(userImagePath()).arg(i);
                if (!QFile::exists(tempImageFile)) {
                    break;
                }
                QFile::remove(tempImageFile);
            }
        }
    }
}

void EbClientApp::onMemberHeadChange(QEvent *e)
{
    const EB_MemberInfo *memberInfo = (const EB_MemberInfo*)e;
    const bool bIsOwnerMember = (memberInfo->GetEventParameter()==1)?true:false;

    checkMyHeadImageChange(memberInfo);
//    // 更新聊天会话列表图标
//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//        if (m_pDlgFrameList!=0) {
//            m_pDlgFrameList->onMemberHeadChange(memberInfo);
//        }
//    }
//    else if (!theApp->isHideMainFrame()) {
////        CFrameWndInfoProxy::MemberHeadChange(pMemberInfo);
//    }
//    /// 更新界面用户状况改变
//    if (m_widgetMyEnterprise!=0) {
//        m_widgetMyEnterprise->onMemberInfo(memberInfo,false);
//    }
//    if (m_widgetMyGroup!=0) {
//        m_widgetMyGroup->onMemberInfo(memberInfo,false);
//    }
//    if (bIsOwnerMember) {
//        eb::bigint nDefaultMemberCode = 0;
//        if (theApp->m_ebum.EB_GetMyDefaultMemberCode(nDefaultMemberCode)
//                && nDefaultMemberCode==memberInfo->m_sMemberCode) {
//            /// 更新头像
//            const QString userHeadFile = memberInfo->m_sHeadResourceFile;
//            if ( !userHeadFile.isEmpty() && QFile::exists(userHeadFile)) {
//                m_labelUserImage->setPixmap( QPixmap(userHeadFile).scaled(m_rectHead.width(),m_rectHead.height(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
//            }
//        }
//    }
}

void EbClientApp::onContactHeadChange(QEvent *e)
{
//    const EB_ContactInfo *pContactInfo = (const EB_ContactInfo*)e;
//    if (m_widgetMyContact!=0) {
//        m_widgetMyContact->onContactHeadChange(pContactInfo);
//    }
//    if (pContactInfo->m_nContactUserId>0) {
//        const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//        if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//            if (m_pDlgFrameList!=0) {
//                m_pDlgFrameList->onContactHeadChange(pContactInfo);
//            }
//        }
//        else if (!theApp->isHideMainFrame()) {
////            CFrameWndInfoProxy::UserHeadChange(pContactInfo);
//        }
//    }
}

void EbClientApp::onVRequestResponse(QEvent *e)
{
//    const EB_VideoInfo *pVideoInfo = (const EB_VideoInfo*)e;
//    const EB_STATE_CODE nState = (EB_STATE_CODE)pVideoInfo->GetEventParameter();
//    if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState) {
//        return;
//    }
//    EbcCallInfo::pointer pEbCallInfo;
//    if (!theApp->m_pCallList.find(pVideoInfo->m_sCallId, pEbCallInfo)) {
//        return;
//    }
//    pEbCallInfo->m_tLastTime = time(0);
//    if (nState==EB_STATE_NOT_AUTH_ERROR) {
//        /// 禁止视频会议功能权限：\r\n请联系公司客服！
//        const QString text = m_locales->getLocalText("message-show.video-not-auth-error","Disable group video");
//        EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
//    }
//    else if (nState==EB_STATE_EXCESS_QUOTA_ERROR) {
//        /// 超过视频会议用户数量：\r\n请联系公司客服！
//        const QString text = m_locales->getLocalText("message-show.video-excess-quota-error","Video excess quota error");
//        EbMessageBox::doShow( NULL, "", QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
//    }
//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo);
//    chatBase->onVRequestResponse(pVideoInfo, nState);
}

void EbClientApp::onVAckResponse(QEvent *e)
{
//    const EB_VideoInfo *pVideoInfo = (const EB_VideoInfo*)e;
//    const EB_STATE_CODE nState = (EB_STATE_CODE)pVideoInfo->GetEventParameter();
//    if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState) {
//        return;
//    }
//    EbcCallInfo::pointer pEbCallInfo;
//    if (!theApp->m_pCallList.find(pVideoInfo->m_sCallId, pEbCallInfo)) {
//        return;
//    }

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo->callId());
//    if (chatBase.get()!=0) {
//        chatBase->onVAckResponse(pVideoInfo, nState);
//    }
}

void EbClientApp::onVideoRequest(QEvent *e)
{
//    const EB_VideoInfo *pVideoInfo = (const EB_VideoInfo*)e;
//    const EB_UserVideoInfo *pUserVideoInfo = (const EB_UserVideoInfo*)pVideoInfo->GetEventData();
//    EbcCallInfo::pointer pEbCallInfo;
//    if (!theApp->m_pCallList.find(pVideoInfo->m_sCallId, pEbCallInfo)) {
//        return;
//    }
//    pEbCallInfo->m_tLastTime = time(0);

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo);
//    chatBase->onVideoRequest(pVideoInfo, pUserVideoInfo);
}

void EbClientApp::onVideoAccept(QEvent *e)
{
//    const EB_VideoInfo *pVideoInfo = (const EB_VideoInfo*)e;
//    const EB_UserVideoInfo *pUserVideoInfo = (const EB_UserVideoInfo*)pVideoInfo->GetEventData();
//    EbcCallInfo::pointer pEbCallInfo;
//    if (!theApp->m_pCallList.find(pVideoInfo->m_sCallId, pEbCallInfo)) {
//        return;
//    }

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo->callId());
//    if (chatBase.get()!=0) {
//        chatBase->onVideoAccept(pVideoInfo, pUserVideoInfo);
//    }
}

void EbClientApp::onVideoReject(QEvent *e)
{
//    const EB_VideoInfo *pVideoInfo = (const EB_VideoInfo*)e;
//    const EB_UserVideoInfo *pUserVideoInfo = (const EB_UserVideoInfo*)pVideoInfo->GetEventData();
//    EbcCallInfo::pointer pEbCallInfo;
//    if (!theApp->m_pCallList.find(pVideoInfo->m_sCallId, pEbCallInfo)) {
//        return;
//    }

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo->callId());
//    if (chatBase.get()!=0) {
//        chatBase->onVideoCancel(pVideoInfo, pUserVideoInfo);
//    }
}

void EbClientApp::onVideoTimeout(QEvent *e)
{
//    const EB_VideoInfo *pVideoInfo = (const EB_VideoInfo*)e;
//    const EB_UserVideoInfo *pUserVideoInfo = (const EB_UserVideoInfo*)pVideoInfo->GetEventData();
//    EbcCallInfo::pointer pEbCallInfo;
//    if (!theApp->m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo)) {
//        return;
//    }

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo->callId());
//    if (chatBase.get()!=0) {
//        chatBase->onVideoTimeout(pVideoInfo, pUserVideoInfo);
//    }
}

void EbClientApp::onVideoClose(QEvent *e)
{
//    const EB_VideoInfo *pVideoInfo = (const EB_VideoInfo*)e;
//    const EB_UserVideoInfo *pUserVideoInfo = (const EB_UserVideoInfo*)pVideoInfo->GetEventData();
//    EbcCallInfo::pointer pEbCallInfo;
//    if (!theApp->m_pCallList.find(pVideoInfo->m_sCallId,pEbCallInfo)) {
//        return;
//    }

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo->callId());
//    if (chatBase.get()!=0) {
//        chatBase->onVideoEnd(pVideoInfo, pUserVideoInfo);
//    }
}

void EbClientApp::onMsgReceipt(QEvent *e)
{
    const CCrRichInfo *pCrMsgInfo = (const CCrRichInfo*)e;
    const eb::bigint sCallId = pCrMsgInfo->GetCallId();
    /// 0:成功 4:撤回消息，6:个人收藏，7:群收藏
    const int nAckType = (int)pCrMsgInfo->GetEventParameter();

    const eb::bigint nFromUserId = pCrMsgInfo->m_sSendFrom;
    const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
    if (pCrMsgInfo->GetStateCode()==EB_STATE_OK) {
        updateMsgReceiptData(nMsgId, nFromUserId, nAckType);
    }
    emit receivedMsgReceiptData(QString::number(sCallId),
                                QString::number(nMsgId),
                                QString::number(nFromUserId),
                                nAckType,
                                (int)pCrMsgInfo->GetStateCode());

//    //const eb::bigint nFromUserId = pCrMsgInfo->m_sSendFrom;
//    //const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();

//    //EbcCallInfo::pointer pEbCallInfo;
//    //if (!theApp.m_pCallList.find(sCallId,pEbCallInfo))
//    //{
//    //	return 1;
//    //}
//    EbDialogChatBase::pointer pDlgDialog = getDialogChatBase(sCallId);
//    if (pDlgDialog.get()==0) {
//        const eb::bigint nFromUserId = pCrMsgInfo->m_sSendFrom;
//        const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
//        if (pCrMsgInfo->GetStateCode()==EB_STATE_OK) {
//            theApp->updateMsgReceiptData(nMsgId, nFromUserId, nAckType);
//        }
//    }
//    else {
//        pDlgDialog->onMsgReceipt(pCrMsgInfo, nAckType);
    //    }
}

bool EbClientApp::isCurrentCallInfo(const EbcCallInfo::pointer &pEbCallInfo) const
{
    if ((pEbCallInfo->isGroupCall() && pEbCallInfo->groupId()==m_currentChatGroupId) ||
            (!pEbCallInfo->isGroupCall() && m_currentChatGroupId==0 &&
             (pEbCallInfo->fromUserId()==m_currentChatUserId ||
              pEbCallInfo->fromUserId()==this->logonUserId()))) {
        return true;
    }
    return false;
}

void EbClientApp::sendMessageTip(const EbcCallInfo::pointer &pEbCallInfo, const QString &messageTip)
{
    if (isCurrentCallInfo(pEbCallInfo)) {
        emit displayMessageTip(QString::number(pEbCallInfo->groupId()),
                               QString::number(pEbCallInfo->fromUserId()),
                               messageTip);
    }
//    if ((pEbCallInfo->isGroupCall() && pEbCallInfo->groupId()==m_currentChatGroupId) ||
//            (!pEbCallInfo->isGroupCall() && m_currentChatGroupId==0 &&
//             (pEbCallInfo->fromUserId()==m_currentChatUserId ||
//              pEbCallInfo->fromUserId()==this->logonUserId()))) {
//        emit displayMessageTip(pEbCallInfo->groupId(), pEbCallInfo->fromUserId(), messageTip);
//    }
}

void EbClientApp::sendRichInfo(EbQuickRichInfo *richInfo)
{
    if ((richInfo->isGroupCall() && richInfo->groupId()==m_currentChatGroupId) ||
            (!richInfo->isGroupCall() && m_currentChatGroupId==0 &&
             (richInfo->sendFromUserId()==m_currentChatUserId ||
              richInfo->sendFromUserId()==this->logonUserId()))) {
        const QDateTime pMsgDateTime = QDateTime::fromSecsSinceEpoch(richInfo->time());
        const QDate dateMsg = pMsgDateTime.date();
        const QDate dateLastMsg = m_tLastMsgDayTime.date();
        if ( /*this->document()->isEmpty() ||*/
             dateMsg.year() != dateLastMsg.year()  ||
             dateMsg.month() != dateLastMsg.month() ||
             dateMsg.day() != dateLastMsg.day() ) {
            m_tLastMsgDayTime = pMsgDateTime;
            int nDayOfWeek = dateMsg.dayOfWeek();
            if (nDayOfWeek>=7) {
                nDayOfWeek = 0; // 0=星期天
            }
            const QDateTime pNow = QDateTime::currentDateTime();
            const QDate nowDate = pNow.date();
            const EbDayOfWeekInfo::pointer &pDayOfWeekInfo = m_locales->getDayOfWeekInfo(nDayOfWeek);
            char sText[256];
            if (nowDate.year()==dateMsg.year()) {
                /// 今年
                sprintf(sText,"%02d-%02d %s",
                        dateMsg.month(),dateMsg.day(),
                        pDayOfWeekInfo->shortName().c_str());
            }
            else {
                sprintf(sText,"%04d-%02d-%02d %s",
                        dateMsg.year(),dateMsg.month(),dateMsg.day(),
                        pDayOfWeekInfo->shortName().c_str());
            }
            emit displayMessageTip(richInfo->groupIdString(),
                                   richInfo->sendFromUserIdString(),
                                   sText);
        }
    }

    emit receivedRichInfo(richInfo, "0");
}

typedef struct eb_waveformat_tag {
    unsigned short	wFormatTag;        /* format type */
    unsigned short	nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    unsigned long	nSamplesPerSec;    /* sample rate */
    unsigned long	nAvgBytesPerSec;   /* for buffer estimation */
    unsigned short	nBlockAlign;       /* block size of data */
} EB_WAVEFORMAT;
typedef struct eb_pcmwaveformat_tag {
    EB_WAVEFORMAT	wf;
    unsigned short	wBitsPerSample;
} EB_PCMWAVEFORMAT;

int GetWaveTimeLength(const char* lpszWavFilePath)
{
//#ifdef _QT_MAKE_
//    const QString fileTemp(lpszWavFilePath);
//    const QByteArray byteArray = fileTemp.toLocal8Bit();
//    FILE * f = fopen(byteArray.constData(),"rb");
//#else
    FILE * f = fopen(lpszWavFilePath,"rb");
//#endif
    if (f==0) {
        return -1;
    }

    char style[4]; ///定义一个四字节的数据，用来存放文件的类型；
    fseek(f,8,SEEK_SET);
    fread(style,1,4,f);
    if(style[0]!='W'||style[1]!='A'||style[2]!='V'||style[3]!='E')//判断该文件是否为"WAVE"文件格式
    {
        fclose(f);
        return -2;
    }

    /// WAV格式文件所占容量（KB) = （取样频率 X 量化位数 X 声道） X 时间 / 8 (字节= 8bit) ，
    /// 每一分钟WAV格式的音频文件的大小为10MB，其大小不随音量大小及清晰度的变化而变化。

    EB_PCMWAVEFORMAT format; /// 定义PCMWAVEFORMAT结构对象，用来判断WAVE文件格式；
    fseek(f,20,SEEK_SET);
    fread((char*)&format,1,sizeof(EB_PCMWAVEFORMAT),f); ///获取该结构的数据；
    if (format.wf.nAvgBytesPerSec==0)
    {
        fclose(f);
        return -3;
    }

    /// 获取WAVE文件 data 数据标识
    fseek(f,36,SEEK_SET);
    fread(style,1,4,f);
    ///判断是否标准data文件，如果是使用44字节文件头，否则使用46字节文件头
    if(style[0]!='d'||style[1]!='a'||style[2]!='t'||style[3]!='a') {
        fseek(f,42,SEEK_SET);
    }
    //fseek(f,40,SEEK_SET);
    ////获取WAVE文件的声音数据的大小；
    unsigned long size = 0;
    fread((char*)&size,1,4,f);

    /// 计算文件时长
    const int timeLength = size/format.wf.nAvgBytesPerSec;
    fclose(f);
    return timeLength;
}

void EbClientApp::writeVoiceMessage(QString &richText, const QString &voiceFile)
{
    const int nWavTimeLength = GetWaveTimeLength(voiceFile.toLocal8Bit().constData());
    QString sText;
    if (nWavTimeLength>=0) {
        /// 语音消息
        sText = QString("%1 %2\"")
                .arg(m_locales->getLocalText( "chat-msg-text.voice-msg","Voice Message" ))
                .arg(nWavTimeLength);
    }
    else if (nWavTimeLength==-1) {
        /// 语音消息不存在
        sText = m_locales->getLocalText( "chat-msg-text.voice-msg-not-exist","Voice Message Not Exist" );
    }
    else {
        /// 语音消息格式错误
        sText = m_locales->getLocalText( "chat-msg-text.voice-msg-format-error","Voice Message Format Error" );
    }
    richText += sText;
}

void EbClientApp::checkSaveCallInfo(const EbcCallInfo::pointer &pEbCallInfo)
{
    char sql[256];
    if (pEbCallInfo->isGroupCall()) {
        sprintf(sql, "SELECT call_id FROM call_record_t WHERE call_id=%lld LIMIT 1",
                pEbCallInfo->groupId());
    }
    else {
        sprintf(sql, "SELECT from_uid FROM call_record_t WHERE from_uid=%lld AND dep_code=0 AND emp_code>=0 LIMIT 1",
                 pEbCallInfo->fromUserId());
    }
    if (m_sqliteUser->select(sql)==0) {
        /// 可能被删除聊天会话，增加一个聊天会话
        saveCallRecord(pEbCallInfo->callId(),
                       pEbCallInfo->groupId(),
                       pEbCallInfo->m_pFromAccountInfo,
                       true);
    }
}

void EbClientApp::processRichInfo(const EbcCallInfo::pointer &pEbCallInfo,
                                  const CCrRichInfo *pCrMsgInfo,
                                  bool receive,
                                  bool saveHistory)
{
    const EB_STATE_CODE nState = pCrMsgInfo->GetStateCode();
    if (nState==EB_STATE_TIMEOUT_ERROR) {
        emit displayMessageTip(QString::number(pEbCallInfo->groupId()),
                               QString::number(pCrMsgInfo->m_sSendFrom),
                               m_locales->getLocalText("chat-msg-text.send-rich-timeout", "Send Timeout"));
        return;
    }
    else if (nState==EB_STATE_MAX_CAPACITY_ERROR) {
        /// 超过最大消息长度！
        emit displayMessageTip(QString::number(pEbCallInfo->groupId()),
                               QString::number(pCrMsgInfo->m_sSendFrom),
                               m_locales->getLocalText("chat-msg-text.max-capacity-error","Max Capacity Error"));
        return;
    }
    else if (nState==EB_STATE_GROUP_FORBID_SPEECH) {
        /// 群禁言中！
        emit displayMessageTip(QString::number(pEbCallInfo->groupId()),
                               QString::number(pCrMsgInfo->m_sSendFrom),
                               m_locales->getLocalText("chat-msg-text.group-forbid-speech","Group Forbid Speech"));
        return;
    }
    else if (nState==EB_STATE_FORBID_SPEECH) {
        /// 你被禁言中！
        emit displayMessageTip(QString::number(pEbCallInfo->groupId()),
                               QString::number(pCrMsgInfo->m_sSendFrom),
                               m_locales->getLocalText("chat-msg-text.forbid-speech","Forbid Speech"));
        return;
    }
    else if (pCrMsgInfo->m_pRichMsg==0 || pCrMsgInfo->m_pRichMsg->GetMsgId()==0) {
        emit displayMessageTip(QString::number(pEbCallInfo->groupId()),
                               QString::number(pCrMsgInfo->m_sSendFrom),
                               m_locales->getLocalText("chat-msg-text.send-rich-error", "Send Error"));
        return;
    }
    const int notifyRet = receive?checkMessageNotify():0;

    checkSaveCallInfo(pEbCallInfo);
    EbQuickRichInfo * richInfo = new EbQuickRichInfo();
    richInfo->setGroupId(pEbCallInfo->groupId());
    richInfo->setCallId(pCrMsgInfo->GetCallId());
    richInfo->setMsgId(pCrMsgInfo->m_pRichMsg->GetMsgId());
    richInfo->setSendFromUserId(pCrMsgInfo->m_sSendFrom);
    richInfo->setSendToUserId(pCrMsgInfo->m_sSendTo);
    richInfo->setRead(false);
    if (pEbCallInfo->isGroupCall()) {
        richInfo->setSendFromName(userName(richInfo->groupId(), richInfo->sendFromUserId()));
        richInfo->setSendToName(userName(richInfo->groupId(), richInfo->sendToUserId()));
    }
    else {
        richInfo->setSendFromName(userNameByUserId(richInfo->sendFromUserId()));
        richInfo->setSendToName(userNameByUserId(richInfo->sendToUserId()));
    }
    richInfo->setPrivate(pCrMsgInfo->m_bPrivate);
    richInfo->setReceive(receive);

    const EB_ChatRoomRichMsg * pRichMsg = pCrMsgInfo->m_pRichMsg;
    char sSql[theMaxDatabaseTextSize+2048];

    tstring sInFromName(richInfo->sendFromName().toStdString());
    m_sqliteUser->escape_string_in(sInFromName);
    tstring sInToName(richInfo->sendToName().toStdString());
    m_sqliteUser->escape_string_in(sInToName);
    eb::bigint sSaveDbToAccount = pCrMsgInfo->m_sSendTo;
    if (!pEbCallInfo->isGroupCall()) {
        sSaveDbToAccount = pEbCallInfo->m_pFromAccountInfo.GetUserId();
    }

    std::string sDBMsgTimeField;
    char sDBMsgTimeValue[64];
    memset(sDBMsgTimeValue,0,sizeof(sDBMsgTimeValue));
    time_t tMsgTime = time(0);
    if (!pRichMsg->m_sMsgTime.empty()) {
        ChangeTime(pRichMsg->m_sMsgTime.c_str(),tMsgTime);
        sDBMsgTimeField = "msg_time,";
        sprintf(sDBMsgTimeValue, "Datetime('%s','utc'),", pRichMsg->m_sMsgTime.c_str());
    }
    richInfo->setTime(tMsgTime);
    int nReadFlag = receive?0:1;
    if (receive) {
        if (m_currentChatGroupId>0 && m_currentChatGroupId==richInfo->groupId()) {
            nReadFlag = 1;
        }
        else if (m_currentChatGroupId==0 && 0==richInfo->groupId() &&
                 m_currentChatUserId==richInfo->sendFromUserId()) {
            nReadFlag = 1;
        }
    }
    QString lastInfo;

    int nDisplayIndex = 0;
    QString richText;
    const std::vector<EB_ChatRoomMsgItem*>& pRichMsgList = pRichMsg->GetList();
    for (size_t i=0; i<pRichMsgList.size(); i++) {
        const EB_ChatRoomMsgItem* pMsgItem = pRichMsgList[i];
        if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_TEXT) {

            /// 计算最后一条聊天消息
            if (lastInfo.isEmpty()) {
                if (!richInfo->isGroupCall() || richInfo->sendFromUserId()==this->logonUserId()) {
                    lastInfo = pMsgItem->GetText().substr(0, 60).c_str();
                }
                else {
                    lastInfo = QString("%1:%2")
                            .arg(richInfo->sendFromName())
                            .arg(pMsgItem->GetText().substr(0, 60).c_str());
                }
            }

            insertPlainTextEb(richText, QString::fromStdString(pMsgItem->GetText().string()),QColor(64,64,64) );

            if (saveHistory && isSaveConversationLocal() && !m_ebum.EB_IsLogonVisitor())
            {
                tstring sText(pMsgItem->GetText());
                CSqliteCdbc::escape_string_in(sText);
                while (!sText.empty()) {
                    const tstring sSaveText = sText.size()>theMaxDatabaseTextSize?sText.substr(0,theMaxDatabaseTextSize):sText;
                    sprintf(sSql,"INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_text,read_flag,display_index) "
                                 "VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s',%d,%d)",
                            sDBMsgTimeField.c_str(),sDBMsgTimeValue,pRichMsg->GetMsgId(),pEbCallInfo->groupId(),pCrMsgInfo->m_sSendFrom,sInFromName.c_str(),
                            sSaveDbToAccount,sInToName.c_str(),(int)(pCrMsgInfo->m_bPrivate?1:0),(int)MRT_TEXT,sSaveText.c_str(),nReadFlag,
                            nDisplayIndex++);
                    m_sqliteUser->execute(sSql);
                    sText = sText.substr(sSaveText.size());
                }
            }
        }
        else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_RESOURCE) {
            const tstring sResourceInfo(pMsgItem->GetText());
            std::vector<tstring> pList;
            if (ParseString(sResourceInfo.c_str(),";",pList)>=3) {
                const tstring sResourceId(pList[0]);
                const tstring sDescription = pList.size()>3?pList[3]:"";

                /// 计算最后一条聊天消息
                if (lastInfo.isEmpty()) {
                    if (!richInfo->isGroupCall() || richInfo->sendFromUserId()==this->logonUserId()) {
                        lastInfo = QString("[%1]").arg(sDescription.c_str());
                    }
                    else {
                        lastInfo = QString("%1:[%2]")
                                .arg(richInfo->sendFromName())
                                .arg(sDescription.c_str());
                    }
                }

                QString sImageFileName = QString("%1/%2").arg(m_ebum.EB_GetResourcePath()).arg(sResourceId.c_str());
//                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
//                    const QString sTemp = QString("<img src=\"%1\" alt=\"%2\"/>").arg(sImageFileName).arg(sDescription.c_str());
//                    nOutMsgLength += 30;
//                    *sOutFirstMsg1 += sTemp;
//                }
//                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
//                    const QString sTemp = QString("[%1]").arg(sDescription.c_str());
//                    *sOutFirstMsg2 += sTemp;
//                }
                /// 使用这种复制后，可以粘贴显示，并发送（以文件图片发送，不是以资源发送，后期再优化成资源发送）
                insertImage(richText, sImageFileName, sDescription.c_str(), 24, 24);
                richInfo->setMsgType(2);
                richInfo->setMsgName(sImageFileName);

                if (saveHistory && isSaveConversationLocal() && !m_ebum.EB_IsLogonVisitor()) {
                    sprintf(sSql,"INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag,display_index) "
                                 "VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s',%d,%d)",
                            sDBMsgTimeField.c_str(),sDBMsgTimeValue,pRichMsg->GetMsgId(),pEbCallInfo->groupId(),pCrMsgInfo->m_sSendFrom,sInFromName.c_str(),
                            sSaveDbToAccount,sInToName.c_str(),(int)(pCrMsgInfo->m_bPrivate?1:0),(int)MRT_JPG,sImageFileName.toStdString().c_str(),sResourceInfo.c_str(),nReadFlag,
                            nDisplayIndex++);
//                    sprintf(sSql,"INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag) "
//                                 "VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s',%d)",
//                            sDBMsgTimeField.c_str(),sDBMsgTimeValue,pRichMsg->GetMsgId(),pEbCallInfo->groupId(),pCrMsgInfo->m_sSendFrom,sInFromName.c_str(),
//                            sSaveDbToAccount,sInToName.c_str(),(int)(pCrMsgInfo->m_bPrivate?1:0),(int)MRT_JPG,sImageFileName.toStdString().c_str(),sDescription.c_str(),nReadFlag);
                    m_sqliteUser->execute(sSql);
                }
            }
        }
        else if (pMsgItem->GetType() == EB_ChatRoomMsgItem::MIT_OBJECT) {
            QString sObjectFileName;
            tstring sObjectSaveData;	/// ** to save msg_text
            const EB_RICH_SUB_TYPE nSubType = (EB_RICH_SUB_TYPE)pCrMsgInfo->m_pRichMsg->GetSubType();
            bool bIsFile = true;
            EB_MSG_RECORD_TYPE nRecordType = MRT_JPG;
            if (receive || nState==EB_STATE_FORWARD_MSG) {
                static unsigned int static_index = 0;
                static_index++;
                if (nSubType == EB_RICH_SUB_TYPE_JPG) {
                    nRecordType = MRT_JPG;
                    sObjectFileName = QString("%1/%2%3%4.jpg").arg(userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                else if (nSubType==EB_RICH_SUB_TYPE_AUDIO) {	/// wav格式
                    nRecordType = MRT_WAV;
                    sObjectFileName = QString("%1/%2%3%4.wav").arg(userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                else if (nSubType==EB_RICH_SUB_TYPE_MAP_POS) {	/// 地图位置
                    bIsFile = false;
                    nRecordType = MRT_MAP_POS;
                    //sObjectFileName = pMsgItem->GetData();
                }
                else if (nSubType==EB_RICH_SUB_TYPE_CARD_INFO) {	/// 名片数据
                    bIsFile = false;
                    nRecordType = MRT_CARD_INFO;
                    //sObjectFileName = pMsgItem->GetData();
                    sObjectSaveData = pMsgItem->GetData();
                }
                else if (nSubType==EB_RICH_SUB_TYPE_USER_DATA) {	/// 用户自定义数据
                    nRecordType = MRT_USER_DATA;
                    sObjectFileName = QString("%1/%2%3%4").arg(userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                else {
                    /// 普通文件；
                    nRecordType = MRT_FILE;
                    sObjectFileName = QString("%1/%2%3%4").arg(userImagePath()).arg(time(0)).arg(rand()%0xff).arg(static_index%0xff);
                }
                if (bIsFile) {
                //if (nRecordType != MRT_MAP_POS) {
                    const char * lpObjectData = pMsgItem->GetData();
                    const unsigned long dwDataSize = pMsgItem->GetSize();
                    FILE * hFile = fopen(sObjectFileName.toLocal8Bit().constData(),"wb");
                    if (hFile!=0) {
                        fwrite(lpObjectData,1,dwDataSize,hFile);
//                        fwrite(lpObjectData,dwDataSize,1,hFile);
                        fclose(hFile);
                    }
                }
            }
            else {
                sObjectFileName = pMsgItem->GetText().c_str();
                if (nSubType == EB_RICH_SUB_TYPE_AUDIO)
                    nRecordType = MRT_WAV;
                else if (nSubType == EB_RICH_SUB_TYPE_MAP_POS)		/// ??
                    nRecordType = MRT_MAP_POS;
                else if (nSubType == EB_RICH_SUB_TYPE_CARD_INFO) {	/// 名片信息
                    nRecordType = MRT_CARD_INFO;
                    sObjectSaveData = pMsgItem->GetData();
                }
                else if (nSubType == EB_RICH_SUB_TYPE_USER_DATA) 	/// ??
                    nRecordType = MRT_USER_DATA;
            }

            if (nSubType == EB_RICH_SUB_TYPE_JPG) {
                /// 计算最后一条聊天消息
                if (lastInfo.isEmpty()) {
                    if (!richInfo->isGroupCall() || richInfo->sendFromUserId()==this->logonUserId()) {
                        lastInfo = QString("[%1]")
                                .arg(m_locales->getLocalText("chat-msg-text.image", "Image"));
                    }
                    else {
                        lastInfo = QString("%1:[%2]")
                                .arg(richInfo->sendFromName())
                                .arg(m_locales->getLocalText("chat-msg-text.image", "Image"));
                    }
                }

                insertImage(richText, sObjectFileName, "");
                richInfo->setMsgType(2);
                richInfo->setMsgName(sObjectFileName);
//                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
//                    nOutMsgLength += 30;
//                    *sOutFirstMsg1 += QString("<img src=\"%1\" width=\"80\" height=\"80\" />").arg(sObjectFileName);
//                }
//                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
//                    /// [图片]
//                    *sOutFirstMsg2 += "[";
//                    *sOutFirstMsg2 += m_locales->getLocalText("chat-msg-text.image","Image");
//                    *sOutFirstMsg2 += "]";
//                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_AUDIO) {
                /// wav格式
                //WriteFileHICON(sObjectFileName);
                QString sText;
                /// 计算最后一条聊天消息
                if (lastInfo.isEmpty()) {
                    if (!richInfo->isGroupCall() || richInfo->sendFromUserId()==this->logonUserId()) {
                        lastInfo = QString("[%1]")
                                .arg(m_locales->getLocalText("chat-msg-text.voice", "Voice"));
                    }
                    else {
                        lastInfo = QString("%1:[%2]")
                                .arg(richInfo->sendFromName())
                                .arg(m_locales->getLocalText("chat-msg-text.voice", "Voice"));
                    }
                }

                writeVoiceMessage(richText, sObjectFileName);
                richInfo->setMsgType(3);
                richInfo->setMsgName(sObjectFileName);

//                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
//                    nOutMsgLength += 12;
//                    *sOutFirstMsg1 +=  sText;
//                }
//                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
//                    /// [语音]
//                    *sOutFirstMsg2 += "[";
//                    *sOutFirstMsg2 += m_locales->getLocalText("chat-msg-text.voice","Voice");
//                    *sOutFirstMsg2 += "]";
//                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_MAP_POS) {
                /// 地图位置 ???
//                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
//                    /// [地图位置]
//                    *sOutFirstMsg2 += "[";
//                    *sOutFirstMsg2 += m_locales->getLocalText("chat-msg-text.map-pos","Map Pos");
//                    *sOutFirstMsg2 += "]";
//                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_CARD_INFO) {
                /// 名片信息
                /// 计算最后一条聊天消息
//                if (lastInfo.isEmpty()) {
//                    if (!richInfo->isGroupCall() || richInfo->sendFromUserId()==this->logonUserId()) {
//                        lastInfo = QString("[%1]")
//                                .arg(m_locales->getLocalText("chat-msg-text.user-ecard", "User ECard"));
//                    }
//                    else {
//                        lastInfo = QString("%1:[%2]")
//                                .arg(richInfo->sendFromName())
//                                .arg(m_locales->getLocalText("chat-msg-text.user-ecard", "User ECard"));
//                    }
//                }

                /// *** 这里要解析并显示名片信息
                QString userName;
                if (writeCardDataMessage(richText, sObjectSaveData, &userName)) {
                    lastInfo = QString("%1[%2]")
                            .arg(userName)
                            .arg(m_locales->getLocalText("chat-msg-text.user-ecard", "User ECard"));
                    richInfo->setMsgType(4);
                    richInfo->setMsgName(sObjectSaveData.c_str());
                    richInfo->setRichDesc(m_locales->getLocalText("chat-msg-text.user-ecard","User Ecard"));
                }
            }
            else if (nSubType==EB_RICH_SUB_TYPE_USER_DATA) {
                /// 用户自定义数据 ???
//                if (sOutFirstMsg2!=0 && sOutFirstMsg2->length()<const_max_length) {
//                    /// [用户自定义数据]
//                    *sOutFirstMsg2 += "[";
//                    *sOutFirstMsg2 += m_locales->getLocalText("chat-msg-text.user-data","User Data");
//                    *sOutFirstMsg2 += "]";
//                }
            }
            else {
                /// 计算最后一条聊天消息
                if (lastInfo.isEmpty()) {
                    if (!richInfo->isGroupCall() || richInfo->sendFromUserId()==this->logonUserId()) {
                        lastInfo = QString("[%1]")
                                .arg(m_locales->getLocalText("chat-msg-text.file", "File"));
                    }
                    else {
                        lastInfo = QString("%1:[%2]")
                                .arg(richInfo->sendFromName())
                                .arg(m_locales->getLocalText("chat-msg-text.file", "File"));
                    }
                }

                QString sFileName;
//                writeFileMessage(receive, msgId, 0, sObjectFileName, 0, false, &sFileName);
//                if (sOutFirstMsg1!=0 && nOutMsgLength<const_max_length) {
//                    /// 《file-name》文件
//                    const QString sTemp = QString("《%1》%2").arg(sFileName).arg(m_locales->getLocalText("chat-msg-text.file","File"));
//                    nOutMsgLength += sTemp.length();
//                    *sOutFirstMsg1 += sTemp;
//                }
            }
            if (saveHistory && isSaveConversationLocal() && !m_ebum.EB_IsLogonVisitor()) {
                CSqliteCdbc::escape_string_in(sObjectSaveData);
                sprintf(sSql,"INSERT INTO msg_record_t(%smsg_id,dep_code,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag,display_index) "
                             "VALUES(%s%lld,%lld,%lld,'%s',%lld,'%s',%d,%d,'%s','%s',%d,%d)",
                        sDBMsgTimeField.c_str(),sDBMsgTimeValue,pRichMsg->GetMsgId(),pEbCallInfo->groupId(),pCrMsgInfo->m_sSendFrom,sInFromName.c_str(),
                        sSaveDbToAccount,sInToName.c_str(),(int)(pCrMsgInfo->m_bPrivate?1:0),nRecordType,sObjectFileName.toStdString().c_str(),sObjectSaveData.c_str(),nReadFlag,
                        nDisplayIndex++);
                m_sqliteUser->execute(sSql);
            }
        }
    }
    if (notifyRet==1) {
        const QString title =  pEbCallInfo->isGroupCall()?
                    this->groupName(pEbCallInfo->groupId()):
                    richInfo->sendFromName();
        this->sendNotify(1, title, lastInfo);
    }
    /// ** 必须放在前面
    if (!lastInfo.isEmpty()) {
        emit receivedSessionLastInfo(richInfo->groupIdString(),
                                     richInfo->sendFromUserIdString(), lastInfo);
    }
    richInfo->setRichText(richText);
    sendRichInfo(richInfo);
}

void EbClientApp::onSendRich(QEvent *e)
{
    const CCrRichInfo *pCrMsgInfo = (const CCrRichInfo*)e;
    const EB_STATE_CODE nState = pCrMsgInfo->GetStateCode();
    if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState)
        return; // 0;

    const eb::bigint sCallId = pCrMsgInfo->GetCallId();
    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(sCallId, pEbCallInfo)) {
        return; // 1;
    }
    pEbCallInfo->m_tLastTime = time(0);
    processRichInfo(pEbCallInfo, pCrMsgInfo, false, true);

//    richInfo->setMsgId(pCrMsgInfo->m_pRichMsg->GetSubType());

//    EbDialogChatBase::pointer pDlgDialog = getDialogChatBase(sCallId);
//    if (pDlgDialog.get()==NULL) {
//        return; // 1;
//    }
//    pDlgDialog->onSendRich(pCrMsgInfo,nState);
//    if (m_pDlgMsgTip!=0) {
//        m_pDlgMsgTip->delMsgTip(pEbCallInfo->m_pCallInfo.m_sGroupCode,pEbCallInfo->m_pFromAccountInfo.GetUserId());
    //    }
}

int EbClientApp::checkMessageNotify()
{
    /// 避免消息多，提示声音太频繁，变成噪声
    const time_t tNow = time(0);
    static time_t theLastCheckTime = 0;
    if ((theLastCheckTime+3)>tNow) {
        return 0;
    }
    theLastCheckTime = tNow;
    Qt::ApplicationState state = qApp->applicationState();
    if (state!=Qt::ApplicationActive) {
        return 1;
    }
    else if (m_currentChatGroupId==0 && m_currentChatUserId==0) {
        if (this->voiceNotify(true)) {
            /// 消息提示音
            this->playTone();
        }
#ifdef Q_OS_ANDROID
        if (this->vibrateNotify(false)) {
            /// 振动
            this->vibrate();
        }
#endif
        return 2;
    }
    return 0;
}

bool EbClientApp::onReceiveRich(QEvent *e)
{
    const CCrRichInfo *pCrMsgInfo = (const CCrRichInfo*)e;
    const eb::bigint nFromUserid = pCrMsgInfo->m_sSendFrom;
    const eb::bigint sCallId = pCrMsgInfo->GetCallId();
    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(sCallId,pEbCallInfo)) {
        return false;
    }
    pEbCallInfo->m_tLastTime = time(0);
    processRichInfo(pEbCallInfo, pCrMsgInfo, true, true);

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo,false);
//    QString sFirstMsg1;
//    QString sFirstMsg2;
//    chatBase->onReceiveRich(pCrMsgInfo,&sFirstMsg1,&sFirstMsg2);
//    const eb::bigint nMsgId = pCrMsgInfo->m_pRichMsg->GetMsgId();
//    //CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_RECEIVE_RICH);
//    //pEbcMsgInfo->m_pEbCallInfo = pEbCallInfo;
//    //m_pEbcMsgList.add(pEbcMsgInfo);

//    if (!chatBase->isVisible()) {
//        QSound::play( ":/wav/msg.wav" );

//        bool bRet = false;
//        const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//        if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=0) {
//            bRet = m_pDlgFrameList->addUnreadMsg(sCallId,nMsgId);
//        }
//        else if (!theApp->isHideMainFrame()) {
////            const bool bRet = CFrameWndInfoProxy::AddUnreadMsg(sCallId, nMsgId);
////            if (bRet && this->IsWindowVisible())
////            {
////                ::FlashWindow(this->GetSafeHwnd(), TRUE);
////                return 0;
////            }
//        }
//        else if (m_pDlgFrameList!=0) {
//            bRet = m_pDlgFrameList->addUnreadMsg(sCallId,nMsgId);
//        }

//        if (bRet && m_pDlgFrameList!=0 && m_pDlgFrameList->isVisible()) {
////            ::FlashWindow(m_pDlgFrameList->GetSafeHwnd(), TRUE);
//            /// 会弹到最前面
//            m_pDlgFrameList->showNormal();
//            m_pDlgFrameList->activateWindow();
//            return true;
//        }
//        //m_btnMySession.SetWindowText(_T("！"));
//        if (m_pDlgMsgTip!=0) {
//            m_pDlgMsgTip->addMsgTip(chatBase->fromImage(), pEbCallInfo->groupId(),nFromUserid,sFirstMsg2);
//        }
//    }
//    else {
//        if (m_pDlgFrameList!=0) {
//            /// 会弹到最前面
//            m_pDlgFrameList->showNormal();
//            m_pDlgFrameList->activateWindow();
//        }
//    }
    return true;
}


tstring EbClientApp::GetFileName(const tstring & sPathName)
{
    tstring sFileName(sPathName);
    int find = sPathName.rfind("\\");
    if (find < 0)
        find = sPathName.rfind("/");
    if (find > 0)
        sFileName = sPathName.substr(find+1);
    return sFileName;
}
QString EbClientApp::getFileText(const CCrFileInfo *fileInfo)
{
    const tstring fileName = GetFileName(fileInfo->m_sFileName.toStdString());
    const qint64 fileSize = fileInfo->m_nFileSize;
    char fileText[260];
    if (fileSize == -1)    /// 文件不存在
        sprintf(fileText,"%s (%s)",fileName.c_str(), m_locales->getLocalStdString("chat-msg-text.file-not-exist","File not exist").c_str());
    else if (fileSize >= const_gb_size)
        sprintf(fileText,"%s (%.02fGB)",fileName.c_str(),(double)fileSize/const_gb_size);
    else if (fileSize >= const_mb_size)
        sprintf(fileText,"%s (%.02fMB)",fileName.c_str(),(double)fileSize/const_mb_size);
    else if (fileSize >= const_kb_size)
        sprintf(fileText,"%s (%.02fKB)",fileName.c_str(),(double)fileSize/const_kb_size);
    else if (fileSize>=0)
        sprintf(fileText,"%s (%lldByte)",fileName.c_str(),fileSize);
    else ///if (resourceId>0)
        sprintf(fileText,"%s",fileName.c_str());
    return fileText;
}
void EbClientApp::onSendingFile(QEvent *e)
{
    const CCrFileInfo *fileInfo = (const CCrFileInfo*)e;
    const EB_STATE_CODE state = (EB_STATE_CODE)fileInfo->GetStateCode();
    if (EB_STATE_APPID_KEY_ERROR==state || EB_STATE_APP_ONLINE_KEY_TIMEOUT==state) {
        return;
    }

    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(fileInfo->GetCallId(), pEbCallInfo)) {
        return;
    }
    pEbCallInfo->m_tLastTime = time(0);

    switch (state) {
    case EB_STATE_GROUP_FORBID_SPEECH: {
        /// 群禁言中：\r\n不能发送群文件！
        QString text = m_locales->getLocalText("on-send-file.group-forbid-speech.text","Group Forbid Speech");
        text.replace( "[STATE_CODE]", QString::number((int)state) );
        sendMessageTip(pEbCallInfo, text);
        return;
    }
    case EB_STATE_FORBID_SPEECH: {
        /// 你被禁言中：<br>不能发送群文件！
        QString text = m_locales->getLocalText("on-send-file.forbid-speech.text","Forbid Speech");
        text.replace( "[STATE_CODE]", QString::number((int)state) );
        sendMessageTip(pEbCallInfo, text);
        return;
    }
    case EB_STATE_FILE_ALREADY_EXIST:
            //return 0;	/// 不能返回，后面处理
    case EB_STATE_OK:
    case EB_STATE_WAITING_PROCESS:	/// * *等待处理
            break;
    case EB_STATE_NOT_AUTH_ERROR: {
        /// 没有权限：\r\n请联系管理员！
        QString text = m_locales->getLocalText("on-send-file.not-auth-error.text","Not Auth Error");
        text.replace( "[STATE_CODE]", QString::number((int)state) );
        sendMessageTip(pEbCallInfo, text);
        return;
    }
    case EB_STATE_EXCESS_QUOTA_ERROR:
    case EB_STATE_FILE_BIG_LONG: {
        /// 当前文件大小超出系统最大配额：\r\n请发送更小文件，或联系公司客服！
        QString text = m_locales->getLocalText("on-send-file.file-big-long.text","File Big Long");
        text.replace( "[STATE_CODE]", QString::number((int)state) );
        sendMessageTip(pEbCallInfo, text);
        return;
    }
    case EB_STATE_TIMEOUT_ERROR: {
        /// 文件发送超时：\r\n请重新发送！
        QString text = m_locales->getLocalText("on-send-file.timeout-error.text","Timeout Error");
        text.replace( "[STATE_CODE]", QString::number((int)state) );
        sendMessageTip(pEbCallInfo, text);
        return;
    }
    case EB_STATE_NOT_MEMBER_ERROR: {
        /// 没有其他群组成员：\r\n不能发送文件！
        QString text = m_locales->getLocalText("on-send-file.not-member-error.text","Not Member Error");
        text.replace( "[STATE_CODE]", QString::number((int)state) );
        sendMessageTip(pEbCallInfo, text);
        return;
    }
    default: {
        /// 文件发送失败，错误代码：%d\r\n请检查后重试！
        QString text = m_locales->getLocalText("on-send-file.other-error.text","Send File Error");
        text.replace( "[STATE_CODE]", QString::number((int)state) );
        sendMessageTip(pEbCallInfo, text);
        return;
    }
    }

    EbFileInfo::pointer ebFileInfo = EbFileInfo::create(*fileInfo);
    if (!pEbCallInfo->m_progressFiles.insert(fileInfo->m_nMsgId, ebFileInfo, false)) {
        return;
    }
    ebFileInfo->setReceive(false);

    if ( fileInfo->GetParam()==0 && fileInfo->m_nMsgId>0 && fileInfo->GetStateCode()==EB_STATE_OK ) {
        if ( isSaveConversationLocal()  && !isLogonVisitor() ) {
            //if (fileInfo->m_sReceiveAccount==theApp->m_ebum.EB_GetLogonUserId())
            {
                tstring sInFromName;
                if (pEbCallInfo->groupId()==0)
                    sInFromName = m_ebum.EB_GetUserName();
                else
                    m_ebum.EB_GetMemberNameByUserId(pEbCallInfo->groupId(), logonUserId(), sInFromName);
                m_sqliteUser->escape_string_in(sInFromName);
                tstring sInFileName(fileInfo->m_sFileName.toStdString());
                m_sqliteUser->escape_string_in(sInFileName);
                const eb::bigint sSaveDbToAccount = pEbCallInfo->groupId()==0?pEbCallInfo->fromUserId():fileInfo->m_sSendTo;
                char sSql[1024];
                sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) "
                               "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')",
                         fileInfo->m_nMsgId,
                         pEbCallInfo->groupId(),
                         fileInfo->m_sSendFrom,
                         sInFromName.c_str(),
                         sSaveDbToAccount,
                         (int)(fileInfo->m_bPrivate?1:0),
                         (int)MRT_FILE,
                         sInFileName.c_str() );
                m_sqliteUser->execute(sSql);
            }
        }
//        const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName.toStdString());
//        /// 发送文件：%s
//        const QString sWindowText = QString("%1: %2").arg(theLocales.getLocalText("chat-msg-text.send-file","Send File")).arg(sFileName.c_str());
//        m_textBrowserMessage->addLineString(pCrFileInfo->m_nMsgId,sWindowText);
    }

    if (fileInfo->GetStateCode()==EB_STATE_FILE_ALREADY_EXIST) {
        if (fileInfo->GetParam()>0 &&
                pEbCallInfo->m_progressFiles.remove((qint64)fileInfo->GetParam())) {
            emit fileSent(QString::number(pEbCallInfo->groupId()),
                          QString::number(pEbCallInfo->fromUserId()),
                          QString::number((qint64)fileInfo->GetParam()));
        }
        return;
    }

//    EbWidgetFileTranItem::pointer tranFile;
//    if (fileInfo->GetParam()>0 && m_tranFileList.find(fileInfo->GetParam(),tranFile,true)) {
//        /// ** 找到前面等待预处理发送文件，更新界面即可
//        tranFile->updateFileInfo(fileInfo);
//    }

    const QString richText = getFileText(fileInfo);
    EbQuickRichInfo *richInfo = new EbQuickRichInfo(fileInfo);
    richInfo->setProgressValue(0);
    richInfo->setRichText(richText);
//    richInfo->setRichDesc("等待对方接收");
//    richInfo->setGroupId(groupId);
//    richInfo->setTime(nMsgTime);
//    richInfo->setSendFromName(sFromName.c_str());
//    richInfo->setSendToName(sToName.c_str());
    richInfo->setReceive(false);
    richInfo->setRead(false);
//    emit receivedRichInfo(richInfo, "0");
    emit receivedRichInfo(richInfo, QString::number(fileInfo->GetParam()));

//    const eb::bigint sCallId = fileInfo->GetCallId();
//    const eb::bigint sResId = fileInfo->m_sResId;
//    if (sResId>0 && sCallId==0) {
//            onClickedPushButtonFileManager();
//            m_dialogFileManager->onSendingFile(fileInfo);
//            m_dialogFileManager->onClickedButtonTraningFile();
//    }

//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo);
//    chatBase->onSendingFile(fileInfo);
}

void EbClientApp::onSentFile(QEvent *e)
{
    const CCrFileInfo *fileInfo = (const CCrFileInfo*)e;
    const eb::bigint callId = fileInfo->GetCallId();
    const EB_STATE_CODE nState = (EB_STATE_CODE)fileInfo->GetStateCode();
    if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState) {
        return;
    }
    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(callId, pEbCallInfo))
    {
        return;
    }
    if (!pEbCallInfo->m_progressFiles.remove(fileInfo->m_nMsgId)) {
        return;
    }
    if (fileInfo->GetParam()==0) {
        //    if (pCrFileInfo->GetParam()==0 && m_pDlgChatInput.get() != NULL && m_pDlgChatInput->GetSafeHwnd())
        if ( isSaveConversationLocal() && !isLogonVisitor() ) {
            //if (pCrFileInfo->m_sReceiveAccount==m_ebum.EB_GetLogonUserId())
            {
                char sSql[1024];
                if (nState==EB_STATE_FILE_ALREADY_EXIST) {
                    /// ** 保存本地数据库
                    if ( pEbCallInfo->isGroupCall() ) {
                        char sMsgText[64];
                        sprintf( sMsgText, "%lld,%lld",fileInfo->m_sResId,fileInfo->m_nFileSize);
                        tstring sInMemberName;
                        m_ebum.EB_GetMemberNameByUserId(pEbCallInfo->groupId(),fileInfo->m_sSendFrom,sInMemberName);
                        m_sqliteUser->escape_string_in(sInMemberName);
                        const tstring sFileName = GetFileName(fileInfo->m_sFileName.toStdString());
                        tstring sInFileName(sFileName);
                        m_sqliteUser->escape_string_in(sInFileName);
                        sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_name,msg_text,read_flag) "
                                       "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s','%s',%d)",
                                 fileInfo->m_nMsgId,pEbCallInfo->groupId(),fileInfo->m_sSendFrom,sInMemberName.c_str(),
                                 fileInfo->m_sSendTo,fileInfo->m_bPrivate?1:0,MRT_RESOURCE,sInFileName.c_str(),sMsgText,(int)EBC_READ_FLAG_SENT);
                    }
                    else {
                        tstring sInFromName(m_ebum.EB_GetUserName());
                        m_sqliteUser->escape_string_in(sInFromName);
                        tstring sInFileName(fileInfo->m_sFileName.toStdString());
                        m_sqliteUser->escape_string_in(sInFileName);
                        const eb::bigint sSaveDbToAccount = pEbCallInfo->fromUserId();
                        //const eb::bigint sSaveDbToAccount = pEbCallInfo->m_pCallInfo.m_sGroupCode==0?pEbCallInfo->m_pFromAccountInfo.GetUserId():pCrFileInfo->m_sSendTo;
                        sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) "
                                       "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')",
                                 fileInfo->m_nMsgId,pEbCallInfo->groupId(),fileInfo->m_sSendFrom,sInFromName.c_str(),
                                 sSaveDbToAccount,fileInfo->m_bPrivate?1:0,MRT_FILE,sInFileName.c_str());
                    }
                }
                else {
                    int nReadFlag = EBC_READ_FLAG_SENT;
                    if (!fileInfo->m_bOffFile) {
                        nReadFlag |= EBC_READ_FLAG_RECEIPT;
                    }
                    sprintf( sSql, "UPDATE msg_record_t SET read_flag=read_flag|%d WHERE msg_id=%lld AND (read_flag&%d)=0",nReadFlag,fileInfo->m_nMsgId,nReadFlag);
                }
                m_sqliteUser->execute(sSql);
            }
        }
    }
    emit fileSent(QString::number(pEbCallInfo->groupId()),
                  QString::number(pEbCallInfo->fromUserId()),
                  QString::number(fileInfo->m_nMsgId));

//    const eb::bigint sCallId = fileInfo->GetCallId();
//    const eb::bigint sResId = fileInfo->m_sResId;
//    if (sResId>0 && sCallId==0) {
//        if (m_dialogFileManager!=0 &&
//                (fileInfo->m_sReceiveAccount==0 || fileInfo->m_sReceiveAccount==theApp->logonUserId()) ) {
//            m_dialogFileManager->deleteTranFile(fileInfo->m_nMsgId);
//            if (m_dialogFileManager->isEmpty()) {
//                m_dialogFileManager->setVisible(false);
//            }
//        }
//    }
//    //EbcCallInfo::pointer pEbCallInfo;
//    //if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
//    //{
//    //	return 1;
//    //}
//    //pEbCallInfo->m_tLastTime = time(0);
//    if (fileInfo->m_sReceiveAccount>0) {
//        EbDialogChatBase::pointer chatBase = getDialogChatBase(sCallId);
//        if (chatBase.get()!=0) {
//            if (chatBase->onSentFile(fileInfo) && !chatBase->isVisible()) {
//                QSound::play( ":/wav/msg.wav" );
//            }
//        }
//        else {
//            if ( fileInfo->m_sReceiveAccount!=theApp->logonUserId() ) {
//                /// *对方接收消息回执
//                const eb::bigint nFromUserId = fileInfo->m_sReceiveAccount;
//                const eb::bigint nMsgId = fileInfo->m_nMsgId;
//                theApp->updateMsgReceiptData(nMsgId, nFromUserId, 0);
//            }
//        }
//    }

}

void EbClientApp::onCancelFile(QEvent *e)
{
    const CCrFileInfo *fileInfo = (const CCrFileInfo*)e;
    const int lParam = (int)fileInfo->GetEventParameter();
    const bool bChangeP2PSending = lParam==1?true:false;
    const eb::bigint callId = fileInfo->GetCallId();
    const eb::bigint resourceId = fileInfo->m_sResId;
    const eb::bigint nReceiveAccount = fileInfo->m_sReceiveAccount;

    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(callId, pEbCallInfo)) {
        return;
    }
    if (!pEbCallInfo->m_progressFiles.remove(fileInfo->m_nMsgId)) {
        return;
    }
    if (fileInfo->GetParam()==0) {
        if (!bChangeP2PSending) {
            char sSql[128];
            sprintf( sSql, "DELETE FROM msg_record_t WHERE msg_id=%lld",fileInfo->m_nMsgId);
            m_sqliteUser->execute(sSql);
        }
    }

    const qint64 fromUserId = nReceiveAccount==0?this->logonUserId():nReceiveAccount;
    emit fileCancel(QString::number(pEbCallInfo->groupId()),
                    QString::number(fromUserId),
                    QString::number(fileInfo->m_nMsgId));

    //const eb::bigint nSendToUid = pCrFileInfo->m_sSendTo;
//    if (resourceId>0 && callId==0) {
//        if (m_dialogFileManager!=0 && nReceiveAccount==0) {
//            m_dialogFileManager->deleteTranFile(fileInfo->m_nMsgId);
//        }
//        if (resourceId==fileInfo->m_nMsgId) {
//            return;
//        }
//    }
//    //EbcCallInfo::pointer pEbCallInfo;
//    //if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
//    //{
//    //	return 1;
//    //}
//    //pEbCallInfo->m_tLastTime = time(0);
//    EbDialogChatBase::pointer chatBase = getDialogChatBase(callId);
//    if (chatBase.get()!=0) {
//        chatBase->onCancelFile(fileInfo,bChangeP2PSending);
//        if (!chatBase->isVisible()) {
//            if (lParam!=10) {
//                QSound::play( ":/wav/msg.wav" );
//            }
//        }
//    }
////    const tstring sFileName = libEbc::GetFileName(pCrFileInfo->m_sFileName);

//    EbcCallInfo::pointer pEbCallInfo;
//    if (nReceiveAccount!=0 && theApp->m_pCallList.find(callId,pEbCallInfo)) {
//        /// sReceiveAccount==0是主动取消
////        if (chatBase.get()==0 || !chatBase->IsWindowVisible())
////        {
////            CString sFirstMsg;
////            if (pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
////            {
////                sFirstMsg.Format(_T("用户取消发送文件：%s"),sFileName.c_str());
////                const CString sTemp(libEbc::ACP2UTF8(sFirstMsg).c_str());
////                sFirstMsg.Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-group://%lld\">%s</a>"),sTemp,pEbCallInfo->m_pCallInfo.m_sGroupCode,libEbc::ACP2UTF8("查看聊天内容").c_str());
////            }else
////            {
////                sFirstMsg.Format(_T("用户[%s]取消发送文件：%s"),pEbCallInfo->m_pCallInfo.GetFromAccount().c_str(),sFileName.c_str());
////                const CString sTemp(libEbc::ACP2UTF8(sFirstMsg).c_str());
////                sFirstMsg.Format(_T("<font color=\"#6c6c6c\">%s</font><br/><a href=\"ebim-call-account://%lld\">%s</a>"),sTemp,pEbCallInfo->m_pCallInfo.GetFromUserId(),libEbc::ACP2UTF8("查看聊天内容").c_str());
////            }

////            /// 显示消息
////            CWnd *pParent = CWnd::FromHandle(::GetDesktopWindow());
////            CDlgFuncWindow *pFuncWindow = new CDlgFuncWindow(pParent,true);
////            pFuncWindow->m_pEbCallInfo = pEbCallInfo;
////            pFuncWindow->m_bDisableContextMenu = true;
////            pFuncWindow->m_bBroadcastMsg = true;
////            pFuncWindow->m_bOpenNewClose = true;
////            pFuncWindow->m_sTitle = _T("文件消息提醒");
////            pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg;
////            pFuncWindow->m_nWidth = 250;
////            pFuncWindow->m_nHeight = 180;
////            CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
////            pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
////            pEbcMsgInfo->m_pParent = pParent;
////            m_pEbcMsgList.add(pEbcMsgInfo);
////        }
//    }
}

bool EbClientApp::onReceivingFile(QEvent *e)
{
    const CCrFileInfo *fileInfo = (const CCrFileInfo*)e;
    const eb::bigint resourceId = fileInfo->m_sResId;
    const int lParam = (int)fileInfo->GetEventParameter();
//    if (resourceId>0 && fileInfo->GetCallId()==0) {
//    //if (sResId>0 && (!pCrFileInfo->m_bOffFile || sResId==pCrFileInfo->m_nMsgId))
//        onClickedPushButtonFileManager();
//        m_dialogFileManager->onReceivingFile(fileInfo);
//        m_dialogFileManager->onClickedButtonTraningFile();
//        if (resourceId==fileInfo->m_nMsgId) {
//            return true;
//        }
//    }
    const eb::bigint callId = fileInfo->GetCallId();
    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(callId,pEbCallInfo))
    {
        return false;// -1;
    }
    pEbCallInfo->m_tLastTime = time(0);

    EbFileInfo::pointer ebFileInfo = EbFileInfo::create(*fileInfo);
    if (!pEbCallInfo->m_progressFiles.insert(fileInfo->m_nMsgId, ebFileInfo, false)) {
        return false;
    }
    ebFileInfo->setReceive(true);

    if (pEbCallInfo->isGroupCall() && fileInfo->m_sResId>0 &&
            (fileInfo->m_sSendFrom>0 || fileInfo->m_sSendTo>0)) {
        /// fileInfo->m_sSendFrom==0 && fileInfo->m_sSendTo==0 是下载资源文件，不处理
        const bool bOffLineUser = pEbCallInfo->m_bOffLineUser;
        const eb::bigint sSendFrom = fileInfo->m_sSendFrom;
        const tstring sFileName = GetFileName(fileInfo->m_sFileName.toStdString());

        CEBString sMemberName;
        m_ebum.EB_GetMemberNameByUserId(pEbCallInfo->groupId(), sSendFrom, sMemberName);

        /// ** 保存本地数据库（这里只保存群共享上传文件记录）
        if ( isSaveConversationLocal() && !isLogonVisitor() ) {
            char sMsgText[64];
            sprintf( sMsgText, "%lld,%lld", fileInfo->m_sResId, fileInfo->m_nFileSize);
            tstring sInMemberName(sMemberName);
            m_sqliteUser->escape_string_in(sInMemberName);
            tstring sInFileName(sFileName);
            m_sqliteUser->escape_string_in(sInFileName);
            char sSql[1024];
            sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_name,msg_text) "
                           "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s','%s')",
                     fileInfo->m_nMsgId,
                     pEbCallInfo->groupId(),
                     fileInfo->m_sSendFrom,
                     sInMemberName.c_str(),
                     fileInfo->m_sSendTo,
                     (int)(fileInfo->m_bPrivate?1:0),
                     (int)MRT_RESOURCE,
                     sInFileName.c_str(),
                     sMsgText);
            m_sqliteUser->execute(sSql);
        }
    }
    const int notifyRet = checkMessageNotify();
    QString richText = getFileText(fileInfo);
    EbQuickRichInfo *richInfo = new EbQuickRichInfo(fileInfo);
    richInfo->setProgressValue(0);
    richInfo->setRichText(richText);
//    richInfo->setGroupId(groupId);
//    richInfo->setTime(nMsgTime);
//    richInfo->setSendFromName(sFromName.c_str());
//    richInfo->setSendToName(sToName.c_str());
    richInfo->setReceive(true);
    emit receivedRichInfo(richInfo, "0");

    if (notifyRet==1) {
        const QString sendFromName = userNameByUserId(fileInfo->m_sSendFrom);
        const QString title =  pEbCallInfo->isGroupCall()?
                    this->groupName(pEbCallInfo->groupId()):
                    sendFromName;
        const tstring fileName = GetFileName(fileInfo->m_sFileName.toStdString());
        const QString text = pEbCallInfo->isGroupCall()?
                    QString("%1: [%2]").arg(sendFromName).arg(fileName.c_str()):
                    QString("[%2]").arg(fileName.c_str());
        this->sendNotify(1, title, text);
    }
//    EbDialogChatBase::pointer chatBase = getDialogChatBase(pEbCallInfo,false);
//    QString sFirstMsg;
//    chatBase->onReceivingFile(fileInfo,&sFirstMsg);

//    if (!chatBase->isVisible()) {
//        if (lParam!=10) {
//            QSound::play( ":/wav/msg.wav" );
//        }

//        bool bRet = false;
//        const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//        if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=0) {
//            bRet = m_pDlgFrameList->addUnreadMsg(callId,0);
//        }
//        else if (!theApp->isHideMainFrame()) {
////            const bool bRet = CFrameWndInfoProxy::AddUnreadMsg(sCallId,0);
////            if (bRet && this->IsWindowVisible())
////            {
////                ::FlashWindow(this->GetSafeHwnd(), TRUE);
////                return 0;
////            }
//        }
//        else if (m_pDlgFrameList!=0) {
//            bRet = m_pDlgFrameList->addUnreadMsg(callId,0);
//        }
//        if ( bRet && m_pDlgFrameList!=0 && m_pDlgFrameList->isVisible() ) {
////            ::FlashWindow(m_pDlgFrameList->GetSafeHwnd(), TRUE);
//            /// 会弹到最前面
//            m_pDlgFrameList->showNormal();
//            m_pDlgFrameList->activateWindow();
//            return true;
//        }

//        if (m_pDlgMsgTip!=0) {
//            m_pDlgMsgTip->addMsgTip(chatBase->fromImage(), pEbCallInfo->groupId(),pEbCallInfo->fromUserId(),sFirstMsg);
//        }
//        /// 显示消息
////        CWnd *pParent = CWnd::FromHandle(::GetDesktopWindow());
////        CDlgFuncWindow *pFuncWindow = new CDlgFuncWindow(pParent,true);
////        pFuncWindow->m_pEbCallInfo = pEbCallInfo;
////        pFuncWindow->m_bDisableContextMenu = true;
////        pFuncWindow->m_bBroadcastMsg = true;
////        pFuncWindow->m_bOpenNewClose = true;
////        pFuncWindow->m_sTitle = _T("文件消息提醒");
////        pFuncWindow->m_sFuncUrl = (LPCTSTR)sFirstMsg;
////        pFuncWindow->m_nWidth = 250;
////        pFuncWindow->m_nHeight = 180;
////        CEBCMsgInfo::pointer pEbcMsgInfo = CEBCMsgInfo::create(CEBCMsgInfo::MSG_TYPE_CREATE_FUNC_WINDOW);
////        pEbcMsgInfo->m_pFuncWindow = pFuncWindow;
////        pEbcMsgInfo->m_pParent = pParent;
////        m_pEbcMsgList.add(pEbcMsgInfo);
//    }
    return true;
}

void EbClientApp::onReceivedFile(QEvent *e)
{
    const CCrFileInfo *fileInfo = (const CCrFileInfo*)e;
    const int lParam = (int)fileInfo->GetEventParameter();
    const eb::bigint callId = fileInfo->GetCallId();
    const eb::bigint resourceId = fileInfo->m_sResId;
    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(callId, pEbCallInfo))
    {
        return;
    }
    if (!pEbCallInfo->m_progressFiles.remove(fileInfo->m_nMsgId)) {
        return;
    }
    if ( fileInfo->GetParam()==0 && isSaveConversationLocal() && !isLogonVisitor() ) {
        const eb::bigint sSaveDbToAccount = fileInfo->m_sSendTo;
        tstring fromName;
        if ( !pEbCallInfo->isGroupCall() ) {
            fromName = userNameByUserId(pEbCallInfo->fromUserId()).toStdString();
        }
        else {
            m_ebum.EB_GetMemberNameByUserId(pEbCallInfo->groupId(),fileInfo->m_sSendFrom,fromName);
        }
        m_sqliteUser->escape_string_in(fromName);
        tstring sInFileName(fileInfo->m_sFileName.toStdString());
        m_sqliteUser->escape_string_in(sInFileName);
        char sSql[1024];
        sprintf( sSql, "INSERT INTO msg_record_t(msg_id,dep_code,from_uid,from_name,to_uid,private,msg_type,msg_text) "
                       "VALUES(%lld,%lld,%lld,'%s',%lld,%d,%d,'%s')",
                 fileInfo->m_nMsgId,
                 pEbCallInfo->groupId(),
                 fileInfo->m_sSendFrom,
                 fromName.c_str(),
                 sSaveDbToAccount,
                 (int)(fileInfo->m_bPrivate?1:0),
                 (int)MRT_FILE,
                 sInFileName.c_str());
        m_sqliteUser->execute(sSql);
    }
    /// *有可能文件名称另存为其他文件
    const QString richText = getFileText(fileInfo);
    emit fileReceived(QString::number(pEbCallInfo->groupId()),
                      QString::number(pEbCallInfo->fromUserId()),
                      QString::number(fileInfo->m_nMsgId),
                      richText,
                      fileInfo->m_sFileName);

//    if (resourceId>0 && fileInfo->GetCallId()==0) {
//    //if (sResId>0)
//        const bool bIsHttpDownloadFile = resourceId==fileInfo->m_nMsgId?true:false;
//        if (m_dialogFileManager!=0) {
//            m_dialogFileManager->onReceivedFile(fileInfo);
//            if (!bIsHttpDownloadFile && m_dialogFileManager->isEmpty()) {
//                m_dialogFileManager->setVisible(false);
//            }
//        }
//        if (bIsHttpDownloadFile) {
//            tstring sInFileName(fileInfo->m_sFileName.toStdString());
//            theApp->m_sqliteUser->escape_string_in(sInFileName);
//            char sql[1024];
//            sprintf( sql, "INSERT INTO msg_record_t(msg_id,from_uid,from_name,to_uid,msg_type,msg_text) "
//                          "VALUES(%lld,%lld,'',%lld,%d,'%s')",
//                     fileInfo->m_nMsgId,fileInfo->m_sSendFrom,fileInfo->m_sSendTo,(int)MRT_FILE,sInFileName.c_str());
//            theApp->m_sqliteUser->execute(sql);
//            if (m_dialogFileManager!=0 && m_dialogFileManager->isVisible() && m_dialogFileManager->isEmpty()) {
//                m_dialogFileManager->onClickedButtonRefresh();
//                m_dialogFileManager->onClickedButtonTranedFile();
//                m_dialogFileManager->activateWindow();
//            }
//            return;
//        }
//    }
//    //CEBCCallInfo::pointer pEbCallInfo;
//    //if (!theApp.m_pCallList.find(lpszCallId,pEbCallInfo))
//    //{
//    //	return 1;
//    //}
//    //pEbCallInfo->m_tLastTime = time(0);
//    EbDialogChatBase::pointer chatBase = getDialogChatBase(callId);
//    if (chatBase.get()!=0) {
//        chatBase->onReceivedFile(fileInfo);
//        if (!chatBase->isVisible()) {
//            if (lParam!=10) {
//                QSound::play( ":/wav/msg.wav" );
//            }
//        }
//    }
}

void EbClientApp::onFilePercent(QEvent *e)
{
    const CChatRoomFilePercent *pFilePercent = (const CChatRoomFilePercent*)e;
    const eb::bigint callId = pFilePercent->GetCallId();
    const eb::bigint sResId = pFilePercent->m_sResId;

    EbcCallInfo::pointer pEbCallInfo;
    if (!m_pCallList.find(callId, pEbCallInfo)) {
        return;
    }
    pEbCallInfo->m_tLastTime = time(0);

    EbFileInfo::pointer ebFileInfo;
    if (!pEbCallInfo->m_progressFiles.find(pFilePercent->m_nMsgId, ebFileInfo)) {
        return;
    }

    char status[128];
    if (pFilePercent->m_percent>100) {
        /// ** 这是传输字节
        const double fReceivedBytes = pFilePercent->m_percent;
        if (fReceivedBytes >= const_gb_size)
            sprintf( status, "%.02fGB",(double)fReceivedBytes/const_gb_size);
        else if (fReceivedBytes >= const_mb_size)
            sprintf( status, "%.02fMB",(double)fReceivedBytes/const_mb_size);
        else if (fReceivedBytes >= const_kb_size)
            sprintf( status, "%.02fKB",(double)fReceivedBytes/const_kb_size);
        else
            sprintf( status, "%dByte",(int)fReceivedBytes);
    }
    else if (pFilePercent->m_nCurSpeed>1024000.0) {
        sprintf( status, "%.2f%% %.2fMB/S", pFilePercent->m_percent, pFilePercent->m_nCurSpeed/(1024.0*1024.0));
    }
    else {
        sprintf( status, "%.2f%% %.2fKB/S", pFilePercent->m_percent, pFilePercent->m_nCurSpeed/1024.0);
    }
    /// ==0.0，没有进度，表示未知文件大小
    int value = 0;
    if (pFilePercent->m_percent==0.0) {
    }
    else {
        value = pFilePercent->m_percent;
        ebFileInfo->setProgressValue(value);
    }
    QString richDesc(status);
    emit filePercent(QString::number(pEbCallInfo->groupId()),
                     QString::number(pEbCallInfo->fromUserId()),
                     QString::number(pFilePercent->m_nMsgId),
                     value,
                     richDesc);

//    if (sResId>0 && sCallId==0) {
//    //if (sResId>0)
//        if (m_dialogFileManager!=0) {
//            m_dialogFileManager->onFilePercent(filePercent);
//        }
//        if (sResId==filePercent->m_nMsgId) {
////            delete pChatRoomFilePercent;
//            return;
//        }
//    }
//    EbDialogChatBase::pointer chatBase = getDialogChatBase(sCallId);
//    if (chatBase.get()!=0) {
//        chatBase->onFilePercent(filePercent);
//    }
}

void EbClientApp::onSave2Cloud(QEvent *e)
{
//    const CCrFileInfo *fileInfo = (const CCrFileInfo*)e;
//    const EB_STATE_CODE stateCode = (EB_STATE_CODE)fileInfo->GetStateCode();
//    if (EB_STATE_APPID_KEY_ERROR==stateCode || EB_STATE_APP_ONLINE_KEY_TIMEOUT==stateCode) {
//        return;
//    }
//    const eb::bigint sCallId = fileInfo->GetCallId();
//    EbDialogChatBase::pointer chatBase = getDialogChatBase(sCallId);
//    if (chatBase.get()!=0) {
//        chatBase->onSave2Cloud(fileInfo);
//        if ( !chatBase->isVisible() ) {
//            QSound::play( ":/wav/msg.wav" );
//        }
//    }

}


void EbClientApp::onCallConnected(QEvent *e)
{
    const EB_CallInfo *pConnectInfo = (const EB_CallInfo*)e;
    const int nConnectFlag = (int)pConnectInfo->GetEventParameter();
    const eb::bigint sCallId = pConnectInfo->GetCallId();
    EB_AccountInfo pFromAccountInfo;
    if ( !m_ebum.EB_GetCallAccountInfo(sCallId,pConnectInfo->GetFromUserId(),&pFromAccountInfo) ) {
        return; // 1;
    }
//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();

    bool bNewCall = false;
    const bool bOwnerCall = (nConnectFlag&EB_CONNECTED_OWNER_CALL)==EB_CONNECTED_OWNER_CALL;
    if (pConnectInfo->m_sGroupCode==0 || pConnectInfo->m_sGroupCode==sCallId) {
        /// ?? || !pCallInfo->m_sChatId.empty())
        const bool bOffLineUser = (nConnectFlag&EB_CONNECTED_OFFLINE_USER)==EB_CONNECTED_OFFLINE_USER;
        const bool bConnectMsg = (nConnectFlag&EB_CONNECTED_MSG)==EB_CONNECTED_MSG;
        bool bNewCall = false;
        EbcCallInfo::pointer pEbCallInfo;
        if ( !m_pCallList.find(sCallId,pEbCallInfo) ) {
            bNewCall = true;
            pEbCallInfo = EbcCallInfo::create();
            pEbCallInfo->m_bOffLineUser = bOffLineUser;
            pEbCallInfo->m_pCallInfo = pConnectInfo;
            pEbCallInfo->m_pFromAccountInfo = pFromAccountInfo;
            m_pCallList.insert(sCallId,pEbCallInfo);
        }
        else {
            pEbCallInfo->m_bOffLineUser = bOffLineUser;
            pEbCallInfo->m_tLastTime = time(0);
        }
        const int nSettingValue = m_ebum.EB_GetMyAccountSetting();
        /// 对方离线转在线，本端自动呼叫，不需要处理界面
        if ( bOwnerCall || bConnectMsg ||
                ( (bOffLineUser&&pConnectInfo->m_sGroupCode==0) &&
                 (nConnectFlag&EB_CONNECTED_AUTO_ACK)!=EB_CONNECTED_AUTO_ACK ) ) {
            /// 本方发起会话，离线会话
            /// 自动打开聊天界面
            EbQuickCallInfo *quickCallInfo = new EbQuickCallInfo(pEbCallInfo.get());
            emit receivedCallConnected(quickCallInfo);
            const bool bAutoCall = (pConnectInfo->m_sGroupCode>0)?
                        m_pAutoCallGroupIdList.exist(pConnectInfo->m_sGroupCode):
                        m_pAutoCallFromUserIdList.exist(pConnectInfo->GetFromUserId());
            const bool bShow = bOwnerCall;
//            EbDialogChatBase::pointer pDlgDialog = getDialogChatBase(pEbCallInfo,bShow,bOwnerCall);
//            if (bOwnerCall && !bAutoCall) {
////                /// **关闭聊天提示窗口2
////                CDlgFuncWindow *pOldFuncWindow = NULL;
////                if (pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
////                    theApp.m_pTempGroupMsg.find(pEbCallInfo->m_pCallInfo.m_sGroupCode,pOldFuncWindow,true);
////                else
////                    theApp.m_pTempFromUserMsg.find(pEbCallInfo->m_pFromAccountInfo.GetUserId(),pOldFuncWindow,true);
////                /// **必须放在前面
////                if (pOldFuncWindow!=NULL)
////                {
////                    pOldFuncWindow->ShowWindow(SW_HIDE);	// *必须隐藏不让用户点击；
////                    pOldFuncWindow->m_pEbCallInfo.reset();	// *必须设为空，否则会清掉后面窗口
////                    theApp.m_pCloseWnd.add(pOldFuncWindow);
////                }

////                if (m_pDlgFrameList!=0) {
////                    if (m_pDlgFrameList->showMinimized(); IsIconic())
////                        m_pDlgFrameList->ShowWindow(SW_RESTORE);
////                    m_pDlgFrameList->SetForegroundWindow();
////                }
//            }

            if (bAutoCall) {
                char sql[256];
                if (pConnectInfo->m_sGroupCode>0)
                    sprintf(sql,"SELECT count(msg_id) FROM msg_record_t WHERE dep_code=%lld AND (read_flag&1)=0",pConnectInfo->m_sGroupCode);
                else
                    sprintf(sql,"SELECT count(msg_id) FROM msg_record_t WHERE from_uid=%lld AND dep_code=0 AND (read_flag&1)=0",pConnectInfo->GetFromUserId());
                int nCookie = 0;
                m_sqliteUser->select(sql,nCookie);
                cgcValueInfo::pointer pRecord = m_sqliteUser->first(nCookie);
                if (pRecord.get()!=0) {
                    const int nUnReadMsgCount = pRecord->getVector()[0]->getIntValue();
                    m_sqliteUser->reset(nCookie);

//                    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//                        if (m_pDlgFrameList!=0) {
//                            m_pDlgFrameList->setUnreadMsg(sCallId,nUnReadMsgCount);
//                        }
//                    }
//                    else {
////                        this->SetUnreadMsg(sCallId,nUnReadMsgCount);
//                    }
                }
            }
        }

        /// 保存会话记录
        if (bNewCall && !m_ebum.EB_IsLogonVisitor()) {
            const bool bSendSessionInfo = true;
//            const bool bSendSessionInfo = bOwnerCall;
            saveCallRecord(sCallId
                           ,pConnectInfo->m_sGroupCode,
                           pEbCallInfo->m_pFromAccountInfo,
                           bSendSessionInfo);
        }
    }
    if (pConnectInfo->m_sOldCallId>0) {
        m_pCallList.remove(pConnectInfo->m_sOldCallId);
    }
    if (bOwnerCall && m_forwardMsgId>0 && (m_forwardTime+10)>time(0) &&
            ((m_forwardGroupId>0 && m_forwardGroupId==pConnectInfo->m_sGroupCode ) ||
             (m_forwardGroupId==0 && pConnectInfo->m_sGroupCode==0 && m_forwardUserId==pConnectInfo->GetFromUserId()) ||
            (m_forwardGroupId==0 && pConnectInfo->m_sGroupCode==0 && m_forwardAccount==QString::fromStdString(pConnectInfo->GetFromAccount().string())))
            ) {
        m_forwardTime = 0;
//        if (bNewCall) {
            m_forwardCallId = pConnectInfo->GetCallId();
            QTimer::singleShot(1000, this, SLOT(onForwardMessage()));
//        }
//        else {
//            this->forwardMessage(pConnectInfo->GetCallId(), m_forwardMsgId);
//            m_forwardMsgId = 0;
//        }
    }

////    DeleteDlgIncomingCall(pConnectInfo->GetFromUserId());
////    DeleteDlgAlertingCall(pConnectInfo->GetFromUserId());


//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//        /// *跑下面
//    }
//    else if (!theApp->isHideMainFrame()) {
////        if (!CFrameWndInfoProxy::ExistCallIdWnd(sCallId)) {
////            // 没有打开聊天，设置为退出标识；
////            theApp->m_ebum.EB_CallExit(sCallId);
////        }
//        return;// 0;
//    }
//    if (m_pDlgFrameList==NULL || !m_pDlgFrameList->existFrameItem(sCallId)) {
//        /// 没有打开聊天，设置为退出标识；
//        theApp->m_ebum.EB_CallExit(sCallId);
//    }
}

void EbClientApp::onCallError(QEvent *e)
{
//    const EB_CallInfo *callInfo = (EB_CallInfo*)e;
////    sndPlaySound(NULL,SND_NODEFAULT);
//    const EB_STATE_CODE stateCode = (EB_STATE_CODE)callInfo->GetEventParameter();
//    if (EB_STATE_APPID_KEY_ERROR==stateCode ||
//        EB_STATE_APP_ONLINE_KEY_TIMEOUT==stateCode) {
//        return; // 0;
//    }

//    const eb::bigint callId = callInfo->GetCallId();
//    EbMessageBox::IMAGE_TYPE imageType = EbMessageBox::IMAGE_WARNING;
//    QString errorText;
////    DeleteDlgAlertingCall(pCallInfo->GetFromUserId());
//    switch (stateCode)
//    {
//    case EB_STATE_ACCOUNT_NOT_EXIST:
//        /// 用户：\r\n%s 不存在！
//        errorText = m_locales->getLocalText("on-call-response.account-not-exist.text","Account Not Exist");
//        break;
//    case EB_STATE_USER_OFFLINE:
//        /// 用户：\r\n%s 离线状态！
//        errorText = m_locales->getLocalText("on-call-response.user-offline.text","User Offline");
//        break;
//    case EB_STATE_UNAUTH_ERROR:
//        /// 帐号：\r\n%s 未通过验证！
//        errorText = m_locales->getLocalText("on-call-response.unauth-error.text","Unauth Error");
//        break;
//    case EB_STATE_ACCOUNT_FREEZE:
//        /// 帐号：\r\n%s 已被临时冻结！
//        errorText = m_locales->getLocalText("on-call-response.account-freeze.text","Account Freeze");
//        break;
//    case EB_STATE_MAX_RETRY_ERROR:
//        /// 错误次数太频繁：\r\n帐号已被临时锁住，请稍候再试！
//        errorText = m_locales->getLocalText("on-call-response.max-retry-error.text","Max Retry Error");
//        break;
//    case EB_STATE_USER_BUSY:
//        /// 用户：\r\n%s 忙！
////        DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
//        errorText = m_locales->getLocalText("on-call-response.user-busy.text","User Busy");
//        break;
//    case EB_STATE_TIMEOUT_ERROR:
//        /// 用户：\r\n%s 会话超时！
//        /// 本地太久未响应，也会有该事件
////        DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
//        errorText = m_locales->getLocalText("on-call-response.timeout-error.text","Timeout Error");
//        break;
//    case EB_STATE_USER_HANGUP:
//        /// 用户：\r\n%s 取消会话！
////        DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
//        errorText = m_locales->getLocalText("on-call-response.user-hangup.text","User Hangup");
//        break;
//    case EB_STATE_ACCOUNT_DISABLE_OFFCALL:
//        /// 用户：\r\n%s 离线，对方屏蔽接收离线消息功能！
//        errorText = m_locales->getLocalText("on-call-response.account-disable-offcall.text","Account Disable Offcall");
//        break;
//    case EB_STATE_ACCOUNT_DISABLE_EXTCALL:
//        /// 用户：\r\n%s 屏蔽外部聊天功能！
//        errorText = m_locales->getLocalText("on-call-response.account-disable-extcall.text","Account Disable Extcall");
//        break;
//    case EB_STATE_OAUTH_FORWARD:
//        /// 操作成功，等待对方：\r\n%s 通过验证！
//        errorText = m_locales->getLocalText("on-call-response.oauth-forward.text","Oauth Forward");
//        imageType = EbMessageBox::IMAGE_INFORMATION;
//        break;
//    default:
//        /// 请求会话：\r\n%s 失败，错误代码：%d
//        errorText = m_locales->getLocalText("on-call-response.other-error.text","Call Error");
//        break;
//    }

//    EB_AccountInfo fromAccountInfo;
//    if (theApp->m_ebum.EB_GetCallAccountInfo(callId,callInfo->GetFromUserId(),&fromAccountInfo)) {
//        saveCallRecord(callId,callInfo->m_sGroupCode,fromAccountInfo);
//    }

//    errorText.replace( "[USER_ACCOUNT]", callInfo->GetFromAccount().c_str() );
//    errorText.replace( "[USER_ID]", QString::number((qlonglong)callInfo->GetFromUserId()) );
//    errorText.replace( "[STATE_CODE]", QString::number((int)stateCode) );
//    EbMessageBox::doShow( NULL, "", QChar::Null, errorText, imageType,default_warning_auto_close );
}

void EbClientApp::onCallHangup(QEvent *e)
{
////    sndPlaySound(NULL,SND_NODEFAULT);
//    const EB_CallInfo *pCallInfo = (EB_CallInfo*)e;
//    const bool bOwner = (bool)(pCallInfo->GetEventParameter()==1);

//    bool bRemoveCall = bOwner;
//    EbDialogChatBase::pointer pDlgDialog = getDialogChatBase(pCallInfo->GetCallId(), bRemoveCall);
//    if (pDlgDialog.get()!=0) {
//        if (!bRemoveCall) {
//            pDlgDialog->onUserExitRoom(pCallInfo->GetFromUserId(),false);
//        }
//    }
//    if (bOwner ||
//        pDlgDialog.get()==0) {		// 本地已经关闭聊天界面
//        theApp->m_pCallList.remove(pCallInfo->GetCallId());
//    }
////    DeleteDlgAlertingCall(pCallInfo->GetFromUserId());
////    DeleteDlgIncomingCall(pCallInfo->GetFromUserId());
}

void EbClientApp::onCallAlerting(QEvent *e)
{
//    const EB_CallInfo *pCallInfo = (EB_CallInfo*)e;
//    const int const_dlg_width = 380;
//    const int const_dlg_height = 262;
//    const QRect& screenRect = theApp->screenRect();

//    const eb::bigint sCallId = pCallInfo->GetCallId();
//    const eb::bigint nFromUserId = pCallInfo->GetFromUserId();
//    const tstring sFromAccount = pCallInfo->GetFromAccount();

//    EB_AccountInfo pFromAccountInfo;
//    if (!theApp->m_ebum.EB_GetCallAccountInfo(sCallId,nFromUserId,&pFromAccountInfo)) return;   // 1;

//    EbDialogChatBase::pointer pDlgDialog = getDialogChatBase(sCallId);
//    if (pDlgDialog.get()!=0) {
//        pDlgDialog->onAlertingCall();
//    }
////    else if (!m_pAlertingCallList.exist(pCallInfo->GetFromUserId())) {
////        CDlgAlertingCall::pointer pDlgAlertingCall = CDlgAlertingCall::create(this);
////        pDlgAlertingCall->m_pFromAccountInfo = pFromAccountInfo;
////        pDlgAlertingCall->m_sCallId = sCallId;
////        pDlgAlertingCall->Create(CDlgAlertingCall::IDD, this);
////        CRect rectDlgIncomingCall;
////        pDlgAlertingCall->GetWindowRect(&rectDlgIncomingCall);
////        pDlgAlertingCall->MoveWindow(0, nScreenHeight-const_dlg_height, const_dlg_width, const_dlg_height);
////        pDlgAlertingCall->ShowWindow(SW_SHOW);
////        //pDlgAlertingCall->SetActiveWindow();
////        pDlgAlertingCall->SetForegroundWindow();
////        m_pAlertingCallList.insert(pCallInfo->GetFromUserId(), pDlgAlertingCall);
////    }
}

void EbClientApp::onCallIncoming(QEvent *e)
{
//    const EB_CallInfo *pCallInfo = (EB_CallInfo*)e;
////    QSound::play( ":/wav/incomingcall.wav" );
////    CString sSoundFile;
////    sSoundFile.Format(_T("%s/wav/incomingcall.wav"), theApp.GetAppDataPath());
////    sndPlaySound(sSoundFile, SND_ASYNC|SND_NOWAIT|SND_LOOP);
//    const int const_dlg_width = 380;
//    const int const_dlg_height = 262;
//    const QRect& screenRect = theApp->screenRect();

//    const EB_AccountInfo *pFromAccount = (const EB_AccountInfo*)pCallInfo->GetEventData();

//    const eb::bigint sCallId = pCallInfo->GetCallId();
//    //EbcCallInfo::pointer pCallInfo = theEBAppClient.GetCallInfo(sCallId);;
//    //if (pCallInfo.get() == NULL) return 1;

////    CDlgIncomingCall::pointer pDlgIncomingCall = CDlgIncomingCall::create(this);
////    pDlgIncomingCall->m_pFromAccountInfo.operator =(pFromAccount);
////    pDlgIncomingCall->m_pCallInfo = pCallInfo;
////    pDlgIncomingCall->Create(CDlgIncomingCall::IDD, this);
////    pDlgIncomingCall->MoveWindow(nScreenWidth-const_dlg_width, nScreenHeight-const_dlg_height, const_dlg_width, const_dlg_height);
////    pDlgIncomingCall->SetCircle();
////    pDlgIncomingCall->ShowWindow(SW_SHOW);
////    //pDlgIncomingCall->SetActiveWindow();
////    pDlgIncomingCall->SetForegroundWindow();
//    //    m_pIncomingCallList.insert(pCallInfo->GetFromUserId(), pDlgIncomingCall);
}


void EbClientApp::onUGInfo(QEvent *e)
{
    const EB_UGInfo *pUGInfo = (const EB_UGInfo*)e;
    EbQuickUGInfo * quickUGInfo = new EbQuickUGInfo(pUGInfo);
    quickUGInfo->setGroupFullName(ugFullName(quickUGInfo->ugId(),
                                             quickUGInfo->groupName()));
    emit receivedUGInfo(quickUGInfo);
}

void EbClientApp::onUGDelete(QEvent *e)
{
    const EB_UGInfo *pUGInfo = (const EB_UGInfo*)e;
    emit ugInfoDeleted(new EbQuickUGInfo(pUGInfo));
//    if (m_widgetMyContact!=0) {
//        m_widgetMyContact->onUGDelete(pUGInfo);
//    }
}

void EbClientApp::onContactDelete(QEvent *e)
{
    const EB_ContactInfo *contactInfo = (const EB_ContactInfo*)e;
    emit contactDeleted(new EbQuickContactInfo(contactInfo));

//    if (m_widgetMyContact!=0) {
//        m_widgetMyContact->onContactDelete(contactInfo);
//    }
}

void EbClientApp::onContactInfo(QEvent *e)
{
    const EB_ContactInfo *contactInfo = (const EB_ContactInfo*)e;
    emit receivedContactInfo(new EbQuickContactInfo(contactInfo));

    if (contactInfo->m_nContactUserId>0 &&
            !contactInfo->m_sHeadResourceFile.isEmpty() &&
            QFile::exists(contactInfo->m_sHeadResourceFile)) {
        emit receivedUserHeadImage(QString::number(contactInfo->m_nContactUserId),
                                   theFileHead+contactInfo->m_sHeadResourceFile);
    }
}

void EbClientApp::onContactStateChanged(QEvent *e)
{
    const EB_ContactInfo *contactInfo = (const EB_ContactInfo*)e;
    emit contactLineStateChanged(new EbQuickContactInfo(contactInfo));

//    if (m_widgetMyContact!=0) {
//        m_widgetMyContact->onContactInfo(contactInfo, true);
//    }
//    if (m_widgetMySession!=0) {
//        m_widgetMySession->onContactStateChanged(contactInfo);
//    }
//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//        /// **跑下面
//    }
//    else if (!theApp->isHideMainFrame()) {
////        CFrameWndInfoProxy::UserLineStateChange(0, contactInfo->m_nContactUserId, contactInfo->m_nLineState);
////        return 0;
//    }
//    if (m_pDlgFrameList!=0) {
//        m_pDlgFrameList->onUserLineStateChange(0, contactInfo->m_nContactUserId, contactInfo->m_nLineState);
//    }
}
void EbClientApp::onAcceptAddContact(QEvent *e)
{
    const EB_ContactInfo *contactInfo = (const EB_ContactInfo*)e;
//    if (m_widgetMyContact!=0) {
//        m_widgetMyContact->onContactInfo(contactInfo, false);
//    }

    if (this->groupMsgSubId()>0) {
        /// <font color=\"#6c6c6c\">用户：%s<br/>添加好友成功</font>
        QString text = m_locales->getLocalText("pop-tip-dialog.accept-add-contact","User accept add contact");
        text.replace("[CONTACT_NAME]", contactInfo->m_sName.c_str());
        emit receivedBcMsg("0",
                           QString::number(this->groupMsgSubId()),
                           text,
                           "",
                           "tab_type=sys_msg");
    }
}

void EbClientApp::onRejectAddContact(QEvent *e)
{
    const EB_AccountInfo *pRequestAccountInfo = (const EB_AccountInfo*)e;
    const EB_APMsgInfo *pApMsgInfo = (const EB_APMsgInfo*)pRequestAccountInfo->GetEventData();
    const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
    const CEBString sDescription(pApMsgInfo->m_sMsgContent);
    const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;

    char sSql[1024];
    /// 删除旧会话
    sprintf(sSql, "DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0",
            nGroupId,
            pRequestAccountInfo->GetUserId());
    m_sqliteUser->execute(sSql);

    /// emp_code=-1,from_type=msg-type
    const int nMsgType = EBC_MSG_TYPE_REJECT_ADD_CONTACT;
    sprintf(sSql, "INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) "
                  "VALUES(%lld,%lld,-1,%lld,'%s',%d)",
            pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
    m_sqliteUser->execute(sSql);

    EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
    sessionInfo->setCallId(pApMsgInfo->m_nMsgId);
    const tstring sCallTime = pApMsgInfo->m_sMsgTime;
    time_t nCallTime = time(0);
    ChangeTime(sCallTime.c_str(),nCallTime);
    sessionInfo->setTime(nCallTime);
    sessionInfo->setMemberId(-1);
    sessionInfo->setFromUserId(pRequestAccountInfo->GetUserId());
    sessionInfo->setFromAccount(pRequestAccountInfo->GetAccount().c_str());
    sessionInfo->setFromType(nMsgType);
    sessionInfo->setRead(false);
    sendSessionInfo(sessionInfo);

    if (this->groupMsgSubId()>0) {
        /// <font color=\"#6c6c6c\">用户：%s<br/>拒绝加为好友<br/>附加消息：%s</font>
        QString text = m_locales->getLocalText("pop-tip-dialog.reject-add-contact","User Reject add contact");
        text.replace("[USER_ACCOUNT]", sRequestAccount.c_str());
        text.replace("[DESCRIPTION]", sDescription.c_str());
        emit receivedBcMsg(QString::number(pApMsgInfo->m_nMsgId),
                           QString::number(this->groupMsgSubId()),
                           text,
                           "",
                           "tab_type=sys_msg");
    }
}

void EbClientApp::onRequestAddContact(QEvent *e)
{
    const EB_AccountInfo *pRequestAccountInfo = (const EB_AccountInfo*)e;
    const EB_APMsgInfo *pApMsgInfo = (const EB_APMsgInfo*)pRequestAccountInfo->GetEventData();
    const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
    const CEBString sDescription(pApMsgInfo->m_sMsgContent);
    const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;

    char sSql[1024];
    /// 删除旧会话
    sprintf(sSql, "DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0",
            nGroupId,
            pRequestAccountInfo->GetUserId());
    m_sqliteUser->execute(sSql);

    /// emp_code=-1,from_type=msg-type
    const int nMsgType = EBC_MSG_TYPE_REQUEST_ADD_CONTACT;
    sprintf(sSql, "INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) "
                  "VALUES(%lld,%lld,-1,%lld,'%s',%d)",
            pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
    m_sqliteUser->execute(sSql);

    EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
    sessionInfo->setCallId(pApMsgInfo->m_nMsgId);
    const tstring sCallTime = pApMsgInfo->m_sMsgTime;
    time_t nCallTime = time(0);
    ChangeTime(sCallTime.c_str(),nCallTime);
    sessionInfo->setTime(nCallTime);
    sessionInfo->setMemberId(-1);
    sessionInfo->setFromUserId(pRequestAccountInfo->GetUserId());
    sessionInfo->setFromAccount(pRequestAccountInfo->GetAccount().c_str());
    sessionInfo->setFromType(nMsgType);
    sessionInfo->setRead(false);
    sendSessionInfo(sessionInfo);

    if (this->groupMsgSubId()>0) {
        /// <font color=\"#6c6c6c\">用户：%s<br/>申请加为好友<br/>附加消息：%s</font>
        QString text = m_locales->getLocalText("pop-tip-dialog.request-add-contact","User Reqeust add contact");
        text.replace("[USER_ACCOUNT]", sRequestAccount.c_str());
        text.replace("[DESCRIPTION]", sDescription.c_str());
        emit receivedBcMsg(QString::number(pApMsgInfo->m_nMsgId),
                           QString::number(this->groupMsgSubId()),
                           text,
                           "",
                           "tab_type=sys_msg");
    }
}

void EbClientApp::onEditInfoResponse(QEvent *e)
{
    const EB_Event *pEvent = (EB_Event*)e;
    const EB_STATE_CODE nState = (EB_STATE_CODE)pEvent->GetEventParameter();
    if (EB_STATE_APPID_KEY_ERROR==nState || EB_STATE_APP_ONLINE_KEY_TIMEOUT==nState) {
        return;
    }
//    if (nState==EB_STATE_OK) {
//        const tstring userName = this->m_ebum.EB_GetUserName();
//        if (m_myAccountInfo.GetUserName()!=userName) {
//            m_myAccountInfo.SetUserName(userName);
//            emit myUserNameChanged();
//        }

//    }
    QString text;
    /// nFlag 1=修改密码返回
    const int nEditInfoFlag = (int)pEvent->GetEventBigParameter();
    if (nState==EB_STATE_ACC_PWD_ERROR) {
        /// 密码错误：\r\n请重新输入！
        text = m_locales->getLocalText("on-editinfo-response.acc-pwd-error.text","Password Error");
        text.replace( "[STATE_CODE]", QString::number((int)nState) );
        if (nEditInfoFlag==1) {
            emit setPasswordResponse(nState, text);
        }
//        const QString title = m_locales->getLocalText("on-editinfo-response.acc-pwd-error.title","");
//        EbMessageBox::doShow( NULL, title, QChar::Null, text, EbMessageBox::IMAGE_INFORMATION,default_warning_auto_close );
        return;
    }
    else if (nEditInfoFlag==1) {
        /// ** *修改密码
        if (nState==EB_STATE_OK) {
            char sSql[256];
            sprintf(sSql,"update user_login_record_t set password='' where user_id=%lld", logonUserId());
            m_sqliteEbc->execute(sSql);
            /// 密码修改成功：\r\n请重新登录。
            const QString text = m_locales->getLocalText("on-editinfo-response.modify-password-ok.text","Edit Password Ok");
            emit setPasswordResponse(nState, text);
//            QString title = m_locales->getLocalText("on-editinfo-response.modify-password-ok.title","");
//            if (title.isEmpty())
//                title = productName();
//            EbMessageBox::doExec( NULL, title, QChar::Null, text, EbMessageBox::IMAGE_INFORMATION,0,QMessageBox::Ok );
//            onTriggeredActionLogout();
            return;
        }
        else {
            /// 密码修改失败，请重试！<br>错误代码: [STATE_CODE]
            text = m_locales->getLocalText("on-editinfo-response.modify-password-error.text","Edit Password Error");
            text.replace( "[STATE_CODE]", QString::number((int)nState) );
            emit setPasswordResponse(nState, text);
//            const QString title = m_locales->getLocalText("on-editinfo-response.modify-password-error.title","");
//            EbMessageBox::doShow( NULL, title, QChar::Null, text, EbMessageBox::IMAGE_INFORMATION,default_warning_auto_close );
            return;
        }
    }
    else if (nState==EB_STATE_OK) {
        /// 个人资料修改成功！
        text = m_locales->getLocalText("on-editinfo-response.state-ok.text","Edit Info Ok");
//        const QString title = m_locales->getLocalText("on-editinfo-response.state-ok.title","");
//        EbMessageBox::doShow( NULL, title, QChar::Null, text, EbMessageBox::IMAGE_INFORMATION,default_warning_auto_close );
    }
    else {
        /// 个人资料修改失败，请重试！<br>错误代码: %d
        text = m_locales->getLocalText("on-editinfo-response.other-error.text","Edit Info Error");
        text.replace( "[STATE_CODE]", QString::number((int)nState) );
//        const QString title = m_locales->getLocalText("on-editinfo-response.other-error.title","");
//        EbMessageBox::doShow( NULL, title, QChar::Null, text, EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
    }
//    ui->lineEditUserDescription->setText( theApp->m_ebum.EB_GetDescription().c_str() );
//    changeTrayTooltip();
    emit setAccountInfoResponse(nState, text);
}

void EbClientApp::onMemberEditResponse(QEvent *e)
{
    const EB_MemberInfo *memberInfo = (const EB_MemberInfo*)e;
    const EB_STATE_CODE stateCode = (EB_STATE_CODE)memberInfo->GetEventParameter();
    QString text;
    switch (stateCode) {
    case EB_STATE_OK: {
        /// 操作成功
        const bool bNewMemberInfo = memberInfo->GetEventBigParameter()==1?true:false;
        if (bNewMemberInfo)
            text = m_locales->getLocalText("on-member-edit-response.new-state-ok.text","");
        else
            text = m_locales->getLocalText("on-member-edit-response.edit-state-ok.text","");
        break;
    }
    case EB_STATE_OAUTH_FORWARD: {
        /// 邀请成员成功：\r\n等待对方通过验证！
        text = m_locales->getLocalText("on-member-edit-response.oauth-forward.text","Oauth Forward");
        break;
    }
    case EB_STATE_NOT_AUTH_ERROR: {
        /// 没有权限：\r\n请联系管理员！
        text = m_locales->getLocalText("on-member-edit-response.not-auth-error.text","Not Auth Error");
//        if (libEbc::IsFullNumber(pMemberInfo->m_sMemberAccount.c_str(),pMemberInfo->m_sMemberAccount.size()))
//            CDlgMessageBox::EbMessageBox(this,_T(""),_T("不支持全数字帐号注册：\r\n请更换帐号后重试！"),CDlgMessageBox::IMAGE_ERROR,5);
//        else
//            CDlgMessageBox::EbMessageBox(this,_T(""),_T("没有权限：\r\n请联系管理员！"),CDlgMessageBox::IMAGE_ERROR,5);
        break;
    }
    case EB_STATE_ACCOUNT_ALREADY_EXIST:
        /// 帐号已经存在：\r\n请重新输入！
        text = m_locales->getLocalText("on-member-edit-response.account-already-exist.text","Account Already Exist");
        break;
    case EB_STATE_ANOTHER_ENT_ACCOUNT:
        /// 其他企业员工帐号：<br>请重新输入！
        text = m_locales->getLocalText("on-member-edit-response.another-ent-account.text","Another Enterprise Account");
        break;
    default: {
        /// 操作失败,请重试或联系公司客服：<br>错误代码: [STATE_CODE]
        text = m_locales->getLocalText("on-member-edit-response.other-error.text","Other Error");
        break;
    }
    }
    if (!text.isEmpty()) {
        text.replace("[STATE_CODE]",QString::number((int)stateCode));
//        EbMessageBox::doShow( NULL, "", QChar::Null, text, imageType,default_warning_auto_close );
    }
    EbQuickMemberInfo *quickMemberInfo = new EbQuickMemberInfo(memberInfo);
    quickMemberInfo->setSubType(memberSubType(memberInfo));
    emit memberEditResponse(quickMemberInfo, stateCode, text);
}

void EbClientApp::onMemberDelete(QEvent *e)
{
    /// ***统一在 EB_WM_REMOVE_GROUP 消息处理
}

void EbClientApp::setWindowTitleAndTrayInfo(void)
{
////    EB_USER_LINE_STATE nOutLineState = theApp->m_ebum.EB_GetLineState();
//    const CEBString sUserName = theApp->m_ebum.EB_GetUserName();
//    const CEBString sSettingEnterprise = theApp->m_setting.GetEnterprise();
//    QString sWindowText;
//    if (theApp->isLogonVisitor())
//        sWindowText = QString("%1-%2").arg(m_locales->getLocalText("name-text.visitor","Visitor")).arg(sUserName.c_str());
//    else if (!theApp->productName().isEmpty())
//        sWindowText = QString("%1-%2(%3)").arg(theApp->productName()).arg(sUserName.c_str()).arg(theApp->logonUserAccount().c_str());
//    else
//        sWindowText = QString("%1-%2(%3)").arg(sSettingEnterprise.c_str()).arg(sUserName.c_str()).arg(theApp->logonUserAccount().c_str());
//    this->setWindowTitle(sWindowText);
//    changeTrayTooltip();
}

void EbClientApp::onMemberInfo(QEvent *e)
{
    const EB_MemberInfo *memberInfo = (EB_MemberInfo*)e;
    const bool bIsMyDefaultMember = (memberInfo->GetEventParameter()==1)?true:false;
    const bool bIsMyGroupMember = (memberInfo->GetEventBigParameter()==1)?true:false;

    emit receivedMemberInfo(new EbQuickMemberInfo(memberInfo));
    checkMyHeadImageChange(memberInfo);

    if (memberInfo->m_nMemberUserId!=logonUserId() &&
            !memberInfo->m_sHeadResourceFile.isEmpty() &&
            QFile::exists(memberInfo->m_sHeadResourceFile)) {
        emit receivedUserHeadImage(QString::number(memberInfo->m_nMemberUserId),
                                   theFileHead+memberInfo->m_sHeadResourceFile);
    }

//    if (bIsMyGroupMember) {
//        emit receivedMyGroupMemberInfo(new EbQuickMemberInfo(memberInfo), false);
//    }

//    if (bIsMyDefaultMember) {
//        QString sSettingEnterprise;
//        if (!theApp->productName().isEmpty())
//            sSettingEnterprise = theApp->productName();
//        else
//            sSettingEnterprise = theApp->m_setting.GetEnterprise().c_str();
//        QString sWindowText;
//        tstring sEnterpriseName;
//        if (theApp->m_ebum.EB_GetEnterpriseName(sEnterpriseName)) {
//            sWindowText = QString("%1-%2(%3) %4").arg(sSettingEnterprise).arg(memberInfo->m_sUserName.c_str())
//                    .arg(memberInfo->m_sMemberAccount.c_str()).arg(sEnterpriseName.c_str());
//        }
//        else {
//            sWindowText = QString("%1-%2(%3)").arg(sSettingEnterprise).arg(memberInfo->m_sUserName.c_str())
//                    .arg(memberInfo->m_sMemberAccount.c_str());
//        }
//        this->setWindowTitle(sWindowText);
//    }
//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//        /// **跑下面
//    }
//    else if (!theApp->isHideMainFrame()) {
////        CFrameWndInfoProxy::OnUserEmpInfo(pMemberInfo,true);
////        return 0;
//    }
//    if (m_pDlgFrameList!=0) {
//        m_pDlgFrameList->onMemberInfo(memberInfo,false);
//    }

}
void EbClientApp::onRejectAdd2Group(QEvent *e)
{
    /// 默认未实现
}

void EbClientApp::onInviteAdd2Group(QEvent *e)
{
    const EB_AccountInfo *pRequestAccountInfo = (const EB_AccountInfo*)e;
    const EB_APMsgInfo *pApMsgInfo = (const EB_APMsgInfo*)pRequestAccountInfo->GetEventData();
    const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
    const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;
    const CEBString sDescription(pApMsgInfo->m_sMsgContent);

    char sSql[1024];
    /// 删除旧会话
    sprintf(sSql, "DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0",
            nGroupId,pRequestAccountInfo->GetUserId());
    m_sqliteUser->execute(sSql);

    /// emp_code=-1,from_type=msg-type
    const int nMsgType = EBC_MSG_TYPE_INVITE_JOIN_2_GROUP;
    sprintf(sSql, "INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) "
                  "VALUES(%lld,%lld,-1,%lld,'%s',%d)",
            pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
    m_sqliteUser->execute(sSql);

    EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
    sessionInfo->setCallId(pApMsgInfo->m_nMsgId);
    const tstring sCallTime = pApMsgInfo->m_sMsgTime;
    time_t nCallTime = time(0);
    ChangeTime(sCallTime.c_str(),nCallTime);
    sessionInfo->setTime(nCallTime);
    sessionInfo->setMemberId(-1);
    sessionInfo->setGroupId(nGroupId);
    sessionInfo->setFromUserId(pRequestAccountInfo->GetUserId());
    sessionInfo->setFromAccount(pRequestAccountInfo->GetAccount().c_str());
    sessionInfo->setFromType(nMsgType);
    sessionInfo->setRead(false);
    sendSessionInfo(sessionInfo);

    if (this->groupMsgSubId()>0) {
        QString sGroupName;
        tstring sMsgContent(sDescription.c_str());
        const std::string::size_type nFind = sMsgContent.find(";");
        if (nFind!=std::string::npos) {
            sGroupName = QString::fromStdString(sMsgContent.substr(0,nFind));
            sMsgContent = sMsgContent.substr(nFind+1);
        }
        if (sGroupName.isEmpty()) {
            sGroupName = QString("%1").arg(nGroupId);
        }
        /// <font color=\"#6c6c6c\">用户：%s<br/>邀请你加入群组：%s<br/>附加消息：%s</font>
        QString text = m_locales->getLocalText("pop-tip-dialog.request-add-to-group","User Reqeust add to group");
        text.replace("[USER_ACCOUNT]", sRequestAccount.c_str());
        text.replace("[GROUP_NAME]", sGroupName);
        text.replace("[DESCRIPTION]", sDescription.c_str());
        emit receivedBcMsg(QString::number(pApMsgInfo->m_nMsgId),
                           QString::number(this->groupMsgSubId()),\
                           text,
                           "",
                           "tab_type=sys_msg");
    }
}

void EbClientApp::onRequestAdd2Group(QEvent *e)
{
    const EB_AccountInfo *pRequestAccountInfo = (const EB_AccountInfo*)e;
    const EB_APMsgInfo *pApMsgInfo = (const EB_APMsgInfo*)pRequestAccountInfo->GetEventData();
    const CEBString sRequestAccount(pRequestAccountInfo->GetAccount());
    const CEBString sDescription(pApMsgInfo->m_sMsgContent);
    const eb::bigint nGroupId = pApMsgInfo->m_nGroupId;

    char sSql[1024];
    /// 删除旧会话
    sprintf(sSql, "DELETE FROM call_record_t WHERE dep_code=%lld AND from_uid=%lld AND emp_code=-1 AND from_type>0",
            nGroupId,pRequestAccountInfo->GetUserId());
    m_sqliteUser->execute(sSql);

    /// emp_code=-1,from_type=msg-type
    const int nMsgType = EBC_MSG_TYPE_REQUEST_JOIN_2_GROUP;
    sprintf(sSql, "INSERT INTO call_record_t(call_id,dep_code,emp_code,from_uid,from_account,from_type) "
                  "VALUES(%lld,%lld,-1,%lld,'%s',%d)",
            pApMsgInfo->m_nMsgId,nGroupId,pRequestAccountInfo->GetUserId(),sRequestAccount.c_str(),nMsgType);
    m_sqliteUser->execute(sSql);

    EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
    sessionInfo->setCallId(pApMsgInfo->m_nMsgId);
    const tstring sCallTime = pApMsgInfo->m_sMsgTime;
    time_t nCallTime = time(0);
    ChangeTime(sCallTime.c_str(),nCallTime);
    sessionInfo->setTime(nCallTime);
    sessionInfo->setMemberId(-1);
    sessionInfo->setGroupId(nGroupId);
    sessionInfo->setFromUserId(pRequestAccountInfo->GetUserId());
    sessionInfo->setFromAccount(pRequestAccountInfo->GetAccount().c_str());
    sessionInfo->setFromType(nMsgType);
    sessionInfo->setRead(false);
    sendSessionInfo(sessionInfo);

    if (this->groupMsgSubId()>0) {
        /// <font color=\"#6c6c6c\">用户：%s<br/>申请加入群组：%lld<br/>附加消息：%s</font>
        QString text = m_locales->getLocalText("pop-tip-dialog.request-add-to-group","User Reqeust add to group");
        text.replace("[USER_ACCOUNT]", sRequestAccount.c_str());
        text.replace("[GROUP_ID]", QString::number(nGroupId));
        text.replace("[DESCRIPTION]", sDescription.c_str());
        emit receivedBcMsg(QString::number(pApMsgInfo->m_nMsgId),
                           QString::number(this->groupMsgSubId()),
                           text,
                           "",
                           "tab_type=sys_msg");
    }
}

void EbClientApp::onExitGroup(QEvent *e)
{
    const EB_GroupInfo *groupInfo = (const EB_GroupInfo*)e;
    const EB_MemberInfo *memberInfo = (const EB_MemberInfo*)groupInfo->GetEventData();
    const eb::bigint sExitUserId(memberInfo->m_nMemberUserId);

    /// 删除本地群组聊天记录
    deleteDbRecord(groupInfo->m_sGroupCode,false);
    /// 找到现在会话，移除用户数据
    if (sExitUserId == logonUserId()) {
        /// 本端退出群组
        emit deletedGroupId(QString::number(groupInfo->m_sGroupCode));
//        deleteMyGroupInfoModelData(groupInfo->m_sGroupCode);

//        theApp->m_pCallList.remove(groupInfo->m_sGroupCode);

//        if (m_widgetMyEnterprise!=0) {
//            m_widgetMyEnterprise->deleteMemberInfo(groupInfo,memberInfo->m_sMemberCode,false);
////            m_widgetMyEnterprise->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
//        }
//        if (m_widgetMyGroup!=0) {
//            m_widgetMyGroup->onRemoveGroup(groupInfo);
////            m_widgetMyGroup->deleteGroupInfo(pGroupInfo);
//        }
    }
    else {

//        if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=0) {
//            m_pDlgFrameList->onRemoveMember( groupInfo, memberInfo->m_sMemberCode, sExitUserId );
//        }
//        else if (!theApp->isHideMainFrame())
//         {
////            CFrameWndInfoProxy::OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
//        }
//        else if (m_pDlgFrameList!=0) {
//            m_pDlgFrameList->onRemoveMember( groupInfo, memberInfo->m_sMemberCode, sExitUserId );
//        }


//        if (m_widgetMyEnterprise!=0) {
//            m_widgetMyEnterprise->deleteMemberInfo(groupInfo,memberInfo->m_sMemberCode,false);
////            m_widgetMyEnterprise->DeleteEmployeeInfo(pGroupInfo,pMemberInfo->m_sMemberCode);
//        }
//        if (m_widgetMyGroup != 0) {// && theEBAppClient.EB_IsMyGroup(pMemberInfo->m_sGroupCode))
//            m_widgetMyGroup->deleteMemberInfo( groupInfo,memberInfo->m_sMemberCode,sExitUserId );
//        }
    }
    emit memberExitGroup(new EbQuickGroupInfo(groupInfo), new EbQuickMemberInfo(memberInfo));

    if ( groupInfo->m_nCreateUserId==logonUserId() ) {
        QString text = m_locales->getLocalText("message-show.member-exit-group","Member Exit Group");
        text.replace( "[USER_NAME]", memberInfo->m_sUserName.c_str() );
        text.replace( "[USER_ACCOUNT]", memberInfo->m_sMemberAccount.c_str() );
        text.replace( "[GROUP_NAME]", groupInfo->m_sGroupName.c_str() );
        text.replace( "[GROUP_ID]", QString::number(groupInfo->m_sGroupCode) );
        emit receivedBcMsg("0", "0", text, "", "");
    }
}
void EbClientApp::onRemoveGroup(QEvent *e)
{
    const EB_GroupInfo *groupInfo = (const EB_GroupInfo*)e;
    const EB_MemberInfo *memberInfo = (const EB_MemberInfo*)groupInfo->GetEventData();

    /// 删除本地群组聊天记录
    deleteDbRecord(groupInfo->m_sGroupCode,false);

//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
    const eb::bigint sRemoveUserId(memberInfo->m_nMemberUserId);
    /// 找到现在会话，移除用户数据
    if (sRemoveUserId == logonUserId()) {
        /// 本人被移出群组
//        theApp->m_pCallList.remove(groupInfo->m_sGroupCode);
//        if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=0) {
//            m_pDlgFrameList->onRemoveGroup(groupInfo);
//        }
//        else if (!theApp->isHideMainFrame()) {
////            CFrameWndInfoProxy::OnRemoveGroup(pGroupInfo->m_sGroupCode);
//        }
//        else if (m_pDlgFrameList!=0) {
//            m_pDlgFrameList->onRemoveGroup(groupInfo);
//        }

////        EbDialogChatBase::pointer pDlgDialog = getDialogChatBase(pGroupInfo->m_sGroupCode,true);
//        if (m_widgetMyGroup!=0) {
//            m_widgetMyGroup->onRemoveGroup(groupInfo);
//        }

        if (groupInfo->m_nMyEmpId>0) {
            deletedGroupId(QString::number(groupInfo->m_sGroupCode));
//            deleteMyGroupInfoModelData(groupInfo->m_sGroupCode);
        }

        /// 你被管理员移除出：\r\n%s
        QString text = m_locales->getLocalText("message-show.remove-from-group","Remove From Group");
        text.replace( "[GROUP_NAME]", groupInfo->m_sGroupName.c_str() );
        text.replace( "[GROUP_ID]", QString::number(groupInfo->m_sGroupCode) );
        emit receivedBcMsg("0", "0", text, "", "");
    }
    else {
//        if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=0) {
//            m_pDlgFrameList->onRemoveMember( groupInfo,pMemberInfo->m_sMemberCode,sRemoveUserId );
//        }
//        else if (!theApp->isHideMainFrame()) {
////            CFrameWndInfoProxy::OnRemoveMember(pGroupInfo->m_sGroupCode, pMemberInfo->m_sMemberCode);
//        }
//        else if (m_pDlgFrameList!=0) {
//            m_pDlgFrameList->onRemoveMember(groupInfo, pMemberInfo->m_sMemberCode, sRemoveUserId);
//        }

//        if (m_widgetMyGroup != 0) { /// && theEBAppClient.EB_IsMyGroup(pMemberInfo->m_sGroupCode))
//            m_widgetMyGroup->deleteMemberInfo(groupInfo,pMemberInfo->m_sMemberCode,false);
//        }

    }
    emit memberRemoveGroup(new EbQuickGroupInfo(groupInfo), new EbQuickMemberInfo(memberInfo));

//    if (groupInfo->m_sEnterpriseCode>0 && m_widgetMyEnterprise!=0) {
//        /// *不能填 true
//        m_widgetMyEnterprise->deleteMemberInfo(groupInfo,pMemberInfo->m_sMemberCode,false);
//    }
}
void EbClientApp::onGroupEditResponse(QEvent *e)
{
    const EB_GroupInfo *groupInfo = (const EB_GroupInfo*)e;
    const EB_STATE_CODE stateCode = (EB_STATE_CODE)groupInfo->GetEventParameter();
    QString text;
    switch (stateCode) {
    case EB_STATE_OK: {
        /// 操作成功
        const bool bNewGroup = groupInfo->GetEventBigParameter()==1?true:false;
        if (bNewGroup)
            text = m_locales->getLocalText("on-group-edit-response.new-state-ok.text","");
        else
            text = m_locales->getLocalText("on-group-edit-response.edit-state-ok.text","");
        break;
    }
    case EB_STATE_NOT_AUTH_ERROR: {
        /// 没有权限：\r\n请联系管理员！
        text = m_locales->getLocalText("on-group-edit-response.not-auth-error.text","Not Auth Error");
        break;
    }
    default: {
        /// 操作失败,请重试或联系公司客服：<br>错误代码: [STATE_CODE]
        text = m_locales->getLocalText("on-group-edit-response.other-error.text","Other Error");
        break;
    }
    }
    if (!text.isEmpty()) {
        text.replace( "[GROUP_NAME]", groupInfo->m_sGroupName.c_str() );
        text.replace( "[GROUP_ID]", QString::number(groupInfo->m_sGroupCode) );
        text.replace( "[STATE_CODE]", QString::number((int)stateCode) );
    }
    emit groupEditResponse(new EbQuickGroupInfo(groupInfo), (int)stateCode, text);
}
void EbClientApp::onGroupDelete(QEvent *e)
{
    const EB_GroupInfo *groupInfo = (const EB_GroupInfo*)e;
    const eb::bigint groupId = groupInfo->m_sGroupCode;
//    bool bIsMyDepartment = (pGroupInfo->GetEventParameter()==1)?true:false;
//    if (groupInfo->m_nMyEmpId>0) {
//        deleteMyGroupInfoModelData(groupId);
//    }

//    if (m_widgetMyEnterprise!=0) {
//        m_widgetMyEnterprise->onRemoveGroup(groupInfo);
////        m_widgetMyEnterprise->DeleteDepartmentInfo(pGroupInfo->m_sGroupCode);
//    }
//    if (m_widgetMyGroup != 0) {
//        m_widgetMyGroup->onRemoveGroup(groupInfo);
////        m_widgetMyGroup->DeleteDepartmentInfo(pGroupInfo);
//    }

//    /// 关闭现在会话窗口
//    theApp->m_pCallList.remove(groupInfo->m_sGroupCode);

//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT && m_pDlgFrameList!=0) {
//        m_pDlgFrameList->onRemoveGroup(groupInfo);
//    }
//    else if (!theApp->isHideMainFrame()) {
////        CFrameWndInfoProxy::OnRemoveGroup(pGroupInfo->m_sGroupCode);
//    }
//    else if (m_pDlgFrameList!=0) {
//        m_pDlgFrameList->onRemoveGroup(groupInfo);
//    }

//    getDialogChatBase(groupInfo->m_sGroupCode, true);

    /// %s\r\n已经被解散！
    emit deletedGroupId(QString::number(groupId));
    QString text = m_locales->getLocalText("message-show.delete-group","Delete Group");
    text.replace( "[GROUP_NAME]", groupInfo->m_sGroupName.c_str() );
    text.replace( "[GROUP_ID]", QString::number(groupInfo->m_sGroupCode) );
    emit deletedGroupInfo(new EbQuickGroupInfo(groupInfo), text);
    deleteDbRecord(groupId,false);
}

//void EbClientApp::deleteMyGroupInfoModelData(qint64 groupId)
//{
//    AUTO_CONST_WLOCK(m_myGroupInfoDataList);
//    CLockList<EbSharedGroupInfo::pointer>::const_iterator iter = m_myGroupInfoDataList.begin();
//    int index = 0;
//    for (; iter!=m_myGroupInfoDataList.end(); iter++) {
//        const EbSharedGroupInfo::pointer &quickGroupInfo = *iter;
//        if (quickGroupInfo->m_sGroupCode==groupId) {
//            m_myGroupInfoDataList.erase(iter);
//            wtLock.unlock();
//            emit deletedMyGroupInfo(groupId, index);
//            return;
//        }
//        index++;
//    }
//}

//void EbClientApp::setMyGroupInfoModelData(const EB_GroupInfo *groupInfo)
//{
//    AUTO_CONST_WLOCK(m_myGroupInfoDataList);
//    CLockList<EbSharedGroupInfo::pointer>::const_iterator iter = m_myGroupInfoDataList.begin();
//    int index = 0;
//    for (; iter!=m_myGroupInfoDataList.end(); iter++) {
//        const EbSharedGroupInfo::pointer &quickGroupInfo = *iter;
//        if (quickGroupInfo->m_sGroupCode==groupInfo->m_sGroupCode) {
//            int moveToIndex = -1;
//            /// 修改数据才需要，暂时不处理
////            EbSharedGroupInfo::pointer moveToGroupInfo;
////            {
////                /// 查找后面的数据，有没有类型相同 ，需要移位的数据
////                CLockList<EbSharedGroupInfo::pointer>::const_iterator iter2 = m_myGroupInfoDataList.begin();
////                int index2 = 0;
////                for (; iter2!=m_myGroupInfoDataList.end(); iter2++) {
////                    const EbSharedGroupInfo::pointer &quickGroupInfoTemp = *iter2;
////                    index2++;
////                    if (quickGroupInfoTemp->m_nGroupType<groupInfo->m_nGroupType ||
////                            quickGroupInfoTemp->m_sGroupCode==groupInfo->m_sGroupCode) {
////                        continue;
////                    }
////                    else if (quickGroupInfoTemp->m_nGroupType>groupInfo->m_nGroupType) {
////                        break;
////                    }
////                    else if (groupInfo->m_sGroupName!=quickGroupInfoTemp->m_sGroupName) {
////                        /// 找到需要移位到前面数据
////                        moveToIndex = index2-1;
////                        moveToGroupInfo = quickGroupInfoTemp;
////                        break;
////                    }
////                }
////            }
////            if (moveToIndex>=0) {
////                /// 交换一个数据，实现移位
////                EB_GroupInfo temp(groupInfo);
////                quickGroupInfo->operator =(moveToGroupInfo.get());
////                moveToGroupInfo->operator =(&temp);
////                emit receivedMyGroupInfo(new EbQuickGroupInfo(groupInfo), false, index, moveToIndex);
////            }
////            else {
////                quickGroupInfo->operator =(groupInfo);
////                emit receivedMyGroupInfo(new EbQuickGroupInfo(groupInfo), false, index, moveToIndex);
////            }
////            const int moveToIndex = -1;//getMyGroupInfoModelMoveToIndex(groupInfo);
//            wtLock.unlock();
//            emit receivedMyGroupInfo(new EbQuickGroupInfo(groupInfo), false, index, moveToIndex);
//            return;
//        }
//        else if (groupInfo->m_nGroupType<quickGroupInfo->m_nGroupType) {
//            /// 部门类型排前面
//            m_myGroupInfoDataList.insert(iter, EbSharedGroupInfo::create(groupInfo));
//            emit receivedMyGroupInfo(new EbQuickGroupInfo(groupInfo), true, index, -1);
//            {
//                /// 查找后面删除存在数据
//                for (; iter!=m_myGroupInfoDataList.end(); iter++) {
//                    const EbSharedGroupInfo::pointer &quickGroupInfoTemp = *iter;
//                    if (quickGroupInfoTemp->m_sGroupCode==groupInfo->m_sGroupCode) {
//                        m_myGroupInfoDataList.erase(iter);
//                        break;
//                    }
//                }
//            }
//            return;
//        }
//        else if (groupInfo->m_nGroupType==quickGroupInfo->m_nGroupType &&
//                 groupInfo->m_sGroupName<quickGroupInfo->m_sGroupName) {
//            /// 部门名称小排前面
//            m_myGroupInfoDataList.insert(iter, EbSharedGroupInfo::create(groupInfo));
//            emit receivedMyGroupInfo(new EbQuickGroupInfo(groupInfo), true, index, -1);
//            {
//                /// 查找后面删除存在数据
//                for (; iter!=m_myGroupInfoDataList.end(); iter++) {
//                    const EbSharedGroupInfo::pointer &quickGroupInfoTemp = *iter;
//                    if (quickGroupInfoTemp->m_sGroupCode==groupInfo->m_sGroupCode) {
//                        m_myGroupInfoDataList.erase(iter);
//                        break;
//                    }
//                }
//            }
//            return;
//        }
//        index++;
//    }
//    m_myGroupInfoDataList.add(EbSharedGroupInfo::create(groupInfo), false);
//    emit receivedMyGroupInfo(new EbQuickGroupInfo(groupInfo), true, -1, -1);
//}

void EbClientApp::onGroupInfo(QEvent *e)
{
    const EB_GroupInfo *groupInfo = (EB_GroupInfo*)e;

    if (groupInfo->m_sParentCode>0) {
        emit receivedSubGroupInfo(new EbQuickGroupInfo(groupInfo),
                                  QString::number(groupInfo->m_sParentCode),
                                  0);
    }
    EbQuickGroupInfo *quickGroupInfo = new EbQuickGroupInfo(groupInfo);
    const QString fullGroupName = this->groupFullName(quickGroupInfo->groupId(),
                                                      quickGroupInfo->groupName(),
                                                      quickGroupInfo->m_nEmpCount);
    quickGroupInfo->setGroupFullName(fullGroupName);
    emit receivedGroupInfo(quickGroupInfo);

    if (m_tWaitCall2Group>0 &&
            (m_tWaitCall2Group+10)>time(0)) {
        /// call2group 返回
        m_tWaitCall2Group = 0;
        emit requestUrlCallGroup(QString::number(groupInfo->m_sGroupCode));
    }

//    if (groupInfo->m_nMyEmpId>0) {
//        setMyGroupInfoModelData(groupInfo);
//    }

//    const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp->defaultUIStyleType();
//    if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT) {
//        /// **跑下面
//    }
//    else if ( !theApp->isHideMainFrame() ) {
////        CFrameWndInfoProxy::ChangeDepartmentInfo(pGroupInfo);
//        return;
//    }

//    if (m_pDlgFrameList!=0) {
//        m_pDlgFrameList->onGroupInfo(groupInfo);
//    }
}

void EbClientApp::onEnterpriseInfo(QEvent *e)
{
    const EB_EnterpriseInfo *pEvent = (EB_EnterpriseInfo*)e;
    setEnterpriseCreateUserId(pEvent->m_nCreateUserId);
    emit receivedEnterpriseInfo(new EbQuickEnterpriseInfo(pEvent));
//    changeTrayTooltip();
}

void EbClientApp::onLogonSuccess(QEvent *e)
{
    if (m_timerIdCheckState==0) {
        /// 十秒检查一次状态
        m_timerIdCheckState = this->startTimer(10*1000);
    }
    const EB_AccountInfo * pAccountInfo = (EB_AccountInfo*)e;
//    m_myAccountInfo.operator =(pAccountInfo);
    if (m_sqliteEbc.get()!=0) {
        if (!is_visitor_uid(pAccountInfo->GetUserId())) {
            const EB_USER_LINE_STATE m_nOutLineState = EB_LINE_STATE_ONLINE_NEW;
            const std::string sAccount = m_sRequestAccount.toStdString();
            m_sRequestAccount.clear();
            char sql[512];
            tstring sSavePassword;
            {
                /// 获取之前保存密码
                int nCookie = 0;
                sprintf(sql, "SELECT password FROM user_login_record_t where account='%s' OR account_r='%s' OR user_id=%lld LIMIT 1",
                        pAccountInfo->GetAccount().c_str(),
                        pAccountInfo->GetAccount().c_str(),
                        pAccountInfo->GetUserId());
                m_sqliteEbc->select(sql, nCookie);
                cgcValueInfo::pointer pRecord = m_sqliteEbc->first(nCookie);
                if (pRecord.get()!=0) {
                    sSavePassword = pRecord->getVector()[0]->getStr();
                    m_sqliteEbc->reset(nCookie);
                }
            }
            sprintf(sql, "delete from user_login_record_t where account='%s' OR account_r='%s' OR user_id=%lld",
                    pAccountInfo->GetAccount().c_str(), pAccountInfo->GetAccount().c_str(), pAccountInfo->GetUserId());
            int ret = (int)m_sqliteEbc->execute(sql);
            const bool savePwd = true;
            if (savePwd) {
                if (ebServerVersion()==0) {
                    /// ** 兼容旧版本服务端
                    const std::string sPassword = m_sRequestPassword.toStdString();
                    m_sRequestPassword.clear();
                    sprintf(sql, "insert into user_login_record_t(account,account_r,password,safepwd,linestate,user_id,phone) VALUES('%s','%s','%s',1,%d,%lld,%lld)",
                            sAccount.c_str(),pAccountInfo->GetAccount().c_str(),sPassword.c_str(),(int)m_nOutLineState,pAccountInfo->GetUserId(),pAccountInfo->GetPhone());
                }
                else {
                    mycp::tstring sLocalHostOAuthKey(sSavePassword);
                    if (sLocalHostOAuthKey.empty()) {
                        entboost::GetLocalHostOAuthKey(sLocalHostOAuthKey, true);
                    }
                    sprintf(sql, "insert into user_login_record_t(account,account_r,password,safepwd,linestate,user_id,phone) VALUES('%s','%s','%s',1,%d,%lld,%lld)",
                            sAccount.c_str(),
                            pAccountInfo->GetAccount().c_str(),
                            sLocalHostOAuthKey.c_str(),
                            m_nOutLineState,
                            pAccountInfo->GetUserId(),
                            pAccountInfo->GetPhone());
                }
            }
            else {
                sprintf(sql, "insert into user_login_record_t(account,account_r,password,linestate,user_id,phone) VALUES('%s','%s','',%d,%lld,%lld)",
                        sAccount.c_str(),pAccountInfo->GetAccount().c_str(),(int)m_nOutLineState,pAccountInfo->GetUserId(),pAccountInfo->GetPhone());
            }
            ret = (int)m_sqliteEbc->execute(sql);
            if (ret<1) {
                emit loginError(1, QString::fromStdString(m_sqliteEbc->lastErrorString()));
                return;
            }
        }
    }
    else {
        emit loginError(1, "sqliteEbc is null");
        return;
    }
    if (!EbClientAppBase::onLogonSuccess()) {
        m_errorCode = EB_STATE_ERROR;
        m_errorString = m_locales->getLocalText("on-logon-response.other-error.text", "Logon Error");
        m_errorString.replace( "[STATE_CODE]", QString::number((int)m_errorCode) );
        emit loginError(m_errorCode, m_errorString);
        return;
    }

    emit loginSuccess(new EbQuickAccountInfo(pAccountInfo));
}

void EbClientApp::onLogonError(QEvent *e)
{
    const EB_AccountInfo * pAccountInfo = (EB_AccountInfo*)e;
    m_errorCode = (EB_STATE_CODE)pAccountInfo->GetEventParameter();

    if (m_errorCode == EB_STATE_APPID_KEY_ERROR) {
//        QString title = m_locales->getLocalText("on-logon-response.appid-key-error.title", "");
//        if (title.isEmpty())
//            title = theApp->productName();
        m_errorString = m_locales->getLocalText("on-logon-response.appid-key-error.text", "AppId Key Error");
        exitApp(true);
        initApp();
        setDevAppId();
        emit loginError(m_errorCode, m_errorString);
        return;
    }

    switch (m_errorCode) {
    case EB_WM_LOGON_TIMEOUT:
        /// 登录超时，请重新登录！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.logon-timeout.text", "Logon Timeout");
        break;
    case EB_STATE_UNAUTH_ERROR:
        /// 帐号未激活，请查收邮件，完成注册！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.unauth-error.text", "Unauth Error");
        break;
    case EB_STATE_ACCOUNT_FREEZE:
        /// 帐号已经被临时冻结，请联系公司客服！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.account-freeze.text", "Account Freeze");
        break;
    case EB_STATE_MAX_RETRY_ERROR:
        /// 错误次数太多，帐号被临时锁住，请稍候再试！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.max-retry-error.text", "Max Retry Error");
        break;
    case EB_STATE_NOT_AUTH_ERROR:
        /// 没有权限错误！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.not-auth-error.text", "Not Auth Error");
        break;
    case EB_STATE_ACCOUNT_NOT_EXIST:
        /// 帐号不存在，请重新输入！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.account-not-exist.text", "Account Not Exist");
        break;
    case EB_STATE_ACC_PWD_ERROR:
        /// 帐号或密码错误，请重新输入！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.acc-pwd-error.text", "Account Or Password Error");
        break;
    default:
        /// 登录失败，请重试！\r\n错误代码:%d
        m_errorString = m_locales->getLocalText("on-logon-response.other-error.text", "Logon Error");
        break;
    }
    m_errorString.replace( "[STATE_CODE]", QString::number((int)m_errorCode) );
//    const QString server = m_ebum.EB_GetAddress().c_str();
//    emit loginError(m_errorCode, server+";"+m_errorString);
    emit loginError(m_errorCode, m_errorString);
}

void EbClientApp::onOnlineAnother(QEvent *e)
{
    const EB_Event *pEvent = (EB_Event*)e;
    const int nOnlineAnotherType = (int)pEvent->GetEventParameter();
    /// 0=已经在其他地方登录，退出当前连接；
    /// 1=修改密码，退出当前连接；
    emit logonAnother(nOnlineAnotherType);
}

void EbClientApp::setTimeString(EbQuickSessionInfo *sessionInfo)
{
    /// 获取日期字符串
    const QDateTime pNow = QDateTime::currentDateTime();
    const QDate nowDate = pNow.date();
    const QDateTime pTime = QDateTime::fromTime_t(sessionInfo->time());
    const QDate timeDate = pTime.date();
    QString timeString;
    if (nowDate.year()==timeDate.year() && nowDate.month()==timeDate.month() && nowDate.day()==timeDate.day()) {
        /// today
        timeString = pTime.toString("HH:mm");
    }
    else if (nowDate.year()==timeDate.year() && nowDate.month()==timeDate.month() && nowDate.day()==timeDate.day()+1)
    {
        /// 昨天 %H:%M
        timeString = QString("%1 %2").arg( m_locales->getLocalText("name-text.yesterday","Yesterday")).arg(pTime.toString("HH:mm"));
    }
    else if (nowDate.year()==timeDate.year()) {
        /// 今年
        timeString = pTime.toString("MM-dd");
    }
    else {
        timeString = pTime.toString("yyyy-MM-dd");
    }
    sessionInfo->setTimeString(timeString);
}

QString EbClientApp::getSessionLastInfo(eb::bigint groupId, eb::bigint fromUserId)
{
    char sql[512];
    if (groupId>0) {
        sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t "
                      "WHERE dep_code=%lld ORDER BY msg_time DESC LIMIT 1",
                 groupId);
    }
    else if (fromUserId>0) {
        sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag FROM msg_record_t "
                      "WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time DESC LIMIT 1",
                 fromUserId, fromUserId);
    }
    else {
        return "";
    }

    QString lastInfo;
    int nCookie = 0;
    const mycp::bigint nRet = m_sqliteUser->select(sql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->last(nCookie);
    if (pRecord.get()!=0) {
        const tstring sMsgTime(pRecord->getVector()[0]->getStrValue());
        const eb::bigint msgId = pRecord->getVector()[1]->getBigIntValue();
//        const tstring soffTime(pRecord->getVector()[2]->getStrValue());
        const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
        const tstring sFromName(pRecord->getVector()[4]->getStrValue());
        const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
        const tstring sToName(pRecord->getVector()[6]->getStrValue());
        const int nPrivate = pRecord->getVector()[7]->getIntValue();
        const int nMsgType = pRecord->getVector()[8]->getIntValue();
        const tstring sMsgName(pRecord->getVector()[9]->getStrValue() );
        const tstring sMsgText(pRecord->getVector()[10]->getStrValue() );
        //theApp.m_pBoUsers->escape_string_out(sMsgName);
        //theApp.m_pBoUsers->escape_string_out(sMsgText);
        const unsigned int nMsgSize = sMsgText.size();
        const int nReadFlag = pRecord->getVector()[11]->getIntValue();
        pRecord = m_sqliteUser->previous(nCookie);

        if ((nReadFlag&EBC_READ_FLAG_WITHDRAW)==EBC_READ_FLAG_WITHDRAW) {
            /// [撤回一条消息]
            const QString text = m_locales->getLocalText("withdraw-msg-response.state-ok-update","");
            const QString newText = QString("<font color=#808080>%1</font>").arg(text);
            if (groupId==0 || sFromAccount==this->logonUserId())
                lastInfo = newText;
            else
                lastInfo = QString("%1:%2").arg(sFromName.c_str()).arg(newText);
        }
        else {
            switch (nMsgType) {
            case MRT_UNKNOWN: {
                break;
            }
            case MRT_TEXT: {
                /// 先临时保存，后面显示；处理分段保存长文本；
                if (groupId==0 ||
                        sFromAccount==this->logonUserId() ||
                        sFromName.empty()) {
                    lastInfo = sMsgText;
                }
                else {
                    lastInfo = QString("%1:%2")
                            .arg(sFromName.c_str()).arg(sMsgText.c_str());
                }
                break;
            }
            case MRT_JPG: {
                /// 图片/表情
                eb::bigint resourceId = 0;
                tstring resourceDesc;
                std::vector<tstring> pList;
                const tstring &sResourceInfo = sMsgText;
                if (ParseString(sResourceInfo.c_str(),";",pList)>=3) {
                    resourceId = cgc_atoi64(pList[0].c_str());
                    resourceDesc = pList.size()>3?pList[3]:"";
                }
                if (resourceId>0) {
                    if (groupId==0 || sFromAccount==this->logonUserId()) {
                        lastInfo = QString("[%1]")
                                .arg(resourceDesc.c_str());
                    }
                    else {
                        lastInfo = QString("%1:[%2]")
                                .arg(sFromName.c_str())
                                .arg(resourceDesc.c_str());
                    }
                }
                else {
                    if (groupId==0 || sFromAccount==this->logonUserId()) {
                        lastInfo = QString("[%1]")
                                .arg(m_locales->getLocalText("chat-msg-text.image", "Image"));
                    }
                    else {
                        lastInfo = QString("%1:[%2]")
                                .arg(sFromName.c_str())
                                .arg(m_locales->getLocalText("chat-msg-text.image", "Image"));
                    }
                }
                break;
            }
            case MRT_MAP_POS:
                /// 地图位置
                break;
            case MRT_CARD_INFO: {
                /// 名片信息
//                if (groupId==0 || sFromAccount==this->logonUserId()) {
//                    lastInfo = QString("[%1]")
//                            .arg(m_locales->getLocalText("chat-msg-text.user-ecard", "User ECard"));
//                }
//                else {
//                    lastInfo = QString("%1:[%2]")
//                            .arg(sFromName.c_str())
//                            .arg(m_locales->getLocalText("chat-msg-text.user-ecard", "User ECard"));
//                }
                QString sCardMessage;
                QString userName;
                if (writeCardDataMessage(sCardMessage, sMsgText.c_str(), &userName)) {
                    lastInfo = QString("%1[%2]")
                            .arg(userName)
                            .arg(m_locales->getLocalText("chat-msg-text.user-ecard", "User ECard"));
                }
                break;
            }
            case MRT_USER_DATA:
                /// 用户自定义数据
                break;
            case MRT_WAV:
                /// 语音消息
                if (groupId==0 || sFromAccount==this->logonUserId()) {
                    lastInfo = QString("[%1]")
                            .arg(m_locales->getLocalText("chat-msg-text.voice", "Voice"));
                }
                else {
                    lastInfo = QString("%1:[%2]")
                            .arg(sFromName.c_str())
                            .arg(m_locales->getLocalText("chat-msg-text.voice", "Voice"));
                }
//                writeVoiceMessage(sMsgName.c_str());
                break;
            case MRT_RESOURCE: {
                /// 资源文件
                const std::string::size_type nFind = sMsgText.find(",");
                if (nFind == std::string::npos) {
                    break;
                }
//                const QString sFileName = sMsgName.c_str();
//                const mycp::bigint nResourceId = cgc_atoi64(sMsgText.substr(0,nFind).c_str());
//                const mycp::bigint nFileSize = cgc_atoi64(sMsgText.substr(nFind+1).c_str());
                if (groupId==0 || sFromAccount==this->logonUserId()) {
                    lastInfo = QString("[%1]")
                            .arg(sMsgText.c_str());
                }
                else {
                    lastInfo = QString("%1:[%2]")
                            .arg(sFromName.c_str())
                            .arg(sMsgText.c_str());
                }
                //            writeFileMessage(receive, msgId, nResourceId, sFileName, nFileSize);
                break;
            }
            case MRT_FILE: {
                /// 文件
                const tstring fileName = GetFileName(sMsgText);
                const QString sFileName = fileName.c_str();
                if (groupId==0 || sFromAccount==this->logonUserId()) {
                    lastInfo = QString("[%1]%2")
                            .arg(m_locales->getLocalText("chat-msg-text.file", "File"))
                            .arg(sFileName);
                }
                else {
                    lastInfo = QString("%1:[%2]%3")
                            .arg(sFromName.c_str())
                            .arg(m_locales->getLocalText("chat-msg-text.file", "File"))
                            .arg(sFileName);
                }
                //            writeFileMessage(receive, msgId, 0, sFileName, 0);
                break;
            }
            default:
                break;
            }
        }
    }
    m_sqliteUser->reset(nCookie);
    return lastInfo;
}

int EbClientApp::getSessionUnreadCount(eb::bigint groupId, eb::bigint fromUserId)
{
    char sql[256];
    if (groupId>0) {
        sprintf( sql, "select COUNT(dep_code) FROM msg_record_t "
                      "WHERE dep_code=%lld AND from_uid<>%lld AND (read_flag&1)=0",
                 groupId, this->logonUserId());
    }
    else if (fromUserId>0) {
        sprintf( sql, "select COUNT(dep_code) FROM msg_record_t "
                      "WHERE dep_code=0 AND from_uid=%lld AND (read_flag&1)=0",
                 fromUserId);
    }
    else {
        return 0;
    }

    int unreadCount = 0;
    int nCookie = 0;
    m_sqliteUser->select(sql, nCookie);
    cgcValueInfo::pointer pRecord = m_sqliteUser->last(nCookie);
    if (pRecord.get()!=0) {
        unreadCount = pRecord->getVector()[0]->getIntValue();
        m_sqliteUser->reset(nCookie);
    }
    return unreadCount;
}

void EbClientApp::sendSessionInfo(EbQuickSessionInfo *sessionInfo, int parameter)
{
    /// 获取日期字符串
    setTimeString(sessionInfo);
    /// 获取最后消息
//    const QString lastInfo = getSessionLastInfo(sessionInfo->groupId(), sessionInfo->fromUserId());
//    if (lastInfo.isEmpty()) {
//        /// 没有聊天消息，不显示到最近会话
//        return;
//    }
//    sessionInfo->setLastInfo(lastInfo);
    if (!sessionInfo->isMsgSession()) {
        sessionInfo->setLastInfo(getSessionLastInfo(sessionInfo->groupId(), sessionInfo->fromUserId()));
        sessionInfo->setUnreadCount(getSessionUnreadCount(sessionInfo->groupId(), sessionInfo->fromUserId()));
    }
    if (sessionInfo->isGroupCall()) {
        const int memberSize = m_ebum.EB_GetGroupMemberSize(sessionInfo->groupId(), 1);
        if (memberSize>0) {
            sessionInfo->setGroupFullName(
                        QString("%1 <font color=\"#999\">(%2)</font>")
                        .arg(sessionInfo->groupName())
                        .arg(memberSize));
        }
        else {
            sessionInfo->setGroupFullName(sessionInfo->groupName());
        }
    }
    emit loadSessionInfoResponse(sessionInfo, parameter);
}

tstring EbClientApp::getFromAccountName(const EB_AccountInfo &fromAccountInfo) const
{
    CEBString fromName;/// = fromAccountInfo.m_pFromCardInfo.m_sName;
    int nFromType = (int)fromAccountInfo.m_pFromCardInfo.m_nAccountType;
    if (nFromType==(int)EB_ACCOUNT_TYPE_VISITOR)
    {
        fromName = fromAccountInfo.m_pFromCardInfo.m_sAddress;
    }
    else {
//        if (m_ebum.EB_GetContactName2(fromAccountInfo.GetUserId(), fromName)) {
//            /// 优化判断是否"好友联系人"
//        }
//        else
        if ( !fromAccountInfo.m_pFromCardInfo.m_sAccountName.empty() ) {
            fromName = fromAccountInfo.m_pFromCardInfo.m_sAccountName;
        }
        else if ( !fromAccountInfo.GetUserName().empty() ) {
            fromName = fromAccountInfo.GetUserName();
        }
        else if (fromAccountInfo.m_pFromCardInfo.m_sMemberCode>0){
            m_ebum.EB_GetMemberNameByMemberCode(fromAccountInfo.m_pFromCardInfo.m_sMemberCode, fromName);
        }
        else {
            m_ebum.EB_GetMemberNameByUserId2( fromAccountInfo.GetUserId(), fromName );
        }
        if (fromName.empty()) {
            fromName = fromAccountInfo.m_pFromCardInfo.m_sName;
        }
    }
    return fromName;
}

void EbClientApp::saveCallRecord(eb::bigint callId,
                                 eb::bigint groupId,
                                 const EB_AccountInfo &fromAccountInfo,
                                 bool bSendSessionInfo)
{
    eb::bigint memberCode = fromAccountInfo.m_pFromCardInfo.m_sMemberCode;
    CEBString groupName = fromAccountInfo.m_pFromCardInfo.m_sGroupName;
    const CEBString fromName = getFromAccountName(fromAccountInfo);
    const int nFromType = (int)fromAccountInfo.m_pFromCardInfo.m_nAccountType;
//    CEBString fromName;/// = fromAccountInfo.m_pFromCardInfo.m_sName;
//    int nFromType = (int)fromAccountInfo.m_pFromCardInfo.m_nAccountType;
//    if (nFromType==(int)EB_ACCOUNT_TYPE_VISITOR)
//    {
//        fromName = fromAccountInfo.m_pFromCardInfo.m_sAddress;
//    }
//    else {
////        if (m_ebum.EB_GetContactName2(fromAccountInfo.GetUserId(), fromName)) {
////            /// 优化判断是否"好友联系人"
////        }
////        else
//        if ( !fromAccountInfo.m_pFromCardInfo.m_sAccountName.empty() ) {
//            fromName = fromAccountInfo.m_pFromCardInfo.m_sAccountName;
//        }
//        else if ( !fromAccountInfo.GetUserName().empty() ) {
//            fromName = fromAccountInfo.GetUserName();
//        }
//        else if (fromAccountInfo.m_pFromCardInfo.m_sMemberCode>0){
//            m_ebum.EB_GetMemberNameByMemberCode(fromAccountInfo.m_pFromCardInfo.m_sMemberCode, fromName);
//        }
//        else {
//            m_ebum.EB_GetMemberNameByUserId2( fromAccountInfo.GetUserId(), fromName );
//        }
//        if (fromName.empty()) {
//            fromName = fromAccountInfo.m_pFromCardInfo.m_sName;
//        }
//    }
    CEBString sCompany = fromAccountInfo.m_pFromCardInfo.m_sEnterprise;
    CEBString sTitle = fromAccountInfo.m_pFromCardInfo.m_sTitle;
    eb::bigint sPhone = eb_atoi64(fromAccountInfo.m_pFromCardInfo.m_sPhone.c_str());
    CEBString sTel = fromAccountInfo.m_pFromCardInfo.m_sTel;
    CEBString sEmail = fromAccountInfo.m_pFromCardInfo.m_sEmail;
    char sql[2048];
    if (groupId==0) {
//        if (m_ebum.EB_GetMemberNameByMemberCode(fromAccountInfo.m_pFromCardInfo.m_sMemberCode,fromName)) {
            char lpszMemberCode[24];
            sprintf(lpszMemberCode,"%lld", fromAccountInfo.m_pFromCardInfo.m_sMemberCode);
            groupName = lpszMemberCode;
//        }
    }
    else {
        if (!m_ebum.EB_GetGroupName(groupId,groupName)) {
            char lpszBuffer[24];
            sprintf(lpszBuffer,"%lld",groupId);
            groupName = lpszBuffer;
        }
    }
    if (groupId>0 || !is_visitor_uid(fromAccountInfo.GetUserId())) {
        /// 删除旧会话
//        EbCallRecordInfo::pointer pOldCallInfo = m_widgetMySession->callRecordInfo(groupId,fromAccountInfo.GetUserId());
//        if (pOldCallInfo.get()!=0) {
//            sprintf( sql, "DELETE FROM call_record_t WHERE call_id=%lld",pOldCallInfo->m_sCallId);
//            theApp->m_sqliteUser->execute(sql);
//            if (sPhone==0 && pOldCallInfo->m_nFromPhone!=0)
//                sPhone = pOldCallInfo->m_nFromPhone;
//            if (fromName.empty() && !pOldCallInfo->m_sFromName.empty())
//                fromName = pOldCallInfo->m_sFromName;
//            if (sCompany.empty() && !pOldCallInfo->m_sCompany.empty())
//                sCompany = pOldCallInfo->m_sCompany;
//            if (sTitle.empty() && !pOldCallInfo->m_sTitle.empty())
//                sTitle = pOldCallInfo->m_sTitle;
//            if (sTel.empty() && !pOldCallInfo->m_sTel.empty())
//                sTel = pOldCallInfo->m_sTel;
//            if (sEmail.empty() && !pOldCallInfo->m_sEmail.empty())
//                sEmail = pOldCallInfo->m_sEmail;
//        }
//        else
        if (groupId>0) {
            sprintf( sql, "DELETE FROM call_record_t WHERE call_id=%lld", groupId);
            m_sqliteUser->execute(sql);
        }
        else {
            /// emp_code=-1 是验证消息
            sprintf( sql, "DELETE FROM call_record_t WHERE from_uid=%lld AND dep_code=0 AND emp_code>=0",
                     fromAccountInfo.GetUserId());
            m_sqliteUser->execute(sql);
        }
    }

    /// * *必须放前面
    if (bSendSessionInfo) {
        EbQuickSessionInfo *sessionInfo = new EbQuickSessionInfo();
        sessionInfo->setCallId(callId);
        sessionInfo->setGroupId(groupId);
        sessionInfo->setGroupName(groupName.c_str());
        sessionInfo->setMemberId(memberCode);
        sessionInfo->setFromUserId(fromAccountInfo.GetUserId());
        sessionInfo->setFromAccount(fromAccountInfo.GetAccount().c_str());
        sessionInfo->setFromName(fromName.c_str());
        sessionInfo->setCompany(sCompany.c_str());
        sessionInfo->setTitle(sTitle.c_str());
        sessionInfo->setFromPhone(sPhone);
        sessionInfo->setTel(sTel.c_str());
        sessionInfo->setEmail(sEmail.c_str());
        sessionInfo->setTime(time(0));
        sessionInfo->setRead(false);
        sendSessionInfo(sessionInfo);
    }

    /// * *必须放后面
    sprintf( sql, "INSERT INTO call_record_t(call_id,dep_code,dep_name,emp_code,from_uid,from_phone,from_name,"
                  "from_type,company,title,tel,email) "
                  "VALUES(%lld,%lld,'%s',%lld,%lld,%lld,'%s',%d,'%s','%s','%s','%s')",
             callId,groupId,groupName.c_str(),memberCode,fromAccountInfo.GetUserId(),sPhone,
             fromName.c_str(),nFromType,sCompany.c_str(),sTitle.c_str(),sTel.c_str(),sEmail.c_str());
    m_sqliteUser->execute(sql);
}
