// POP.h : PROJECT_NAME 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "DlgEmotionSelect.h"
//#include "DlgVideoConference.h"
#include "DlgDialog.h"
#include "DlgMsgRecord.h"
#include "DlgResourceMgr.h"
#include "EBCCallInfo.h"
#include "DlgContactInfo.h"
#include "DlgMessageBox.h"
#include "DlgFuncWindow.h"
#include "include/cef_app.h"

CString GetLineStateText(EB_USER_LINE_STATE nLineState);
CString GetGroupTypeText(EB_GROUP_TYPE nGroupType, bool bShortName=false);
CString GetGroupManagerText(EB_GROUP_TYPE nGroupType);
int deleteDir(const char * pSrc);
bool DownloadHttpFile(const CString& strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified);
HRESULT GetPostData(LPCTSTR cszPostData,LPVARIANT pvPostData);

//class CPOPConfigInfo
//{
//public:
//	CPOPConfigInfo(void)
//		: m_sServer("192.168.0.208:8012")
//		//: m_sServer("myphone.pop.com")
//	{
//	}
//
//	void SetServer(const tstring & value) {m_sServer = value;}
//	const tstring & GetServer(void) const {return m_sServer;}
//
//private:
//	tstring m_sServer;
//};

typedef enum MSG_RECORD_TYPE
{
	MRT_UNKNOWN
	//MRT_TITLE	// 谁对谁说
	, MRT_TEXT
	, MRT_JPG
	, MRT_FILE
	, MRT_WAV
	, MRT_RESOURCE
	, MRT_MAP_POS		= 10	// 地图位置
	, MRT_CARD_INFO				// 名片信息，如用户名片等
	, MRT_USER_DATA	= 200
};
inline bool IsCanCollectRecordType(MSG_RECORD_TYPE nType) {return (nType==MRT_FILE || nType==MRT_RESOURCE)?false:true;}

const CEBString const_default_group_name = "默认分组";
const eb::bigint const_default_group_ugid = 0;
const int const_default_menu_image_size = 24;

#define DEFAULT_TRIANGLE_BTN_WIDTH 5		// 7
#define DEFAULT_TRIANGLE_BTN_HEIGHT 10	// 14

//#define MENU_TOOLBAR_ICON_OFFSET_DELETE_GROUP	4
#define MENU_TOOLBAR_ICON_OFFSET_DELETE						5	// 删除
//#define MENU_TOOLBAR_ICON_OFFSET_MSG			6
//#define MENU_TOOLBAR_ICON_OFFSET_SHARE			7
//#define MENU_TOOLBAR_ICON_OFFSET_UPLOAD			8
//#define MENU_TOOLBAR_ICON_OFFSET_DELSHARE		9
//#define MENU_TOOLBAR_ICON_OFFSET_ADDUSER		10

#define USES_LIBCEF_USER_CACHE
//class CAboutDlg;
// CPOPApp:
/*
 * ebc程序，APP类
 * 启动开始执行，调用CDlgOAuth自动验证窗口后，登录成功之后，显示CPOPD主界面。
 * 
*//////////////////////////////////////////////////////

