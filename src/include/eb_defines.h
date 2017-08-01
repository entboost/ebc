// eb_defines.h file here
#ifndef __eb_defines_h__
#define __eb_defines_h__
#include "eb_define1.h"

const mycp::bigint theMaxUserId		= 0x1000000000000LL;	// =281474976710656(15位)
const mycp::bigint theVisitorStartId = 0x2000000000000LL;	// =562949953421312(15位)
#define is_visitor_uid(id) (id>=theVisitorStartId)
//const int const_max_uid_length = 13;					// UID最长13位；可以达到万亿级别

typedef enum EB_TO_SHARE_TYPE
{
	EB_TO_SHARE_TYPE_UNKNOWN		//
	, EB_TO_SHARE_TYPE_TEMP				// 临时共享（所有人下线自动清除）
}EB_TO_SHARE_TYPE;

typedef enum EB_SHARE_OWNER_TYPE
{
	EB_SHARE_OWNER_TYPE_UNKNOWN
	, EB_SHARE_OWNER_TYPE_USER				// 用户
	, EB_SHARE_OWNER_TYPE_GROUP			// 群组
}EB_SHARE_OWNER_TYPE;
typedef enum EB_SHARE_RESOURCE_TYPE
{
	EB_SHARE_RESOURCE_TYPE_RESOURCE		// 资源 resource_id
	, EB_SHARE_RESOURCE_TYPE_BC_MSG		// 广播消息 msg_id
}EB_SHARE_RESOURCE_TYPE;

typedef enum EB_CONTACT_TYPE
{
	EB_CONTACT_TYPE_COMMON			= 0x0		// 普通联系人
	, EB_CONTACT_TYPE_AUTH			= 0x01	// 通过验证（好友）
	, EB_CONTACT_TYPE_MAIL			= 0x10	// 邮件联系人
	, EB_CONTACT_TYPE_WHITE			= 0x20	// 白名单
	, EB_CONTACT_TYPE_BLACK			= 0x40	// 黑名单
	, EB_CONTACT_TYPE_WASTE			= 0x80	// 垃圾箱名称
}EB_CONTACT_TYPE;

typedef enum EB_MAIL_CONTENT_FLAG
{
	EB_MAIL_CONTENT_FLAG_EDIT_MAIL
	, EB_MAIL_CONTENT_FLAG_SEND_MAIL
	, EB_MAIL_CONTENT_FLAG_RECEIVE_MAIL
	, EB_MAIL_CONTENT_FLAG_BLACK_MAIL
	, EB_MAIL_CONTENT_FLAG_WASTE_MAIL
	, EB_MAIL_CONTENT_FLAG_ARCHIVE_MAIL
	, EB_MAIL_CONTENT_FLAG_DELETE_FLAG1_MAIL	= 21
	, EB_MAIL_CONTENT_FLAG_DELETE_FLAG2_MAIL
}EB_MAIL_CONTENT_FLAG;

//typedef enum EB_NAMELIST_TYPE
//{
//	EB_NAMELIST_TYPE_OTHER_CONTACT	// 普通联系人名单
//	, EB_NAMELIST_TYPE_MAIL_CONTACT	// 邮件联系人名单
//	, EB_NAMELIST_TYPE_WHITE_LIST	// 白名单
//	, EB_NAMELIST_TYPE_BLACK_LIST	// 黑名单
//	, EB_NAMELIST_TYPE_WASTE_LIST	// 垃圾箱名单
//};

typedef enum EB_MEMORY_LOAD_STATE
{
	EB_MEMORY_LOAD_STATE_INIT
	, EB_MEMORY_LOAD_ENT_DATA
	, EB_MEMORY_LOAD_GROUP_DATA
	, EB_MEMORY_LOAD_GROUP_VER_DATA
	, EB_MEMORY_LOAD_GROUP_MEMBER_DATA
	, EB_MEMORY_LOAD_USER_CONTACT_DATA
	, EB_MEMORY_LOAD_UNAUTH_DATA
	, EB_MEMORY_LOAD_OTHER_SERVER_ONLINE_ACCOUNT
	, EB_MEMORY_LOAD_FINISHED
}EB_MEMORY_LOAD_STATE;

