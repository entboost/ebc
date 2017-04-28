// DlgUserList.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgUserList.h"
#include "DlgViewContactInfo.h"

#define TIMERID_EDIT_MEMBERINFO 100
#define TIMERID_CHECK_ITEM_HOT		202
#define TIMERID_LOAD_USERLIST		203

// CDlgUserList dialog

IMPLEMENT_DYNAMIC(CDlgUserList, CEbDialogBase)

CDlgUserList::CDlgUserList(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgUserList::IDD, pParent)
{
	m_pViewContactInfo = NULL;
	m_hCurrentHotItem = NULL;
}

CDlgUserList::~CDlgUserList()
{
}

void CDlgUserList::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_USERS, m_treeUsers);
	DDX_Control(pDX, IDC_EDIT_DEP_DESCRIPTION, m_editDescription);
}


BEGIN_MESSAGE_MAP(CDlgUserList, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	//ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_USERS, &CDlgUserList::OnTvnSelchangedTreeUsers)
	ON_WM_MOUSEMOVE()
	ON_EN_KILLFOCUS(IDC_EDIT_DEP_DESCRIPTION, &CDlgUserList::OnEnKillfocusEditDepDescription)
	ON_EN_SETFOCUS(IDC_EDIT_DEP_DESCRIPTION, &CDlgUserList::OnEnSetfocusEditDepDescription)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDblclk)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_NOTIFY(NM_CLICK, IDC_TREE_USERS, &CDlgUserList::OnNMClickTreeUsers)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_USERS, &CDlgUserList::OnNMRClickTreeUsers)
	ON_COMMAND(EB_COMMAND_EDIT_DEPARTMENT, &CDlgUserList::OnDepartmentEdit)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_DEL_EMP, &CDlgUserList::OnDepartmentDelEmp)
	ON_COMMAND(EB_COMMAND_DEPARTMENT_EDIT_EMP, &CDlgUserList::OnDepartmentEditEmp)
	ON_COMMAND(EB_COMMAND_CALL_USER, &CDlgUserList::OnCallUser)
	ON_COMMAND(EB_COMMAND_MEMBER_DEL_ADMIN, &CDlgUserList::OnMemberDelAdminLevel)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_ADMIN, &CDlgUserList::OnMemberAddAdminLevel)
	ON_COMMAND(EB_COMMAND_MEMBER_DEL_GROUP_FORBID_SPEECH, &CDlgUserList::OnMemberDelGroupForbidSpeech)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_GROUP_FORBID_SPEECH, &CDlgUserList::OnMemberAddGroupForbidSpeech)
	ON_COMMAND(EB_COMMAND_MEMBER_DEL_FORBID_SPEECH, &CDlgUserList::OnMemberDelForbidSpeech)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_10, &CDlgUserList::OnMemberAddForbidSpeech_10)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_60, &CDlgUserList::OnMemberAddForbidSpeech_60)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_720, &CDlgUserList::OnMemberAddForbidSpeech_720)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_1440, &CDlgUserList::OnMemberAddForbidSpeech_1440)
	ON_COMMAND(EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_0, &CDlgUserList::OnMemberAddForbidSpeech_0)
	ON_COMMAND(EB_MSG_VIEW_MSG_RECORD, &CDlgUserList::OnViewMsgRecord)
END_MESSAGE_MAP()


// CDlgUserList message handlers

void CDlgUserList::OnDestroy()
{
	m_btnCallTrack.DestroyWindow();
	m_btnEditEmpTrack.DestroyWindow();
	CEbDialogBase::OnDestroy();
	if (m_pViewContactInfo)
	{
		m_pViewContactInfo->DestroyWindow();
		delete m_pViewContactInfo;
		m_pViewContactInfo = NULL;
	}
}

void CDlgUserList::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_dep_des_height = 0;	//100;
	if (m_editDescription.GetSafeHwnd())
	{
		m_editDescription.MoveWindow(0, 0, cx, const_dep_des_height);
	}
	const int const_intever = 2;
	if (m_treeUsers.GetSafeHwnd())
	{
		m_treeUsers.MoveWindow(0, const_dep_des_height+const_intever, cx, cy-const_dep_des_height-const_intever);
	}
}

void CDlgUserList::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgUserList::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

