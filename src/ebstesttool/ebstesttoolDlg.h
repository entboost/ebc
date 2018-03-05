
// ebstesttoolDlg.h : header file
//

#pragma once
#include "../include/POPCLogonCenter.h"
#include "afxcmn.h"
using namespace mycp;
using namespace entboost;

#define DEFAULT_MAX_THREAD_COUNT	100

// CebstesttoolDlg dialog
class CebstesttoolDlg : public CDialog
	, public CPOPLCHandler
{
// Construction
public:
	CebstesttoolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EBSTESTTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	int m_nReturnCount;
	int m_nSendCount;
	int m_nDuration;
	DWORD m_tTestStart;
	int m_nMinTime;
	int m_nMaxTime;
	int m_nNAckCount;	// NCAKÊýÁ¿
	int m_nUseTime;
	boost::shared_mutex m_pRichMutex; 
	boost::shared_mutex m_pSendMutex; 

	//virtual void OnSessionOpen(int nResultCode,const tstring& sSessionId,const CPOPCLogonCenter* pLCOwner);
	virtual void OnLCULQInfoResponse(const CPOPSotpRequestInfo::pointer & pReqeustInfo, const CPOPSotpResponseInfo::pointer & pResponseInfo,const CPOPCLogonCenter* pLCOwner);

	//static void process_thread_svr(void* lparam);
	void DoProcess(void);
	bool m_bKilled;
	std::vector<boost::shared_ptr<boost::thread> > m_pProcessThread;
	//boost::thread * m_pProcessThread[DEFAULT_MAX_THREAD_COUNT];

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonTestVisitor();
	afx_msg void OnBnClickedButtonTestIo();
	CString m_sServer;
	int m_nCount;
	int m_nThreadCount;
	CRichEditCtrl m_pRichEdit;
public:
	afx_msg void OnDestroy();
protected:
	virtual void OnOK();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