#define EB_MAKELONG(lo, hi)    ((long)(((unsigned short)(((unsigned long)(lo)) & 0xffff)) | ((unsigned long)((unsigned short)(((unsigned long)(hi)) & 0xffff))) << 16))
#define EB_LOWORD(l)           ((unsigned short)(((unsigned long)(l)) & 0xffff))
#define EB_HIWORD(l)           ((unsigned short)((((unsigned long)(l)) >> 16) & 0xffff))
#define USES_OFFMSG_PARAM_VERSION2
/*==========================================================
离线消息标识
===========================================================*/
typedef enum EB_OFFMSG_PARAM
{
	EB_OFFMSG_PARAM_NULL							= 0x0
	, EB_OFFMSG_PARAM_ACCEPT					= 0x001
	, EB_OFFMSG_PARAM_REJECT					= 0x002
	, EB_OFFMSG_PARAM_MSG							= 0x004
	, EB_OFFMSG_PARAM_SENDER_HIDE			= 0x008
	, EB_OFFMSG_PARAM_PRIVATE					= 0x010
#ifndef USES_OFFMSG_PARAM_VERSION2
	, EB_OFFMSG_PARAM_SUB_TYPE_AUDIO	= 0x020	// ????1.25(2017-01-12)，兼容旧版本，1.30以后删除该代码；
#endif
	, EB_OFFMSG_PARAM_RICH_CONTENT		= 0x040
	, EB_OFFMSG_PARAM_NOTIFY					= 0x080
}EB_OFFMSG_PARAM;

typedef enum PASSWD_AUTH_MODE
{
	PASSWD_AUTH_MODE_ENTBOOST
	, PASSWD_AUTH_MODE_MD5
	, PASSWD_AUTH_MODE_PBKDF2
}PASSWD_AUTH_MODE;


/*==========================================================
应用功能扩展
===========================================================*/
typedef enum EB_FUNC_EXT
{
	EB_FUNC_EXT_UNKNOWN							// 未知
	, EB_FUNC_EXT_DISABLE_CONTEXTMENU	= 0x1	// 屏蔽右键菜单
	, EB_FUNC_EXT_DISABLE_SCROLLBAR		= 0x2	// 屏蔽滚动条
	, EB_FUNC_EXT_CLOSE_PROMPT			= 0x10	// 关闭窗口提示
	, EB_FUNC_EXT_WIN_RESIZABLE			= 0x20	// 窗口可调整大小
	, EB_FUNC_EXT_BROWSER_SET_CEF		= 0x100	// Chrome浏览器
	, EB_FUNC_EXT_BROWSER_SET_IE		= 0x200	// IE浏览器
}EB_FUNC_EXT;

/*==========================================================
开放类型
===========================================================*/
typedef enum EB_ENABLE_LEVEL
{
	EB_ENABLE_LEVEL_UNKNOWN
	, EB_ENABLE_LEVEL_VISITOR			= 0x01
	, EB_ENABLE_LEVEL_AUTH				= 0x02
	, EB_ENABLE_LEVEL_MSG				= 0x04
	, EB_ENABLE_LEVEL_NONE_REG_CODE		= 0x08	// 没有reg_code权限，不需要发送验证邮件
	, EB_ENABLE_LEVEL_FUNC_AUTH			= 0x10	// 
	, EB_ENABLE_LEVEL_FUNC_MGR			= 0x20	// 应用功能管理
	, EB_ENABLE_LEVEL_PUSH_MSG			= 0x40	// 消息推送功能
	, EB_ENABLE_LEVEL_EXECUTE_SQL		= 0x100	// 
	, EB_ENABLE_LEVEL_SELECT_SQL		= 0x200	// 

	, EB_ENABLE_LEVEL_DEV_MGR			= 0x10000	// ***开发者应用管理（这是超级应用管理权限）
	, EB_ENABLE_LEVEL_SYS_EDIT			= 0x20000	// ***系统配置管理（这是超级应用管理权限）
	, EB_ENABLE_LEVEL_LICENSE_MGR		= 0x40000	// ***授权许可管理（这是超级应用管理权限）
	//, EB_ENABLE_LEVEL_PWD_EDIT			= 0x80000	// ***密码管理（这是超级应用管理权限）
}EB_ENABLE_LEVEL;
// 0x17 = 23
#define DEFAULT_LEVEL (EB_ENABLE_LEVEL_VISITOR|EB_ENABLE_LEVEL_AUTH|EB_ENABLE_LEVEL_MSG|EB_ENABLE_LEVEL_FUNC_AUTH)


/*==========================================================
呼叫状态
===========================================================*/
typedef enum EB_CALL_STATE
{
	EB_CALL_STATE_UNKNOWN
	, EB_CALL_STATE_INCOMING
	, EB_CALL_STATE_ALERTING
	, EB_CALL_STATE_CONNECTED
	, EB_CALL_STATE_INVALIDATE	// 网络失效，需要重新邀请
	, EB_CALL_STATE_EXIT
	, EB_CALL_STATE_HANGUP
	, EB_CALL_STATE_ONLINE_INCALL
	, EB_CALL_STATE_AUTO_ACK
}EB_CALL_STATE;


