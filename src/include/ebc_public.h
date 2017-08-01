// ebc_public.h file here
#ifndef __ebc_public_h__
#define __ebc_public_h__
#ifdef _MSC_VER
#pragma warning(disable:4305)
#endif // _MSC_VER

#ifdef _QT_MAKE_
#include <QObject>
#include <QImage>
#include <QEvent>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#define EBC_WM_USER QEvent::User
//typedef void * HTREEITEM;
#else
#define EBC_WM_USER WM_USER
#endif

#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace mycp;

#ifndef CEBString
#define CEBString cgcSmartString
#endif
//#include "ebstring.h"
//#include "eb_object.h"
extern std::wstring theFontFamily;	// "Times New Roman"
extern tstring	theFontFace;	// "Times New Roman"
const int theDefaultDialogFontSize = 90;
const std::wstring theFontFamily0 = L"微软雅黑";
const std::wstring theFontFamily1 = L"宋体";
const int theMaxDatabaseTextSize = 6000;
const short theArcOffset = 6;		// 聊天边框圆弧
const short thePoloygonWidth = 10;	// 12
const short thePoloygonHeight = 13;	// 16
const int theColorSkinSize = 10;
const int theDefaultBtnRightIntever = 18;
const int theDefaultBtnBottomIntever = 10;
const int theDefaultBtnWidth = 81;
const int theDefaultBtnHeight = 32;
const int theDefaultBtnIntever = 10;

#ifndef WIN32
#ifndef COLORREF
typedef unsigned int   COLORREF;
#endif
#endif

#ifndef RGB
#define RGB(r,g,b)          ((COLORREF)(((unsigned char)(r)|((unsigned int)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))
#endif
const COLORREF theDefaultButtonColor			= RGB(86,190,245);
const COLORREF theDefaultFlatBgColor			= RGB(250,250,250);
const COLORREF theDefaultFlatBg2Color			= RGB(244,244,244);
//const COLORREF theDefaultFlatBottomColor		= RGB(244,247,252);
const COLORREF theDefaultFlatLineColor			= RGB(224,224,224);
const COLORREF theDefaultFlatLine2Color			= RGB(192,192,192);
const COLORREF theDefaultFlatBlackText2Color	= RGB(96,96,96);
const COLORREF theDefaultFlatBlackTextColor		= RGB(64,64,64);	// 128
const COLORREF theDefaultTextBlackColor			= RGB(32,32,32);	// RGB(38,38,38);		// text
const COLORREF theDefaultTextWhiteColor			= RGB(252,252,252);		// text
const COLORREF theDefaultBtnWhiteColor			= RGB(255,255,255);	// RGB(246,248,250);		// button
const COLORREF theDefaultBtnCloseColor			= RGB(237,28,36);
const float    theDefaultBtnNormal				= 0.0;					// 按钮正常颜色
const float    theDefaultBtnHot					= 0.15;					// 比正常颜色亮
const float    theDefaultBtnPre					= -0.07;				// 比正常颜色深
const float    theDefaultBtnTrue				= -0.1;					// 比正常颜色更深
const float    theDefaultMaxBtnHot				= 0.8;					// 亮-最大亮度
const float    theDefaultMaxBtnPre				= 0.6;					// 深-最大亮度
const float    theDefaultMaxBtnTrue				= 0.5;					// 更深-最大亮度

//const float    theDefaultBtnOkColorLight0		= 0.8;					// 确定，保存按钮颜色亮度
//const float    theDefaultBtnOkColorLight1		= 0.72;					// 确定，保存按钮颜色亮度
//const float    theDefaultBtnHotColorLight1		= 0.65;					// 按钮移动颜色亮度1
//const float    theDefaultBtnHotColorLight2		= 0.5;					// 按钮移动颜色亮度2（深色）
////const float    theDefaultBtnPreColorLight		= 0.8;					// 按钮按下颜色亮度
//const float	   theDefaultColorTureLight1		= 0.5;					// 亮度－最深
//const float	   theDefaultColorNormalLight		= 0.8;					// 亮度－普通
const int	   theDefaultBgColorTitleHeight		= 42;					// 标题高度
//const float	   theDefaultBgColorTitleLight0		= 0.8;					// 背景颜色亮度－标题1
//const float	   theDefaultBgColorTitleLight1		= 0.7;					// 背景颜色亮度－标题2（深一点）
////const float	   theDefaultBgColorBgLight3		= 0.85;					// 背景颜色亮度－最下面
//const float    theDefaultBgColorBgLight0		= 0.99;					// 背景颜色亮度－背景底色
//const float    theDefaultBgColorBgLight1		= 0.98;					// 背景颜色亮度－背景底色 普通
//const float    theDefaultBgColorBgLight2		= 0.96;					// 背景颜色亮度－背景底色（深一点）
//const float    theDefaultBgColorBgLight3		= 0.88;					// 背景颜色亮度－背景底色（更深，用于渐变）

