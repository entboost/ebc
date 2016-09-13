#pragma once


// CDlgEditPasswd dialog

class CDlgEditPasswd : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgEditPasswd)

public:
	CDlgEditPasswd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditPasswd();

	void Save(void);
	void SetCtrlColor(void);

// Dialog Data
	enum { IDD = IDD_DLG_EDIT_PASSWD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editOldPwd;
	CHoverEdit	m_editNewPwd;
	CHoverEdit	m_editConfirm;

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