#ifdef USES_EBCOM_TEST
void CDlgUserList::ChangeDepartmentInfo(IEB_GroupInfo* pGroupInfo)
{
	if (m_editDescription.GetSafeHwnd())
	{
		m_editDescription.SetWindowText(CEBString(pGroupInfo->Description.GetBSTR()).c_str());
	}
}
#else
void CDlgUserList::ChangeDepartmentInfo(const EB_GroupInfo* pGroupInfo)
{
	if (m_editDescription.GetSafeHwnd())
	{
		m_editDescription.SetWindowText(pGroupInfo->m_sDescription.c_str());
	}
}
#endif

void CDlgUserList::LineStateChange(eb::bigint nUserId, EB_USER_LINE_STATE bLineState)
{
	CTreeItemInfo::pointer pTreeItemInfo;
	if (m_pUserItem.find(nUserId,pTreeItemInfo))
	{
		if (pTreeItemInfo->m_dwItemData != bLineState)
		{
			pTreeItemInfo->m_dwItemData = bLineState;
			m_treeUsers.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
		}
	}
}

void CDlgUserList::OnExitUser(eb::bigint nUserId,bool bExitDep)
{
	if (!bExitDep)
	{
		bool bIsDepCall = this->m_pCallInfo.m_sGroupCode>0;
		if (bIsDepCall)
			return;
	}
	CTreeItemInfo::pointer pTreeItemInfo;
	if (m_pUserItem.find(nUserId,pTreeItemInfo,true))
	{
		m_treeUsers.DeleteItem(pTreeItemInfo->m_hItem);
	}
	////UpdateUserInfo();
	//return;
	//m_treeUsers.InsertItem(

	//int nindex = m_listUsers.FindString(0, sAccount);
	//if (nindex >= 0)
	//	m_listUsers.DeleteString(nindex);
}
void CDlgUserList::OnEnterUser(eb::bigint nUserId, bool bSort)
{
	if (!m_pUserItem.exist(nUserId))
	{
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_MemberInfo> pEBMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(this->m_pCallInfo.m_sGroupCode,nUserId);
		OnUserEmpInfo(pEBMemberInfo,bSort);
#else
		EB_MemberInfo pMemberInfo;
		if (theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,this->m_pCallInfo.m_sGroupCode,nUserId))
		{
			OnUserEmpInfo(&pMemberInfo,bSort);
		}
#endif
	}
}