const COLORREF theDefaultChatTextColor1			= RGB(64, 64, 64);		// my text
const COLORREF theDefaultChatTextColor2			= RGB(64, 64, 64);		// user text
const COLORREF theDefaultChatSystemColor		= RGB(128, 128, 128);	// system
const COLORREF theDefaultChatTitleColor1		= RGB(0, 110, 254);		// my title
const COLORREF theDefaultChatTitleColor2		= RGB(66, 180, 117);	// user title
const COLORREF theDefaultChatBackGroundColor1	= RGB(183,253,159);		// my
const COLORREF theDefaultChatBackGroundColor2	= RGB(255,255,255);		// user
const COLORREF theColorSkinsValue[] = {RGB(163,73,164),RGB(63,72,204),RGB(0,162,232),RGB(34,177,76),RGB(185,122,87),RGB(255,127,39),RGB(237,28,36),RGB(255,174,201),RGB(112,146,190),RGB(96,96,96)};
const std::string theColorSkinsString[] = {"紫色","靛青","蓝色","绿色","褐色","橙色","红色","玫瑰色","蓝灰色","深灰色"};
//const COLORREF theColorSkinsValue[] = {RGB(218,219,223),RGB(86,190,245),RGB(1,103,204),RGB(162,237,119),RGB(255,111,128),RGB(244,236,138),RGB(208,177,94),RGB(246,176,80),RGB(193,195,194),RGB(162,161,167)};
//const CString theColorSkinsString[] = {_T("白色"),_T("蓝色"),_T("深蓝"),_T("绿色"),_T("红色"),_T("黄色"),_T("金色"),_T("橙色"),_T("银色"),_T("黑色")};
// 灰色 = 240,240,240
// 蓝色 = 23,167,237
static std::string theDayOfWeek[] = {"周日","周一","周二","周三","周四","周五","周六"};
static char theAllFilesFilter[] = "All Files (*.*)|*.*||";

//#ifndef bigint
////typedef __int64				bigint;
//typedef mycp::bigint        bigint;
//#endif
const double const_kb_size = 1024.0;
const double const_mb_size = 1024*const_kb_size;
const double const_gb_size = 1024*const_mb_size;

#define MAKEINT64(lo, hi)    ((INT64)(((DWORD)(((INT64)(lo)) & 0xffffffff)) | ((INT64)((DWORD)(((INT64)(hi)) & 0xffffffff))) << 32))
#define LOINT64(l)         ((DWORD)(((INT64)(l)) & 0xffffffff))
#define HIINT64(l)         ((DWORD)((((INT64)(l)) >> 32) & 0xffffffff))

#define EBC_MSG_TYPE_REQUEST_JOIN_2_GROUP	1
#define EBC_MSG_TYPE_INVITE_JOIN_2_GROUP	2
#define EBC_MSG_TYPE_REQUEST_ADD_CONTACT	3
#define EBC_MSG_TYPE_ACCERPT_ADD_CONTACT	4
#define EBC_MSG_TYPE_REJECT_ADD_CONTACT		5
#define EBC_MSG_TYPE_BC_MSG					10

#define EBC_READ_FLAG_TRUE				0x1	// 消息已读标识
#define EBC_READ_FLAG_RECEIPT			0x2	// 对方已接收消息回执
#define EBC_READ_FLAG_SENT				0x4	// 发送成功（文件）
#define EBC_READ_FLAG_WITHDRAW		0x8	// 请求撤回消息

