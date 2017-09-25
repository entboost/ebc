// eb_define1.h file here
#ifndef __eb_define1_h__
#define __eb_define1_h__
#include "eb_define.h"

/*==========================================================
消息通知类型
===========================================================*/
typedef enum EB_NOTIFY_TYPE {
	EB_NOTIFY_UNKNOWN
        , EB_NOTIFY_USER_INPUT				// 用户正在输入
	, EB_NOTIFY_UPLOAD_GROUP_FILE			// 用户上传群文件 notify-data<=1024 size
}EB_NOTIFY_TYPE;

/*==========================================================
消息类型
===========================================================*/
typedef enum EB_MSG_TYPE
{
	EB_MSG_UNKNOWN
	, EB_MSG_RICH		// text&image
	, EB_MSG_FILE

	, EB_MSG_DELETE_GROUP			= 0x101		// 解散群
	, EB_MSG_EXIT_GROUP							// 主动退出群
	, EB_MSG_UPDATE_GROUP						// 群资料已经修改，需要重新加载群资料
	, EB_MSG_REMOVE_GROUP						// 被动管理员移出群
	, EB_MSG_CALL_2_GROUP						// 一对一会话转换多人讨论组
	, EB_MSG_ADD_2_GROUP						// 添加进群组(部门)
	, EB_MSG_REQ_ADD_2_GROUP					// 申请进入群组(部门)
	, EB_MSG_REJECT_ADD_2_GROUP					// 拒绝进入群组(部门)
	, EB_MSG_INVITE_ADD_2_GROUP					// 邀请添加进群组(部门)
	, EB_MSG_USER_LINE_STATE		= 0x111		// 用户在线状态通知
	, EB_MSG_ONLINE_ANOTHER						// 通知自己，已经在其他地方登录，退出前一个连接
	, EB_MSG_USER_ONLINE_INCALL					// 用户上线，邀请用户进现有会话
	, EB_MSG_RESET_ACCOUNT						// 重置帐号，包括
	, EB_MSG_USER_HEAD_CHANGE		= 0x121		// 用户修改头像资源
	, EB_MSG_BROADCAST_MESSAGE					// 广播消息
	, EB_MSG_GROUP_MEMBER_CHANGE				// 群成员资料已经修改，需要重新加载群成员资料
	, EB_MSG_ENT_GROUP_INFO_VERSION				// 企业部门资料版本
	, EB_MSG_GROUP_MEMBER_VERSION				// 部门成员资料版本
	, EB_MSG_USER_INFO_CHANGE						// 用户个人资料修改
	, EB_MSG_DELETE_RESOURCE		= 0x131		// 删除在线资源
	, EB_MSG_EMOTION_INFO						// 表情资源信息
	, EB_MSG_SUBSCRIBE_FUNC_INFO				// 订购功能信息
	, EB_MSG_ENTGROUP_VER_INFO					// 企业部门版本信息
	, EB_MSG_MYGROUP_VER_INFO					// 个人群组版本信息
	, EB_MSG_GROUP_MEMBER_LINESTATE				// 群组（部门）成员（或联系人）在线状态
	, EB_MSG_AREA_DICT_INFO						// 地区字典信息
	, EB_MSG_GROUP_LINESTATE_COUNT				// 群组（部门）成员在线人数
	, EB_MSG_EMOTION_COUNT							// 表情资源汇总
	, EB_MSG_CONTACT_HEAD_VER_INFO			// 好友/联系人头像版本信息
	, EB_MSG_REQ_ADD_CONTACT		= 0x141		// 申请好友
	, EB_MSG_ACCEPT_ADD_CONTACT					// 接受加好友
	, EB_MSG_REJECT_ADD_CONTACT					// 拒绝加好友
	, EB_MSG_DELETE_CONTACT							// 删除好友
	, EB_MSG_UPDATE_CONTACT							// 好友资料更新
	, EB_MSG_UPDATE_USER_GROUP					// 更新好友分组
	, EB_MSG_DELETE_USER_GROUP					// 删除好友分组
	, EB_MSG_UPDATE_USER_GROUP2					// 更新好友分组
	, EB_MSG_DELETE_USER_GROUP2					// 删除好友分组
}EB_MSG_TYPE;

