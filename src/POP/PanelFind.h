#pragma once

#include "PanelText.h"

#define DEFAULT_PANEL_FIND_WIDTH	280
#define DEFAULT_PANEL_FIND_HEIGHT	38
#define DEFAULT_PANEL_FIND_RIGHT	20

class CFindHandler
{
public:
	virtual void OnFindClose(void) = 0;
	virtual void OnFindText(const char* lpszFindText, bool bFindUp, bool bMatchCase, bool bFindNext) = 0;
};

// CPanelFind dialog

class CPanelFind : public CEbDialogBase
{
	DECLARE_DYNAMIC(CPanelFind)

public:
	CPanelFind(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelFind();

	void SetCtrlColor(bool bInvalidate);
	void SetFindFocus(void);
	void SetFindHandler(CFindHandler* pHandler) {m_pFindHandler = pHandler;}
	void Cef_OnFindResult(int count,int activeMatchOrdinal,bool finalUpdate);
	void SetFindResult(int nCurrentIndex, int nFindCount);
	void OnMove(void);
	void OnShowHide(bool bShow);
	void SetUserData(DWORD dwUserData) {m_dwUserData = dwUserData;}
	DWORD GetUserData(void) const {return m_dwUserData;}

// Dialog Data
	enum { IDD = IDD_PAN_FIND };

protected:
	CFindHandler* m_pFindHandler;
	bool m_bFindUp;
	CString m_sCurentFindText;
	DWORD m_dwUserData;

	int m_nFindResultCountTemp;
	int m_nFindResultIndexTemp;

	CPanelText * m_pPanelText;
	void Fire_OnFind(void);
	void Fire_OnClose(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CHoverEdit	m_editFindText;
	//CLabelEx m_labelResult;
	CTraButton m_btnFindUp;
	CTraButton m_btnFindDown;
	CTraButton m_btnClose;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonFindup();
	afx_msg void OnBnClickedButtonFinddown();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnEnChangeEditFindtext();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMove(int x, int y);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