#define EBC_CONTRON_RECEIPT_FLAG_HIDE 0x0		// 隐藏消息回执标识
#define EBC_CONTRON_RECEIPT_FLAG_TRUE 0x1		// 对方已接收消息回执
#define EBC_CONTRON_RECEIPT_FLAG_SHOW 0x2		// 显示消息回执标识
#define EBC_CONTRON_PARAMETER_DISABLE_RECEIPT 1

typedef enum EB_MR_CTRL_TYPE {
	EB_MR_CTRL_TYPE_NONE				= 0x0
	, EB_MR_CTRL_TYPE_LCLICK_CB			= 0x1
	, EB_MR_CTRL_TYPE_LDBLCLICK_CB		= 0x2
	, EB_MR_CTRL_TYPE_RCLICK_CB			= 0x4
	, EB_MR_CTRL_TYPE_LCLICK_OPEN		= 0x10
	, EB_MR_CTRL_TYPE_LDBLCLICK_OPEN	= 0x20
	, EB_MR_CTRL_TYPE_MOVE_ENTER_CB		= 0x100
	, EB_MR_CTRL_TYPE_MOVE_LEAVE_CB		= 0x200
}EB_MR_CTRL_TYPE;

typedef enum EB_MR_CTRL_DATA_TYPE {
	EB_MR_CTRL_DATA_TYPE_URL
	, EB_MR_CTRL_DATA_TYPE_UID	// call user
	, EB_MR_CTRL_DATA_TYPE_IMAGE
	, EB_MR_CTRL_DATA_TYPE_FILE
	, EB_MR_CTRL_DATA_TYPE_SELECT_STRING
	, EB_MR_CTRL_DATA_TYPE_READ_FLAG
	, EB_MR_CTRL_DATA_TYPE_DOWNLOAD_RESOURCE
	, EB_MR_CTRL_DATA_TYPE_OPEN_SHARE
	, EB_MR_CTRL_DATA_TYPE_USER_ECARD
	//, EB_MR_CTRL_DATA_TYPE_DELETE_MSGID
	//, EB_MR_CTRL_DATA_TYPE_SEND_MSGID
}EB_MR_CTRL_DATA_TYPE;

typedef enum EB_BROWSER_TYPE {
	EB_BROWSER_TYPE_CEF		// default
	, EB_BROWSER_TYPE_IE
}EB_BROWSER_TYPE;

typedef enum EB_UI_STYLE_TYPE {
	EB_UI_STYLE_TYPE_OFFICE			// 
	, EB_UI_STYLE_TYPE_CHAT
}EB_UI_STYLE_TYPE;

typedef enum EB_CONTACT_TYPE {
	EB_CONTACT_TYPE_COMMON			= 0x0		// 普通联系人
	, EB_CONTACT_TYPE_AUTH			= 0x01	// 通过验证（好友）
	, EB_CONTACT_TYPE_MAIL			= 0x10	// 邮件联系人
	, EB_CONTACT_TYPE_WHITE			= 0x20	// 白名单
	, EB_CONTACT_TYPE_BLACK			= 0x40	// 黑名单
	, EB_CONTACT_TYPE_WASTE			= 0x80	// 垃圾箱名称
}EB_CONTACT_TYPE;

