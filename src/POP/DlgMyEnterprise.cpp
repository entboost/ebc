// DlgMyEnterprise.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgMyEnterprise.h"
#include "DlgEnterpriseInfo.h"
#include "DlgGroupInfo.h"
#include "DlgMemberInfo.h"
#include "DlgViewContactInfo.h"

#define TIMERID_EDIT_MEMBERINFO 100
#define TIMERID_CHECK_ITEM_HOT		202

// CDlgMyEnterprise dialog

IMPLEMENT_DYNAMIC(CDlgMyEnterprise, CDialog)

CDlgMyEnterprise::CDlgMyEnterprise(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMyEnterprise::IDD, pParent)
{
	m_pViewContactInfo = NULL;
	m_hCurrentHotItem = NULL;
}

CDlgMyEnterprise::~CDlgMyEnterprise()
{
}

void CDlgMyEnterprise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ENTERPRISE, m_treeEnterprise);
}


BEGIN_MESSAGE_MAP(CDlgMyEnterprise, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(EB_COMMAND_EDIT_ENTERPRISE, &CDlgMyEnterprise::OnEnterpriseEdit)
	ON_COMMAND(EB_COMMAND_IMPORT_ENT_DATA, &CDlgMyEnterprise::OnImportEntData)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ENTERPRISE, &CDlgMyEnterprise::OnNMRClickTreeEnterprise)
	ON_COMMAND(EB_COMMAND_NEW_DEPARTMENT1, &CDlgMyEnterprise::OnDepartment1New)
	ON_COMMAND(EB_COMMAND_NEW_DEPARTMENT2, &CDlgMyEnterprise::OnDepartment2New)
	ON_COMMAND(EB_COMMAND_DELETE_DEPARTMENT, &CDlgMyEnterprise::OnDepartmentDelete)
	ON_COMMAND(EB_COMMAND_EDIT_DEPARTMENT, &CDlgMyEnterprise::OnDepartmentEdit)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_REFRESH, &CDlgMyEnterprise::OnDepartmentRefresh)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_ADD_EMP, &CDlgMyEnterprise::OnEmployeeNew)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_DEL_EMP, &CDlgMyEnterprise::OnEmployeeDelete)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_EDIT_EMP, &CDlgMyEnterprise::OnEmployeeEdit)
	ON_COMMAND(EB_COMMAND_MEMBER_DEL_ADMIN, &CDlgMyEnterprise::OnMemberDelAdminLevel)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_ADMIN, &CDlgMyEnterprise::OnMemberAddAdminLevel)
	ON_COMMAND(EB_COMMAND_CALL_USER, &CDlgMyEnterprise::OnCallUser)
	ON_COMMAND(EB_MSG_VIEW_MSG_RECORD, OnViewMsgRecord)
	//ON_COMMAND(EB_COMMAND_DELETE_MSG_RECORD, OnDeleteMsgRecord)

	//ON_NOTIFY(NM_DBLCLK, IDC_TREE_ENTERPRISE, &CDlgMyEnterprise::OnNMDblclkTreeEnterprise)
	ON_MESSAGE(WM_ITEM_SEL_CHANGED, OnTreeItemSelChanged)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDblclk)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND(EB_COMMAND_VIEW_GROUP_SHARE, &CDlgMyEnterprise::OnGroupShare)
	ON_COMMAND_RANGE(EB_COMMAND_SUBSCRIBE_FUNC,EB_COMMAND_SUBSCRIBE_FUNC+0x20,OnSubscribeFunc)
END_MESSAGE_MAP()


// CDlgMyEnterprise message handlers
void CDlgMyEnterprise::SetCtrlColor(void)
{
	//if (theApp.GetColorFlat())
	//	m_treeEnterprise.SetBkGradients(theApp.GetFlatBgColor(),theApp.GetFlatBgColor());
	//else
	//	m_treeEnterprise.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());
}
BOOL CDlgMyEnterprise::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnCallTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeEnterprise, 0xffff);
	m_btnCallTrack.SetAutoSize(false);
	m_btnCallTrack.SetAutoFocus(true);
	m_btnCallTrack.Load(IDB_PNG_HOT_CALL);
	m_btnCallTrack.SetToolTipText(_T("打开会话"));
	m_btnEditEmpTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeEnterprise, 0xffff);
	m_btnEditEmpTrack.SetAutoSize(false);
	m_btnEditEmpTrack.SetAutoFocus(true);
	m_btnEditEmpTrack.Load(IDB_PNG_HOT_EDIT);
	m_btnEditEmpTrack.SetToolTipText(_T("修改我的名片"));

	//m_treeEnterprise.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeEnterprise.SetCallback((CTreeCallback*)&theApp);
	m_treeEnterprise.ModifyStyle(0, TVS_SINGLEEXPAND);
	//m_treeEnterprise.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeEnterprise.SetItemHeight(40);
	m_treeEnterprise.SetIconSize(32,32);
	//m_treeEnterprise.SetRootIcon( theApp.GetIconEnt() );
	//m_treeEnterprise.SetItemIcon( theApp.GetIconCon() );
	//m_treeEnterprise.SetParentIcon( theApp.GetIconDep() );

	SetCtrlColor();

	//SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMyEnterprise::OnDestroy()
{
	CDialog::OnDestroy();
	m_btnCallTrack.DestroyWindow();
	m_btnEditEmpTrack.DestroyWindow();
	if (m_pViewContactInfo!=0)
	{
		m_pViewContactInfo->DestroyWindow();
		delete m_pViewContactInfo;
		m_pViewContactInfo = NULL;
	}
}

void CDlgMyEnterprise::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CDlgMyEnterprise::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void CDlgMyEnterprise::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_treeEnterprise.GetSafeHwnd())
		m_treeEnterprise.MoveWindow(0, 0, cx, cy);
}

void CDlgMyEnterprise::OnEnterpriseEdit()
{
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(0);
	if (pEnterpriseInfo==NULL)
		return;
	CDlgEnterpriseInfo pDlgEnterpriseInfo(this);
	pDlgEnterpriseInfo.m_sEnterpriseName = pEnterpriseInfo->EnterpriseName.GetBSTR();
	pDlgEnterpriseInfo.m_sPhone = pEnterpriseInfo->Phone.GetBSTR();
	pDlgEnterpriseInfo.m_sFax = pEnterpriseInfo->Fax.GetBSTR();
	pDlgEnterpriseInfo.m_sEmail = pEnterpriseInfo->Email.GetBSTR();
	pDlgEnterpriseInfo.m_sUrl = pEnterpriseInfo->Url.GetBSTR();
	pDlgEnterpriseInfo.m_sAddress = pEnterpriseInfo->Address.GetBSTR();
	pDlgEnterpriseInfo.m_sDescription = pEnterpriseInfo->Description.GetBSTR();
	if (pDlgEnterpriseInfo.DoModal() == IDOK)
	{
		pEnterpriseInfo->Phone = (LPCTSTR)pDlgEnterpriseInfo.m_sPhone;
		pEnterpriseInfo->Fax = (LPCTSTR)pDlgEnterpriseInfo.m_sFax;
		pEnterpriseInfo->Email = (LPCTSTR)pDlgEnterpriseInfo.m_sEmail;
		pEnterpriseInfo->Url = (LPCTSTR)pDlgEnterpriseInfo.m_sUrl;
		pEnterpriseInfo->Address = (LPCTSTR)pDlgEnterpriseInfo.m_sAddress;
		pEnterpriseInfo->Description = (LPCTSTR)pDlgEnterpriseInfo.m_sDescription;
		pEnterpriseInfo->EntExt  = pEnterpriseInfo->EntExt;
		theEBClientCore->EB_EditEnterprise(pEnterpriseInfo);
	}
#else
	EB_EnterpriseInfo pEnterpriseInfo;
	if (!theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo))
		return;
	CDlgEnterpriseInfo pDlgEnterpriseInfo(this);
	pDlgEnterpriseInfo.m_sEnterpriseName = pEnterpriseInfo.m_sEnterpriseName.c_str();
	pDlgEnterpriseInfo.m_sPhone = pEnterpriseInfo.m_sPhone.c_str();
	pDlgEnterpriseInfo.m_sFax = pEnterpriseInfo.m_sFax.c_str();
	pDlgEnterpriseInfo.m_sEmail = pEnterpriseInfo.m_sEmail.c_str();
	pDlgEnterpriseInfo.m_sUrl = pEnterpriseInfo.m_sUrl.c_str();
	pDlgEnterpriseInfo.m_sAddress = pEnterpriseInfo.m_sAddress.c_str();
	pDlgEnterpriseInfo.m_sDescription = pEnterpriseInfo.m_sDescription.c_str();
	if (pDlgEnterpriseInfo.DoModal() == IDOK)
	{
		EB_EnterpriseInfo pEditEnterpriseInfo(pEnterpriseInfo.m_sEnterpriseCode, (LPCTSTR)pDlgEnterpriseInfo.m_sEnterpriseName);
		pEditEnterpriseInfo.m_nCreateUserId = pEnterpriseInfo.m_nCreateUserId;
		pEditEnterpriseInfo.m_sPhone = (LPCTSTR)pDlgEnterpriseInfo.m_sPhone;
		pEditEnterpriseInfo.m_sFax = (LPCTSTR)pDlgEnterpriseInfo.m_sFax;
		pEditEnterpriseInfo.m_sEmail = (LPCTSTR)pDlgEnterpriseInfo.m_sEmail;
		pEditEnterpriseInfo.m_sUrl = (LPCTSTR)pDlgEnterpriseInfo.m_sUrl;
		pEditEnterpriseInfo.m_sAddress = (LPCTSTR)pDlgEnterpriseInfo.m_sAddress;
		pEditEnterpriseInfo.m_sDescription = (LPCTSTR)pDlgEnterpriseInfo.m_sDescription;
		pEditEnterpriseInfo.m_sCreateTime = pEnterpriseInfo.m_sCreateTime;
		pEditEnterpriseInfo.m_nEntExt = pEnterpriseInfo.m_nEntExt;
		theEBAppClient.EB_EditEnterprise(&pEditEnterpriseInfo);
	}