/*==========================================================
请求类型
===========================================================*/
typedef enum EB_REQEUST_TYPE
{
	EB_REQ_TYPE_UNKNOWN
	, EB_REQ_TYPE_REG
	, EB_REQ_TYPE_LOGON
	, EB_REQ_TYPE_INVITE
	, EB_REQ_TYPE_DEP
	, EB_REQ_TYPE_FINPWD
	, EB_REQ_TYPE_USER_INFO
	, EB_REQ_TYPE_HTTP_REQ

}EB_REQEUST_TYPE;

/*==========================================================
请求类型
===========================================================*/
typedef enum EB_PUT_INFO_TYPE
{
	EB_PUT_INFO_UNKNOWN
	, EB_PUT_INFO_UPDATE_SYS_INFO				= 0x01
	, EB_PUT_INFO_SEND_BC_MSG//					= 0x02
	, EB_PUT_INFO_RESET_DEFAULT_PWD//				= 0x04
	, EB_PUT_INFO_ENT_GROUP_VERSION_CHANGE//		= 0x08
	, EB_PUT_INFO_GROUP_MEMBER_VERSION_CHANGE//	= 0x10
	, EB_PUT_INFO_GROUP_MEMBER_CHANGE		//	= 0x20
	, EB_PUT_INFO_GROUP_MEMBER_DELETE
	, EB_PUT_INFO_LC_STARTUP
	, EB_PUT_INFO_CHECK_USER_EMAIL				// um sendto ebsemail
	, EB_PUT_INFO_USER_RECEIVE_EMAIL			// ebsemail sendto um
	, EB_PUT_INFO_USER_ADD_EMAIL_CONTACT	// ebsemail sendto um
	, EB_PUT_INFO_UM_CURRENT_USERS				// um sendto lc
	, EB_PUT_INFO_LC_CURRENT_USERS				// lc sendto lc
	, EB_PUT_INFO_UNAUTH_ERROR						// um sendto lc
	, EB_PUT_INFO_GROUP_INFO_CHANGE				// um sendto cm
	, EB_PUT_INFO_GROUP_MEMBER_CHANGE2		// um sendto cm

}EB_PUT_INFO_TYPE;

/*==========================================================
重设类型
===========================================================*/
typedef enum EB_RESET_TYPE
{
	EB_RESET_UNKNOWN
	, EB_RESET_RESET_DEFAULT_PWD
	, EB_RESET_FREEZE_ACCOUNT
	, EB_RESET_ENABLE_ACCOUNT
	, EB_RESET_DELETE_ACCOUNT
	//, EB_RESET_ACTIVE_ACCOUNT

}EB_RESET_TYPE;

/*==========================================================
同步类型
===========================================================*/
typedef enum EB_SYNC_TYPE
{
	EB_SYNC_UNKNOWN
	, EB_SYNC_EDIT_PWD
	, EB_SYNC_NEW_ACCOUNT
	, EB_SYNC_LINE_STATE

}EB_SYNC_TYPE;


/*==========================================================
字典类型
===========================================================*/
typedef enum EB_DICT_TYPE
{
	EB_DICT_UNKNOWN
	, EB_DICT_AREA_DICT_LIST
	, EB_DICT_AREA_DICT_INFO

}EB_DICT_TYPE;


/*==========================================================
用户扩展数据
===========================================================*/
typedef enum EB_ACCOUNT_DATA
{
	EB_DATA_DELETE_OFFLINE_CALL		= 0x01
	, EB_DATA_FROM_OFFLINE_USER		= 0x02
	, EB_DATA_GET_ENT_ID			= 0x04	// 是否已经获取企业ID
	, EB_DATA_ERROR_LOAD_CALL		= 0x08	// 异常加载会话信息
	, EB_DATA_OFF_MSG_CALL_USER		= 0x10	// 离线消息，呼叫在线用户
	, EB_DATA_LOAD_APP_SUB_INFO		= 0x20	// 加载应用功能订购信息
	, EB_DATA_NEED_CHANGE_LINESTATE	= 0x40	// 需要通知在线状态
	, EB_DATA_LOAD_SYS_MSG			= 0x80	// 已经加载系统消息
	//, EB_DATA_GET_DEVICE_TOKEN		= 0x100	// 是否已经获取IOS SSL ID & DEVICE TOKEN
	, EB_DATA_GET_PHONE				= 0x200	// 是否已经获取 phone
	, EB_DATA_GET_SYMMETRIC_CIPHER	= 0x400	// 是否已经获取 对称密码SC
	, EB_DATA_GET_CURRENT_ONLINE	= 0x800	// 是否当前登录UM
	, EB_DATA_SEND_FILE_CALL		= 0x1000	// 发送文件请求，是否已经呼叫一次；
	, EB_DATA_CHECK_UNREAD_EMAIL	= 0x2000
	, EB_DATA_ADMIN_MEMBER				= 0x4000	// 管理成员权限（不受禁言限制）
	, EB_DATA_FORBID_SPEECH				= 0x8000	// 禁言
	, EB_DATA_GET_IOS_DEVICE_TOKEN				= 0x10000	// 是否已经获取IOS SSL ID & DEVICE TOKEN
	, EB_DATA_GET_ANDROID_DEVICE_TOKEN		= 0x20000	// 是否已经获取ANDROID DEVICE TOKEN
}EB_ACCOUNT_DATA;