typedef enum EB_COMMAND_ID {
    EB_COMMAND_LOGOUT			= EBC_WM_USER+0x5100
    , EB_COMMAND_EXIT_APP
	, EB_MSG_EXIT_CHAT
#ifdef _QT_MAKE_
    , EB_COMMAND_SYS_MIN
    , EB_COMMAND_SYS_MAX
    , EB_COMMAND_SYS_CLOSE
#endif
    , EB_COMMAND_COPY
	, EB_COMMAND_SELECTED_IMAGE
	//, EB_MSG_SELECTED_IMAGE
	, EB_MSG_EBSC
	, EB_MSG_EBSC_OK
	, EB_MSG_VIEW_MSG_RECORD
//    , EB_COMMAND_UPDATE_PRODUCT_NAME
//    , EB_COMMAND_UPDATE_ENT_LOGO
    , EB_COMMAND_VIEW_GROUP_SHARE
	, EB_COMMAND_DOWNLOAD_RESOURCE
	, EB_COMMAND_DELETE_MSG_RECORD
	, EB_COMMAND_FILE_MANAGER
	, EB_COMMAND_SHOWHIDE_MAIN
	, EB_COMMAND_SHOWHEAD_ONLY
	, EB_COMMAND_OPEN_MAIN
	, EB_COMMAND_OPEN_WORKFRAME
	, EB_COMMAND_CLOSE_TAB
	, EB_COMMAND_MY_SETTING
	, EB_COMMAND_AUTO_LOGIN
	, EB_COMMAND_MY_SHARE
	, EB_COMMAND_MY_COLLECTION
	, EB_COMMAND_SYS_SETTING
	, EB_COMMAND_DELETE_ACCOUNT
	, EB_COMMAND_OPEN_APP_URL
	//, EB_COMMAND_SHELLEXECUTE
	, EB_COMMAND_CHANGE_APP_URL
	, EB_COMMAND_CHANGE_HEAD
	, EB_COMMAND_REDRAW_APPFRAME
	//, EB_COMMAND_REFRESH_URL
	, EB_COMMAND_REFRESH_OR_STOP_URL
	, EB_COMMAND_SHOW_REFRESH_OR_STOP
	//, EB_COMMAND_QUERY_IS_LOADING
	, EB_COMMAND_GO_BACK
	, EB_COMMAND_GO_FORWARD
	, EB_COMMAND_RETURN_MAINFRAME
	, EB_COMMAND_RAME_WND_MIN
	, EB_COMMAND_RAME_WND_MAX
	, EB_COMMAND_RAME_WND_CLOSE
	, EB_COMMAND_RAME_WND_OPEN
	, EB_COMMAND_RAME_WND_TITLE
	, EB_COMMAND_RAME_WND_COUNT
	, EB_COMMAND_RAME_WND_VIEW_ICO
	, EB_COMMAND_RAME_WND_MOVE_UP
	, EB_COMMAND_RAME_WND_MOVE_DOWN
	, EB_COMMAND_RAME_WND_MOVE_OFFSET
	, EB_COMMAND_RAME_WND_ADJUST_WIDTH
	, EB_COMMAND_SEARCH_SET_FOCUS_SEL
	, EB_COMMAND_CLEAR_SUBID_UNREAD_MSG

	, EB_MSG_SELECTED_EMP
	, EB_COMMAND_NEW_DEPARTMENT1	// 部门
	, EB_COMMAND_NEW_DEPARTMENT2	// 项目组
	, EB_COMMAND_NEW_DEPARTMENT3	// 个人群组
	, EB_COMMAND_EDIT_DEPARTMENT
	, EB_COMMAND_DELETE_DEPARTMENT
	, EB_COMMAND_EXIT_DEPARTMENT
	, EB_COMMAND_EDIT_ENTERPRISE
	, EB_COMMAND_IMPORT_ENT_DATA
	, EB_COMMAND_DEPARTMENT_DEL_EMP
	, EB_COMMAND_DEPARTMENT_ADD_EMP
#ifdef _QT_MAKE_
    , EB_COMMAND_DEPARTMENT_EDIT_MY_EMP
    , EB_COMMAND_DEPARTMENT_EDIT_USER_EMP
#else
    , EB_COMMAND_DEPARTMENT_EDIT_EMP
#endif
	, EB_COMMAND_DEPARTMENT_REFRESH
	, EB_COMMAND_MEMBER_ADD_ADMIN
	, EB_COMMAND_MEMBER_DEL_ADMIN
	, EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_10		// 禁言10分钟
	, EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_60		// 禁言1小时
	, EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_720		// 禁言12小时
	, EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_1440	// 禁言1天＝24小时
	, EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_0			// 永久禁言
	, EB_COMMAND_MEMBER_DEL_FORBID_SPEECH
	, EB_COMMAND_MEMBER_ADD_GROUP_FORBID_SPEECH		// 群禁言
	, EB_COMMAND_MEMBER_DEL_GROUP_FORBID_SPEECH		// 解除群禁言

	, EB_COMMAND_NEW_GROUP
	, EB_COMMAND_DELETE_GROUP
	, EB_COMMAND_RENAME_GROUP
	, EB_COMMAND_NEW_CONTACT
	, EB_COMMAND_EDIT_CONTACT
	, EB_COMMAND_DELETE_CONTACT
	, EB_COMMAND_CALL_USER
	, EB_COMMAND_SEND_USER_ECARD

	, EB_COMMAND_DELETE_SESSION
	, EB_COMMAND_CLEAR_SESSION
	, EB_COMMAND_CLEAR_CHROME_TEMPFILE
	, EB_COMMAND_CLEAR_IE_TEMPFILE
    , EB_COMMAND_SAVE_HISTORY
    , EB_COMMAND_CLEAR_HISTORY
	, EB_COMMAND_SEARCH_BAIDU
	, EB_COMMAND_SEARCH_SOGOU
	, EB_COMMAND_SEARCH_360
    , EB_COMMAND_NEW_DIR_RES	= EBC_WM_USER+0x5250
	, EB_COMMAND_DELETE_DIR_RES
	, EB_COMMAND_EDIT_DIR_RES
	, EB_COMMAND_NEW_SUBDIR_RES
	, EB_COMMAND_NEW_FILE_RES
	, EB_COMMAND_NEW_NOTE_RES
	, EB_COMMAND_DELETE_ITEM_RES
	, EB_COMMAND_EDIT_ITEM_RES
	, EB_COMMAND_DOWNLOAD_FILE
	, EB_COMMAND_ONLINE_VIEW
    , EB_COMMAND_MOVE_2_DIR0	= EBC_WM_USER+0x5270
    , EB_COMMAND_MOVE_2_DIRX	= EBC_WM_USER+0x5290
    , EB_COMMAND_STATE_ONLINE	= EBC_WM_USER+0x5300
	, EB_COMMAND_STATE_AWAY
	, EB_COMMAND_STATE_BUSY
    , EB_COMMAND_MY_EMPLOYEE	= EBC_WM_USER+0x5400	// 后面至少留50个
    , EB_COMMAND_SUBSCRIBE_FUNC	= EBC_WM_USER+0x5450	// 后面至少留50个
    , EB_COMMAND_MAINFRAME_FUNC	= EBC_WM_USER+0x5500	// 后面至少留50个
    , EB_COMMAND_MY_UGINFO		= EBC_WM_USER+0x5550	// 后面至少留100个
    , EB_COMMAND_SKIN_SETTING	= EBC_WM_USER+0x5650
	, EB_COMMAND_SKIN_1
	, EB_COMMAND_SKIN_2			= EB_COMMAND_SKIN_1+10
    , EB_COMMAND_SKIN_GRADIENT	= EBC_WM_USER+0x5700
	, EB_COMMAND_SKIN_FLAT
	, EB_COMMAND_SERVER_SETTING
	, EB_COMMAND_BROWSER_TYPE_CEF
	, EB_COMMAND_BROWSER_TYPE_IE
	, EB_COMMAND_UISTYLE_TYPE_OFFIACE
	, EB_COMMAND_UISTYLE_TYPE_CHAT
	, EB_COMMAND_QUERY_CAN_SAVE_HISTORY
	, EB_COMMAND_QUERY_BROWSER_TYPE
	, EB_COMMAND_CHANGE_BROWSER_TYPE
	, EB_COMMAND_CHECK_NEW_VERSION
    , EB_COMMAND_RD_CONTROL_DEST	= EBC_WM_USER+0x5710
	, EB_COMMAND_RD_CONTROL_ME
    , EB_COMMAND_SEND_TYPE_RETURN	= EBC_WM_USER+0x5720
	, EB_COMMAND_SEND_TYPE_CTRL_RETURN
	, EB_COMMAND_SEND_100TEST
	, EB_COMMAND_SWITCH_MIN_EBSC
//    , EB_COMMAND_APPID_SUCCESS	= EBC_WM_USER+0x6100  // for qt
//    , EB_COMMAND_APPID_ERROR

}EB_COMMAND_ID;

