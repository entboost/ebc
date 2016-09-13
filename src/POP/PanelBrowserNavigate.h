#pragma once


// CPanelBrowserNavigate dialog

class CPanelBrowserNavigate : public CEbDialogBase
{
	DECLARE_DYNAMIC(CPanelBrowserNavigate)

public:
	CPanelBrowserNavigate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelBrowserNavigate();

// Dialog Data
	enum { IDD = IDD_PAN_BROWSER_NAVIGATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnGoBack;
	CTraButton m_btnGoForward;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonGoback();
	afx_msg void OnBnClickedButtonGoforward();
	afx_msg void OnPaint();
};