/*==========================================================
群组（部门）成员管理权限
===========================================================*/
typedef enum EB_MANAGER_LEVEL
{
	EB_LEVEL_NONE
	, EB_LEVEL_DEP_EDIT			= 0x0001
	, EB_LEVEL_DEP_DELETE		= 0x0002
	, EB_LEVEL_EMP_EDIT			= 0x0004
	, EB_LEVEL_EMP_DELETE		= 0x0008
	, EB_LEVEL_DEP_RES_EDIT		= 0x0010
	, EB_LEVEL_DEP_RES_DELETE	= 0x0020
	, EB_LEVEL_DEP_ADMIN			= EB_LEVEL_DEP_EDIT|EB_LEVEL_EMP_EDIT|EB_LEVEL_EMP_DELETE|EB_LEVEL_DEP_RES_EDIT|EB_LEVEL_DEP_RES_DELETE
	, EB_LEVEL_FORBID_SPEECH	= 0x1000

	//, EB_LEVEL_MGR				= 0x0001
	//, EB_LEVEL_EDIT				= 0x0002
	//, EB_LEVEL_DELETE			= 0x0004
	////, EB_LEVEL_ENT_MGR			= 0x0010
	//, EB_LEVEL_DEP_MGR			= 0x0020
	//, EB_LEVEL_EMP_MGR			= 0x0040
	//, EB_LEVEL_RES_MGR			= 0x0100
	////, EB_LEVEL_ENT_EDIT			= EB_LEVEL_ENT_MGR|EB_LEVEL_EDIT
	//, EB_LEVEL_DEP_EDIT			= EB_LEVEL_DEP_MGR|EB_LEVEL_EDIT
	//, EB_LEVEL_DEP_DELETE		= EB_LEVEL_DEP_MGR|EB_LEVEL_DELETE
	//, EB_LEVEL_EMP_EDIT			= EB_LEVEL_EMP_MGR|EB_LEVEL_EDIT
	//, EB_LEVEL_EMP_DELETE		= EB_LEVEL_EMP_MGR|EB_LEVEL_DELETE
	////, EB_LEVEL_RES_EDIT		= EB_LEVEL_RES_MGR|EB_LEVEL_EDIT	// 个人资源管理不能通过这里实现，否则会跟企业，群组有冲突。
	////, EB_LEVEL_ENT_RES_EDIT		= EB_LEVEL_ENT_MGR|EB_LEVEL_RES_MGR|EB_LEVEL_EDIT
	////, EB_LEVEL_ENT_RES_DELETE	= EB_LEVEL_ENT_MGR|EB_LEVEL_RES_MGR|EB_LEVEL_DELETE
	////, EB_LEVEL_ENT_RES_VIEW		= EB_LEVEL_ENT_MGR|EB_LEVEL_RES_MGR
	//, EB_LEVEL_DEP_RES_EDIT		= EB_LEVEL_DEP_MGR|EB_LEVEL_RES_MGR|EB_LEVEL_EDIT
	//, EB_LEVEL_DEP_RES_DELETE	= EB_LEVEL_DEP_MGR|EB_LEVEL_RES_MGR|EB_LEVEL_DELETE
	////, EB_LEVEL_RES_VIEW		= EB_LEVEL_RES_MGR|EB_LEVEL_MGR
}EB_MANAGER_LEVEL;


///*==========================================================
//ANDROID推送平台
//===========================================================*/
//typedef enum EB_ANDROID_PUSH_TYPE
//{
//	EB_ANDROID_PUSH_TYPE_UNKNOWN			= 0	// 未知类似
//	, EB_ANDROID_PUSH_TYPE_IOS						// IOS推送平台
//	, EB_ANDROID_PUSH_TYPE_HUAWEI					// 华为推送平台
//	, EB_ANDROID_PUSH_TYPE_XIAOMI					// 小米推送平台
//
//};