#ifdef USES_EBCOM_TEST
void CDlgUserList::OnUserEmpInfo(IEB_MemberInfo* pMemberInfo, bool bSort)
{
	if (pMemberInfo==NULL) return;
	const eb::bigint sGroupCode(pMemberInfo->GroupCode);
	const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
	const eb::bigint sMemberUserId = pMemberInfo->MemberUserId;
	const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());
	const eb::bigint sMemberCode(pMemberInfo->MemberCode);
	const CEBString sJobTitle(pMemberInfo->JobTitle.GetBSTR());
	CTreeItemInfo::pointer pTreeItemInfo;
	CString sText;
	if (sJobTitle.empty())
		sText.Format(_T("%s"),sUserName.c_str());
	else
		sText.Format(_T("%s-%s"),sUserName.c_str(),sJobTitle.c_str());
	if (!m_pUserItem.find(sMemberUserId,pTreeItemInfo))
	{
		HTREEITEM hItem = m_treeUsers.InsertItem(sText);
		pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hItem);
		m_treeUsers.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
		m_pUserItem.insert(sMemberUserId,pTreeItemInfo);
	}else
	{
		m_treeUsers.SetItemText(pTreeItemInfo->m_hItem,sText);
	}
	pTreeItemInfo->m_sGroupCode = sGroupCode;
	pTreeItemInfo->m_sMemberCode = sMemberCode;
	pTreeItemInfo->m_nUserId = sMemberUserId;
	pTreeItemInfo->m_sAccount = sMemberAccount;
	pTreeItemInfo->m_sName = sUserName;
	pTreeItemInfo->m_dwItemData = pMemberInfo->LineState;

	if (bSort)
		m_treeUsers.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
}
#else
int CDlgUserList::OnUserEmpInfo(const EB_MemberInfo* pMemberInfo, bool bSort)
{
	if (pMemberInfo==NULL) return -1;
	int nResult = 0;
	CTreeItemInfo::pointer pTreeItemInfo;
	CString sText;
	if (pMemberInfo->m_sJobTitle.empty())
		sText.Format(_T("%s"),pMemberInfo->m_sUserName.c_str());
	else
		sText.Format(_T("%s-%s"),pMemberInfo->m_sUserName.c_str(),pMemberInfo->m_sJobTitle.c_str());
	if (!m_pUserItem.find(pMemberInfo->m_nMemberUserId,pTreeItemInfo))
	{
		nResult = 1;
		HTREEITEM hItem = m_treeUsers.InsertItem(sText);
		pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hItem);
		m_treeUsers.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
		m_pUserItem.insert(pMemberInfo->m_nMemberUserId,pTreeItemInfo);
	}else
	{
		m_treeUsers.SetItemText(pTreeItemInfo->m_hItem,sText);
	}
	pTreeItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
	pTreeItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
	pTreeItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
	pTreeItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
	pTreeItemInfo->m_sName = pMemberInfo->m_sUserName;
	pTreeItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
	pTreeItemInfo->m_nIndex = pMemberInfo->m_nDisplayIndex;
	if ((pMemberInfo->m_nManagerLevel&EB_LEVEL_FORBID_SPEECH)==0)
		pTreeItemInfo->m_nExtData &= ~CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
	else
		pTreeItemInfo->m_nExtData |= CTreeItemInfo::ITEM_EXT_DATA_FORBID_SPEECH;
	if (theApp.IsEnterpriseCreateUserId(pMemberInfo->m_nMemberUserId))
		pTreeItemInfo->m_nSubType = 11;
	else if (theEBAppClient.EB_IsGroupCreator(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
		pTreeItemInfo->m_nSubType = 10;
	else if (theEBAppClient.EB_IsGroupAdminLevel(pMemberInfo->m_sGroupCode, pMemberInfo->m_nMemberUserId))
		pTreeItemInfo->m_nSubType = 9;
	else if (theApp.GetLogonUserId()==pMemberInfo->m_nMemberUserId)
		pTreeItemInfo->m_nSubType = 1;
	else
		pTreeItemInfo->m_nSubType = 0;

	if (bSort)
		m_treeUsers.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	return nResult;
}
#endif

void CDlgUserList::UpdateUserInfo(void)
{
	m_pUserItem.clear();
	m_treeUsers.DeleteAllItems();

#ifdef USES_EBCOM_TEST
	_variant_t pCallMemberList = theEBClientCore->EB_GetCallUserIdList(m_pCallInfo.GetCallId());
	if (pCallMemberList.vt!=VT_EMPTY && pCallMemberList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pCallMemberList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_UI8)
				continue;
			// ??? 是否用户进入
			OnEnterUser(var.ullVal);
			//const CEBString sMemberAccount(var.bstrVal);
			//OnEnterUser(sMemberAccount.c_str());
		}
	}
#else
	std::vector<eb::bigint> pCallMemberList;
	theEBAppClient.EB_GetCallUserIdList(m_pCallInfo.GetCallId(),pCallMemberList);
	for (size_t i=0; i<pCallMemberList.size(); i++)
	{
		const eb::bigint sAccount = pCallMemberList[i];
		const bool bSort = (i+1)==pCallMemberList.size()?true:false;
		OnEnterUser(sAccount,bSort);
	}
#endif
}

void CDlgUserList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}
void CDlgUserList::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgUserList::SetCtrlColor(bool bInvalidate)
{
	if (m_pViewContactInfo!=NULL && m_pViewContactInfo->GetSafeHwnd()!=NULL)
		m_pViewContactInfo->SetCtrlColor();

	//m_treeUsers.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());
	//if (bInvalidate)
	//	this->Invalidate();
}

BOOL CDlgUserList::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetTransparentType(CEbDialogBase::TT_DLG);
	this->SetMouseMove(FALSE);
	this->EnableToolTips();
	//this->SetToolTipText(IDC_TREE_USERS,_T("可以选择私聊"));	// 显示的位置不好

#ifdef USES_EBCOM_TEST
	m_pGroupInfo = theEBClientCore->EB_GetGroupInfo(m_pCallInfo.m_sGroupCode);
	if (m_pGroupInfo != NULL)
	{
		m_editDescription.SetWindowText(CEBString(m_pGroupInfo->Description.GetBSTR()).c_str());
	}
#else
	if (theEBAppClient.EB_GetGroupInfo(m_pCallInfo.m_sGroupCode,&m_pGroupInfo))
	{
		m_editDescription.SetWindowText(m_pGroupInfo.m_sDescription.c_str());
	}