class COpenAppUrlInfo
{
public:
	std::string m_sShellExecute;
	std::string m_sUrl;
	std::string m_sPostData;
	int m_nOpenParam;
#ifdef _QT_MAKE_
	QObject* m_hwndFrom;
#else
	HWND m_hwndFrom;
#endif
	bool m_bSaveBrowseTitle;
	mycp::bigint m_sOpenSubId;
	std::string m_sParam;
	std::string m_sDir;
	COpenAppUrlInfo(const std::string& sShellExecute, const std::string& sParam, const std::string& sDir)
		: m_sShellExecute(sShellExecute)
		, m_nOpenParam(0), m_hwndFrom(NULL)
		, m_bSaveBrowseTitle(false)
		, m_sOpenSubId(0), m_sParam(sParam), m_sDir(sDir)
	{}
	COpenAppUrlInfo(mycp::bigint nSubId, const std::string& sParam)
		: m_nOpenParam(0), m_hwndFrom(NULL)
		, m_bSaveBrowseTitle(false)
		, m_sOpenSubId(nSubId), m_sParam(sParam)
	{}
	COpenAppUrlInfo(const char* sUrl, const char* sPostData)
		: m_sUrl(sUrl), m_sPostData(sPostData), m_nOpenParam(0), m_hwndFrom(NULL)
		, m_bSaveBrowseTitle(false)
		, m_sOpenSubId(0)
	{}
	COpenAppUrlInfo(const std::string& sUrl, const std::string& sPostData)
		: m_sUrl(sUrl), m_sPostData(sPostData), m_nOpenParam(0), m_hwndFrom(NULL)
		, m_bSaveBrowseTitle(false)
		, m_sOpenSubId(0)
	{}
	COpenAppUrlInfo(void)
		: m_nOpenParam(0), m_hwndFrom(NULL)
		, m_bSaveBrowseTitle(false)
		, m_sOpenSubId(0)
	{}
};

