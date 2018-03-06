#pragma once
#include "Resource.h"
#include "DlgEmotionSelect.h"
//#include "DlgVideoConference.h"
#include "DlgDialog.h"
#include "DlgMsgRecord.h"
#include "DlgResourceMgr.h"
#include "EBCCallInfo.h"
#include "DlgContactInfo.h"
#include "DlgMessageBox.h"
#include "DlgFuncWindow.h"
#include "ebuihandle.h"


CString GetLineStateText(EB_USER_LINE_STATE nLineState);
CString GetGroupTypeText(EB_GROUP_TYPE nGroupType, bool bShortName=false);

typedef enum MSG_RECORD_TYPE
{
	MRT_UNKNOWN
	//MRT_TITLE	// 谁对谁说
	, MRT_TEXT
	, MRT_JPG
	, MRT_FILE
	, MRT_WAV
};
const CEBString const_default_group_name = "默认分组";
const eb::bigint const_default_group_ugid = 0;

class CEBUISwitchInstance
{
public:
	CEBUISwitchInstance(void);
	virtual ~CEBUISwitchInstance(void);
private:
	HINSTANCE m_hOldInstanceHandle;
	HINSTANCE m_hOldResourceHandle;
};
#define AFX_SWITCH_INSTANCED() CEBUISwitchInstance theEBUISwitchInstance

const COLORREF theDefaultTitleTextColor = RGB(250,250,250);

class CDlgMainFrame;

#ifdef _USRDLL
class CEBUIApp : public CWinApp
	, public CTreeCallback
