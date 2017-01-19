#pragma once


// CDlgRequestAddContact dialog

class CDlgRequestAddContact : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgRequestAddContact)

public:
	CDlgRequestAddContact(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRequestAddContact();

	tstring m_sHeadFilePath;
	tstring m_sHeadName;
	CString m_sDescription;
// Dialog Data
	enum { IDD = IDD_DLG_REQUEST_ADD_CONTACT };

protected:
	CRect m_rectHead;
	RectF m_rectName;
	void DrawInfo(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editDescription;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
