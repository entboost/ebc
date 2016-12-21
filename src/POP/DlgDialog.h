#pragma once
#include "DlgChatInput.h"
#include "DlgChatRight.h"
#include "DlgSelectUser.h"
#include "EBCCallInfo.h"

#define DEFAULT_P2P_WIDTH	710
//#define DEFAULT_P2P_WIDTH	560
#define DEFAULT_DEP_WIDTH	760
#define DEFAULT_DLG_HEIGHT	588

// CDlgDialog dialog
class CDlgUserList;

/*
 * 聊天对话框窗口类
 * 界面最上边显示聊天对方基本信息，和基本聊天操作按钮，如发送文件，添加用户，视频聊天等。
 * 
*//////////////////////////////////////////////////////

class CDlgDialog : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgDialog)

public:
	typedef boost::shared_ptr<CDlgDialog> pointer;
	CDlgDialog(CWnd* pParent = NULL, bool bDeleteThis = true);   // standard constructor
	virtual ~CDlgDialog();
	static CDlgDialog::pointer create(CWnd * pParent = NULL)
	{
		return CDlgDialog::pointer(new CDlgDialog(pParent, false));
	}

	void RefreshBtnShow(void);	// for USES_SUPPORT_UI_STYLE
	void NotifyMoveOrResizeStarted(void);
	void SetCtrlColor(bool bInvalidate = true);
	void SetOwnerCall(bool bOwnerCall) {m_bOwnerCall = bOwnerCall;}
	void ScrollToEnd(void);
	void SetBtnMax(bool bMax);
	void RefreshWeb(void);
	void OnMove(void);
	void SetInputFocus(void);
	//void ChangeFromName(void);
	const tstring& GetFromName(void) const {return m_sFromName;}
	const CString& GetFullName(void) const {return m_sFullName;}
	const CString& GetShortName(void) const {return m_sShortName;}
	const tstring& GetFromDescriptoin(void) const {return m_sFromDescription;}
	const Gdiplus::Image* GetFromImage(void) const {return m_pFromImage;}
	EB_USER_LINE_STATE GetFromLineState(void) const {return m_nFromLineState;}
	bool GetFromIsOffLineState(void) const {return m_nFromLineState==EB_LINE_STATE_OFFLINE || m_nFromLineState==EB_LINE_STATE_UNKNOWN;}
	void LineStateChange(eb::bigint nUserId, EB_USER_LINE_STATE bLineState);

	void ExitRD(void);
	void SetCallInfo(const CEBCCallInfo::pointer& pCallInfo);
	const CEBCCallInfo::pointer GetCallInfo(void) const {return m_pEbCallInfo;}
	eb::bigint GetCallId(void) const {return m_pEbCallInfo.get()!=NULL?m_pEbCallInfo->m_pCallInfo.GetCallId():0;}
	eb::bigint GetGroupId(void) const {return m_pEbCallInfo.get()!=NULL?m_pEbCallInfo->m_pCallInfo.m_sGroupCode:0;}
	eb::bigint GetFromUserId(void) const {return m_pEbCallInfo.get()!=NULL?m_pEbCallInfo->m_pFromAccountInfo.GetUserId():0;}

	// chatroom msg
	//void OnEnterRoom(void);
