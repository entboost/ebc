#pragma once

/*
 * 窗口管理基类
 * 用于主界面和聊天会话列表，管理所有各种窗口，包括工作区窗口、聊天会话窗口等。
 * 
*//////////////////////////////////////////////////////

#define FRAME_BTN_ID_MIN		101
#define FRAME_BTN_ID_WORKFRAME	FRAME_BTN_ID_MIN
#define FRAME_BTN_ID_MAINFRAME	(FRAME_BTN_ID_WORKFRAME+1)
#define FRAME_BTN_ID_BEGIN	(FRAME_BTN_ID_MAINFRAME+1)
#define FRAME_BTN_ID_MAX		200

const int const_frame_height = 36;
const int const_border_left = 140;//135;
const int const_hide_border_left1 = 36;
const int const_hide_border_left2 = const_hide_border_left1+6;
const int const_border_size = 1;

class CFrameWndInfo
{
public:
	typedef boost::shared_ptr<CFrameWndInfo> pointer;
	enum FRAME_WND_TYPE
	{
		FRAME_WND_WORK_FRAME
		, FRAME_WND_MAIN_FRAME
		, FRAME_WND_CALL_DIALOG
		, FRAME_WND_UNKNOWN
	};
	static CFrameWndInfo::pointer create(bool bChildMode,const CDlgDialog::pointer& pDialog)
	{
		return CFrameWndInfo::pointer(new CFrameWndInfo(bChildMode,pDialog));
	}
	static CFrameWndInfo::pointer create(CWnd * pWnd, bool bChildMode, FRAME_WND_TYPE nType)
	{
		return CFrameWndInfo::pointer(new CFrameWndInfo(pWnd,bChildMode,nType));
	}
	void SetUserData(unsigned int nUserData);
	unsigned int GetUserData(void) const {return m_nUserData;}
	const CDlgDialog::pointer& GetDialog(void) const {return m_pDialog;}
	CWnd * GetWnd(void) const {return m_pWnd;}
	CWnd * GetParent(void) const {return m_btn.GetSafeHwnd()==NULL?NULL:m_btn.GetParent();}
	bool IsWndAddress(const CWnd* pWnd) const {return m_pDialog.get()==pWnd || m_pWnd==pWnd;}
	FRAME_WND_TYPE GetType(void) const {return m_nType;}
	eb::bigint GetCallId(void) const {return m_pDialog.get()!=NULL?m_pDialog->GetCallId():0;}
	eb::bigint GetGroupId(void) const {return m_pDialog.get()!=NULL?m_pDialog->GetGroupId():0;}
	eb::bigint GetFromUserId(void) const {return m_pDialog.get()!=NULL?m_pDialog->GetFromUserId():0;}
	void SetCallInfo(const CEBCCallInfo::pointer & pEbCallInfo) {
		if (m_pDialog.get()!=NULL)
			m_pDialog->SetCallInfo(pEbCallInfo);}

	void SetCtrlColor(bool bInvalidate = true);
	BOOL Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID, CFont* pFont, bool bShowClose, bool bShowText);
	//void SetBtnShowHideText(bool bShow);
	void SetBtnText(LPCTSTR lpszBtnText, LPCTSTR lpszToolTipText);
	void SetToolTipText(LPCTSTR lpszToolTipText);
	void ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo);
	void LineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE bLineState);

	bool IsChecked(void) const {return m_btn.GetChecked();}
	void MoveBtnWindow(LPCRECT rect, bool bShowText);
	void SetMsgBtnText(LPCTSTR lpszText);
	void CheckMousePos(POINT point);

	bool IsClickClose(POINT point) const;
	void SendClose(void);
	void ShowHide(bool bShowAndChecked);
	void ClearUnreadMsg(bool bFromUserClick=false);
	void AddUnreadMsg(void);
	void SetUnreadMsg(size_t nUnreadMsgCount);

	void OnExitRD(void) {
		if (m_pDialog.get()!=NULL)
			m_pDialog->ExitRD();
	}
	CFrameWndInfo(void);
	CFrameWndInfo(bool bChildMode, const CDlgDialog::pointer& pDialog);
	CFrameWndInfo(CWnd* pWnd, bool bChildMode, FRAME_WND_TYPE nType);
	virtual ~CFrameWndInfo(void);
private:
	unsigned int m_nUserData;
	CDlgDialog::pointer m_pDialog;
	CWnd * m_pWnd;
	bool m_bChildMode;
	FRAME_WND_TYPE m_nType;
	CRect m_rectBtn;
	CTraButton m_btn;
	bool m_bBtnShowText;
	CString m_sBtnShowText;
	CRect m_rectMsg;
	CTraButton m_msg;
	int m_nUnreadMsgCount;
	CRect m_rectClose;
	CTraButton m_close;
};
const CFrameWndInfo::pointer NullFrameWndInfo;

