#pragma once

const tstring theAppGroupResultName = _T("集成应用搜索：");
const tstring theUrlGroupName("[* *]");
const CString theUrlGroupResultName = _T("最新浏览记录：");

class CPanelSearchResult;
// CPanelSearch dialog
/*
 * 搜索面板工具条
 * 用于主界面、和工作区面板里面，显示搜索工具条。
 * 
*//////////////////////////////////////////////////////

class CPanelSearch : public CEbDialogBase
{
	DECLARE_DYNAMIC(CPanelSearch)

public:
	CPanelSearch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSearch();

	void SetParentMouseMove(BOOL bEnable) {m_bParentMouseMove = bEnable;}
	void SetSearchEditShow(bool bShow);
	void OnMove(void);
	void CheckSearchResultShowHide(void);
	void HideSearchResult(void);
	void ShowSearchResult(void);

// Dialog Data
	enum { IDD = IDD_PAN_SEARCH };

protected:
	BOOL m_bParentMouseMove;
	//CNewMenu m_pOptionsMenu;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnGoBack;
	CTraButton m_btnGoForward;
	CTraButton m_btnRefresh;
	CTraButton m_btnStop;
	CTraButton m_btnOptions;
	CPanelSearchResult * m_pPanelSearchResult;
	//VividTree m_treeSearch;				// itemdata:1=employee;2=contact
	CHoverEdit m_editSearch;
	CString theSearchString;
	bool m_bCanSearch;
	boost::mutex m_mutexSearch;
	std::vector<EB_SubscribeFuncInfo> m_pSubscribeFuncList;	// for menu

	void CreatePanelSearchResult(void);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCmdClearChromeTempFile();
	afx_msg void OnCmdClearIeTempFile();
	afx_msg void OnCmdSaveHistory();
	afx_msg void OnCmdClearHistory();
	afx_msg void OnCmdChangeBrowserType();
	afx_msg void OnCmdFileManager();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonOptions();
	afx_msg void OnBnClickedButtonGoback();
	afx_msg void OnBnClickedButtonGoforward();
	afx_msg void OnDestroy();
	//CBitmap* ConvertIconToBitmap(HICON hIcon);
	//afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	LRESULT OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgSearchSetFocusSel(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowRefreshOrStop(WPARAM wParam, LPARAM lParam);
	void OnEditSearch(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEditSearch();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMove(int x, int y);
};