#ifdef USES_EBCOM_TEST
	void OnUserEnterRoom(IEB_ChatAccountInfo* pAccountInfo);
	void OnUserExitRoom(IEB_ChatAccountInfo* pAccountInfo);
	void OnReceiveRich(IEB_ChatRichInfo* pCrMsgInfo, CString* sOutFirstMsg);
	void OnSendRich(IEB_ChatRichInfo* pCrMsgInfo,EB_STATE_CODE nState);
	void OnSendingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnSentFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnCancelFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo,CString* sOutFirstMsg);
	void OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent);
	void OnSave2CloutDrive(IEB_ChatFileInfo* pCrFileInfo,int nStateValue);
	void VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoTimeout(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void OnUserEmpInfo(IEB_MemberInfo* pMemberInfo);
#else
	void OnUserNotify(const CCrNotifyInfo* pNotifyInfo,CString* pOutFirstMsg=NULL);
	void OnUserEnterRoom(const CCrAccountInfo* pAccountInfo,bool bSort);
	void OnUserExitRoom(const CCrAccountInfo* pAccountInfo);
	void OnReceiveRich(const CCrRichInfo * pCrMsgInfo,CString* sOutFirstMsg1,CString* sOutFirstMsg2);
	void OnSendRich(const CCrRichInfo * pCrMsgInfo,EB_STATE_CODE nState);
	void OnMsgReceipt(const CCrRichInfo * pCrMsgInfo,int nAckType);
	void OnSendingFile(const CCrFileInfo * pCrFileInfo);
	bool OnSentFile(const CCrFileInfo * pCrFileInfo, EB_STATE_CODE nState);
	void OnCancelFile(const CCrFileInfo * pCrFileInfo, bool bChangeP2PSending);
	void OnReceivingFile(const CCrFileInfo * pCrFileInfo,CString* sOutFirstMsg);
	void OnReceivedFile(const CCrFileInfo * pCrFileInfo);
	void OnFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent);
	void OnSave2CloutDrive(const CCrFileInfo * pCrFileInfo,int nStateValue);
	void RDRequestResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void RDAckResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void RDRequest(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDAccept(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDCancel(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDEnd(const EB_RemoteDesktopInfo* pVideoInfo);
	void VRequestResponse(const EB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(const EB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void VideoTimeout(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
	void OnUserEmpInfo(const EB_MemberInfo* pMemberInfo, bool bSort);
#endif
	void OnRemoveGroup(eb::bigint nGroupId);
	void OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId);
	bool OnResourceMove(const EB_ResourceInfo& pResourceInfo,eb::bigint nOldParentResId);
	bool OnResourceInfo(const EB_ResourceInfo& pResourceInfo);
	bool OnResourceDelete(const EB_ResourceInfo& pResourceInfo);

	void OnAlertingCall(void);
	//void OnCall2Group(void);
	//void ShowDialog(void);
	//void EnterRoom(void);
#ifdef USES_EBCOM_TEST
	void ChangeDepartmentInfo(IEB_GroupInfo* pGroupInfo);
#else
	void ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo);
#endif
	bool ClearUnreadMsg(bool bFromUserClick);
	int GetUnreadMsgCount(void) const;

	//void ExitRoom(int nExitSes);
	void UserExitRoom(eb::bigint nUserId, bool bExitDep);
	//void ReceiveMsg(CChatMsgInfo::pointer pChatMsgInfo);
	//Cchatroom::pointer m_pChatRoom;
	////bool m_bReceiveOffLineMsg;
	////tstring m_sLogonAccount;
	//CEBCCallInfo::pointer m_pCallInfo;
	//CEBAccountInfo::pointer m_pFromAccountInfo;

// Dialog Data
	enum { IDD = IDD_DLG_DIALOG };

protected:
	CNewMenu m_menuRemoteDesktop;
	CNewMenu m_menuFunc;
	//CNewMenu m_menuState;
	int m_nChatFuncSize;
	bool m_bOwnerCall;
	//CComPtr<IEBRemoteDesktop>	theEBRemoteDesktop;

	CEBCCallInfo::pointer m_pEbCallInfo;
	tstring m_sFromName;
	CString m_sFullName;
	CString m_sShortName;
	tstring m_sFromDescription;
	Gdiplus::Image* m_pFromImage;
	EB_USER_LINE_STATE m_nFromLineState;
	bool m_bDeleteThis;
	//CDlgUserList* m_pDlgUserList;
	CDlgChatInput::pointer m_pDlgChatInput;
	CSplitterControl m_wndSplitter;
	CDlgChatRight::pointer m_pDlgChatRight;
	EB_GROUP_TYPE m_nGroupType;
	CDlgSelectUser m_pDlgSelectUser;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;
	CTraButton m_btnSendFile;
	CTraButton m_btnVideo;
	CTraButton m_btnRemoteDesktop;
	CTraButton m_btnGroupShare;
	CTraButton m_btnAddUser;
	CTraButton m_btnExitChat;
	CTraButton m_btnChatFunc;
	bool m_bShowedRight;
	CTraButton m_btnSwitchRight;
	void DoResize(int delta, bool bInvalidate = true);

	LRESULT OnMessageSelectedEmp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageExitChat(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageEBSC(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageEBSCOK(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMsgRecord(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageGroupShare(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAdjustWidth(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndMax(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam);

	virtual void OnDraw(void);
	void DrawInfo(void);
	void MoveSize(int cx, int cy);

	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonAddUser();
	afx_msg void OnBnClickedButtonSendFile();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonExitChat();
	afx_msg void OnBnClickedButtonChatFunc();
	afx_msg void OnDropFiles(HDROP hDropInfo);
public:
	afx_msg void OnBnClickedButtonVideo();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBnClickedButtonGroupShare();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRdControlMe(void);
	afx_msg void OnRdControlDest(void);
	afx_msg void OnSubscribeFunc(UINT nID);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonRd();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonSwitchRight();
};
const CDlgDialog::pointer NullDlgDialog;