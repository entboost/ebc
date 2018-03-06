#pragma once
#include "EBFuncButton.h"

class CDlgAppWindow;

#define DEFAULT_RETURN_BTN_WIDTH 14
#define DEFAULT_TRIANGLE_BTN_WIDTH 7
#define DEFAULT_TRIANGLE_BTN_HEIGHT 14
#define WM_RETURN_MAINFRAME WM_USER+21

class CAppWindowInfo
{
public:
	typedef boost::shared_ptr<CAppWindowInfo> pointer;
	static CAppWindowInfo::pointer create(const std::string& sAppUrl)
	{
		return CAppWindowInfo::pointer(new CAppWindowInfo(sAppUrl));
	}

	void SetAppUrl(const std::string& sUrl);
	const std::string& GetAppUrl(void) const {return m_sAppUrl;}
	CString GetLocationURL(void);
	eb::bigint GetSubscribeId(void) const;
	BOOL Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID, const EB_SubscribeFuncInfo& pFuncInfo, bool nOpenNewClose, int nLeft);
	void MoveSize(int left, int cx, int cy);

	void SetCtrlColor(void);
	void SetUserData(unsigned int nUserData) {m_nUserData = nUserData;}
	unsigned int GetUserData(void) const {return m_nUserData;}
	CDlgAppWindow* GetAppWindow(void) const {return m_pAppWindow;}
	//void SetFunInfo(const EB_SubscribeFuncInfo& pFuncInfo);

	bool IsChecked(void) const {return m_btn.GetChecked();}
	bool IsCheckedFocus(void);
	bool IsCheckedRefresh(void);
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
	void CheckMousePos(const POINT& point){
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

	bool IsClickClose(POINT point) const {return m_rectClose.PtInRect(point)?true:false;}
	void SendClose(void);
	void ShowHide(bool bShowAndChecked);
	void SetTitle(const CString& sTitle);
	void SetIco(void);
	void doRefresh(void);
	void goBack(void);
	void goForward(void);

public:
	CAppWindowInfo(const std::string& sAppUrl);
	virtual ~CAppWindowInfo(void);
private:
	std::string m_sAppUrl;
	CRect m_rectBtn;
	CTraButton m_btn;
	CRect m_rectClose;
	CTraButton m_close;
	CDlgAppWindow* m_pAppWindow;
	unsigned int m_nUserData;
};

// CDlgAppFrame dialog

class CDlgAppFrame : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgAppFrame)

public:
	CDlgAppFrame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAppFrame();

	void SetCtrlColor(void);
	//void OnMove(void);

	void doRefresh(void);
	void goBack(void);
	void goForward(void);
	void SetCurrentFocus(void);
	//void RefreshWindow(void);
	bool AddSubscribeFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo);
	void AddAppUrl(const std::string& sAppUrl, const EB_SubscribeFuncInfo& pFuncInfo, bool nOpenNewClose=false);
	bool IsEmpty(void) const;

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

	void ClickFrame(unsigned int nWndIndex);
	void CheckMousePos(const POINT& point);
	void ShowApp(unsigned int nWndIndex);
	void DelApp(unsigned int nWndIndex);
	void DelApp(const CWnd* pWnd);
	void SetAppTitle(const CWnd* pWnd, const CString& sTitle);
	void SetAppIco(const CWnd* pWnd);
	void SendMsgChangeUrl(const CAppWindowInfo::pointer& pAppWindowInfo);
	void ShowFirstWnd(void);
	void RebuildBtnSize(void);
	void RefreshAppWnd(void);

	void MoveSize(int cx, int cy);
	void MoveWindowSize(int cx, int cy);
	LRESULT OnFrameWndClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndTitle(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameWndOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnFrameViewIco(WPARAM wParam, LPARAM lParam);
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
};