#define EB_PUSH_SSLID_HUAWEI_MIN 100	// 华为推送 100-199
#define EB_PUSH_SSLID_HUAWEI_MAX 199
#define EB_PUSH_SSLID_XIAOMI_MIN 200	// 小米推送 200-299
#define EB_PUSH_SSLID_XIAOMI_MAX 299
#define EB_PUSH_SSLID_IOS_DEV 7606616803963781LL	// IOS 开发环境
#define EB_PUSH_SSLID_IOS_RES 7715636013842198LL	// IOS 正式环境

/*==========================================================
登录类型
===========================================================*/
typedef enum EB_PUSH_SSLID_TYPE
{
	EB_PUSH_SSLID_TYPE_UNKNOWN								= 0x0		// 未知类型（私有云默认推送）
	, EB_PUSH_SSLID_TYPE_USES_ENTBOOST_PUSH		= 0x01	// 通过恩布平台推送
	, EB_PUSH_SSLID_TYPE_SUPPORT_OTHER_PHONE	= 0x10	// 支持推送其他品牌手机
}EB_PUSH_SSLID_TYPE;

/*==========================================================
登录类型
===========================================================*/
typedef enum EB_UM_LOAD2_TYPE
{
	EB_UM_LOAD2_TYPE_UNKNOWN								= 0
	, EB_UM_LOAD2_TYPE_CONTACT_HEAD_VER						// 1 加载好友联系人头像版本
	//, EB_UM_LOAD2_TYPE_GROUP_FORBID_INFO					// 2 加载群组禁言状态 -1:解除禁言 0:永久禁言 >0禁言分钟数
	//, EB_UM_LOAD2_TYPE_MEMBER_FORBID_INFO					// 3 加载群成员禁言状态 -1:解除禁言 0:永久禁言 >0禁言分钟数
}EB_UM_LOAD2_TYPE;

/*==========================================================
消息响应类型
===========================================================*/
typedef enum EB_MSG_ACK_TYPE
{
	EB_MAT_SUCCESS											// 0 成功
	, EB_MAT_ERROR											// 1 错误
	, EB_MAT_CANCEL											// 2 取消（或拒绝）
	, EB_MAT_REQUEST										// 3 请求
	, EB_MAT_WITHDRAW										// 4 撤回消息
	//, EB_MAT_DELETE										// 5 （保留未用）
	, EB_MAT_SELF_COLLECT					= 6		// 6 个人收藏
	, EB_MAT_GROUP_COLLECT							// 7 群收藏
	, EB_MAT_SAVE2CLOUDDRIVE	= 10			// 10 存入云盘
	, EB_MAT_RESEND											// 11 重新发送
}EB_MSG_ACK_TYPE;

/*==========================================================
消息聊天类型
===========================================================*/
typedef enum EB_MSG_CHAT_TYPE
{
	EB_MSG_CHAT_TYPE_UNKNOWN
	, EB_MSG_CHAT_TYPE_USER
	, EB_MSG_CHAT_TYPE_GROUP
}EB_MSG_CHAT_TYPE;
/*==========================================================
收藏数据类型
===========================================================*/
typedef enum EB_COLLECT_FROM_TYPE
{
	EB_COLLECT_FROM_TYPE_UNKNOWN
	, EB_COLLECT_FROM_TYPE_USER
	, EB_COLLECT_FROM_TYPE_GROUP
}EB_COLLECT_FROM_TYPE;


/*==========================================================
数据流响应类型
===========================================================*/
typedef enum EB_DATASTREAM_ACK_TYPE
{
	EB_DSAT_UNKNOWN
	, EB_DSAT_OK
	, EB_DSAT_REQUEST
}EB_DATASTREAM_ACK_TYPE;

/*==========================================================
会话响应类型
===========================================================*/
typedef enum EB_CALL_ACK_TYPE
{
	EB_CAT_UNKNOWN
	, EB_CAT_ACCEPT
	, EB_CAT_REJECT
	, EB_CAT_OFF_ACCEPT
	, EB_CAT_TIMEOUT
	, EB_CAT_DELETE
}EB_CALL_ACK_TYPE;