#endif

}
void CDlgMyEnterprise::OnImportEntData()
{
	if (!theApp.GetEntManagerurl().empty())
	{
		const tstring sUrl(theApp.GetEntManagerurl());
		COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(sUrl.string(),"");
		this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
	}
}

//CTreeItemInfo::pointer CDlgMyEnterprise::GetDepItemInfo(HTREEITEM hItem) const
//{
//	if (hItem == NULL) return NullTreeItemInfo;
//	boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(m_pDepItemInfo.mutex()));
//	CLockMap<std::string, CTreeItemInfo::pointer>::const_iterator pIter = m_pDepItemInfo.begin();
//	for (; pIter!=m_pDepItemInfo.end(); pIter++)
//	{
//		CTreeItemInfo::pointer pTreeItemInfo = pIter->second;
//		if (pTreeItemInfo->m_hItem == hItem)
//			return pTreeItemInfo;
//	}
//	return NullTreeItemInfo;
//}

void CDlgMyEnterprise::SetGroupCount(HTREEITEM hGroupItem, eb::bigint sGroupCode)
{
	if (hGroupItem != NULL &&sGroupCode>0)
	{
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(sGroupCode);
		SetGroupCount(hGroupItem, pGroupInfo);
#else
		EB_GroupInfo pGroupInfo;
		if (theEBAppClient.EB_GetGroupInfo(sGroupCode,&pGroupInfo))
			SetGroupCount(hGroupItem, &pGroupInfo);
#endif
	}
}
#ifdef USES_EBCOM_TEST
void CDlgMyEnterprise::SetGroupCount(HTREEITEM hGroupItem, IEB_GroupInfo* pGroupInfo)
{
	if (hGroupItem != NULL && pGroupInfo != NULL)
	{
		const eb::bigint sEnterpriseCode = pGroupInfo->EnterpriseCode;
		CTreeItemInfo::pointer pEntItemInfo;
		if (m_pEntItemInfo.find(sEnterpriseCode,pEntItemInfo))
		{
			long nEnterpriseMemberSize = 0;
			long nEnterpriseOnlineSize = 0;
			theEBClientCore->EB_GetEnterpriseMemberSize(pGroupInfo->EnterpriseCode,&nEnterpriseMemberSize,&nEnterpriseOnlineSize);
			CString sText;
			sText.Format(_T("%s [%d/%d]"), pEntItemInfo->m_sName.c_str(),nEnterpriseOnlineSize,nEnterpriseMemberSize);
			m_treeEnterprise.SetItemText(pEntItemInfo->m_hItem, sText);
		}
		const int nMemberSize = pGroupInfo->EmpCount;//theEBClientCore->EB_GetGroupMemberSize(pGroupInfo->GroupCode);
		const int nOnlineSize = theEBClientCore->EB_GetGroupOnlineSize(pGroupInfo->GroupCode);
		CString sText;
		if (nOnlineSize>=0 && nMemberSize>0)
			sText.Format(_T("%s [%d/%d]"), CEBString(pGroupInfo->GroupName.GetBSTR()).c_str(),nOnlineSize,nMemberSize);
		else if (nMemberSize>0)
			sText.Format(_T("%s [%d]"), CEBString(pGroupInfo->GroupName.GetBSTR()).c_str(),nMemberSize);
		else
			sText.Format(_T("%s"), CEBString(pGroupInfo->GroupName.GetBSTR()).c_str());
		m_treeEnterprise.SetItemText(hGroupItem, sText);
	}
}
#else
void CDlgMyEnterprise::SetGroupCount(HTREEITEM hGroupItem, const EB_GroupInfo* pGroupInfo)
{
	if (hGroupItem != NULL && pGroupInfo != NULL)
	{
		CTreeItemInfo::pointer pEntItemInfo;
		if (m_pEntItemInfo.find(pGroupInfo->m_sEnterpriseCode,pEntItemInfo))
		{
			int nEnterpriseMemberSize = 0;
			int nEnterpriseOnlineSize = 0;
			theEBAppClient.EB_GetEnterpriseMemberSize(pGroupInfo->m_sEnterpriseCode,nEnterpriseMemberSize,nEnterpriseOnlineSize);
			CString sText;
			sText.Format(_T("%s [%d/%d]"), pEntItemInfo->m_sName.c_str(),nEnterpriseOnlineSize,nEnterpriseMemberSize);
			m_treeEnterprise.SetItemText(pEntItemInfo->m_hItem, sText);
		}
		const int nMemberSize = pGroupInfo->m_nEmpCount;//theEBAppClient.EB_GetGroupMemberSize(pGroupInfo->m_sGroupCode);
		const int nOnlineSize = theEBAppClient.EB_GetGroupOnlineSize(pGroupInfo->m_sGroupCode);
		CString sText;
		if (nOnlineSize>=0 && nMemberSize>0)
			sText.Format(_T("%s [%d/%d]"), pGroupInfo->m_sGroupName.c_str(),nOnlineSize,nMemberSize);
		else if (nMemberSize>0)
			sText.Format(_T("%s [%d]"), pGroupInfo->m_sGroupName.c_str(),nMemberSize);
		else
			sText.Format(_T("%s"), pGroupInfo->m_sGroupName.c_str());
		m_treeEnterprise.SetItemText(hGroupItem, sText);
	}
}
#endif

void CDlgMyEnterprise::OnDepartment1New()
{
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(0);
	if (pEnterpriseInfo==NULL)
		return;
	const eb::bigint sEnterpriseCode = pEnterpriseInfo->EnterpriseCode;
#else
	EB_EnterpriseInfo pEnterpriseInfo;
	if (!theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo))
		return;
	const eb::bigint sEnterpriseCode = pEnterpriseInfo.m_sEnterpriseCode;
#endif

	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
	const eb::bigint sParentCode = pTreeItemInfo->m_sGroupCode;
	// test
