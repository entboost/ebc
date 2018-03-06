#pragma once
#include "CDialogSK.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "../resource.h"		// main symbols
#include "PeakMeterCtrl.h"

class CDlgVoiceBar : public CDialogSK
{ 
	DECLARE_DYNAMIC(CDlgVoiceBar)

public:
	CDlgVoiceBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVoiceBar();

	bool SetVoiceValue(int nValue);

// Dialog Data
	enum { IDD = IDD_DLG_VOICEBAR };

protected:
	time_t	m_tReceiveVoiceTime;
	int		m_nLastVoiceValue;
	int		m_nSamCount;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CPeakMeterCtrl	m_PeakMeter1;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