#define OFFLINE_SESSION_CLOSE	1
#define OFFLINE_USES_CLOSE		2
#define OFFLINE_SERVER_CLOSE	3

/*==========================================================
SOTP SIGN：
===========================================================*/
typedef enum EB_CALL_SIGN
{
	EB_SIGN_UNKNOWN
	//, EB_CS_CS_ONLINE		= 0x001
	//, EB_CS_CS_OFFLINE
	//, EB_CS_CS_ACTIVE
	//, EB_CS_CS_QUERY
	//, EB_CS_CS_LOAD
	// LC
	, EB_SIGN_S_ONLINE		= 0x101		// 257 *服务端组件登记上线
	, EB_SIGN_S_OFFLINE							// 258 *服务端组件注销下线
	, EB_SIGN_S_ACTIVE							// 259
	, EB_SIGN_S_QUERY								// 260
	, EB_SIGN_L_LOGON								// 261 登录验证，包括APPID验证，用户验证等
	, EB_SIGN_L_LOGOUT							// 262 注销退出
	, EB_SIGN_L_QUERY								// 263 用户查询
	, EB_SIGN_L_REGAUTH		= 0x111		// 273 注册验证
	, EB_SIGN_L_FINDPWD							// 274 请求找回密码
	, EB_SIGN_L_RESETPWD						// 275 重置密码
	, EB_SIGN_Q_INFO			= 0x121		// 289 *服务端组件查询数据
	, EB_SIGN_P_INFO								// 290 *服务端组件设置数据1
	, EB_SIGN_SP_INFO								// 291 *服务端组件查询数据2
	// UM
	, EB_SIGN_U_REG				= 0x201		// 513 注册帐号
	, EB_SIGN_U_SINFO								// 514 
	, EB_SIGN_U_SHEAD
	, EB_SIGN_U_ONLINE
	, EB_SIGN_U_LOAD
	, EB_SIGN_U_OFFLINE
	, EB_SIGN_U_QUERY
	, EB_SIGN_U_MGR
	, EB_SIGN_U_GINFO
	, EB_SIGN_U_SPUSH
	, EB_SIGN_U_LOAD2
	, EB_SIGN_V_REQUEST			= 0x211	// 529 请求视频会话
	, EB_SIGN_FV_REQUEST
	, EB_SIGN_V_ACK
	, EB_SIGN_FV_ACK
	, EB_SIGN_V_END
	, EB_SIGN_FV_END
	, EB_SIGN_RD_REQUEST		= 0x218	// 536 请求远程协助/远程桌面
	, EB_SIGN_FRD_REQUEST
	, EB_SIGN_RD_ACK
	, EB_SIGN_FRD_ACK
	, EB_SIGN_RD_END
	, EB_SIGN_FRD_END

	, EB_SIGN_C_CALL			= 0x221		// 545 请求发起会话
	, EB_SIGN_C_ENTER
	, EB_SIGN_FC_CALL
	, EB_SIGN_FC_ENTER
	, EB_SIGN_C_ACK
	, EB_SIGN_FC_ACK
	, EB_SIGN_C_HANGUP
	, EB_SIGN_FC_HANGUP
	, EB_SIGN_U_MSG
	, EB_SIGN_FU_MSG
	, EB_SIGN_U_MACK

	, EB_SIGN_AB_EDIT			= 0x231		// 561 新建或修改联系人资料
	, EB_SIGN_AB_DEL								// 562 删除联系人资料
	, EB_SIGN_AB_LOAD								// 563 查询加载联系人资料
	, EB_SIGN_UG_EDIT								// 564 新建或修改好友分组
	, EB_SIGN_UG_DEL								// 565 删除好友分组
	, EB_SIGN_UG_LOAD								// 566 查询加载好友分组
	, EB_SIGN_ENT_EDIT		= 0x241		// 577 修改企业资料
	, EB_SIGN_DEP_EDIT							// 578 新建或修改部门资料
	, EB_SIGN_DEP_DEL								// 579 删除部门资料
	, EB_SIGN_EMP_EDIT							// 580 新建或修改员工资料
	, EB_SIGN_EMP_DEL								// 581 删除员工资料
	, EB_SIGN_ENT_LOAD							// 582 查询加载组织结构，部门，员工等资料
	, EB_SIGN_FENT_INFO							// 583 下行企业资料
	, EB_SIGN_FDEP_INFO							// 584 下行部门资料
	, EB_SIGN_FEMP_INFO							// 585 下行员工资料
	, EB_SIGN_DEP_GET			= 0x24E		// 590 查询获取部门资料
	, EB_SIGN_EMP_GET								// 591 查询获取员工资料
	, EB_SIGN_R_EDIT		= 0x251			// 593 新建或修改云盘资源
	, EB_SIGN_R_DEL									// 594 删除云盘资源
	, EB_SIGN_R_LOAD								// 595 删除云盘资源
	, EB_SIGN_R_INFO								// 596 删除云盘资源
	, EB_SIGN_CS_LOAD		= 0x261
	, EB_SIGN_CS_ADD
	, EB_SIGN_CS_DEL
	, EB_SIGN_FUNC_REQ		= 0x271
	, EB_SIGN_FUNC_AUTH
	, EB_SIGN_FUNC_LOAD
	, EB_SIGN_FUNC_EDIT
	, EB_SIGN_FUNC_DEL
	, EB_SIGN_FUNC_SUB
	, EB_SIGN_FUNC_SETICON
	, EB_SIGN_FNAV_SET
	, EB_SIGN_FNAV_DEL
	, EB_SIGN_FNAV_LOAD

	, EB_SIGN_VER_CHECK		= 0x281
	, EB_SIGN_DICT_LOAD		= 0x291

	// CM
	, EB_SIGN_CM_ENTER		= 0x301
	, EB_SIGN_FCM_ENTER
	, EB_SIGN_CM_EXIT
	, EB_SIGN_FCM_EXIT
	, EB_SIGN_CM_ACTIVE
	, EB_SIGN_CM_MSG
	, EB_SIGN_FCM_MSG
	, EB_SIGN_CM_MACK
	, EB_SIGN_FCM_MACK
	, EB_SIGN_DS_SEND
	, EB_SIGN_FDS_SEND
	, EB_SIGN_DS_CHECK
	, EB_SIGN_DS_ACK
	, EB_SIGN_CM_NOTIFY
	, EB_SIGN_FCM_NOTIFY
	, EB_SIGN_P2P_REQUEST	= 0x311
	, EB_SIGN_P2P_RESPONSE
	, EB_SIGN_FP2P_REQUEST
	, EB_SIGN_FP2P_RESPONSE
	, EB_SIGN_P2P_TRY
	//, EB_SIGN_CR_GET
	, EB_SIGN_PS_SEND	= 0x321
	, EB_SIGN_RTP_ON		= 0x331
	, EB_SIGN_RTP_OFF

	, EB_SIGN_A_ON		= 0x401
	, EB_SIGN_A_OFF
	, EB_SIGN_A_MSG
	, EB_SIGN_FA_MSG
	, EB_SIGN_A_MACK
	, EB_SIGN_FA_MACK
	, EB_SIGN_AU_MSG
	, EB_SIGN_AU_MAIL
	, EB_SIGN_EXE_JOB
	, EB_SIGN_SYNC_EDITPWD	= 0x411
	, EB_SIGN_SYNC_USERINFO
	, EB_SIGN_SYS_EDIT

}EB_CALL_SIGN;

