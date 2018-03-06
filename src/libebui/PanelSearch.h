#pragma once


// CPanelSearch dialog

class CPanelSearch : public CEbDialogBase
{
	DECLARE_DYNAMIC(CPanelSearch)

public:
	CPanelSearch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSearch();

// Dialog Data
	enum { IDD = IDD_PAN_SEARCH };

protected:
	CMenu m_pOptionsMenu;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnGoBack;
	CTraButton m_btnGoForward;
	CTraButton m_btnRefresh;
	CTraButton m_btnOptions;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCmdClearIeTempFile();
	afx_msg void OnCmdClearHistory();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonOptions();
	afx_msg void OnBnClickedButtonGoback();
	afx_msg void OnBnClickedButtonGoforward();
	afx_msg void OnDestroy();
};
