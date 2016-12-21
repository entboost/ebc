#pragma once
#include "afxcmn.h"

class CDlgViewContactInfo;

// CDlgMyEnterprise dialog
/*
 * 主界面，企业组织结构窗口类
 * 显示本人企业组织架构，支持多级显示；
 * 个人用户不显示该窗口。
 * 
*//////////////////////////////////////////////////////

class CDlgMyEnterprise : public CDialog
	//, public CTreeCallback
{
	DECLARE_DYNAMIC(CDlgMyEnterprise)

public:
	CDlgMyEnterprise(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMyEnterprise();

	//const std::string& GetEntManagerurl(void) const {return m_sEntManagerUrl;}

#ifdef USES_EBCOM_TEST
	void EnterpriseInfo(IEB_EnterpriseInfo* pEnterpriseInfo);
	void DepartmentInfo(IEB_GroupInfo* pGroupInfo);
	void EmployeeInfo(IEB_MemberInfo* pMemberInfo, bool bChangeLineState=false);
	void DeleteEmployeeInfo(IEB_GroupInfo* pGroupInfo, eb::bigint nMemberCode);
#else
	void EnterpriseInfo(const EB_EnterpriseInfo* pEnterpriseInfo);
	void DepartmentInfo(const EB_GroupInfo* pGroupInfo);
	void EmployeeInfo(const EB_MemberInfo* pMemberInfo,bool bChangeLineState=false);
	void DeleteEmployeeInfo(const EB_GroupInfo* pGroupInfo, eb::bigint nMemberCode);
#endif
	void DeleteDepartmentInfo(eb::bigint nGroupCode);
	//void ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo);
	void SetCtrlColor(void);
// Dialog Data
	enum { IDD = IDD_DLG_MY_ENTERPRISE };

protected:
	CDlgViewContactInfo* m_pViewContactInfo;
	//HTREEITEM m_hEnterpriseItem;
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pEntItemInfo;	// ent_code->
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pDepItemInfo;	// dep_code->
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pEmpItemInfo;	// emp_code->
	CLockMap<eb::bigint,CTreeItemInfo::pointer> m_pUpdateGroupCountList;	// group_id->
	CTreeItemInfo::pointer m_pUpdateEntItemInfo;
	CLockMap<eb::bigint,bool> m_pUpdateParentGroupCountList;	// group_id->

	//CLockMap<std::string, HTREEITEM> m_pDepTreeItem;		// dep_code->
	//CLockMap<const EB_MemberInfo*, HTREEITEM> m_pEmpTreeItem;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnCallTrack;
	CTraButton m_btnEditEmpTrack;
	VividTree m_treeEnterprise;
	HTREEITEM m_hCurrentHotItem;

	//CTreeItemInfo::pointer GetDepItemInfo(HTREEITEM hItem) const;
	void SetGroupCount(const CTreeItemInfo::pointer& pGroupItem, eb::bigint sGroupCode, bool bFromMemberInfo);
#ifdef USES_EBCOM_TEST
	void SetGroupCount(const CTreeItemInfo::pointer& pGroupItem, IEB_GroupInfo* pGroupInfo);
#else
	void SetGroupCount(const CTreeItemInfo::pointer& pGroupItem, const EB_GroupInfo* pGroupInfo, bool bFromMemberInfo);
#endif
	void UpdateParentGroupCount(eb::bigint nParentGroupCode);

	//static int CALLBACK TreeCmpFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	void CallItem(HTREEITEM hItem);
	void EditEmployeeInfo(HTREEITEM hItem);

	//bool GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const;
	//virtual HICON GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;
	//virtual int GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON& pOutIcon) const;
	//virtual bool GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const;

	LRESULT OnTreeItemSelChanged(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemDblclk(WPARAM wp, LPARAM lp);
	LRESULT OnTreeItemTrackHot(WPARAM wp, LPARAM lp);

	//tstring m_sEntManagerUrl;
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnterpriseEdit();
	afx_msg void OnImportEntData();
	afx_msg void OnNMRClickTreeEnterprise(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDepartment1New();
	afx_msg void OnDepartment2New();
	afx_msg void OnDepartmentDelete();
	afx_msg void OnDepartmentEdit();
	afx_msg void OnDepartmentRefresh();
	afx_msg void OnEmployeeNew();
	afx_msg void OnEmployeeDelete();
	afx_msg void OnEmployeeEdit();
	afx_msg void OnMemberDelAdminLevel();
	afx_msg void OnMemberAddAdminLevel();
	afx_msg void OnCallUser();
	afx_msg void OnViewMsgRecord();
	afx_msg void OnDeleteMsgRecord();
	afx_msg void OnNMDblclkTreeEnterprise(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSubscribeFunc(UINT nID);
	afx_msg void OnGroupShare();
};