#else
class CEBUIApp : public CTreeCallback
#endif
{
public:
	CEBUIHandler* m_pHandler;
	BOOL IM_OAuth(const char* lpszParameter);
	BOOL IM_Login(const char* lpszAccount, const char* lpszPasswd);
	BOOL IM_IsLogined(void) const;
	BOOL IMInitData(void);
	void IM_Logout(void);

	BOOL MainFrame_Create(HWND hParentHwnd, UINT nMsgId);
	HWND MainFrame_GetSafeHwnd(void) const;
	void MainFrame_Destroy(void);
	void MainFrame_ShowWindow(int nShow);
	void MainFrame_MoveWindow(int x, int y, int w, int h, BOOL bRepaint);
	void MainFrame_MoveWindow(LPCRECT lpRect, BOOL bRepaint=TRUE);
	void MainFrame_Invalidate(BOOL bErase = TRUE);
	void ChatWindow_SetTopMost(bool bTopMost);

	//void SetMainColor(COLORREF v) {m_colorMain = v;}
	//COLORREF GetMainColor(void) const {return m_colorMain;}

	CWnd* GetMainWnd(void) const {return (CWnd*)m_pMainWnd;}
	void AdaptIeVersion(void);
	//void PostEbuiMessage(WPARAM wParam, LPARAM lParam);

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
	tstring m_sDefaultAppImage;

	bool GetAppFrameShowed(void) const;

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
	bool OpenSubscribeFuncWindow(const EB_SubscribeFuncInfo& pSubscribeFuncInfo,const std::string& sPostData="", const std::string& sParameter="");
	bool OpenAutoSubscribeFunc(const EB_SubscribeFuncInfo& pSubscribeFuncInfo);
	void ShowAutoOpenSubscribeFunc(int nCmdShow);
	void ShowHideAutoOpenSubscribeFunc(bool bShow);
	bool IsEnterpriseuserUser(void);

	//CDlgVideoConference::pointer GetDlgVideoConference(const CEBCCallInfo::pointer& pEbCallInfo,bool bBuildNew = true);
	bool m_bTopMost;
	int m_nSendType;
	int GetSendType(void);
	void SetSendType(int nSendType);

	CString GetSettingFile(void) const;
	CString GetSettingIniFile(void) const;
	CString GetUserSettingIniFile(void) const;
	void EBC_SetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszValue);
	CString EBC_GetProfileString(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,LPCTSTR lpszDefault=NULL);
	void EBC_SetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue);
	void WriteProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue);
	UINT EBC_GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nDefault=0);
	UINT GetProfileInt(LPCTSTR lpszAppName,LPCTSTR lpszKeyName,UINT nValue);

	HICON GetFileHIcon(const std::string& sFilePath);
	CLockMap<tstring,HICON> m_pFileHIcon;				// ext->

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
	std::string m_sSelectAccount;
	eb::bigint m_nSelectGroupId;
	std::vector<EB_SubscribeFuncInfo> m_pChatMenuSubscribeFuncList;	// for chat record menu
	size_t BuildChatMenuSubscribeFuncList(void);

	//std::string GetFullAccount(const char* lpszAccount) const;
	//int GetManagerLevel(const char* sGroupCode) const;
	static void GetScreenSize(int & pWidth, int & pHeight);
	const CString & GetAppPath(void) const {return m_sAppPath;}
	const CString & GetAppImgTempPath(void) const {return m_sAppImgTempPath;}
	const CString & GetAppDataPath(void) const {return m_sAppDataPath;}
	const CString & GetUserMainPath(void) const {return m_sUserMainPath;}
	const CString & GetUserImagePath(void) const {return m_sUserImagePath;}
	const CString & GetUserFilePath(void) const {return m_sUserFilePath;}
	const CString& GetLogonAccount(void) const {return m_sLogonAccount;}
	eb::bigint GetLogonUserId(void) const {return m_nLogonUserId;}
	const CString& GetLogonPassword(void) const {return m_sLogonPassword;}
	void SetEntManagerurl(const std::string& v) {m_sEntManagerUrl = v;}
	const std::string& GetEntManagerurl(void) const {return m_sEntManagerUrl;}
	//const HICON GetIconAway(void) const {return m_iconAway;}
	//const HICON GetIconBusy(void) const {return m_iconBusy;}
	//const HICON GetIconEnt(void) const {return m_iconEnterprise;}
	//const HICON GetIconDep(void) const {return m_iconDepartment;}
	//const HICON GetIconCon(void) const {return m_iconContact;}
	void SetTreeListColor(COLORREF nSelectColor, COLORREF nHotColor, COLORREF nBoderColor);
	COLORREF GetTitleTextColor(void) const {return theDefaultTitleTextColor;}
	void SetMainColor(COLORREF v);
	COLORREF GetMainColor(void) const {return m_nMainColor;}
	COLORREF GetFuncColor(void) const {return m_nFuncColor;}
	COLORREF GetHotColor1(void) const {return m_nHotColor1;}
	COLORREF GetHotColor2(void) const {return m_nHotColor2;}
	COLORREF GetHotColor(void) const {return m_nHotColor;}
	COLORREF GetBgTitle0(void) const {return m_nBgTitle0;}
	COLORREF GetBgColor1(void) const {return m_nBgColor1;}
	COLORREF GetBgColor2(void) const {return m_nBgColor2;}
	COLORREF GetBgColor3(void) const {return m_nBgColor3;}
	COLORREF GetOkColor0(void) const {return m_nOkColor0;}
	COLORREF GetOkColor1(void) const {return m_nOkColor1;}
	COLORREF GetNormalColor(void) const {return m_nNormalColor;}
	void SetColorGradient(bool v) {if (m_bColorGradient!=v){m_bColorGradient=v;SetMainColor(GetMainColor());}}
	bool GetColorGradient(void) const {return m_bColorGradient;}
	const CString& GetAccountPrefix(void) const {return m_sAccountPrefix;}
	const CString& GetProductName(void) const {return m_sProductName;}
	bool GetLicenseUser(void) const {return m_bLicenseUser;}
	bool GetSendRegMail(void) const {return m_bSendRegMail;}
	bool GetSaveConversationLocal(void) const {return m_bSaveConversationLocal;}
	bool GetSaveConversationServer(void) const {return m_bSaveConversationServer;}
	bool GetAuthContact(void) const {return m_bAuthContact;}
	eb::bigint GetDeployId(void) const {return m_nDeployId;}
	eb::bigint GetGroupMsgSugId(void);
	eb::bigint GetFindAppSugId(void);
	eb::bigint GetAutoOpenSubId(void) const {return m_nAutoOpenSubId;}
	bool GetAutoHideMainFrame(void) const {return m_bAutoHideMainFrame;}
	bool GetHideMainFrame(void) const {return m_bHideMainFrame;}
	bool GetDisableUserCloudDrive(void) const {return m_bDisableUserCloudDrive;}
	bool GetDisableGroupSharedCloud(void) const {return m_bDisableGroupSharedCloud;}
	bool GetDisableModifyPasswd(void) const {return m_bDisableModifyPasswd;}
	bool GetDisableVideo(void) const {return m_bDisableVideo;}
	bool GetDisableRD(void) const {return m_bDisableRemoteDesktop;}

	void NewEmployeeInfo(CWnd* pParent,const CTreeItemInfo * pTreeItemInfo);
