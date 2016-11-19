#pragma once

// CDlgAlertingCall dialog
/*
 * 振铃响应类
 * 用于第一次请求跟外部企业员工（或用户）聊天时，本端等待对方接受应答聊天窗口，显示相应会话信息。
 * 
*//////////////////////////////////////////////////////

class CDlgAlertingCall : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgAlertingCall)

public:
	typedef boost::shared_ptr<CDlgAlertingCall> pointer;
	CDlgAlertingCall(CWnd* pParent = NULL, bool bDeleteThis = true);   // standard constructor
	virtual ~CDlgAlertingCall();
	static CDlgAlertingCall::pointer create(CWnd * pParent = NULL)
	{
		return CDlgAlertingCall::pointer(new CDlgAlertingCall(pParent, false));
	}

	EB_AccountInfo m_pFromAccountInfo;
	eb::bigint m_sCallId;

// Dialog Data
	enum { IDD = IDD_DLG_ALERTING_CALL };

protected:
	bool m_bDeleteThis;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnHangup;

	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonHangup();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
