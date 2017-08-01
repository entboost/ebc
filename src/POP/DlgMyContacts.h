#pragma once
#include "afxcmn.h"

class CDlgViewContactInfo;

// CDlgMyContacts dialog
/*
 * 主界面，好友联系人窗口类
 * 显示好友联系人信息，支持所有好友联系人操作，包括添加、修改，删除等。
 * 
*//////////////////////////////////////////////////////

class CDlgMyContacts : public CDialog
	//, public CTreeCallback
{
	DECLARE_DYNAMIC(CDlgMyContacts)

public:
	CDlgMyContacts(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMyContacts();

#ifdef USES_EBCOM_TEST
	void UGInfo(IEB_UGInfo* pUGInfo);
	void UGDelete(IEB_UGInfo* pUGInfo);
	void ContactInfo(IEB_ContactInfo* pPopContactInfo);
	void DeleteContact(IEB_ContactInfo* pPopContactInfo);
#else
	void UGInfo(const EB_UGInfo* pUGInfo);
	void UGDelete(const EB_UGInfo* pUGInfo);
	void ContactHeadChange(const EB_ContactInfo* pPopContactInfo);
	void ContactInfo(const EB_ContactInfo* pPopContactInfo);
	void DeleteContact(const EB_ContactInfo* pPopContactInfo);
#endif
	void SetCtrlColor(void);

// Dialog Data
	enum { IDD = IDD_DLG_MY_CONTACTS };

protected:
	CDlgViewContactInfo* m_pViewContactInfo;
	CLockMap<mycp::bigint,CTreeItemInfo::pointer> m_pGroupItemInfo;		// ugid->
	CLockMap<mycp::bigint,CTreeItemInfo::pointer> m_pContactItemInfo;	// con_id->
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnDeleteTrack;
	CTraButton m_btnEditTrack;
	CTraButton m_btnCallTrack;
	VividTree m_treeContacts;
	HTREEITEM m_hCurrentHotItem;
	HTREEITEM m_hEditGroupTempItem;

	//CTreeItemInfo::pointer GetGroupItemInfo(HTREEITEM hItem) const;
	//CTreeItemInfo::pointer GetContactItemInfo(HTREEITEM hItem) const;
	//static int CALLBACK TreeCmpFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	void DeleteItem(HTREEITEM hItem);
	void EditItem(HTREEITEM hItem);
	void CallItem(HTREEITEM hItem);
	LRESULT OnTreeItemDblclk(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	BOOL IsTreeCtrlEditMessage(WPARAM KeyCode);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnViewMsgRecord();
	afx_msg void OnDeleteMsgRecord();
	afx_msg void OnGroupNew();
	afx_msg void OnGroupDelete();
	afx_msg void OnGroupRename();
	afx_msg void OnContactNew();
	afx_msg void OnNMRClickTreeContacts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditTreeContacts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContactEdit();
	afx_msg void OnContactDelete();
	afx_msg void OnCallUser();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSubscribeFunc(UINT nID);
	afx_msg void OnMyUGInfo(UINT nID);
};
