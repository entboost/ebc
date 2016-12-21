// eb_define.h file here
#ifndef __eb_define_h__
#define __eb_define_h__
#ifdef WIN32
#include "Windows.h"
#endif // WIN32

namespace entboost {

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#endif // min
namespace eb
{
#ifdef WIN32
	typedef __int64				bigint;
#define eb_atoi64(a) _atoi64(a)
#define eb_sprintfi64(a,i64) sprintf(a,"%I64d",i64)
#else
	typedef long long			bigint;
#define eb_atoi64(a) atoll(a)
#define eb_sprintfi64(a,i64) sprintf(a,"%lld",i64)
#endif
} // cgc namespace


/*==========================================================
企业扩展数据
===========================================================*/
typedef enum EB_ENT_EXT_DATA
{
	EB_ENT_EXT_DATA_NULL
	, EB_ENT_EXT_DATA_ENABLE_MODIFY_MEMBER_INFO		= 0x1	// 允许员工修改自己部门资料
	, EB_ENT_EXT_DATA_DISABLE_MODIFY_ACCOUNT_INFO	= 0x2	// 允许员工修改自己个人资料
};

/*==========================================================
系统参数
===========================================================*/
typedef enum EB_SYSTEM_PARAMETER
{
	EB_SYSTEM_PARAMETER_UNKNOWN
	, EB_SYSTEM_PARAMETER_ACCOUNT_PREFIX				// char*
	, EB_SYSTEM_PARAMETER_OPEN_REGISTER					// int
	, EB_SYSTEM_PARAMETER_OPEN_VISITOR					// int
	, EB_SYSTEM_PARAMETER_LICENSE_USER					// int 0/1
	, EB_SYSTEM_PARAMETER_FORGET_PWD_URL				// char*
	, EB_SYSTEM_PARAMETER_ENT_LOGO_URL					// char*
	, EB_SYSTEM_PARAMETER_SEND_REG_MAIL					// int 0/1
	, EB_SYSTEM_PARAMETER_DEPLOY_ID						// char*
	, EB_SYSTEM_PARAMETER_PRODUCT_NAME					// char*
	, EB_SYSTEM_PARAMETER_ENT_MANAGER_URL				// char*
	, EB_SYSTEM_PARAMETER_SAVE_CONVERSATIONS			// int 0/1
	, EB_SYSTEM_PARAMETER_GROUP_MSG_SUBID				// char*
	, EB_SYSTEM_PARAMETER_FIND_APP_SUBID				// char*
	, EB_SYSTEM_PARAMETER_AUTH_CONTACT					// int 0/1
	, EB_SYSTEM_PARAMETER_AUTH_INVITEADD2GROUP			// int 0/1
	, EB_SYSTEM_PARAMETER_REGISTER_URL					// char*
	, EB_SYSTEM_PARAMETER_AUTO_OPEN_SUBID				// char*
	, EB_SYSTEM_PARAMETER_AUTOHIDE_MAINFRAME			// int 0/1
	, EB_SYSTEM_PARAMETER_HIDE_MAINFRAME				// int 0/1
	, EB_SYSTEM_PARAMETER_DISABLE_USER_CLOUD_DRIVE		// int 0/1
	, EB_SYSTEM_PARAMETER_DISABLE_GROUP_SHARED_CLOUD	// int 0/1
	, EB_SYSTEM_PARAMETER_DISABLE_MODIFY_PASSWD			// int 0/1
	, EB_SYSTEM_PARAMETER_LOGON_HTTP_REQ_URL			// char*
	, EB_SYSTEM_PARAMETER_DISABLE_VIDEO					// int 0/1
	, EB_SYSTEM_PARAMETER_DISABLE_REMOTE_DESKTOP		// int 0/1
	, EB_SYSTEM_PARAMETER_DISABLE_ACCOUNT_EDIT			// int 0/1
	, EB_SYSTEM_PARAMETER_DEFAULT_BROWSER_TYPE			// int 0=谷歌Chrome浏览器 1=微软IE浏览器
	, EB_SYSTEM_PARAMETER_LOGON_USER_DATA					// char* set only
	, EB_SYSTEM_PARAMETER_DISABLE_MSG_RECEIPT			// int 0/1
	, EB_SYSTEM_PARAMETER_LICENSE_TYPE						// int 许可类型；0=未授权；1=终身授权；2=时间授权
	, EB_SYSTEM_PARAMETER_STAT_SUB_GROUP_MEMBER		// int 是否统计子部门人数 0/1
};

/*==========================================================
FUNC BROWSER TYPE
===========================================================*/
typedef enum EB_FUNC_BROWSER_TYPE
{
	EB_FUNC_BROWSER_TYPE_DEFAULT
	, EB_FUNC_BROWSER_TYPE_CEF
	, EB_FUNC_BROWSER_TYPE_IE
};

/*==========================================================
RICH SUB TYPE
===========================================================*/
typedef enum EB_RICH_SUB_TYPE
{
	EB_RICH_SUB_TYPE_JPG
	, EB_RICH_SUB_TYPE_AUDIO			= 11
	, EB_RICH_SUB_TYPE_MAP_POS		= 21	// 地图位置
	, EB_RICH_SUB_TYPE_USER_DATA	= 200	// 用户自定义数据
};

/*==========================================================
RICH SUB TYPE
===========================================================*/
typedef enum EB_BROADCAST_SUB_TYPE
{
	EB_BROADCAST_SUB_TYPE_BC
	, EB_BROADCAST_SUB_TYPE_NEW_EMAIL
	, EB_BROADCAST_SUB_TYPE_UNREAD_EMAIL
	, EB_BROADCAST_SUB_TYPE_TW_MSG							// 协同办公提醒消息
	, EB_BROADCAST_SUB_TYPE_SUBID_UNREAD_MSG		// 订购ID，未读消息；content格式: [订购ID],[未读消息数量]，例如: "1002300105,3" 表示 1002300105 该订购ID，有3条未读消息；
};

/*==========================================================
应用功能显示位置
===========================================================*/
typedef enum EB_FUNC_LOCATION
{
	EB_FUNC_LOCATION_UNKNOWN						= 0x0
	, EB_FUNC_LOCATION_MAINFRAME_BTN1				= 0x0001		// 主界面按钮（上）
	, EB_FUNC_LOCATION_MAINFRAME_BTN2				= 0x0002		// 主界面按钮（下）
	, EB_FUNC_LOCATION_PHONE_APPFRAME				= 0x0004		// 手机应用（应用面板按钮）
	, EB_FUNC_LOCATION_APPFRAME_BTN					= 0x0008		// 应用面板按钮
	, EB_FUNC_LOCATION_RIGHT_CLICK_MENU_MAINFRAME	= 0x0010		// 主界面右键菜单
	, EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER		= 0x0020		// 用户右键菜单
	, EB_FUNC_LOCATION_RIGHT_CLICK_MENU_GROUP		= 0x0040		// 群组（部门）右键菜单
	, EB_FUNC_LOCATION_RIGHT_CLICK_MENU_ENTERPRISE	= 0x0080		// 企业右键菜单
	, EB_FUNC_LOCATION_GROUP_CHAT_BTN1				= 0x0100		// 群组会话上面按钮
	, EB_FUNC_LOCATION_GROUP_CHAT_BTN2				= 0x0200		// 群组会话下面按钮
	, EB_FUNC_LOCATION_USER_CHAT_BTN1				= 0x0400		// 用户会话上面按钮
	, EB_FUNC_LOCATION_USER_CHAT_BTN2				= 0x0800		// 用户会话下面按钮
	, EB_FUNC_LOCATION_CALL_CHAT = EB_FUNC_LOCATION_GROUP_CHAT_BTN1|EB_FUNC_LOCATION_GROUP_CHAT_BTN2|EB_FUNC_LOCATION_USER_CHAT_BTN1|EB_FUNC_LOCATION_USER_CHAT_BTN2
	, EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER_CHAT	= 0x1000		// 个人聊天界面右键菜单
	, EB_FUNC_LOCATION_RIGHT_CLICK_MENU_GROUP_CHAT	= 0x2000		// 群组聊天界面右键菜单
	, EB_FUNC_LOCATION_RIGHT_CLICK_MENU_CHAT_RECORD	= 0x4000		// 聊天记录右键菜单
	, EB_FUNC_LOCATION_ALL_RIGHT_CLICK_MENU_CHAT = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER_CHAT|EB_FUNC_LOCATION_RIGHT_CLICK_MENU_GROUP_CHAT|EB_FUNC_LOCATION_RIGHT_CLICK_MENU_CHAT_RECORD
	, EB_FUNC_LOCATION_CHAT_COPY_TEXT_TOOLBAR				= 0x8000		// 聊天窗口复制文本工具栏
	, EB_FUNC_LOCATION_DISABLE_PC										= 0x10000		// 禁用PC端
};

/*==========================================================
应用功能模式
===========================================================*/
typedef enum EB_FUNC_MODE
{
	EB_FUNC_MODE_BROWSER			// 浏览器模式
	, EB_FUNC_MODE_MAINFRAME		// 主面板（默认）
	, EB_FUNC_MODE_MODAL			// 对话框模式（模式）
	, EB_FUNC_MODE_PROGRAM			// 打开应用程序
	, EB_FUNC_MODE_SERVER			// 服务模式（HTTP POST）
	, EB_FUNC_MODE_WINDOW			// 窗口模式（无模式）
};

/*==========================================================
应用功能参数
===========================================================*/
typedef enum EB_FUNC_PARAM
{
	EB_FUNC_PARAM_NONE				// 没有参数
	, EB_FUNC_PARAM_EB_DEFAULT		// 恩布默认参数，authid&fk&ums
	, EB_FUNC_PARAM_USER_INFO		// 用户信息，uid&account
};

/*==========================================================
会话连接标识
===========================================================*/
typedef enum EB_CONNECTED_FLAG
{
	EB_CONNECTED_NORMAL				= 0x000
	, EB_CONNECTED_OFFLINE_USER		= 0x001
	, EB_CONNECTED_OWNER_CALL		= 0x002
	, EB_CONNECTED_AUTO_ACK			= 0x004
	, EB_CONNECTED_MSG				= 0x008
};

/*==========================================================
资源类型
===========================================================*/
typedef enum EB_RESOURCE_TYPE
{
	EB_RESOURCE_UNKNOWN
	, EB_RESOURCE_NOTE				// 文本笔记
	, EB_RESOURCE_MSG				// 消息资源 rich&image need cm
	, EB_RESOURCE_HEAD				// 头像资源	need cm
	, EB_RESOURCE_DIR				// 目录资源
	, EB_RESOURCE_FILE				// 文件资源 need cm
	, EB_RESOURCE_EMOTION			// 表情资源
	, EB_RESOURCE_FUNCICON			// 应用功能图标 
	, EB_RESOURCE_CONTENT			// 内容（如邮件content-id附件）
};
/*==========================================================
资源共享类型
===========================================================*/
typedef enum EB_RESOURCE_SHARE_TYPE
{
	EB_RESOURCE_SHARE_UNKNOWN
	, EB_RESOURCE_SHARE_ALL		// 共享资源
	, EB_RESOURCE_SHARE_TEMP	// 群组临时文件
	, EB_RESOURCE_SHARE_COMMENT	// 计划、任务等评论附件
};
/*==========================================================
文件扩展类型
===========================================================*/
typedef enum EB_FILE_EXT_TYPE
{
	EB_FILE_EXT_TYPE_OTHER
	, EB_FILE_EXT_TYPE_PDF
	, EB_FILE_EXT_TYPE_IMAGE
	, EB_FILE_EXT_TYPE_OFFICE
};
/*==========================================================
资源来源类型
===========================================================*/
typedef enum EB_RESOURCE_FROM_TYPE
{
	EB_RESOURCE_FROM_TYPE_UNKNOWN			= 0
	, EB_RESOURCE_FROM_TYPE_ENT
	, EB_RESOURCE_FROM_TYPE_GROUP
	, EB_RESOURCE_FROM_TYPE_USER
	, EB_RESOURCE_FROM_TYPE_OFFMSG
	, EB_RESOURCE_FROM_TYPE_FUNC			= 10
	, EB_RESOURCE_FROM_TYPE_PLAN			= 11
	, EB_RESOURCE_FROM_TYPE_TASK
	, EB_RESOURCE_FROM_TYPE_REPORT
	, EB_RESOURCE_FROM_TYPE_MCON_ID_ATT		= 20

};


/*==========================================================
视频类型
===========================================================*/
typedef enum EB_VIDEO_TYPE
{
	EB_VIDEO_UNKNOWN	= 0				// 未知
	, EB_VIDEO_AUDIO	= 1				// 语音
	, EB_VIDEO_BOTH		= 2				// 语音&视频
};

/*==========================================================
远程桌面类型
===========================================================*/
typedef enum EB_REMOTE_DESKTOP_TYPE
{
	EB_RD_UNKNOWN			= 0				// 未知
	, EB_RD_DESKTOP_SRC		= 1				// 申请共享本端桌面
	, EB_RD_DESKTOP_DEST	= 2				// 申请共享对方桌面
};

/*==========================================================
性别
===========================================================*/
typedef enum EB_GENDER_TYPE
{
	EB_GENDER_UNKNOWN
	, EB_GENDER_MALE				// 男性
	, EB_GENDER_FEMALE				// 女性
};

/*==========================================================
群组类型
===========================================================*/
typedef enum EB_GROUP_TYPE
{
	EB_GROUP_TYPE_DEPARTMENT		// 企业部门 （由公司人员设定，不能随意添加用户，或退出）
	, EB_GROUP_TYPE_PROJECT			// 项目组 （同上）
	, EB_GROUP_TYPE_GROUP			// 固定群组 （所有人可以创建，管理员随时添加成员，或退出）
	, EB_GROUP_TYPE_TEMP = 9		// 临时讨论组 （由聊天成员动态创建，所有人随时添加成员，或退出）
};

/*==========================================================
在线状态
===========================================================*/
typedef enum EB_USER_LINE_STATE
{
	EB_LINE_STATE_FREEZE		= -1	// 帐号临时冻结
	, EB_LINE_STATE_UNKNOWN		= 0		// 帐号未验证
	, EB_LINE_STATE_ONLINE_OLD			// 在线
	, EB_LINE_STATE_OFFLINE				// 离线
	, EB_LINE_STATE_BUSY				// 忙
	, EB_LINE_STATE_AWAY				// 离开
	, EB_LINE_STATE_ONLINE_NEW			// 在线
	, EB_USER_CHANGE_STATE		= 0x100
};

/*==========================================================
状态码
===========================================================*/
typedef enum EB_STATE_CODE
{
	EB_STATE_OK							= 0
	, EB_STATE_ERROR					= 1
	, EB_STATE_NOT_AUTH_ERROR					// 没有权限
	, EB_STATE_ACC_PWD_ERROR					// 帐号或密码错误
	, EB_STATE_NEED_RESEND						// 需要重发数据
	, EB_STATE_TIMEOUT_ERROR					// 超时错误
	, EB_STATE_EXIST_OFFLINE_MSG				// 存在离线消息
	, EB_STATE_USER_OFFLINE						// 用户离线状况
	, EB_STATE_USER_BUSY						// 用户线路忙
	, EB_STATE_USER_HANGUP						// 用户挂断会话
	, EB_STATE_OAUTH_FORWARD					// OAUTH转发
	, EB_STATE_UNAUTH_ERROR						// 未验证错误
	, EB_STATE_ACCOUNT_FREEZE					// 帐号已经冻结
	, EB_STATE_NEED_DELETE_MSG				// 13 需要删除消息
	, EB_STATE_WAITING_PROCESS				// 14 等待处理（消息）
	, EB_STATE_PARAMETER_ERROR			= 15	// 参数错误
	, EB_STATE_DATABASE_ERROR					// 数据库操作错误
	, EB_STATE_NEW_VERSION						// 新版本
	, EB_STATE_FILE_ALREADY_EXIST				// 文件已经存在
	, EB_STATE_FILE_BIG_LONG					// 文件超过大小
	, EB_STATE_ACCOUNT_NOT_EXIST		= 20	// 帐号不存在
	, EB_STATE_ACCOUNT_ALREADY_EXIST			// 帐号已经存在
	, EB_STATE_ACCOUNT_DISABLE_OFFCALL			// 禁止离线会话
	, EB_STATE_ACCOUNT_DISABLE_EXTCALL			// 禁止外部会话
	, EB_STATE_DISABLE_REGISTER_USER	= 25	// 禁止用户注册功能
	, EB_STATE_DISABLE_REGISTER_ENT				// 禁止企业注册功能
	, EB_STATE_ENTERPRISE_ALREADY_EXIST	= 30	// 公司名称已经存在
	, EB_STATE_ENTERPRISE_NOT_EXIST				// 没有公司信息（企业不存在）
	, EB_STATE_DEP_NOT_EXIST					// 不存在群组（部门）
	, EB_STATE_EXIST_SUB_DEPARTMENT				// 存在子部门
	, EB_STATE_DEP_ACC_ERROR					// 群组或成员不存在
	, EB_STATE_ENT_ACC_ERROR					// 企业员工成员不存在
	, EB_STATE_CS_MAX_ERROR						// 超过客服座席最大数量
	, EB_STATE_NOT_CS_ERROR						// 没有客服座席
	, EB_STATE_EXCESS_QUOTA_ERROR				// 超过最大流量配额
	, EB_STATE_ENT_GROUP_ERROR					// 企业部门
	, EB_STATE_ONLINE_KEY_ERROR			= 40
	, EB_STATE_UM_KEY_ERROR
	, EB_STATE_CM_KEY_ERROR
	, EB_STATE_DEVID_KEY_ERROR
	, EB_STATE_APPID_KEY_ERROR
	, EB_STATE_DEVID_NOT_EXIST
	, EB_STATE_APPID_NOT_EXIST
	, EB_STATE_APP_ONLINE_KEY_TIMEOUT
	, EB_STATE_KEY_ERROR						// KEY错误
	, EB_STATE_CALL_NOT_EXIST			= 50
	, EB_STATE_CHAT_NOT_EXIST
	, EB_STATE_MSG_NOT_EXIST
	, EB_STATE_RES_NOT_EXIST
	, EB_STATE_NOT_MEMBER_ERROR
	, EB_STATE_ATTACHMENT_NOT_EXIST
	, EB_STATE_FILE_NOT_EXIST
	, EB_STATE_NO_UM_SERVER				= 60
	, EB_STATE_NO_CM_SERVER
	, EB_STATE_NO_VM_SERVER
	, EB_STATE_NO_AP_SERVER
	, EB_STATE_NO_RD_SERVER
	, EB_STATE_ENT_BLACKLIST			= 70	// 企业黑名单用户
	, EB_STATE_ANOTHER_ENT_ACCOUNT				// 其他企业帐号
	, EB_STATE_MAX_CAPACITY_ERROR				// 最大容量错误
	, EB_STATE_NOT_SUPPORT_VERSION_ERROR		// 不支持当前版本
	, EB_STATE_FORWARD_MSG						// 转发消息
	, EB_STATE_MAX_RETRY_ERROR					// 错误次数太多，请三十分钟后再试！
	, EB_STATE_TOKEN_ERROR						// TOKEN错误
	, EB_STATE_MAX_UG_ERROR						// 超过最大分组数量
	, EB_STATE_MAX_CONTACT_ERROR				// 超过最大联系人数量
	, EB_STATE_CONTACT_NOT_EXIST				// 联系人不存在
	, EB_STATE_PHONE_ALREADY_EXIST				// 手机帐号已经存在
	, EB_STATE_ALREADY_IN_REMOTE_DESKTOP		// 已经在远程桌面中
	, EB_STATE_CONTENT_BIG_LENGTH				// 内容超过长度
	, EB_STATE_MEMORY_ERROR				// 内存不够
	, EB_STATE_DATA_ERROR				// 数据或格式错误
	, EB_STATE_CREATE_FILE_ERROR				// 创建文件失败

	, EB_STATE_PLAN_NOT_EXIST			= 100	// 计划不存在
	, EB_STATE_TASK_NOT_EXIST					// 任务不存在
	, EB_STATE_REPORT_NOT_EXIST					// 报告不存在

};

/*==========================================================
用户类型
===========================================================*/
typedef enum EB_ACCOUNT_TYPE
{
	EB_ACCOUNT_TYPE_VISITOR		// 游客
	, EB_ACCOUNT_TYPE_IN_ENT	// 同企业或同群组
	, EB_ACCOUNT_TYPE_OUT_ENT	// 外部成员
	, EB_ACCOUNT_TYPE_USER		// 普通用户
};

/*==========================================================
个人设置
===========================================================*/
typedef enum EB_SETTING_VALUE
{
	EB_SETTING_ENABLE_OUTENT_CALL		= 0x0001
	, EB_SETTING_AUTO_OUTENT_ACCEPT		= 0x0002
	, EB_SETTING_ENABLE_USER_CALL		= 0x0004
	, EB_SETTING_AUTO_USER_ACCEPT		= 0x0008
	, EB_SETTING_ENABLE_VISITOR_CALL	= 0x0010
	, EB_SETTING_AUTO_VISITOR_ACCEPT	= 0x0020
	, EB_SETTING_ENABLE_OFF_CALL		= 0x0040
	, EB_SETTING_ENABLE_OFF_FILE		= 0x0080
	//, EB_SETTING_CONNECTED_OPEN_CHAT	= 0x0100
	, EB_SETTING_AUTO_CONTACT_ACCEPT	= 0x0200
};
//(EB_SETTING_ENABLE_OUTENT_CALL|EB_SETTING_AUTO_OUTENT_ACCEPT|EB_SETTING_ENABLE_USER_CALL|EB_SETTING_AUTO_USER_ACCEPT|EB_SETTING_ENABLE_VISITOR_CALL|EB_SETTING_AUTO_VISITOR_ACCEPT|EB_SETTING_ENABLE_OFF_CALL)
#define EB_SETTING_DEFAULT				0x027F // EB_SETTING_ENABLE_OUTENT_CALL-EB_SETTING_ENABLE_OFF_CALL
#define EB_SETTING_DEFAULT_BASE			EB_SETTING_AUTO_CONTACT_ACCEPT
#define EB_SETTING_DEFAULT_OUT_CALL		EB_SETTING_ENABLE_OUTENT_CALL|EB_SETTING_ENABLE_USER_CALL|EB_SETTING_ENABLE_VISITOR_CALL|EB_SETTING_ENABLE_OFF_CALL
#define EB_SETTING_DEFAULT_OUT_ACCEPT	EB_SETTING_AUTO_OUTENT_ACCEPT|EB_SETTING_AUTO_USER_ACCEPT|EB_SETTING_AUTO_VISITOR_ACCEPT

} // namespace entboost

#endif // __eb_define_h__