/*==========================================================
系统常量
===========================================================*/
const int	EB_MAX_REQUEST_OS_COUNT	= 140;		// 一次最多请求数据补偿数据包

///////////////////
#define POP_APP_NAME_CENTER_SERVER		"POPCenterServer"
#define POP_APP_NAME_LOGON_CENTER		"POPLogonCenter"
#define POP_APP_NAME_USER_MANAGER		"POPUserManager"
#define POP_APP_NAME_CHAT_MANAGER		"POPChatManager"
#define POP_APP_NAME_APP_CENTER			"POPAppCenter"
#define EB_APP_NAME_FILE_SERVER			"ebsemail"
#define EB_APP_NAME_RD_SERVER			"ebrds"
#define EB_APP_NAME_MMV_SERVER			"ebmmvs"
#define EB_APP_NAME_MMA_SERVER			"ebmmas"
#define EB_APP_NAME_LOGON_CENTER		"eblc"

//#define EB_CALL_NAME_CS_ONLINE			"pop_cs_online"
//#define EB_CALL_NAME_CS_OFFLINE		"pop_cs_offline"
//#define EB_CALL_NAME_CS_ACTIVE			"pop_cs_active"
//#define EB_CALL_NAME_CS_QUERY			"pop_cs_query"
//#define EB_CALL_NAME_CS_LOAD			"pop_cs_load"