#ifdef USES_EBCOM_TEST
	void EditEmployeeInfo(CWnd* pParent,IEB_MemberInfo* pMemberInfo);
#else
	void EditEmployeeInfo(CWnd* pParent,const EB_MemberInfo* pMemberInfo);
#endif
	void EditGroupInfo(CWnd* pParent, eb::bigint sGroupCode);
	bool IsLogonVisitor(void) const;
private:
	CString m_sAppPath;
	CString m_sAppName;
	CString m_sAppImgTempPath;
	CString m_sAppDataPath;
	CString m_sUserMainPath;
	CString m_sUserImagePath;
	CString m_sUserFilePath;
	CString m_sLogonAccount;
	eb::bigint m_nLogonUserId;
	CString m_sLogonPassword;
public:
	int m_nOpenRegister;
	bool m_bOpenVisitor;
	std::string m_sEntManagerUrl;
	//HICON m_iconAway;
	//HICON m_iconBusy;
	//HICON m_iconEnterprise;
	//HICON m_iconDepartment;
	//HICON m_iconContact;
	COLORREF m_nTreeListSelColor;
	COLORREF m_nTreeListHotColor;
	COLORREF m_nTreeListBorderColor;
	COLORREF m_nMainColor;
	bool m_bColorGradient;
	COLORREF m_nFuncColor;
	COLORREF m_nHotColor1;
	COLORREF m_nHotColor2;
	COLORREF m_nHotColor;
	COLORREF m_nNormalColor;
	COLORREF m_nBgTitle0;
	COLORREF m_nBgColor1;
	COLORREF m_nBgColor2;
	COLORREF m_nBgColor3;
	COLORREF m_nOkColor0;
	COLORREF m_nOkColor1;
	CString m_sAccountPrefix;
	CString m_sProductName;
	bool m_bLicenseUser;
	bool m_bSendRegMail;
	bool m_bSaveConversationLocal;
	bool m_bSaveConversationServer;
	bool m_bAuthContact;
	eb::bigint m_nDeployId;
	eb::bigint m_nGroupMsgSubId;
	eb::bigint m_nFindAppSubId;
	eb::bigint m_nAutoOpenSubId;
	CDlgFuncWindow* m_pAutoOpenFuncWindow;
	bool m_bAutoHideMainFrame;
	bool m_bHideMainFrame;
	bool m_bDisableUserCloudDrive;
	bool m_bDisableGroupSharedCloud;
	bool m_bDisableModifyPasswd;
	bool m_bDisableVideo;
	bool m_bDisableRemoteDesktop;

	bool m_bAdaptIeVersion;

	virtual bool GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const;
	virtual bool GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;

	//bool m_bInited;
	virtual BOOL InitInstance();
	virtual int ExitInstance();
#ifdef _USRDLL
	DECLARE_MESSAGE_MAP()
#endif
	void ReadUserSetting(void);
	void SaveUserSetting(void);

#ifndef _USRDLL
	HINSTANCE m_hInstance;
#endif
private:
	CDlgMainFrame* m_pMainWnd;
public:
	CEBUIApp(void);
	virtual ~CEBUIApp(void);
	
};

extern CEBUIApp theApp;
extern CEBParseSetting theSetting;
extern CEBAppClient theEBAppClient;
