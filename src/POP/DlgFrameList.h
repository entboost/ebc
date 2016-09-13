#pragma once
//#define EB_COMMAND_RAME_WND_MIN		WM_USER+1
//#define EB_COMMAND_RAME_WND_MAX		WM_USER+2
//#define EB_COMMAND_RAME_WND_CLOSE	WM_USER+3

const int theSearchToolbarHeight = 32;

class CPanelSearch;
class CDlgShrinkageBar;
#include "FrameWndInfo.h"
#ifdef USES_FRAMELIST_APPFRAME
class CDlgAppFrame;
#endif

// CDlgFrameList dialog

class CDlgFrameList : public CEbDialogBase
	, public CFrameWndInfoProxy,CFrameWndInfoProxyCallback
{
	DECLARE_DYNAMIC(CDlgFrameList)

public:
	CDlgFrameList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFrameList();

	void SetCtrlColor(bool bInvalidate = true);
#ifdef USES_FRAMELIST_APPFRAME
	void OnLogonSuccess(void);
	void OnOffline(int nServerState);
#endif
	void CheckUnreadMsgTip(bool bShowTip);
	void ShowFrameList(bool bShow=true);
	//void SetBtnMax(bool bMax);
#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame* GetDlgAppFrame(void) const {return m_pDlgAppFrame;}
//#else
//	void SetWorkFrameCallback(CFrameWndInfoProxyCallback* v) {m_pWorkFrameCallback = v;}
#endif
	void SetWorkFrameCallback(CFrameWndInfoProxyCallback* v) {m_pWorkFrameCallback = v;}
	CFrameWndInfo::pointer GetCurrentFrameWndInfo(void) const {return m_pCurrentFrameWndInfo;}
	bool IsShrinkageWin(void) const {return m_bShrinkageWin;}

// Dialog Data
	enum { IDD = IDD_DLG_FRAMELIST };

protected:
	bool m_bOpenOk;
	bool m_bShrinkageWin;	// -1=up 1=down
	CDlgShrinkageBar* m_pDlgShrinkageBar;
	CRect m_rectWin;
	CRect m_rectAdjustOld;
	CFrameWndInfo::pointer m_pCurrentFrameWndInfo;
	//CFrameWndInfo::FRAME_WND_TYPE m_nCurrentFrameWndType;
	CNewMenu m_menuTray;
	bool m_bShowedCloseTabCommand;
#ifdef USES_FRAMELIST_APPFRAME
	CDlgAppFrame* m_pDlgAppFrame;
//#else
//	CFrameWndInfoProxyCallback* m_pWorkFrameCallback;
#endif
	CFrameWndInfoProxyCallback* m_pWorkFrameCallback;

	bool CheckAutoHide(int x, int y, bool bFromShowCheck);
	//void CheckMousePos(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit m_editSearch;
	CTraButton m_btnSwitchLeft;
	CPanelSearch* m_pPanelSearch;
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;

	// CFrameWndInfoProxyCallback
	virtual void OnFrameWndEmpty(void);
	virtual void OnFrameWndShow(const CFrameWndInfo::pointer& pFrameWndInfo, bool bShow);

	void DrawInfo(void);
	LRESULT OnMsgRefreshUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgGoBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgGoForward(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgChangeBrowserType(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgQueryCanSaveHistory(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgQueryBrowserType(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgSaveHistory(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndMin(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndMax(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveOffset(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageAdjustWidth(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnShowHeadOnly();
	afx_msg void OnOpenWorkFrame();
	afx_msg void OnCloseTab();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	void MoveSize(int cx, int cy);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnFrameBtnClicked(UINT nID);
	//afx_msg void OnFrameCloseClicked(UINT nID);
	afx_msg void OnEnChangeEditSearch();
	LRESULT OnMessageReturnMainFrame(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndCount(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSwitchLeft();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonClose();
	void RefreshBtnShow(bool bShow);
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
