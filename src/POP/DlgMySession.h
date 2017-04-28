#pragma once
#include "Core/VividTree.h"

class CDlgViewContactInfo;
/*
 * 主界面，最近会话窗口类
 * 显示最近50条，有联系的会话，可以直接点击进入聊天会话。
 * 
*//////////////////////////////////////////////////////

class CCallRecordInfo
{
public:
	typedef boost::shared_ptr<CCallRecordInfo> pointer;
	static CCallRecordInfo::pointer create(void)
	{
		return CCallRecordInfo::pointer(new CCallRecordInfo());
	}

	eb::bigint m_sCallId;
	time_t m_tTime;
	eb::bigint m_sGroupCode;
	tstring m_sGroupName;
	eb::bigint m_sMemberCode;
	eb::bigint m_nFromUserId;
	eb::bigint m_nFromPhone;
	tstring m_sFromAccount;
	tstring m_sFromName;
	int m_nFromType;
	tstring m_sCompany;
	tstring m_sTitle;
	//tstring m_sPhone;
	tstring m_sTel;
	tstring m_sEmail;
	HTREEITEM m_hItem;
	bool m_bRead;
	CCallRecordInfo(void)
		: m_tTime(0)
		, m_hItem(NULL)
		, m_nFromType(0)
		, m_bRead(true)
	{
		m_sCallId = 0;
		m_sGroupCode = 0;
		m_sMemberCode = 0;
		m_nFromUserId = 0;
		m_nFromPhone = 0;
	}
};
const CCallRecordInfo::pointer NullCallRecordInfo;
// CDlgMySession dialog

class CDlgMySession : public CDialog
	, public CTreeCallback
{
	DECLARE_DYNAMIC(CDlgMySession)

public:
	CDlgMySession(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMySession();

	void InsertCallRecord(const CCallRecordInfo::pointer& pCallRecordInfo, bool bInsertNew);
	//void InsertCallRecord(const CCallRecordInfo::pointer& pCallRecordInfo,bool bSysMsg, bool bInsertNew);
	CCallRecordInfo::pointer GetCallRecordInfo(eb::bigint sDepCode, eb::bigint sAccount, int nMsgType = 0) const;
	//CCallRecordInfo::pointer GetCallRecordInfo(eb::bigint sDepCode, eb::bigint sAccount, bool bSysMsg = false) const;
	void SetCtrlColor(void);

// Dialog Data
	enum { IDD = IDD_DLG_MY_SESSION };

protected:
	CDlgViewContactInfo* m_pViewContactInfo;
	CLockMap<HTREEITEM, CCallRecordInfo::pointer> m_pCallRecordInfo;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnDeleteTrack;
	CTraButton m_btnCallTrack;
	CTraButton m_btnAddContact;
	VividTree m_treeSession;
	HTREEITEM m_hCurrentHotItem;
	bool m_bReloadRecordData;

	void DeleteMsgRecord(const CCallRecordInfo::pointer& pCallRecordInfo, bool bQuesiton);
	void DeleteItem(HTREEITEM hItem);
	void CallItem(HTREEITEM hItem);

	bool GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,Gdiplus::Image*& pImage3,int& pState) const;
	virtual COLORREF GetItemTextColor(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, COLORREF colorDefault) const;
	LRESULT OnTreeItemDblclk(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMRClickTreeSession(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSessionDeleteSes();
	LRESULT OnMsgDeleteSes(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgNewContact(WPARAM wp, LPARAM lp);
	afx_msg void OnSessionClearSes();
	afx_msg void OnDestroy();
	afx_msg void OnCallUser();
	afx_msg void OnViewMsgRecord();
	afx_msg void OnDeleteMsgRecord();
	afx_msg void OnSubscribeFunc(UINT nID);
	afx_msg void OnGroupShare();

};