#endif

	m_btnCallTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeUsers, 0xffff);
	m_btnCallTrack.SetAutoSize(false);
	m_btnCallTrack.SetAutoFocus(true);
	m_btnCallTrack.Load(IDB_PNG_HOT_CALLS);
	m_btnCallTrack.SetToolTipText(_T("打开会话"));
	m_btnEditEmpTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeUsers, 0xffff);
	m_btnEditEmpTrack.SetAutoSize(false);
	m_btnEditEmpTrack.SetAutoFocus(true);
	m_btnEditEmpTrack.Load(IDB_PNG_HOT_EDITS);
	m_btnEditEmpTrack.SetToolTipText(_T("修改我的名片"));

	SetCtrlColor(false);
	m_treeUsers.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeUsers.SetCallback((CTreeCallback*)&theApp);
	m_treeUsers.ModifyStyle(0,TVS_SINGLEEXPAND);
	m_treeUsers.SetItemHeight(28);
	m_treeUsers.SetIconSize(26,26);
	m_treeUsers.SetBkGradients(theDefaultFlatBgColor,theDefaultFlatBgColor);
	//m_treeUsers.SetIntervalLine(true,theDefaultBtnWhiteColor,theDefaultFlatBgColor);

	SetTimer(TIMERID_LOAD_USERLIST,1,NULL);
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//bool CDlgUserList::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return false;
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
//int CDlgUserList::GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, HICON& pOutIcon) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return 0;
//
//#ifdef USES_EBCOM_TEST
//	EB_USER_LINE_STATE pOutLineState = (EB_USER_LINE_STATE)theEBClientCore->EB_GetMemberLineState(pTreeItemInfo->m_sMemberCode);
//	if (pOutLineState == EB_LINE_STATE_UNKNOWN)
//		return 0;
//#else
//	EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
//	if (!theEBAppClient.EB_GetMemberLineState(pTreeItemInfo->m_sMemberCode,pOutLineState))
//		return 0;
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

void CDlgUserList::SelectItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hItem);
	if (pTreeItemInfo==NULL) return;
	if (pTreeItemInfo->m_nUserId == theApp.GetLogonUserId())
	{
		return;
	}
	// this->chat_right->dialog
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
	if (pMemberInfo == NULL) return;
	this->GetParent()->GetParent()->SendMessage(EB_MSG_SELECTED_EMP,(WPARAM)(IEB_MemberInfo*)pMemberInfo,0);
#else
	EB_MemberInfo pMemberInfo;
	if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) return;
	this->GetParent()->GetParent()->SendMessage(EB_MSG_SELECTED_EMP,(WPARAM)&pMemberInfo,0);
#endif
}
void CDlgUserList::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hItem);
	if (pTreeItemInfo==NULL) return;
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#else
	theEBAppClient.EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#endif
}

//void CDlgUserList::OnTvnSelchangedTreeUsers(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//	//SelectItem(pNMTreeView->itemNew.hItem);
//}

void CDlgUserList::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//if (m_editDescription.GetSafeHwnd() && this->GetFocus() != &m_editDescription)
	//{
	//	EB_GroupInfo::pointer pGroupInfo = theEBAppClient.GetGroupInfo(m_pCallInfo->m_sGroupCode);
	//	if (pGroupInfo.get() != NULL && pGroupInfo->m_sCreator == theEBAppClient.EB_GetLogonAccount())
	//	{
	//		// 部门管理者才有权限修改
	//		static bool theDescIsEnableWindow = false;
	//		CPoint pos;
	//		GetCursorPos(&pos);
	//		ScreenToClient(&pos);
	//		CRect rect;
	//		m_editDescription.GetWindowRect(&rect);
	//		this->ScreenToClient(&rect);
	//		if (rect.PtInRect(pos))
	//		{
	//			theDescIsEnableWindow = true;
	//			//m_editDescription.EnableWindow(TRUE);
	//			m_editDescription.SetReadOnly(FALSE);
	//			//::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	//			m_editDescription.SetFocus();
	//			m_editDescription.SetSel(0,-1);
	//			return;
	//		}else if (theDescIsEnableWindow)
	//		{
	//			theDescIsEnableWindow = false;
	//			RefreshEditDescription();
	//		}
	//	}
	//}

	__super::OnMouseMove(nFlags, point);
}

void CDlgUserList::RefreshEditDescription(void)
{
	m_editDescription.SetReadOnly(TRUE);
	CRect rect;
	m_editDescription.GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	this->InvalidateRect(rect);
}

