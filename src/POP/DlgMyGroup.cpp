// DlgMyGroup.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgMyGroup.h"
#include "DlgGroupInfo.h"
#include "DlgMemberInfo.h"
#include "DlgViewContactInfo.h"


#define TIMERID_EDIT_MEMBERINFO			100
#define TIMERID_CHECK_ITEM_HOT			101
#define TIMERID_CHECK_SELECT_FIRST	102

// CDlgMyGroup dialog

IMPLEMENT_DYNAMIC(CDlgMyGroup, CDialog)

CDlgMyGroup::CDlgMyGroup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMyGroup::IDD, pParent)
{
	m_pViewContactInfo = NULL;
	m_hCurrentHotItem = NULL;
}

CDlgMyGroup::~CDlgMyGroup()
{
}

void CDlgMyGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DEPARTMENT, m_treeDepartment);
}


BEGIN_MESSAGE_MAP(CDlgMyGroup, CDialog)
	ON_WM_SIZE()
	ON_COMMAND(EB_COMMAND_CALL_USER, &CDlgMyGroup::OnCallUser)
	ON_COMMAND(EB_MSG_VIEW_MSG_RECORD, &CDlgMyGroup::OnViewMsgRecord)
	//ON_COMMAND(EB_COMMAND_DELETE_MSG_RECORD, &CDlgMyGroup::OnDeleteMsgRecord)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_DEPARTMENT, &CDlgMyGroup::OnNMRClickTreeDepartment)
	ON_MESSAGE(WM_ITEM_SEL_CHANGED, OnTreeItemSelChanged)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDblclk)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_COMMAND(EB_COMMAND_VIEW_GROUP_SHARE, &CDlgMyGroup::OnGroupShare)
	ON_COMMAND(EB_COMMAND_NEW_DEPARTMENT3, &CDlgMyGroup::OnDepartment3New)
	ON_COMMAND(EB_COMMAND_DELETE_DEPARTMENT, &CDlgMyGroup::OnDepartmentDelete)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_ADD_EMP, &CDlgMyGroup::OnDepartmentAddEmp)
	ON_COMMAND(EB_COMMAND_EXIT_DEPARTMENT, &CDlgMyGroup::OnDepartmentExit)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_DEL_EMP, &CDlgMyGroup::OnDepartmentDelEmp)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_EDIT_EMP, &CDlgMyGroup::OnDepartmentEditEmp)
	ON_COMMAND(EB_COMMAND_MEMBER_DEL_ADMIN, &CDlgMyGroup::OnMemberDelAdminLevel)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_ADMIN, &CDlgMyGroup::OnMemberAddAdminLevel)
	
	ON_WM_TIMER()
	ON_COMMAND(EB_COMMAND_EDIT_DEPARTMENT, &CDlgMyGroup::OnDepartmentEdit)
	ON_WM_DESTROY()
	ON_COMMAND_RANGE(EB_COMMAND_SUBSCRIBE_FUNC,EB_COMMAND_SUBSCRIBE_FUNC+0x20,OnSubscribeFunc)
END_MESSAGE_MAP()


// CDlgMyGroup message handlers
void CDlgMyGroup::SetCtrlColor(void)
{
	if (m_pViewContactInfo!=NULL && m_pViewContactInfo->GetSafeHwnd()!=NULL)
		m_pViewContactInfo->SetCtrlColor();

	//if (theApp.GetColorFlat())
	//	m_treeDepartment.SetBkGradients(theApp.GetFlatBgColor(),theApp.GetFlatBgColor());
	//else
	//	m_treeDepartment.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());
}

BOOL CDlgMyGroup::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnCallTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeDepartment, 0xffff);
	m_btnCallTrack.SetAutoSize(false);
	m_btnCallTrack.SetAutoFocus(true);
	m_btnCallTrack.Load(IDB_PNG_HOT_CALL);
	m_btnCallTrack.SetToolTipText(_T("打开会话"));
	m_btnEditEmpTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeDepartment, 0xffff);
	m_btnEditEmpTrack.SetAutoSize(false);
	m_btnEditEmpTrack.SetAutoFocus(true);
	m_btnEditEmpTrack.Load(IDB_PNG_HOT_EDIT);
	m_btnEditEmpTrack.SetToolTipText(_T("修改我的名片"));

	//m_treeDepartment.SetBkMode(VividTree::BK_MODE_GRADIENT);
	//m_treeDepartment.SetBkGradients(theDefaultBtnWhiteColor,theDefaultBtnWhiteColor);
	m_treeDepartment.SetCallback(&theApp);
	m_treeDepartment.ModifyStyle(0, TVS_SINGLEEXPAND);
	//m_treeDepartment.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeDepartment.SetItemHeight(40);	// 24
	m_treeDepartment.SetIconSize(32,32);
	//m_treeDepartment.SetBkGradients(RGB(252,252,252),RGB(252,252,252));
	//m_treeDepartment.SetItemIcon( theApp.GetIconCon() );
	//m_icon = AfxGetApp()->LoadIcon(IDI_DEPARTMENT);
	//m_treeDepartment.SetRootIcon( m_icon );
	//m_icon = AfxGetApp()->LoadIcon(IDI_DEPARTMENT);
	//m_treeDepartment.SetParentIcon( m_icon );

	SetCtrlColor();

	//SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMyGroup::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void CDlgMyGroup::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CDlgMyGroup::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_treeDepartment.GetSafeHwnd())
		m_treeDepartment.MoveWindow(0, 0, cx, cy);
}

