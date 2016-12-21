#pragma once


class CToolbarCallback
{
public:
	virtual tstring GetSelectString(void) const = 0;
	virtual void OnDeleteMsg(mycp::bigint nMsgId) = 0;

};
// CDlgToolbar dialog

class CDlgToolbar : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgToolbar)

public:
	CDlgToolbar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgToolbar();

	EB_CallInfo m_pCallInfo;
	void SetCtrlColor(bool bInvalidate = true);
	int SetMoveEnterData(EB_MR_CTRL_DATA_TYPE nDataType, const std::string& sString, mycp::bigint nMsgId, bool& pOutChangeData);
	void SetMsgHwnd(HWND hWnd, CToolbarCallback* pCallback);
	void HideReset(void);
	void SetChildMode(bool bChildMode);

// Dialog Data
	enum { IDD = IDD_DLG_TOOLBAR };

protected:
	EB_MR_CTRL_DATA_TYPE m_nDataType;
	int m_nButtonCount;
	tstring m_sMoveEnterString;
	mycp::bigint m_nMsgId;
	mycp::bigint m_nMsgFromUserId;
	int m_nMsgType;
	int m_nMsgReadFlag;
	HWND m_hMsgWnd;
	CToolbarCallback* m_pCallback;
	bool m_bChildMode;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnOpen1;
	CTraButton m_btnOpen2;
	CTraButton m_btnOpen3;
	CTraButton m_btnOpen4;
	CTraButton m_btnOpen5;
	CTraButton m_btnOpen6;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtn1();
	afx_msg void OnBnClickedBtn2();
	afx_msg void OnBnClickedBtn3();
	afx_msg void OnBnClickedBtn4();
	afx_msg void OnBnClickedBtn5();
	afx_msg void OnBnClickedBtn6();
};