void CDlgUserList::OnEnSetfocusEditDepDescription()
{
#ifdef USES_EBCOM_TEST
	const eb::bigint nGroupCreateUserId = theEBClientCore->EB_GetGroupCreator(m_pCallInfo.m_sGroupCode);
#else
	eb::bigint nGroupCreateUserId=0;
	if (!theEBAppClient.EB_GetGroupCreator(m_pCallInfo.m_sGroupCode,nGroupCreateUserId)) return;
#endif
	if (nGroupCreateUserId == theApp.GetLogonUserId())
	{
		if (this->GetFocus() != &m_editDescription)
		{
			m_editDescription.SetFocus();
			m_editDescription.SetSel(0,-1);
		}
		m_editDescription.SetReadOnly(FALSE);
	}
}

void CDlgUserList::OnEnKillfocusEditDepDescription()
{
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(m_pCallInfo.m_sGroupCode);
	if (pGroupInfo != NULL)
	{
		m_editDescription.SetWindowText(CEBString(pGroupInfo->Description.GetBSTR()).c_str());
	}
#else
	EB_GroupInfo pGroupInfo;
	if (theEBAppClient.EB_GetGroupInfo(m_pCallInfo.m_sGroupCode,&pGroupInfo))
	{
		m_editDescription.SetWindowText(pGroupInfo.m_sDescription.c_str());
	}
#endif
	RefreshEditDescription();
}

BOOL CDlgUserList::PreTranslateMessage(MSG* pMsg)
{
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
			}
		}
	}

	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnCallTrack.GetSafeHwnd())
	{
		CallItem(m_treeUsers.GetTrackItem());
		//if (m_treeUsers.GetTrackItem()==NULL)
		//	this->GetParent()->PostMessage(EB_MSG_SELECTED_EMP,0,0);
		//else
		//	SelectItem(m_treeUsers.GetTrackItem());
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeUsers.GetSafeHwnd())
	{
		CallItem(m_treeUsers.GetSelectedItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnEditEmpTrack.GetSafeHwnd())
	{
		SetTimer(TIMERID_EDIT_MEMBERINFO, 1, NULL);
	}else if (pMsg->message == WM_LBUTTONDOWN && pMsg->hwnd == m_editDescription.GetSafeHwnd())
	{
		OnEnSetfocusEditDepDescription();
	}else if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_editDescription.GetSafeHwnd())
	{
		if (pMsg->wParam == VK_RETURN)
		{
			// 回车，保存
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(m_pCallInfo.m_sGroupCode);
			if (pGroupInfo != NULL)
			{
				CString sCurrentDescription;
				m_editDescription.GetWindowText(sCurrentDescription);
				if (CEBString(pGroupInfo->Description.GetBSTR()) != (LPCTSTR)sCurrentDescription)
				{
					if (sCurrentDescription.IsEmpty())
						sCurrentDescription = _T("''");
					pGroupInfo->Description = (LPCTSTR)sCurrentDescription;
					theEBClientCore->EB_EditGroup(pGroupInfo);
				}
			}
#else
			EB_GroupInfo pGroupInfo;
			if (theEBAppClient.EB_GetGroupInfo(m_pCallInfo.m_sGroupCode,&pGroupInfo))
			{
				CString sCurrentDescription;
				m_editDescription.GetWindowText(sCurrentDescription);
				if (pGroupInfo.m_sDescription != (LPCTSTR)sCurrentDescription)
				{
					if (sCurrentDescription.IsEmpty())
						sCurrentDescription = _T("''");
					EB_GroupInfo pEditPopDepartment(pGroupInfo);
					pEditPopDepartment.m_sDescription = (LPCTSTR)sCurrentDescription;
					theEBAppClient.EB_EditGroup(&pEditPopDepartment);
				}
			}
#endif
			RefreshEditDescription();
		}else if (pMsg->wParam == VK_ESCAPE)
		{
			// 退出，取消
			OnEnKillfocusEditDepDescription();
		}
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}

LRESULT CDlgUserList::OnTreeItemDblclk(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hDblClkItem = (HTREEITEM)wp;
	this->CallItem(hDblClkItem);
	return 0;
}