//#ifdef USES_EBCOM_TEST
//void CDlgMyGroup::SetMemberInfo(HTREEITEM hGroupItem, IEB_MemberInfo* pEBEmployeeInfo)
//{
//	const eb::bigint sGroupCode = pEBEmployeeInfo->GroupCode;
//	const eb::bigint sMemberCode = pEBEmployeeInfo->MemberCode;
//	const CEBString sMemberAccount = pEBEmployeeInfo->MemberAccount.GetBSTR();
//	const CEBString sUserName = pEBEmployeeInfo->UserName.GetBSTR();
//	const CEBString sJobTitle = pEBEmployeeInfo->JobTitle.GetBSTR();
//	const CEBString sCellPhone = pEBEmployeeInfo->CellPhone.GetBSTR();
//	if (hGroupItem != NULL && pEBEmployeeInfo != NULL && !sMemberCode.empty())
//	{
//		CString sText;
//		sText.Format(_T("%s-%s %s"), sUserName.c_str(), sJobTitle.c_str(), sCellPhone.c_str());
//		CTreeItemInfo::pointer pEmpItemInfo;
//		if (!m_pEmpItemInfo.find(sMemberCode, pEmpItemInfo))
//		{
//			HTREEITEM hEmpItem = m_treeDepartment.InsertItem(sText, hGroupItem);
//			pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
//			m_pEmpItemInfo.insert(sMemberCode, pEmpItemInfo);
//			m_treeDepartment.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
//		}else
//		{
//			m_treeDepartment.SetItemText(pEmpItemInfo->m_hItem, sText);
//		}
//		pEmpItemInfo->m_sGroupCode = sGroupCode;
//		pEmpItemInfo->m_sMemberCode = sMemberCode;
//		pEmpItemInfo->m_sAccount = sMemberAccount;
//		pEmpItemInfo->m_sName = sUserName;
//		pEmpItemInfo->m_dwItemData = pEBEmployeeInfo->LineState;
//
//		m_treeDepartment.SelectItem(pEmpItemInfo->m_hItem);
//		// ?? 这里要实现，状况改变
//		m_treeDepartment.Sort(hGroupItem, CPOPApp::TreeCmpFunc);
//	}
//}
//#endif
#ifdef USES_EBCOM_TEST
void CDlgMyGroup::SetMemberInfo(HTREEITEM hGroupItem, IEB_MemberInfo* pEBEmployeeInfo)
{
	const eb::bigint sGroupCode = pEBEmployeeInfo->GroupCode;
	const eb::bigint sMemberCode = pEBEmployeeInfo->MemberCode;
	const eb::bigint sMemberUserId = pEBEmployeeInfo->MemberUserId;
	const CEBString sMemberAccount = pEBEmployeeInfo->MemberAccount.GetBSTR();
	const CEBString sUserName = pEBEmployeeInfo->UserName.GetBSTR();
	const CEBString sJobTitle = pEBEmployeeInfo->JobTitle.GetBSTR();
	const CEBString sCellPhone = pEBEmployeeInfo->CellPhone.GetBSTR();
	if (hGroupItem != NULL && pEBEmployeeInfo != NULL && sMemberCode>0)
	{
		CString sText;
		if (sJobTitle.empty())
			sText.Format(_T("%s"), sUserName.c_str());
		else
			sText.Format(_T("%s - %s"), sUserName.c_str(), sJobTitle.c_str());
		CTreeItemInfo::pointer pEmpItemInfo;
		if (!m_pEmpItemInfo.find(sMemberCode, pEmpItemInfo))
		{
			HTREEITEM hEmpItem = m_treeDepartment.InsertItem(sText, hGroupItem);
			pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			m_pEmpItemInfo.insert(sMemberCode, pEmpItemInfo);
			m_treeDepartment.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
		}else
		{
			m_treeDepartment.SetItemText(pEmpItemInfo->m_hItem, sText);
		}
		pEmpItemInfo->m_sGroupCode = sGroupCode;
		pEmpItemInfo->m_sMemberCode = sMemberCode;
		pEmpItemInfo->m_sAccount = sMemberAccount;
		pEmpItemInfo->m_nUserId = sMemberUserId;
		pEmpItemInfo->m_sName = sUserName;
		pEmpItemInfo->m_dwItemData = pEBEmployeeInfo->LineState;

		// **不需要选择
		//m_treeDepartment.SelectItem(pEmpItemInfo->m_hItem);
		// ?? 这里要实现，状况改变
		m_treeDepartment.Sort(hGroupItem, CPOPApp::TreeCmpFunc);
	}
}
#else
void CDlgMyGroup::SetMemberInfo(HTREEITEM hGroupItem, const EB_MemberInfo* pMemberInfo)
{
	if (hGroupItem != NULL && pMemberInfo != NULL && pMemberInfo->m_sMemberCode>0)
	{
		CString sText;
		if (pMemberInfo->m_sJobTitle.empty())
			sText.Format(_T("%s"), pMemberInfo->m_sUserName.c_str());
		else
			sText.Format(_T("%s - %s"), pMemberInfo->m_sUserName.c_str(), pMemberInfo->m_sJobTitle.c_str());
		CTreeItemInfo::pointer pEmpItemInfo;
		if (!m_pEmpItemInfo.find(pMemberInfo->m_sMemberCode, pEmpItemInfo))
		{
			HTREEITEM hEmpItem = m_treeDepartment.InsertItem(sText, hGroupItem);
			pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			m_pEmpItemInfo.insert(pMemberInfo->m_sMemberCode, pEmpItemInfo);
			m_treeDepartment.SetItemData(hEmpItem, (DWORD)pEmpItemInfo.get());
		}else
		{
			m_treeDepartment.SetItemText(pEmpItemInfo->m_hItem, sText);
		}
		pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
		pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
		pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
		pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
		pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
		pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;

		if (theApp.IsEnterpriseCreateUserId(pMemberInfo->m_nMemberUserId))
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

		// **不需要选择
		//m_treeDepartment.SelectItem(pEmpItemInfo->m_hItem);
		// ?? 这里要实现，状况改变
		m_treeDepartment.Sort(hGroupItem, CPOPApp::TreeCmpFunc);
	}
}
#endif
//
//#ifdef USES_EBCOM_TEST
//void CDlgMyGroup::MyDepartmentInfo(IEB_GroupInfo* pEBGroupInfo)
//{
//	//HTREEITEM hGroupItem = m_treeDepartment.InsertItem("adsf");
//	//m_treeDepartment.SetItemData(hGroupItem, 0);
//	//return;
//
//	const CEBString sEnterpriseCode = pEBGroupInfo->EnterpriseCode;
//	const eb::bigint sGroupCode = pEBGroupInfo->GroupCode;
//	const CEBString sGroupName = pEBGroupInfo->GroupName.GetBSTR();
//	std::vector<CComPtr<IEB_MemberInfo>> pOutMemberInfoList;
//	_variant_t pMemberInfoList = theEBClientCore->EB_GetGroupMemberInfoList(sGroupCode);
//	if (pMemberInfoList.vt!=VT_EMPTY && pMemberInfoList.parray != NULL)
//	{
//		CComSafeArray<VARIANT> m_sa(pMemberInfoList.parray);
//		for (ULONG i=0;i<m_sa.GetCount();i++)
//		{
//			CComVariant var = m_sa.GetAt(i);
//			if (var.vt != VT_DISPATCH)
//				continue;
//			CComPtr<IEB_MemberInfo> pInterface;
//			var.pdispVal->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
//			pOutMemberInfoList.push_back(pInterface);
//		}
//	}
//
//	CString sText;
//	sText.Format(_T("%s(%d)"), sGroupName.c_str(), pOutMemberInfoList.size());
//	CTreeItemInfo::pointer pDepItemInfo;
//	if (!m_pDepItemInfo.find(sGroupCode, pDepItemInfo))
//	{
//		HTREEITEM hGroupItem = m_treeDepartment.InsertItem(sText);
//		pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
//		pDepItemInfo->m_sEnterpriseCode = sEnterpriseCode;
//		pDepItemInfo->m_sGroupCode = sGroupCode;
//		pDepItemInfo->m_sName = sGroupName;
//		m_pDepItemInfo.insert(sGroupCode,pDepItemInfo);
//		m_treeDepartment.SetItemData(hGroupItem, (DWORD)pDepItemInfo.get());
//		if (m_treeDepartment.GetTrackItem()==NULL)
//		{
//			m_treeDepartment.SelectItem(hGroupItem);
//		}
//	}else
//	{
//		m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
//	}
//	for (size_t i=0;i<pOutMemberInfoList.size(); i++)
//	{
//		const CComPtr<IEB_MemberInfo> pMemberInfo = pOutMemberInfoList[i];
//		if (pMemberInfo->MemberCode.length()==0) continue;
//		SetMemberInfo(pDepItemInfo->m_hItem, pMemberInfo);
//	}
//
//	m_treeDepartment.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
//	m_treeDepartment.Expand(pDepItemInfo->m_hItem,TVE_EXPAND);
//}
//#endif
#ifdef USES_EBCOM_TEST
void CDlgMyGroup::MyDepartmentInfo(IEB_GroupInfo* pEBGroupInfo)
{
	//HTREEITEM hGroupItem = m_treeDepartment.InsertItem("adsf");
	//m_treeDepartment.SetItemData(hGroupItem, 0);
	//return;

	const eb::bigint sEnterpriseCode = pEBGroupInfo->EnterpriseCode;
	const eb::bigint sGroupCode = pEBGroupInfo->GroupCode;
	const CEBString sGroupName = pEBGroupInfo->GroupName.GetBSTR();
	std::vector<CComPtr<IEB_MemberInfo>> pOutMemberInfoList;
	_variant_t pMemberInfoList = theEBClientCore->EB_GetGroupMemberInfoList(sGroupCode);
	if (pMemberInfoList.vt!=VT_EMPTY && pMemberInfoList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pMemberInfoList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_MemberInfo> pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
			pOutMemberInfoList.push_back(pInterface);
		}
	}

	CString sText;
	sText.Format(_T("%s(%d)"), sGroupName.c_str(), pEBGroupInfo->EmpCount);
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(sGroupCode, pDepItemInfo))
	{
		HTREEITEM hGroupItem = m_treeDepartment.InsertItem(sText);
		pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		pDepItemInfo->m_sEnterpriseCode = sEnterpriseCode;
		pDepItemInfo->m_sGroupCode = sGroupCode;
		pDepItemInfo->m_nSubType = pGroupInfo->GroupType;
		pDepItemInfo->m_sName = sGroupName;
		pDepItemInfo->m_nBigId = pGroupInfo->MyEmpId;
		m_pDepItemInfo.insert(sGroupCode,pDepItemInfo);
		m_treeDepartment.SetItemData(hGroupItem, (DWORD)pDepItemInfo.get());
		if (m_treeDepartment.GetTrackItem()==NULL)
		{
			m_treeDepartment.SelectItem(hGroupItem);
		}
	}else
	{
		m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
	}
	// **需要加载，否则会导致成员显示不完整；
	for (size_t i=0;i<pOutMemberInfoList.size(); i++)
	{
		const CComPtr<IEB_MemberInfo> pMemberInfo = pOutMemberInfoList[i];
		if (pMemberInfo->MemberCode==0) continue;
		SetMemberInfo(pDepItemInfo->m_hItem, pMemberInfo);
	}
	const int nMemberSize = pEBGroupInfo->EmpCount;
	const int nOnlineSize = theEBClientCore->EB_GetGroupOnlineSize(sGroupCode);
	if (nOnlineSize>=0)
		sText.Format(_T("%s [%d/%d]"), sGroupName.c_str(),nOnlineSize,nMemberSize);
	else
		sText.Format(_T("%s [%d]"), sGroupName.c_str(),nMemberSize);
	m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
	m_treeDepartment.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	//m_treeDepartment.Expand(pDepItemInfo->m_hItem,TVE_EXPAND);
}
#else
void CDlgMyGroup::MyDepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	std::vector<EB_MemberInfo> pOutMemberInfoList;
	if (!theEBAppClient.EB_GetGroupMemberInfoList(pGroupInfo->m_sGroupCode,pOutMemberInfoList))
		return;

	CString sText;
	sText.Format(_T("%s(%d)"), pGroupInfo->m_sGroupName.c_str(), pGroupInfo->m_nEmpCount);
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(pGroupInfo->m_sGroupCode, pDepItemInfo))
	{
		HTREEITEM hGroupItem = m_treeDepartment.InsertItem(sText);
		pDepItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hGroupItem);
		pDepItemInfo->m_sEnterpriseCode = pGroupInfo->m_sEnterpriseCode;
		pDepItemInfo->m_sGroupCode = pGroupInfo->m_sGroupCode;
		pDepItemInfo->m_nSubType = pGroupInfo->m_nGroupType;
		pDepItemInfo->m_sName = pGroupInfo->m_sGroupName;
		pDepItemInfo->m_dwItemData = 0;
		pDepItemInfo->m_nBigId = pGroupInfo->m_nMyEmpId;
		m_pDepItemInfo.insert(pGroupInfo->m_sGroupCode,pDepItemInfo);
		m_treeDepartment.SetItemData(hGroupItem, (DWORD)pDepItemInfo.get());
		//if (m_treeDepartment.GetTrackItem()==NULL)
		//{
		//	m_treeDepartment.SelectItem(hGroupItem);
		//}
	}else
	{
		m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
	}
	// **需要加载，否则会导致成员显示不完整；
	for (size_t i=0;i<pOutMemberInfoList.size(); i++)
	{
		const EB_MemberInfo& pMemberInfo = pOutMemberInfoList[i];
		if (pMemberInfo.m_sMemberCode==0) continue;
		SetMemberInfo(pDepItemInfo->m_hItem, &pMemberInfo);
	}
	const int nMemberSize = pGroupInfo->m_nEmpCount;
	const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(pGroupInfo->m_sGroupCode,1);
	if (nOnlineSize>=0)
		sText.Format(_T("%s [%d/%d]"), pGroupInfo->m_sGroupName.c_str(),nOnlineSize,nMemberSize);
	else
		sText.Format(_T("%s [%d]"), pGroupInfo->m_sGroupName.c_str(),nMemberSize);
	m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
	m_treeDepartment.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	//m_treeDepartment.Expand(pDepItemInfo->m_hItem,TVE_EXPAND);
	
	if (m_treeDepartment.GetSelectedItem()==NULL)
	{
		KillTimer(TIMERID_CHECK_SELECT_FIRST);
		SetTimer(TIMERID_CHECK_SELECT_FIRST,2000,NULL);
	}
}
#endif

