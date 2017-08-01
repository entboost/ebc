// DlgSelectUser.cpp : implementation file
//
#include "stdafx.h"
#include "POP.h"
#include "DlgSelectUser.h"

#define TIMERID_LOAD_ENTERPRISE_INFO 0x111

const int const_left_width	= 218;

// CDlgSelectUser dialog

IMPLEMENT_DYNAMIC(CDlgSelectUser, CEbDialogBase)

CDlgSelectUser::CDlgSelectUser(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgSelectUser::IDD, pParent)
{
	//m_hEnterprise = NULL;
	m_bSingleSelect = false;
	m_nSelectedGroupId = 0;
}

CDlgSelectUser::~CDlgSelectUser()
{
}

void CDlgSelectUser::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_DEPARTMENT, m_btnMyDepartment);
	DDX_Control(pDX, IDC_BUTTON_CONTACT, m_btnMyContacts);
	DDX_Control(pDX, IDC_BUTTON_ENTERPRISE, m_btnMyEnterprise);
	DDX_Control(pDX, IDC_TREE_DEPARTMENT, m_treeDepartment);
	DDX_Control(pDX, IDC_TREE_CONTACT, m_treeContacts);
	DDX_Control(pDX, IDC_TREE_ENTERPRISE, m_treeEnterprise);
	DDX_Control(pDX, IDC_TREE_SELECTED, m_treeSelected);
	DDX_Control(pDX, IDC_TREE_SEARCH, m_treeSearch);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);

}


BEGIN_MESSAGE_MAP(CDlgSelectUser, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_ENTERPRISE, &CDlgSelectUser::OnBnClickedButtonEnterprise)
	ON_BN_CLICKED(IDC_BUTTON_CONTACT, &CDlgSelectUser::OnBnClickedButtonContact)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgSelectUser::OnBnClickedButtonClose)
	ON_MESSAGE(WM_ITEM_SEL_CHANGED, OnTreeItemSelChanged)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDoubleClieck)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CDlgSelectUser::OnEnChangeEditSearch)
	ON_BN_CLICKED(IDC_BUTTON_DEPARTMENT, &CDlgSelectUser::OnBnClickedButtonDepartment)
	ON_WM_DESTROY()
	//ON_NOTIFY(NM_THEMECHANGED, IDC_TREE_DEPARTMENT, &CDlgSelectUser::OnNMThemeChangedTreeDepartment)
END_MESSAGE_MAP()


// CDlgSelectUser message handlers

BOOL CDlgSelectUser::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	SetTransparentType(CEbDialogBase::TT_STATIC);

	this->EnableToolTips();
	this->SetToolTipText(IDC_EDIT_SEARCH,_T("输入查找关键字"));

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetDrawPanelRgn(false);
	//m_btnClose.SetToolTipText(_T("关闭"));
	m_btnMyDepartment.SetWindowText(_T("部门"));
	m_btnMyDepartment.SetAutoSize(false);
	m_btnMyDepartment.Load(IDB_PNG_TAB_SELGROUP);
	m_btnMyDepartment.SetChecked(true);
	m_btnMyDepartment.SetTextTop(35);
	m_btnMyContacts.SetWindowText(_T("联系人"));
	m_btnMyContacts.SetAutoSize(false);
	m_btnMyContacts.Load(IDB_PNG_TAB_SELCONTACT);
	m_btnMyContacts.SetTextTop(35);
	m_btnMyEnterprise.SetWindowText(_T("公司"));
	m_btnMyEnterprise.SetAutoSize(false);
	m_btnMyEnterprise.Load(IDB_PNG_TAB_SELORG);
	m_btnMyEnterprise.SetTextTop(35);
	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnOk.SetNorTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetHotTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetPreTextColor(theDefaultBtnWhiteColor); 
	m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLineColor,theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor);
	m_btnCancel.SetHotTextColor(theApp.GetNormalColor()); 
	m_btnCancel.SetPreTextColor(theApp.GetNormalColor()); 

	m_editSearch.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_editSearch.SetPromptText(_T("搜索：用户、帐号、联系人"));

	m_btnTrackAdd0.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeDepartment, 0xffff);
	m_btnTrackAdd0.SetAutoSize(false);
	m_btnTrackAdd0.SetAutoFocus(true);
	m_btnTrackAdd0.Load(IDB_PNG_HOT_NEWS);
	m_btnTrackAdd0.SetToolTipText(_T("添加"));
	m_btnTrackAdd1.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeEnterprise, 0xffff);
	m_btnTrackAdd1.SetAutoSize(false);
	m_btnTrackAdd1.SetAutoFocus(true);
	m_btnTrackAdd1.Load(IDB_PNG_HOT_NEWS);
	m_btnTrackAdd1.SetToolTipText(_T("添加"));
	m_btnTrackAdd2.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeContacts, 0xffff);
	m_btnTrackAdd2.SetAutoSize(false);
	m_btnTrackAdd2.SetAutoFocus(true);
	m_btnTrackAdd2.Load(IDB_PNG_HOT_NEWS);
	m_btnTrackAdd2.SetToolTipText(_T("添加"));
	m_btnTrackAdd3.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSearch, 0xffff);
	m_btnTrackAdd3.SetAutoSize(false);
	m_btnTrackAdd3.SetAutoFocus(true);
	m_btnTrackAdd3.Load(IDB_PNG_HOT_NEWS);
	m_btnTrackAdd3.SetToolTipText(_T("添加"));
	m_btnTrackDelete.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSelected, 0xffff);
	m_btnTrackDelete.SetAutoSize(false);
	m_btnTrackDelete.SetAutoFocus(true);
	m_btnTrackDelete.Load(IDB_PNG_HOT_DELETES);
	m_btnTrackDelete.SetToolTipText(_T("删除"));

	// department
	//m_treeDepartment.ModifyStyleEx(TVS_EX_AUTOHSCROLL,0);
	m_treeDepartment.ModifyStyle(0, TVS_SINGLEEXPAND);
	//m_treeDepartment.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeDepartment.SetItemHeight(28);
	m_treeDepartment.SetIconSize(26,26);
	m_treeDepartment.SetRectangle(theDefaultFlatLineColor);	// RGB(144,174,205)
	m_treeDepartment.SetCallback((CTreeCallback*)&theApp);
	m_treeDepartment.ShowWindow(SW_SHOW);
	// contact
	m_treeContacts.ModifyStyle(0, TVS_SINGLEEXPAND);
	//m_treeContacts.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeContacts.SetItemHeight(28);
	m_treeContacts.SetIconSize(26,26);
	m_treeContacts.SetRectangle(theDefaultFlatLineColor);
	m_treeContacts.SetCallback((CTreeCallback*)&theApp);
	m_treeContacts.ShowWindow(SW_HIDE);
	// enterprise
	m_treeEnterprise.ModifyStyle(0, TVS_SINGLEEXPAND);
	//m_treeEnterprise.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeEnterprise.SetItemHeight(28);
	m_treeEnterprise.SetIconSize(26,26);
	m_treeEnterprise.SetRectangle(theDefaultFlatLineColor);
	m_treeEnterprise.SetCallback((CTreeCallback*)&theApp);
	m_treeEnterprise.ShowWindow(SW_HIDE);

	m_treeSelected.SetItemHeight(28);
	m_treeSelected.SetIconSize(26,26);
	m_treeSelected.SetRectangle(theDefaultFlatLineColor);
	//m_treeSelected.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());
	//m_treeSelected.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeSelected.SetCallback((CTreeCallback*)&theApp);
	m_treeSelected.SetIntervalLine(true,theDefaultBtnWhiteColor,theDefaultFlatBgColor);

	m_treeSearch.SetItemHeight(28);
	m_treeSearch.SetIconSize(26,26);
	m_treeSearch.SetRectangle(theDefaultFlatLineColor);
	m_treeSearch.SetCallback((CTreeCallback*)&theApp);
	m_treeSearch.ShowWindow(SW_HIDE);
	m_treeSearch.SetIntervalLine(true,theDefaultBtnWhiteColor,theDefaultFlatBgColor);

	//m_treeSearch.SetWindowPos(&m_treeDepartment, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_treeSearch.SetWindowPos(&m_treeEnterprise, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//m_treeSearch.SetWindowPos(&m_treeContacts, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
#ifdef USES_EBCOM_TEST
	((CEBSearchEventsSink*)this)->AddRef();	// 增加计数，避免删除自己
#endif
	SetTimer(TIMERID_LOAD_ENTERPRISE_INFO, 100, NULL);

	const int POS_DLG_LOGIN_WIDTH = 508;
	const int POS_DLG_LOGIN_HEIGHT = 452;
	CRect rectClient;
	this->GetWindowRect(&rectClient);
	rectClient.right = rectClient.left + POS_DLG_LOGIN_WIDTH;
	rectClient.bottom = rectClient.top + POS_DLG_LOGIN_HEIGHT;
	this->MoveWindow(&rectClient);
	SetCircle();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#ifdef USES_EBCOM_TEST
