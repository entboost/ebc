#ifndef EBDEFINES_H
#define EBDEFINES_H

#include <QSize>

const int const_button_icon_size = 9;

const int const_common_edit_height = 22;
const int const_common_title_font_size = 11;
const int const_common_title_image_font_size = 12;
const int default_warning_auto_close = 3;       /// 警告提示，显示3秒后，自动关闭
const int default_information_auto_close = 5;   /// 消息提示，显示5秒后，自动关闭

const QSize const_tree_icon_size(32,32);
const QSize const_default_menu_image_size(24,24);

#define UISTYLE_CHAT_DEFAULT_DLG_WIDTH	288
#define UISTYLE_CHAT_DEFAULT_DLG_HEIGHT	568

#ifndef is_visitor_uid
const qint64 theVisitorStartId = 0x2000000000000LL;	// =562949953421312(15位)
#define is_visitor_uid(id) (id>=theVisitorStartId)
#endif // is_visitor_uid

/// "eb-view-ecard:///[ECARD_DATA]"
static const char * theViewEcard  = "eb-view-ecard";
/// "eb-play-voice:///[FILE_NAME]"
static const char * thePlayVoice  = "eb-play-voice";
/// eb-download-resource://[TYPE],[RESOURCEID]
/// eb-download-resource://[TYPE],[RESOURCEID],[FILENAME]
static const char * theDownloadResource  = "eb-download-resource";
/// eb-add-contact://contact_userid,description
static const char * theReqAddContact  = "eb-add-contact";
static const char * theCallAccount  = "ebim-call-account";
static const char * theCallGroup  = "ebim-call-group";
/// eb-open-subid://[sub_id][,xxx]
/// eb-open-subid://[sub_id],0[,xxx]	// 当前窗口打开
/// eb-open-subid://[sub_id],1[,xxx]	// 新窗口打开
static const char * theOpenSubId  = "eb-open-subid";
//static const char * theOpenUrl  = "eb-open-url";
/// eb-open-file:///[FILE_PATH]
static const char * theOpenFile  = "eb-open-file";
static const char * theEBWindow2Url  = "eb-open2";  /// ?
static const char * theEBWindowUrl  = "eb-open";    /// ?
static const char * theClose  = "eb-close";         /// ?
static const char * theExitApp  = "eb-exit-app";
static const char * theLogout  = "eb-logout";
static const char * theShowMainFrame  = "eb-show-mainframe";
static const char * theHideMainFrame  = "eb-hide-mainframe";
static const char * theOpenFileManager  = "eb-open-filemanager";
static const char * theMailTo  = "mailto";

const qint64 constGroupShareSubId = 1002300105;


typedef enum EB_VIEW_MODE {
    EB_VIEW_MAIN_FRAME,    /// 主界面
    EB_VIEW_SELECT_USER    /// 选择用户
}EB_VIEW_MODE;

typedef enum EB_SORT_ITEMS_FLAG {
    EB_DISABLE_SORT,        /// 禁排序
    EB_AUTO_SORT,           /// 自动排序
    EB_FORCE_SORT           /// 强制排序
}EB_SORT_ITEMS_FLAG;

typedef enum EB_SEARCH_FROM_FLAG{
    EB_SEARCH_FROM_SEARCH_BAR,
    EB_SEARCH_FROM_MAIN_FRAME,
    EB_SEARCH_FROM_SELECT_USER
}EB_SEARCH_FROM_FLAG;

typedef enum EB_SEND_KEY_TYPE {
    EB_SEND_KEY_ENTER,
    EB_SEND_KEY_CTRL_ENTER
}EB_SEND_KEY_TYPE;

typedef enum EB_MSG_RECORD_TYPE
{
    MRT_UNKNOWN
    , MRT_TEXT
    , MRT_JPG
    , MRT_FILE
    , MRT_WAV
    , MRT_RESOURCE
    , MRT_MAP_POS		= 10	/// 地图位置
    , MRT_CARD_INFO				/// 名片信息，如用户名片等
    , MRT_USER_DATA	= 200
}EB_MSG_RECORD_TYPE;
inline bool isCanCollectRecordType(EB_MSG_RECORD_TYPE nType) {return (nType==MRT_FILE || nType==MRT_RESOURCE)?false:true;}


#endif // EBDEFINES_H
