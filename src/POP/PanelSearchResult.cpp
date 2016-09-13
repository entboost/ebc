// PanelSearchResult.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "PanelSearchResult.h"
#include "DlgViewContactInfo.h"

#define TIMERID_CALL_SEARCH_TRACK				0x134
#define TIMERID_CALL_SEARCH_SEL_CHANGE	0x135
#define TIMERID_CALL_SEARCH_SELECTED		0x136

const tstring theAppGroupResultName = _T("集成应用搜索：");
const tstring theUrlGroupName("[* *]");
const CString theUrlGroupResultName = _T("最新浏览记录：");

// CPanelSearchResult dialog

IMPLEMENT_DYNAMIC(CPanelSearchResult, CEbDialogBase)

CPanelSearchResult::CPanelSearchResult(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPanelSearchResult::IDD, pParent)
	, m_pParentMsg(pParent)
{
	m_pViewContactInfo = NULL;

}

CPanelSearchResult::~CPanelSearchResult()
{
}

void CPanelSearchResult::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SEARCH, m_treeSearch);
}


BEGIN_MESSAGE_MAP(CPanelSearchResult, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDoubleClick)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SEARCH, &CPanelSearchResult::OnNMClickTreeSearch)
END_MESSAGE_MAP()


// CPanelSearchResult message handlers

BOOL CPanelSearchResult::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	this->SetMouseMove(FALSE);

	m_btnSearchTrackDel.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSearch, 0xffff);
	m_btnSearchTrackDel.SetAutoSize(false);
	m_btnSearchTrackDel.Load(IDB_PNG_HOT_DELETE);
	m_btnSearchTrackDel.SetToolTipText(_T("删除历史记录"));

	m_btnSearchTrackCall.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSearch, 0xffff);
	m_btnSearchTrackCall.SetAutoSize(false);
	m_btnSearchTrackCall.Load(IDB_PNG_HOT_CALL);
	m_btnSearchTrackCall.SetToolTipText(_T("打开会话"));

	m_treeSearch.SetItemHeight(40);
	m_treeSearch.SetIconSize(32,32);
	m_treeSearch.SetCallback(&theApp);
	m_treeSearch.ShowWindow(SW_SHOW);
	m_treeSearch.SetRectangle(theDefaultFlatLineColor);

	this->SetCircle(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelSearchResult::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	if (m_treeSearch.GetSafeHwnd()!=0)
	{
		m_treeSearch.MoveWindow(0,0,cx,cy);
	}
}

void CPanelSearchResult::DeleteAllResult(void)
{
	m_pSubFuncItem.clear();
	m_pUrlItem.clear();
	m_pContactItem.clear();
	m_pEntGroupMemberItem.clear();
	m_pMyGroupMemberItem.clear();
	m_treeSearch.DeleteAllItems();

}

CTreeItemInfo::pointer CPanelSearchResult::GetAppDirItem(void)
{
	CTreeItemInfo::pointer pContactGroupInfo;
	if (!m_pSubFuncItem.find(0,pContactGroupInfo))
	{
		HTREEITEM hEmpItem = m_treeSearch.InsertItem(theAppGroupResultName.c_str());
		pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
		pContactGroupInfo->m_nSubType = 1;	// bAppFrameShowed sort
		pContactGroupInfo->m_dwItemData = 3;	// 保证永远排前面
		//pContactGroupInfo->m_dwItemData = theApp.GetWorkFrameShowed()?3:0;	// 保证永远排前面
		m_pSubFuncItem.insert(0,pContactGroupInfo);
		m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
		m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	}
	return pContactGroupInfo;
}

//void ShowSearchResult(void)
//{
//	// ???
//}