/*==========================================================
消息扩展数据
===========================================================*/
typedef enum EB_MSG_DATA
{
	EB_MSG_DATA_OFFSET				= 0x01
	,EB_MSG_DATA_FILE_EXIST			= 0x02
	,EB_MSG_DATA_SEND_DS_ACK		= 0x04	// 已经请求ack
	,EB_MSG_DATA_MSG_2_DB			= 0x08	// 
	,EB_MSG_DATA_MSG_2_FILE			= 0x10	// 
	,EB_MSG_DATA_UP_LOAD_FILE		= 0x20	// 
	,EB_MSG_DATA_FROM_LOCAL_FILE	= 0x40	// 
	,EB_MSG_DATA_SAVE_TO_FILE			= 0x80	// 
	,EB_MSG_DATA_FINISHED_TO_REMOVEFILE			= 0x100	// 
	,EB_MSG_DATA_RES_FROM_OFFMSG			= 0x100	// 
}EB_MSG_DATA;
/*==========================================================
限制类型
===========================================================*/
typedef enum EB_PERMIT_TYPE
{
	EB_PERMIT_TYPE_IP			= 0
	, EB_PERMIT_TYPE_APPID
}EB_PERMIT_TYPE;

/*==========================================================
服务类型
===========================================================*/
typedef enum EB_SERVER_TYPE
{
	EB_SERVER_TYPE_UNKNOWN
	, EB_SERVER_TYPE_LOGON_CENTER
	, EB_SERVER_TYPE_APP_CENTER
	, EB_SERVER_TYPE_USER_MANAGER	= 11
	, EB_SERVER_TYPE_CHAT_MANAGER
	, EB_SERVER_TYPE_VIDEO_MANAGE
	, EB_SERVER_TYPE_RD_MANAGER
	, EB_SERVER_TYPE_MMV_MANAGE
	, EB_SERVER_TYPE_MMA_MANAGE
	, EB_SERVER_TYPE_FILE_SERVER	= 20
}EB_SERVER_TYPE;

/*==========================================================
应用类型
===========================================================*/
typedef enum EB_APP_TYPE
{
	EB_APP_TYPE_CLIENT				= 0x0
	, EB_SERVER_TYPE_SERVER			= 0x1
	, EB_SERVER_TYPE_PUSH			= 0x2	// 推送服务
}EB_APP_TYPE;

/*==========================================================
服务状态
===========================================================*/
typedef enum EB_SERVER_STATE
{
	EB_SS_INIT					// 初始化
	, EB_SS_RUNNING				// 运行中...
	, EB_SS_DISCONNECT			// 断线，需要重新连接
	, EB_SS_STOPED				// 已经停止
	, EB_SS_TIMEOUT				// 超时
	, EB_SS_MOVED		// 
	, EB_SS_RESTART
	, EB_SS_EXIT
}EB_SERVER_STATE;