#define USES_FRAMELIST_APPFRAME
class CFrameWndInfoProxyCallback
{
public:
	virtual void OnFrameWndEmpty(void){}
	virtual void OnFrameWndShow(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow){}
#ifndef USES_FRAMELIST_APPFRAME
	virtual void OnFrameSize(const CFrameWndInfo::pointer& pFrameWndInfo, int cx, int cy){}
	virtual size_t OnGetFrameSize(const CFrameWndInfo::pointer& pFrameWndInfo) const {return 0;}	// for FRAME_WND_WORK_FRAME
#endif
	
};
class CFrameWndInfoProxy
{
protected:
	CLockList<CFrameWndInfo::pointer> m_pList;
	CLockList<CFrameWndInfo::pointer> m_pHideList;
	CLockList<CFrameWndInfo::pointer> m_pCloseList;
	CLockList<int> m_pIndexIdList;

public:
	CFrameWndInfoProxy(CWnd* pParent, bool bChildMode, CFrameWndInfoProxyCallback* pCallback);
	virtual ~CFrameWndInfoProxy(void);
	void SetShowHideLeft(bool bShow);
	void SetCtrlColor(bool bInvalidate);
	void SetChildFont(int nFontSize, LPCTSTR lpszFace);
	void AddWnd(CFrameWndInfo::FRAME_WND_TYPE nType, LPCTSTR lpszName, LPCTSTR lpszTooltip, bool bShow, bool bAutoCall);
	void AddWnd(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow, bool bAutoCall, bool bForceAddToList = false);
	void SetTitle(CFrameWndInfo::FRAME_WND_TYPE nType, LPCTSTR lpszName, LPCTSTR lpszTooltip);
	const CLockList<CFrameWndInfo::pointer>& GetList(void) const {return m_pList;}
	size_t GetSize(void) const {return m_pList.size();}
	void RebuildBtnSize(void);
	bool AddUnreadMsg(eb::bigint nCallId, eb::bigint nMsgId);
	bool SetUnreadMsg(eb::bigint nCallId, size_t nUnreadMsgCount);
	void SetUnreadMsg(CFrameWndInfo::FRAME_WND_TYPE nType, size_t nUnreadMsgCount);	// for FRAME_WND_WORK_FRAME
	void ClearWnd(void);
	void DelWnd(eb::bigint nCallId);
	//void DelWnd(unsigned int nWndIndex);
	void DelWnd(const CWnd* pWnd);
	void ShowWnd(eb::bigint nCallId);
	void ShowWnd(unsigned int nWndIndex);
	void ShowWnd(const CWnd* pWnd);
	void ShowHideCurrent(bool bShow);
	void ClickFrame(unsigned int nWndIndex);
	void OnFirstWorkFrameWndShow(void) const;
	void ShowFirstWnd(void);
	bool ShowUpWnd(void);
	bool ShowDownWnd(void);
	bool ShowOffsetWnd(int nShowOffset);
	bool ExistCallIdWnd(eb::bigint nCallId) const;
	bool RemoveHideWnd(eb::bigint nCallId);
	//bool SetNewCallInfo(eb::bigint nOldCallId, const CEBCCallInfo::pointer & pEbCallInfo);
	CDlgDialog::pointer GetCallIdDialog(eb::bigint nCallId, bool bRemove, bool bAutoCall);
	void ResetUserData(const CFrameWndInfo::pointer& pFrameWndInfo);
	void RefreshChecked(void);

	void OnMove(void);
	void ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo);
	void UserLineStateChange(eb::bigint nGroupCode, eb::bigint nUserId, EB_USER_LINE_STATE nLineState);

#ifdef USES_EBCOM_TEST
	void OnUserEmpInfo(IEB_MemberInfo* pMemberInfo);
#else
	void OnUserEmpInfo(const EB_MemberInfo* pMemberInfo, bool bSort);
#endif
	void OnRemoveGroup(eb::bigint nGroupId);
	void OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId);
	void OnResourceMove(const EB_ResourceInfo& pResourceInfo,eb::bigint nOldParentResId);
	void OnResourceInfo(const EB_ResourceInfo& pResourceInfo);
	void OnResourceDelete(const EB_ResourceInfo& pResourceInfo);
	void OnExitRD(void);

	void CheckMousePos(void);
	void SetCurrentFocus(void);
	bool GetShowedLeft(void) const {return m_bShowedLeft;}

private:
	CWnd * m_pParent;
	bool m_bChildMode;
	CFont m_childFont;
	CFrameWndInfoProxyCallback* m_pCallback;
	unsigned int m_nCurrentHideIndex;
	bool m_bShowedLeft;
};