//#ifdef USES_EBCOM_TEST
//void CDlgMyGroup::MyDepMemberInfo(IEB_MemberInfo* pEBEmployeeInfo)
//{
//	const eb::bigint sGroupCode = pEBEmployeeInfo->GroupCode;
//	CTreeItemInfo::pointer pDepItemInfo;
//	if (!m_pDepItemInfo.find(sGroupCode,pDepItemInfo))
//	{
//		return;
//	}
//	SetMemberInfo(pDepItemInfo->m_hItem, pEBEmployeeInfo);
//	const CEBString sGroupName = theEBClientCore->EB_GetGroupName(sGroupCode).GetBSTR();
//	if (!sGroupName.empty())
//	{
//		int nMemberSize = theEBClientCore->EB_GetGroupMemberSize(sGroupCode);
//		CString sText;
//		sText.Format(_T("%s(%d)"), sGroupName.c_str(),nMemberSize);
//		m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
//	}
//	m_treeDepartment.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
//}
//#endif
#ifdef USES_EBCOM_TEST
void CDlgMyGroup::MyDepMemberInfo(IEB_MemberInfo* pEBEmployeeInfo, bool bChangeLineState)
{
	const eb::bigint sGroupCode = pEBEmployeeInfo->GroupCode;
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(sGroupCode,pDepItemInfo))
	{
		return;
	}
	// 已经加载成员
	if (pDepItemInfo->m_dwItemData==0)
		pDepItemInfo->m_dwItemData = 1;
	SetMemberInfo(pDepItemInfo->m_hItem, pEBEmployeeInfo);
	if (bChangeLineState)
	{
		const CEBString sGroupName = theEBClientCore->EB_GetGroupName(sGroupCode).GetBSTR();
		if (!sGroupName.empty())
		{
			const int nMemberSize = theEBClientCore->EB_GetGroupMemberSize(sGroupCode);
			const int nOnlineSize = theEBClientCore->EB_GetGroupOnlineSize(sGroupCode);
			CString sText;
			if (nOnlineSize>=0)
				sText.Format(_T("%s [%d/%d]"), sGroupName.c_str(),nOnlineSize,nMemberSize);
			else
				sText.Format(_T("%s [%d]"), sGroupName.c_str(),nMemberSize);
			m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
		}
	}
	m_treeDepartment.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
}
#else
void CDlgMyGroup::MyDepMemberInfo(const EB_MemberInfo* pMemberInfo, bool bChangeLineState)
{
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(pMemberInfo->m_sGroupCode,pDepItemInfo))
	{
		return;
	}
	// 已经加载成员
	if (pDepItemInfo->m_dwItemData==0)
		pDepItemInfo->m_dwItemData = 1;
	SetMemberInfo(pDepItemInfo->m_hItem, pMemberInfo);
	if (bChangeLineState)
	{
		CEBString sGroupName;
		if (theEBAppClient.EB_GetGroupName(pMemberInfo->m_sGroupCode,sGroupName))
		{
			int nMemberSize = 0;
			int nOnlineSize = 0;
			theEBAppClient.EB_GetGroupMemberSize(pMemberInfo->m_sGroupCode,1,nMemberSize,nOnlineSize);
			//const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(pMemberInfo->m_sGroupCode,1);
			//const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(pMemberInfo->m_sGroupCode,1);
			CString sText;
			if (nOnlineSize>=0)
				sText.Format(_T("%s [%d/%d]"), sGroupName.c_str(),nOnlineSize,nMemberSize);
			else
				sText.Format(_T("%s [%d]"), sGroupName.c_str(),nMemberSize);
			m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
		}
	}
	m_treeDepartment.Sort(pDepItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
}
#endif

//#ifdef USES_EBCOM_TEST
//void CDlgMyGroup::DeleteEmployeeInfo(IEB_MemberInfo* pMemberInfo)
//{
//	if (pMemberInfo == NULL) return;
//	const eb::bigint sGroupCode(pMemberInfo->GroupCode);
//	const eb::bigint sMemberCode(pMemberInfo->MemberCode);
//	CTreeItemInfo::pointer pDepItemInfo;
//	if (m_pDepItemInfo.find(sGroupCode, pDepItemInfo))
//	{
//		CTreeItemInfo::pointer pEmpItemInfo;
//		if (m_pEmpItemInfo.find(sMemberCode, pEmpItemInfo, true))
//		{
//			m_treeDepartment.DeleteItem(pEmpItemInfo->m_hItem);
//		}
//	}
//}
//#endif
#ifdef USES_EBCOM_TEST
void CDlgMyGroup::DeleteEmployeeInfo(IEB_GroupInfo* pGroupInfo, eb::bigint sMemberCode)
{
	eb::bigint nGroupCode = pGroupInfo->GroupCode;
	CTreeItemInfo::pointer pDepItemInfo;
	if (m_pDepItemInfo.find(nGroupCode, pDepItemInfo))
	{
		CTreeItemInfo::pointer pEmpItemInfo;
		if (m_pEmpItemInfo.find(sMemberCode, pEmpItemInfo, true))
		{
			m_treeDepartment.DeleteItem(pEmpItemInfo->m_hItem);
		}
		const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
		//const CEBString sGroupName = theEBClientCore->EB_GetGroupName(sGroupCode).GetBSTR();
		//if (!sGroupName.empty())
		{
			const int nMemberSize = theEBClientCore->EB_GetGroupMemberSize(nGroupCode);
			const int nOnlineSize = theEBClientCore->EB_GetGroupOnlineSize(nGroupCode);
			CString sText;
			sText.Format(_T("%s [%d/%d]"), sGroupName.c_str(),nOnlineSize,nMemberSize);
			m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
		}
	}
}
#else
void CDlgMyGroup::DeleteEmployeeInfo(const EB_GroupInfo* pGroupInfo, eb::bigint nMemberCode)
{
	eb::bigint nGroupCode = pGroupInfo->m_sGroupCode;
	CTreeItemInfo::pointer pDepItemInfo;
	if (m_pDepItemInfo.find(nGroupCode, pDepItemInfo))
	{
		CTreeItemInfo::pointer pEmpItemInfo;
		if (m_pEmpItemInfo.find(nMemberCode, pEmpItemInfo, true))
		{
			m_treeDepartment.DeleteItem(pEmpItemInfo->m_hItem);
		}
		//CEBString sGroupName;
		//if (theEBAppClient.EB_GetGroupName(nGroupCode,sGroupName))
		{
			int nMemberSize = 0;
			int nOnlineSize = 0;
			theEBAppClient.EB_GetGroupMemberSize(nGroupCode,1,nMemberSize,nOnlineSize);
			//const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(nGroupCode,1);
			//const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(nGroupCode,1);
			CString sText;
			sText.Format(_T("%s [%d/%d]"), pGroupInfo->m_sGroupName.c_str(),nOnlineSize,nMemberSize);
			m_treeDepartment.SetItemText(pDepItemInfo->m_hItem, sText);
		}
	}
	// 
}
#endif