class CPOPApp : public CWinApp
	, public CTreeCallback
{
public:
	CPOPApp();

	void AdaptIeVersion(void);

	CLockMap<eb::bigint,bool> m_pAutoCallGroupIdList;
	CLockMap<eb::bigint,bool> m_pAutoCallFromUserIdList;
	CLockMap<eb::bigint,bool> m_pCancelFileList;
	CImageList m_pMenuImageList;

	CImageEx * m_imageExBtnSendECard;
	Gdiplus::Image * m_imageWorkFrame;
	Gdiplus::Image * m_imageMainFrame;
	//Gdiplus::Image * m_imageEbIcon;
	//Gdiplus::Image * m_imageEntLogo;
	Gdiplus::Image * m_image1Group;
	Gdiplus::Image * m_image1People;
	Gdiplus::Image * m_image1Cloud;
	Gdiplus::Image * m_image2Cloud;
	Gdiplus::Image * m_imageDefaultVisitor;
	Gdiplus::Image * m_imageDefaultOrg;
	Gdiplus::Image * m_imageDefaultContact;
	Gdiplus::Image * m_imageDefaultMember;
	Gdiplus::Image * m_imageDefaultDepartment;
	Gdiplus::Image * m_imageDefaultProject;
	Gdiplus::Image * m_imageDefaultGroup;
	Gdiplus::Image * m_imageDefaultTempGroup;
	Gdiplus::Image * m_imageStateAway;
	Gdiplus::Image * m_imageStateBusy;
	Gdiplus::Image * m_imageStateForbid;
	tstring m_sDefaultAppImage;

	Image * GetUserHeadImage(mycp::bigint nUserId,const tstring& sAccount) const;
	tstring GetUserHeadFilePath(mycp::bigint nUserId,const tstring& sAccount) const;
	bool GetMainFrameShowed(void) const;
	bool GetWorkFrameShowed(void) const;

	CDlgEmotionSelect* m_pDlgEmotionSelect;
	void ShowImageWindow(bool bShow, CImageSelectCallback* pCallback,LPRECT lpRect);
	static void RunEBSC(const CString& sParameter);
	static void InvalidateParentRect(const CWnd* pWnd);
	static int CALLBACK TreeCmpFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void SetLineState2Db(EB_USER_LINE_STATE nLineState);
	void DeleteDbRecord(eb::bigint sId, bool bAccount);	// 删除聊天记录和临时图片文件
	void DeleteDbRecord(eb::bigint sMsgId);	// 删除聊天记录和临时图片文件
	void OpenGroupShareWindow(eb::bigint sGroupCode,const CWnd* pWndLayout);
	void OpenMyShareWindow(const CWnd* pWndLayout);
	bool OpenSubscribeFuncWindow(const EB_SubscribeFuncInfo& pSubscribeFuncInfo,const std::string& sPostData, const std::string& sParameter,HWND hwndFrom);
	CString GetSubscribeFuncUrl(const eb::bigint nSubId, const CString& sParameter) const;
	bool OpenAutoSubscribeFunc(const EB_SubscribeFuncInfo& pSubscribeFuncInfo);
	//void ShowAutoOpenSubscribeFunc(int nCmdShow);
	void ShowHideAutoOpenSubscribeFunc(bool bShow);
	bool IsEnterpriseuserUser(void);
	void UpdateMsgReceiptData(eb::bigint nMsgId, eb::bigint nFromUserId, int nAckType);

	//CDlgVideoConference::pointer GetDlgVideoConference(const CEBCCallInfo::pointer& pEbCallInfo,bool bBuildNew = true);
	int m_nSendType;
	int GetSendType(void);
	void SetSendType(int nSendType);
	bool m_bLogDebug;
	FILE * m_pLogDebug;
	void LogMessage(const char * format,...);

	CString GetSettingFile(void) const;
	CString GetSettingIniFile(void) const;
	CString GetUserSettingIniFile(void) const;
	void EBC_SetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszValue);
	CString EBC_GetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszDefault=NULL);
	void EBC_SetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue);
	UINT EBC_GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nDefault=0);

	static HICON GetFileHIcon(const std::string& sFilePath);
	static void SelectFirstSearch(VividTree& treeSearch, CHoverEdit& editSearch, bool& bCanSearch);

	//CPOPConfigInfo	m_config;
	CHARFORMAT		m_cfDefault;
	bo::CBodbHandler::pointer m_pBoEB;
	CSqliteCdbc::pointer m_pBoUsers;
	//bo::CBodbHandler::pointer m_pBoUsers;

	CLockMap<eb::bigint,CDlgFuncWindow*> m_pTempFromUserMsg;		// uid->
	CLockMap<eb::bigint,CDlgFuncWindow*> m_pTempGroupMsg;			// gid->
	CLockMap<eb::bigint,CDlgMsgRecord*> m_pMsgRecord;				// account/groupcode->
	CLockMap<eb::bigint,CDlgResourceMgr*> m_pResourceMgr;			// account/groupcode->
	CLockMap<eb::bigint,CEBCCallInfo::pointer> m_pCallList;		// callid->
