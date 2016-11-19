#pragma once


// CDlgEditAccInfo dialog
/*
 * 个人中心基本信息窗口类
 * 用于显示（或修改）个人基本资料。
 * 
*//////////////////////////////////////////////////////

class CDlgEditAccInfo : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgEditAccInfo)

public:
	CDlgEditAccInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditAccInfo();

	void OnAreaInfo(const EB_AreaInfo* pAreaInfo,unsigned int nParameter);
	void Load(void);
	void Save(void);
	void CheckData(void);
	void SetCtrlColor(void);

// Dialog Data
	enum { IDD = IDD_DLG_EDIT_ACCINFO };

protected:
	CBrush m_brush;
	EB_AccountInfo m_pMyAccountInfo;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editAccount;
	CHoverEdit	m_editUserName;
	CBitComboBox m_comboGender;
	CBitComboBox m_comboBirYear;
	CBitComboBox m_comboBirMonth;
	CBitComboBox m_comboBirDay;
	CHoverEdit	m_editCellPhone;
	CHoverEdit	m_editWorkPhone;
	CHoverEdit	m_editEmail;
	CHoverEdit	m_editUrl;
	CBitComboBox m_comboArea1;
	CBitComboBox m_comboArea2;
	CBitComboBox m_comboArea3;
	CBitComboBox m_comboArea4;
	CHoverEdit	m_editAddress;
	CHoverEdit	m_editZipCode;
	CHoverEdit	m_editDescription;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnCbnSelchangeComboArea1();
	afx_msg void OnCbnSelchangeComboArea2();
	afx_msg void OnCbnSelchangeComboArea3();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