#define EB_CALL_NAME_S_ONLINE			"eb_s_online"
#define EB_CALL_NAME_S_OFFLINE			"eb_s_offline"
#define EB_CALL_NAME_S_ACTIVE			"eb_s_active"
#define EB_CALL_NAME_S_QUERY			"eb_s_query"
#define EB_CALL_NAME_LC_LOGON			"eb_l_logon"
#define EB_CALL_NAME_LC_LOGOUT			"eb_l_logout"
#define EB_CALL_NAME_LC_QUERY			"eb_l_query"
#define EB_CALL_NAME_LC_REGAUTH			"eb_l_regauth"
#define EB_CALL_NAME_LC_FINDPWD			"eb_l_findpwd"
#define EB_CALL_NAME_LC_RESETPWD		"eb_l_resetpwd"
#define EB_CALL_NAME_Q_INFO				"eb_q_info"
#define EB_CALL_NAME_P_INFO				"eb_p_info"
#define EB_CALL_NAME_SP_INFO			"eb_sp_info"

#define EB_CALL_NAME_RD_REQUEST			"eb_rd_request"
#define EB_CALL_NAME_RD_ACK				"eb_rd_ack"
#define EB_CALL_NAME_RD_END				"eb_rd_end"
#define EB_CALL_NAME_V_REQUEST			"eb_v_request"
#define EB_CALL_NAME_V_ACK				"eb_v_ack"
#define EB_CALL_NAME_V_END				"eb_v_end"
#define EB_CALL_NAME_UM_REG				"eb_u_reg"
#define EB_CALL_NAME_UM_QUERY			"eb_u_query"
#define EB_CALL_NAME_UM_SINFO			"eb_u_sinfo"
#define EB_CALL_NAME_UM_GINFO			"eb_u_ginfo"
#define EB_CALL_NAME_UM_MGR				"eb_u_mgr"
#define EB_CALL_NAME_UM_SHEAD			"eb_u_shead"
#define EB_CALL_NAME_UM_SPUSH			"eb_u_spush"
#define EB_CALL_NAME_UM_ONLINE			"eb_u_online"
#define EB_CALL_NAME_UM_LOAD			"eb_u_load"
#define EB_CALL_NAME_UM_LOAD2			"eb_u_load2"
#define EB_CALL_NAME_UM_OFFLINE			"eb_u_offline"
//#define EB_CALL_NAME_UMIU_GROUP		"eb_umiu_group"
#define EB_CALL_NAME_UM_MSG				"eb_u_msg"
#define EB_CALL_NAME_UM_MACK			"eb_u_mack"
#define EB_CALL_NAME_UM_CALL			"eb_c_call"
#define EB_CALL_NAME_C_ENTER			"eb_c_enter"
#define EB_CALL_NAME_UM_CACK			"eb_c_ack"
#define EB_CALL_NAME_UM_HANGUP			"eb_c_hangup"

#define EB_CALL_NAME_AB_EDIT			"eb_ab_edit"
#define EB_CALL_NAME_AB_DEL				"eb_ab_del"
#define EB_CALL_NAME_AB_LOAD			"eb_ab_load"
#define EB_CALL_NAME_UG_EDIT			"eb_ug_edit"
#define EB_CALL_NAME_UG_DEL				"eb_ug_del"
#define EB_CALL_NAME_UG_LOAD			"eb_ug_load"

