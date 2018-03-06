#pragma once
//#include "usermanagerhandle.h"

// CDlgIncomingCall dialog

class CDlgIncomingCall : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgIncomingCall)

public:
	typedef boost::shared_ptr<CDlgIncomingCall> pointer;
	CDlgIncomingCall(CWnd* pParent = NULL, bool bDeleteThis = true);   // standard constructor
	virtual ~CDlgIncomingCall();
	static CDlgIncomingCall::pointer create(CWnd * pParent = NULL)
	{
		return CDlgIncomingCall::pointer(new CDlgIncomingCall(pParent, false));
	}

	EB_AccountInfo m_pFromAccountInfo;
	EB_CallInfo m_pCallInfo;

// Dialog Data
	enum { IDD = IDD_DLG_INCOMING_CALL };

protected:
	bool m_bDeleteThis;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnAccept;
	CTraButton m_btnReject;

	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonAccept();
	afx_msg void OnBnClickedButtonReject();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

