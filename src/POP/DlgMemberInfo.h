#pragma once
#include "afxwin.h"
#include "DlgChangeHead.h"


// CDlgMemberInfo dialog
/*
 * 群组成员（或部门员工）信息显示类
 * 用于显示（或修改）群组成员（或部门员工）的资料。
 * 
*//////////////////////////////////////////////////////

class CDlgMemberInfo : public CEbDialogBase
	, public CImageSelectCallback
{
	DECLARE_DYNAMIC(CDlgMemberInfo)

public:
	CDlgMemberInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMemberInfo();

	EB_GROUP_TYPE m_nGroupType;
	eb::bigint m_sGroupCode;
	eb::bigint m_sMemberCode;
	eb::bigint m_sMemberUserId;
	CString m_sMemberAccount;
	CString m_sGroupName;
	CString m_sUserName;
	CString m_sJobTitle;
	int m_nJobPosition;
	CString m_sCellPhone;
	CString m_sWorkPhone;
	CString m_sEmail;
	CString m_sFax;
	CString m_sAddress;
	CString m_sDescription;
	int m_nGender;
	int m_nBirthday;
	EB_MemberInfo m_pMemberInfo; // 主要用于做资源文件显示（添加和新建）
	//CString m_sHeadResourceFile;

// Dialog Data
	enum { IDD = IDD_DLG_MEMBER_INFO };

protected:
	CRect m_rectHead;
	Gdiplus::Image * m_imageHead;
	CDlgChangeHead* m_pDlgChangeHead;
	int m_nOldFileSize;
	bool m_bSetTimer;

	bool m_bNewEemployee;
	virtual void OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//CHoverEdit	m_editGroupCode;
	//CHoverEdit	m_editMemberCode;
	CHoverEdit	m_editMemberAccount;
	CHoverEdit	m_editGroupName;
	CHoverEdit	m_editUserName;
	CHoverEdit	m_editJobTitle;
	CHoverEdit	m_editJobPosition;
	CHoverEdit	m_editCellPhone;
	CHoverEdit	m_editWorkPhone;
	CHoverEdit	m_editEmail;
	CHoverEdit	m_editFax;
	CHoverEdit	m_editAddress;
	CHoverEdit	m_editDescription;
	CTraButton m_btnClose;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;
	CTraButton m_btnChangeHead;
	CTraButton m_btnDefaultEmp;

	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CBitComboBox m_comboGender;
	CBitComboBox m_comboBirYear;
	CBitComboBox m_comboBirMonth;
	CBitComboBox m_comboBirDay;
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonDefaultEmp();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonChangehead();
	LRESULT OnMsgChangeHead(WPARAM wp, LPARAM lp);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