#define EB_CALL_NAME_R_EDIT				"eb_r_edit"
#define EB_CALL_NAME_R_DEL				"eb_r_del"
#define EB_CALL_NAME_R_LOAD				"eb_r_load"

#define EB_CALL_NAME_ENT_EDIT			"eb_ent_edit"
#define EB_CALL_NAME_DEP_EDIT			"eb_dep_edit"
#define EB_CALL_NAME_DEP_DEL			"eb_dep_del"
#define EB_CALL_NAME_DEP_OP				"eb_dep_op"
#define EB_CALL_NAME_EMP_EDIT			"eb_emp_edit"
#define EB_CALL_NAME_EMP_DEL			"eb_emp_del"
#define EB_CALL_NAME_ENT_LOAD			"eb_ent_load"

#define EB_CALL_NAME_FUNC_REQ			"eb_func_req"
#define EB_CALL_NAME_FUNC_AUTH			"eb_func_auth"
#define EB_CALL_NAME_FUNC_LOAD			"eb_func_load"
#define EB_CALL_NAME_FUNC_EDIT			"eb_func_edit"
#define EB_CALL_NAME_FUNC_SETICON		"eb_func_seticon"
#define EB_CALL_NAME_FUNC_DEL			"eb_func_del"
#define EB_CALL_NAME_FUNC_SUB			"eb_func_sub"
#define EB_CALL_NAME_FNAV_SET			"eb_fnav_set"
#define EB_CALL_NAME_FNAV_DEL			"eb_fnav_del"
#define EB_CALL_NAME_FNAV_LOAD			"eb_fnav_load"

#define EB_CALL_NAME_CS_ADD				"eb_cs_add"
#define EB_CALL_NAME_CS_DEL				"eb_cs_del"

#define EB_CALL_NAME_VER_CHECK			"eb_ver_check"
#define EB_CALL_NAME_DICT_LOAD			"eb_dict_load"

//#define EB_CALL_NAME_CR_SET				"eb_cr_set"
//#define EB_CALL_NAME_CR_GET				"eb_cr_get"
#define EB_CALL_NAME_CM_ENTER			"eb_cm_enter"
#define EB_CALL_NAME_CM_EXIT			"eb_cm_exit"
#define EB_CALL_NAME_CM_NOTIFY			"eb_cm_notify"
#define EB_CALL_NAME_CM_ACTIVE			"eb_cm_active"
#define EB_CALL_NAME_CM_MSG				"eb_cm_msg"
#define EB_CALL_NAME_CM_QUERY			"eb_cm_query"
#define EB_CALL_NAME_CM_MACK			"eb_cm_mack"
#define EB_CALL_NAME_DS_SEND			"eb_ds_send"
#define EB_CALL_NAME_DS_CHECK			"eb_ds_check"
#define EB_CALL_NAME_DS_ACK				"eb_ds_ack"
#define EB_CALL_NAME_P2P_REQ			"eb_p2p_req"
#define EB_CALL_NAME_P2P_RES			"eb_p2p_res"
#define EB_CALL_NAME_P2P_TRY			"~eb_p2p_try"
#define EB_CALL_NAME_PS_SEND			"eb_ps_send"

#define EB_CALL_NAME_RTP_ON				"eb_rtp_on"
#define EB_CALL_NAME_RTP_OFF			"eb_rtp_off"

#define EB_CALL_NAME_A_ON				"eb_a_on"
#define EB_CALL_NAME_A_OFF				"eb_a_off"
#define EB_CALL_NAME_A_MSG				"eb_a_msg"
#define EB_CALL_NAME_A_MACK				"eb_a_mack"
#define EB_CALL_NAME_AU_MSG				"eb_au_msg"
#define EB_CALL_NAME_AU_MAIL			"eb_au_mail"
#define EB_CALL_NAME_EXE_JOB			"eb_exe_job"
#define EB_CALL_NAME_SYNC_EDITPWD		"eb_sync_editpwd"
#define EB_CALL_NAME_SYNC_USERINFO		"eb_sync_userinfo"
#define EB_CALL_NAME_SYS_EDIT			"eb_sys_edit"

#endif // __eb_define1_h__