typedef enum EB_SYS_EXT_KEY
{
	EB_SYS_EXT_KEY_UNKNOWN
	, EB_SYS_EXT_KEY_CLEAR_UNAUTH_ACCOUNT		// 1 多长时间清空未注册用户（单位：小时）
	, EB_SYS_EXT_KEY_OAUTH_URL							// 2 
	, EB_SYS_EXT_KEY_SEND_MAIL_APPID				// 3
	, EB_SYS_EXT_KEY_OPEN_REGISTER					// 4 是否开放注册 0=不开放；1=开放用户注册；2=开放企业注册
	, EB_SYS_EXT_KEY_OPEN_VISITOR						// 5 是否开放游客功能 0/1
	, EB_SYS_EXT_KEY_ACCOUNT_PREFIX					// 6
	, EB_SYS_EXT_KEY_SUPPORT_VERSION				// 7 最小支持版本号
	, EB_SYS_EXT_KEY_DATABASE_TYPE					// 8 数据库类型 0=PostgreSQL; 1=MySQL
	, EB_SYS_EXT_KEY_SEND_CALL_APPID				// 9
	, EB_SYS_EXT_KEY_DEFAULT_PWD						// 10 默认密码
	, EB_SYS_EXT_KEY_MY_GROUP0				// 默认部门名称
	, EB_SYS_EXT_KEY_MY_GROUP2				// 默认群组名称
	, EB_SYS_EXT_KEY_MY_GROUP9				// 默认讨论组名称
	, EB_SYS_EXT_KEY_FUNC_REQ_URL			// "/rest.v01.ebwebum.freq"
	, EB_SYS_EXT_KEY_AUTH_MSG_URL			// "/"
	, EB_SYS_EXT_KEY_USER_PWD_CALLBACK_URL	// 长度必须大于1，否则自动设为空；
	, EB_SYS_EXT_KEY_CALLBACK_APPID			// 回调APPID
	, EB_SYS_EXT_KEY_MAX_APP_TEST_ACCOUNT	// 
	, EB_SYS_EXT_KEY_CALLBACK_ENC_MODE		// 回调加密模式，3des,aes_ecb,aes_cbc
	, EB_SYS_EXT_KEY_FORGET_PWD_URL					// 20 忘记密码URL
	, EB_SYS_EXT_KEY_ENT_LOGO_URL						// 21 企业LOGO URL
	, EB_SYS_EXT_KEY_DEPLOY_TYPE			// 部署类型 0：企业私有云；1：恩布公有云
	, EB_SYS_EXT_KEY_DEPLOY_ID				// 部署ID（服务器ID）
	, EB_SYS_EXT_KEY_DEPLOY_KEY				// 部署KEY；＝md5('eb_deploy_id'+[deploy_id])
	, EB_SYS_EXT_KEY_PRODUCT_NAME			// IM产品名称“恩布互联”，只有授权用户才可以修改；
	, EB_SYS_EXT_KEY_PASSWD_AUTH_MODE		// 密码验证方式 0=默认恩布; 1=md5(password)禅道合作
	, EB_SYS_EXT_KEY_ENT_MANAGER_URL		// 企业管理后台URL
	, EB_SYS_EXT_KEY_SAVE_CONVERSATIONS		// 保存聊天记录
	, xEB_SYS_EXT_KEY_INVITE_ADD2GROUP_AUTH	// 邀请用户进群验证 0/1 default=0
	, xEB_SYS_EXT_KEY_REQ_ADD2GROUP_AUTH			// 30 申请进群验证		0/1 default=1
	, EB_SYS_EXT_KEY_MEMBER_CHANGE_EVENT			// 31 群成员资料修改，通知企业事件		0/1 default=1
	, EB_SYS_EXT_KEY_CONVERSATIONS_URL		// 在线聊天记录URL
	, EB_SYS_EXT_KEY_GROUP_MSG_SUBID		// 群组消息应用订购ID
	, EB_SYS_EXT_KEY_OFFFILE_SPACE_SIZE		// 离线文件空间大小（默认50MB），0不支持离线空间
	, EB_SYS_EXT_KEY_ONLINE_MSG_SAVE_DAY	// 授权用户漫游消息保存天数（默认30天）
	, EB_SYS_EXT_KEY_OFFLINE_MSG_SAVE_DAY	// 授权用户离线消息保存天数（默认90天）
	, EB_SYS_EXT_KEY_USER_CD_SPACE_SIZE		// 授权用户云盘空间大小（默认50MB），0不支持个人用户云盘
	, EB_SYS_EXT_KEY_USERGROUP_CD_SPACE_SIZE	// 授权个人群组云盘空间大小（默认50MB），0不支持
	, EB_SYS_EXT_KEY_ENTGROUP_CD_SPACE_SIZE		// 授权公司部门云盘空间大小（默认50MB），0不支持
	, EB_SYS_EXT_KEY_PUSH_TIME								// 40 推送消息时间间隔，默认180S,3分钟
	, EB_SYS_EXT_KEY_TEXT_RESOURCE						// 41 文本资源：表情,图片,语音,文件,离线文件,名片,个人名片,
	, EB_SYS_EXT_KEY_DEFAULT_ALLOW_OUT_CHAT		// 新用户默认是否允许外部聊天请求，0/1，默认1接受，0拒绝外部聊天请求
	, EB_SYS_EXT_KEY_DEFAULT_ACCEPT_OUT_CHAT	// 新用户默认是否自动接受外部聊天请求，0/1，默认1接受，0需要手工确认接受
	, EB_SYS_EXT_KEY_MAX_RETRY_COUNT			// 最大重试错误次数，默认5，0不限制
	, EB_SYS_EXT_KEY_TEMP_LOCK_MINUTE			// 临时锁定用户时间，单位分钟，默认30分钟，0不锁定
	, EB_SYS_EXT_KEY_DELETE_ENT_MEMBER_OP		// 删除企业员工操作，默认0：保留帐号；1：临时冻结帐号，不能登录；2：删除帐号及所有数据
	, xEB_SYS_EXT_KEY_AUTH_CONTACT				// 是否验证联系人，默认0：不验证；1：需要验证（同时外部联系人，不能聊天）
	, EB_SYS_EXT_KEY_FIND_APP_SUBID				// 找群找人订购ID
	, EB_SYS_EXT_KEY_MAX_UG_COUNT				// 最大分组数量，默认15
	, EB_SYS_EXT_KEY_MAX_CONTACT_COUNT				// 50 最大联系人，默认500
	, EB_SYS_EXT_KEY_USES_UID_LIST						// 51 使用UID数据池
	, EB_SYS_EXT_KEY_EMPTY_UID_LIST_AUTO		// UID数据池为空，是否自动创建
	, EB_SYS_EXT_KEY_REGISTER_URL				// 注册URL
	, EB_SYS_EXT_KEY_AUTO_OPEN_SUBID			// 登录自动打开订购APP
	, xEB_SYS_EXT_KEY_AUTOHIDE_MAIN_FRAME_PC	// 登录自动隐藏主界面
	, xEB_SYS_EXT_KEY_HIDE_MAIN_FRAME_PC			// 隐藏主界面（使用打开配置APP界面）
	, EB_SYS_EXT_KEY_ENCRYPTION_SALT			// 加密附加串，类似于md5(salt)
	, EB_SYS_EXT_KEY_PBKDF2_COUNT				// HASH运算次数，默认1
	, EB_SYS_EXT_KEY_PBKDF2_LENGTH				// 需要多少位的密码；默认32，＝64个字节密码
	, EB_SYS_EXT_KEY_OPEN_ENT_MGR							// 60 是否开放普通企业管理功能 0/1
	, EB_SYS_EXT_KEY_OFFLINE_FILE_SAVE_DAY		// 61 授权用户离线文件保存天数（默认90天）
	//, EB_SYS_EXT_KEY_OPEN_UID_REGISTER			// 是否开放UID注册 0=不开放；1=开放注册 默认0不开放
	, xEB_SYS_EXT_KEY_DISABLE_USER_CLOUD_DRIVE	// 是否禁用个人云盘功能 0/1 默认0
	, xEB_SYS_EXT_KEY_DISABLE_GROUP_SHARED_CLOUD	// 是否禁用群组（部门）共享功能 0/1 默认0
	, xEB_SYS_EXT_KEY_DISABLE_MODIFY_PASSWD		// 是否禁用修改密码功能 0/1 默认0
	, EB_SYS_EXT_KEY_CALLBACK_PARAM				// 回调参数
	, EB_SYS_EXT_KEY_ACCOUNT_AUTH_URL			// 帐号验证接口
	, EB_SYS_EXT_KEY_ACCOUNT_ACTIVE_URL			// 帐号激活接口
	, EB_SYS_EXT_KEY_SYSTEM_VALUE				// xxx
	, EB_SYS_EXT_KEY_AUTO_ADD_GROUPID			// 新注册用户，自动添加到群组（或部门）
	//, EB_SYS_EXT_KEY_DISABLE_VIDEO				// 是否禁用视频聊天 0/1 默认0
	//, EB_SYS_EXT_KEY_DISABLE_REMOTE_DESKTOP		// 是否禁用视频聊天 0/1 默认0
	, EB_SYS_EXT_KEY_CLUSTER_TYPE						// 70 集群类型，默认1：自动用户动态分配 0：不动态分配
	, EB_SYS_EXT_KEY_EMAIL_SUBID						// 71 邮件应用ID
	, EB_SYS_EXT_KEY_POP3_SERVER_LIST				// 用于配置邮箱对应POP3,SMTP地址；格式：tom.com,pop.tom.com,110,smtp.tom.com,25;xxx ~~pop3.server，格式例子：vip.sina.com 未设置，采用pop.xxx.com
	, EB_SYS_EXT_KEY_AUTO_TRANSFORM_ACCOUNT		// 自动转换帐号小写，（是否区分帐号大小写） 默认1：转换（不区分大小字）；0：不转换（区分大小写）
	, EB_SYS_EXT_KEY_REQUEST_RECEIVE_MIN_TIME		// 最小接收邮件时间间隔，默认2分钟
	, EB_SYS_EXT_KEY_REQUEST_RECEIVE_MAX_TIME		// 最大接收邮件时间间隔，默认5分钟
	, EB_SYS_EXT_KEY_VIEW_OFFICE_URL				// https://view.officeapps.live.com/op/view.aspx?src=
	, EB_SYS_EXT_KEY_VIEW_OFFICE_EXT				// .doc.docx.docm.dotx.dotm.xls.xlsx.xlsm.xltx.xltm.xlsb.xlam.ppt.pptx.pptm.potx.potm.ppam.ppsx.ppsm.sldx.sldm.thmx.pdf
	, EB_SYS_EXT_KEY_VIEW_IMAGE_EXT					// .png.jpg.jpeg.bmp.ico
	, EB_SYS_EXT_KEY_MAX_OFF_FILE_SIZE				// default 20mb
	, EB_SYS_EXT_KEY_MAX_RES_FILE_SIZE					// 80 default 20mb
	//, EB_SYS_EXT_KEY_DISABLE_ENTBOOST_TW			// 禁用协同办公功能
	//, EB_SYS_EXT_KEY_MAIL_SERVER_LIST					// 用于配置邮件对应地址，pop3.server，格式例子：vip.sina.com 未设置，采用pop.xxx.com
	, EB_SYS_EXT_KEY_MAX_SYSTEM_TEMP_VALUE			// 81 用于配置一些临时数据
	, EB_SYS_EXT_KEY_MAX_RECEIVE_OFFMSG_COUNT		// 82 最大一次接收离线消息数量 default:200
	, EB_SYS_EXT_KEY_PUSH_CONTENT_TYPE					// 83 推送消息类型 0:聊天内容 1:只显示新消息
	, EB_SYS_EXT_KEY_MAX_OAUTH_TIME_DAYS				// 84 客户端设备自动登录，验证KEY保存天数，默认90天；
	, EB_SYS_EXT_KEY_ECARD_INFO_FLAG						// 85 默认电子名片资料开放设置，0:默认 1:禁止开放联系电话和手机号码
	, EB_SYS_EXT_KEY_MAX_BCMSG_SAVE_DAYS				// 86 广播消息，保存天数，默认30天，超过自动删除
	, EB_SYS_EXT_KEY_EB_SERVER_VERSION					// 87 服务端版本号，如 544=1.25.0.544 
	, EB_SYS_EXT_KEY_DEFAULT_URL						// 88 默认URL网址，默认 http://www.entboost.com
	, EB_SYS_EXT_KEY_SYSTEM_BANDWIDTH					// 89 系统带宽设置，默认 10MB，主要用于限制下行速度
	, EB_SYS_EXT_KEY_ATTEND_START_DATE	= 2000	// 2000 考勤开始时期，格式:YYYY-mm-dd 默认空未开始
	, EB_SYS_EXT_KEY_ATTEND_DAILY_JOB_DATE			// 2001 考勤每天作业时期，格式:YYYY-mm-dd 默认空未开始

}EB_SYS_EXT_KEY;

