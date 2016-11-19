#pragma once

#include "EBCCallInfo.h"

// CDlgRDWindows dialog
/*
 * 远程桌面窗口类
 * 用于“远程桌面”，“远程协助”连接时，显示对方桌面。
 * 
*//////////////////////////////////////////////////////

class CDlgRDWindows : public CDialog
{
	DECLARE_DYNAMIC(CDlgRDWindows)

public:
	typedef boost::shared_ptr<CDlgRDWindows> pointer;
	CDlgRDWindows(CWnd* pParent = NULL, bool bDeleteThis = true);   // standard constructor
	virtual ~CDlgRDWindows();
	static CDlgRDWindows::pointer create(CWnd * pParent = NULL)
	{
		return CDlgRDWindows::pointer(new CDlgRDWindows(pParent, false));
	}

	void SetTitle(const tstring& sTitle) {m_sTitle = sTitle;}
	void SetCallId(eb::bigint nCallId) {m_nCallId = nCallId;}
	void SetUserId(eb::bigint nUserId) {m_nUserId = nUserId;}

// Dialog Data
	enum { IDD = IDD_DLG_RD_WINDOWS };

protected:
	tstring m_sTitle;
	eb::bigint m_nCallId;
	eb::bigint m_nUserId;
	bool m_bDeleteThis;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnMin;
	CTraButton m_btnMax;
	CTraButton m_btnClose;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//virtual void OnOK();
	//virtual void OnCancel();
	afx_msg void OnClose();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonMax();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
};