LRESULT CDlgUserList::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	m_hCurrentHotItem = item;
	if (item == NULL || !this->IsWindowVisible())
	{
		if (m_pViewContactInfo!=NULL)// && m_pViewContactInfo->IsWindowVisible())
		{
			m_pViewContactInfo->SetMoveLeave();
			//m_pViewContactInfo->HideReset();
		}
		m_btnCallTrack.ShowWindow(SW_HIDE);
		m_btnEditEmpTrack.ShowWindow(SW_HIDE);
	}else// if (m_btnCallTrack.GetSafeHwnd() != NULL)
	{
		if (m_pViewContactInfo == NULL)
		{
			m_pViewContactInfo = new CDlgViewContactInfo(this);
			m_pViewContactInfo->Create(CDlgViewContactInfo::IDD,this);
		}
		const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(item);
		if (pTreeItemInfo == NULL)
		{
			if (m_pViewContactInfo!=0)// && m_pViewContactInfo->IsWindowVisible())
			{
				m_pViewContactInfo->SetMoveLeave();
				//m_pViewContactInfo->HideReset();
			}
			return 1;
		}
		CPoint pos;
		GetCursorPos(&pos);
		SetTimer(TIMERID_CHECK_ITEM_HOT,1000,NULL);
		CRect rectItem;
		m_treeUsers.GetItemRect(item, &rectItem, TRUE);
		CRect rectHead(rectItem);
		rectHead.right = rectHead.left + m_treeUsers.GetIconWidth();
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
				rectViewContactInfo.left = rect.right+5;
				rectViewContactInfo.right = rectViewContactInfo.left+const_dlg_width;
			}
			m_pViewContactInfo->MoveWindow(&rectViewContactInfo);
			m_pViewContactInfo->SetCircle();

#ifdef USES_EBCOM_TEST
			CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode);
			if (pGroupInfo == NULL) return 1;
			CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
			if (pMemberInfo == NULL) return 1;
			m_pViewContactInfo->SetEmployeeInfo(pMemberInfo,pGroupInfo);
#else
			EB_GroupInfo pGroupInfo;
			if (!theEBAppClient.EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode,&pGroupInfo)) return 1;
			EB_MemberInfo pMemberInfo;
			if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) return 1;
			m_pViewContactInfo->SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
#endif
			m_pViewContactInfo->SetMoveEnter();
			//m_pViewContactInfo->ShowWindow(SW_SHOW);
			//m_pViewContactInfo->SetCheckLeave();
		}else// if (m_pViewContactInfo->IsWindowVisible())
		{
			m_pViewContactInfo->SetMoveLeave();
			//m_pViewContactInfo->ShowWindow(SW_HIDE);
		}

		const int nTop = rectItem.top;
		m_treeUsers.GetClientRect(&rectItem);
		const int nRight = rectItem.right;
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
	}
	return 0;
}

void CDlgUserList::OnNMClickTreeUsers(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CRect rect;
	POINT pt;
	TVHITTESTINFO pHitTestInfo;
	GetCursorPos(&pt);            //鼠标位置
	m_treeUsers.GetWindowRect(&rect); //窗口位置 
	pHitTestInfo.pt.x = pt.x - rect.left;  //鼠标相对x位置
	pHitTestInfo.pt.y = pt.y - rect.top;   //鼠标相对y位置
	HTREEITEM hTrackItem = m_treeUsers.HitTest(&pHitTestInfo);     //停留测试
	if (hTrackItem==NULL)
		this->GetParent()->GetParent()->PostMessage(EB_MSG_SELECTED_EMP,0,0);	// this->chatright->dialog
		//this->GetParent()->PostMessage(EB_MSG_SELECTED_EMP,0,0);
	else
		SelectItem(hTrackItem);
}

void CDlgUserList::EditEmployeeInfo(HTREEITEM hSelItem)
{
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
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

void CDlgUserList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TIMERID_LOAD_USERLIST==nIDEvent)
	{
		KillTimer(TIMERID_LOAD_USERLIST);
		UpdateUserInfo();
	}else if (TIMERID_EDIT_MEMBERINFO == nIDEvent)
	{
		KillTimer(TIMERID_EDIT_MEMBERINFO);
		EditEmployeeInfo(m_treeUsers.GetTrackItem());
	}else if (TIMERID_CHECK_ITEM_HOT==nIDEvent)
	{
		KillTimer(TIMERID_CHECK_ITEM_HOT);
		if (m_hCurrentHotItem!=NULL && m_hCurrentHotItem==m_treeUsers.GetTrackItem())
		{
			this->PostMessage(WM_ITEM_TRACK_HOT,(WPARAM)m_treeUsers.GetTrackItem(),(LPARAM)&m_treeUsers);
		}
	}
	__super::OnTimer(nIDEvent);
}

