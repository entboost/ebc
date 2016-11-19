#pragma once

class CDlgUserList;
class CPanelVideos;
class CPanelRemoteDesktop;
class CDlgUserInfo;
class CPanelFiles;
class CDlgMsgRecord;
class CDlgResourceMgr;
class CDlgAppWindow;

class CRightTabInfo
{
public:
	typedef boost::shared_ptr<CRightTabInfo> pointer;
	static CRightTabInfo::pointer create(void)
	{
		return CRightTabInfo::pointer(new CRightTabInfo());
	}
	bool Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID, bool bShowClose=true);
	void SetTabText(LPCTSTR lpszCaption);
	UINT GetCtrlID(void) const {return m_btn.GetSafeHwnd()==NULL?0:m_btn.GetDlgCtrlID();}
	//const CDlgAppWindow* GetDlgAppWindow(void) const {return m_pAppWindow;}
	int GetTabWidth(void) const {return m_rectBtn.Width();}

	bool IsChecked(void) const {return m_btn.GetChecked();}
	bool IsCheckedRefresh(void);
	void OnMove(void);
	bool IsClickClose(POINT point) const {return (m_close.GetSafeHwnd()!=NULL&&m_rectClose.PtInRect(point))?true:false;}
	void CheckMousePos(POINT point);
	void ShowHide(bool bShowAndChecked);
	void MoveBtnWindow(LPCRECT rect);

	void SetSubscribeFuncInfo(const RECT& rect, const EB_SubscribeFuncInfo& pSubscribeFuncInfo, const std::string& sUrl, const std::string& sPostData, bool bSaveBrowseTitle);
	eb::bigint GetSubscribeId(void) const;
	bool IsAppWindow(HWND hWnd) const;
	void MoveAppWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	bool SendClose(void);
	int GetAppWidth(void) const;
	bool ChangeTabText(const CWnd* pWnd, LPCTSTR lpszCaption);

	CRightTabInfo(void);
	virtual ~CRightTabInfo(void);
private:
	CTraButton m_btn;
	CRect m_rectBtn;
	CTraButton m_close;
	CRect m_rectClose;
	CDlgAppWindow* m_pAppWindow;
};

// CDlgChatRight dialog
/*
 * 聊天对话框界面右边窗口类
 * 用于显示在聊天对话框右边，用于管理包括：电子名片，视频面板，文件传输，历史消息，等窗口。
 * 
*//////////////////////////////////////////////////////

class CDlgChatRight : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgChatRight)

public:
	typedef boost::shared_ptr<CDlgChatRight> pointer;
	CDlgChatRight(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChatRight();
	static CDlgChatRight::pointer create(CWnd * pParent = NULL)
	{
		return CDlgChatRight::pointer(new CDlgChatRight(pParent));
	}
	void SetCallInfo(const EB_CallInfo& pCallInfo, const EB_AccountInfo& pFromAccountInfo);

	void SetCtrlColor(bool bInvalidate = true);
	void RefreshAppWnd(void);
	void LineStateChange(eb::bigint nUserId, EB_USER_LINE_STATE bLineState);
	void ExitRD(void);
	void RefreshWeb(void);
	void OnMove(void);
	void GetProcessing(bool& pVideoProcessing, bool& pFileProcessing, bool& pDesktopProcessing) const;
	int GetNeedWidth(void) const;
	void ExitChat(bool bHangup);
	void OnExitUser(eb::bigint nUserId, bool bExitDep);
	void OnEnterUser(eb::bigint nUserId, const char * sFromInfo,bool bSort);
#ifdef USES_EBCOM_TEST
	void OnUserEmpInfo(IEB_MemberInfo* pMemberInfo);
	void RDRequestResponse(IEB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
#else
	void OnUserEmpInfo(const EB_MemberInfo* pMemberInfo, bool bSort);
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
	void VideoEnd(const EB_VideoInfo* pVideoInfo,const EB_UserVideoInfo* pUserVideoInfo);
#endif
	void DeleteDlgTranFile(eb::bigint nMsgId);
#ifdef USES_EBCOM_TEST
	void OnSendingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo);
	void OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo);
	void SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent);
#else
	void OnSendingFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivingFile(const CCrFileInfo * pCrFileInfo);
	void OnReceivedFile(const CCrFileInfo * pCrFileInfo);
	void SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent);
#endif
	void OnResourceMove(const EB_ResourceInfo& pResourceInfo,eb::bigint nOldParentResId);
	void OnResourceInfo(const EB_ResourceInfo& pResourceInfo);
	void OnResourceDelete(const EB_ResourceInfo& pResourceInfo);

	void OpenVideoPan(void);
	void OpenRdPan(void);
	void OpenFilePan(void);

	EB_CallInfo m_pCallInfo;
	//Cchatroom::pointer m_pChatRoom;
	EB_AccountInfo m_pFromAccountInfo;


// Dialog Data
	enum { IDD = IDD_DLG_CHATRIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//CTraButton m_labelUserList;
	//CTraButton m_labelECard;
	//CTraButton m_labelVideos;
	//CTraButton m_labelRemoteDesktop;
	//CTraButton m_labelFiles;
	CDlgUserList* m_pDlgUserList;
	CDlgUserInfo* m_pDlgUserInfo;
	CPanelVideos* m_pPanVideos;
	CPanelRemoteDesktop* m_pPanRemoteDesktop;
	CPanelFiles* m_pPanFiles;
	CDlgMsgRecord* m_pMsgRecord;
	CDlgResourceMgr* m_pResourceMgr;

	//HWND m_hCurrentSelect;
	//void Update(void);
	CLockList<CRightTabInfo::pointer> m_pTabList;
	CLockList<UINT> m_pAppCtrlIDList;
	void CheckMousePos(void);
	bool ShowTabID(UINT nID, bool bCheckClose=false);	// 0:show first
	bool CloseTabID(UINT nID);
	bool ChangeTabText(UINT nID, LPCTSTR lpszCaption);
	bool ChangeTabText(const CWnd* pWnd, LPCTSTR lpszCaption);
	int GetTabSizeWidth(void) const;
	void CheckAndSendAdjustWidth(void);
	void ChangeTabPos(void);
	void ShowHideTab(const CRightTabInfo::pointer& pTabInfo, bool bShow);
	UINT GetTabIDBySubscribeId(eb::bigint nSubscribeId) const;
	int GetOffsetIndexByHwnd(HWND hWnd) const;

	//LRESULT OnMessageWndClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMsgRecord(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageGroupShare(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndMax(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndTitle(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//afx_msg void OnBnClickedButtonEcard();
	//afx_msg void OnBnClickedButtonVideos();
	//afx_msg void OnBnClickedButtonUserlist();
	//afx_msg void OnBnClickedButtonFiles();
	//afx_msg void OnBnClickedButtonRd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRightTabClicked(UINT nID);
};
