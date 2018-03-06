#pragma once
#define EBWM_FRAME_WND_MIN		WM_USER+1
#define EBWM_FRAME_WND_MAX		WM_USER+2
#define EBWM_FRAME_WND_CLOSE	WM_USER+3

class CDlgShrinkageBar;

class CFrameWndInfo
{
public:
	typedef boost::shared_ptr<CFrameWndInfo> pointer;
	enum FRAME_WND_TYPE
	{
		FRAME_WND_CALL_DIALOG
	};
	static CFrameWndInfo::pointer create(const CDlgDialog::pointer& pDialog)
	{
		return CFrameWndInfo::pointer(new CFrameWndInfo(pDialog));
	}
	static CFrameWndInfo::pointer create(CWnd * pWnd, FRAME_WND_TYPE nType)
	{
		return CFrameWndInfo::pointer(new CFrameWndInfo(pWnd,nType));
	}
	void SetUserData(unsigned int nUserData) {m_nUserData = nUserData;}
	unsigned int GetUserData(void) const {return m_nUserData;}
	const CDlgDialog::pointer& GetDialog(void) const {return m_pDialog;}
	CWnd * GetWnd(void) const {return m_pWnd;}
	bool IsWndAddress(const CWnd* pWnd) const {return m_pDialog.get()==pWnd || m_pWnd==pWnd;}
	FRAME_WND_TYPE GetType(void) const {return m_nType;}
	eb::bigint GetCallId(void) const {return m_pDialog.get()!=NULL?m_pDialog->GetCallId():0;}
	eb::bigint GetGroupId(void) const {return m_pDialog.get()!=NULL?m_pDialog->GetGroupId():0;}
	eb::bigint GetFromUserId(void) const {return m_pDialog.get()!=NULL?m_pDialog->GetFromUserId():0;}
	void SetCallInfo(const CEBCCallInfo::pointer & pEbCallInfo) {
		if (m_pDialog.get()!=NULL)
			m_pDialog->SetCallInfo(pEbCallInfo);}

	void SetCtrlColor(void){
		if (m_pDialog.get()!=NULL)
			m_pDialog->SetCtrlColor();
		if (m_btn.GetSafeHwnd()!=NULL)
			m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetBgTitle0(),theApp.GetBgColor3());
	}
	BOOL Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID);
	void LineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE bLineState);

	bool IsChecked(void) const {return m_btn.GetChecked();}
	void MoveBtnWindow(LPCRECT rect) {
		m_rectBtn = rect;
		if (m_btn.GetSafeHwnd()!=NULL)
			m_btn.MoveWindow(m_rectBtn);
		const int const_size = 12;
		m_rectClose.right = m_rectBtn.right-5;
		m_rectClose.left = m_rectClose.right - const_size;
		m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2;
		m_rectClose.bottom = m_rectClose.top+const_size;
		if (m_close.GetSafeHwnd()!=NULL)
			m_close.MoveWindow(m_rectClose);
		if (m_msg.GetSafeHwnd()!=NULL)
		{
			CRect rectMsg(rect);
			rectMsg.left = rectMsg.right - 25;
			m_msg.MoveWindow(&rectMsg);
		}
	}
	void SetMsgBtnText(LPCTSTR lpszText) {
		if (m_msg.GetSafeHwnd()!=NULL)
			m_msg.SetWindowText(lpszText);
	}
	void CheckMousePos(POINT point){
		if (m_btn.GetSafeHwnd()==NULL || m_close.GetSafeHwnd()==NULL)
			return;
		if (m_rectBtn.PtInRect(point))
		{
			if (m_rectClose.PtInRect(point))
			{
				//if (m_btn.GetChecked())
				//	m_close.SetDrawClosePic(true,theApp.GetBgColor3(),-1,-1,-1,2);
				//else
				//	m_close.SetDrawClosePic(true,theApp.GetBgTitle0(),-1,-1,-1,2);
				m_close.SetDrawClosePic(true,RGB(250,250,250),-1,-1,-1,2);
				m_close.SetDrawPanel(true,RGB(237,28,36));
			}else
			{
				m_close.SetDrawClosePic(true,RGB(27,27,27),-1,-1,-1,2);
				if (m_btn.GetChecked())
					m_close.SetDrawPanel(true,theApp.GetBgColor3());
				else
					m_close.SetDrawPanel(true,theApp.GetBgTitle0());
			}
			m_close.ShowWindow(SW_SHOW);
			m_close.Invalidate();
		}else if (m_close.IsWindowVisible())
		{
			m_close.ShowWindow(SW_HIDE);
		}
	}

	bool IsClickClose(POINT point) const {
		return m_rectClose.PtInRect(point)?true:false;
	}
	void SendClose(void){
		if (m_pDialog.get()!=NULL)
			m_pDialog->PostMessage(WM_CLOSE, 0, 0);
	}
	void ShowHide(bool bShowAndChecked)
	{
		if (m_pDialog.get()!=NULL)
		{
			m_pDialog->ShowWindow(bShowAndChecked?SW_SHOW:SW_HIDE);
			//if (bShowAndChecked)
			//{
			//	CEBCCallInfo::pointer m_pEbCallInfo = m_pDialog->GetCallInfo();
			//	CDlgFuncWindow * pOldFuncWindow = NULL;
			//	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			//		theApp.m_pTempGroupMsg.find(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pOldFuncWindow,true);
			//	else
			//		theApp.m_pTempFromUserMsg.find(m_pEbCallInfo->m_pFromAccountInfo.GetUserId(),pOldFuncWindow,true);
			//	// **必须放在前面
			//	if (pOldFuncWindow!=NULL)
			//	{
			//		pOldFuncWindow->ShowWindow(SW_HIDE);	// *必须隐藏不让用户点击；
			//		pOldFuncWindow->m_pEbCallInfo.reset();	// *必须设为空，否则会清掉后面窗口
			//		theApp.m_pCloseWnd.add(pOldFuncWindow);
			//	}
			//}
		}
		if (m_btn.GetSafeHwnd()!=NULL)
		{
			if (m_btn.GetChecked()!=bShowAndChecked)
			{
				m_btn.SetChecked(bShowAndChecked);
				m_btn.Invalidate();
			}
		}
		if (bShowAndChecked && m_nUnreadMsgCount>0)
		{
			ClearUnreadMsg();
		}
	}
	void ClearUnreadMsg(void) {
		m_nUnreadMsgCount = 0;
		if (m_msg.GetSafeHwnd()!=NULL)
		{
			m_msg.SetWindowText(_T(""));
		}
	}
	void AddUnreadMsg(void) {
		m_nUnreadMsgCount++;
		if (m_msg.GetSafeHwnd()!=NULL)
		{
			CString stext;
			stext.Format(_T("%d"),m_nUnreadMsgCount);
			m_msg.ShowWindow(SW_HIDE);
			m_msg.SetWindowText(stext);
			m_msg.ShowWindow(SW_SHOW);
		}
	}

	void OnExitRD(void) {
		if (m_pDialog.get()!=NULL)
			m_pDialog->ExitRD();
	}
	CFrameWndInfo(void)
		: m_nUserData(0)
		, m_pWnd(NULL)
		, m_nType(FRAME_WND_CALL_DIALOG)
		, m_nUnreadMsgCount(0)
	{}
	CFrameWndInfo(const CDlgDialog::pointer& pDialog)
		: m_nUserData(0)
		, m_pDialog(pDialog)
		, m_pWnd(NULL)
		, m_nType(FRAME_WND_CALL_DIALOG)
		, m_nUnreadMsgCount(0)
	{}
	CFrameWndInfo(CWnd* pWnd, FRAME_WND_TYPE nType)
		: m_nUserData(0)
		, m_pWnd(pWnd)
		, m_nType(nType)
		, m_nUnreadMsgCount(0)
	{}
	virtual ~CFrameWndInfo(void)
	{
		if (m_pDialog.get()!=NULL)
			m_pDialog->DestroyWindow();
		if (m_btn.GetSafeHwnd()!=NULL)
			m_btn.DestroyWindow();
		if (m_close.GetSafeHwnd()!=NULL)
			m_close.DestroyWindow();
		if (m_msg.GetSafeHwnd()!=NULL)
			m_msg.DestroyWindow();
	}