#define EB_LEVEL_FORBID_SPEECH	0x1000

#ifdef _QT_MAKE_
typedef enum EB_WIDGET_ITEM_USER_ROLE_TYPE {
    EB_WIDGET_ITEM_USER_ROLE_FILE_MD5 = Qt::UserRole
//    EB_WIDGET_ITEM_USER_ROLE_ITEM_TYPE = Qt::UserRole
//    , EB_WIDGET_ITEM_USER_ROLE_ITEM_ID
//    , EB_WIDGET_ITEM_USER_ROLE_ITEM_NAME
//    , EB_WIDGET_ITEM_USER_ROLE_GROUP_TYPE
//    , EB_WIDGET_ITEM_USER_ROLE_USER_LINE_STATE
//    , EB_WIDGET_ITEM_USER_ROLE_DISPLAY_INDEX
//    , EB_WIDGET_ITEM_USER_ROLE_FILE_NAME
//    , EB_WIDGET_ITEM_USER_ROLE_FILE_MD5
}EB_WIDGET_USER_ROLE_TYPE;
#endif

class CTreeItemInfo
{
public:
	typedef boost::shared_ptr<CTreeItemInfo> pointer;
    enum ITEM_TYPE {
		ITEM_TYPE_ENTERPRISE
		, ITEM_TYPE_GROUP
		, ITEM_TYPE_MEMBER
		, ITEM_TYPE_CONTACT
		, ITEM_TYPE_URL
		, ITEM_TYPE_DIR
		, ITEM_TYPE_FILE
		, ITEM_TYPE_NOTE
		, ITEM_TYPE_OTHERRES
		, ITEM_TYPE_SUBINFO
		, ITEM_TYPE_EMAIL
		, ITEM_TYPE_SUBMSG
    };
    enum ITEM_EXT_DATA
	{
		ITEM_EXT_DATA_FORBID_SPEECH = 0x1	// 禁言
    };

