#pragma once


// CDlgSetting dialog

class CDlgSetting : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetting();

	const CString & GetServer(void) const {return m_sServer;}

// Dialog Data
	enum { IDD = IDD_DLG_SETTING };

protected:
	CString m_sServer;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editServer;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

	void DrawInfo(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedButtonClose();
};