typedef enum EB_APP_EXT_KEY
{
	EB_APP_EXT_KEY_UNKNOWN
	, EB_APP_EXT_KEY_CAPACITY_REG_USER	// 支持注册个人用户数；-1=限制；0=无限制；>0=每数量单位注册一千用户
	, EB_APP_EXT_KEY_CURRENT_REG_USER	// 当前注册用户数
	, EB_APP_EXT_KEY_CAPACITY_REG_ENT	// 支持注册企业数；-1=限制；0=无限制；>0=每数量单位注册一个企业
	, EB_APP_EXT_KEY_CURRENT_REG_ENT	// 当前注册企业数

}EB_APP_EXT_KEY;

#define MAX_USER_EXT_STRING_LEN 1024
typedef enum EB_USER_EXT_KEY
{
	EB_USER_EXT_KEY_UNKNOWN
	, EB_USER_EXT_KEY_LICENSE						// 是否有授权信息
	, EB_USER_EXT_KEY_VIDEO_CONFERENCE_COUNT		// 授权视频会议数
	, EB_USER_EXT_KEY_REG_EXT						// 用户注册扩展信息
	, EB_USER_EXT_KEY_AUTO_ACK_UID					// 用户自动应答UID
	, EB_USER_EXT_KEY_GROUP_INFO_VERSION			// 个人群组（讨论组）资料版本号
	, EB_USER_EXT_KEY_OLD_AUTH_MODE					// 保存旧的验证方式；
	, EB_USER_EXT_KEY_OLD_ACCOUNT_PREFIX			// 保存旧的帐号前缀
	, EB_USER_EXT_KEY_DEVICE_TOKEN_IOS			// SSL ID & device token for ios
	, EB_USER_EXT_KEY_PUSH_TIME						// 最后消息推送时间 for ios
	, EB_USER_EXT_KEY_CONTACT_INFO_VERSION			// 联系人资料版本号
	, EB_USER_EXT_KEY_ACCESS_TOKEN					// 
	//, EB_USER_EXT_KEY_CSP_LOGIN						// 保存CSP登录KEY
	, EB_USER_EXT_KEY_DEVICE_TOKEN_ANDROID		// device token for android
	, EB_USER_EXT_KEY_USER_HEAD_VERSION				// 用户头像版本号
	, EB_USER_EXT_KEY_LOGON_TYPE_IP_BEGIN	= 101	// 保存用户不同登录类型 IP1
	, EB_USER_EXT_KEY_LOGON_TYPE_IP_END		= 120	// 保存用户不同登录类型 IP2
	, EB_USER_EXT_KEY_LOGON_TYPE_KEY_BEGIN	= 121	// 保存用户不同登录类型 LOGONKEY1
	, EB_USER_EXT_KEY_LOGON_TYPE_KEY_END	= 140	// 保存用户不同登录类型 LOGONKEY2
	, EB_USER_EXT_KEY_LOGON_TYPE_SOTP_BEGIN	= 141	// 保存用户不同登录类型 SOTP ID
	, EB_USER_EXT_KEY_LOGON_TYPE_SOTP_END	= 160	// 保存用户不同登录类型 SOTP ID

}EB_USER_EXT_KEY;