//#ifndef USES_FRAME_LIST
//	CLockMap<eb::bigint, CDlgDialog::pointer> m_pDialogList;	// callid->
//#endif
	//CLockMap<eb::bigint, CDlgVideoConference::pointer> m_pVideoConferenceList;	// callid->
	CLockList<CDlgDialog::pointer> m_pCloseDialog;			// 
	//CLockList<CDlgVideoConference::pointer> m_pCloseVideoConference;			// 
	CLockList<CWnd*> m_pCloseWnd;							// 
	std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;	// for menu
	void ClearSubscribeSelectInfo(void);
	eb::bigint m_nSelectCallId;
	eb::bigint m_nSelectUserId;
	tstring m_sSelectAccount;
	eb::bigint m_nSelectGroupId;
	std::vector<EB_SubscribeFuncInfo> m_pChatMenuSubscribeFuncList;	// for chat record menu
	size_t BuildChatMenuSubscribeFuncList(void);
	std::vector<EB_SubscribeFuncInfo> m_pChatCopySubscribeFuncList;	// for chat text copy menu
	size_t BuildChatCopySubscribeFuncList(void);

	//tstring GetFullAccount(const char* lpszAccount) const;
	//int GetManagerLevel(const char* sGroupCode) const;
	static void GetScreenSize(int & pWidth, int & pHeight);
	static int GetScreenWidth(void);
	static int GetScreenHeight(void);
	const CString & GetSysPath(void);
	const CString & GetAppPath(void) const {return m_sAppPath;}
	const CString & GetCefCachePath(void) const {return m_sCefCachePath;}
	const CString & GetAppImgTempPath(void) const {return m_sAppImgTempPath;}
	const CString & GetAppDataPath(void) const {return m_sAppDataPath;}
	const CString & GetUserMainPath(void) const {return m_sUserMainPath;}
	const CString & GetUserImagePath(void) const {return m_sUserImagePath;}
	const CString & GetUserFilePath(void) const {return m_sUserFilePath;}
	const CString& GetLogonAccount(void) const {return m_sLogonAccount;}
	eb::bigint GetLogonUserId(void) const {return m_nLogonUserId;}
	const CString& GetLogonPassword(void) const {return m_sLogonPassword;}
	void SetEntManagerurl(const tstring& v) {m_sEntManagerUrl = v;}
	const tstring& GetEntManagerurl(void) const {return m_sEntManagerUrl;}
	const tstring& GetDefaultUrl(void) const {return m_sDefaultUrl;}
	const CString& GetDefaultLoadingImageFile(void) const {return m_sDefaultLoadingImageFile;}
	const CString& GetDefaultPageImageFile(void) const {return m_sDefaultPageImageFile;}
	//const HICON GetIconAway(void) const {return m_iconAway;}
	//const HICON GetIconBusy(void) const {return m_iconBusy;}
	//const HICON GetIconEnt(void) const {return m_iconEnterprise;}
	//const HICON GetIconDep(void) const {return m_iconDepartment;}
	//const HICON GetIconCon(void) const {return m_iconContact;}
	void SetMainColor(COLORREF v);
	COLORREF GetMainColor(void) const {return m_nMainColor;}
	COLORREF GetFlatBgColor(void) const {return m_nFlatBgColor;}
	//COLORREF GetFuncColor(void) const {return m_nFuncColor;}
	COLORREF GetBgColor(void) const {return m_nBgColor;}
	COLORREF GetHotColor1(void) const {return m_nHotColor1;}
	COLORREF GetHotColor2(void) const {return m_nHotColor2;}
	COLORREF GetTrueColor(void) const {return m_nTrueColor;}
	COLORREF GetHotColor(void) const {return m_nHotColor;}
	COLORREF GetPreColor(void) const {return m_nPreColor;}
	COLORREF GetSelColor(void) const {return m_nSelColor;}
	//COLORREF GetBgTitle0(void) const {return m_nBgTitle0;}
	//COLORREF GetBgColor1(void) const {return m_nBgColor1;}
	//COLORREF GetBgColor2(void) const {return m_nBgColor2;}
	//COLORREF GetBgColor3(void) const {return m_nBgColor3;}
	//COLORREF GetOkColor0(void) const {return m_nOkColor0;}
	////COLORREF GetOkColor1(void) const {return m_nOkColor1;}
	COLORREF GetNormalColor(void) const {return m_nNormalColor;}
	void SetColorGradient(bool v) {if (m_bColorGradient!=v){m_bColorGradient=v;SetMainColor(GetMainColor());}}
	bool GetColorGradient(void) const {return m_bColorGradient;}
	void SetColorFlat(bool v) {m_bColorFlat = v;}
	bool GetColorFlat(void) const {return m_bColorFlat;}
	const CString& GetAccountPrefix(void) const {return m_sAccountPrefix;}
	const CString& GetProductName(void) const {return m_sProductName;}
	int GetSystemAccountFlag(void) const {return m_nSystemAccountFlag;}
	bool GetLicenseUser(void) const {return m_bLicenseUser;}
	bool GetSendRegMail(void) const {return m_bSendRegMail;}
	bool GetSaveConversationLocal(void) const {return m_bSaveConversationLocal;}
	bool GetSaveConversationServer(void) const {return m_bSaveConversationServer;}
	bool GetAuthContact(void) const {return m_bAuthContact;}
	eb::bigint GetDeployId(void) const {return m_nDeployId;}
	int GetLicenseType(void) const {return m_nLicenstType;}
	bool IsLicenseType(void) const {return (m_nLicenstType==1||m_nLicenstType==2)?true:false;}
	int GetEBServerVersion(void) const {return m_nEBServerVersion;}
	eb::bigint GetGroupMsgSugId(void);
	eb::bigint GetFindAppSugId(void);
	eb::bigint GetMyCollectionSugId(void);
	eb::bigint GetAutoOpenSubId(void) const {return m_nAutoOpenSubId;}
	bool GetAutoHideMainFrame(void) const {return m_bAutoHideMainFrame;}
	bool GetHideMainFrame(void) const {return m_bHideMainFrame;}
	bool GetDisableUserCloudDrive(void) const {return m_bDisableUserCloudDrive;}
	bool GetDisableGroupSharedCloud(void) const {return m_bDisableGroupSharedCloud;}
	bool GetDisableModifyPasswd(void) const {return m_bDisableModifyPasswd;}
	bool GetDisableVideo(void) const {return m_bDisableVideo;}
	bool GetDisableRD(void) const {return m_bDisableRemoteDesktop;}
	bool GetDisableAccountEdit(void) const {return m_bDisableAccountEdit;}
	bool GetDisableMsgReceipt(void) const {return m_bDisableMsgReceipt;}
	bool GetStatSubGroupMember(void) const {return m_bStatSubGroupMember;}
	void SetEnterpriseCreateUserId(eb::bigint nUserId) {m_nEnterpriseCreateUserId = nUserId;}
	eb::bigint GetEnterpriseCreateUserId(void) const {return m_nEnterpriseCreateUserId;}
	bool IsEnterpriseCreateUserId(eb::bigint nUserId) {return (nUserId>0 && nUserId==m_nEnterpriseCreateUserId)?true:false;}

	void SetDefaultBrowserType(EB_BROWSER_TYPE newValue) {m_nDefaultBrowserType = newValue;}
	EB_BROWSER_TYPE GetDefaultBrowserType(void) const {return m_nDefaultBrowserType;}
	bool SetDefaultUIStyleType(EB_UI_STYLE_TYPE newValue);
	EB_UI_STYLE_TYPE GetDefaultUIStyleType(void) const {return m_nDefaultUIStyleType;}
	void SetMinEBSC(bool newValue);
	bool GetMinEBSC(void) const {return m_nMinEBSC;}

	bool GetIeException(void) const {return m_bIeException;}

	void NewEmployeeInfo(CWnd* pParent,const CTreeItemInfo * pTreeItemInfo);