//void CDlgMyGroup::DeleteDepartmentInfo(const EB_GroupInfo* pGroupInfo)
//{
//	CTreeItemInfo::pointer pDepItemInfo;
//	if (m_pDepItemInfo.find(pGroupInfo->m_sGroupCode, pDepItemInfo, true))
//	{
//		m_treeDepartment.DeleteItem(pDepItemInfo->m_hItem);
//	}
//}
//void CDlgMyGroup::ExitDepartment(eb::bigint sDepCode, eb::bigint nMemberUserId)
//{
//#ifdef USES_EBCOM_TEST
//	CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(sDepCode, nMemberUserId);
//	DeleteEmployeeInfo(pMemberInfo);
//#else
//	EB_MemberInfo pMemberInfo;
//	theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,sDepCode, nMemberUserId);
//	DeleteEmployeeInfo(&pMemberInfo);
//#endif
//}
#ifdef USES_EBCOM_TEST
void CDlgMyGroup::DeleteDepartmentInfo(IEB_GroupInfo* pGroupInfo)
{
	eb::bigint nGroupCode = pGroupInfo->GroupCode;
	_variant_t pOutMemberInfoList = theEBClientCore->EB_GetGroupMemberCodeList(nGroupCode);
	if (pOutMemberInfoList.vt!=VT_EMPTY && pOutMemberInfoList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pOutMemberInfoList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			const eb::bigint sMemberCode = var.ullVal;
			DeleteEmployeeInfo(pGroupInfo,sMemberCode);

			//CComPtr<IEB_MemberInfo> pInterface;
			//var.pdispVal->QueryInterface(__uuidof(IEB_MemberInfo),(void**)&pInterface);
			////const eb::bigint sGroupCode = pInterface->GroupCode;
			//const eb::bigint sMemberCode = pInterface->MemberCode;
			//DeleteEmployeeInfo(pGroupInfo,sMemberCode);
		}
	}
	CTreeItemInfo::pointer pDepItemInfo;
	if (m_pDepItemInfo.find(nGroupCode, pDepItemInfo, true))
	{
		m_treeDepartment.DeleteItem(pDepItemInfo->m_hItem);
	}
}
#else
void CDlgMyGroup::DeleteDepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	//EB_GroupInfo pGroupInfo;
	//if (theEBAppClient.EB_GetGroupInfo(sDepCode,&pGroupInfo))
	{
		std::vector<eb::bigint> pOutMemberCodeList;
		if (theEBAppClient.EB_GetGroupMemberCodeList(pGroupInfo->m_sGroupCode,pOutMemberCodeList))
		{
			for (size_t i=0;i<pOutMemberCodeList.size(); i++)
			{
				eb::bigint nMemberCode = pOutMemberCodeList[i];
				DeleteEmployeeInfo(pGroupInfo,nMemberCode);
			}
		}
	}

	CTreeItemInfo::pointer pDepItemInfo;
	if (m_pDepItemInfo.find(pGroupInfo->m_sGroupCode, pDepItemInfo, true))
	{
		m_treeDepartment.DeleteItem(pDepItemInfo->m_hItem);
	}
}
#endif

CTreeItemInfo::pointer CDlgMyGroup::GetDepItemInfo(HTREEITEM hItem) const
{
	if (hItem == NULL) return NullTreeItemInfo;
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_pDepItemInfo.mutex()));
	//boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(m_pDepItemInfo.mutex()));
	CLockMap<eb::bigint, CTreeItemInfo::pointer>::const_iterator pIter = m_pDepItemInfo.begin();
	for (; pIter!=m_pDepItemInfo.end(); pIter++)
	{
		CTreeItemInfo::pointer pTreeItemInfo = pIter->second;
		if (pTreeItemInfo->m_hItem == hItem)
			return pTreeItemInfo;
	}
	return NullTreeItemInfo;
}

void CDlgMyGroup::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#else
		theEBAppClient.EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#endif
	}else if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		theApp.m_pAutoCallGroupIdList.remove(pTreeItemInfo->m_sGroupCode);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallGroup(pTreeItemInfo->m_sGroupCode);
#else
		theEBAppClient.EB_CallGroup(pTreeItemInfo->m_sGroupCode);
#endif
	}
}
void CDlgMyGroup::EditEmployeeInfo(HTREEITEM hSelItem)
{
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	//if (pTreeItemInfo->m_nUserId != theApp.GetLogonUserId()) return;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
	if (pMemberInfo != NULL)
		theApp.EditEmployeeInfo(this,pMemberInfo);
#else
	EB_MemberInfo pMemberInfo;
	if (theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
		theApp.EditEmployeeInfo(this,&pMemberInfo);
#endif
}

void CDlgMyGroup::OnCallUser()
{
	CallItem(m_treeDepartment.GetSelectedItem());
}
void CDlgMyGroup::OnViewMsgRecord()
{
	const HTREEITEM hItem = m_treeDepartment.GetSelectedItem();
	if (hItem==NULL) return;
	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		bIsAccount = true;
		sId = pTreeItemInfo->m_nUserId;
		sName = pTreeItemInfo->m_sName;
	}else if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		sId = pTreeItemInfo->m_sGroupCode;
		sName = pTreeItemInfo->m_sName;
	}else
	{
		return;
	}
	CDlgMsgRecord * pDlgMsgRecord = NULL;
	if (!theApp.m_pMsgRecord.find(sId,pDlgMsgRecord))
	{
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		pDlgMsgRecord = new CDlgMsgRecord(pParent);
		pDlgMsgRecord->Create(CDlgMsgRecord::IDD,pParent);
		theApp.m_pMsgRecord.insert(sId,pDlgMsgRecord);
	}
	CRect rect;
	this->GetParent()->GetWindowRect(&rect);
	const int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度 
	rect.left = (m_nScreenWidth-520)/2;	// rect.right;
	rect.right = rect.left + 520;
	pDlgMsgRecord->MoveWindow(&rect);
	pDlgMsgRecord->SetCircle();
	if (bIsAccount)
		pDlgMsgRecord->LoadAccountMsgRecord(sId,sName);
	else
		pDlgMsgRecord->LoadDepartmentMsgRecord(sId,sName);
}
void CDlgMyGroup::OnDeleteMsgRecord()
{
	const HTREEITEM hItem = m_treeDepartment.GetSelectedItem();
	if (hItem==NULL) return;
	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		bIsAccount = true;
		sId = pTreeItemInfo->m_nUserId;
		sName = pTreeItemInfo->m_sName;
	}else if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		sId = pTreeItemInfo->m_sGroupCode;
		sName = pTreeItemInfo->m_sName;
	}else
	{
		return;
	}

	CString sText;
	sText.Format(_T("确定清空：\r\n%s(%lld) 本地聊天记录吗？"), sName.c_str(),sId);
	if (CDlgMessageBox::EbDoModal(this,"清空聊天记录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("清空聊天记录"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		theApp.DeleteDbRecord(sId,bIsAccount);
	}
}

//
//int CDlgMyGroup::GetItemType(HTREEITEM hItem) const
//{
//	// 0: NULL
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
//	if (hSelItem == NULL || pTreeItemInfo==NULL)
//	{
//		return 0;
//	}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
//	{
//	}
//}

