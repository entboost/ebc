#pragma once

class CDlgViewContactInfo;

// CDlgMyGroup dialog

class CDlgMyGroup : public CDialog
	//, public CTreeCallback
{
	DECLARE_DYNAMIC(CDlgMyGroup)

public:
	CDlgMyGroup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMyGroup();

//#ifdef USES_EBCOM_TEST
//	void MyDepartmentInfo(IEB_GroupInfo* pEBGroupInfo);
//	void MyDepMemberInfo(IEB_MemberInfo* pEBEmployeeInfo);
//	void DeleteEmployeeInfo(IEB_MemberInfo* pMemberInfo);
//#endif
#ifdef USES_EBCOM_TEST
	void MyDepartmentInfo(IEB_GroupInfo* pEBGroupInfo);
	void MyDepMemberInfo(IEB_MemberInfo* pEBEmployeeInfo, bool bChangeLineState);
	void DeleteEmployeeInfo(IEB_GroupInfo* pGroupInfo, eb::bigint nMemberCode);
	void DeleteDepartmentInfo(IEB_GroupInfo* pGroupInfo);
#else
	void MyDepartmentInfo(const EB_GroupInfo* pGroupInfo);
	void MyDepMemberInfo(const EB_MemberInfo* pMemberInfo, bool bChangeLineState);
	void DeleteEmployeeInfo(const EB_GroupInfo* pGroupInfo, eb::bigint nMemberCode);
	void DeleteDepartmentInfo(const EB_GroupInfo* pGroupInfo);
#endif
	//void ExitDepartment(eb::bigint sDepCode, eb::bigint nMemberUserId);
	//void DeleteDepartment(eb::bigint nGroupCode);
	void SetCtrlColor(void);

// Dialog Data
	enum { IDD = IDD_DLG_MY_GROUP };

protected:
	CDlgViewContactInfo* m_pViewContactInfo;

	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pDepItemInfo;	// dep_code->
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pEmpItemInfo;	// emp_code->

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnCallTrack;
	CTraButton m_btnEditEmpTrack;
	HTREEITEM m_hCurrentHotItem;
	VividTree m_treeDepartment;
#ifdef USES_EBCOM_TEST
	void SetMemberInfo(HTREEITEM hGroupItem, IEB_MemberInfo* pEBEmployeeInfo);
#else
	void SetMemberInfo(HTREEITEM hGroupItem, const EB_MemberInfo* pMemberInfo);
#endif
	CTreeItemInfo::pointer GetDepItemInfo(HTREEITEM hItem) const;
	void CallItem(HTREEITEM hItem);
	void EditEmployeeInfo(HTREEITEM hItem);

	//virtual bool GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const;
	//virtual bool GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON &pIcon1, HICON &pIcon2,int& pState) const;
	//virtual int GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON& pOutIcon) const;
	//virtual bool GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;
	LRESULT OnTreeItemSelChanged(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemDblclk(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);
	//int GetItemType(HTREEITEM hItem) const;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCallUser();
	afx_msg void OnViewMsgRecord();
	afx_msg void OnDeleteMsgRecord();
	afx_msg void OnNMRClickTreeDepartment(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnGroupShare();
	afx_msg void OnDepartment3New();
	afx_msg void OnDepartmentDelete();
	afx_msg void OnDepartmentAddEmp();
	afx_msg void OnDepartmentExit();
	afx_msg void OnDepartmentDelEmp();
	afx_msg void OnDepartmentEditEmp();
	afx_msg void OnMemberDelAdminLevel();
	afx_msg void OnMemberAddAdminLevel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDepartmentEdit();
	afx_msg void OnDestroy();
	afx_msg void OnSubscribeFunc(UINT nID);
};