#ifdef USES_EBCOM_TEST
	void EditEmployeeInfo(CWnd* pParent,IEB_MemberInfo* pMemberInfo);
#else
	void EditEmployeeInfo(CWnd* pParent,const EB_MemberInfo* pMemberInfo);
#endif
	void EditGroupInfo(CWnd* pParent, eb::bigint sGroupCode);
	bool IsLogonVisitor(void) const;
private:
	CString m_sSystemDir;
	CString m_sAppPath;
	CString m_sCefCachePath;
	CString m_sAppName;
	CString m_sAppImgTempPath;
	CString m_sAppDataPath;
	CString m_sUserMainPath;
	CString m_sUserImagePath;
	CString m_sUserFilePath;
	CString m_sLogonAccount;
	eb::bigint m_nLogonUserId;
	CString m_sLogonPassword;
	tstring m_sEntManagerUrl;
	tstring m_sDefaultUrl;
	CString m_sDefaultLoadingImageFile;
	CString m_sDefaultPageImageFile;
	//HICON m_iconAway;
	//HICON m_iconBusy;
	//HICON m_iconEnterprise;
	//HICON m_iconDepartment;
	//HICON m_iconContact;
	COLORREF m_nMainColor;
	COLORREF m_nFlatBgColor;
	bool m_bColorGradient;
	bool m_bColorFlat;
	//COLORREF m_nFuncColor;
	COLORREF m_nBgColor;
	COLORREF m_nHotColor1;
	COLORREF m_nHotColor2;
	COLORREF m_nTrueColor;
	COLORREF m_nHotColor;
	COLORREF m_nPreColor;
	COLORREF m_nSelColor;	// 主要用于左边导航条选择
	COLORREF m_nNormalColor;
	//COLORREF m_nBgTitle0;
	//COLORREF m_nBgColor1;
	//COLORREF m_nBgColor2;
	//COLORREF m_nBgColor3;
	//COLORREF m_nOkColor0;
	////COLORREF m_nOkColor1;
	CString m_sAccountPrefix;
	CString m_sProductName;
	int m_nSystemAccountFlag;	// 系统帐号标示，1=系统管理员；2=普通企业管理员；0=普通用户
	bool m_bLicenseUser;
	bool m_bSendRegMail;
	bool m_bSaveConversationLocal;
	bool m_bSaveConversationServer;
	bool m_bAuthContact;
	eb::bigint m_nDeployId;
	int m_nLicenstType;	// 许可类型；0=未授权；1=终身授权；2=时间授权
	int m_nEBServerVersion;
	eb::bigint m_nGroupMsgSubId;
	eb::bigint m_nFindAppSubId;
	eb::bigint m_nMyCollectionSubId;
	eb::bigint m_nAutoOpenSubId;
	CDlgFuncWindow::pointer m_pAutoOpenFuncWindow;
	bool m_bAutoHideMainFrame;
	bool m_bHideMainFrame;
	bool m_bDisableUserCloudDrive;
	bool m_bDisableGroupSharedCloud;
	bool m_bDisableModifyPasswd;
	bool m_bDisableVideo;
	bool m_bDisableRemoteDesktop;
	bool m_bDisableAccountEdit;
	eb::bigint m_nEnterpriseCreateUserId;
	EB_BROWSER_TYPE m_nDefaultBrowserType;
	EB_UI_STYLE_TYPE m_nDefaultUIStyleType;
	bool m_nMinEBSC;
	bool m_bDisableMsgReceipt;
	bool m_bStatSubGroupMember;

	bool m_bIeException;
	bool m_bAdaptIeVersion;

	CBitmap m_bmpTreeDirClosed;
	CBitmap m_bmpTreeDirOpened;
	CBitmap m_bmpTreeType0Closed;	// department
	CBitmap m_bmpTreeType0Opened;
	CBitmap m_bmpTreeType1Closed;	// project
	CBitmap m_bmpTreeType1Opened;
	CBitmap m_bmpTreeType2Closed;	// group
	CBitmap m_bmpTreeType2Opened;
	CBitmap m_bmpTreeType9Closed;	// temp
	CBitmap m_bmpTreeType9Opened;

	public:
	virtual bool GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,Gdiplus::Image*& pImage3,int& pState) const;
	virtual bool GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON& pIcon1, HICON& pIcon2,int& pState) const;
	virtual bool GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,const CBitmap** pBitmapOpened,const CBitmap** pBitmapClosed) const;
	virtual COLORREF GetItemTextColor(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, COLORREF colorDefault) const;
private:
	//CAboutDlg* m_dlgAbout;
	int GetExistAppCount(void) const;
// 重写
	//bool openebuser(void);
	virtual BOOL InitInstance();
	void ReadUserSetting(void);
	void SaveUserSetting(void);

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	afx_msg void OnAbout();
};
extern bool theWantExitApp;

extern CPOPApp theApp;
#ifdef USES_EBCOM_TEST
extern CComPtr<IEBParseSetting>	theEBSetting;
extern CComPtr<IEBClientCore>	theEBClientCore;
//extern CEBAppClient theEBAppClient;
#else
extern CEBParseSetting theSetting;
extern CEBAppClient theEBAppClient;
#endif