private:
	unsigned int m_nUserData;
	CDlgDialog::pointer m_pDialog;
	CWnd * m_pWnd;
	FRAME_WND_TYPE m_nType;
	CRect m_rectBtn;
	CTraButton m_btn;
	CTraButton m_msg;
	int m_nUnreadMsgCount;
	CRect m_rectClose;
	CTraButton m_close;
};

// CDlgFrameList dialog

class CDlgFrameList : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgFrameList)

public:
	CDlgFrameList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFrameList();

	void SetCtrlColor(void);
	void AddWnd(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow);
	void RebuildBtnSize(void);
	bool AddUnreadMsg(eb::bigint nCallId);
	void DelWnd(eb::bigint nCallId);
	//void DelWnd(unsigned int nWndIndex);
	void DelWnd(const CWnd* pWnd);
	void ShowWnd(eb::bigint nCallId);
	void ShowWnd(unsigned int nWndIndex);
	void ShowWnd(const CWnd* pWnd);
	void ClickFrame(unsigned int nWndIndex);
	void ShowFirstWnd(void);
	bool ExistCallIdWnd(eb::bigint nCallId) const;
	//bool SetNewCallInfo(eb::bigint nOldCallId, const CEBCCallInfo::pointer & pEbCallInfo);
	CDlgDialog::pointer GetCallIdDialog(eb::bigint nCallId, bool bRemove = false);

	void UserLineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE nLineState);

#ifdef USES_EBCOM_TEST
	void OnUserEmpInfo(IEB_MemberInfo* pMemberInfo);
#else
	void OnUserEmpInfo(const EB_MemberInfo* pMemberInfo);
#endif
	void OnRemoveGroup(eb::bigint nGroupId);
	void OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId);

	void OnExitRD(void);

// Dialog Data
	enum { IDD = IDD_DLG_FRAMELIST };

protected:
	bool m_nShrinkageWin;	// -1=up 1=down
	CDlgShrinkageBar* m_pDlgShrinkageBar;
	CRect m_rectWin;
	//unsigned int m_nCurrentData;
	CLockList<CFrameWndInfo::pointer> m_pList;
	CLockList<CFrameWndInfo::pointer> m_pHideList;
	CLockList<CFrameWndInfo::pointer> m_pCloseList;
	CLockList<int> m_pIndexIdList;

	void CheckMousePos(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit m_editSearch;

	void DrawInfo(void);
	LRESULT OnFrameWndMin(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndMax(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndClose(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnFrameBtnClicked(UINT nID);
	//afx_msg void OnFrameCloseClicked(UINT nID);
	afx_msg void OnEnChangeEditSearch();
};