//	for (int i=0;i<500; i++)
//	{
//		EB_GroupInfo pPopDepartment(sEnterpriseCode, 0);
//		pPopDepartment.m_sParentCode = sParentCode;
//		pPopDepartment.m_nGroupType = EB_GROUP_TYPE_DEPARTMENT;
//		CString sGroupName;
//		sGroupName.Format(_T("测试部门 %d"),i);
//		pPopDepartment.m_sGroupName = (LPCTSTR)sGroupName;
//		pPopDepartment.m_sPhone = "0755-27362216";
//		pPopDepartment.m_sFax = "0755-27362216";
//		pPopDepartment.m_sEmail = "yhz@entboost.com";
//		pPopDepartment.m_sUrl = "www.entboost.com";
//		pPopDepartment.m_sAddress = "深圳宝安翻身路47区银丰商务大厦501室";
//		pPopDepartment.m_sDescription = "这是测试部门描述。";
//#ifdef USES_EBCOM_TEST
//		CIEB_GroupInfo* pEBGroupInfo = new CIEB_GroupInfo(pPopDepartment);
//		CComPtr<IEB_GroupInfo> pInterface;
//		pEBGroupInfo->QueryInterface(__uuidof(IEB_GroupInfo),(void**)&pInterface);
//		theEBClientCore->EB_EditGroup(pInterface);
//#else
//		theEBAppClient.EB_EditGroup(&pPopDepartment);
//#endif
//	}
//	return;
	// test end
	//tstring sParentCode;
	//if (hSelItem != m_hEnterpriseItem)
	//{
	//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	//	if (pTreeItemInfo != NULL)
	//		sParentCode = pTreeItemInfo->m_sGroupCode;
	//}
	CDlgGroupInfo pDlgGroupInfo(this);
	pDlgGroupInfo.m_nGroupType = EB_GROUP_TYPE_DEPARTMENT;
	if (pDlgGroupInfo.DoModal() == IDOK)
	{
		EB_GroupInfo pPopDepartment(sEnterpriseCode, 0);
		pPopDepartment.m_sParentCode = sParentCode;
		pPopDepartment.m_nManagerUserId = pDlgGroupInfo.m_nManagerUserId;
		pPopDepartment.m_nGroupType = pDlgGroupInfo.m_nGroupType;
		pPopDepartment.m_sGroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
		pPopDepartment.m_sPhone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
		pPopDepartment.m_sFax = (LPCTSTR)pDlgGroupInfo.m_sFax;
		pPopDepartment.m_sEmail = (LPCTSTR)pDlgGroupInfo.m_sEmail;
		pPopDepartment.m_sUrl = (LPCTSTR)pDlgGroupInfo.m_sUrl;
		pPopDepartment.m_sAddress = (LPCTSTR)pDlgGroupInfo.m_sAddress;
		pPopDepartment.m_nDisplayIndex = pDlgGroupInfo.m_nDisplayIndex;
		pPopDepartment.m_sDescription = (LPCTSTR)pDlgGroupInfo.m_sDescription;
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
void CDlgMyEnterprise::OnDepartment2New()
{
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(0);
	if (pEnterpriseInfo==NULL)
		return;
	const eb::bigint sEnterpriseCode = pEnterpriseInfo->EnterpriseCode;
#else
	EB_EnterpriseInfo pEnterpriseInfo;
	if (!theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo))
		return;
	const eb::bigint sEnterpriseCode = pEnterpriseInfo.m_sEnterpriseCode;
#endif

	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
	eb::bigint sParentCode = pTreeItemInfo->m_sGroupCode;
	//tstring sParentCode;
	//if (hSelItem != m_hEnterpriseItem)
	//{
	//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	//	if (pTreeItemInfo != NULL)
	//		sParentCode = pTreeItemInfo->m_sGroupCode;
	//}
	CDlgGroupInfo pDlgGroupInfo(this);
	pDlgGroupInfo.m_nGroupType = EB_GROUP_TYPE_PROJECT;
	if (pDlgGroupInfo.DoModal() == IDOK)
	{
		EB_GroupInfo pPopDepartment(sEnterpriseCode, 0);
		pPopDepartment.m_sParentCode = sParentCode;
		pPopDepartment.m_nManagerUserId = pDlgGroupInfo.m_nManagerUserId;
		pPopDepartment.m_nGroupType = pDlgGroupInfo.m_nGroupType;
		pPopDepartment.m_sGroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
		pPopDepartment.m_sPhone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
		pPopDepartment.m_sFax = (LPCTSTR)pDlgGroupInfo.m_sFax;
		pPopDepartment.m_sEmail = (LPCTSTR)pDlgGroupInfo.m_sEmail;
		pPopDepartment.m_sUrl = (LPCTSTR)pDlgGroupInfo.m_sUrl;
		pPopDepartment.m_sAddress = (LPCTSTR)pDlgGroupInfo.m_sAddress;
		pPopDepartment.m_nDisplayIndex = pDlgGroupInfo.m_nDisplayIndex;
		pPopDepartment.m_sDescription = (LPCTSTR)pDlgGroupInfo.m_sDescription;
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
void CDlgMyEnterprise::OnDepartmentDelete()
{
	//EB_EnterpriseInfo::pointer pEnterpriseInfo = theEBAppClient.GetEnterpriseInfo();
	//if (pEnterpriseInfo.get() == NULL)
	//	return;

	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pDepItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pDepItemInfo == NULL || pDepItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_GROUP)
		return;
	bool bExistSubDepartment = false;
	{
		BoostReadLock rdlock(m_pDepItemInfo.mutex());
		CLockMap<eb::bigint,CTreeItemInfo::pointer>::iterator pIter = m_pDepItemInfo.begin();
		for (; pIter!=m_pDepItemInfo.end(); pIter++)
		{
			CTreeItemInfo::pointer pTreeItemInfo = pIter->second;
			if (pTreeItemInfo->m_sParentId == pDepItemInfo->m_sGroupCode)
			{
				bExistSubDepartment = true;
				break;
			}
		}
	}
	if (bExistSubDepartment)
	{
		CDlgMessageBox::EbMessageBox(this,"",_T("存在下级子部门：\r\n不能删除部门资料！"),CDlgMessageBox::IMAGE_WARNING,5);
		return;
	}
	const bool bHasMemberInfo = m_treeEnterprise.GetNextItem(hSelItem, TVGN_CHILD)==NULL?false:true;
	//HTREEITEM hChildItem = m_treeEnterprise.GetNextItem(hSelItem, TVGN_CHILD);
	//if (hChildItem != NULL)
	//{
	//	CDlgMessageBox::EbDoModal(this,_T("不能删除非空部门资料！"),_T(""),CDlgMessageBox::IMAGE_ERROR);
	//	return;
	//}
	CString sText;
	if (bHasMemberInfo)
		sText.Format(_T("确定删除部门：\r\n%s 和所有员工资料吗？"), pDepItemInfo->m_sName.c_str());
	else
		sText.Format(_T("确定删除部门：\r\n%s 吗？"), pDepItemInfo->m_sName.c_str());
	if (CDlgMessageBox::EbDoModal(this,"",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_DeleteGroup(pDepItemInfo->m_sGroupCode);
#else
		theEBAppClient.EB_DeleteGroup(pDepItemInfo->m_sGroupCode);
#endif
	}
}
void CDlgMyEnterprise::OnDepartmentEdit()
{
	//EB_EnterpriseInfo::pointer pEnterpriseInfo = theEBAppClient.GetEnterpriseInfo();
	//if (pEnterpriseInfo.get() == NULL)
	//	return;

	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pDepItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pDepItemInfo == NULL || pDepItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_GROUP) return;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pDepItemInfo->m_sGroupCode);
	if (pGroupInfo != NULL)
	{
		CDlgGroupInfo pDlgGroupInfo(this);
		pDlgGroupInfo.m_nGroupId = pGroupInfo->GroupCode;
		pDlgGroupInfo.m_nGroupType = (EB_GROUP_TYPE)pGroupInfo->GroupType;
		pDlgGroupInfo.m_sGroupName = pGroupInfo->GroupName.GetBSTR();
		pDlgGroupInfo.m_sPhone = pGroupInfo->Phone.GetBSTR();
		pDlgGroupInfo.m_sFax = pGroupInfo->Fax.GetBSTR();
		pDlgGroupInfo.m_sEmail = pGroupInfo->Email.GetBSTR();
		pDlgGroupInfo.m_sUrl = pGroupInfo->Url.GetBSTR();
		pDlgGroupInfo.m_sAddress = pGroupInfo->Address.GetBSTR();
		pDlgGroupInfo.m_nDisplayIndex = pGroupInfo->DisplayIndex;
		pDlgGroupInfo.m_sDescription = pGroupInfo->Description.GetBSTR();
		if (pDlgGroupInfo.DoModal() == IDOK)
		{
			pGroupInfo->GroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
			pGroupInfo->Phone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
			pGroupInfo->Fax = (LPCTSTR)pDlgGroupInfo.m_sFax;
			pGroupInfo->Email = (LPCTSTR)pDlgGroupInfo.m_sEmail;
			pGroupInfo->Url = (LPCTSTR)pDlgGroupInfo.m_sUrl;
			pGroupInfo->Address = (LPCTSTR)pDlgGroupInfo.m_sAddress;
			pGroupInfo->DisplayIndex = pDlgGroupInfo.m_nDisplayIndex;
			pGroupInfo->Description = (LPCTSTR)pDlgGroupInfo.m_sDescription;
			theEBClientCore->EB_EditGroup(pGroupInfo);
		}
	}
#else
	EB_GroupInfo pGroupInfo;
	if (theEBAppClient.EB_GetGroupInfo(pDepItemInfo->m_sGroupCode,&pGroupInfo))
	{
		CDlgGroupInfo pDlgGroupInfo(this);
		pDlgGroupInfo.m_nGroupId = pGroupInfo.m_sGroupCode;
		pDlgGroupInfo.m_nManagerUserId = pGroupInfo.m_nManagerUserId;
		pDlgGroupInfo.m_nGroupType = pGroupInfo.m_nGroupType;
		pDlgGroupInfo.m_sGroupName = pGroupInfo.m_sGroupName.c_str();
		pDlgGroupInfo.m_sPhone = pGroupInfo.m_sPhone.c_str();
		pDlgGroupInfo.m_sFax = pGroupInfo.m_sFax.c_str();
		pDlgGroupInfo.m_sEmail = pGroupInfo.m_sEmail.c_str();
		pDlgGroupInfo.m_sUrl = pGroupInfo.m_sUrl.c_str();
		pDlgGroupInfo.m_sAddress = pGroupInfo.m_sAddress.c_str();
		pDlgGroupInfo.m_nDisplayIndex = pGroupInfo.m_nDisplayIndex;
		pDlgGroupInfo.m_sDescription = pGroupInfo.m_sDescription.c_str();
		if (pDlgGroupInfo.DoModal() == IDOK)
		{
			EB_GroupInfo pEditPopDepartment(pGroupInfo.m_sEnterpriseCode, pGroupInfo.m_sGroupCode);
			pEditPopDepartment.m_sParentCode = pGroupInfo.m_sParentCode;
			pEditPopDepartment.m_nManagerUserId = pDlgGroupInfo.m_nManagerUserId;
			pEditPopDepartment.m_sGroupName = (LPCTSTR)pDlgGroupInfo.m_sGroupName;
			pEditPopDepartment.m_sPhone = (LPCTSTR)pDlgGroupInfo.m_sPhone;
			pEditPopDepartment.m_sFax = (LPCTSTR)pDlgGroupInfo.m_sFax;
			pEditPopDepartment.m_sEmail = (LPCTSTR)pDlgGroupInfo.m_sEmail;
			pEditPopDepartment.m_sUrl = (LPCTSTR)pDlgGroupInfo.m_sUrl;
			pEditPopDepartment.m_sAddress = (LPCTSTR)pDlgGroupInfo.m_sAddress;
			pEditPopDepartment.m_nDisplayIndex = pDlgGroupInfo.m_nDisplayIndex;
			pEditPopDepartment.m_sDescription = (LPCTSTR)pDlgGroupInfo.m_sDescription;
			theEBAppClient.EB_EditGroup(&pEditPopDepartment);
		}
	}
#endif
}
void CDlgMyEnterprise::OnDepartmentRefresh()
{
	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pDepItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pDepItemInfo == NULL || pDepItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_GROUP) return;
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LoadGroup(pDepItemInfo->m_sGroupCode,VARIANT_TRUE);
#else
	theEBAppClient.EB_LoadGroup(pDepItemInfo->m_sGroupCode,true);
