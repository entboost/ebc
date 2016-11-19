#pragma once
#include "Core/TraButton.h"


class CDlgEditAccInfo;
class CDlgEditAccSetting;
class CDlgEditPasswd;
class CDlgEditVideo;

// CDlgEditInfo dialog
/*
 * 个人中心基础窗口类
 * 管理个人中心几个窗口对象。
 * 
*//////////////////////////////////////////////////////

class CDlgEditInfo : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgEditInfo)

public:
	CDlgEditInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditInfo();

	void OnAreaInfo(const EB_AreaInfo* pAreaInfo,unsigned int nParameter);
	void CheckData(void);
	void SetCtrlColor(void);
// Dialog Data
	enum { IDD = IDD_DLG_EDIT_INFO };

protected:
	HWND m_pCurrentLabel;
	CDlgEditAccInfo* m_pDlgEditAccInfo;
	CDlgEditAccSetting* m_pDlgEditAccSetting;
	CDlgEditPasswd* m_pDlgEditPasswd;
	CDlgEditVideo* m_pDlgEditVideo;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnClose;
	CTraButton m_btnAccInfo;
	CTraButton m_btnAccSetting;
	CTraButton m_btnPasswd;
	CTraButton m_btnVideo;
	CTraButton m_btnDefaultEmp;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;

	void RefreshLabelWindow(void);
	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonAccinfo();
	afx_msg void OnBnClickedButtonAccSetting();
	afx_msg void OnBnClickedButtonPasswd();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonVideo();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnMove(int x, int y);
public:
	afx_msg void OnBnClickedButtonDefaultEmp();
};