size_t CPanelSearchResult::InsertAppItem(const EB_SubscribeFuncInfo& pFuncInfo)
{
	CTreeItemInfo::pointer pContactGroupInfo = GetAppDirItem();
	if (!m_pSubFuncItem.exist(pFuncInfo.m_nSubscribeId))
	{
		const std::string sTitle(pFuncInfo.m_sFunctionName);
		CString sSearchName;
		sSearchName.Format(_T("%s\nCLR=%d;%s"),sTitle.c_str(),(int)RGB(0,128,255),pFuncInfo.m_sDescription.c_str());
		HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
		CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_SUBINFO,hContactItem);
		pContactItemInfo->m_sName = sTitle;
		pContactItemInfo->m_sId = pFuncInfo.m_nSubscribeId;
		pContactItemInfo->m_sGroupName = pFuncInfo.m_sResFile;
		pContactItemInfo->m_sAccount = pFuncInfo.m_sFunctionName;
		m_pSubFuncItem.insert(pContactItemInfo->m_sId,pContactItemInfo);
		m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
		//ShowSearchResult();
		const int nGroupResultSize = m_pSubFuncItem.size()-1;
		if (nGroupResultSize==1)
		{
			//if (this->GetWorkFrameShowed() && m_treeSearch.GetSelectedItem()==NULL)
			if (m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hContactItem,TVGN_CARET);
			m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
		}
		sSearchName.Format(_T("%s%d"),theAppGroupResultName.c_str(),nGroupResultSize);
		m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
		//if (nGroupResultSize>=15) break;
	}
	return m_pSubFuncItem.size(false);
}

void CPanelSearchResult::InsertUrlItem(const tstring& sTitle, const tstring& sUrl, const tstring& sLastTime)
{
	// search
	CTreeItemInfo::pointer pContactGroupInfo;
	if (!m_pUrlItem.find(theUrlGroupName,pContactGroupInfo))
	{
		HTREEITEM hEmpItem = m_treeSearch.InsertItem(theUrlGroupResultName);
		pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
		pContactGroupInfo->m_nSubType = 1;	// bAppFrameShowed sort
		pContactGroupInfo->m_dwItemData = 2;
		m_pUrlItem.insert(theUrlGroupName,pContactGroupInfo);
		m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
		m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	}
	if (!m_pUrlItem.exist(sUrl))
	{
		CString sSearchName;
		sSearchName.Format(_T("%s (%s)\nCLR=%d;%s"),sTitle.c_str(),sLastTime.c_str(),(int)RGB(0,128,255),sUrl.c_str());
		HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
		CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_URL,hContactItem);
		pContactItemInfo->m_sAccount = sUrl;
		pContactItemInfo->m_sName = sTitle;
		m_pUrlItem.insert(pContactItemInfo->m_sAccount,pContactItemInfo);
		m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
		//ShowSearchResult();
		const int nGroupResultSize = m_pUrlItem.size()-1;
		if (nGroupResultSize==1)
		{
			if (m_treeSearch.GetSelectedItem()==NULL)
				//if (this->GetWorkFrameShowed() && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hContactItem,TVGN_CARET);
			m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
		}
		sSearchName.Format(_T("%s%d"),theUrlGroupResultName,nGroupResultSize);
		m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
		//ScrollTreeSearchToTop();
	}
}
void CPanelSearchResult::FocusTree(void)
{
	this->SetFocus();
	m_treeSearch.SetFocus();
	if (m_treeSearch.GetSelectedItem()!=0 && m_treeSearch.GetParentItem(m_treeSearch.GetSelectedItem())==0)
	{
		m_treeSearch.Expand(m_treeSearch.GetSelectedItem(),TVE_EXPAND);
	}
}