#endif
}

void CDlgMyEnterprise::OnEmployeeNew()
{
	//EB_EnterpriseInfo::pointer pEnterpriseInfo = theEBAppClient.GetEnterpriseInfo();
	//if (pEnterpriseInfo.get() == NULL) return;
	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	theApp.NewEmployeeInfo(this,pTreeItemInfo);
}
void CDlgMyEnterprise::OnEmployeeEdit()
{
	EditEmployeeInfo(m_treeEnterprise.GetSelectedItem());
	/*
	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER)
		return;
	const EB_MemberInfo::pointer pMemberInfo = theEBAppClient.GetEmployeeInfo(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_sAccount.c_str());
	if (pMemberInfo.get() == NULL)
		return;
	EB_GroupInfo::pointer pGroupInfo = theEBAppClient.GetGroupInfo(pMemberInfo->m_sGroupCode);
	if (pGroupInfo.get() == NULL)
		return;

	CDlgMemberInfo pDlgMemberInfo(this);
	pDlgMemberInfo.m_sGroupCode = pMemberInfo->m_sGroupCode;
	pDlgMemberInfo.m_sMemberAccount = pMemberInfo->m_sMemberAccount.c_str();
	pDlgMemberInfo.m_sUserName = pMemberInfo->m_sUserName.c_str();
	pDlgMemberInfo.m_sGroupName = pGroupInfo->m_sGroupName.c_str();
	pDlgMemberInfo.m_sJobTitle = pMemberInfo->m_sJobTitle.c_str();
	pDlgMemberInfo.m_nJobPosition = pMemberInfo->m_nJobPosition;
	pDlgMemberInfo.m_sCellPhone = pMemberInfo->m_sCellPhone.c_str();
	pDlgMemberInfo.m_sWorkPhone = pMemberInfo->m_sWorkPhone.c_str();
	pDlgMemberInfo.m_sEmail = pMemberInfo->m_sEmail.c_str();
	pDlgMemberInfo.m_sFax = pMemberInfo->m_sFax.c_str();
	pDlgMemberInfo.m_sAddress = pMemberInfo->m_sAddress.c_str();
	pDlgMemberInfo.m_sDescription = pMemberInfo->m_sDescription.c_str();
	pDlgMemberInfo.m_nGender = pMemberInfo->m_nGender;
	pDlgMemberInfo.m_nBirthday = pMemberInfo->m_nBirthday;
	if (pDlgMemberInfo.DoModal() == IDOK)
	{
		EB_MemberInfo::pointer pEditPopMemberInfo = EB_MemberInfo::create(pMemberInfo->m_sGroupCode, pMemberInfo->m_sMemberAccount);
		pEditPopMemberInfo->m_sUserName = (LPCTSTR)pDlgMemberInfo.m_sUserName;
		pEditPopMemberInfo->m_sJobTitle = (LPCTSTR)pDlgMemberInfo.m_sJobTitle;
		pEditPopMemberInfo->m_nJobPosition = pDlgMemberInfo.m_nJobPosition;
		pEditPopMemberInfo->m_sCellPhone = (LPCTSTR)pDlgMemberInfo.m_sCellPhone;
		pEditPopMemberInfo->m_sWorkPhone = (LPCTSTR)pDlgMemberInfo.m_sWorkPhone;
		pEditPopMemberInfo->m_sEmail = (LPCTSTR)pDlgMemberInfo.m_sEmail;
		pEditPopMemberInfo->m_sFax = (LPCTSTR)pDlgMemberInfo.m_sFax;
		pEditPopMemberInfo->m_sAddress = (LPCTSTR)pDlgMemberInfo.m_sAddress;
		pEditPopMemberInfo->m_sDescription = (LPCTSTR)pDlgMemberInfo.m_sDescription;
		pEditPopMemberInfo->m_nGender = (EB_GENDER_TYPE)pDlgMemberInfo.m_nGender;
		pEditPopMemberInfo->m_nBirthday = pDlgMemberInfo.m_nBirthday;
		theEBAppClient.EmpEdit(pEditPopMemberInfo);
	}
	*/
}
void CDlgMyEnterprise::OnEmployeeDelete()
{
	// ?? 权限
	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER)
		return;
	CString sText;
	sText.Format(_T("确定删除员工：\r\n%s(%s) 吗？"), pTreeItemInfo->m_sName.c_str(), pTreeItemInfo->m_sAccount.c_str());
	if (CDlgMessageBox::EbDoModal(this,"删除员工",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("删除员工"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_DeleteMember(pTreeItemInfo->m_sMemberCode, VARIANT_TRUE);
#else
		theEBAppClient.EB_DeleteMember(pTreeItemInfo->m_sMemberCode, true);
#endif
	}
}
void CDlgMyEnterprise::OnMemberDelAdminLevel()
{
	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_DelGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
}
void CDlgMyEnterprise::OnMemberAddAdminLevel()
{
	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_AddGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
}
void CDlgMyEnterprise::OnCallUser()
{
	CallItem(m_treeEnterprise.GetSelectedItem());
}
void CDlgMyEnterprise::OnViewMsgRecord()
{
	const HTREEITEM hItem = m_treeEnterprise.GetSelectedItem();
	if (hItem==NULL) return;
	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hItem);
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
void CDlgMyEnterprise::OnDeleteMsgRecord()
{
	const HTREEITEM hItem = m_treeEnterprise.GetSelectedItem();
	if (hItem==NULL) return;
	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hItem);
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

//int CALLBACK CDlgMyEnterprise::TreeCmpFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
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
//	}else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
//	{
//		// 第一个是部门，排前面
//		return -1;
//	}else if (pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
//	{
//		// 第二个是部门，对调位置
//		return -1;
//	}
//	return 0;
//}

void CDlgMyEnterprise::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return;
	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		theApp.m_pAutoCallGroupIdList.remove(pTreeItemInfo->m_sGroupCode);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallGroup(pTreeItemInfo->m_sGroupCode);
#else
		theEBAppClient.EB_CallGroup(pTreeItemInfo->m_sGroupCode);
#endif
	}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#else
		theEBAppClient.EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#endif
	}
}