void CDlgMyGroup::OnNMRClickTreeDepartment(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	// ?? 群组云盘功能
	EB_FUNC_LOCATION nFuncLocation = EB_FUNC_LOCATION_UNKNOWN;
	theApp.ClearSubscribeSelectInfo();
	CNewMenu m_menu2;
	m_menu2.CreatePopupMenu();
	m_menu2.SetBitmapBackground(RGB(192,192,192));
	const HTREEITEM hSelItem = m_treeDepartment.SelectHitTest();
	//const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	const CTreeItemInfo * pTreeItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_DEPARTMENT3,_T("新建个人群组"));
	if (hSelItem == NULL || pTreeItemInfo==NULL)
	{
	}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		// emp
		nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER;
		theApp.m_nSelectUserId = pTreeItemInfo->m_nUserId;
		if (pTreeItemInfo->m_nUserId == theApp.GetLogonUserId())
		{
			// 选择自己
			m_menu2.AppendMenu(MF_SEPARATOR);
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改我的名片"));
			//m_menu2.InsertODMenu(-1,_T("修改我的名片"),MF_BYPOSITION,EB_COMMAND_DEPARTMENT_EDIT_EMP,IDB_BITMAP_MENU_EDIT);
		}else
		{
			// 选择别人
			m_menu2.AppendMenu(MF_SEPARATOR);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CALL_USER,_T("打开会话"));
			m_menu2.InsertODMenu(-1,_T("打开会话(&C)"),MF_BYPOSITION,EB_COMMAND_CALL_USER,IDB_BITMAP_MENU_CALL);
			// 判断聊天记录
			CString sSql;
			sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) LIMIT 1"),
						   pTreeItemInfo->m_nUserId,pTreeItemInfo->m_nUserId);
			if (theApp.m_pBoUsers->select(sSql)>0)
			{
				m_menu2.InsertODMenu(-1,_T("聊天记录"),MF_BYPOSITION,EB_MSG_VIEW_MSG_RECORD,IDB_BITMAP_MENU_MSG);
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_MSG_RECORD,_T("清空聊天记录"));
			}
			//if ((theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) ||
			//	(theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)))
			//{
			//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改成员名片"));
			//}
		}
		// 我是群主，选择对象不是群主
		if (theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
		{
			m_menu2.AppendMenu(MF_SEPARATOR);
			if (theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_DEL_ADMIN,_T("撤销管理员"));
			else
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_ADMIN,_T("加为管理员"));
		}

		HTREEITEM hSelParentItem = m_treeDepartment.GetParentItem(hSelItem);
		const CTreeItemInfo::pointer pDepItemInfo = GetDepItemInfo(hSelParentItem);
		if (pDepItemInfo.get() == NULL)
			return;

		if ((theEBAppClient.EB_IsGroupCreator(pDepItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupCreator(pDepItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) ||
			(theEBAppClient.EB_IsGroupAdminLevel(pDepItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupAdminLevel(pDepItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)))
		{
			// 选择用户不是群管理员
			m_menu2.AppendMenu(MF_SEPARATOR);
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改成员名片"));
			//m_menu2.InsertODMenu(-1,_T("修改成员名片"),MF_BYPOSITION,EB_COMMAND_DEPARTMENT_EDIT_EMP,IDB_BITMAP_MENU_EDIT);
			m_menu2.AppendODMenu(_T("移除成员(&D)"),MF_BYPOSITION,EB_COMMAND_DEPARTMENT_DEL_EMP,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_DEL_EMP,_T("移除成员"));
		}else
		{
			// 我不是群管理员，普通成员
			//m_menu2.AppendMenu(nPosIndex++,MF_BYCOMMAND,EB_COMMAND_EXIT_DEPARTMENT,_T("退出该群"));
		}
//
//#ifdef USES_EBCOM_TEST
//		const eb::bigint nGroupCreateUserId = theEBClientCore->EB_GetGroupCreator(pDepItemInfo->m_sGroupCode);
//		if (nGroupCreateUserId==0) return;
//#else
//		eb::bigint nGroupCreateUserId = 0;
//		if (!theEBAppClient.EB_GetGroupCreator(pDepItemInfo->m_sGroupCode,nGroupCreateUserId)) return;
//#endif
//		if (theEBAppClient.EB_IsGroupCreator(pDepItemInfo->m_sGroupCode) ||
//			(theEBAppClient.EB_IsGroupAdminLevel(pDepItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupAdminLevel(pDepItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)))
//		//if (nGroupCreateUserId == theApp.GetLogonUserId())
//		{
//			// 我是该群管理员
//			//m_menu2.AppendMenu(nPosIndex++,MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,_T("修改群资料"));
//			//m_menu2.AppendMenu(nPosIndex++,MF_BYCOMMAND,EB_COMMAND_DELETE_DEPARTMENT,_T("解散该群"));
//			//m_menu2.AppendMenu(nPosIndex++,MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_ADD_EMP,_T("邀请成员"));
//			if (pTreeItemInfo->m_nUserId != nGroupCreateUserId)
//			{
//				// 选择用户不是群管理员
//				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_DEL_EMP,_T("移除成员"));
//			}
//		}else
//		{
//			// 我不是群管理员，普通成员
//			//m_menu2.AppendMenu(nPosIndex++,MF_BYCOMMAND,EB_COMMAND_EXIT_DEPARTMENT,_T("退出该群"));
//		}
	}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		// dep
#ifdef USES_EBCOM_TEST
		//const eb::bigint nGroupCreateUserId = theEBClientCore->EB_GetGroupCreator(pTreeItemInfo->m_sGroupCode);
		//if (nGroupCreateUserId==0) return;
		const short nGroupType = theEBClientCore->EB_GetGroupType(pTreeItemInfo->m_sGroupCode);
#else
		//eb::bigint nGroupCreateUserId = 0;
		//if (!theEBAppClient.EB_GetGroupCreator(pTreeItemInfo->m_sGroupCode,nGroupCreateUserId)) return;
		EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
		if (!theEBAppClient.EB_GetGroupType(pTreeItemInfo->m_sGroupCode,nGroupType)) return;
#endif
		nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_GROUP;
		theApp.m_nSelectGroupId = pTreeItemInfo->m_sGroupCode;
		m_menu2.AppendMenu(MF_SEPARATOR);
		CString sText;
		//sText.Format(_T("%s共享"),GetGroupTypeText((EB_GROUP_TYPE)nGroupType));
		//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_VIEW_GROUP_SHARE,sText);
		m_menu2.InsertODMenu(-1,_T("打开会话(&C)"),MF_BYPOSITION,EB_COMMAND_CALL_USER,IDB_BITMAP_MENU_CALL);
		//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_VIEW_GROUP_SHARE,_T("群共享"));IDB_BITMAP_MENU_SHARE
		CString sSql;
		sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=%lld LIMIT 1"),pTreeItemInfo->m_sGroupCode);
		if (theApp.m_pBoUsers->select(sSql)>0)
		{
			m_menu2.InsertODMenu(-1,_T("聊天记录"),MF_BYPOSITION,EB_MSG_VIEW_MSG_RECORD,IDB_BITMAP_MENU_MSG);
		}
		m_menu2.InsertODMenu(-1,_T("群共享(&S)"),MF_BYPOSITION,EB_COMMAND_VIEW_GROUP_SHARE,IDB_BITMAP_MENU_SHARE);
		bool bNeedSeparator = true;
		if (theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode))
		{
			m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = false;
			// 我是群管理员
			sText.Format(_T("修改%s资料"),GetGroupTypeText((EB_GROUP_TYPE)nGroupType));
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,sText);
			if (theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode) &&
				nGroupType != EB_GROUP_TYPE_DEPARTMENT &&	// 公司部门
				nGroupType != EB_GROUP_TYPE_PROJECT)			// 公司项目组
			{
				sText.Format(_T("解散该%s(&D)"),GetGroupTypeText((EB_GROUP_TYPE)nGroupType));
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_DEPARTMENT,sText);
			}
		}
		if (!theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode) &&
			nGroupType != EB_GROUP_TYPE_DEPARTMENT &&	// 公司部门
			nGroupType != EB_GROUP_TYPE_PROJECT)			// 公司项目组
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			sText.Format(_T("退出该%s"),GetGroupTypeText((EB_GROUP_TYPE)nGroupType));
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EXIT_DEPARTMENT,sText);
		}

		//if (nGroupCreateUserId == theApp.GetLogonUserId())
		//{
		//	// 我是群管理员
		//	if (nGroupType == EB_GROUP_TYPE_DEPARTMENT || nGroupType == EB_GROUP_TYPE_PROJECT)
		//		m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,_T("修改部门资料"));
		//	else
		//		m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,_T("修改群资料"));
		//	if (nGroupType != EB_GROUP_TYPE_DEPARTMENT &&	// 公司部门
		//		nGroupType != EB_GROUP_TYPE_PROJECT)			// 公司项目组
		//	{
		//		m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_DEPARTMENT,_T("解散该群"));
		//	}
		//}else if (nGroupType == EB_GROUP_TYPE_DEPARTMENT ||	// 公司部门
		//	nGroupType == EB_GROUP_TYPE_PROJECT)			// 公司项目组
		//{
		//	// 公司部门或项目组（不能退出）
		//	// ...
		//}else
		//{
		//	// 普通群组成员
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EXIT_DEPARTMENT,_T("退出该群"));
		//}
	}

	// 应用功能菜单
	CNewMenu pPopupMenuApps;
	if (nFuncLocation != EB_FUNC_LOCATION_UNKNOWN)
	//if (theApp.m_nSelectUserId > 0 || theApp.m_nSelectGroupId > 0)
	{
		pPopupMenuApps.CreatePopupMenu();
#ifdef USES_EBCOM_TEST
		_variant_t pSubscribeFuncList = theEBClientCore->EB_GetSubscribeFuncList(nFuncLocation);
		if (pSubscribeFuncList.vt!=VT_EMPTY && pSubscribeFuncList.parray != NULL)
		{
			CComSafeArray<VARIANT> m_sa(pSubscribeFuncList.parray);
			for (ULONG i=0;i<m_sa.GetCount();i++)
			{
				CComVariant var = m_sa.GetAt(i);
				if (var.vt != VT_DISPATCH)
					continue;
				CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo;
				var.pdispVal->QueryInterface(__uuidof(IEB_SubscribeFuncInfo),(void**)&pSubscribeFuncInfo);
				if (pSubscribeFuncInfo == NULL) continue;
				theApp.m_pSubscribeFuncList.push_back(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
			}
		}
#else
		theEBAppClient.EB_GetSubscribeFuncList(nFuncLocation,theApp.m_pSubscribeFuncList);
#endif
		if (!theApp.m_pSubscribeFuncList.empty())
		{
			for (size_t i=0;i<theApp.m_pSubscribeFuncList.size();i++)
			{
				const EB_SubscribeFuncInfo & pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[i];
				bool bAppendMenuError = true;
				if (::PathFileExists(pSubscribeFuncInfo.m_sResFile.c_str()))
				{
					CImageEx pImageEx;
					pImageEx.LoadFilename(pSubscribeFuncInfo.m_sResFile.c_str());
					const int nWidth = pImageEx.GetWidth();
					const int nHeight = pImageEx.GetHeight();
					if (nWidth>0 && nHeight>0)
					{
						HGDIOBJ hBitmap = NULL;
						if ((nWidth/nHeight)>2)
						{
							if (nHeight>const_default_menu_image_size)
								hBitmap = pImageEx.Copy(0,0,const_default_menu_image_size,const_default_menu_image_size);
							else
								hBitmap = pImageEx.Copy(0,0,nHeight,nHeight);
						}else
						{
							if (nHeight!=const_default_menu_image_size)
								hBitmap = pImageEx.StrctchCopy(0,0,const_default_menu_image_size,const_default_menu_image_size);
							else
								hBitmap = pImageEx.Copy(0,0,const_default_menu_image_size,const_default_menu_image_size);
						}
						if (hBitmap!=NULL)
						{
							CBitmap pBitmap;
							pBitmap.Attach(hBitmap);
							pPopupMenuApps.AppendODMenu(pSubscribeFuncInfo.m_sFunctionName.c_str(),MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,&pBitmap);
							bAppendMenuError = false;
						}
					}
				}
				if (bAppendMenuError)
					pPopupMenuApps.AppendMenu(MF_BYCOMMAND,EB_COMMAND_SUBSCRIBE_FUNC+i,pSubscribeFuncInfo.m_sFunctionName.c_str());
			}
			m_menu2.AppendMenu(MF_SEPARATOR);
			m_menu2.InsertODMenu(-1,_T("集成应用"),MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuApps.m_hMenu,IDB_BITMAP_SELECT_COLOR);
		}
	}

	CPoint point;
	GetCursorPos(&point);
	CPoint point2(point);
	this->ScreenToClient(&point2);
	if (point2.y>180)
		m_menu2.TrackPopupMenu(TPM_LEFTBUTTON|TPM_BOTTOMALIGN,point.x,point.y,this);
	else
		m_menu2.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
}
void CDlgMyGroup::OnSubscribeFunc(UINT nID)
{
	//theEBAppClient.EB_JoinGroup(999121,"恩布网络申请加入！！！");
	//return;
	size_t nIndex = nID-EB_COMMAND_SUBSCRIBE_FUNC;
	if (nIndex>=0 && nIndex<theApp.m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[nIndex];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
	}
}