typedef enum EB_ENT_EXT_KEY
{
	EB_ENT_EXT_KEY_UNKNOWN
	, EB_ENT_EXT_KEY_LICENSE							// 是否有授权信息
	, EB_ENT_EXT_KEY_VIDEO_CONFERENCE_COUNT				// 授权视频会议数
	, EB_ENT_EXT_KEY_USER_PWD_CALLBACK_URL				// 用户密码回调URL
	, EB_ENT_EXT_KEY_DEPARTMENT_INFO_VERSION			// 企业部门资料版本号
	, EB_ENT_EXT_KEY_SETTING_VALUE						// 配置信息
	, xEB_ENT_EXT_KEY_ENABLE_MODIFY_MEMBER_INFO = 0x10	// 允许员工修改自己资料 0/1 default 0
}EB_ENT_EXT_KEY;
//typedef enum EB_ENT_EXT_KEY_SETTING_VALUE
//{
//	EB_ENT_EXT_KEY_SETTING_VALUE_ENABLE_MODIFY_MEMBER_INFO		= 0x1	// 允许员工修改自己资料 0/1 default 0
//	, EB_ENT_EXT_KEY_SETTING_VALUE_DISABLE_MODIFY_ACCOUNT_INFO	= 0x2	// 允许员工修改个人资料 0/1 default 1
//};