#ifdef USES_EBCOM_TEST
void CDlgMyEnterprise::EnterpriseInfo(IEB_EnterpriseInfo* pEnterpriseInfo)
{
	const eb::bigint sEnterpriseCode = pEnterpriseInfo->EnterpriseCode;
	const CEBString sEnterpriseName = pEnterpriseInfo->EnterpriseName.GetBSTR();
	CTreeItemInfo::pointer pEntItemInfo;
	if (m_pEntItemInfo.find(sEnterpriseCode,pEntItemInfo))
	{
		long nEnterpriseMemberSize = 0;
		long nEnterpriseOnlineSize = 0;
		theEBClientCore->EB_GetEnterpriseMemberSize(pEnterpriseInfo->EnterpriseCode,&nEnterpriseMemberSize,&nEnterpriseOnlineSize);
		CString sText;
		sText.Format(_T("%s [%d/%d]"), sEnterpriseName.c_str(),nEnterpriseOnlineSize,nEnterpriseMemberSize);
		m_treeEnterprise.SetItemText(pEntItemInfo->m_hItem, sText);
	}else
	{
		HTREEITEM hItem = m_treeEnterprise.InsertItem(sEnterpriseName.c_str());
		pEntItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_ENTERPRISE,hItem);
		pEntItemInfo->m_sEnterpriseCode = sEnterpriseCode;
		pEntItemInfo->m_sName = sEnterpriseName;
		m_pEntItemInfo.insert(sEnterpriseCode,pEntItemInfo);
		m_treeEnterprise.SetItemData(pEntItemInfo->m_hItem,(DWORD)pEntItemInfo.get());
	}
	m_treeEnterprise.SelectItem(pEntItemInfo->m_hItem);
}
#else
void CDlgMyEnterprise::EnterpriseInfo(const EB_EnterpriseInfo* pEnterpriseInfo)
{
	CTreeItemInfo::pointer pEntItemInfo;
	if (m_pEntItemInfo.find(pEnterpriseInfo->m_sEnterpriseCode,pEntItemInfo))
	{
		int nEnterpriseMemberSize = 0;
		int nEnterpriseOnlineSize = 0;
		theEBAppClient.EB_GetEnterpriseMemberSize(pEnterpriseInfo->m_sEnterpriseCode,nEnterpriseMemberSize,nEnterpriseOnlineSize);
		CString sText;
		sText.Format(_T("%s [%d/%d]"), pEnterpriseInfo->m_sEnterpriseName.c_str(),nEnterpriseOnlineSize,nEnterpriseMemberSize);
		m_treeEnterprise.SetItemText(pEntItemInfo->m_hItem, sText);
	}else
	{
		HTREEITEM hItem = m_treeEnterprise.InsertItem(pEnterpriseInfo->m_sEnterpriseName.c_str());
		pEntItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_ENTERPRISE,hItem);
		pEntItemInfo->m_sEnterpriseCode = pEnterpriseInfo->m_sEnterpriseCode;
		pEntItemInfo->m_sName = pEnterpriseInfo->m_sEnterpriseName;
		m_pEntItemInfo.insert(pEnterpriseInfo->m_sEnterpriseCode,pEntItemInfo);
		m_treeEnterprise.SetItemData(pEntItemInfo->m_hItem,(DWORD)pEntItemInfo.get());
	}
	m_treeEnterprise.SelectItem(pEntItemInfo->m_hItem);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgMyEnterprise::DepartmentInfo(IEB_GroupInfo* pGroupInfo)
{
	const eb::bigint sEnterpriseCode = pGroupInfo->EnterpriseCode;
	const eb::bigint sGroupCode = pGroupInfo->GroupCode;
	const CEBString sGroupName = pGroupInfo->GroupName.GetBSTR();
	const eb::bigint sParentCode = pGroupInfo->ParentCode;
	CTreeItemInfo::pointer pEntItemInfo;
	if (!m_pEntItemInfo.find(sEnterpriseCode,pEntItemInfo))
	{
		return;
	}
	HTREEITEM hParentItem = NULL;
	if (sParentCode==0)
	{
		hParentItem = pEntItemInfo->m_hItem;
	}else
	{
		CTreeItemInfo::pointer pParentItemInfo;
		if (!m_pDepItemInfo.find(sParentCode,pParentItemInfo))
		{
			return;
		}
		hParentItem = pParentItemInfo->m_hItem;
	}
	CTreeItemInfo::pointer pTreeItemInfo;
	if (!m_pDepItemInfo.find(sGroupCode,pTreeItemInfo))
	{
		HTREEITEM hItem = m_treeEnterprise.InsertItem(sGroupName.c_str(), hParentItem);
		pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hItem);
		pTreeItemInfo->m_sEnterpriseCode = sEnterpriseCode;
		pTreeItemInfo->m_sParentId = sParentCode;
		pTreeItemInfo->m_sGroupCode = sGroupCode;
		pTreeItemInfo->m_nSubType = pGroupInfo->GroupType;
		pTreeItemInfo->m_dwItemData = 0;
		pTreeItemInfo->m_nBigId = pGroupInfo->MyEmpId;
		m_pDepItemInfo.insert(sGroupCode,pTreeItemInfo);
		m_treeEnterprise.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
	}
	pTreeItemInfo->m_sName = sGroupName;
	pTreeItemInfo->m_nIndex = pGroupInfo->DisplayIndex;
	pTreeItemInfo->m_nSubType = pGroupInfo->GroupType;
	m_treeEnterprise.Sort(hParentItem,CPOPApp::TreeCmpFunc);
	//m_treeEnterprise.Expand(pEntItemInfo->m_hItem, TVE_EXPAND);
	SetGroupCount(pTreeItemInfo->m_hItem, pGroupInfo);
}
#else
void CDlgMyEnterprise::DepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	CTreeItemInfo::pointer pEntItemInfo;
	if (!m_pEntItemInfo.find(pGroupInfo->m_sEnterpriseCode,pEntItemInfo))
	{
		return;
	}
	HTREEITEM hParentItem = NULL;
	if (pGroupInfo->m_sParentCode==0)
	{
		hParentItem = pEntItemInfo->m_hItem;
	}else
	{
		CTreeItemInfo::pointer pParentItemInfo;
		if (!m_pDepItemInfo.find(pGroupInfo->m_sParentCode,pParentItemInfo))
		{
			return;
		}
		hParentItem = pParentItemInfo->m_hItem;
	}
	CTreeItemInfo::pointer pTreeItemInfo;
	if (!m_pDepItemInfo.find(pGroupInfo->m_sGroupCode,pTreeItemInfo))
	{
		HTREEITEM hItem = m_treeEnterprise.InsertItem(pGroupInfo->m_sGroupName.c_str(), hParentItem);
		pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hItem);
		pTreeItemInfo->m_sEnterpriseCode = pGroupInfo->m_sEnterpriseCode;
		pTreeItemInfo->m_sParentId = pGroupInfo->m_sParentCode;
		pTreeItemInfo->m_sGroupCode = pGroupInfo->m_sGroupCode;
		pTreeItemInfo->m_nSubType = pGroupInfo->m_nGroupType;
		pTreeItemInfo->m_dwItemData = 0;
		pTreeItemInfo->m_nBigId = pGroupInfo->m_nMyEmpId;
		m_pDepItemInfo.insert(pGroupInfo->m_sGroupCode,pTreeItemInfo);
		m_treeEnterprise.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
	}
	pTreeItemInfo->m_sName = pGroupInfo->m_sGroupName;
	pTreeItemInfo->m_nIndex = pGroupInfo->m_nDisplayIndex;
	pTreeItemInfo->m_nSubType = pGroupInfo->m_nGroupType;
	m_treeEnterprise.Sort(hParentItem,CPOPApp::TreeCmpFunc);
	//m_treeEnterprise.Expand(pEntItemInfo->m_hItem, TVE_EXPAND);
	SetGroupCount(pTreeItemInfo->m_hItem, pGroupInfo);
}
#endif