	static CTreeItemInfo::pointer create(ITEM_TYPE nItemType,int nIndex)
	{
		return CTreeItemInfo::pointer(new CTreeItemInfo(nItemType,nIndex));
	}
#ifdef _QT_MAKE_
    static CTreeItemInfo::pointer create(ITEM_TYPE nItemType,QListWidgetItem* hItem) {
        return CTreeItemInfo::pointer(new CTreeItemInfo(nItemType,hItem));
    }
#endif
#ifdef _QT_MAKE_
    static CTreeItemInfo::pointer create(ITEM_TYPE nItemType,QTreeWidgetItem* hItem) {
#else
    static CTreeItemInfo::pointer create(ITEM_TYPE nItemType,HTREEITEM hItem) {
#endif
		return CTreeItemInfo::pointer(new CTreeItemInfo(nItemType,hItem));
	}
	void operator =(const CTreeItemInfo* pItemInfo)
	{
		if (pItemInfo)
		{
			m_nItemType = pItemInfo->m_nItemType;
			m_nSubType = pItemInfo->m_nSubType;
			m_hItem = pItemInfo->m_hItem;
#ifdef _QT_MAKE_
            m_listItem = pItemInfo->m_listItem;
#endif
			m_nIndex = pItemInfo->m_nIndex;
			m_sEnterpriseCode = pItemInfo->m_sEnterpriseCode;
			m_sGroupCode = pItemInfo->m_sGroupCode;
			m_sMemberCode = pItemInfo->m_sMemberCode;
			m_sName = pItemInfo->m_sName;
			m_sAccount = pItemInfo->m_sAccount;
			m_dwItemData = pItemInfo->m_dwItemData;
			m_nExtData = pItemInfo->m_nExtData;
			m_nCount1 = pItemInfo->m_nCount1;
			m_nCount2 = pItemInfo->m_nCount2;
			if (m_pHeadImage!=NULL)
			{
				delete m_pHeadImage;
			}
			if (pItemInfo->m_pHeadImage!=NULL)
			{
#ifdef _QT_MAKE_
				m_pHeadImage = new QImage(*pItemInfo->m_pHeadImage);
#else
				m_pHeadImage = pItemInfo->m_pHeadImage->Clone();
#endif
			}else
			{
				m_pHeadImage = NULL;
			}
			m_sHeadMd5 = pItemInfo->m_sHeadMd5;
		}
	}
	ITEM_TYPE m_nItemType;
	int m_nSubType;
#ifdef _QT_MAKE_
    QTreeWidgetItem * m_hItem;
    QListWidgetItem * m_listItem;
#else
	HTREEITEM m_hItem;		// for tree
#endif
	int m_nIndex;			// for list
	mycp::bigint m_sEnterpriseCode;
	mycp::bigint m_sGroupCode;
	mycp::bigint m_sMemberCode;
	CEBString m_sGroupName;
	CEBString m_sName;	// group_name,emp_name,res_name...
	CEBString m_sAccount;	// time,...
	mycp::bigint m_nUserId;
	mycp::bigint m_nBigId;
	mycp::bigint m_sId;			// resid,...
	mycp::bigint m_sParentId;
    unsigned long m_dwItemData;
	int m_nExtData;
	int m_nCount1;
	int m_nCount2;
#ifdef _QT_MAKE_
	QImage* m_pHeadImage;
#else
	Gdiplus::Image* m_pHeadImage;
#endif
	tstring m_sHeadMd5;
#ifdef _QT_MAKE_
	void UpdateHead(QImage* pHeadImage,const tstring& sHeadMd5)
#else
	void UpdateHead(Gdiplus::Image* pHeadImage,const tstring& sHeadMd5)
#endif
	{
		if (m_pHeadImage!=NULL)
		{
			delete m_pHeadImage;
		}
		m_pHeadImage = pHeadImage;
		m_sHeadMd5 = sHeadMd5;
	}

	//EB_EnterpriseInfo m_pEnterpriseInfo;
	//EB_GroupInfo m_pGroupInfo;
	//EB_MemberInfo m_pMemberInfo;
	//EB_ContactInfo m_pContactInfo;

	CTreeItemInfo(ITEM_TYPE nItemType,int nIndex)
		: m_nItemType(nItemType),m_nSubType(0)
        , m_hItem(NULL)
    #ifdef _QT_MAKE_
        , m_listItem(0)
    #endif
        ,m_nIndex(nIndex)
		, m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
		, m_nUserId(0),m_nBigId(0)
		, m_sId(0),m_sParentId(0)
		, m_dwItemData(0), m_nExtData(0)
		, m_nCount1(0), m_nCount2(0)
		, m_pHeadImage(NULL)
	{
	}
#ifdef _QT_MAKE_
    CTreeItemInfo(ITEM_TYPE nItemType,QListWidgetItem* hItem)
        : m_nItemType(nItemType),m_nSubType(0)
        , m_hItem(0)
        , m_listItem(hItem)
        , m_nIndex(0)
        , m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
        , m_nUserId(0),m_nBigId(0)
        , m_sId(0),m_sParentId(0)
        , m_dwItemData(0), m_nExtData(0)
        , m_nCount1(0), m_nCount2(0)
        , m_pHeadImage(NULL)
    {
    }
#endif

#ifdef _QT_MAKE_
    CTreeItemInfo(ITEM_TYPE nItemType,QTreeWidgetItem* hItem)
#else
    CTreeItemInfo(ITEM_TYPE nItemType,HTREEITEM hItem)
#endif
		: m_nItemType(nItemType),m_nSubType(0)
        , m_hItem(hItem)
    #ifdef _QT_MAKE_
        , m_listItem(0)
    #endif
        , m_nIndex(0)
		, m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
		, m_nUserId(0),m_nBigId(0)
		, m_sId(0),m_sParentId(0)
		, m_dwItemData(0), m_nExtData(0)
		, m_nCount1(0), m_nCount2(0)
		, m_pHeadImage(NULL)
	{
	}
	CTreeItemInfo(void)
		: m_nItemType(ITEM_TYPE_CONTACT),m_nSubType(0)
        , m_hItem(NULL)
    #ifdef _QT_MAKE_
        , m_listItem(0)
    #endif
        , m_nIndex(0)
		, m_sEnterpriseCode(0),m_sGroupCode(0),m_sMemberCode(0)
		, m_nUserId(0),m_nBigId(0)
		, m_sId(0),m_sParentId(0)
		, m_dwItemData(0), m_nExtData(0)
		, m_nCount1(0), m_nCount2(0)
		, m_pHeadImage(NULL)
	{
	}
};
const CTreeItemInfo::pointer NullTreeItemInfo;

namespace libEbc
{
    long GetFileSize(const char* lpszFile);
#ifdef _QT_MAKE_
    qint64 GetFileSize(const QString &lpszFile);
#endif
	tstring GetFileName(const tstring & sPathName);
	bool GetFileMd5(const char* sFilePath,mycp::bigint& pOutFileSize,tstring& pOutFileMd5);
#ifdef _QT_MAKE_
    bool GetFileMd5(const QString &sFilePath,mycp::bigint& pOutFileSize,tstring& pOutFileMd5);
#endif
	void GetFileExt(const tstring & sFileName, tstring & sOutName, tstring & sOutExt);
#ifdef _QT_MAKE_
    QString fileExt(const QString & filePath);
    QString GBK2UTF8(const QString &inStr);
    QString UTF82GBK(const QString &inStr);
    std::string gbk2utf8(const QString &inStr);
    QString utf82gbk(const std::string &inStr);
    QImage imageToGray(const QImage& image);
    QImage imageAdd(const QImage &image1, const QImage &image2,int xoffset,int yoffset);
#else
	int   GetCodecClsid(const   WCHAR*   format,   CLSID*   pClsid);
	int bmp_2_jpg(const WCHAR * sBmpFile, const WCHAR * sJpgFile, long quality = 100);
	int bmp_2_png(const WCHAR * sBmpFile, const WCHAR * sJpgFile, long quality = 100);
	BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image * & pImg);
	BOOL ImageExFromIDResource(UINT nID, LPCTSTR sTR, CImageEx * & pImg);
	BOOL BitmapFromIDResource(UINT nID, LPCTSTR sTR, Bitmap * & pImg);
	Gdiplus::Image* LoadImageFromFile(const TCHAR* file_name);
	Gdiplus::Bitmap* LoadBitmapFromFile(const TCHAR* file_name);
#endif
	bool ChangeTime(const char* sTimeString, time_t& pOutTime, int* pOutMS = NULL);	// YYYY-mm-dd HH:MM:SS.XXXXX -> time_t

#ifndef _QT_MAKE_
	tstring ACP2UTF8(const char* sString);
	tstring UTF82ACP(const char* sString);
#endif
	const tstring & replace(tstring & strSource, const tstring & strFind, const tstring &strReplace);
	int ParseString(const char * lpszString, const char * lpszInterval, std::vector<tstring> & pOut);
	bool IsFullNumber(const char* pString, size_t nLen);
#ifdef _QT_MAKE_
    int GetWaveTimeLength(const QString &lpszWavFilePath);
#endif
    int GetWaveTimeLength(const char* lpszWavFilePath);
	tstring URLEncode(const char *sIn);
	tstring URLDecode(const char *sIn, bool bTranPlusSign);	// '+'->' '
	tstring GetHostIp(const char* lpszHostName,const char* lpszDefaultName="");

};


#endif //__ebc_public_h__