typedef enum EB_DEP_EXT_KEY
{
	EB_DEP_EXT_KEY_UNKNOWN
	, EB_DEP_EXT_KEY_ORG_GROUP_ID						// 原始群组（部门）ID，导数据输入
	, EB_DEP_EXT_KEY_DISPLAY_INDEX					// 群组（部门），显示排序，低排上面，高排下面

}EB_DEP_EXT_KEY;

typedef enum EB_EMP_EXT_KEY
{
	EB_EMP_EXT_KEY_UNKNOWN
	, EB_EMP_EXT_KEY_MEMBER_FORBID_SPEECH		// 群组（部门），成员禁言设置 0永久禁言，>0表示禁言分钟

}EB_EMP_EXT_KEY;

typedef enum EB_MAIL_MSG_TYPE
{
	EB_MAIL_MSG_TYPE_REGISTER
	, EB_MAIL_MSG_TYPE_NEW_MEMBER
	, EB_MAIL_MSG_TYPE_RESET_PASSWD
	, EB_MAIL_MSG_TYPE_RESEND_REGCODE	= 5
	, EB_MAIL_MSG_TYPE_MAX_SYSTEM		= 100

}EB_MAIL_MSG_TYPE;


//const int	EB_MAX_FILESIZE_SAVE_DISK			= 50*1024*1025;	// 50M
const int	EB_MAX_TIME_OFFLINE						= 5*1000;		// 5-minute
const int	EB_MAX_APP_ONLINE_KEY_TIMEOUT	= 7*24*3600;		// 7-day //24-hour
const int	EB_MAX_HEART_BEAT_SECOND			= 25;			// 25-second，40S某些环境会有问题；（如WIN8虚拟机）；30好一些，但还是会超时；
const int	EB_MAX_RICH_FORMAT_SIZE				= 1024*8;		// *8KB
const int	EB_MAX_RICH_TOTAL_SIZE				= 1024*1024*10;	// 10MB
const int	EB_MAX_ALL_RICH_SIZE					= 1024*1024*100;// 100MB
const int	EB_MAX_MEMORY_SIZE						= 1024*1024*50;	// 50MB

#define TEMP_INT_VALUE_LICENSE_INFO	1	// 授权信息
//#define TEMP_INT_VALUE_LICENSE		1	// 是否已经授权
//#define TEMP_INT_VALUE_USER_COUNT	2	// 授权用户数量
//#define TEMP_INT_VALUE_VCC_COUNT	3	// 授权视频会议数量

#define TEMP_INT_VALUE_USER_EB_SID			100
#define TEMP_INT_VALUE_USER_ONLINE_KEY	101

#endif // __eb_defines_h__
