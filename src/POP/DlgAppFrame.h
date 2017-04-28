#pragma once
#include "EBFuncButton.h"

class CDlgAppWindow;

#define DEFAULT_RETURN_BTN_WIDTH 14
//#define DEFAULT_TRIANGLE_BTN_WIDTH 7
//#define DEFAULT_TRIANGLE_BTN_HEIGHT 14
//#define WM_RETURN_MAINFRAME WM_USER+21

/*
 * 工作区窗口信息类
 * 管理工作区标签按钮，窗口信息等。
 * 
*//////////////////////////////////////////////////////

class CAppWindowInfo
{
public:
	typedef boost::shared_ptr<CAppWindowInfo> pointer;
	static CAppWindowInfo::pointer create(const std::string& sAppUrl)
	{
		return CAppWindowInfo::pointer(new CAppWindowInfo(sAppUrl));
	}

	void SetAppUrl(bool bOpenAppUrlInfo, const std::string& sUrl, const std::string& sPostData);
	const tstring& GetAppUrl(void) const {return m_sAppUrl;}
	CString GetLocationURL(void) const;
	bool IsAboutBlank(void) const;
	eb::bigint GetSubscribeId(void) const;
	eb::bigint GetFromSubscribeId(void) const;
	BOOL Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID, CFont * pFont, const EB_SubscribeFuncInfo& pFuncInfo, const std::string& sPostData, bool nOpenNewClose, int nLeft, bool bSaveBrowseTitle);
	//void Update(const RECT& rect, CWnd* pParent, UINT nID, CFont * pFont, int nLeft);
	void MoveSize(int left, int cx, int cy);

	void SetCtrlColor(void);
	void SetUserData(unsigned int nUserData) {m_nUserData = nUserData;}
	unsigned int GetUserData(void) const {return m_nUserData;}
	CDlgAppWindow* GetAppWindow(void) const {return m_pAppWindow;}
	bool IsAppWindow(HWND hWnd) const;
	void SetFunInfo(const EB_SubscribeFuncInfo& pFuncInfo, bool bOpenNewClose);

	bool IsChecked(void) const {return m_btn.GetChecked();}
	bool IsCheckedFocus(void);
	bool IsCheckedRefresh(void);
	bool NotifyMoveOrResizeStarted(void);
	void OnLogonSuccess(void);
	void OnOffline(int nServerState);
	void MoveBtnWindow(LPCRECT rect) {
		m_rectBtn = rect;
		if (m_btn.GetSafeHwnd()!=NULL)
		{
			m_btn.MoveWindow(m_rectBtn);
			m_btn.Invalidate();
		}
		const int const_size = 12;
		m_rectClose.right = m_rectBtn.right-5;
		m_rectClose.left = m_rectClose.right - const_size;
		m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2;
		m_rectClose.bottom = m_rectClose.top+const_size;
		if (m_close.GetSafeHwnd()!=NULL)
			m_close.MoveWindow(m_rectClose);
	}
	void CheckMousePos(const POINT& point);

	bool IsClickClose(POINT point) const {return m_rectClose.PtInRect(point)?true:false;}
	void SendClose(void);
	void ShowHide(bool bShowAndChecked);
	void SetTitle(const CString& sTitle);
	void SetIco(void);
	void doRefreshOrStop(void);
	bool IsLoading(void) const;
	void goBack(void);
	void goForward(void);
	void changeBrowserType(void);
	EB_BROWSER_TYPE queryBrowserType(void) const;
	bool canSaveHistory(void) const;
	void saveHistory(void);
	const tstring& GetWindowText(void) const {return m_sWindowText;}
	void SetWindowText(const tstring& v) {m_sWindowText = v;}

public:
	CAppWindowInfo(const std::string& sAppUrl);
	virtual ~CAppWindowInfo(void);
private:
	tstring m_sAppUrl;
	tstring m_sWindowText;
	CRect m_rectBtn;
	CTraButton m_btn;
	CRect m_rectClose;
	CTraButton m_close;
	CDlgAppWindow* m_pAppWindow;
	unsigned int m_nUserData;
};

