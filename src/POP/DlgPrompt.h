#pragma once


// CDlgPrompt dialog

class CDlgPrompt : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgPrompt)

public:
	CDlgPrompt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPrompt();

	CString m_sTitle;
	CString m_sPrompt;
	CString m_sUserInput;

// Dialog Data
	enum { IDD = IDD_DLG_PROMPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editUserInput;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonClose();
};