void CDlgSelectUser::Fire_onSearchGroupInfo(IEB_GroupInfo* pGroupInfo, IEB_EnterpriseInfo* pEnterpriseInfo, ULONG dwParam)
{
	const eb::bigint sEnterpriseCode(pGroupInfo->EnterpriseCode);
	CTreeItemInfo::pointer pEntItemInfo;
	if (pEnterpriseInfo!=NULL && sEnterpriseCode>0)
	{
		if (!m_pEntTreeItem.find(sEnterpriseCode,pEntItemInfo))
		{
			const CEBString sEnterpriseName(pEnterpriseInfo->EnterpriseName.GetBSTR());
			HTREEITEM hEntItem = m_treeEnterprise.InsertItem(sEnterpriseName.c_str());
			pEntItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_ENTERPRISE,hEntItem);
			pEntItemInfo->m_sEnterpriseCode = sEnterpriseCode;
			pEntItemInfo->m_sName = sEnterpriseName;
			m_pEntTreeItem.insert(sEnterpriseCode,pEntItemInfo);
			m_treeEnterprise.SelectItem(hEntItem);
			//m_treeEnterprise.SortChildren(TVI_ROOT);
		}
	}
	const eb::bigint sGroupCode = pGroupInfo->GroupCode;
	const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
	bool bIsMyGroup = theEBClientCore->EB_IsMyGroup(sGroupCode)?true:false;
	if (bIsMyGroup && !m_pMyDepTreeItem.exist(sGroupCode))
	{
		// 这是我的群组
		const int nMemberSize = pGroupInfo->EmpCount;
		const int nOnlineSize = theEBClientCore->EB_GetGroupOnlineSize(sGroupCode);
		CString sText;
		if (nOnlineSize>=0)
			sText.Format(_T("%s [%d/%d]"),sGroupName.c_str(), nOnlineSize, nMemberSize);
		else if (nMemberSize>0)
			sText.Format(_T("%s [%d]"),sGroupName.c_str(), nMemberSize);
		else
			sText.Format(_T("%s"),sGroupName.c_str());
		HTREEITEM hGroupItem = m_treeDepartment.InsertItem(sText);
		CTreeItemInfo::pointer pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		pDepItemInfo->m_sEnterpriseCode = sEnterpriseCode;
		pDepItemInfo->m_sGroupCode = sGroupCode;
		pDepItemInfo->m_nSubType = pGroupInfo->GroupType;
		pDepItemInfo->m_nIndex = pGroupInfo->DisplayIndex;
		pDepItemInfo->m_sName = sGroupName;
		m_pMyDepTreeItem.insert(sGroupCode,pDepItemInfo);
		m_treeDepartment.SetItemData(hGroupItem,(DWORD)pDepItemInfo.get());
		m_treeDepartment.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
		//m_treeDepartment.SortChildren(TVI_ROOT);
	}
	if (pEntItemInfo.get()!=NULL)
	{
		// 这是企业部门
		CTreeItemInfo::pointer pParentDepItemInfo;
		if (!m_pEntDepTreeItem.find(pGroupInfo->ParentCode,pParentDepItemInfo))
		{
			pParentDepItemInfo = pEntItemInfo;
		}
		const int nMemberSize = pGroupInfo->EmpCount;
		const int nOnlineSize = theEBClientCore->EB_GetGroupOnlineSize(sGroupCode);
		CString sText;
		if (nOnlineSize>=0)
			sText.Format(_T("%s [%d/%d]"),sGroupName.c_str(), nOnlineSize, nMemberSize);
		else if (nMemberSize>0)
			sText.Format(_T("%s [%d]"),sGroupName.c_str(), nMemberSize);
		else
			sText.Format(_T("%s"),sGroupName.c_str());
		HTREEITEM hGroupItem = m_treeEnterprise.InsertItem(sText,pParentDepItemInfo->m_hItem);
		CTreeItemInfo::pointer pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		pDepItemInfo->m_sEnterpriseCode = sEnterpriseCode;
		pDepItemInfo->m_sGroupCode = sGroupCode;
		pDepItemInfo->m_nSubType = pGroupInfo->GroupType;
		pDepItemInfo->m_nIndex = pGroupInfo->DisplayIndex;
		pDepItemInfo->m_sName = sGroupName;
		pDepItemInfo->m_dwItemData = nMemberSize;
		m_pEntDepTreeItem.insert(sGroupCode,pDepItemInfo);
		m_treeEnterprise.SetItemData(hGroupItem,(DWORD)pDepItemInfo.get());
		m_treeEnterprise.Sort(pParentDepItemInfo->m_hItem,CPOPApp::TreeCmpFunc);

		//HTREEITEM hGroupItem = m_treeEnterprise.InsertItem(sGroupName.c_str(),pEntItemInfo->m_hItem);
		//CTreeItemInfo::pointer pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		//pDepItemInfo->m_sEnterpriseCode = sEnterpriseCode;
		//pDepItemInfo->m_sGroupCode = sGroupCode;
		//pDepItemInfo->m_sName = sGroupName;
		//m_pEntDepTreeItem.insert(sGroupCode,pDepItemInfo);
		////m_treeEnterprise.SortChildren(pEntItemInfo->m_hItem);
	}
}
#else
void CDlgSelectUser::onGroupInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo, unsigned long dwParam)
{
	BOOST_ASSERT(pGroupInfo!=NULL);
	CTreeItemInfo::pointer pEntItemInfo;
	if (pEnterpriseInfo!=NULL && pGroupInfo->m_sEnterpriseCode>0)
	{
		if (!m_pEntTreeItem.find(pGroupInfo->m_sEnterpriseCode,pEntItemInfo))
		{
			HTREEITEM hEntItem = m_treeEnterprise.InsertItem(pEnterpriseInfo->m_sEnterpriseName.c_str());
			pEntItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_ENTERPRISE,hEntItem);
			pEntItemInfo->m_sEnterpriseCode = pEnterpriseInfo->m_sEnterpriseCode;
			pEntItemInfo->m_sName = pEnterpriseInfo->m_sEnterpriseName;
			m_pEntTreeItem.insert(pEnterpriseInfo->m_sEnterpriseCode,pEntItemInfo);
			m_treeEnterprise.SelectItem(hEntItem);
			m_treeEnterprise.SetItemData(hEntItem,(DWORD)pEntItemInfo.get());
			//m_treeEnterprise.SortChildren(TVI_ROOT);
		}
	}
#ifdef USES_EBCOM_TEST
	bool bIsMyGroup = theEBClientCore->EB_IsMyGroup(pGroupInfo->m_sGroupCode)?true:false;
#else
	bool bIsMyGroup = theEBAppClient.EB_IsMyGroup(pGroupInfo->m_sGroupCode);
#endif
	if (bIsMyGroup && !m_pMyDepTreeItem.exist(pGroupInfo->m_sGroupCode))
	{
		// 这是我的群组
		const int nMemberSize = pGroupInfo->m_nEmpCount;//theEBAppClient.EB_GetGroupMemberSize(pGroupInfo->m_sGroupCode);
		const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(pGroupInfo->m_sGroupCode,1);
		CString sText;
		if (nOnlineSize>=0)
			sText.Format(_T("%s [%d/%d]"),pGroupInfo->m_sGroupName.c_str(), nOnlineSize, nMemberSize);
		else if (nMemberSize>0)
			sText.Format(_T("%s [%d]"),pGroupInfo->m_sGroupName.c_str(), nMemberSize);
		else
			sText.Format(_T("%s"),pGroupInfo->m_sGroupName.c_str());
		HTREEITEM hGroupItem = m_treeDepartment.InsertItem(sText);
		CTreeItemInfo::pointer pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		pDepItemInfo->m_sEnterpriseCode = pGroupInfo->m_sEnterpriseCode;
		pDepItemInfo->m_sGroupCode = pGroupInfo->m_sGroupCode;
		pDepItemInfo->m_nSubType = pGroupInfo->m_nGroupType;
		pDepItemInfo->m_nIndex = pGroupInfo->m_nDisplayIndex;
		pDepItemInfo->m_sName = pGroupInfo->m_sGroupName;
		m_pMyDepTreeItem.insert(pGroupInfo->m_sGroupCode,pDepItemInfo);
		m_treeDepartment.SetItemData(hGroupItem,(DWORD)pDepItemInfo.get());
		//m_treeDepartment.SortChildren(TVI_ROOT);
		m_treeDepartment.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	}
	if (pEntItemInfo.get()!=NULL)
	{
		// 这是企业部门
		CTreeItemInfo::pointer pParentDepItemInfo;
		if (!m_pEntDepTreeItem.find(pGroupInfo->m_sParentCode,pParentDepItemInfo))
		{
			pParentDepItemInfo = pEntItemInfo;
		}
		int nMemberSize = 0;
		int nOnlineSize = 0;
		theEBAppClient.EB_GetGroupMemberSize(pGroupInfo->m_sGroupCode,0,nMemberSize,nOnlineSize);
		//const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(pGroupInfo->m_sGroupCode,0);
		////const int nMemberSize = pGroupInfo->m_nEmpCount;//theEBAppClient.EB_GetGroupMemberSize(pGroupInfo->m_sGroupCode);
		//const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(pGroupInfo->m_sGroupCode,0);
		CString sText;
		if (nOnlineSize>=0)
			sText.Format(_T("%s [%d/%d]"),pGroupInfo->m_sGroupName.c_str(), nOnlineSize, nMemberSize);
		else if (nMemberSize>0)
			sText.Format(_T("%s [%d]"),pGroupInfo->m_sGroupName.c_str(), nMemberSize);
		else
			sText.Format(_T("%s"),pGroupInfo->m_sGroupName.c_str());
		HTREEITEM hGroupItem = m_treeEnterprise.InsertItem(sText,pParentDepItemInfo->m_hItem);
		CTreeItemInfo::pointer pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		pDepItemInfo->m_sEnterpriseCode = pGroupInfo->m_sEnterpriseCode;
		pDepItemInfo->m_sGroupCode = pGroupInfo->m_sGroupCode;
		pDepItemInfo->m_nSubType = pGroupInfo->m_nGroupType;
		pDepItemInfo->m_nIndex = pGroupInfo->m_nDisplayIndex;
		pDepItemInfo->m_sName = pGroupInfo->m_sGroupName;
		pDepItemInfo->m_dwItemData = nMemberSize;
		m_pEntDepTreeItem.insert(pGroupInfo->m_sGroupCode,pDepItemInfo);
		m_treeEnterprise.SetItemData(hGroupItem,(DWORD)pDepItemInfo.get());
		m_treeEnterprise.Sort(pParentDepItemInfo->m_hItem,CPOPApp::TreeCmpFunc);

		//HTREEITEM hGroupItem = m_treeEnterprise.InsertItem(pGroupInfo->m_sGroupName.c_str(),pEntItemInfo->m_hItem);
		//CTreeItemInfo::pointer pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		//pDepItemInfo->m_sEnterpriseCode = pGroupInfo->m_sEnterpriseCode;
		//pDepItemInfo->m_sGroupCode = pGroupInfo->m_sGroupCode;
		//pDepItemInfo->m_sName = pGroupInfo->m_sGroupName;
		//m_pEntDepTreeItem.insert(pGroupInfo->m_sGroupCode,pDepItemInfo);
		//m_treeEnterprise.SetItemData(hGroupItem,(DWORD)pDepItemInfo.get());
		////m_treeEnterprise.SortChildren(pEntItemInfo->m_hItem);
	}
}
#endif
void CDlgSelectUser::UpdateSelectedUsers(void)
{
	CString sText;
	const UINT nCount = m_treeSelected.GetCount();
	if (nCount==0)
	{
		sText = _T("已选择用户：");
	}else
	{
		sText.Format(_T("已选择用户：(%d)"),(int)nCount);
	}
	this->GetDlgItem(IDC_STATIC_SELECTED)->SetWindowText(sText);
	//this->GetDlgItem(IDC_STATIC_SELECTED)->Invalidate();
	theApp.InvalidateParentRect(this->GetDlgItem(IDC_STATIC_SELECTED));
}