// CDlgAppFrame dialog
/*
 * 工作区应用管理类
 * 管理工作区所有窗口。
 * 
*//////////////////////////////////////////////////////

class CDlgAppFrame : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgAppFrame)

public:
	CDlgAppFrame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAppFrame();

	void SetCtrlColor(void);
	void OnMove(void);

	//void RefreshBtnShow(void);	// for USES_SUPPORT_UI_STYLE
	void doRefreshOrStop(void);
	bool IsLoading(void) const;
	void goBack(void);
	void goForward(void);
	void changeBrowserType(void);
	EB_BROWSER_TYPE queryBrowserType(void) const;
	bool canSaveHistory(void) const;
	void saveHistory(void);
	void SetCurrentFocus(void);
	//void RefreshWindow(void);
	bool AddSubscribeFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo);
	bool IsSubscribeFuncInfoEmpty(void) const {return m_pMainFuncButtonList.empty();}
	void AddAppUrl(bool bSaveBrowseTitle, const std::string& sAppUrl, const std::string& sPostData, const EB_SubscribeFuncInfo& pFuncInfo, bool nOpenNewClose=false, int nInsertOffset = -1);
	void AddFromAppFrame(const CDlgAppFrame* pDlgAppFrame);
	bool IsEmpty(void) const;
	size_t GetSize(void) const;
	const CLockList<CAppWindowInfo::pointer>& GetList(void) const {return m_pList;}
	void ResetUserData(const CAppWindowInfo::pointer& pAppWindowInfo);
	void DelAll(void);
	int GetNeedWidth(void) const {return m_rectNewWeb.right;}
	void RefreshAppWnd(void);
	void NotifyMoveOrResizeStarted(void);
	void OnLogonSuccess(void);
	void OnOffline(int nServerState);
	int GetOffsetIndexByHwnd(HWND hHwnd) const;
	void ClearUnreadMsg(mycp::bigint nSubId);
	void AddUnreadMsg(mycp::bigint nSubId);
	void SetUnreadMsg(mycp::bigint nSubId, size_t nUnreadMsgCount);

// Dialog Data
	enum { IDD = IDD_DLG_APPFRAME };

protected:
	std::vector<CEBFuncButton::pointer> m_pMainFuncButtonList;	// for mainframe
	CLockList<int> m_pIndexIdList;
	CLockList<CAppWindowInfo::pointer> m_pList;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnSwitchFrame;
	CRect m_rectSwitchFrame;
	CTraButton m_btnSwitchToolbar;
	CRect m_rectSwitchToolbar;
	bool m_bShowedToolbar;
	CTraButton m_btnNewWeb;
	CRect m_rectNewWeb;
	//CTraButton m_btnMin;
	//CTraButton m_btnMax;
	//CTraButton m_btnClose;

	void ClickFrame(unsigned int nWndIndex);
	void CheckMousePos(const POINT& point);
	bool ShowPrev(void);
	bool ShowNext(void);
	void ShowApp(unsigned int nWndIndex, bool bSearchFocus);
	void DelApp(unsigned int nWndIndex);
	void DelApp(const CWnd* pWnd);
	void SetAppTitle(const CWnd* pWnd, const CString& sTitle);
	void SetAppIco(const CWnd* pWnd);
	void SendMsgChangeUrl(const CAppWindowInfo::pointer& pAppWindowInfo, bool bSearchFocus);
	void ShowFirstWnd(void);
	void RebuildBtnSize(void);

	void MoveSize(int cx, int cy);
	void MoveWindowSize(int cx, int cy);
	LRESULT OnFrameWndClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndTitle(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameViewIco(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgShowRefreshOrStop(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageMoveDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgChangeAppUrl(WPARAM wParam, LPARAM lParam);
	//LRESULT OnMessageMoveOffset(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonSwitchFrame();
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFrameBtnClicked(UINT nID);
	afx_msg void OnMainFrameFunc(UINT nID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonSwitchToolbar();
	afx_msg void OnBnClickedButtonNewWeb();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//afx_msg void OnBnClickedButtonMin();
	//afx_msg void OnBnClickedButtonMax();
	//afx_msg void OnBnClickedButtonClose();
};
