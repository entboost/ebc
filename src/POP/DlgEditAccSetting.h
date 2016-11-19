#pragma once


// CDlgEditAccSetting dialog
/*
 * 个人中心配置信息窗口类
 * 用于显示（或修改）个人配置信息，如是否接受离线文件等。
 * 
*//////////////////////////////////////////////////////

class CDlgEditAccSetting : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgEditAccSetting)

public:
	CDlgEditAccSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditAccSetting();

	void Load(void);
	void Save(void);
	void SetCtrlColor(void);

// Dialog Data
	enum { IDD = IDD_DLG_EDIT_ACCSETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CLabelEx m_bEnableOutEntCall;
	CLabelEx m_bEnableOutEntAccept;
	CLabelEx m_bEnableUserCall;
	CLabelEx m_bEnableUserAccept;
	CLabelEx m_bEnableVisitorCall;
	CLabelEx m_bEnableVisitorAccept;
	CLabelEx m_bEnableOffCall;
	CLabelEx m_bEnableOffFile;
	CLabelEx m_bEnableContactAccept;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