void CDlgUserList::OnNMRClickTreeUsers(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;


	if (m_pCallInfo.m_sGroupCode==0)
		return;

#ifdef USES_EBCOM_TEST
	if (m_pGroupInfo==NULL)
		return;
	//const eb::bigint nGroupCreateUserId = m_pGroupInfo->CreateUserId;
	const EB_GROUP_TYPE nGroupType = m_pGroupInfo->GroupType;
#else
	//const eb::bigint nGroupCreateUserId = m_pGroupInfo.m_nCreateUserId;
	const EB_GROUP_TYPE nGroupType = m_pGroupInfo.m_nGroupType;
#endif

	const HTREEITEM hSelItem = m_treeUsers.SelectHitTest();
	//const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;

	CNewMenu pPopupMenuForbidSpeech;
	CNewMenu m_menu2;
	m_menu2.CreatePopupMenu();
	m_menu2.SetBitmapBackground(RGB(192,192,192));
	bool bNeedSeparator = false;
	if (pTreeItemInfo->m_nUserId == theApp.GetLogonUserId())
	{
		m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改我的名片"));
		//switch (nGroupType)
		//{
		//case EB_GROUP_TYPE_DEPARTMENT:
		//case EB_GROUP_TYPE_PROJECT:
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改我的名片"));
		//	break;
		//case EB_GROUP_TYPE_GROUP:
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改群名片"));
		//	break;
		//default:
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改讨论组名片"));
		//	break;
		//}
	}else
	{
		// 判断聊天记录
		CString sSql;
		sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) LIMIT 1"),
			pTreeItemInfo->m_nUserId,pTreeItemInfo->m_nUserId);
		if (theApp.m_pBoUsers->select(sSql)>0)
		{
			m_menu2.InsertODMenu(-1,_T("聊天记录"),MF_BYPOSITION,EB_MSG_VIEW_MSG_RECORD,IDB_BITMAP_MENU_MSG);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_MSG_VIEW_MSG_RECORD,_T("聊天记录"));
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_MSG_RECORD,_T("清空聊天记录"));
			bNeedSeparator = true;
		}

		//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CALL_USER,_T("打开会话"));
		if ((theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) ||
			(theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)))
		//if (nGroupCreateUserId==theApp.GetLogonUserId())
		{
			if (bNeedSeparator)
				m_menu2.AppendMenu(MF_SEPARATOR);
			bNeedSeparator = true;
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_EDIT_EMP,_T("修改成员名片"));
			m_menu2.AppendODMenu(_T("移除成员(&D)"),MF_BYPOSITION,EB_COMMAND_DEPARTMENT_DEL_EMP,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DEPARTMENT_DEL_EMP,_T("移除成员"));

			int nForbidMinutes = 0;
			if (theEBAppClient.EB_IsMemberForbidSpeech(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId,nForbidMinutes))
			{
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_DEL_FORBID_SPEECH,_T("解除禁言"));
			}else
			{
				pPopupMenuForbidSpeech.CreatePopupMenu();
				pPopupMenuForbidSpeech.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_10,_T("10分钟"));
				pPopupMenuForbidSpeech.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_60,_T("1小时"));
				pPopupMenuForbidSpeech.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_720,_T("12小时"));
				pPopupMenuForbidSpeech.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_1440,_T("1天"));
				pPopupMenuForbidSpeech.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_FORBID_SPEECH_0,_T("永久"));
				m_menu2.InsertODMenu(-1,_T("禁言"),MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuForbidSpeech.m_hMenu,IDB_BITMAP_FORBID_SPEECH);
			}
		}
	}
	if (theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode))
	//if (nGroupCreateUserId==theApp.GetLogonUserId())
	{
		if (bNeedSeparator)
			m_menu2.AppendMenu(MF_SEPARATOR);
		if (theEBAppClient.EB_IsGroupForbidSpeech(pTreeItemInfo->m_sGroupCode))
			m_menu2.InsertODMenu(-1,_T("解除群禁言"),MF_BYPOSITION,EB_COMMAND_MEMBER_DEL_GROUP_FORBID_SPEECH,IDB_BITMAP_FORBID_SPEECH);
		else
			m_menu2.InsertODMenu(-1,_T("群禁言"),MF_BYPOSITION,EB_COMMAND_MEMBER_ADD_GROUP_FORBID_SPEECH,IDB_BITMAP_FORBID_SPEECH);
		CString sText;
		sText.Format(_T("修改%s资料"),GetGroupTypeText((EB_GROUP_TYPE)nGroupType,true));
		m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,sText);
		//switch (nGroupType)
		//{
		//case EB_GROUP_TYPE_DEPARTMENT:
		//case EB_GROUP_TYPE_PROJECT:
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,_T("修改部门资料"));
		//	break;
		//case EB_GROUP_TYPE_GROUP:
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,_T("修改群资料"));
		//	break;
		//default:
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DEPARTMENT,_T("修改讨论组资料"));
		//	break;
		//}
	}
	// 我是群主，选择对象不是群主
	if (theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode) && !theEBAppClient.EB_IsGroupCreator(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
	{
		if (bNeedSeparator)
			m_menu2.AppendMenu(MF_SEPARATOR);
		if (theEBAppClient.EB_IsGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId))
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_DEL_ADMIN,_T("取消管理员资格"));
		else
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MEMBER_ADD_ADMIN,_T("加为管理员"));
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