//bool CDlgMyGroup::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return false;
//	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
//		return false;
//	else
//	{
//		CEBString sImagePath;
//		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
//#ifdef USES_EBCOM_TEST
//		CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pTreeItemInfo->m_sMemberCode);
//		if (pMemberInfo != NULL)
//		{
//			pOutLineState = pMemberInfo->LineState;
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
//}
//bool CDlgMyGroup::GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,HICON &pIcon1, HICON &pIcon2,int& pState) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return NULL;
//	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
//		return false;
//	else
//	{
//		CEBString sImagePath;
//		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
//#ifdef USES_EBCOM_TEST
//		CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pTreeItemInfo->m_sMemberCode);
//		if (pMemberInfo != NULL)
//		{
//			pOutLineState = pMemberInfo->LineState;
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
//			pOutLineState = pMemberInfo.m_nLineState;
//			//sDescription = pMemberInfo.m_sDescription;
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
//			pState = 1;
//			break;
//		case EB_LINE_STATE_BUSY:
//			{
//				pState = 1;
//				pIcon2 = theApp.GetIconBusy();
//			}break;
//		case EB_LINE_STATE_AWAY:
//			{
//				pState = 1;
//				pIcon2 = theApp.GetIconAway();
//			}break;
//		default:
//			break;
//		}
//		if (!sImagePath.empty())
//		{
//			USES_CONVERSION;
//			Gdiplus::Bitmap pImage((const WCHAR*)A2W_ACP(sImagePath.c_str()));
//			pImage.GetHICON(&pIcon1);
//			return true;
//		}else
//		{
//			return false;
//		}
//		//return theApp.GetIconCon();
//	}
//}
//int CDlgMyGroup::GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, HICON& pOutIcon) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return 1;
//#ifdef USES_EBCOM_TEST
//	EB_USER_LINE_STATE pOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_GetMemberLineState(pTreeItemInfo->m_sMemberCode);
//	if (pOutLineState == EB_LINE_STATE_UNKNOWN)
//		return 1;
//#else
//	EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
//	if (!theEBAppClient.EB_GetMemberLineState(pTreeItemInfo->m_sMemberCode,pOutLineState))
//		return 1;
//#endif
//	switch (pOutLineState)
//	{
//	case EB_LINE_STATE_UNKNOWN:
//	case EB_LINE_STATE_OFFLINE:
//		return 0;
//	case EB_LINE_STATE_ONLINE_NEW:
//		return 1;
//	case EB_LINE_STATE_BUSY:
//		pOutIcon = theApp.GetIconBusy();
//		return 1;
//	case EB_LINE_STATE_AWAY:
//		pOutIcon = theApp.GetIconAway();
//		return 1;
//		break;
//	default:
//		break;
//	}
//	return 0;
//}
//bool CDlgMyGroup::GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hItem);
//	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP) return true;
//	return false;
//}

LRESULT CDlgMyGroup::OnTreeItemSelChanged(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hSelItem = (HTREEITEM)wp;
	if (hSelItem==NULL) return 0;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)// && pTreeItemInfo->m_dwItemData==0)
	{
#ifdef USES_EBCOM_TEST
		//const int nMemberSize = theEBClientCore->EB_GetGroupMemberSize(pTreeItemInfo->m_sGroupCode);
		//if (nMemberSize>0)
			theEBClientCore->EB_LoadGroup(pTreeItemInfo->m_sGroupCode,VARIANT_TRUE);
#else
		//const int nMemberSize = theEBAppClient.EB_GetGroupMemberSize(pTreeItemInfo->m_sGroupCode);
		//if (nMemberSize>0)
			theEBAppClient.EB_LoadGroup(pTreeItemInfo->m_sGroupCode,true);
#endif
	}
	return 0;
}
LRESULT CDlgMyGroup::OnTreeItemDblclk(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hDblClkItem = (HTREEITEM)wp;
	if (hDblClkItem==NULL) return 0;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hDblClkItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER && pTreeItemInfo->m_nUserId==theApp.GetLogonUserId())
	{
		OnDepartmentEditEmp();
	}else
	{
		this->CallItem(hDblClkItem);
	}
	return 0;
}

LRESULT CDlgMyGroup::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	m_hCurrentHotItem = item;
	if (item == NULL || !this->IsWindowVisible())
	{
		if (m_pViewContactInfo!=0)// && m_pViewContactInfo->IsWindowVisible())
		{
			m_pViewContactInfo->SetMoveLeave();
			//m_pViewContactInfo->HideReset();
		}
		m_btnCallTrack.ShowWindow(SW_HIDE);
		m_btnEditEmpTrack.ShowWindow(SW_HIDE);
	}else if (m_btnCallTrack.GetSafeHwnd() != NULL)
	{
		const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(item);
		if (pTreeItemInfo == NULL)
		{
			if (m_pViewContactInfo!=0)// && m_pViewContactInfo->IsWindowVisible())
			{
				m_pViewContactInfo->SetMoveLeave();
				//m_pViewContactInfo->HideReset();
			}
			return 1;
		}
		if (m_pViewContactInfo == NULL)
		{
			m_pViewContactInfo = new CDlgViewContactInfo(this);
			m_pViewContactInfo->Create(CDlgViewContactInfo::IDD,this);
		}
		CPoint pos;
		GetCursorPos(&pos);

		SetTimer(TIMERID_CHECK_ITEM_HOT,1000,NULL);
		CRect rectItem;
		m_treeDepartment.GetItemRect(item, &rectItem, TRUE);
		CRect rectHead(rectItem);
		rectHead.right = rectHead.left + m_treeDepartment.GetIconWidth();
		this->ClientToScreen(rectHead);
		if (rectHead.PtInRect(pos))
		{
			const int const_dlg_width = 380;
			const int const_dlg_height = 220;
			CRect rect;
			this->GetWindowRect(&rect);
			CRect rectViewContactInfo;
			rectViewContactInfo.top = pos.y-80;
			rectViewContactInfo.bottom = rectViewContactInfo.top+const_dlg_height;
			if (rect.left-const_dlg_width > 0)
			{
				rectViewContactInfo.right = rect.left-5;
				rectViewContactInfo.left = rectViewContactInfo.right-const_dlg_width;
			}else
			{
				int nScreenWidth = 0;
				int nScreenHeight = 0;
				theApp.GetScreenSize(nScreenWidth, nScreenHeight);
				if (rect.right+const_dlg_width<nScreenWidth)
					rectViewContactInfo.left = min((nScreenWidth-const_dlg_width),(rect.right+5));
				else
					rectViewContactInfo.left = pos.x+30;
				rectViewContactInfo.right = rectViewContactInfo.left+const_dlg_width;
			}
			m_pViewContactInfo->MoveWindow(&rectViewContactInfo);
			m_pViewContactInfo->SetCircle();

#ifdef USES_EBCOM_TEST
			CComPtr<IEB_MemberInfo> pMemberInfo;
			CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode);
			if (pGroupInfo == NULL) return 1;
			if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
			{
				CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo;
				if (pTreeItemInfo->m_sEnterpriseCode>0)
					pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(pTreeItemInfo->m_sEnterpriseCode);
				m_pViewContactInfo->SetDepartemntInfo(pGroupInfo,pEnterpriseInfo);
			}else
			{
				pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
				if (pMemberInfo == NULL) return 1;
				m_pViewContactInfo->SetEmployeeInfo(pMemberInfo,pGroupInfo);
			}
#else
			EB_MemberInfo pMemberInfo;
			EB_GroupInfo pGroupInfo;
			if (!theEBAppClient.EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode,&pGroupInfo)) return 1;
			if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
			{
				EB_EnterpriseInfo pEnterpriseInfo;
				if (pTreeItemInfo->m_sEnterpriseCode>0)
					theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo,pTreeItemInfo->m_sEnterpriseCode);
				m_pViewContactInfo->SetDepartemntInfo(&pGroupInfo,&pEnterpriseInfo);
			}else
			{
				if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) return 1;
				m_pViewContactInfo->SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
			}
#endif
			m_pViewContactInfo->SetMoveEnter();
			//m_pViewContactInfo->ShowWindow(SW_SHOW);
			//m_pViewContactInfo->SetCheckLeave();
		}else// if (m_pViewContactInfo->IsWindowVisible())
		{
			m_pViewContactInfo->SetMoveLeave();
			//m_pViewContactInfo->HideReset();
		}

		const int nTop = rectItem.top;
		m_treeDepartment.GetClientRect(&rectItem);
		const int nRight = rectItem.right;
		//rectItem.right = m_treeDepartment.GetHSize();
		const int const_btn_width = m_btnEditEmpTrack.GetImgWidth();
		if (pTreeItemInfo->m_nUserId == theApp.GetLogonUserId())
		{
			m_btnCallTrack.ShowWindow(SW_HIDE);
			m_btnEditEmpTrack.MovePoint(nRight-const_btn_width, nTop);
			m_btnEditEmpTrack.ShowWindow(SW_SHOW);
			m_btnEditEmpTrack.Invalidate();
		}else
		{
			m_btnEditEmpTrack.ShowWindow(SW_HIDE);
			m_btnCallTrack.MovePoint(nRight-const_btn_width, nTop);
			m_btnCallTrack.ShowWindow(SW_SHOW);
			m_btnCallTrack.Invalidate();
		}
		//this->SetFocus();
	}
	return 0;
}

