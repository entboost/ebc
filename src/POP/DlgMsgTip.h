#pragma once
#include "afxcmn.h"


// CDlgMsgTip dialog

class CDlgMsgTip : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgMsgTip)

public:
	CDlgMsgTip(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMsgTip();

	void AddSubMsgTip(eb::bigint nId, eb::bigint nSubId, const CString& sMsgTip, const CString& sParam, eb::bigint nMsgId);
	void AddEmailMsgTip(eb::bigint nId, eb::bigint nEmailSubId, const CString& sMsgTip, const CString& sParam);
	void DelEmailMsgTip(eb::bigint nId);
	void AddMsgTip(eb::bigint nGroupId, eb::bigint nFromUserId, const CString& sMsgTip);
	void DelMsgTip(eb::bigint nGroupId, eb::bigint nFromUserId);
	bool IsMsgEmpty(void) const;
	void SetCurrentTrayIconPos(void);
	void ShowMsgTip(void);
	void ViewAllMsg(void);
	void CheckPosInMsgArea(void);
	void SetCtrlColor(void);

// Dialog Data
	enum { IDD = IDD_DLG_MSG_TIP };

protected:
	CPoint m_posTrayIcon;
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pIdItemInfo;		// id->
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pGroupItemInfo;	// dep_code->
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pUserItemInfo;	// user_id->
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnSetting;
	CTraButton m_btnClose;
	CTraButton m_btnDeleteTrack;
	VividTree m_pTreeMessage;
	CTraButton m_btnDeleteAll;
	CRect m_rectDeleteAll;
	CTraButton m_btnViewAll;
	CRect m_rectViewAll;

	EB_SubscribeFuncInfo m_pEmailSubscribeFuncInfo;
	void CallItem(HTREEITEM hItem);
	void DeleteItem(HTREEITEM hItem);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnNMClickTreeMessage(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonViewall();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