int CDlgSelectUser::GetLocalMemberSize(eb::bigint nGroupId) const
{
	int nResult = 0;
	AUTO_CONST_RLOCK(m_pEntEmpTreeItem);
	CLockMap<eb::bigint, CTreeItemInfo::pointer>::const_iterator pIter = m_pEntEmpTreeItem.begin();
	for (;pIter!=m_pEntEmpTreeItem.end(); pIter++)
	{
		const CTreeItemInfo::pointer& pItemInfo = pIter->second;
		if (pItemInfo->m_sGroupCode==nGroupId)
			nResult++;
	}
	return nResult;
}
#ifdef USES_EBCOM_TEST
void CDlgSelectUser::onMemberInfo(IEB_MemberInfo* pMemberInfo)
{
	const eb::bigint sGroupCode(pMemberInfo->GroupCode);
	const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
	const eb::bigint sMemberCode(pMemberInfo->MemberCode);
	const eb::bigint sMemberUserId(pMemberInfo->MemberUserId);
	const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());
	const CEBString sJobTitle(pMemberInfo->JobTitle.GetBSTR());
	CString sText;
	if (sJobTitle.empty())
		sText.Format(_T("%s"),sUserName.c_str());
	else
		sText.Format(_T("%s - %s"),sUserName.c_str(),sJobTitle.c_str());
	CTreeItemInfo::pointer pDepItemInfo;
	if (m_pEntDepTreeItem.find(sGroupCode,pDepItemInfo))
	{
		// 企业部门员工
		CTreeItemInfo::pointer pEmpItemInfo;
		if (!m_pEntEmpTreeItem.find(sMemberCode,pEmpItemInfo))
		{
			HTREEITEM hEmpItem = m_treeEnterprise.InsertItem(sText,pDepItemInfo->m_hItem);
			pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = sMemberCode;
			pEmpItemInfo->m_nUserId = sMemberUserId;
			pEmpItemInfo->m_sAccount = sMemberAccount;
			pEmpItemInfo->m_sGroupCode = sGroupCode;
			pEmpItemInfo->m_sName = sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->LineState;
			m_pEntEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeEnterprise.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
			m_treeEnterprise.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
		}
	}
	if (m_pMyDepTreeItem.find(sGroupCode,pDepItemInfo))
	{
		// 我的群组成员
		CTreeItemInfo::pointer pEmpItemInfo;
		if (!m_pMyEmpTreeItem.find(sMemberCode,pEmpItemInfo))
		{
			HTREEITEM hEmpItem = m_treeDepartment.InsertItem(sText,pDepItemInfo->m_hItem);
			pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = sMemberCode;
			pEmpItemInfo->m_nUserId = sMemberUserId;
			pEmpItemInfo->m_sAccount = sMemberAccount;
			pEmpItemInfo->m_sGroupCode = sGroupCode;
			pEmpItemInfo->m_sName = sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->LineState;
			m_pMyEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeDepartment.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
			m_treeDepartment.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
		}
	}
}
#else
void CDlgSelectUser::onMemberInfo(const EB_MemberInfo* pMemberInfo)
{
	CString sText;
	if (pMemberInfo->m_sJobTitle.empty())
		sText.Format(_T("%s"),pMemberInfo->m_sUserName.c_str());
	else
		sText.Format(_T("%s - %s"),pMemberInfo->m_sUserName.c_str(),pMemberInfo->m_sJobTitle.c_str());
	CTreeItemInfo::pointer pDepItemInfo;
	if (m_pEntDepTreeItem.find(pMemberInfo->m_sGroupCode,pDepItemInfo))
	{
		// 企业部门员工
		CTreeItemInfo::pointer pEmpItemInfo;
		if (!m_pEntEmpTreeItem.find(pMemberInfo->m_sMemberCode,pEmpItemInfo))
		{
			HTREEITEM hEmpItem = m_treeEnterprise.InsertItem(sText,pDepItemInfo->m_hItem);
			pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
			pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
			pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
			pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
			m_pEntEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeEnterprise.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
		}else
		{
			m_treeEnterprise.SetItemText(pEmpItemInfo->m_hItem, sText);
		}
		pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
		pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
		pEmpItemInfo->m_nIndex = pMemberInfo->m_nDisplayIndex;
		if ((pMemberInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)
			pEmpItemInfo->m_nExtData &= ~CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
		else
			pEmpItemInfo->m_nExtData |= CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;

		if (pDepItemInfo->m_nSubType<=EB_GROUP_TYPE_PROJECT && theApp.IsEnterpriseCreateUserId(pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 11;
		else if (theEBAppClient.EB_IsGroupCreator(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 10;
		else if (theEBAppClient.EB_IsGroupManager(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 10;
		else if (theEBAppClient.EB_IsGroupAdminLevel(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 9;
		else if (theApp.GetLogonUserId()==pMemberInfo->m_nMemberUserId)
			pEmpItemInfo->m_nSubType = 1;
		else
			pEmpItemInfo->m_nSubType = 0;
		m_treeEnterprise.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
	}
	if (m_pMyDepTreeItem.find(pMemberInfo->m_sGroupCode,pDepItemInfo))
	{
		// 我的群组成员
		CTreeItemInfo::pointer pEmpItemInfo;
		if (!m_pMyEmpTreeItem.find(pMemberInfo->m_sMemberCode,pEmpItemInfo))
		{
			HTREEITEM hEmpItem = m_treeDepartment.InsertItem(sText,pDepItemInfo->m_hItem);
			pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
			pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
			pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
			pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
			m_pMyEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeDepartment.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
		}else
		{
			m_treeDepartment.SetItemText(pEmpItemInfo->m_hItem, sText);
		}
		pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
		pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
		pEmpItemInfo->m_nIndex = pMemberInfo->m_nDisplayIndex;
		if ((pMemberInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)
			pEmpItemInfo->m_nExtData &= ~CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
		else
			pEmpItemInfo->m_nExtData |= CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;

		if (pDepItemInfo->m_nSubType<=EB_GROUP_TYPE_PROJECT && theApp.IsEnterpriseCreateUserId(pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 11;
		else if (theEBAppClient.EB_IsGroupCreator(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 10;
		else if (theEBAppClient.EB_IsGroupManager(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 10;
		else if (theEBAppClient.EB_IsGroupAdminLevel(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
			pEmpItemInfo->m_nSubType = 9;
		else if (theApp.GetLogonUserId()==pMemberInfo->m_nMemberUserId)
			pEmpItemInfo->m_nSubType = 1;
		else
			pEmpItemInfo->m_nSubType = 0;
		m_treeDepartment.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
	}

	//eb::bigint nSelectedEmpId = 0;
	//if (m_pSelectedTreeItem.find(pMemberInfo->m_sMemberAccount,nSelectedEmpId))
	//{
	//	if (nSelectedEmpId==pMemberInfo->m_sMemberCode)
	//	{
	//		HTREEITEM hEmpItem = m_treeSelected.InsertItem(pMemberInfo->m_sUserName.c_str());
	//		CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
	//		pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
	//		pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
	//		pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
	//		pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
	//		m_treeSelected.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
	//		m_pSelectedTreeItem.insert(pMemberInfo->m_sMemberAccount, pMemberInfo->m_sMemberCode);
	//		m_pSelectedItem.insert(pMemberInfo->m_sMemberAccount,pEmpItemInfo);
	//		m_treeSelected.Invalidate();
	//	}
	//}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgSelectUser::Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam)
{
	const eb::bigint sGroupCode(pMemberInfo->GroupCode);
	const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
	const eb::bigint sMemberCode(pMemberInfo->MemberCode);
	const eb::bigint sMemberUserId(pMemberInfo->MemberUserId);
	const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());
	const CEBString sJobTitle(pMemberInfo->JobTitle.GetBSTR());
	CString sText;
	if (sJobTitle.empty())
		sText.Format(_T("%s"),sUserName.c_str());
	else
		sText.Format(_T("%s - %s"),sUserName.c_str(),sJobTitle.c_str());
	if (dwParam == 0)
	{
		CTreeItemInfo::pointer pDepItemInfo;
		if (m_pEntDepTreeItem.find(sGroupCode,pDepItemInfo))
		{
			// 企业部门员工
			CTreeItemInfo::pointer pEmpItemInfo;
			if (!m_pEntEmpTreeItem.find(sMemberCode,pEmpItemInfo))
			{
				HTREEITEM hEmpItem = m_treeEnterprise.InsertItem(sText,pDepItemInfo->m_hItem);
				pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
				pEmpItemInfo->m_sMemberCode = sMemberCode;
				pEmpItemInfo->m_nUserId = sMemberUserId;
				pEmpItemInfo->m_sAccount = sMemberAccount;
				pEmpItemInfo->m_sGroupCode = sGroupCode;
				pEmpItemInfo->m_sName = sUserName;
				pEmpItemInfo->m_dwItemData = pMemberInfo->LineState;
				m_pEntEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
				m_treeEnterprise.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
				m_treeEnterprise.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
			}
		}
		if (m_pMyDepTreeItem.find(sGroupCode,pDepItemInfo))
		{
			// 我的群组成员
			CTreeItemInfo::pointer pEmpItemInfo;
			if (!m_pMyEmpTreeItem.find(sMemberCode,pEmpItemInfo))
			{
				HTREEITEM hEmpItem = m_treeDepartment.InsertItem(sText,pDepItemInfo->m_hItem);
				pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
				pEmpItemInfo->m_sMemberCode = sMemberCode;
				pEmpItemInfo->m_nUserId = sMemberUserId;
				pEmpItemInfo->m_sAccount = sMemberAccount;
				pEmpItemInfo->m_sGroupCode = sGroupCode;
				pEmpItemInfo->m_sName = sUserName;
				pEmpItemInfo->m_dwItemData = pMemberInfo->LineState;
				m_pMyEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
				m_treeDepartment.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
				m_treeDepartment.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
			}
		}
	}else if (dwParam == 1)
	{
		// search employee
		if (!m_pSearchItem1.exist(sMemberAccount))
		{
			HTREEITEM hSearchItem = m_treeSearch.InsertItem(sText);
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hSearchItem);
			pEmpItemInfo->m_sMemberCode = sMemberCode;
			pEmpItemInfo->m_nUserId = sMemberUserId;
			pEmpItemInfo->m_sAccount = sMemberAccount;
			pEmpItemInfo->m_sGroupCode = sGroupCode;
			pEmpItemInfo->m_sName = sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->LineState;
			m_treeSearch.SetItemData(hSearchItem,(DWORD)pEmpItemInfo.get());
			m_pSearchItem1.insert(sMemberAccount, pEmpItemInfo);
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
		}
	}
}
#else
void CDlgSelectUser::onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam)
{
	CString sText;
	if (pMemberInfo->m_sJobTitle.empty())
		sText.Format(_T("%s"),pMemberInfo->m_sUserName.c_str());
	else
		sText.Format(_T("%s - %s"),pMemberInfo->m_sUserName.c_str(),pMemberInfo->m_sJobTitle.c_str());
	if (dwParam == 0)
	{
		CTreeItemInfo::pointer pDepItemInfo;
		if (m_pEntDepTreeItem.find(pMemberInfo->m_sGroupCode,pDepItemInfo))
		{
			// 企业部门员工
			CTreeItemInfo::pointer pEmpItemInfo;
			if (!m_pEntEmpTreeItem.find(pMemberInfo->m_sMemberCode,pEmpItemInfo))
			{
				HTREEITEM hEmpItem = m_treeEnterprise.InsertItem(sText,pDepItemInfo->m_hItem);
				pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
				pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
				pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
				pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
				pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
				m_pEntEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
				m_treeEnterprise.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
			}else
			{
				m_treeEnterprise.SetItemText(pEmpItemInfo->m_hItem,sText);
			}
			pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
			pEmpItemInfo->m_nIndex = pMemberInfo->m_nDisplayIndex;
			if ((pMemberInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)
				pEmpItemInfo->m_nExtData &= ~CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
			else
				pEmpItemInfo->m_nExtData |= CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;

			if (pDepItemInfo->m_nSubType<=EB_GROUP_TYPE_PROJECT && theApp.IsEnterpriseCreateUserId(pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 11;
			else if (theEBAppClient.EB_IsGroupCreator(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 10;
			else if (theEBAppClient.EB_IsGroupManager(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 10;
			else if (theEBAppClient.EB_IsGroupAdminLevel(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 9;
			else if (theApp.GetLogonUserId()==pMemberInfo->m_nMemberUserId)
				pEmpItemInfo->m_nSubType = 1;
			else
				pEmpItemInfo->m_nSubType = 0;
			m_treeEnterprise.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
		}
		if (m_pMyDepTreeItem.find(pMemberInfo->m_sGroupCode,pDepItemInfo))
		{
			// 我的群组成员
			CTreeItemInfo::pointer pEmpItemInfo;
			if (!m_pMyEmpTreeItem.find(pMemberInfo->m_sMemberCode,pEmpItemInfo))
			{
				HTREEITEM hEmpItem = m_treeDepartment.InsertItem(sText,pDepItemInfo->m_hItem);
				pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
				pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
				pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
				pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
				pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
				m_pMyEmpTreeItem.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
				m_treeDepartment.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
			}else
			{
				m_treeDepartment.SetItemText(pEmpItemInfo->m_hItem,sText);
			}
			pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
			pEmpItemInfo->m_nIndex = pMemberInfo->m_nDisplayIndex;
			if ((pMemberInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)
				pEmpItemInfo->m_nExtData &= ~CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
			else
				pEmpItemInfo->m_nExtData |= CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;

			if (pDepItemInfo->m_nSubType<=EB_GROUP_TYPE_PROJECT && theApp.IsEnterpriseCreateUserId(pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 11;
			else if (theEBAppClient.EB_IsGroupCreator(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 10;
			else if (theEBAppClient.EB_IsGroupManager(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 10;
			else if (theEBAppClient.EB_IsGroupAdminLevel(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
				pEmpItemInfo->m_nSubType = 9;
			else if (theApp.GetLogonUserId()==pMemberInfo->m_nMemberUserId)
				pEmpItemInfo->m_nSubType = 1;
			else
				pEmpItemInfo->m_nSubType = 0;
			m_treeDepartment.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
		}

		if (m_nSelectedGroupId==0 || m_nSelectedGroupId==pMemberInfo->m_sGroupCode)
		{
#ifdef USES_SELECTED_ITEM_UID
			eb::bigint nSelectedUserId = 0;
			if (m_pSelectedUserTreeItem.find(pMemberInfo->m_sMemberAccount,nSelectedUserId))
#else
			eb::bigint nSelectedEmpId = 0;
			if (m_pSelectedTreeItem.find(pMemberInfo->m_sMemberAccount,nSelectedEmpId))
#endif
			{
#ifdef USES_SELECTED_ITEM_UID
				if (nSelectedUserId==pMemberInfo->m_nMemberUserId)
#else
				if (nSelectedEmpId==pMemberInfo->m_sMemberCode)
#endif
				{
					HTREEITEM hEmpItem = m_treeSelected.InsertItem(pMemberInfo->m_sUserName.c_str());
					CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
					pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
					pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
					pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
					pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
					pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
					//pEmpItemInfo->m_nIndex = pMemberInfo->m_nDisplayIndex;
					//if ((pMemberInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)
					//	pEmpItemInfo->m_nExtData &= ~CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
					//else
					//	pEmpItemInfo->m_nExtData |= CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;

					//if (theApp.IsEnterpriseCreateUserId(pMemberInfo->m_nMemberUserId))
					//	pEmpItemInfo->m_nSubType = 11;
					//else if (theEBAppClient.EB_IsGroupCreator(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
					//	pEmpItemInfo->m_nSubType = 10;
					//else if (theEBAppClient.EB_IsGroupManager(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
					//	pEmpItemInfo->m_nSubType = 10;
					//else if (theEBAppClient.EB_IsGroupAdminLevel(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
					//	pEmpItemInfo->m_nSubType = 9;
					//else if (theApp.GetLogonUserId()==pMemberInfo->m_nMemberUserId)
					//	pEmpItemInfo->m_nSubType = 1;
					//else
					//	pEmpItemInfo->m_nSubType = 0;

					m_treeSelected.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
					//m_pSelectedTreeItem.insert(pMemberInfo->m_sMemberAccount, pMemberInfo->m_sMemberCode);
					m_pSelectedItem.insert(pMemberInfo->m_sMemberAccount,pEmpItemInfo);
					m_treeSelected.Invalidate();
					UpdateSelectedUsers();
				}
			}
		}
	}else if (dwParam == 1)
	{
		// search employee
		if (!m_pSearchItem1.exist(pMemberInfo->m_sMemberAccount))
		{
			HTREEITEM hSearchItem = m_treeSearch.InsertItem(sText);
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hSearchItem);
			pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
			pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
			pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
			pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
			pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
			//pEmpItemInfo->m_nIndex = pMemberInfo->m_nDisplayIndex;
			m_treeSearch.SetItemData(hSearchItem,(DWORD)pEmpItemInfo.get());
			m_pSearchItem1.insert(pMemberInfo->m_sMemberAccount, pEmpItemInfo);
		}
		m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	}
}
#endif
void CDlgSelectUser::OnRemoveGroup(eb::bigint nGroupId)
{
	CTreeItemInfo::pointer pDepItemInfo;
	if (m_pMyDepTreeItem.find(nGroupId,pDepItemInfo,true))
	{
		m_treeDepartment.DeleteItem(pDepItemInfo->m_hItem);
	}
	if (m_pEntDepTreeItem.find(nGroupId,pDepItemInfo,true))
	{
		m_treeEnterprise.DeleteItem(pDepItemInfo->m_hItem);
	}
}
void CDlgSelectUser::OnRemoveMember(eb::bigint nGroupId, eb::bigint nMemberId)
{
	CTreeItemInfo::pointer pEmpItemInfo;
	if (m_pMyEmpTreeItem.find(nMemberId,pEmpItemInfo,true))
	{
		m_treeDepartment.DeleteItem(pEmpItemInfo->m_hItem);
	}
	if (m_pEntEmpTreeItem.find(nMemberId,pEmpItemInfo,true))
	{
		m_treeEnterprise.DeleteItem(pEmpItemInfo->m_hItem);
	}

}

#ifdef USES_EBCOM_TEST
void CDlgSelectUser::Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam)
{
	if (pContactInfo==NULL || pContactInfo->ContactUserId==0) return;
	const CEBString sContact(pContactInfo->Name.GetBSTR());
	const CEBString sGroup(pContactInfo->GroupName.GetBSTR());
	const CEBString sName(pContactInfo->Name.GetBSTR());
	CString sText;
	if (sContact.empty())
		sText.Format(_T("%s"),sName.c_str());
	else
		sText.Format(_T("%s - %s"),sName.c_str(),sContact.c_str());
	if (dwParam == 0)
	{
		const eb::bigint nUGId = pContactInfo->UGId==0?const_default_group_ugid:pContactInfo->UGId;
		const CEBString sGroupName = pContactInfo->UGId==0?const_default_group_name:pContactInfo->GroupName.GetBSTR();
		CTreeItemInfo::pointer pGroupItemInfo;
		if (!m_pContactGroupItem.find(nUGId,pGroupItemInfo))
		{
			HTREEITEM hGroupItem = m_treeContacts.InsertItem(sGroupName.c_str());
			pGroupItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
			pGroupItemInfo->m_sName = sGroup;
			pGroupItemInfo->m_nSubType = -1;
			m_pContactGroupItem.insert(nUGId, pGroupItemInfo);
			//m_treeContacts.SortChildren(TVI_ROOT);
		}
		HTREEITEM hContactItem = m_treeContacts.InsertItem(sText,pGroupItemInfo->m_hItem);
		CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
		pContactItemInfo->m_sId = pContactInfo->ContactId;
		pContactItemInfo->m_nUserId = pContactInfo->ContactUserId;
		pContactItemInfo->m_sAccount = sContact;
		pContactItemInfo->m_sName = sName;
		m_pContactItem.insert(pContactItemInfo->m_sId,pContactItemInfo);
		m_treeContacts.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
		//m_treeContacts.SortChildren(pGroupItemInfo->m_hItem);
	}else if (dwParam == 1)
	{
		// search contact
		if (!m_pSearchItem1.exist(sContact))
		{
			HTREEITEM hSearchItem = m_treeSearch.InsertItem(sText);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hSearchItem);
			pContactItemInfo->m_sId = pContactInfo->ContactId;
			pContactItemInfo->m_nUserId = pContactInfo->ContactUserId;
			pContactItemInfo->m_sAccount = sContact;
			pContactItemInfo->m_sName = sName;
			m_treeSearch.SetItemData(hSearchItem, (DWORD)pContactItemInfo.get());
			m_pSearchItem1.insert(sContact, pContactItemInfo);
			//m_treeSearch.SortChildren(TVI_ROOT);
		}
	}
}
#else
void CDlgSelectUser::onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam)
{
	if (pContactInfo==NULL || pContactInfo->m_nContactUserId==0) return;
	CString sText;
	if (pContactInfo->m_sContact.empty())
		sText.Format(_T("%s"),pContactInfo->m_sName.c_str());
	else
		sText.Format(_T("%s - %s"),pContactInfo->m_sName.c_str(),pContactInfo->m_sContact.c_str());
	if (dwParam == 0)
	{
		const eb::bigint nUGId = pContactInfo->m_nUGId==0?const_default_group_ugid:pContactInfo->m_nUGId;
		const tstring sGroupName = pContactInfo->m_nUGId==0?const_default_group_name:pContactInfo->m_sGroupName;
		CTreeItemInfo::pointer pGroupItemInfo;
		if (!m_pContactGroupItem.find(nUGId,pGroupItemInfo))
		{
			HTREEITEM hGroupItem = m_treeContacts.InsertItem(sGroupName.c_str());
			pGroupItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
			pGroupItemInfo->m_sName = pContactInfo->m_sGroupName;
			pGroupItemInfo->m_nSubType = -1;
			m_pContactGroupItem.insert(nUGId, pGroupItemInfo);
			m_treeContacts.SetItemData(hGroupItem,(DWORD)pGroupItemInfo.get());
			//m_treeContacts.SortChildren(TVI_ROOT);
		//}else
		//{
		//	m_treeContacts.SetItemText(pGroupItemInfo->m_hItem,sGroupName.c_str());
		}
		HTREEITEM hContactItem = m_treeContacts.InsertItem(sText,pGroupItemInfo->m_hItem);
		CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
		pContactItemInfo->m_sId = pContactInfo->m_nContactId;
		pContactItemInfo->m_nUserId = pContactInfo->m_nContactUserId;
		pContactItemInfo->m_sAccount = pContactInfo->m_sContact;
		pContactItemInfo->m_sName = pContactInfo->m_sName;
		m_pContactItem.insert(pContactItemInfo->m_sId,pContactItemInfo);
		m_treeContacts.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
		//m_treeContacts.SortChildren(pGroupItemInfo->m_hItem);

		if (m_nSelectedGroupId==0)
		{
#ifdef USES_SELECTED_ITEM_UID
			eb::bigint nSelectedUserId = 0;
			if (m_pSelectedUserTreeItem.find(pContactInfo->m_sContact,nSelectedUserId))
#else
			eb::bigint nSelectedEmpId = 0;
			if (m_pSelectedTreeItem.find(pContactInfo->m_sContact,nSelectedEmpId))
#endif
			{
				//if (nSelectedEmpId==0)
				{
					CString sText;
					sText.Format(_T("%s(%s)"),pContactInfo->m_sName.c_str(),pContactInfo->m_sContact.c_str());
					HTREEITEM hContactItem = m_treeSelected.InsertItem(sText);
					CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
					pContactItemInfo->m_sAccount = pContactInfo->m_sContact;
					pContactItemInfo->m_nUserId = pContactInfo->m_nContactUserId;
					pContactItemInfo->m_sName = pContactInfo->m_sName;
					m_treeSelected.SetItemData(hContactItem, (DWORD)pContactItemInfo.get());
					//m_pSelectedTreeItem.insert(pContactInfo->m_sAccount, 0);
					m_pSelectedItem.insert(pContactInfo->m_sContact,pContactItemInfo);
					m_treeSelected.Invalidate();
					UpdateSelectedUsers();
				}
			}
		}
	}else if (dwParam == 1)
	{
		// search contact
		if (!m_pSearchItem1.exist(pContactInfo->m_sContact))
		{
			HTREEITEM hSearchItem = m_treeSearch.InsertItem(sText);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hSearchItem);
			pContactItemInfo->m_sId = pContactInfo->m_nContactId;
			pContactItemInfo->m_nUserId = pContactInfo->m_nContactUserId;
			pContactItemInfo->m_sAccount = pContactInfo->m_sContact;
			pContactItemInfo->m_sName = pContactInfo->m_sName;
			m_treeSearch.SetItemData(hSearchItem, (DWORD)pContactItemInfo.get());
			m_pSearchItem1.insert(pContactInfo->m_sContact, pContactItemInfo);
			//m_treeSearch.SortChildren(TVI_ROOT);
		}
	}
}
#endif

void CDlgSelectUser::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	const int const_left_intever = 12;
	int y = 37;
	const int const_Tab_Width	= m_btnMyDepartment.GetImgWidth();// 60;
	int x = const_left_intever;
	m_btnMyDepartment.MovePoint(x, y);
	x += const_Tab_Width;
	m_btnMyContacts.MovePoint(x, y);
	x += const_Tab_Width;
	m_btnMyEnterprise.MovePoint(x, y);
	y += m_btnMyDepartment.GetImgHeight()+20;	// 20按钮下面间隔
	const int const_search_height = 22;
	if (m_editSearch.GetSafeHwnd())
		m_editSearch.MoveWindow(const_left_intever,y, const_left_width, const_search_height);
	y += (const_search_height+1);
	const int const_height = 238;
	if (m_treeDepartment.GetSafeHwnd())
		m_treeDepartment.MoveWindow(const_left_intever, y, const_left_width, const_height);
	if (m_treeContacts.GetSafeHwnd())
		m_treeContacts.MoveWindow(const_left_intever, y, const_left_width, const_height);
	if (m_treeEnterprise.GetSafeHwnd())
		m_treeEnterprise.MoveWindow(const_left_intever, y, const_left_width, const_height);
	const int const_right_interval = 15;	// 中间间隔
	const int const_right_left = const_left_intever+const_left_width+const_right_interval;
	const int const_right_width = cx-const_left_width-const_left_intever*2-const_right_interval;
	if (this->GetDlgItem(IDC_STATIC_SELECTED)->GetSafeHwnd())
		this->GetDlgItem(IDC_STATIC_SELECTED)->MoveWindow(const_right_left,y-const_search_height, const_right_width, const_search_height);
	if (m_treeSelected.GetSafeHwnd())
		m_treeSelected.MoveWindow(const_right_left, y, const_right_width, const_height);
	x = cx-theDefaultBtnWidth-theDefaultBtnRightIntever;					// 最右边间隔
	const int const_btn_y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;	// 最下面间隔
	m_btnCancel.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	x -= (theDefaultBtnWidth+theDefaultBtnIntever);
	m_btnOk.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	//const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 60;
	//const int const_btn_height = 32;
	//x = cx-const_btn_width-10;									// 最右边间隔
	//const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	//m_btnCancel.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	//x -= (const_btn_width+6);									// 按钮间隔
	//m_btnOk.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
}

void CDlgSelectUser::OnPaint()
{
	if (!IsIconic())
	{
		DrawInfo();
	}
}
void CDlgSelectUser::DrawInfo(void)
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);

	this->ClearBgDrawInfo();
	this->AddBgDrawInfo(CEbBackDrawInfo(106,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,106,2);
	//this->ClearBgDrawInfo();
	//if (theApp.GetColorGradient())
	//	this->AddBgDrawInfo(CEbBackDrawInfo(106,theDefaultBgColorTitleLight1,true));
	//else
	//	this->AddBgDrawInfo(CEbBackDrawInfo(106,1.0,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(296,theDefaultBgColorBgLight1,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);
	//Gdiplus::Image * pImage;
	//if (libEbc::ImageFromIDResource(IDB_PNG_IMG1_PEOPLE,_T("png"),pImage))
	//{
	//	graphics.DrawImage(pImage,PointF(10,7));
	//	delete pImage;
	//}
	graphics.DrawImage(theApp.m_image1People,PointF(10,7));
	// 写标题
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	const Gdiplus::PointF pointTitle(40,11);
	graphics.DrawString(L"选择用户",-1,&fontEbTitle,pointTitle,&brushEbTitle);

	//this->DrawPopBg(&dc, theApp.GetMainColor());
	//HDC m_hdcMemory = dc.m_hDC;

	//// 写标题
	//CFont pNewFontTitle;
	//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
	//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
	//CString sOutText = _T("选择用户");
	//TextOut(m_hdcMemory, 10, 10, sOutText, sOutText.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);
}

void CDlgSelectUser::OnBnClickedButtonDepartment()
{
	m_btnMyDepartment.SetChecked(true);
	m_btnMyDepartment.Invalidate();
	m_treeDepartment.ShowWindow(SW_SHOW);
	m_btnMyContacts.SetChecked(false);
	m_btnMyContacts.Invalidate();
	m_treeContacts.ShowWindow(SW_HIDE);
	m_btnMyEnterprise.SetChecked(false);
	m_btnMyEnterprise.Invalidate();
	m_treeEnterprise.ShowWindow(SW_HIDE);
	InvalidateParentRect();
}

void CDlgSelectUser::OnBnClickedButtonEnterprise()
{
	m_btnMyEnterprise.SetChecked(true);
	m_btnMyEnterprise.Invalidate();
	m_treeEnterprise.ShowWindow(SW_SHOW);
	m_btnMyDepartment.SetChecked(false);
	m_btnMyDepartment.Invalidate();
	m_treeDepartment.ShowWindow(SW_HIDE);
	m_btnMyContacts.SetChecked(false);
	m_btnMyContacts.Invalidate();
	m_treeContacts.ShowWindow(SW_HIDE);
	InvalidateParentRect();
}

void CDlgSelectUser::OnBnClickedButtonContact()
{
	m_btnMyContacts.SetChecked(true);
	m_btnMyContacts.Invalidate();
	m_treeContacts.ShowWindow(SW_SHOW);
	m_btnMyDepartment.SetChecked(false);
	m_btnMyDepartment.Invalidate();
	m_treeDepartment.ShowWindow(SW_HIDE);
	m_btnMyEnterprise.SetChecked(false);
	m_btnMyEnterprise.Invalidate();
	m_treeEnterprise.ShowWindow(SW_HIDE);
	InvalidateParentRect();
}
void CDlgSelectUser::InvalidateParentRect(void)
{
	theApp.InvalidateParentRect(&m_btnMyDepartment);
	theApp.InvalidateParentRect(&m_btnMyContacts);
	theApp.InvalidateParentRect(&m_btnMyEnterprise);
}

void CDlgSelectUser::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgSelectUser::ResetSelected(void)
{
#ifdef USES_SELECTED_ITEM_UID
	m_pSelectedUserTreeItem.clear();
#else
	m_pSelectedTreeItem.clear();
#endif
	m_pSelectedItem.clear();
	if (m_treeSelected.GetSafeHwnd()!=NULL)
		m_treeSelected.DeleteAllItems();
	this->GetDlgItem(IDC_STATIC_SELECTED)->SetWindowText(_T("已选择用户："));
}

void CDlgSelectUser::SelectEnterprise(HTREEITEM hSelItem)
{
	if (hSelItem != NULL)
	{
		const CTreeItemInfo* pDepItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
#ifdef USES_SELECTED_ITEM_UID
		if (pDepItemInfo != NULL && pDepItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && !m_pSelectedUserTreeItem.exist(pDepItemInfo->m_sAccount))
#else
		if (pDepItemInfo != NULL && pDepItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && !m_pSelectedTreeItem.exist(pDepItemInfo->m_sAccount))
#endif
		{
			if (m_bSingleSelect)
				ResetSelected();

			HTREEITEM hEmpItem = m_treeSelected.InsertItem(pDepItemInfo->m_sName.c_str());
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sGroupCode = pDepItemInfo->m_sGroupCode;
			pEmpItemInfo->m_sMemberCode = pDepItemInfo->m_sMemberCode;
			pEmpItemInfo->m_nUserId = pDepItemInfo->m_nUserId;
			pEmpItemInfo->m_sAccount = pDepItemInfo->m_sAccount;
			pEmpItemInfo->m_sName = pDepItemInfo->m_sName;
			//pEmpItemInfo->m_nIndex = pDepItemInfo->m_nIndex;	// ?
			m_treeSelected.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
#ifdef USES_SELECTED_ITEM_UID
			m_pSelectedUserTreeItem.insert(pEmpItemInfo->m_sAccount, pEmpItemInfo->m_nUserId);
#else
			m_pSelectedTreeItem.insert(pEmpItemInfo->m_sAccount, pEmpItemInfo->m_sMemberCode);
#endif
			m_pSelectedItem.insert(pEmpItemInfo->m_sAccount,pEmpItemInfo);
			m_treeSelected.Invalidate();
			UpdateSelectedUsers();
		}
	}
}
void CDlgSelectUser::SelectDepartment(HTREEITEM hSelItem)
{
	if (hSelItem != NULL)
	{
		const CTreeItemInfo* pDepItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
#ifdef USES_SELECTED_ITEM_UID
		if (pDepItemInfo != NULL && pDepItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && !m_pSelectedUserTreeItem.exist(pDepItemInfo->m_sAccount))
#else
		if (pDepItemInfo != NULL && pDepItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && !m_pSelectedTreeItem.exist(pDepItemInfo->m_sAccount))
#endif
		{
			if (m_bSingleSelect)
				ResetSelected();

			HTREEITEM hEmpItem = m_treeSelected.InsertItem(pDepItemInfo->m_sName.c_str());
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sGroupCode = pDepItemInfo->m_sGroupCode;
			pEmpItemInfo->m_sMemberCode = pDepItemInfo->m_sMemberCode;
			pEmpItemInfo->m_nUserId = pDepItemInfo->m_nUserId;
			pEmpItemInfo->m_sAccount = pDepItemInfo->m_sAccount;
			pEmpItemInfo->m_sName = pDepItemInfo->m_sName;
			//pEmpItemInfo->m_nIndex = pDepItemInfo->m_nIndex;	// ?
			m_treeSelected.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
#ifdef USES_SELECTED_ITEM_UID
			m_pSelectedUserTreeItem.insert(pEmpItemInfo->m_sAccount, pEmpItemInfo->m_nUserId);
#else
			m_pSelectedTreeItem.insert(pEmpItemInfo->m_sAccount, pEmpItemInfo->m_sMemberCode);
#endif
			m_pSelectedItem.insert(pEmpItemInfo->m_sAccount,pEmpItemInfo);
			m_treeSelected.Invalidate();
			UpdateSelectedUsers();
		}
	}
}

void CDlgSelectUser::SelectContact(HTREEITEM hSelItem)
{
	if (hSelItem != NULL)
	{
		const CTreeItemInfo* pConItemInfo = (const CTreeItemInfo*)m_treeContacts.GetItemData(hSelItem);
#ifdef USES_SELECTED_ITEM_UID
		if (pConItemInfo != NULL && pConItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT && pConItemInfo->m_nUserId>0 && !m_pSelectedUserTreeItem.exist(pConItemInfo->m_sAccount))
#else
		if (pConItemInfo != NULL && pConItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT && pConItemInfo->m_nUserId>0 && !m_pSelectedTreeItem.exist(pConItemInfo->m_sAccount))
#endif
		{
			if (m_bSingleSelect)
				ResetSelected();

			CString sText;
			sText.Format(_T("%s(%s)"),pConItemInfo->m_sName.c_str(),pConItemInfo->m_sAccount.c_str());
			HTREEITEM hContactItem = m_treeSelected.InsertItem(sText);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
			pContactItemInfo->m_sAccount = pConItemInfo->m_sAccount;
			pContactItemInfo->m_nUserId = pConItemInfo->m_nUserId;
			pContactItemInfo->m_sName = pConItemInfo->m_sName;
			m_treeSelected.SetItemData(hContactItem, (DWORD)pContactItemInfo.get());
#ifdef USES_SELECTED_ITEM_UID
			m_pSelectedUserTreeItem.insert(pConItemInfo->m_sAccount, pConItemInfo->m_nUserId);
#else
			m_pSelectedTreeItem.insert(pConItemInfo->m_sAccount, 0);
#endif
			m_pSelectedItem.insert(pConItemInfo->m_sAccount,pContactItemInfo);
			m_treeSelected.Invalidate();
			UpdateSelectedUsers();
		}

		//const EB_ContactInfo * pPopContactData = (EB_ContactInfo*)m_treeContacts.GetItemData(hSelItem);
		//if (pPopContactData != NULL)
		//{
		//	//EB_ContactInfo::pointer pContactInfo = theEBAppClient.GetContactInfo(pPopContactData);
		//}
	}
}
void CDlgSelectUser::SelectSearch(HTREEITEM hSelItem)
{
	const CTreeItemInfo* pItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hSelItem);
#ifdef USES_SELECTED_ITEM_UID
	if (pItemInfo == NULL || pItemInfo->m_nUserId==0 || m_pSelectedUserTreeItem.exist(pItemInfo->m_sAccount)) return;
#else
	if (pItemInfo == NULL || pItemInfo->m_nUserId==0 || m_pSelectedTreeItem.exist(pItemInfo->m_sAccount)) return;
#endif
	CString sText;
	sText.Format(_T("%s(%s)"),pItemInfo->m_sName.c_str(),pItemInfo->m_sAccount.c_str());
	if (pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		// employee info
		if (m_bSingleSelect)
			ResetSelected();

		HTREEITEM hEmpItem = m_treeSelected.InsertItem(sText);
		CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
		pEmpItemInfo->m_sAccount = pItemInfo->m_sAccount;
		pEmpItemInfo->m_nUserId = pItemInfo->m_nUserId;
		pEmpItemInfo->m_sName = pItemInfo->m_sName;
		//pEmpItemInfo->m_nIndex = pItemInfo->m_nIndex;	// ?
		m_treeSelected.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
#ifdef USES_SELECTED_ITEM_UID
		m_pSelectedUserTreeItem.insert(pItemInfo->m_sAccount,pItemInfo->m_nUserId);
#else
		m_pSelectedTreeItem.insert(pItemInfo->m_sAccount,pItemInfo->m_sMemberCode);
#endif
		m_pSelectedItem.insert(pItemInfo->m_sAccount,pEmpItemInfo);
		m_editSearch.SetWindowText(pItemInfo->m_sAccount.c_str());
		m_editSearch.SetSel(0,-1);
		m_treeSelected.Invalidate();
		UpdateSelectedUsers();
	}else if (pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
	{
		// contact info
		if (m_bSingleSelect)
			ResetSelected();

		HTREEITEM hContactItem = m_treeSelected.InsertItem(sText);
		CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
		pContactItemInfo->m_sAccount = pItemInfo->m_sAccount;
		pContactItemInfo->m_nUserId = pItemInfo->m_nUserId;
		pContactItemInfo->m_sName = pItemInfo->m_sName;
		m_treeSelected.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
#ifdef USES_SELECTED_ITEM_UID
		m_pSelectedUserTreeItem.insert(pItemInfo->m_sAccount,pItemInfo->m_nUserId);
#else
		m_pSelectedTreeItem.insert(pItemInfo->m_sAccount,pItemInfo->m_sMemberCode);
#endif
		m_pSelectedItem.insert(pItemInfo->m_sAccount,pContactItemInfo);
		m_editSearch.SetWindowText(pItemInfo->m_sAccount.c_str());
		m_editSearch.SetSel(0,-1);
		m_treeSelected.Invalidate();
		UpdateSelectedUsers();
	}
}

void CDlgSelectUser::DeleteSelected(HTREEITEM hSelItem)
{
	if (hSelItem != NULL)
	{
		const CTreeItemInfo* pTreItemInfo = (const CTreeItemInfo*)m_treeSelected.GetItemData(hSelItem);
		if (pTreItemInfo == NULL) return;
		m_treeSelected.DeleteItem(hSelItem);
#ifdef USES_SELECTED_ITEM_UID
		m_pSelectedUserTreeItem.remove(pTreItemInfo->m_sAccount);
#else
		m_pSelectedTreeItem.remove(pTreItemInfo->m_sAccount);
#endif
		m_pSelectedItem.remove(pTreItemInfo->m_sAccount);	// 这个必须放在最后面，否则pTreItemInfo会无效。
	}
}

LRESULT CDlgSelectUser::OnTreeItemSelChanged(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hSelItem = (HTREEITEM)wp;
	if (hSelItem==NULL) return 0;
	const VividTree * pOwnerTree = (VividTree*)lp;
	if (pOwnerTree != &m_treeEnterprise)
		return 0;

	CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP &&
		pTreeItemInfo->m_dwItemData>0 && ((int)pTreeItemInfo->m_dwItemData)>GetLocalMemberSize(pTreeItemInfo->m_sGroupCode))
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_LoadGroup(pTreeItemInfo->m_sGroupCode,VARIANT_TRUE);
#else
		theEBAppClient.EB_LoadGroup(pTreeItemInfo->m_sGroupCode,true);
#endif
	}
	return 0;
}

LRESULT CDlgSelectUser::OnTreeItemDoubleClieck(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	VividTree* pOwner = (VividTree*)lp;
	if (pOwner == &m_treeDepartment)
	{
		SelectDepartment(m_treeDepartment.GetSelectedItem());
	}else if (pOwner == &m_treeEnterprise)
	{
		SelectEnterprise(m_treeEnterprise.GetSelectedItem());
	}else if (pOwner == &m_treeContacts)
	{
		SelectContact(m_treeContacts.GetSelectedItem());
	}else if (pOwner == &m_treeSearch)
	{
		SelectSearch(m_treeSearch.GetSelectedItem());
	}else if (pOwner == &m_treeSelected)
	{
		DeleteSelected(m_treeSelected.GetSelectedItem());
	}
	return 0;
}

LRESULT CDlgSelectUser::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	VividTree * pOwner = (VividTree*)lp;
	const int const_btn_width = m_btnTrackAdd0.GetImgWidth();
	//const int const_btn_height = m_btnTrackAdd0.GetImgHeight();
	if (item == NULL)
	{
		m_btnTrackAdd0.ShowWindow(SW_HIDE);
		m_btnTrackAdd1.ShowWindow(SW_HIDE);
		m_btnTrackAdd2.ShowWindow(SW_HIDE);
		m_btnTrackAdd3.ShowWindow(SW_HIDE);
		m_btnTrackDelete.ShowWindow(SW_HIDE);
	}else if (pOwner == &m_treeDepartment)
	{
		if (m_treeDepartment.GetNextItem(item, TVGN_CHILD)==NULL)
		{
			const CTreeItemInfo* pDepItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(item);
#ifdef USES_SELECTED_ITEM_UID
			if (pDepItemInfo != NULL && !m_pSelectedUserTreeItem.exist(pDepItemInfo->m_sAccount))
#else
			if (pDepItemInfo != NULL && !m_pSelectedTreeItem.exist(pDepItemInfo->m_sAccount))
#endif
			{
				CRect rect;
				m_treeDepartment.GetItemRect(item, &rect, TRUE);
				const int nTop = rect.top;
				m_treeDepartment.GetClientRect(&rect);
				const int nRight = rect.right;
				m_btnTrackAdd0.MovePoint(nRight-const_btn_width, nTop);
				m_btnTrackAdd0.ShowWindow(SW_SHOW);
				m_btnTrackAdd0.Invalidate();
			}else
			{
				m_btnTrackAdd0.ShowWindow(SW_HIDE);
			}
		}else
		{
			m_btnTrackAdd0.ShowWindow(SW_HIDE);
		}
	}else if (pOwner == &m_treeEnterprise)
	{
		if (m_treeEnterprise.GetNextItem(item, TVGN_CHILD)==NULL)
		{
			const CTreeItemInfo* pDepItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(item);
#ifdef USES_SELECTED_ITEM_UID
			if (pDepItemInfo != NULL && !m_pSelectedUserTreeItem.exist(pDepItemInfo->m_sAccount))
#else
			if (pDepItemInfo != NULL && !m_pSelectedTreeItem.exist(pDepItemInfo->m_sAccount))
#endif
			{
				CRect rect;
				m_treeEnterprise.GetItemRect(item, &rect, TRUE);
				const int nTop = rect.top;
				m_treeEnterprise.GetClientRect(&rect);
				const int nRight = rect.right;
				m_btnTrackAdd1.MovePoint(nRight-const_btn_width, nTop);
				m_btnTrackAdd1.ShowWindow(SW_SHOW);
				m_btnTrackAdd1.Invalidate();
			}else
			{
				m_btnTrackAdd1.ShowWindow(SW_HIDE);
			}
		}else
		{
			m_btnTrackAdd1.ShowWindow(SW_HIDE);
		}
	}else if (pOwner == &m_treeContacts)
	{
		if (m_treeContacts.GetNextItem(item, TVGN_CHILD)==NULL)
		{
			const CTreeItemInfo* pConItemInfo = (const CTreeItemInfo*)m_treeContacts.GetItemData(item);
#ifdef USES_SELECTED_ITEM_UID
			if (pConItemInfo != NULL && pConItemInfo->m_nUserId>0 && !m_pSelectedUserTreeItem.exist(pConItemInfo->m_sAccount))
#else
			if (pConItemInfo != NULL && pConItemInfo->m_nUserId>0 && !m_pSelectedTreeItem.exist(pConItemInfo->m_sAccount))
#endif
			{
				CRect rect;
				m_treeContacts.GetItemRect(item, &rect, TRUE);
				const int nTop = rect.top;
				m_treeContacts.GetClientRect(&rect);
				const int nRight = rect.right;
				m_btnTrackAdd2.MovePoint(nRight-const_btn_width, nTop);
				m_btnTrackAdd2.ShowWindow(SW_SHOW);
				m_btnTrackAdd2.Invalidate();
			}else
			{
				m_btnTrackAdd2.ShowWindow(SW_HIDE);
			}
		}else
		{
			m_btnTrackAdd2.ShowWindow(SW_HIDE);
		}
	}else if (pOwner == &m_treeSearch)
	{
		const CTreeItemInfo* pItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(item);
#ifdef USES_SELECTED_ITEM_UID
		if (pItemInfo == NULL || m_pSelectedUserTreeItem.exist(pItemInfo->m_sAccount))
#else
		if (pItemInfo == NULL || m_pSelectedTreeItem.exist(pItemInfo->m_sAccount))
#endif
		{
			m_btnTrackAdd3.ShowWindow(SW_HIDE);
		}else
		{
			CRect rect;
			m_treeSearch.GetItemRect(item, &rect, TRUE);
			const int nTop = rect.top;
			m_treeSearch.GetClientRect(&rect);
			const int nRight = rect.right;
			m_btnTrackAdd3.MovePoint(nRight-const_btn_width, nTop);
			m_btnTrackAdd3.ShowWindow(SW_SHOW);
			m_btnTrackAdd3.Invalidate();
		}
	}else if (pOwner == &m_treeSelected)
	{
		CRect rect;
		m_treeSelected.GetItemRect(item, &rect, TRUE);
		const int nTop = rect.top;
		m_treeSelected.GetClientRect(&rect);
		const int nRight = rect.right;
		m_btnTrackDelete.MovePoint(nRight-const_btn_width, nTop);
		m_btnTrackDelete.ShowWindow(SW_SHOW);
		m_btnTrackDelete.Invalidate();
	}
	return 0;
}
BOOL CDlgSelectUser::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnTrackAdd0.GetSafeHwnd())
	{
		SelectDepartment(m_treeDepartment.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnTrackAdd1.GetSafeHwnd())
	{
		SelectEnterprise(m_treeEnterprise.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnTrackAdd2.GetSafeHwnd())
	{
		SelectContact(m_treeContacts.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnTrackAdd3.GetSafeHwnd())
	{
		SelectSearch(m_treeSearch.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnTrackDelete.GetSafeHwnd())
	{
		DeleteSelected(m_treeSelected.GetTrackItem());
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}

void CDlgSelectUser::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TIMERID_LOAD_ENTERPRISE_INFO == nIDEvent)
	{
		KillTimer(TIMERID_LOAD_ENTERPRISE_INFO);
		//EB_EnterpriseInfo::pointer pEnterpriseInfo = theEBAppClient.GetEnterpriseInfo();
		//if (pEnterpriseInfo.get() != NULL)
		//{
		//	m_hEnterprise = m_treeEnterprise.InsertItem(pEnterpriseInfo->m_sEnterpriseName.c_str());
		//	theEBAppClient.LoadDepartmentInfo(pEnterpriseInfo->m_sEnterpriseCode, (CEBSearchCallback*)this);
		//	m_treeEnterprise.Expand(m_hEnterprise, TVE_EXPAND);
		//}
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_FindAllGroupInfo((IDispatch*)this,0,0);
		theEBClientCore->EB_FindAllContactInfo((IDispatch*)this,0);
#else
		theEBAppClient.EB_FindAllGroupInfo((CEBSearchCallback*)this,0);
		theEBAppClient.EB_FindAllContactInfo((CEBSearchCallback*)this);
#endif
	}
	__super::OnTimer(nIDEvent);
}

void CDlgSelectUser::OnEnChangeEditSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the __super::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString sSearchString;
	this->GetDlgItemText(IDC_EDIT_SEARCH, sSearchString);
	if (!sSearchString.IsEmpty())
	{
		m_treeSearch.DeleteAllItems();
		m_pSearchItem1.clear();
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_SearchUserInfo((IDispatch*)this,(LPCTSTR)sSearchString, 1);
#else
		theEBAppClient.EB_SearchUserInfo((CEBSearchCallback*)this, sSearchString, 1);
#endif

		m_treeDepartment.ShowWindow(SW_HIDE);
		m_treeEnterprise.ShowWindow(SW_HIDE);
		m_treeContacts.ShowWindow(SW_HIDE);
		CRect rect;
		m_treeDepartment.GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		m_treeSearch.MoveWindow(&rect);
		m_treeSearch.ShowWindow(SW_SHOW);
	}
	if (m_treeSearch.GetCount() == 0 || sSearchString.IsEmpty())
	{
		m_treeSearch.ShowWindow(SW_HIDE);
		if (m_btnMyDepartment.GetChecked())
			m_treeDepartment.ShowWindow(SW_SHOW);
		else if (m_btnMyContacts.GetChecked())
			m_treeContacts.ShowWindow(SW_SHOW);
		else if (m_btnMyEnterprise.GetChecked())
			m_treeEnterprise.ShowWindow(SW_SHOW);
	}
}


void CDlgSelectUser::OnDestroy()
{
	__super::OnDestroy();
	m_btnTrackAdd0.DestroyWindow();
	m_btnTrackAdd1.DestroyWindow();
	m_btnTrackAdd2.DestroyWindow();
	m_btnTrackAdd3.DestroyWindow();
	m_btnTrackDelete.DestroyWindow();
}

//bool CDlgSelectUser::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return false;
//	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE)
//	{
//		pImage1 = theApp.m_imageDefaultOrg->Clone();
//		return true;
//	}
//	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
//	{
//		return false;
//	}
//	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
//	{
//		pImage1 = theApp.m_imageDefaultContact->Clone();
//		return true;
//	}
//	else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
//	{
//		CEBString sImagePath;
//		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
//#ifdef USES_EBCOM_TEST
//		CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pTreeItemInfo->m_sMemberCode);
//		if (pMemberInfo != NULL)
//		{
//			pOutLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
//			//sDescription = pMemberInfo->Description.GetBSTR();
//			const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
//			if (PathFileExists(sMemberHeadFile.c_str()))
//			{
//				sImagePath = sMemberHeadFile;
//			}
//		}
//#else
//		EB_MemberInfo pMemberInfo;
//		if (theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,pTreeItemInfo->m_sMemberCode))
//		{
//			//sDescription = pMemberInfo.m_sDescription;
//			pOutLineState = pMemberInfo.m_nLineState;
//			if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
//			{
//				sImagePath = pMemberInfo.m_sHeadResourceFile;
//			}
//		}
//#endif
//		switch (pOutLineState)
//		{
//		case EB_LINE_STATE_UNKNOWN:
//		case EB_LINE_STATE_OFFLINE:
//			pState = 0;
//			break;
//		case EB_LINE_STATE_ONLINE_NEW:
//			break;
//		case EB_LINE_STATE_BUSY:
//			{
//				pImage2 = theApp.m_imageStateBusy->Clone();
//			}break;
//		case EB_LINE_STATE_AWAY:
//			{
//				pImage2 = theApp.m_imageStateAway->Clone();
//			}break;
//		default:
//			break;
//		}
//		if (!sImagePath.empty())
//		{
//			USES_CONVERSION;
//			pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
//			return true;
//		}else
//		{
//			pImage1 = theApp.m_imageDefaultMember->Clone();
//			return true;
//			//return libEbc::ImageFromIDResource(IDB_PNG_DEFAULT_MEMBER,_T("png"),pImage1)?true:false;
//		}
//	}
//	return false;
//}

void CDlgSelectUser::OnOK()
{
	CString sAddAccount;
	m_editSearch.GetWindowText(sAddAccount);
	if (!sAddAccount.IsEmpty())
	{
#ifdef USES_SELECTED_ITEM_UID
		m_pSelectedUserTreeItem.insert((LPCTSTR)sAddAccount,0,false);
#else
		m_pSelectedTreeItem.insert((LPCTSTR)sAddAccount,0,false);
#endif
		UpdateSelectedUsers();
	}
	__super::OnOK();
}

void CDlgSelectUser::OnNMThemeChangedTreeDepartment(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
