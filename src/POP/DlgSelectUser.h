#pragma once

// CDlgSelectUser dialog
/*
 * 选择用户窗口类
 * 用于在一对一聊天界面，和群聊界面，点击添加用户，选择用户。
 * 
*//////////////////////////////////////////////////////
#define USES_SELECTED_ITEM_UID

class CDlgSelectUser : public CEbDialogBase
	//, public CTreeCallback
#ifdef USES_EBCOM_TEST
	, public CEBSearchEventsSink
#else
	, public CEBSearchCallback
#endif
{
	DECLARE_DYNAMIC(CDlgSelectUser)

public:
	CDlgSelectUser(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectUser();
#ifdef USES_SELECTED_ITEM_UID
	CLockMap<tstring, eb::bigint> m_pSelectedUserTreeItem;	// emp_account->emp_uid
#else
	CLockMap<tstring, eb::bigint> m_pSelectedTreeItem;			// emp_account->emp_code
#endif

	void ResetSelected(void);
	void SetSingleSelect(bool b) {m_bSingleSelect = b;}
	void SetSeledtedGroupId(eb::bigint v) {m_nSelectedGroupId = v;}

#ifdef USES_EBCOM_TEST
	virtual void onMemberInfo(IEB_MemberInfo* pMemberInfo);
#else
	virtual void onMemberInfo(const EB_MemberInfo* pMemberInfo);
#endif
	void OnRemoveGroup(eb::bigint nGroupId);
	void OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId);

// Dialog Data
	enum { IDD = IDD_DLG_SEL_USER };

protected:
	bool m_bSingleSelect;
	eb::bigint m_nSelectedGroupId;
	//HTREEITEM m_hEnterprise;
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pEntTreeItem;		// ent_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pMyDepTreeItem;		// dep_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pEntDepTreeItem;		// dep_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pEntEmpTreeItem;		// emp_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pMyEmpTreeItem;		// emp_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pContactGroupItem;	// ugid->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pContactItem;		// con_id->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pSelectedItem;		// account->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pSearchItem1;		// account->
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnClose;
	CTraButton m_btnMyDepartment;
	CTraButton m_btnMyContacts;
	CTraButton m_btnMyEnterprise;
	VividTree m_treeDepartment;
	VividTree m_treeContacts;
	VividTree m_treeEnterprise;
	VividTree m_treeSelected;
	VividTree m_treeSearch;
	CTraButton m_btnOk;
	CTraButton m_btnCancel;
	CTraButton m_btnTrackAdd0;		// department add
	CTraButton m_btnTrackAdd1;		// enterprise add
	CTraButton m_btnTrackAdd2;		// contact add
	CTraButton m_btnTrackAdd3;		// search add
	CTraButton m_btnTrackDelete;	// slected delete
	CHoverEdit m_editSearch;

	void DrawInfo(void);
	void UpdateSelectedUsers(void);
	int GetLocalMemberSize(eb::bigint nGroupId) const;

#ifdef USES_EBCOM_TEST
	virtual void Fire_onSearchGroupInfo(IEB_GroupInfo* pGroupInfo, IEB_EnterpriseInfo* pEnterpriseInfo, ULONG dwParam);
	virtual void Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam);
	virtual void Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam);
#else
	virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo, unsigned long dwParam);
	virtual void onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam);
	virtual void onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam);
#endif
	void SelectDepartment(HTREEITEM hSelItem);
	void SelectContact(HTREEITEM hSelItem);
	void SelectEnterprise(HTREEITEM hSelItem);
	void SelectSearch(HTREEITEM hSelItem);
	void DeleteSelected(HTREEITEM hSelItem);

	//bool GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const;
	//virtual bool GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;

	LRESULT OnTreeItemSelChanged(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemDoubleClieck(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	void InvalidateParentRect(void);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonDepartment();
	afx_msg void OnBnClickedButtonEnterprise();
	afx_msg void OnBnClickedButtonContact();
	afx_msg void OnBnClickedButtonClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEditSearch();
	afx_msg void OnNMThemeChangedTreeDepartment(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnDestroy();
protected:
	virtual void OnOK();
};
