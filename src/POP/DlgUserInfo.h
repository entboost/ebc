#pragma once

#include "afxwin.h"

// CDlgUserInfo dialog
/*
 * 电子名片信息显示窗口类
 * 用于在一对一聊天界面右边，显示聊天对方用户基本信息。
 * 
*//////////////////////////////////////////////////////

class CDlgUserInfo : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgUserInfo)

public:
	CDlgUserInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUserInfo();

	EB_CallInfo m_pCallInfo;
	EB_AccountInfo m_pFromAccountInfo;

// Dialog Data
	enum { IDD = IDD_DLG_USERINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CLabelEx m_staName;
	CEbTranEdit m_editName;
	CEbTranEdit m_editAccount;
	CEbTranEdit m_editTel;
	CEbTranEdit m_editPhone;
	CEbTranEdit m_editEmail;
	CEbTranEdit m_editJobTitle;
	CEbTranEdit m_editGroup;
	CEbTranEdit m_editEnterprise;
	CEbTranEdit m_editAddress;
	//void DrawInfo(void);

	//void InvalidateParentRect(UINT nID);
	DECLARE_MESSAGE_MAP()
	//afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
