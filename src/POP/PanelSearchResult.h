#pragma once

//const tstring theAppGroupResultName = _T("集成应用搜索：");
//const tstring theUrlGroupName("[* *]");
//const CString theUrlGroupResultName = _T("最新浏览记录：");

class CDlgViewContactInfo;

// CPanelSearchResult dialog
/*
 * 搜索结果显示面板
 * 用于搜索工具条下面，显示搜索结果。
 * 
*//////////////////////////////////////////////////////

class CPanelSearchResult : public CEbDialogBase
#ifdef USES_EBCOM_TEST
	, public CEBCoreEventsSink
	, public CEBSearchEventsSink
#else
	, public CEBSearchCallback
#endif
{
	DECLARE_DYNAMIC(CPanelSearchResult)

public:
	CPanelSearchResult(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSearchResult();

	void DeleteAllResult(void);
	size_t InsertAppItem(const EB_SubscribeFuncInfo& pFuncInfo);
	void InsertUrlItem(const tstring& sTitle, const tstring& sUrl, const tstring& sLastTime);
	bool IsEmpty(void) const {return m_treeSearch.GetCount()==0?true:false;}
	void FocusTree(bool bMoveNext=true);
	VividTree& GetTreeCtrl(void) {return m_treeSearch;}
	bool CallSelectedItem(void);

// Dialog Data
	enum { IDD = IDD_PAN_SEARCH_RESULT };

protected:
	CWnd* m_pParentMsg;
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pSubFuncItem;		// subid->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pUrlItem;		// url->
	CLockMap<std::string, CTreeItemInfo::pointer> m_pContactItem;		// contact->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pEntGroupMemberItem;		// emp_code->
	CLockMap<eb::bigint, CTreeItemInfo::pointer> m_pMyGroupMemberItem;		// emp_code->
	CDlgViewContactInfo* m_pViewContactInfo;

	CTreeItemInfo::pointer GetAppDirItem(void);
	void DeleteItem(HTREEITEM hItem);
	bool CallItem(HTREEITEM hItem);

#ifdef USES_EBCOM_TEST
	virtual void Fire_onSearchGroupInfo(IEB_GroupInfo* pGroupInfo, IEB_EnterpriseInfo* pEnterpriseInfo, ULONG dwParam) {}
	virtual void Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam);
	virtual void Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam);
#else
	// CEBSearchCallback
	virtual void onGroupInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo,unsigned long dwParam) {}
	virtual void onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam);
	virtual void onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam);
#endif
	void ScrollTreeSearchToTop(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	VividTree m_treeSearch;				// itemdata:1=employee;2=contact
	CTraButton m_btnSearchTrackCall;	// search call
	CTraButton m_btnSearchTrackDel;		// search del
	CTraButton m_btnAddContact;

	DECLARE_MESSAGE_MAP()
	LRESULT OnTreeItemDoubleClick(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	LRESULT OnMsgNewContact(WPARAM wp, LPARAM lp);
	afx_msg void OnNMClickTreeSearch(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
};