void CDlgUserList::OnDepartmentEdit()
{
	if (m_pCallInfo.m_sGroupCode==0) return;
	theApp.EditGroupInfo(this,m_pCallInfo.m_sGroupCode);
}

void CDlgUserList::OnDepartmentDelEmp()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
#ifdef USES_EBCOM_TEST
	if (m_pGroupInfo == NULL) return;
	const CEBString sGroupName(m_pGroupInfo->GroupName.GetBSTR());
	const eb::bigint nGroupCreateUserId = m_pGroupInfo->CreateUserId;
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
	if (pTreeItemInfo->m_nUserId == m_pGroupInfo.m_nCreateUserId)
	{
		// 不能移除管理者自己
		return;
	//}else if (m_pGroupInfo.m_nCreateUserId!=theApp.GetLogonUserId())
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

void CDlgUserList::OnDepartmentEditEmp()
{
	EditEmployeeInfo(m_treeUsers.GetSelectedItem());
}
void CDlgUserList::OnCallUser()
{
	CallItem(m_treeUsers.GetSelectedItem());
}

void CDlgUserList::OnMemberDelAdminLevel()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_DelGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
}
void CDlgUserList::OnMemberAddAdminLevel()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_AddGroupAdminLevel(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
}

void CDlgUserList::OnMemberDelGroupForbidSpeech()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
	if (pTreeItemInfo->m_sGroupCode>0)
	{
		// 解除群禁言
		theEBAppClient.EB_SetGroupForbidSpeech(pTreeItemInfo->m_sGroupCode,false);
	}
}
void CDlgUserList::OnMemberAddGroupForbidSpeech()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
	if (pTreeItemInfo->m_sGroupCode>0)
	{
		// 群组禁言
		theEBAppClient.EB_SetGroupForbidSpeech(pTreeItemInfo->m_sGroupCode,true);
	}
}

void CDlgUserList::OnMemberDelForbidSpeech()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		theEBAppClient.EB_SetMemberForbidSpeech(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId, false);
	}
}
void CDlgUserList::OnMemberAddForbidSpeech_10()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_SetMemberForbidSpeech(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId, true, 10);
}
void CDlgUserList::OnMemberAddForbidSpeech_60()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_SetMemberForbidSpeech(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId, true, 60);
}
void CDlgUserList::OnMemberAddForbidSpeech_720()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_SetMemberForbidSpeech(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId, true, 720);
}
void CDlgUserList::OnMemberAddForbidSpeech_1440()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_MEMBER) return;
	theEBAppClient.EB_SetMemberForbidSpeech(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId, true, 1440);
}
void CDlgUserList::OnMemberAddForbidSpeech_0()
{
	const HTREEITEM hSelItem = m_treeUsers.GetSelectedItem();
	if (hSelItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		theEBAppClient.EB_SetMemberForbidSpeech(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId, true, 0);	// 永久禁言
	}
}

void CDlgUserList::OnViewMsgRecord()
{
	const HTREEITEM hItem = m_treeUsers.GetSelectedItem();
	if (hItem==NULL) return;
	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeUsers.GetItemData(hItem);
	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_MEMBER)
	{
		bIsAccount = true;
		sId = pTreeItemInfo->m_nUserId;
		sName = pTreeItemInfo->m_sName;
	//}else if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP)
	//{
	//	sId = pTreeItemInfo->m_sGroupCode;
	//	sName = pTreeItemInfo->m_sName;
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