void CDlgMyEnterprise::DeleteDepartmentInfo(eb::bigint nGroupCode)
{
	CTreeItemInfo::pointer pTreeItemInfo;
	if (m_pDepItemInfo.find(nGroupCode, pTreeItemInfo, true))
	{
		m_treeEnterprise.DeleteItem(pTreeItemInfo->m_hItem);

		CTreeItemInfo::pointer pEntItemInfo;
		if (m_pEntItemInfo.find(pTreeItemInfo->m_sEnterpriseCode,pEntItemInfo))
		{
			int nEnterpriseMemberSize = 0;
			int nEnterpriseOnlineSize = 0;
			theEBAppClient.EB_GetEnterpriseMemberSize(pTreeItemInfo->m_sEnterpriseCode,nEnterpriseMemberSize,nEnterpriseOnlineSize);
			CString sText;
			sText.Format(_T("%s [%d/%d]"), pEntItemInfo->m_sName.c_str(),nEnterpriseOnlineSize,nEnterpriseMemberSize);
			m_treeEnterprise.SetItemText(pEntItemInfo->m_hItem, sText);
		}
	}
}
//void CDlgMyEnterprise::ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo)
//{
//	// ? change parent_code.
//	CTreeItemInfo::pointer pTreeItemInfo;
//	if (m_pDepItemInfo.find(pGroupInfo->m_sGroupCode, pTreeItemInfo))
//	{
//		m_treeEnterprise.Sort(pTreeItemInfo->m_hItem,TreeCmpFunc);
//		SetGroupCount(pTreeItemInfo->m_hItem, pGroupInfo);
//	}
//}
#ifdef USES_EBCOM_TEST
void CDlgMyEnterprise::EmployeeInfo(IEB_MemberInfo* pEBEmployeeInfo, bool bChangeLineState)
{
	const eb::bigint sGroupCode = pEBEmployeeInfo->GroupCode;
	const eb::bigint sMemberCode = pEBEmployeeInfo->MemberCode;
	const eb::bigint nMemberUserId = pEBEmployeeInfo->MemberUserId;
	const CEBString sMemberAccount = pEBEmployeeInfo->MemberAccount.GetBSTR();
	const CEBString sUserName = pEBEmployeeInfo->UserName.GetBSTR();
	const CEBString sJobTitle = pEBEmployeeInfo->JobTitle.GetBSTR();
	const CEBString sCellPhone = pEBEmployeeInfo->CellPhone.GetBSTR();
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(sGroupCode, pDepItemInfo))
	{
		return;
	}
	// 已经加载成员
	if (pDepItemInfo->m_dwItemData==0)
		pDepItemInfo->m_dwItemData = 1;

	bool bNew = false;
	CTreeItemInfo::pointer pEmpItemInfo;
	if (!m_pEmpItemInfo.find(sMemberCode,pEmpItemInfo))
	{
		bNew = true;
		CString sText;
		if (sJobTitle.empty())
			sText.Format(_T("%s"), sUserName.c_str());
		else
			sText.Format(_T("%s - %s"), sUserName.c_str(), sJobTitle.c_str());
		HTREEITEM hItem = m_treeEnterprise.InsertItem(sText, pDepItemInfo->m_hItem);
		pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hItem);
		pEmpItemInfo->m_sGroupCode = sGroupCode;
		pEmpItemInfo->m_sMemberCode = sMemberCode;
		pEmpItemInfo->m_nUserId = nMemberUserId;
		pEmpItemInfo->m_sName = sUserName;
		pEmpItemInfo->m_sAccount = sMemberAccount;
		pEmpItemInfo->m_dwItemData = pEBEmployeeInfo->LineState;
		m_pEmpItemInfo.insert(sMemberCode,pEmpItemInfo);
		m_treeEnterprise.SetItemData(hItem, (DWORD)pEmpItemInfo.get());
		m_treeEnterprise.Sort(pDepItemInfo->m_hItem,CPOPApp::TreeCmpFunc);
	}else
	{
		pEmpItemInfo->m_sName = sUserName;
		pEmpItemInfo->m_dwItemData = pEBEmployeeInfo->LineState;
		CString sText;
		if (sJobTitle.empty())
			sText.Format(_T("%s"), sUserName.c_str());
		else
			sText.Format(_T("%s - %s"), sUserName.c_str(), sJobTitle.c_str());
		m_treeEnterprise.SetItemText(pEmpItemInfo->m_hItem, sText);
		// **不需要选择
		//m_treeEnterprise.SelectItem(pEmpItemInfo->m_hItem);
		// ?? 这里要实现，状况改变
		m_treeEnterprise.Sort(pDepItemInfo->m_hItem,CPOPApp::TreeCmpFunc);
	}
	if (bChangeLineState)
		SetGroupCount(pDepItemInfo->m_hItem, sGroupCode);
	// **不需要选择
	//if (bNew)
	//{
	//	m_treeEnterprise.Expand(pDepItemInfo->m_hItem, TVE_EXPAND);
	//	m_treeEnterprise.SelectItem(pEmpItemInfo->m_hItem);
	//}
}
#else
void CDlgMyEnterprise::EmployeeInfo(const EB_MemberInfo* pMemberInfo, bool bChangeLineState)
{
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(pMemberInfo->m_sGroupCode, pDepItemInfo))
	{
		return;
	}
	// 已经加载成员
	if (pDepItemInfo->m_dwItemData==0)
		pDepItemInfo->m_dwItemData = 1;

	bool bNew = false;
	CTreeItemInfo::pointer pEmpItemInfo;
	if (!m_pEmpItemInfo.find(pMemberInfo->m_sMemberCode,pEmpItemInfo))
	{
		bNew = true;
		CString sText;
		if (pMemberInfo->m_sJobTitle.empty())
			sText.Format(_T("%s"), pMemberInfo->m_sUserName.c_str());
		else
			sText.Format(_T("%s - %s"), pMemberInfo->m_sUserName.c_str(), pMemberInfo->m_sJobTitle.c_str());
		HTREEITEM hItem = m_treeEnterprise.InsertItem(sText, pDepItemInfo->m_hItem);
		pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hItem);
		pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
		pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
		//if (pMemberInfo->m_sMemberCode==9277623900592183 ||
		//	pMemberInfo->m_sMemberCode==9377317600878899 ||
		//	pMemberInfo->m_sMemberCode==9358279100713793)
		//{
		//	int i=0;
		//}
		pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
		pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
		pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
		pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
		m_pEmpItemInfo.insert(pMemberInfo->m_sMemberCode,pEmpItemInfo);
		m_treeEnterprise.SetItemData(hItem, (DWORD)pEmpItemInfo.get());
		m_treeEnterprise.Sort(pDepItemInfo->m_hItem,CPOPApp::TreeCmpFunc);
	}else
	{
		pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
		pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
		CString sText;
		if (pMemberInfo->m_sJobTitle.empty())
			sText.Format(_T("%s"), pMemberInfo->m_sUserName.c_str());
		else
			sText.Format(_T("%s - %s"), pMemberInfo->m_sUserName.c_str(), pMemberInfo->m_sJobTitle.c_str());
		m_treeEnterprise.SetItemText(pEmpItemInfo->m_hItem, sText);
		// **不需要选择
		//m_treeEnterprise.SelectItem(pEmpItemInfo->m_hItem);
		// ?? 这里要实现，状况改变
		m_treeEnterprise.Sort(pDepItemInfo->m_hItem,CPOPApp::TreeCmpFunc);
	}

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

	if (bChangeLineState)
		SetGroupCount(pDepItemInfo->m_hItem, pMemberInfo->m_sGroupCode);
	// **不需要选择
	//if (bNew)
	//{
	//	m_treeEnterprise.Expand(pDepItemInfo->m_hItem, TVE_EXPAND);
	//	m_treeEnterprise.SelectItem(pEmpItemInfo->m_hItem);
	//}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgMyEnterprise::DeleteEmployeeInfo(IEB_GroupInfo* pGroupInfo, eb::bigint sMemberCode)
{
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(pGroupInfo->GroupCode, pDepItemInfo))
	{
		return;
	}
	CTreeItemInfo::pointer pEmpItemInfo;
	if (m_pEmpItemInfo.find(sMemberCode, pEmpItemInfo, true))
	{
		m_treeEnterprise.DeleteItem(pEmpItemInfo->m_hItem);
		SetGroupCount(pDepItemInfo->m_hItem, pGroupInfo);
	}
}
#else
void CDlgMyEnterprise::DeleteEmployeeInfo(const EB_GroupInfo* pGroupInfo, eb::bigint nMemberCode)
{
	CTreeItemInfo::pointer pDepItemInfo;
	if (!m_pDepItemInfo.find(pGroupInfo->m_sGroupCode, pDepItemInfo))
	{
		return;
	}
	CTreeItemInfo::pointer pEmpItemInfo;
	if (m_pEmpItemInfo.find(nMemberCode, pEmpItemInfo, true))
	{
		m_treeEnterprise.DeleteItem(pEmpItemInfo->m_hItem);
		SetGroupCount(pDepItemInfo->m_hItem, pGroupInfo);
	}
}
#endif

void CDlgMyEnterprise::OnNMRClickTreeEnterprise(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	EB_FUNC_LOCATION nFuncLocation = EB_FUNC_LOCATION_UNKNOWN;
	theApp.ClearSubscribeSelectInfo();
	const HTREEITEM hSelItem = m_treeEnterprise.SelectHitTest();
	//const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
	CNewMenu m_menu2;
	m_menu2.CreatePopupMenu();
	m_menu2.SetBitmapBackground(RGB(192,192,192));
	bool bNeedSeparator = false;
#ifdef USES_EBCOM_TEST
	bool bIsMyGroup = theEBClientCore->EB_IsMyGroup(pTreeItemInfo->m_sGroupCode)?true:false;
#else
	bool bIsMyGroup = theEBAppClient.EB_IsMyGroup(pTreeItemInfo->m_sGroupCode);
#endif
	if ((pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pTreeItemInfo->m_nUserId!=theApp.GetLogonUserId()) ||
		(pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && bIsMyGroup))
	{
		bNeedSeparator = false;
		//if (bIsMyGroup)
		//{
		//	bNeedSeparator = true;
		//	m_menu2.InsertODMenu(-1,_T("打开会话(&C)"),MF_BYPOSITION,EB_COMMAND_CALL_USER,IDB_BITMAP_MENU_CALL);
		//}
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && bIsMyGroup)
		{
			//if (bNeedSeparator)
			//	m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = true;
			m_menu2.InsertODMenu(-1,_T("群共享(&S)"),MF_BYPOSITION,EB_COMMAND_VIEW_GROUP_SHARE,IDB_BITMAP_MENU_SHARE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_VIEW_GROUP_SHARE,_T("群共享"));
		}
		// 判断聊天记录
		CString sSql;
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
		{
			nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER;
			theApp.m_nSelectUserId = pTreeItemInfo->m_nUserId;
			sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) LIMIT 1"),
				pTreeItemInfo->m_nUserId,pTreeItemInfo->m_nUserId);
		}else
		{
			nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_GROUP;
			theApp.m_nSelectGroupId = pTreeItemInfo->m_sGroupCode;
			sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=%lld LIMIT 1"),pTreeItemInfo->m_sGroupCode);
		}
		if (theApp.m_pBoUsers->select(sSql)>0)
		{
			m_menu2.InsertODMenu(-1,_T("聊天记录"),MF_BYPOSITION,EB_MSG_VIEW_MSG_RECORD,IDB_BITMAP_MENU_MSG);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_MSG_VIEW_MSG_RECORD,_T("聊天记录"));
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_MSG_RECORD,_T("清空聊天记录"));
			bNeedSeparator = true;
		}
	}
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		// 我是群主，选择对象不是群主
		if (theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = true;
			if (theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_DEL_ADMIN,_T("撤销管理员"));
			else
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_ADMIN,_T("加为管理员"));
		}
	}

#ifdef USES_EBCOM_TEST
	if (theEBClientCore->GetEB_CanDeleteGroupInfo(pTreeItemInfo->m_sEnterpriseCode))
#else
	if (theEBAppClient.EB_CanEditEnterprise(pTreeItemInfo->m_sEnterpriseCode))
#endif
	{
		nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_ENTERPRISE;
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE)
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = true;
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_ENTERPRISE,_T("修改公司资料"));

//			if (m_sEntManagerUrl.empty())
//			{
//#ifdef USES_EBCOM_TEST
//				unsigned long pEntManagerUrl = theEBClientCore->EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL);
//#else
//				unsigned long pEntManagerUrl = 0;
//				theEBAppClient.EB_GetSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL, &pEntManagerUrl);
//#endif
//				if (pEntManagerUrl != NULL && strlen((const char*)pEntManagerUrl)>0)
//				{
//					m_sEntManagerUrl = (const char*)pEntManagerUrl;
//#ifdef USES_EBCOM_TEST
//					theEBClientCore->EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
//#else
//					theEBAppClient.EB_FreeSystemParameter(EB_SYSTEM_PARAMETER_ENT_MANAGER_URL,pEntManagerUrl);
//#endif
//				}
//			}
			if (!theApp.GetEntManagerurl().empty())
			{
				m_menu2.AppendMenu(MF_SEPARATOR);
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_IMPORT_ENT_DATA,_T("管理中心"));
			}
		}
	}
	//if ((nManagerLevel & (int)EB_LEVEL_DEP_EDIT) == EB_LEVEL_DEP_EDIT)
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->GetEB_CanEditGroupInfo(pTreeItemInfo->m_sEnterpriseCode,pTreeItemInfo->m_sGroupCode))
#else
	if (theEBAppClient.EB_CanEditGroupInfo(pTreeItemInfo->m_sEnterpriseCode,pTreeItemInfo->m_sGroupCode))
