#pragma once
#include "afxwin.h"


class CUserDiaInfo
{
public:
	typedef enum DiaType
	{
		DT_CALL_IN
		, DT_CALL_OUT
	};
	typedef enum DiaStatus
	{
		DS_NORMAL
		, DS_ERROR
	};
	CUserDiaInfo(void)
		: m_tBeginTime(0)
		, m_tEndTime(0)
		, m_nType(DT_CALL_IN)
		, m_nStatus(DS_NORMAL)
		, m_nSeconds(0)
	{
	}
	CUserDiaInfo(time_t tBeginTime, time_t tEndTime, DiaType nType, DiaStatus nStatus, unsigned int nSeconds, const std::string& sAccount)
		: m_tBeginTime(tBeginTime)
		, m_tEndTime(tEndTime)
		, m_nType(nType)
		, m_nStatus(nStatus)
		, m_nSeconds(nSeconds)
		, m_sFromAccount(sAccount)
	{
	}
	time_t m_tBeginTime;
	time_t m_tEndTime;
	DiaType m_nType;
	DiaStatus m_nStatus;
	unsigned int m_nSeconds;
	tstring m_sFromAccount;
	tstring m_sText;
};

// CDlgDiaRecord dialog

class CDlgDiaRecord : public CDialog
{
	DECLARE_DYNAMIC(CDlgDiaRecord)

public:
	CDlgDiaRecord(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDiaRecord();

	CString m_sCallNumber;
// Dialog Data
	enum { IDD = IDD_DLG_DIARECORD };

protected:
	std::vector<CUserDiaInfo> m_pDiaList;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnOk;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	CListBox m_listNumbers;
	afx_msg void OnLbnDblclkListNumbers();
	afx_msg void OnLbnSelchangeListNumbers();
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnEnChangeEditNumber();
};