BOOL CDlgMyGroup::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (GetKeyState(VK_CONTROL)&0x80)
		{
			if (pMsg->wParam==VK_UP)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
				return TRUE;
			}else if (pMsg->wParam==VK_DOWN)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
				return TRUE;
			}else if (pMsg->wParam>=VK_NUMPAD0 && pMsg->wParam<=VK_NUMPAD9)
			{
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_OFFSET, (WPARAM)(pMsg->wParam-VK_NUMPAD0), 0);
				return TRUE;
			}
		}
	}

	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnCallTrack.GetSafeHwnd())
	{
		CallItem(m_treeDepartment.GetTrackItem());
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeDepartment.GetSafeHwnd())
	{
		CallItem(m_treeDepartment.GetSelectedItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnEditEmpTrack.GetSafeHwnd())
	{
		SetTimer(TIMERID_EDIT_MEMBERINFO, 1, NULL);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgMyGroup::OnGroupShare()
{
	const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_GROUP) return;
	theApp.OpenGroupShareWindow(pTreeItemInfo->m_sGroupCode,this->GetParent());
}
void CDlgMyGroup::OnDepartment3New()
{
	//EB_EnterpriseInfo::pointer pEnterpriseInfo = theEBAppClient.GetEnterpriseInfo();
	//const tstring sEntCode = pEnterpriseInfo.get()==NULL "" ? pEnterpriseInfo->m_sEnterpriseCode;
	const eb::bigint sEntCode = 0;
	CDlgGroupInfo pDlgGroupInfo(this);
	pDlgGroupInfo.m_nGroupType = EB_GROUP_TYPE_GROUP;
	if (pDlgGroupInfo.DoModal() == IDOK)
	{
		EB_GroupInfo pPopDepartment(sEntCode, 0);
		//pPopDepartment->m_sParentCode = sParentCode;
		pPopDepartment.m_nGroupType = pDlgGroupInfo.m_nGroupType;
		pPopDepartment.m_sGroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
		pPopDepartment.m_sPhone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
		pPopDepartment.m_sFax = (LPCTSTR)pDlgGroupInfo.m_sFax;
		pPopDepartment.m_sEmail = (LPCTSTR)pDlgGroupInfo.m_sEmail;
		pPopDepartment.m_sUrl = (LPCTSTR)pDlgGroupInfo.m_sUrl;
		pPopDepartment.m_sAddress = (LPCTSTR)pDlgGroupInfo.m_sAddress;
		pPopDepartment.m_sDescription = (LPCTSTR)pDlgGroupInfo.m_sDescription;
		pPopDepartment.m_nCreateUserId = theApp.GetLogonUserId();
		pPopDepartment.m_nManagerUserId = pDlgGroupInfo.m_nManagerUserId;
#ifdef USES_EBCOM_TEST
		CIEB_GroupInfo* pEBGroupInfo = new CIEB_GroupInfo(pPopDepartment);
		CComPtr<IEB_GroupInfo> pInterface;
		pEBGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface);
		theEBClientCore->EB_EditGroup(pInterface);
#else
		theEBAppClient.EB_EditGroup(&pPopDepartment);
#endif
	}
}