#endif
	{
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = true;
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_DEPARTMENT1,_T("新建下级部门"));
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_DEPARTMENT2,_T("新建下级项目组"));
		}
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			CString sText;
			sText.Format(_T("修改%s资料"),GetGroupTypeText((EB_GROUP_TYPE)pTreeItemInfo->m_nSubType));
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,sText);
		}
	}
	//if ((nManagerLevel & (int)EB_LEVEL_DEP_DELETE) == EB_LEVEL_DEP_DELETE)
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->GetEB_CanDeleteGroupInfo(pTreeItemInfo->m_sGroupCode))
#else
	if (theEBAppClient.EB_CanDeleteGroupInfo(pTreeItemInfo->m_sGroupCode))
#endif
	{
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
		{
			CString sText;
			sText.Format(_T("删除%s(&D)"),GetGroupTypeText((EB_GROUP_TYPE)pTreeItemInfo->m_nSubType));
			m_menu2.AppendODMenu(sText,MF_BYPOSITION,EB_COMMAND_DELETE_DEPARTMENT,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_DEPARTMENT,sText);
		}
	}
	//if ((nManagerLevel & (int)EB_LEVEL_EMP_EDIT) == EB_LEVEL_EMP_EDIT)
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->GetEB_CanEditMemberInfo(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
#else
	if (theEBAppClient.EB_CanEditMemberInfo(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
#endif
	{
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = false;
			CString sText;
			if (pTreeItemInfo->m_nSubType==EB_GROUP_TYPE_DEPARTMENT)
				sText = _T("新建部门员工");
			else
				sText.Format(_T("新建%s成员"),GetGroupTypeText((EB_GROUP_TYPE)pTreeItemInfo->m_nSubType));
			m_menu2.InsertODMenu(-1,sText,MF_BYPOSITION,EB_COMMAND_DEPARTMENT_ADD_EMP,IDB_BITMAP_MENU_ADDUSER);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_ADD_EMP,sText);
		}
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = false;
			if (pTreeItemInfo->m_nUserId==theApp.GetLogonUserId())
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改我的名片"));
			else
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改员工名片"));
		}
	}
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->GetEB_CanDeleteMemberInfo(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
#else
	if (theEBAppClient.EB_CanDeleteMemberInfo(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
#endif
	{
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = false;
			m_menu2.AppendODMenu(_T("删除员工资料(&D)"),MF_BYPOSITION,EB_COMMAND_DEPARTMENT_DEL_EMP,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_DEL_EMP,_T("删除员工资料"));
		}
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
			//m_menu2.AppendMenu(MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenu.m_hMenu,_T("集成应用"));
		}
	}

	//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_REFRESH,_T("刷新"));
	//if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	//	m_menu2.EnableMenuItem(EB_COMMAND_DEPARTMENT_REFRESH,MF_BYCOMMAND|MF_ENABLED);
	//else
	//	m_menu2.EnableMenuItem(EB_COMMAND_DEPARTMENT_REFRESH,MF_BYCOMMAND|MF_GRAYED);
	CPoint point;
	GetCursorPos(&point);
	CPoint point2(point);
	this->ScreenToClient(&point2);
	if (point2.y>180)
		m_menu2.TrackPopupMenu(TPM_LEFTBUTTON|TPM_BOTTOMALIGN,point.x,point.y,this);
	else
		m_menu2.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
}
void CDlgMyEnterprise::OnSubscribeFunc(UINT nID)
{
	size_t nIndex = nID-EB_COMMAND_SUBSCRIBE_FUNC;
	if (nIndex>=0 && nIndex<theApp.m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[nIndex];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
	}
}


void CDlgMyEnterprise::OnNMDblclkTreeEnterprise(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

//bool CDlgMyEnterprise::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return NULL;
//	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_ENTERPRISE)
//	{
//		pImage1 = theApp.m_imageDefaultOrg->Clone();
//		return true;
//	}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
//	{
//		// **部门不显示图标
//		return false;
//	}else	
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
//HICON CDlgMyEnterprise::GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return NULL;
//	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_ENTERPRISE)
//		return theApp.GetIconEnt();
//	else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
//		return NULL;	//theApp.GetIconDep();
//	else
//		return theApp.GetIconCon();
//}
//
//int CDlgMyEnterprise::GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, HICON& pOutIcon) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hItem);
//	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return 1;
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
//bool CDlgMyEnterprise::GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo != NULL &&
//		(pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE))
//		return true;
//	return false;
//}

LRESULT CDlgMyEnterprise::OnTreeItemSelChanged(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hSelItem = (HTREEITEM)wp;
	if (hSelItem==NULL) return 0;
	CTreeItemInfo * pTreeItemInfo = (CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)// && pTreeItemInfo->m_dwItemData==0)
	{
		// 已经自动加载一次
		pTreeItemInfo->m_dwItemData = 1;
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_LoadGroup(pTreeItemInfo->m_sGroupCode,VARIANT_TRUE);
#else
		theEBAppClient.EB_LoadGroup(pTreeItemInfo->m_sGroupCode,true);
#endif
	}
	return 0;
}
LRESULT CDlgMyEnterprise::OnTreeItemDblclk(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hDblClkItem = (HTREEITEM)wp;
	if (hDblClkItem==NULL) return 0;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hDblClkItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER && pTreeItemInfo->m_nUserId==theApp.GetLogonUserId())
	{
		OnEmployeeEdit();
	}else
	{
		CallItem(hDblClkItem);
	}
	return 0;
}

LRESULT CDlgMyEnterprise::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
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
		const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(item);
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
		m_treeEnterprise.GetItemRect(item, &rectItem, TRUE);
		CRect rectHead(rectItem);
		rectHead.right = rectHead.left + m_treeEnterprise.GetIconWidth();
		this->ClientToScreen(rectHead);
		if (rectHead.PtInRect(pos))
		{
			const int const_dlg_width = 380;
			const int const_dlg_height = 188;
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
			if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_ENTERPRISE)
			{
#ifdef USES_EBCOM_TEST
				CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(pTreeItemInfo->m_sEnterpriseCode);
				if (pEnterpriseInfo == NULL) return 1;
				m_pViewContactInfo->SetEnterpriseInfo(pEnterpriseInfo);
#else
				EB_EnterpriseInfo pEnterpriseInfo;
				if (!theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo,pTreeItemInfo->m_sEnterpriseCode)) return 1;
				m_pViewContactInfo->SetEnterpriseInfo(&pEnterpriseInfo);
#endif
			}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
			{
#ifdef USES_EBCOM_TEST
				CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(pTreeItemInfo->m_sEnterpriseCode);
				if (pEnterpriseInfo == NULL) return 1;
				CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode);
				if (pGroupInfo == NULL) return 1;
				m_pViewContactInfo->SetDepartemntInfo(pGroupInfo,pEnterpriseInfo);
#else
				EB_EnterpriseInfo pEnterpriseInfo;
				if (!theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo,pTreeItemInfo->m_sEnterpriseCode)) return 1;
				EB_GroupInfo pGroupInfo;
				if (!theEBAppClient.EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode,&pGroupInfo)) return 1;
				m_pViewContactInfo->SetDepartemntInfo(&pGroupInfo,&pEnterpriseInfo);
#endif
			}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
			{
#ifdef USES_EBCOM_TEST
				CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode);
				if (pGroupInfo == NULL) return 1;
				CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pTreeItemInfo->m_sMemberCode);
				if (pMemberInfo == NULL) return 1;
				m_pViewContactInfo->SetEmployeeInfo(pMemberInfo,pGroupInfo);
#else
				EB_GroupInfo pGroupInfo;
				if (!theEBAppClient.EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode,&pGroupInfo)) return 1;
				EB_MemberInfo pMemberInfo;
				if (!theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,pTreeItemInfo->m_sMemberCode)) return 1;
				m_pViewContactInfo->SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
#endif
			}
			m_pViewContactInfo->SetMoveEnter();
			//m_pViewContactInfo->ShowWindow(SW_SHOW);
			//m_pViewContactInfo->SetCheckLeave();
		}else// if (m_pViewContactInfo->IsWindowVisible())
		{
			m_pViewContactInfo->SetMoveLeave();
			//m_pViewContactInfo->ShowWindow(SW_HIDE);
		}

		const int nTop = rectItem.top;
		m_treeEnterprise.GetClientRect(&rectItem);
		const int nRight = rectItem.right;
		const int const_btn_width = m_btnCallTrack.GetImgWidth();
		//const int const_btn_height = 24;
#ifdef USES_EBCOM_TEST
		const bool bIsMyGroup = theEBClientCore->EB_IsMyGroup(pTreeItemInfo->m_sGroupCode)?true:false;
#else
		const bool bIsMyGroup = theEBAppClient.EB_IsMyGroup(pTreeItemInfo->m_sGroupCode);