void CPanelSearchResult::DeleteItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType!=CTreeItemInfo::ITEM_TYPE_URL) return;

	tstring sUrl(pTreeItemInfo->m_sAccount);
	m_treeSearch.DeleteItem(hItem);
	if (m_pUrlItem.remove(sUrl))
	{
		CTreeItemInfo::pointer pUrlGroupInfo;
		if (m_pUrlItem.find(theUrlGroupName,pUrlGroupInfo))
		{
			if (m_pUrlItem.size()<=1)
				m_treeSearch.DeleteItem(pUrlGroupInfo->m_hItem);
			else
			{
				const int nGroupResultSize = m_pUrlItem.size()-1;
				CString sSearchName;
				sSearchName.Format(_T("%s%d"),theUrlGroupResultName,nGroupResultSize);
				m_treeSearch.SetItemText(pUrlGroupInfo->m_hItem,sSearchName);
			}
		}
	}
	CSqliteCdbc::escape_string_in(sUrl);
	sUrl = libEbc::ACP2UTF8(sUrl.c_str());

	CString sSql;
	sSql.Format(_T("DELETE FROM url_record_t WHERE url='%s'"),sUrl.c_str());
	theApp.m_pBoUsers->execute(sSql);
}
bool CPanelSearchResult::CallSelectedItem(void)
{
	return CallItem(m_treeSearch.GetSelectedItem());
}
bool CPanelSearchResult::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return false;
	//if (bFindChild)
	////if (!bSelChange)
	//{
	//	HTREEITEM hChildItem = m_treeSearch.GetChildItem(hItem);
	//	if (hChildItem!=NULL)
	//		hItem = hChildItem;
	//}
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return false;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
	{
		if (m_treeSearch.GetItemState(hItem,TVIS_EXPANDED)&TVIS_EXPANDED)
			m_treeSearch.Expand(hItem,TVE_COLLAPSE);
		else
			m_treeSearch.Expand(hItem,TVE_EXPAND);
		return false;
	}
	bool bCallResult = true;
	if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_SUBINFO)
	{
		EB_SubscribeFuncInfo pSubscribeFuncInfo;
		if (theEBAppClient.EB_GetSubscribeFuncInfo(pTreeItemInfo->m_sId,&pSubscribeFuncInfo))
		{
			bCallResult = theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
		}else
		{
			bCallResult = false;
		}
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_URL)
	{
		COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(pTreeItemInfo->m_sAccount.c_str(),"");
		pOpenAppUrlInfo->m_bSaveBrowseTitle = true;
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
		//this->AddAppUrl(true,pTreeItemInfo->m_sAccount,NULL);
	}else if (pTreeItemInfo->m_nUserId>0)
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallUserId(pTreeItemInfo->m_nUserId,0);
#else
		theEBAppClient.EB_CallUserId(pTreeItemInfo->m_nUserId,0);
#endif
	}else if (pTreeItemInfo->m_sMemberCode>0)
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#else
		theEBAppClient.EB_CallMember(pTreeItemInfo->m_sMemberCode,0);
#endif
	}else
	{
		theApp.m_pAutoCallFromUserIdList.remove(pTreeItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_CallAccount(pTreeItemInfo->m_sAccount.c_str(),0);
#else
		theEBAppClient.EB_CallAccount(pTreeItemInfo->m_sAccount.c_str(),0);
#endif
	}
	this->ShowWindow(SW_HIDE);
	this->DeleteAllResult();
	return bCallResult;
}

void CPanelSearchResult::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TIMERID_CALL_SEARCH_SELECTED:
		KillTimer(nIDEvent);
		CallItem(m_treeSearch.GetSelectedItem());
		break;
	case TIMERID_CALL_SEARCH_TRACK:
		KillTimer(nIDEvent);
		CallItem(m_treeSearch.GetTrackItem());
		break;
	case TIMERID_CALL_SEARCH_SEL_CHANGE:
		KillTimer(nIDEvent);
		CallItem(m_treeSearch.GetTrackItem());
		break;
	default:
		break;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