void CDlgMyGroup::OnDepartmentDelete()
{
	//EB_EnterpriseInfo::pointer pEnterpriseInfo = theEBAppClient.GetEnterpriseInfo();
	//if (pEnterpriseInfo.get() == NULL)
	//	return;

	const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	if (hSelItem == NULL || m_treeDepartment.GetParentItem(hSelItem) != NULL)	// != NULL表示不是部门
		return;
	const CTreeItemInfo::pointer pDepItemInfo = GetDepItemInfo(hSelItem);
	if (pDepItemInfo.get()==NULL) return;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pDepItemInfo->m_sGroupCode);
	if (pGroupInfo == NULL) return;
	const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
	const eb::bigint nGroupCreateUserId = pGroupInfo->CreateUserId;
	if (nGroupCreateUserId != theApp.GetLogonUserId())
	{
		// 不是部门创建者，不能删除
		return;
	}else if (pGroupInfo->GroupType == EB_GROUP_TYPE_DEPARTMENT				// 企业部门
		&& theEBClientCore->EB_GetGroupMemberSize(pDepItemInfo->m_sGroupCode,1)>0)				// 不为空，不能删除
	{
		return;
	}
	if (this->m_pViewContactInfo && m_pViewContactInfo->IsWindowVisible())
	{
		m_pViewContactInfo->HideReset();
	}

	CString sText;
	sText.Format(_T("[%s]吗？"), sGroupName.c_str());
	if (CDlgMessageBox::EbDoModal(this,"你确定解散群组：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("解散"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		theEBClientCore->EB_DeleteGroup(pDepItemInfo->m_sGroupCode);
	}
#else
	EB_GroupInfo pGroupInfo;
	if (!theEBAppClient.EB_GetGroupInfo(pDepItemInfo->m_sGroupCode,&pGroupInfo))
	{
		return;
	}
	if (pGroupInfo.m_nCreateUserId != theApp.GetLogonUserId())
	{
		// 不是部门创建者，不能删除
		return;
	}else if (pGroupInfo.m_nGroupType == EB_GROUP_TYPE_DEPARTMENT			// 企业部门
		&& theEBAppClient.EB_GetGroupMemberSize(pDepItemInfo->m_sGroupCode,1)>0)								// 不为空，不能删除
	{
		return;
	}
	if (this->m_pViewContactInfo && m_pViewContactInfo->IsWindowVisible())
	{
		m_pViewContactInfo->HideReset();
	}

	CString sText;
	sText.Format(_T("确定解散：\r\n%s 吗？"), pGroupInfo.m_sGroupName.c_str());
	if (CDlgMessageBox::EbDoModal(this,"解散群组",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("解散"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		theEBAppClient.EB_DeleteGroup(pDepItemInfo->m_sGroupCode);
	}
#endif
}

void CDlgMyGroup::OnDepartmentAddEmp()
{
	const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	theApp.NewEmployeeInfo(this,pTreeItemInfo);
}
//int CALLBACK CDlgMyGroup::TreeCmpFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
//{
//	const CTreeItemInfo * pItemInfo1 = (const CTreeItemInfo*)lParam1;
//	const CTreeItemInfo * pItemInfo2 = (const CTreeItemInfo*)lParam2;
//	if (pItemInfo1==NULL || pItemInfo2==NULL) return 1;
//	if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
//	{
//		// 群（部门）比较，用名称排序
//		return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
//	}else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
//	{
//		// 员工比较，先比较在线状态
//		if (pItemInfo1->m_dwItemData==pItemInfo2->m_dwItemData)
//		{
//			// 相同在线状态，比较名称
//			return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
//		}else if (pItemInfo1->m_dwItemData==EB_LINE_STATE_ONLINE_NEW)
//		{
//			// 第一个在线，不用换位置
//			return -1;
//		}else if (pItemInfo2->m_dwItemData==EB_LINE_STATE_ONLINE_NEW)
//		{
//			// 第二个在线，对调位置
//			return 1;
//		}else if (pItemInfo1->m_dwItemData==EB_LINE_STATE_OFFLINE)
//		{
//			// 第一个离线，对调位置
//			return 1;
//		}else if (pItemInfo2->m_dwItemData==EB_LINE_STATE_OFFLINE)
//		{
//			// 第二个离线，不用换位置
//			return -1;
//		}else if (pItemInfo1->m_dwItemData!=EB_LINE_STATE_OFFLINE)
//		{
//			// 第一个不是离线，不用换位置
//			return -1;
//		}else if (pItemInfo2->m_dwItemData!=EB_LINE_STATE_OFFLINE)
//		{
//			// 第二个不是离线，对调位置
//			return 1;
//		}
//		return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
//	}
//	return 0;
//}

void CDlgMyGroup::OnDepartmentExit()
{
	const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	if (hSelItem == NULL || m_treeDepartment.GetParentItem(hSelItem) != NULL)	// != NULL表示不是部门
		return;
	const CTreeItemInfo::pointer pDepItemInfo = GetDepItemInfo(hSelItem);
	if (pDepItemInfo.get()==NULL) return;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pDepItemInfo->m_sGroupCode);
	if (pGroupInfo == NULL) return;
	const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
	const eb::bigint nGroupCreateUserId = pGroupInfo->CreateUserId;
	if (nGroupCreateUserId == theApp.GetLogonUserId())
	{
		// 部门创建者，不能退出
		return;
	}
	CString sText;
	sText.Format(_T("[%s]吗？"), sGroupName.c_str());
	if (CDlgMessageBox::EbDoModal(this,"你确定退出群组：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("退出"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		theEBClientCore->EB_ExitGroup(pDepItemInfo->m_sGroupCode);
	}
#else
	EB_GroupInfo pGroupInfo;
	if (!theEBAppClient.EB_GetGroupInfo(pDepItemInfo->m_sGroupCode,&pGroupInfo))
	{
		return;
	}else if (pGroupInfo.m_nCreateUserId == theEBAppClient.EB_GetLogonUserId())
	{
		// 部门创建者，不能退出
		return;
	}
	CString sText;
	sText.Format(_T("确定退出：\r\n%s 吗？"), pGroupInfo.m_sGroupName.c_str());
	if (CDlgMessageBox::EbDoModal(this,"退出群组",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("退出"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		theEBAppClient.EB_ExitGroup(pDepItemInfo->m_sGroupCode);
	}
#endif
}

void CDlgMyGroup::OnDepartmentDelEmp()
{
	HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	const CTreeItemInfo * pTreeItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER)
		return;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode);
	if (pGroupInfo == NULL) return;
	const CEBString sGroupName(pGroupInfo->GroupName.GetBSTR());
	const eb::bigint nGroupCreateUserId = pGroupInfo->CreateUserId;
	if (pTreeItemInfo->m_nUserId == nGroupCreateUserId)
	{
		// 不能移除管理者自己
		return;
	//}else if (nGroupCreateUserId!=theApp.GetLogonUserId())
	//{
	//	CDlgMessageBox::EbDoModal(this,"移除成员：","没有操作权限！",CDlgMessageBox::IMAGE_WARNING);
	//	return;
	}
	CString sText;
	sText.Format(_T("[%s(%s)]吗？"), pTreeItemInfo->m_sName.c_str(),pTreeItemInfo->m_sAccount.c_str());
	if (CDlgMessageBox::EbDoModal(this,"你确定移除成员：",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	{
		theEBClientCore->EB_DeleteMember(pTreeItemInfo->m_sMemberCode,VARIANT_TRUE);
	}
#else
	EB_GroupInfo pGroupInfo;
	if (!theEBAppClient.EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode,&pGroupInfo))
		return;
	if (pTreeItemInfo->m_nUserId == pGroupInfo.m_nCreateUserId)
	{
		// 不能移除管理者自己
		return;
	//}else if (pGroupInfo.m_nCreateUserId!=theApp.GetLogonUserId())
	//{
	//	CDlgMessageBox::EbDoModal(this,"移除成员：","没有操作权限！",CDlgMessageBox::IMAGE_WARNING);
	//	return;
	}

	CString sText;
	sText.Format(_T("确定移除：\r\n%s(%s) 吗？"), pTreeItemInfo->m_sName.c_str(),pTreeItemInfo->m_sAccount.c_str());
	if (CDlgMessageBox::EbDoModal(this,"移除成员",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	{
		theEBAppClient.EB_DeleteMember(pTreeItemInfo->m_sMemberCode);
	}
#endif
}
void CDlgMyGroup::OnDepartmentEditEmp()
{
	EditEmployeeInfo(m_treeDepartment.GetSelectedItem());
}

void CDlgMyGroup::OnMemberDelAdminLevel()
{
	const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_DelGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
}
void CDlgMyGroup::OnMemberAddAdminLevel()
{
	const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeDepartment.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_AddGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
}

void CDlgMyGroup::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_EDIT_MEMBERINFO == nIDEvent)
	{
		KillTimer(TIMERID_EDIT_MEMBERINFO);
		EditEmployeeInfo(m_treeDepartment.GetTrackItem());
	}else if (TIMERID_CHECK_ITEM_HOT==nIDEvent)
	{
		KillTimer(TIMERID_CHECK_ITEM_HOT);
		if (m_hCurrentHotItem!=NULL && m_hCurrentHotItem==m_treeDepartment.GetTrackItem())
		{
			this->PostMessage(WM_ITEM_TRACK_HOT,(WPARAM)m_treeDepartment.GetTrackItem(),(LPARAM)&m_treeDepartment);
		}
	}else if (TIMERID_CHECK_SELECT_FIRST==nIDEvent)
	{
		KillTimer(TIMERID_CHECK_SELECT_FIRST);
		if (m_treeDepartment.GetSelectedItem()==NULL && m_treeDepartment.GetFirstVisibleItem()!=NULL)
		{
			m_treeDepartment.SelectItem(m_treeDepartment.GetFirstVisibleItem());
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgMyGroup::OnDepartmentEdit()
{
	const HTREEITEM hSelItem = m_treeDepartment.GetSelectedItem();
	const CTreeItemInfo::pointer pDepItemInfo = GetDepItemInfo(hSelItem);
	if (pDepItemInfo.get()==NULL) return;
	theApp.EditGroupInfo(this,pDepItemInfo->m_sGroupCode);
//
//#ifdef USES_EBCOM_TEST
//	CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pDepItemInfo->m_sGroupCode);
//	if (pGroupInfo == NULL) return;
//	const eb::bigint nGroupCreateUserId = pGroupInfo->CreateUserId;
//	if (nGroupCreateUserId != theApp.GetLogonUserId())
//	{
//		// 不是部门创建者，不能删除
//		return;
//	}
//
//	CDlgGroupInfo pDlgGroupInfo(this);
//	pDlgGroupInfo.m_nGroupId = pGroupInfo->GroupCode;
//	pDlgGroupInfo.m_nGroupType = (EB_GROUP_TYPE)pGroupInfo->GroupType;
//	pDlgGroupInfo.m_sGroupName = pGroupInfo->GroupName.GetBSTR();
//	pDlgGroupInfo.m_sPhone = pGroupInfo->Phone.GetBSTR();
//	pDlgGroupInfo.m_sFax = pGroupInfo->Fax.GetBSTR();
//	pDlgGroupInfo.m_sEmail = pGroupInfo->Email.GetBSTR();
//	pDlgGroupInfo.m_sUrl = pGroupInfo->Url.GetBSTR();
//	pDlgGroupInfo.m_sAddress = pGroupInfo->Address.GetBSTR();
//	pDlgGroupInfo.m_sDescription = pGroupInfo->Description.GetBSTR();
//	if (pDlgGroupInfo.DoModal() == IDOK)
//	{
//		pGroupInfo->GroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
//		pGroupInfo->Phone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
//		pGroupInfo->Fax = (LPCTSTR)pDlgGroupInfo.m_sFax;
//		pGroupInfo->Email = (LPCTSTR)pDlgGroupInfo.m_sEmail;
//		pGroupInfo->Url = (LPCTSTR)pDlgGroupInfo.m_sUrl;
//		pGroupInfo->Address = (LPCTSTR)pDlgGroupInfo.m_sAddress;
//		pGroupInfo->Description = (LPCTSTR)pDlgGroupInfo.m_sDescription;
//		theEBClientCore->EB_EditGroup(pGroupInfo);
//	}
//#else
//	EB_GroupInfo pGroupInfo;
//	if (!theEBAppClient.EB_GetGroupInfo(pDepItemInfo->m_sGroupCode,&pGroupInfo))
//	{
//		return;
//	}else if (pGroupInfo.m_nCreateUserId != theEBAppClient.EB_GetLogonUserId())
//	{
//		// 不是部门创建者，不能删除
//		return;
//	}
//
//	CDlgGroupInfo pDlgGroupInfo(this);
//	pDlgGroupInfo.m_nGroupId = pGroupInfo.m_sGroupCode;
//	pDlgGroupInfo.m_nGroupType = pGroupInfo.m_nGroupType;
//	pDlgGroupInfo.m_sGroupName = pGroupInfo.m_sGroupName.c_str();
//	pDlgGroupInfo.m_sPhone = pGroupInfo.m_sPhone.c_str();
//	pDlgGroupInfo.m_sFax = pGroupInfo.m_sFax.c_str();
//	pDlgGroupInfo.m_sEmail = pGroupInfo.m_sEmail.c_str();
//	pDlgGroupInfo.m_sUrl = pGroupInfo.m_sUrl.c_str();
//	pDlgGroupInfo.m_sAddress = pGroupInfo.m_sAddress.c_str();
//	pDlgGroupInfo.m_sDescription = pGroupInfo.m_sDescription.c_str();
//	if (pDlgGroupInfo.DoModal() == IDOK)
//	{
//		EB_GroupInfo pEditPopDepartment(pGroupInfo.m_sEnterpriseCode, pGroupInfo.m_sGroupCode);
//		pEditPopDepartment.operator =(pGroupInfo);
//		pEditPopDepartment.m_sParentCode = pGroupInfo.m_sParentCode;
//		pEditPopDepartment.m_sGroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
//		pEditPopDepartment.m_sPhone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
//		pEditPopDepartment.m_sFax = (LPCTSTR)pDlgGroupInfo.m_sFax;
//		pEditPopDepartment.m_sEmail = (LPCTSTR)pDlgGroupInfo.m_sEmail;
//		pEditPopDepartment.m_sUrl = (LPCTSTR)pDlgGroupInfo.m_sUrl;
//		pEditPopDepartment.m_sAddress = (LPCTSTR)pDlgGroupInfo.m_sAddress;
//		pEditPopDepartment.m_sDescription = (LPCTSTR)pDlgGroupInfo.m_sDescription;
//		theEBAppClient.EB_EditGroup(&pEditPopDepartment);
//	}
//#endif
}

void CDlgMyGroup::OnDestroy()
{
	__super::OnDestroy();
	m_btnCallTrack.DestroyWindow();
	m_btnEditEmpTrack.DestroyWindow();
	if (m_pViewContactInfo)
	{
		m_pViewContactInfo->DestroyWindow();
		delete m_pViewContactInfo;
		m_pViewContactInfo = NULL;
	}
}