#endif
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pTreeItemInfo->m_nUserId==theApp.GetLogonUserId())
		{
			m_btnCallTrack.ShowWindow(SW_HIDE);
			m_btnEditEmpTrack.MovePoint(nRight-const_btn_width, nTop);
			m_btnEditEmpTrack.ShowWindow(SW_SHOW);
			m_btnEditEmpTrack.Invalidate();
		}else if ((pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pTreeItemInfo->m_nUserId!=theApp.GetLogonUserId()) ||
			(pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && bIsMyGroup))
		{
			m_btnEditEmpTrack.ShowWindow(SW_HIDE);
			m_btnCallTrack.MovePoint(nRight-const_btn_width, nTop);
			m_btnCallTrack.ShowWindow(SW_SHOW);
			m_btnCallTrack.Invalidate();
		}else
		{
			m_btnCallTrack.ShowWindow(SW_HIDE);
			m_btnEditEmpTrack.ShowWindow(SW_HIDE);
		}
	}
	return 0;
}

BOOL CDlgMyEnterprise::PreTranslateMessage(MSG* pMsg)
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

	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd != NULL && pMsg->hwnd == m_btnCallTrack.GetSafeHwnd())
	{
		CallItem(m_treeEnterprise.GetTrackItem());
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeEnterprise.GetSafeHwnd())
	{
		CallItem(m_treeEnterprise.GetSelectedItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnEditEmpTrack.GetSafeHwnd())
	{
		SetTimer(TIMERID_EDIT_MEMBERINFO, 1, NULL);
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CDlgMyEnterprise::EditEmployeeInfo(HTREEITEM hSelItem)
{
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	
	//if (pTreeItemInfo->m_sAccount != theEBAppClient.EB_GetLogonAccount()) return;
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->GetEB_CanEditMemberInfo(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)==VARIANT_FALSE) return;

	CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pTreeItemInfo->m_sMemberCode);
	if (pMemberInfo == NULL) return;
	const CEBString sGroupName = theEBClientCore->EB_GetGroupName(pMemberInfo->GroupCode).GetBSTR();
	if (sGroupName.empty()) return;
	CDlgMemberInfo pDlgMemberInfo(this);
	pDlgMemberInfo.m_sGroupCode = pMemberInfo->GroupCode;
	pDlgMemberInfo.m_sMemberAccount = pMemberInfo->MemberAccount.GetBSTR();
	pDlgMemberInfo.m_sUserName = pMemberInfo->UserName.GetBSTR();
	pDlgMemberInfo.m_sGroupName = sGroupName.c_str();
	pDlgMemberInfo.m_sJobTitle = pMemberInfo->JobTitle.GetBSTR();
	pDlgMemberInfo.m_nJobPosition = pMemberInfo->JobPosition;
	pDlgMemberInfo.m_sCellPhone = pMemberInfo->CellPhone.GetBSTR();
	pDlgMemberInfo.m_sWorkPhone = pMemberInfo->WorkPhone.GetBSTR();
	pDlgMemberInfo.m_sEmail = pMemberInfo->Email.GetBSTR();
	pDlgMemberInfo.m_sFax = pMemberInfo->Fax.GetBSTR();
	pDlgMemberInfo.m_sAddress = pMemberInfo->Address.GetBSTR();
	pDlgMemberInfo.m_sDescription = pMemberInfo->Description.GetBSTR();
	pDlgMemberInfo.m_nGender = pMemberInfo->Gender;
	pDlgMemberInfo.m_nBirthday = pMemberInfo->Birthday;
	if (pDlgMemberInfo.DoModal() == IDOK)
	{
		pMemberInfo->UserName = (LPCTSTR)pDlgMemberInfo.m_sUserName;
		pMemberInfo->JobTitle = (LPCTSTR)pDlgMemberInfo.m_sJobTitle;
		pMemberInfo->JobPosition = pDlgMemberInfo.m_nJobPosition;
		pMemberInfo->CellPhone = (LPCTSTR)pDlgMemberInfo.m_sCellPhone;
		pMemberInfo->WorkPhone = (LPCTSTR)pDlgMemberInfo.m_sWorkPhone;
		pMemberInfo->Email = (LPCTSTR)pDlgMemberInfo.m_sEmail;
		pMemberInfo->Fax = (LPCTSTR)pDlgMemberInfo.m_sFax;
		pMemberInfo->Address = (LPCTSTR)pDlgMemberInfo.m_sAddress;
		pMemberInfo->Description = (LPCTSTR)pDlgMemberInfo.m_sDescription;
		pMemberInfo->Gender = (EB_GENDER_TYPE)pDlgMemberInfo.m_nGender;
		pMemberInfo->Birthday = pDlgMemberInfo.m_nBirthday;
		theEBClientCore->EB_EditMember(pMemberInfo);
	}
#else
	if (!theEBAppClient.EB_CanEditMemberInfo(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) return;

	EB_MemberInfo pMemberInfo;
	if (!theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,pTreeItemInfo->m_sMemberCode)) return;
	CEBString sGroupName;
	if (!theEBAppClient.EB_GetGroupName(pMemberInfo.m_sGroupCode,sGroupName)) return;
	CDlgMemberInfo pDlgMemberInfo(this);
	pDlgMemberInfo.m_sGroupCode = pMemberInfo.m_sGroupCode;
	pDlgMemberInfo.m_sMemberUserId = pMemberInfo.m_nMemberUserId;
	pDlgMemberInfo.m_sMemberAccount = pMemberInfo.m_sMemberAccount.c_str();
	pDlgMemberInfo.m_sUserName = pMemberInfo.m_sUserName.c_str();
	pDlgMemberInfo.m_sGroupName = sGroupName.c_str();
	pDlgMemberInfo.m_sJobTitle = pMemberInfo.m_sJobTitle.c_str();
	pDlgMemberInfo.m_nJobPosition = pMemberInfo.m_nJobPosition;
	pDlgMemberInfo.m_sCellPhone = pMemberInfo.m_sCellPhone.c_str();
	pDlgMemberInfo.m_sWorkPhone = pMemberInfo.m_sWorkPhone.c_str();
	pDlgMemberInfo.m_sEmail = pMemberInfo.m_sEmail.c_str();
	pDlgMemberInfo.m_sFax = pMemberInfo.m_sFax.c_str();
	pDlgMemberInfo.m_sAddress = pMemberInfo.m_sAddress.c_str();
	pDlgMemberInfo.m_sDescription = pMemberInfo.m_sDescription.c_str();
	pDlgMemberInfo.m_nGender = pMemberInfo.m_nGender;
	pDlgMemberInfo.m_nBirthday = pMemberInfo.m_nBirthday;
	if (pDlgMemberInfo.DoModal() == IDOK)
	{
		EB_MemberInfo pEditPopMemberInfo(&pMemberInfo);
		pEditPopMemberInfo.m_sUserName = (LPCTSTR)pDlgMemberInfo.m_sUserName;
		pEditPopMemberInfo.m_sJobTitle = (LPCTSTR)pDlgMemberInfo.m_sJobTitle;
		pEditPopMemberInfo.m_nJobPosition = pDlgMemberInfo.m_nJobPosition;
		pEditPopMemberInfo.m_sCellPhone = (LPCTSTR)pDlgMemberInfo.m_sCellPhone;
		pEditPopMemberInfo.m_sWorkPhone = (LPCTSTR)pDlgMemberInfo.m_sWorkPhone;
		pEditPopMemberInfo.m_sEmail = (LPCTSTR)pDlgMemberInfo.m_sEmail;
		pEditPopMemberInfo.m_sFax = (LPCTSTR)pDlgMemberInfo.m_sFax;
		pEditPopMemberInfo.m_sAddress = (LPCTSTR)pDlgMemberInfo.m_sAddress;
		pEditPopMemberInfo.m_sDescription = (LPCTSTR)pDlgMemberInfo.m_sDescription;
		pEditPopMemberInfo.m_nGender = (EB_GENDER_TYPE)pDlgMemberInfo.m_nGender;
		pEditPopMemberInfo.m_nBirthday = pDlgMemberInfo.m_nBirthday;
		theEBAppClient.EB_EditMember(&pEditPopMemberInfo);
	}
#endif
}

void CDlgMyEnterprise::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_EDIT_MEMBERINFO == nIDEvent)
	{
		KillTimer(TIMERID_EDIT_MEMBERINFO);
		EditEmployeeInfo(m_treeEnterprise.GetTrackItem());
	}else if (TIMERID_CHECK_ITEM_HOT==nIDEvent)
	{
		KillTimer(TIMERID_CHECK_ITEM_HOT);
		if (m_hCurrentHotItem!=NULL && m_hCurrentHotItem==m_treeEnterprise.GetTrackItem())
		{
			this->PostMessage(WM_ITEM_TRACK_HOT,(WPARAM)m_treeEnterprise.GetTrackItem(),(LPARAM)&m_treeEnterprise);
		}
	}
	__super::OnTimer(nIDEvent);
}
void CDlgMyEnterprise::OnGroupShare()
{
	const HTREEITEM hSelItem = m_treeEnterprise.GetSelectedItem();
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeEnterprise.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_GROUP) return;
	theApp.OpenGroupShareWindow(pTreeItemInfo->m_sGroupCode,this->GetParent());
}