BOOL CPanelSearchResult::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnSearchTrackDel.GetSafeHwnd())
	{
		DeleteItem(m_treeSearch.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnSearchTrackCall.GetSafeHwnd())
	{
		if (m_pParentMsg!=NULL)
			m_pParentMsg->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,0);
		SetTimer(TIMERID_CALL_SEARCH_TRACK,1,NULL);
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeSearch.GetSafeHwnd())
	{
		SetTimer(TIMERID_CALL_SEARCH_SELECTED,1,NULL);
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_ESCAPE && pMsg->hwnd == m_treeSearch.GetSafeHwnd())
	{
		if (m_pParentMsg!=NULL)
			m_pParentMsg->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,1);
		this->ShowWindow(SW_HIDE);
		this->DeleteAllResult();
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_UP && pMsg->hwnd == m_treeSearch.GetSafeHwnd())
	{
		// 已经移到最上面
		if (m_treeSearch.GetCount()>0 && m_treeSearch.GetSelectedItem()!=NULL &&
			m_treeSearch.GetSelectedItem()==m_treeSearch.GetFirstVisibleItem() && m_treeSearch.GetParentItem(m_treeSearch.GetSelectedItem())==NULL)
		{
			if (m_pParentMsg!=NULL)
				m_pParentMsg->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,1);
		}
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}


