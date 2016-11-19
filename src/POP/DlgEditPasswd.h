#pragma once


// CDlgEditPasswd dialog
/*
 * 个人中心修改密码窗口类
 * 用于修改个人帐号密码，私有云部署如果配置“限制客户端修改帐号密码”，将不能修改。
 * 
*//////////////////////////////////////////////////////

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