#ifdef USES_EBCOM_TEST
void CPOPDlg::Fire_onSearchMemberInfo(IEB_GroupInfo* pGroupInfo, IEB_MemberInfo* pMemberInfo, ULONG dwParam)
{
	if (dwParam == 1)
	{
		// search
		const eb::bigint sEnterpriseCode(pGroupInfo->EnterpriseCode);
		const eb::bigint sMemberCode(pMemberInfo->MemberCode);
		const eb::bigint sMemberUserid(pMemberInfo->MemberUserId);
		const CEBString sMemberAccount(pMemberInfo->MemberAccount.GetBSTR());
		const CEBString sUserName(pMemberInfo->UserName.GetBSTR());
		if (sMemberUserid==theApp.GetLogonUserId())
			return;

		const CString sGroupResultName = pGroupInfo->m_sEnterpriseCode==0?_T("群组成员搜索结果："):_T("部门员工搜索结果：");
		const eb::bigint nGroupMemberCode = pGroupInfo->m_sEnterpriseCode==0?0:-1;
		CLockMap<eb::bigint, CTreeItemInfo::pointer>& pMemberItemList = pGroupInfo->m_sEnterpriseCode==0?m_pMyGroupMemberItem:m_pEntGroupMemberItem;

		CTreeItemInfo::pointer pMemberGroupInfo;
		if (!pMemberItemList.find(nGroupMemberCode,pMemberGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pMemberGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pMemberGroupInfo->m_dwItemData = 1;
			pMemberItemList.insert(nGroupMemberCode,pMemberGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pMemberGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
			if ((!m_bAppFrameShowed || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!pMemberItemList.exist(sMemberCode))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),sUserName.c_str(),sMemberAccount.c_str());
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sSearchName,pMemberGroupInfo->m_hItem);
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = sMemberCode;
			pEmpItemInfo->m_nUserId = sMemberUserid;
			pEmpItemInfo->m_sAccount = sMemberAccount;
			pEmpItemInfo->m_sGroupCode = pMemberInfo->GroupCode;
			pEmpItemInfo->m_sName = sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->LineState;
			//pEmpItemInfo->m_pGroupInfo = pGroupInfo;
			//pEmpItemInfo->m_pMemberInfo = pMemberInfo;
			pMemberItemList.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
			m_treeSearch.Sort(pMemberGroupInfo->m_hItem,CPOPApp::TreeCmpFunc);
			ShowSearchResult();
			const int nGroupResultSize = pMemberItemList.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pMemberGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pMemberGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
}
#else
void CPanelSearchResult::onMemberInfo(const EB_GroupInfo* pGroupInfo, const EB_MemberInfo* pMemberInfo, unsigned long dwParam)
{
	if (dwParam == 1)
	{
		// search
		if (pMemberInfo->m_nMemberUserId==theApp.GetLogonUserId())
			return;

		const CString sGroupResultName = pGroupInfo->m_sEnterpriseCode==0?_T("群组成员搜索结果："):_T("部门员工搜索结果：");
		const eb::bigint nGroupMemberCode = pGroupInfo->m_sEnterpriseCode==0?0:-1;
		CLockMap<eb::bigint, CTreeItemInfo::pointer>& pMemberItemList = pGroupInfo->m_sEnterpriseCode==0?m_pMyGroupMemberItem:m_pEntGroupMemberItem;

		CTreeItemInfo::pointer pMemberGroupInfo;
		if (!pMemberItemList.find(nGroupMemberCode,pMemberGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pMemberGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pMemberGroupInfo->m_nSubType = 1;	// bAppFrameShowed sort
			pMemberGroupInfo->m_dwItemData = 1;
			pMemberItemList.insert(nGroupMemberCode,pMemberGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pMemberGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
			if (m_treeSearch.GetSelectedItem()==NULL)
			//if ((GetMainFrameShowed() || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!pMemberItemList.exist(pMemberInfo->m_sMemberCode))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),pMemberInfo->m_sUserName.c_str(),pMemberInfo->m_sMemberAccount.c_str());
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sSearchName,pMemberGroupInfo->m_hItem);
			CTreeItemInfo::pointer pEmpItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_MEMBER,hEmpItem);
			pEmpItemInfo->m_sMemberCode = pMemberInfo->m_sMemberCode;
			pEmpItemInfo->m_nUserId = pMemberInfo->m_nMemberUserId;
			pEmpItemInfo->m_sAccount = pMemberInfo->m_sMemberAccount;
			pEmpItemInfo->m_sGroupCode = pMemberInfo->m_sGroupCode;
			pEmpItemInfo->m_sName = pMemberInfo->m_sUserName;
			pEmpItemInfo->m_dwItemData = pMemberInfo->m_nLineState;
			//pEmpItemInfo->m_pGroupInfo = pGroupInfo;
			//pEmpItemInfo->m_pMemberInfo = pMemberInfo;
			pMemberItemList.insert(pEmpItemInfo->m_sMemberCode,pEmpItemInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pEmpItemInfo.get());
			m_treeSearch.Sort(pMemberGroupInfo->m_hItem,CPOPApp::TreeCmpFunc);
			//ShowSearchResult();
			const int nGroupResultSize = pMemberItemList.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pMemberGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pMemberGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CPOPDlg::Fire_onSearchContactInfo(IEB_ContactInfo* pContactInfo, ULONG dwParam)
{
	 if (dwParam == 1)
	{
		// search
		const CEBString sContact(pContactInfo->Contact.GetBSTR());
		const CEBString sName(pContactInfo->Name.GetBSTR());

		const tstring theGroupContactAccount("[* *]");
		const CString sGroupResultName = _T("联系人搜索结果：");
		CTreeItemInfo::pointer pContactGroupInfo;
		if (!m_pContactItem.find(theGroupContactAccount,pContactGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pContactGroupInfo->m_dwItemData = 1;
			m_pContactItem.insert(theGroupContactAccount,pContactGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
			if ((!m_bAppFrameShowed || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!m_pContactItem.exist(sContact))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),sName.c_str(),sContact.c_str());
			HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
			pContactItemInfo->m_sId = pContactInfo->ContactId;
			pContactItemInfo->m_sAccount = sContact;
			pContactItemInfo->m_sName = sName;
			//pContactItemInfo->m_pContactInfo = pContactInfo;
			m_pContactItem.insert(pContactItemInfo->m_sAccount,pContactItemInfo);
			m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
			ShowSearchResult();
			const int nGroupResultSize = m_pContactItem.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
}
#else
void CPanelSearchResult::onContactInfo(const EB_ContactInfo* pContactInfo, unsigned long dwParam)
{
	 if (dwParam == 1)
	{
		// search
		const tstring theGroupContactAccount("[* *]");
		const CString sGroupResultName = _T("联系人搜索结果：");
		CTreeItemInfo::pointer pContactGroupInfo;
		if (!m_pContactItem.find(theGroupContactAccount,pContactGroupInfo))
		{
			HTREEITEM hEmpItem = m_treeSearch.InsertItem(sGroupResultName);
			pContactGroupInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hEmpItem);
			pContactGroupInfo->m_nSubType = 1;	// bAppFrameShowed sort
			pContactGroupInfo->m_dwItemData = 1;
			m_pContactItem.insert(theGroupContactAccount,pContactGroupInfo);
			m_treeSearch.SetItemData(hEmpItem,(DWORD)pContactGroupInfo.get());
			m_treeSearch.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
			if (m_treeSearch.GetSelectedItem()==NULL)
			//if ((GetMainFrameShowed() || m_pUrlItem.empty()) && m_treeSearch.GetSelectedItem()==NULL)
				m_treeSearch.Select(hEmpItem,TVGN_CARET);
		}
		if (!m_pContactItem.exist(pContactInfo->m_sContact))
		{
			CString sSearchName;
			sSearchName.Format(_T("%s(%s)"),pContactInfo->m_sName.c_str(),pContactInfo->m_sContact.c_str());
			HTREEITEM hContactItem = m_treeSearch.InsertItem(sSearchName,pContactGroupInfo->m_hItem);
			CTreeItemInfo::pointer pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
			pContactItemInfo->m_sId = pContactInfo->m_nContactId;
			pContactItemInfo->m_nUserId = pContactInfo->m_nContactUserId;
			pContactItemInfo->m_sAccount = pContactInfo->m_sContact;
			pContactItemInfo->m_sName = pContactInfo->m_sName;
			//pContactItemInfo->m_pContactInfo = pContactInfo;
			m_pContactItem.insert(pContactItemInfo->m_sAccount,pContactItemInfo);
			m_treeSearch.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
			//ShowSearchResult();
			const int nGroupResultSize = m_pContactItem.size()-1;
			if (nGroupResultSize==1)
			{
				m_treeSearch.Expand(pContactGroupInfo->m_hItem,TVE_EXPAND);
			}
			sSearchName.Format(_T("%s%d"),sGroupResultName,nGroupResultSize);
			m_treeSearch.SetItemText(pContactGroupInfo->m_hItem,sSearchName);
			ScrollTreeSearchToTop();
		}
	}
}
#endif
void CPanelSearchResult::ScrollTreeSearchToTop(void)
{
	const int nScrollPos = m_treeSearch.GetScrollPos(SB_VERT);
	if (nScrollPos>0)
	{
		WPARAM wParam = MAKELONG(SB_TOP,nScrollPos);
		m_treeSearch.SendMessage(WM_VSCROLL,wParam);
	}
}

void CPanelSearchResult::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
	this->DrawPopBg(&memDC, theDefaultFlatBgColor,0);

}

void CPanelSearchResult::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//__super::OnOK();
}

void CPanelSearchResult::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//__super::OnCancel();
}
LRESULT CPanelSearchResult::OnTreeItemDoubleClick(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	VividTree* pOwner = (VividTree*)lp;
	if (pOwner == &m_treeSearch)
	{
		if (m_pParentMsg!=NULL)
			m_pParentMsg->PostMessage(EB_COMMAND_SEARCH_SET_FOCUS_SEL,1,1);
		//m_editSearch.SetFocus();
		CallItem(item);
	}
	return 0;
}

LRESULT CPanelSearchResult::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	try
	{
		//boost::mutex::scoped_lock lockSearch(m_mutexSearch);
		HTREEITEM hItem = (HTREEITEM)wp;
		VividTree * pOwner = (VividTree*)lp;
		if (pOwner == &m_treeSearch)
		{
			if (hItem == NULL)// || !m_treeSearch.IsWindowVisible())
			{
				if (m_pViewContactInfo!=0 && m_pViewContactInfo->IsWindowVisible())
				{
					m_pViewContactInfo->HideReset();
				}
				m_btnSearchTrackCall.ShowWindow(SW_HIDE);
				m_btnSearchTrackDel.ShowWindow(SW_HIDE);
			}else
			{
				const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_treeSearch.GetItemData(hItem);
				if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR || pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_SUBINFO)
				{
					if (m_pViewContactInfo!=0 && m_pViewContactInfo->IsWindowVisible())
					{
						m_pViewContactInfo->HideReset();
					}
					m_btnSearchTrackCall.ShowWindow(SW_HIDE);
					m_btnSearchTrackDel.ShowWindow(SW_HIDE);
					return 0;
				}
				if (m_pViewContactInfo == NULL)
				{
					m_pViewContactInfo = new CDlgViewContactInfo(this);
					m_pViewContactInfo->Create(CDlgViewContactInfo::IDD,this);
				}
				CPoint pos;
				GetCursorPos(&pos);

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

				if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER)
				{
#ifdef USES_EBCOM_TEST
					CComPtr<IEB_MemberInfo> pMemberInfo;
					CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode);
					if (pGroupInfo == NULL) return 1;
					pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
					if (pMemberInfo == NULL) return 1;
					m_pViewContactInfo->SetEmployeeInfo(pMemberInfo,pGroupInfo);
#else
					EB_MemberInfo pMemberInfo;
					EB_GroupInfo pGroupInfo;
					if (!theEBAppClient.EB_GetGroupInfo(pTreeItemInfo->m_sGroupCode,&pGroupInfo)) return 1;
					if (!theEBAppClient.EB_GetMemberInfoByUserId(&pMemberInfo,pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId)) return 1;
					m_pViewContactInfo->SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
#endif
				}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
				{
#ifdef USES_EBCOM_TEST
					CComPtr<IEB_ContactInfo> pContactInfo = theEBClientCore->EB_GetContactInfo1(pTreeItemInfo->m_sId);
					if (pContactInfo==NULL)
						return 1;
					m_pViewContactInfo->SetContactInfo(pContactInfo);
#else
					EB_ContactInfo pContactInfo;
					if (!theEBAppClient.EB_GetContactInfo1(pTreeItemInfo->m_sId,&pContactInfo))
						return 1;
					m_pViewContactInfo->SetContactInfo(&pContactInfo);
#endif
				}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_URL)
				{
					if (m_pViewContactInfo->IsWindowVisible())
						m_pViewContactInfo->HideReset();

					const int const_btn_width = m_btnSearchTrackDel.GetImgWidth();
					m_treeSearch.GetItemRect(hItem, &rect, TRUE);
					const int nTop = rect.top;
					m_treeSearch.GetClientRect(&rect);
					const int nRight = rect.right;
					m_btnSearchTrackDel.MovePoint(nRight-const_btn_width, nTop);
					m_btnSearchTrackDel.ShowWindow(SW_SHOW);
					m_btnSearchTrackDel.Invalidate();
					return 0;
				}else if (m_pViewContactInfo->IsWindowVisible())
				{
					m_pViewContactInfo->HideReset();
					return 0;
				}
				m_pViewContactInfo->ShowWindow(SW_SHOW);
				m_treeSearch.SetFocus();

				//const int const_btn_width = m_btnSearchTrackCall.GetImgWidth();
				//m_treeSearch.GetItemRect(hItem, &rect, TRUE);
				//const int nTop = rect.top;
				//m_treeSearch.GetClientRect(&rect);
				//const int nRight = rect.right;
				//m_btnSearchTrackCall.MovePoint(nRight-const_btn_width, nTop);
				//m_btnSearchTrackCall.ShowWindow(SW_SHOW);
				//m_btnSearchTrackCall.Invalidate();
			}
		}
	}catch(const std::exception&)
	{
	}catch(...)
	{
	}
	return 0;
}
void CPanelSearchResult::OnNMClickTreeSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	SetTimer(TIMERID_CALL_SEARCH_SEL_CHANGE,1,NULL);
}

void CPanelSearchResult::OnDestroy()
{
	__super::OnDestroy();
	m_btnSearchTrackDel.DestroyWindow();
	m_btnSearchTrackCall.DestroyWindow();
	if (m_pViewContactInfo!=0)
	{
		m_pViewContactInfo->DestroyWindow();
		delete m_pViewContactInfo;
		m_pViewContactInfo = NULL;
	}
}
