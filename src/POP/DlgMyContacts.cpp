// DlgMyContacts.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgMyContacts.h"
#include "DlgViewContactInfo.h"

#define TIMERID_DELETE_ITEM 100
#define TIMERID_EDIT_ITEM 101
#define TIMERID_CHECK_ITEM_HOT	102

// CDlgMyContacts dialog

IMPLEMENT_DYNAMIC(CDlgMyContacts, CDialog)

CDlgMyContacts::CDlgMyContacts(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMyContacts::IDD, pParent)
{
	m_pViewContactInfo = NULL;
	m_hCurrentHotItem = NULL;
	m_hEditGroupTempItem = NULL;
}

CDlgMyContacts::~CDlgMyContacts()
{
}

void CDlgMyContacts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CONTACTS, m_treeContacts);
}


BEGIN_MESSAGE_MAP(CDlgMyContacts, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(EB_MSG_VIEW_MSG_RECORD, &CDlgMyContacts::OnViewMsgRecord)
	//ON_COMMAND(EB_COMMAND_DELETE_MSG_RECORD, &CDlgMyContacts::OnDeleteMsgRecord)
	ON_COMMAND(EB_COMMAND_NEW_GROUP, &CDlgMyContacts::OnGroupNew)
	ON_COMMAND(EB_COMMAND_DELETE_GROUP, &CDlgMyContacts::OnGroupDelete)
	ON_COMMAND(EB_COMMAND_RENAME_GROUP, &CDlgMyContacts::OnGroupRename)
	ON_COMMAND(EB_COMMAND_NEW_CONTACT, &CDlgMyContacts::OnContactNew)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CONTACTS, &CDlgMyContacts::OnNMRClickTreeContacts)
	ON_COMMAND(EB_COMMAND_EDIT_CONTACT, &CDlgMyContacts::OnContactEdit)
	ON_COMMAND(EB_COMMAND_DELETE_CONTACT, &CDlgMyContacts::OnContactDelete)
	ON_COMMAND(EB_COMMAND_CALL_USER, &CDlgMyContacts::OnCallUser)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDblclk)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_WM_TIMER()
	ON_COMMAND_RANGE(EB_COMMAND_SUBSCRIBE_FUNC,EB_COMMAND_SUBSCRIBE_FUNC+0x20,OnSubscribeFunc)
	ON_COMMAND_RANGE(EB_COMMAND_MY_UGINFO,EB_COMMAND_MY_UGINFO+100,OnMyUGInfo)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_CONTACTS, &CDlgMyContacts::OnTvnEndlabeleditTreeContacts)
END_MESSAGE_MAP()


// CDlgMyContacts message handlers
void CDlgMyContacts::SetCtrlColor(void)
{
	if (m_pViewContactInfo!=NULL && m_pViewContactInfo->GetSafeHwnd()!=NULL)
		m_pViewContactInfo->SetCtrlColor();

	//if (theApp.GetColorFlat())
	//	m_treeContacts.SetBkGradients(theApp.GetFlatBgColor(),theApp.GetFlatBgColor());
	//else
	//	m_treeContacts.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());
}

BOOL CDlgMyContacts::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnDeleteTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeContacts, 0xffff);
	m_btnDeleteTrack.SetAutoSize(false);
	m_btnDeleteTrack.SetAutoFocus(true);
	m_btnDeleteTrack.Load(IDB_PNG_HOT_DELETE);
	m_btnDeleteTrack.SetToolTipText(_T("删除"));
	m_btnEditTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeContacts, 0xffff);
	m_btnEditTrack.SetAutoSize(false);
	m_btnEditTrack.SetAutoFocus(true);
	m_btnEditTrack.Load(IDB_PNG_HOT_EDIT);
	m_btnEditTrack.SetToolTipText(_T("修改"));
	m_btnCallTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeContacts, 0xffff);
	m_btnCallTrack.SetAutoSize(false);
	m_btnCallTrack.SetAutoFocus(true);
	m_btnCallTrack.Load(IDB_PNG_HOT_CALL);
	m_btnCallTrack.SetToolTipText(_T("打开会话"));
	m_treeContacts.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeContacts.SetBkGradients(theDefaultFlatBgColor,theDefaultFlatBgColor);
	m_treeContacts.SetCallback((CTreeCallback*)&theApp);
	m_treeContacts.ModifyStyle(0, TVS_SINGLEEXPAND);
	//m_treeContacts.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeContacts.SetItemHeight(40);
	m_treeContacts.SetIconSize(32,32);
	//m_treeContacts.SetItemIcon( theApp.GetIconCon() );
	//m_icon = AfxGetApp()->LoadIcon(IDI_DEPARTMENT);
	//m_treeContacts.SetRootIcon( m_icon );
	//m_icon = AfxGetApp()->LoadIcon(IDI_DEPARTMENT);
	//m_treeContacts.SetParentIcon( m_icon );

	CString sGroupText;
	if (theApp.GetAuthContact())
		sGroupText.Format(_T("%s [0]"),const_default_group_name.c_str());
	else
		sGroupText.Format(_T("%s (0)"),const_default_group_name.c_str());
	HTREEITEM pGroupTreeItem = m_treeContacts.InsertItem(sGroupText);
	CTreeItemInfo::pointer pContactInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,pGroupTreeItem);
	pContactInfo->m_nSubType = -1;
	pContactInfo->m_sId = const_default_group_ugid;
	pContactInfo->m_sName = const_default_group_name;
	/// 默认分组，排在前面
	pContactInfo->m_nIndex = 1;
	m_treeContacts.SetItemData(pGroupTreeItem,(DWORD)pContactInfo.get());
	m_pGroupItemInfo.insert(const_default_group_ugid, pContactInfo);

	SetCtrlColor();

	//SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMyContacts::OnDestroy()
{
	CDialog::OnDestroy();
	m_btnDeleteTrack.DestroyWindow();
	m_btnEditTrack.DestroyWindow();
	m_btnCallTrack.DestroyWindow();
	if (m_pViewContactInfo!=0)
	{
		m_pViewContactInfo->DestroyWindow();
		delete m_pViewContactInfo;
		m_pViewContactInfo = NULL;
	}
}

void CDlgMyContacts::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_treeContacts.GetSafeHwnd())
		m_treeContacts.MoveWindow(0, 0, cx, cy);
}

void CDlgMyContacts::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	IsTreeCtrlEditMessage(VK_ESCAPE);

	//CDialog::OnCancel();
}

void CDlgMyContacts::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	IsTreeCtrlEditMessage(VK_RETURN);

	//CDialog::OnOK();
}

#ifdef USES_EBCOM_TEST
void CDlgMyContacts::UGInfo(IEB_UGInfo* pUGInfo)
{
	if (pUGInfo==NULL || pUGInfo->UGId==0) return;
	const eb::bigint nUGId = pUGInfo->UGId;
	const CEBString sGroupName = pUGInfo->GroupName.GetBSTR();

	CString sGroupText;
	CTreeItemInfo::pointer pGroupItemInfo;
	if (!m_pGroupItemInfo.find(nUGId,pGroupItemInfo))
	{
		if (theApp.GetAuthContact())
			sGroupText.Format(_T("%s [0]"), sGroupName.c_str());
		else
			sGroupText.Format(_T("%s (0)"), sGroupName.c_str());
		HTREEITEM pGroupTreeItem = m_treeContacts.InsertItem(sGroupText);
		pGroupItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,pGroupTreeItem);
		pGroupItemInfo->m_nSubType = -1;
		pGroupItemInfo->m_sId = nUGId;
		pGroupItemInfo->m_sName = sGroupName;
		m_pGroupItemInfo.insert(nUGId,pGroupItemInfo);
		m_treeContacts.SetItemData(pGroupTreeItem, (DWORD)pGroupItemInfo.get());
		m_treeContacts.SortChildren(TVI_ROOT);
	}else
	{
		pGroupItemInfo->m_sName = sGroupName;
		if (theApp.GetAuthContact())
		{
			int nContactSize = 0;
			int nOnlineSize = 0;
			theEBClientCore->EB_GetUGContactSize(pGroupItemInfo->m_sId,nContactSize,nOnlineSize);
			sGroupText.Format(_T("%s (%d)"), sGroupName.c_str(), nOnlineSize, nContactSize);
		}else
			sGroupText.Format(_T("%s (%d)"), sGroupName.c_str(), pGroupItemInfo->m_dwItemData);
		m_treeContacts.SetItemText(pGroupItemInfo->m_hItem,sGroupText);
	}
}
#else
void CDlgMyContacts::UGInfo(const EB_UGInfo* pUGInfo)
{
	if (pUGInfo==NULL || pUGInfo->m_nUGId==0) return;
	CString sGroupText;
	CTreeItemInfo::pointer pGroupItemInfo;
	if (!m_pGroupItemInfo.find(pUGInfo->m_nUGId,pGroupItemInfo))
	{
		if (theApp.GetAuthContact())
			sGroupText.Format(_T("%s [0]"), pUGInfo->m_sGroupName.c_str());
		else
			sGroupText.Format(_T("%s (0)"), pUGInfo->m_sGroupName.c_str());
		HTREEITEM pGroupTreeItem = m_treeContacts.InsertItem(sGroupText);
		pGroupItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,pGroupTreeItem);
		pGroupItemInfo->m_nSubType = -1;
		pGroupItemInfo->m_sId = pUGInfo->m_nUGId;
		pGroupItemInfo->m_sName = pUGInfo->m_sGroupName;
		m_pGroupItemInfo.insert(pUGInfo->m_nUGId,pGroupItemInfo);
		m_treeContacts.SetItemData(pGroupTreeItem, (DWORD)pGroupItemInfo.get());
		m_treeContacts.SortChildren(TVI_ROOT);
	}else
	{
		pGroupItemInfo->m_sName = pUGInfo->m_sGroupName;
		if (theApp.GetAuthContact())
		{
			int nContactSize = 0;
			int nOnlineSize = 0;
			theEBAppClient.EB_GetUGContactSize(pGroupItemInfo->m_sId,nContactSize,nOnlineSize);
			sGroupText.Format(_T("%s (%d/%d)"), pUGInfo->m_sGroupName.c_str(), nOnlineSize, nContactSize);
		}else
			sGroupText.Format(_T("%s (%d)"), pUGInfo->m_sGroupName.c_str(), pGroupItemInfo->m_dwItemData);
		m_treeContacts.SetItemText(pGroupItemInfo->m_hItem,sGroupText);
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgMyContacts::UGDelete(IEB_UGInfo* pUGInfo)
{
	const eb::bigint nUGId = pUGInfo->UGId;
	CTreeItemInfo::pointer pGroupItemInfo;
	if (!m_pGroupItemInfo.find(nUGId,pGroupItemInfo,true))
	{
		return;
	}

	{
		CTreeItemInfo::pointer pDefaultGroupItemInfo;
		CString sContactText;

		// 先处理该分组下面，并移到默认分组去；
		AUTO_RLOCK(m_pContactItemInfo);
		CLockMap<eb::bigint,CTreeItemInfo::pointer>::const_iterator pIter;
		for (pIter=m_pContactItemInfo.begin(); pIter!=m_pContactItemInfo.end(); pIter++)
		{
			CTreeItemInfo::pointer pContactItemInfo = pIter->second;
			if (pContactItemInfo->m_sParentId == pGroupItemInfo->m_sId)
			{
				if (pDefaultGroupItemInfo.get()==NULL)
				{
					if (!m_pGroupItemInfo.find(const_default_group_ugid,pDefaultGroupItemInfo))
					{
						continue;
					}
				}
				pContactItemInfo->m_sParentId = pDefaultGroupItemInfo->m_sId;
				pDefaultGroupItemInfo->m_dwItemData++;

				// 移到默认分组去；
				sContactText.Format(_T("%s"), pContactItemInfo->m_sName.c_str());
				pContactItemInfo->m_hItem = m_treeContacts.InsertItem(sContactText,pDefaultGroupItemInfo->m_hItem);
				m_treeContacts.SetItemData(pContactItemInfo->m_hItem,(DWORD)pContactItemInfo.get());
			}
		}
		if (pDefaultGroupItemInfo.get()!=NULL)
		{
			// 有移到默认分组，重新设置一次数量；
			CString sGroupText;
			sGroupText.Format(_T("%s(%d)"), pDefaultGroupItemInfo->m_sName.c_str(),pDefaultGroupItemInfo->m_dwItemData);
			m_treeContacts.SetItemText(pDefaultGroupItemInfo->m_hItem,sGroupText);
		}
	}

	m_treeContacts.DeleteItem(pGroupItemInfo->m_hItem);
}
#else
void CDlgMyContacts::UGDelete(const EB_UGInfo* pUGInfo)
{
	CTreeItemInfo::pointer pGroupItemInfo;
	if (!m_pGroupItemInfo.find(pUGInfo->m_nUGId,pGroupItemInfo,true))
	{
		return;
	}

	{
		CTreeItemInfo::pointer pDefaultGroupItemInfo;
		CString sContactText;

		// 先处理该分组下面，并移到默认分组去；
		AUTO_RLOCK(m_pContactItemInfo);
		CLockMap<eb::bigint,CTreeItemInfo::pointer>::const_iterator pIter;
		for (pIter=m_pContactItemInfo.begin(); pIter!=m_pContactItemInfo.end(); pIter++)
		{
			const CTreeItemInfo::pointer& pContactItemInfo = pIter->second;
			if (pContactItemInfo->m_sParentId == pGroupItemInfo->m_sId)
			{
				if (pDefaultGroupItemInfo.get()==NULL)
				{
					if (!m_pGroupItemInfo.find(const_default_group_ugid,pDefaultGroupItemInfo))
					{
						continue;
					}
				}
				pContactItemInfo->m_sParentId = pDefaultGroupItemInfo->m_sId;
				pDefaultGroupItemInfo->m_dwItemData++;

				// 移到默认分组去；
				sContactText.Format(_T("%s"), pContactItemInfo->m_sName.c_str());
				pContactItemInfo->m_hItem = m_treeContacts.InsertItem(sContactText,pDefaultGroupItemInfo->m_hItem);
				m_treeContacts.SetItemData(pContactItemInfo->m_hItem,(DWORD)pContactItemInfo.get());
			}
		}
		if (pDefaultGroupItemInfo.get()!=NULL)
		{
			/// 有移到默认分组，重新设置一次数量；
			CString sGroupText;
			if (theApp.GetAuthContact())
			{
				int nContactSize = 0;
				int nOnlineSize = 0;
				theEBAppClient.EB_GetUGContactSize(0,nContactSize,nOnlineSize);
				sGroupText.Format(_T("%s [%d/%d]"), pDefaultGroupItemInfo->m_sName.c_str(),nOnlineSize,nContactSize);
			}else
			{
				sGroupText.Format(_T("%s (%d)"), pDefaultGroupItemInfo->m_sName.c_str(), pDefaultGroupItemInfo->m_dwItemData);
			}
			m_treeContacts.SetItemText(pDefaultGroupItemInfo->m_hItem, sGroupText);
			//CString sGroupText;
			//sGroupText.Format(_T("%s (%d)"), pDefaultGroupItemInfo->m_sName.c_str(),pDefaultGroupItemInfo->m_dwItemData);
			//m_treeContacts.SetItemText(pDefaultGroupItemInfo->m_hItem,sGroupText);
		}
	}

	m_treeContacts.DeleteItem(pGroupItemInfo->m_hItem);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgMyContacts::ContactInfo(IEB_ContactInfo* pPopContactInfo)
{
	if (pPopContactInfo==NULL || pPopContactInfo->ContactId==0) return;
	const eb::bigint nContactId = pPopContactInfo->ContactId;
	const CEBString sGroup = pPopContactInfo->GroupName.GetBSTR();
	const CEBString sName = pPopContactInfo->Name.GetBSTR();
	const CEBString sContact = pPopContactInfo->Contact.GetBSTR();
	const eb::bigint nUGId = pPopContactInfo->UGId==0?const_default_group_ugid:pPopContactInfo->UGId;
	const CEBString sGroupName = sGroup.empty()?const_default_group_name:sGroup;
	CString sGroupText;
	CTreeItemInfo::pointer pGroupItemInfo;
	if (!m_pGroupItemInfo.find(nUGId,pGroupItemInfo))
	{
		if (theApp.GetAuthContact())
			sGroupText.Format(_T("%s [0]"), sGroupName.c_str());
		else
			sGroupText.Format(_T("%s (0)"), sGroupName.c_str());
		HTREEITEM pGroupTreeItem = m_treeContacts.InsertItem(sGroupText);
		pGroupItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,pGroupTreeItem);
		pGroupItemInfo->m_nSubType = -1;
		pGroupItemInfo->m_sId = nUGId;
		pGroupItemInfo->m_sGroupName = sGroup;
		pGroupItemInfo->m_sName = sGroupName;
		m_pGroupItemInfo.insert(nUGId,pGroupItemInfo);
		m_treeContacts.SetItemData(pGroupTreeItem, (DWORD)pGroupItemInfo.get());
		m_treeContacts.SortChildren(TVI_ROOT);
	}
	CString sContactText;
	sContactText.Format(_T("%s(%s)"), sName.c_str(), sContact.c_str());
	CTreeItemInfo::pointer pContactItemInfo;
	bool bChangeGroupCount = false;
	bool bChangeLineState = false;
	if (!m_pContactItemInfo.find(nContactId, pContactItemInfo))
	{
		pGroupItemInfo->m_dwItemData++;
		bChangeGroupCount = true;
		HTREEITEM hContactItem = m_treeContacts.InsertItem(sContactText, pGroupItemInfo->m_hItem);
		pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
		pContactItemInfo->m_sId = nContactId;
		pContactItemInfo->m_sParentId = pGroupItemInfo->m_sId;
		pContactItemInfo->m_sGroupName = pGroupItemInfo->m_sGroupName;
		pContactItemInfo->m_sName = sName;
		pContactItemInfo->m_sAccount = sContact;
		pContactItemInfo->m_dwItemData = pPopContactInfo->LineState;
		m_pContactItemInfo.insert(nContactId,pContactItemInfo);
		m_treeContacts.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
	}else if (pContactItemInfo->m_sGroupName != sGroup)
	{
		// 换group
		HTREEITEM hParentItem = m_treeContacts.GetParentItem(pContactItemInfo->m_hItem);
		CTreeItemInfo* pOldGroupItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hParentItem);
		if (pOldGroupItemInfo!=NULL && pOldGroupItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
		{
			if (pOldGroupItemInfo->m_dwItemData>1 || pOldGroupItemInfo->m_sGroupName.empty())	// empty是默认分组
			{
				pOldGroupItemInfo->m_dwItemData--;
				if (theApp.GetAuthContact())
				{
					int nContactSize = 0;
					int nOnlineSize = 0;
					theEBClientCore->EB_GetUGContactSize(pOldGroupItemInfo->m_sId,nContactSize,nOnlineSize);
					sGroupText.Format(_T("%s [%d/%d]"), pOldGroupItemInfo->m_sName.c_str(),nOnlineSize,nContactSize);
				}else
					sGroupText.Format(_T("%s (%d)"), pOldGroupItemInfo->m_sName.c_str(),pOldGroupItemInfo->m_dwItemData);
				m_treeContacts.SetItemText(pOldGroupItemInfo->m_hItem,sGroupText);
			}else// if (pOldGroupItemInfo->m_dwItemData<=0 && !pOldGroupItemInfo->m_sGroupCode.empty())
			{
				// 无用分组，删除
				m_treeContacts.DeleteItem(pOldGroupItemInfo->m_hItem);
				m_pGroupItemInfo.remove(pOldGroupItemInfo->m_sId);
				//m_pGroupItemInfo.remove(pOldGroupItemInfo->m_sGroupName);
			}
		}
		m_treeContacts.DeleteItem(pContactItemInfo->m_hItem);
		m_pContactItemInfo.remove(nContactId);

		pGroupItemInfo->m_dwItemData++;
		bChangeGroupCount = true;
		HTREEITEM hContactItem = m_treeContacts.InsertItem(sContactText, pGroupItemInfo->m_hItem);
		pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
		pContactItemInfo->m_sId = nContactId;
		pContactItemInfo->m_sParentId = pGroupItemInfo->m_sId;
		pContactItemInfo->m_sGroupName = pGroupItemInfo->m_sGroupName;
		pContactItemInfo->m_sName = sName;
		pContactItemInfo->m_sAccount = sContact;
		pContactItemInfo->m_dwItemData = pPopContactInfo->LineState;
		m_pContactItemInfo.insert(nContactId,pContactItemInfo);
		m_treeContacts.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
	}else
	{
		if (theApp.GetAuthContact())
		{
			if ((pContactItemInfo->m_dwItemData <= EB_LINE_STATE_OFFLINE && pPopContactInfo->LineState>=EB_LINE_STATE_BUSY) ||	// 下线
				(pContactItemInfo->m_dwItemData >= EB_LINE_STATE_BUSY && pPopContactInfo->LineState<=EB_LINE_STATE_OFFLINE))		// 上线
			{
				bChangeLineState = true;
			}
		}
		pContactItemInfo->m_dwItemData = pPopContactInfo->LineState;
		m_treeContacts.SetItemText(pContactItemInfo->m_hItem, sContactText);
	}
	m_treeContacts.Expand(pGroupItemInfo->m_hItem, TVE_EXPAND);
	m_treeContacts.SelectItem(pContactItemInfo->m_hItem);
	if (bChangeGroupCount || bChangeLineState)
	{
		if (theApp.GetAuthContact())
		{
			int nContactSize = 0;
			int nOnlineSize = 0;
			theEBAppClient.EB_GetUGContactSize(pGroupItemInfo->m_sId,nContactSize,nOnlineSize);
			sGroupText.Format(_T("%s [%d/%d]"), sGroupName.c_str(),nOnlineSize,nContactSize);
		}else
		{
			sGroupText.Format(_T("%s (%d)"), sGroupName.c_str(), pGroupItemInfo->m_dwItemData);
		}
		m_treeContacts.SetItemText(pGroupItemInfo->m_hItem, sGroupText);
	}
	if (pGroupItemInfo->m_dwItemData>1)
		m_treeContacts.SortChildren(pGroupItemInfo->m_hItem);
	m_treeContacts.Sort(pGroupItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
}
#else
void CDlgMyContacts::ContactHeadChange(const EB_ContactInfo* pPopContactInfo)
{
	if (pPopContactInfo==NULL || pPopContactInfo->m_nContactId==0) return;
	//CTreeItemInfo::pointer pGroupItemInfo;
	//if (m_pContactItemInfo.find(pPopContactInfo->m_nContactId, pContactItemInfo))
	//{
	//	// ???
	//}
}
void CDlgMyContacts::ContactInfo(const EB_ContactInfo* pPopContactInfo)
{
	if (pPopContactInfo==NULL || pPopContactInfo->m_nContactId==0) return;
	const eb::bigint nUGId = pPopContactInfo->m_nUGId==0?const_default_group_ugid:pPopContactInfo->m_nUGId;
	CString sGroupText;
	CTreeItemInfo::pointer pGroupItemInfo;
	if (!m_pGroupItemInfo.find(nUGId,pGroupItemInfo))
	{
		if (theApp.GetAuthContact())
			sGroupText.Format(_T("%s [0]"), pPopContactInfo->m_sGroupName.c_str());
		else
			sGroupText.Format(_T("%s (0)"), pPopContactInfo->m_sGroupName.c_str());
		HTREEITEM pGroupTreeItem = m_treeContacts.InsertItem(sGroupText);
		pGroupItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,pGroupTreeItem);
		pGroupItemInfo->m_nSubType = -1;
		pGroupItemInfo->m_sId = nUGId;
		pGroupItemInfo->m_sGroupName = pPopContactInfo->m_sGroupName;
		pGroupItemInfo->m_sName = pPopContactInfo->m_sGroupName;
		m_pGroupItemInfo.insert(nUGId,pGroupItemInfo);
		m_treeContacts.SetItemData(pGroupTreeItem, (DWORD)pGroupItemInfo.get());
		m_treeContacts.SortChildren(TVI_ROOT);
	}
	const CString sContactText = pPopContactInfo->m_sName.c_str();
	CTreeItemInfo::pointer pContactItemInfo;
	bool bChangeGroupCount = false;
	bool bChangeLineState = false;
	if (!m_pContactItemInfo.find(pPopContactInfo->m_nContactId, pContactItemInfo))
	{
		pGroupItemInfo->m_dwItemData++;
		bChangeGroupCount = true;
		HTREEITEM hContactItem = m_treeContacts.InsertItem(sContactText, pGroupItemInfo->m_hItem);
		pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
		pContactItemInfo->m_sId = pPopContactInfo->m_nContactId;
		pContactItemInfo->m_sParentId = pGroupItemInfo->m_sId;
		pContactItemInfo->m_sGroupName = pGroupItemInfo->m_sGroupName;
		pContactItemInfo->m_sName = pPopContactInfo->m_sName;
		pContactItemInfo->m_nUserId = pPopContactInfo->m_nContactUserId;
		pContactItemInfo->m_sAccount = pPopContactInfo->m_sContact;
		pContactItemInfo->m_dwItemData = pPopContactInfo->m_nLineState;
		pContactItemInfo->m_nSubType = pPopContactInfo->m_nContactType;
		m_pContactItemInfo.insert(pPopContactInfo->m_nContactId,pContactItemInfo);
		m_treeContacts.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
	}else if (pContactItemInfo->m_sParentId != pPopContactInfo->m_nUGId)
	{
		// 换group
		HTREEITEM hParentItem = m_treeContacts.GetParentItem(pContactItemInfo->m_hItem);
		CTreeItemInfo* pOldGroupItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hParentItem);
		if (pOldGroupItemInfo!=NULL && pOldGroupItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
		{
			if (pOldGroupItemInfo->m_dwItemData>1 || pOldGroupItemInfo->m_sGroupCode==0)	// empty是默认分组
			{
				pOldGroupItemInfo->m_dwItemData--;
				if (theApp.GetAuthContact())
				{
					int nContactSize = 0;
					int nOnlineSize = 0;
					theEBAppClient.EB_GetUGContactSize(pOldGroupItemInfo->m_sId,nContactSize,nOnlineSize);
					sGroupText.Format(_T("%s [%d/%d]"), pOldGroupItemInfo->m_sName.c_str(),nOnlineSize,nContactSize);
				}else
					sGroupText.Format(_T("%s (%d)"), pOldGroupItemInfo->m_sName.c_str(),pOldGroupItemInfo->m_dwItemData);
				m_treeContacts.SetItemText(pOldGroupItemInfo->m_hItem,sGroupText);
			}else// if (pOldGroupItemInfo->m_dwItemData<=0 && !pOldGroupItemInfo->m_sGroupCode.empty())
			{
				// 无用分组，删除
				m_treeContacts.DeleteItem(pOldGroupItemInfo->m_hItem);
				m_pGroupItemInfo.remove(pOldGroupItemInfo->m_sId);
				//m_pGroupItemInfo.remove(pOldGroupItemInfo->m_sGroupName);
			}
		}
		m_treeContacts.DeleteItem(pContactItemInfo->m_hItem);
		m_pContactItemInfo.remove(pPopContactInfo->m_nContactId);

		pGroupItemInfo->m_dwItemData++;
		bChangeGroupCount = true;
		HTREEITEM hContactItem = m_treeContacts.InsertItem(sContactText, pGroupItemInfo->m_hItem);
		pContactItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hContactItem);
		pContactItemInfo->m_sId = pPopContactInfo->m_nContactId;
		pContactItemInfo->m_sParentId = pGroupItemInfo->m_sId;
		pContactItemInfo->m_sGroupName = pGroupItemInfo->m_sGroupName;
		pContactItemInfo->m_sName = pPopContactInfo->m_sName;
		pContactItemInfo->m_nUserId = pPopContactInfo->m_nContactUserId;
		pContactItemInfo->m_sAccount = pPopContactInfo->m_sContact;
		pContactItemInfo->m_dwItemData = pPopContactInfo->m_nLineState;
		pContactItemInfo->m_nSubType = pPopContactInfo->m_nContactType;
		m_pContactItemInfo.insert(pPopContactInfo->m_nContactId,pContactItemInfo);
		m_treeContacts.SetItemData(hContactItem,(DWORD)pContactItemInfo.get());
	}else
	{
		if (theApp.GetAuthContact())
		{
			if ((pContactItemInfo->m_dwItemData <= EB_LINE_STATE_OFFLINE && pPopContactInfo->m_nLineState>=EB_LINE_STATE_BUSY) ||	// 下线
				(pContactItemInfo->m_dwItemData >= EB_LINE_STATE_BUSY && pPopContactInfo->m_nLineState<=EB_LINE_STATE_OFFLINE))		// 上线
			{
				bChangeLineState = true;
			}
		}
		pContactItemInfo->m_dwItemData = pPopContactInfo->m_nLineState;
		m_treeContacts.SetItemText(pContactItemInfo->m_hItem, sContactText);
	}
	m_treeContacts.Expand(pGroupItemInfo->m_hItem, TVE_EXPAND);
	m_treeContacts.SelectItem(pContactItemInfo->m_hItem);
	if (bChangeGroupCount || bChangeLineState)
	{
		if (theApp.GetAuthContact())
		{
			int nContactSize = 0;
			int nOnlineSize = 0;
			theEBAppClient.EB_GetUGContactSize(pGroupItemInfo->m_sId,nContactSize,nOnlineSize);
			sGroupText.Format(_T("%s [%d/%d]"), pGroupItemInfo->m_sName.c_str(),nOnlineSize,nContactSize);
		}else
		{
			sGroupText.Format(_T("%s (%d)"), pGroupItemInfo->m_sName.c_str(), pGroupItemInfo->m_dwItemData);
		}
		m_treeContacts.SetItemText(pGroupItemInfo->m_hItem, sGroupText);
	}
	if (pGroupItemInfo->m_dwItemData>1)
		m_treeContacts.SortChildren(pGroupItemInfo->m_hItem);
	m_treeContacts.Sort(pGroupItemInfo->m_hItem, CPOPApp::TreeCmpFunc);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgMyContacts::DeleteContact(IEB_ContactInfo* pPopContactInfo)
{
	const eb::bigint nContactId = pPopContactInfo->ContactId;
	const CEBString sContact = pPopContactInfo->Contact.GetBSTR();
	CTreeItemInfo::pointer pContactItemInfo;
	if (m_pContactItemInfo.find(nContactId, pContactItemInfo, true))
	{
		HTREEITEM hGroupItem = m_treeContacts.GetParentItem(pContactItemInfo->m_hItem);
		CTreeItemInfo* pGroupItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hGroupItem);
		if (pGroupItemInfo != NULL)
		{
			if (pGroupItemInfo->m_dwItemData>0)
				pGroupItemInfo->m_dwItemData--;
			const tstring sGroupName = pGroupItemInfo->m_sName;
			//if (pGroupItemInfo->m_dwItemData == 0 && sGroupName != const_default_group_name)
			//{
			//	// delete group item
			//	m_pGroupItemInfo.remove(sGroupName);
			//	m_treeContacts.DeleteItem(hGroupItem);
			//}else
			{
				CString sGroupText;
				if (theApp.GetAuthContact())
				{
					int nContactSize = 0;
					int nOnlineSize = 0;
					theEBAppClient.EB_GetUGContactSize(pGroupItemInfo->m_sId,nContactSize,nOnlineSize);
					sGroupText.Format(_T("%s [%d/%d]"),sGroupName.c_str(),nOnlineSize,nContactSize);
				}else
					sGroupText.Format(_T("%s (%d)"),sGroupName.c_str(),pGroupItemInfo->m_dwItemData);
				m_treeContacts.SetItemText(hGroupItem, sGroupText);
			}
			m_treeContacts.Expand(hGroupItem, TVE_EXPAND);
		}
		m_treeContacts.DeleteItem(pContactItemInfo->m_hItem);
	}
}
#else
void CDlgMyContacts::DeleteContact(const EB_ContactInfo* pPopContactInfo)
{
	CTreeItemInfo::pointer pContactItemInfo;
	if (m_pContactItemInfo.find(pPopContactInfo->m_nContactId, pContactItemInfo, true))
	{
		const HTREEITEM hGroupItem = m_treeContacts.GetParentItem(pContactItemInfo->m_hItem);
		CTreeItemInfo* pGroupItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hGroupItem);
		if (pGroupItemInfo != NULL)
		{
			if (pGroupItemInfo->m_dwItemData>0)
				pGroupItemInfo->m_dwItemData--;
			//const eb::bigint nUGId = pGroupItemInfo->m_sId;
			//if (pGroupItemInfo->m_dwItemData == 0 && nUGId != const_default_group_ugid)
			//{
			//	// delete group item
			//	m_pGroupItemInfo.remove(nUGId);
			//	m_treeContacts.DeleteItem(hGroupItem);
			//}else
			{
				CString sGroupText;
				if (theApp.GetAuthContact())
				{
					int nContactSize = 0;
					int nOnlineSize = 0;
					theEBAppClient.EB_GetUGContactSize(pGroupItemInfo->m_sId,nContactSize,nOnlineSize);
					sGroupText.Format(_T("%s [%d/%d]"),pGroupItemInfo->m_sName.c_str(),nOnlineSize,nContactSize);
				}else
					sGroupText.Format(_T("%s (%d)"),pGroupItemInfo->m_sName.c_str(),pGroupItemInfo->m_dwItemData);
				m_treeContacts.SetItemText(hGroupItem, sGroupText);
			}
			m_treeContacts.Expand(hGroupItem, TVE_EXPAND);
		}
		m_treeContacts.DeleteItem(pContactItemInfo->m_hItem);
	}
}
#endif

//CTreeItemInfo::pointer CDlgMyContacts::GetGroupItemInfo(HTREEITEM hItem) const
//{
//	boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(m_pGroupItemInfo.mutex()));
//	CLockMap<std::string,CTreeItemInfo::pointer>::const_iterator pIter;
//	for (pIter=m_pGroupItemInfo.begin(); pIter!=m_pGroupItemInfo.end(); pIter++)
//	{
//		CTreeItemInfo::pointer pContactItemInfo = pIter->second;
//		if (pContactItemInfo->m_hItem == hItem)
//			return pContactItemInfo;
//	}
//	return NullTreeItemInfo;
//}
//CTreeItemInfo::pointer CDlgMyContacts::GetContactItemInfo(HTREEITEM hItem) const
//{
//	boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(m_pContactItemInfo.mutex()));
//	CLockMap<std::string,CTreeItemInfo::pointer>::const_iterator pIter;
//	for (pIter=m_pContactItemInfo.begin(); pIter!=m_pContactItemInfo.end(); pIter++)
//	{
//		CTreeItemInfo::pointer pContactItemInfo = pIter->second;
//		if (pContactItemInfo->m_hItem == hItem)
//			return pContactItemInfo;
//	}
//	return NullTreeItemInfo;
//}

void CDlgMyContacts::DeleteItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	CTreeItemInfo* pContactItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hItem);
	if (pContactItemInfo == NULL || pContactItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_CONTACT) return;
	CString sText;
	if (theApp.GetAuthContact() && pContactItemInfo->m_nUserId>0)
	{
		sText.Format(_T("确定删除联系人：\r\n%s(%s) 同时从对方联系人列表中消失吗？"), pContactItemInfo->m_sName.c_str(), pContactItemInfo->m_sAccount.c_str());
	}else
		sText.Format(_T("确定删除联系人：\r\n%s(%s) 吗？"), pContactItemInfo->m_sName.c_str(), pContactItemInfo->m_sAccount.c_str());
	if (CDlgMessageBox::EbDoModal(this,"删除联系人",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	{
#ifdef USES_EBCOM_TEST
		if (theApp.GetAuthContact())
			theEBClientCore->EB_DeleteContact(pContactItemInfo->m_sId,VARIANT_TRUE);
		else
			theEBClientCore->EB_DeleteContact(pContactItemInfo->m_sId,VARIANT_FALSE);
#else
		if (theApp.GetAuthContact())
			theEBAppClient.EB_DeleteContact(pContactItemInfo->m_sId,true);
		else
			theEBAppClient.EB_DeleteContact(pContactItemInfo->m_sId,false);
#endif
	}
}

void CDlgMyContacts::EditItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	CTreeItemInfo* pContactItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hItem);
	if (pContactItemInfo == NULL || pContactItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_CONTACT) return;
#ifdef USES_EBCOM_TEST
	CComPtr<IEB_ContactInfo> pContactInfo = theEBClientCore->EB_GetContactInfo1(pContactItemInfo->m_sId);
	if (pContactInfo==NULL)
		return;
	CDlgContactInfo pDlgContactInfo(this);
	pDlgContactInfo.m_nContactUserId = pContactInfo->ContactUserId;
	pDlgContactInfo.m_nUGId = pContactInfo->UGId;
	pDlgContactInfo.m_nUGId = pContactInfo->UGId;
	pDlgContactInfo.m_sName = pContactInfo->Name.GetBSTR();
	pDlgContactInfo.m_sPhone = pContactInfo->Phone.GetBSTR();
	pDlgContactInfo.m_sCompany = pContactInfo->Company.GetBSTR();
	pDlgContactInfo.m_sJobTitle = pContactInfo->JobTitle.GetBSTR();
	pDlgContactInfo.m_sUrl = pContactInfo->Url.GetBSTR();
	pDlgContactInfo.m_sTel = pContactInfo->Tel.GetBSTR();
	pDlgContactInfo.m_sFax = pContactInfo->Fax.GetBSTR();
	pDlgContactInfo.m_sEmail = pContactInfo->Email.GetBSTR();
	pDlgContactInfo.m_sAddress = pContactInfo->Address.GetBSTR();
	pDlgContactInfo.m_sDescription = pContactInfo->Description.GetBSTR();
	if (pDlgContactInfo.DoModal() == IDOK)
	{
		pContactInfo->Contact = (LPCTSTR)pDlgContactInfo.m_sContact;
		pContactInfo->UGId = pDlgContactInfo.m_nUGId;
		pContactInfo->Name = (LPCTSTR)pDlgContactInfo.m_sName;
		pContactInfo->Phone = (LPCTSTR)pDlgContactInfo.m_sPhone;
		pContactInfo->Company = (LPCTSTR)pDlgContactInfo.m_sCompany;
		pContactInfo->JobTitle = (LPCTSTR)pDlgContactInfo.m_sJobTitle;
		pContactInfo->Url = (LPCTSTR)pDlgContactInfo.m_sUrl;
		pContactInfo->Tel = (LPCTSTR)pDlgContactInfo.m_sTel;
		pContactInfo->Fax = (LPCTSTR)pDlgContactInfo.m_sFax;
		pContactInfo->Email = (LPCTSTR)pDlgContactInfo.m_sEmail;
		pContactInfo->Address = (LPCTSTR)pDlgContactInfo.m_sAddress;
		pContactInfo->Description = (LPCTSTR)pDlgContactInfo.m_sDescription;
		theEBClientCore->EB_EditContact(pContactInfo);
	}
#else
	EB_ContactInfo pContactInfo;
	if (!theEBAppClient.EB_GetContactInfo1(pContactItemInfo->m_sId,&pContactInfo))
		return;
	CDlgContactInfo pDlgContactInfo(this);
	pDlgContactInfo.m_nContactType = pContactInfo.m_nContactType;
	pDlgContactInfo.m_nContactUserId = pContactInfo.m_nContactUserId;
	pDlgContactInfo.m_sContact = pContactInfo.m_sContact.c_str();
	pDlgContactInfo.m_nUGId = pContactInfo.m_nUGId;
	pDlgContactInfo.m_sName = pContactInfo.m_sName.c_str();
	pDlgContactInfo.m_sPhone = pContactInfo.m_sPhone.c_str();
	pDlgContactInfo.m_sCompany = pContactInfo.m_sCompany.c_str();
	pDlgContactInfo.m_sJobTitle = pContactInfo.m_sJobTitle.c_str();
	pDlgContactInfo.m_sUrl = pContactInfo.m_sUrl.c_str();
	pDlgContactInfo.m_sTel = pContactInfo.m_sTel.c_str();
	pDlgContactInfo.m_sFax = pContactInfo.m_sFax.c_str();
	pDlgContactInfo.m_sEmail = pContactInfo.m_sEmail.c_str();
	pDlgContactInfo.m_sAddress = pContactInfo.m_sAddress.c_str();
	pDlgContactInfo.m_sDescription = pContactInfo.m_sDescription.c_str();
	if (pDlgContactInfo.DoModal() == IDOK)
	{
		pContactInfo.m_sContact = (LPCTSTR)pDlgContactInfo.m_sContact;
		pContactInfo.m_nUGId = pDlgContactInfo.m_nUGId;
		pContactInfo.m_sName = (LPCTSTR)pDlgContactInfo.m_sName;
		pContactInfo.m_sPhone = (LPCTSTR)pDlgContactInfo.m_sPhone;
		pContactInfo.m_sCompany = (LPCTSTR)pDlgContactInfo.m_sCompany;
		pContactInfo.m_sJobTitle = (LPCTSTR)pDlgContactInfo.m_sJobTitle;
		pContactInfo.m_sUrl = (LPCTSTR)pDlgContactInfo.m_sUrl;
		pContactInfo.m_sTel = (LPCTSTR)pDlgContactInfo.m_sTel;
		pContactInfo.m_sFax = (LPCTSTR)pDlgContactInfo.m_sFax;
		pContactInfo.m_sEmail = (LPCTSTR)pDlgContactInfo.m_sEmail;
		pContactInfo.m_sAddress = (LPCTSTR)pDlgContactInfo.m_sAddress;
		pContactInfo.m_sDescription = (LPCTSTR)pDlgContactInfo.m_sDescription;
		theEBAppClient.EB_EditContact(&pContactInfo);
	}
#endif
}

void CDlgMyContacts::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo* pContactItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hItem);
	if (pContactItemInfo == NULL || pContactItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_CONTACT) return;
	theApp.m_pAutoCallFromUserIdList.remove(pContactItemInfo->m_nUserId);
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CallAccount(pContactItemInfo->m_sAccount.c_str(),0);	// ??
#else
	theEBAppClient.EB_CallAccount(pContactItemInfo->m_sAccount.c_str(),0);	// ??
#endif
}

//bool CDlgMyContacts::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,int& pState) const
//{
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_CONTACT) return false;
//
//	CEBString sImagePath;
//	bool bIsMemberAccount = false;
//	EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
//#ifdef USES_EBCOM_TEST
//	CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByAccount2(pTreeItemInfo->m_sAccount.c_str());
//	if (pMemberInfo != NULL)
//	{
//		bIsMemberAccount = true;
//		pOutLineState = pMemberInfo->LineState;
//		const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
//		if (PathFileExists(sMemberHeadFile.c_str()))
//		{
//			sImagePath = sMemberHeadFile;
//		}
//	}
//#else
//	EB_MemberInfo pMemberInfo;
//	if (theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,pTreeItemInfo->m_sAccount.c_str()))
//	{
//		bIsMemberAccount = true;
//		pOutLineState = pMemberInfo.m_nLineState;
//		if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
//		{
//			sImagePath = pMemberInfo.m_sHeadResourceFile;
//		}
//	}
//#endif
//	if (bIsMemberAccount)
//	{
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
//		}
//	}else
//	{
//		pImage1 = theApp.m_imageDefaultContact->Clone();
//		return true;
//	}
//}

//int CDlgMyContacts::GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, HICON& pOutIcon) const
//{
//	// 全部显示为灰色
//	return 0;
//}

void CDlgMyContacts::OnViewMsgRecord()
{
	const HTREEITEM hItem = m_treeContacts.GetTrackItem();
	const CTreeItemInfo* pContactItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hItem);
	if (pContactItemInfo == NULL || pContactItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_CONTACT) return;
	const eb::bigint sId = pContactItemInfo->m_nUserId;
	const tstring sName = pContactItemInfo->m_sName;

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
	pDlgMsgRecord->LoadAccountMsgRecord(sId,sName);
}
void CDlgMyContacts::OnDeleteMsgRecord()
{
	const HTREEITEM hItem = m_treeContacts.GetTrackItem();
	const CTreeItemInfo* pContactItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hItem);
	if (pContactItemInfo == NULL || pContactItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_CONTACT) return;
	const eb::bigint sId = pContactItemInfo->m_nUserId;
	const tstring sName = pContactItemInfo->m_sName;

	CString sText;
	sText.Format(_T("确定清空：\r\n%s(%lld) 本地聊天记录吗？"), sName.c_str(),sId);
	if (CDlgMessageBox::EbDoModal(this,"清空聊天记录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("清空聊天记录"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		theApp.DeleteDbRecord(sId,true);
	}
}

void CDlgMyContacts::OnGroupNew()
{
	m_hEditGroupTempItem = m_treeContacts.InsertItem(_T("未命名分组"));
	m_treeContacts.ModifyStyle(0, TVS_EDITLABELS);
	m_treeContacts.EditLabel(m_hEditGroupTempItem);
}
void CDlgMyContacts::OnGroupDelete()
{
	const HTREEITEM hSelectItem = m_treeContacts.GetSelectedItem();
	if (hSelectItem == NULL) return;
	const CTreeItemInfo* pGroupItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hSelectItem);
	if (pGroupItemInfo==NULL || pGroupItemInfo->m_nItemType!=CTreeItemInfo::ITEM_TYPE_GROUP) return;

	CString sText;
	sText.Format(_T("确定删除：\r\n%s 并将该分组所有联系人移到默认分组吗？"), pGroupItemInfo->m_sName.c_str());
	if (CDlgMessageBox::EbDoModal(this,"删除分组",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_DeleteUGInfo(pGroupItemInfo->m_sId);
#else
		theEBAppClient.EB_DeleteUGInfo(pGroupItemInfo->m_sId);
#endif
	}

}
void CDlgMyContacts::OnGroupRename()
{
	const HTREEITEM hSelectItem = m_treeContacts.GetSelectedItem();
	if (hSelectItem == NULL) return;
	const CTreeItemInfo* pGroupItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hSelectItem);
	if (pGroupItemInfo==NULL || pGroupItemInfo->m_nItemType!=CTreeItemInfo::ITEM_TYPE_GROUP) return;

	m_hEditGroupTempItem = hSelectItem;
	m_treeContacts.SetItemText(hSelectItem,pGroupItemInfo->m_sName.c_str());
	m_treeContacts.ModifyStyle(0, TVS_EDITLABELS);
	m_treeContacts.EditLabel(hSelectItem);
}


void CDlgMyContacts::OnContactNew()
{
	if (theApp.GetAuthContact())
	{
		// 添加好友需要验证
		if (theApp.GetFindAppSugId()==0)
			CDlgMessageBox::EbDoModal(this,_T(""),_T("暂时未开放添加好友验证功能：\r\n请联系管理员！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		else
		{
			// 找群找人
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo = theEBClientCore->EB_GetSubscribeFuncInfo(theApp.GetFindAppSugId());
			if (pSubscribeFuncInfo!=NULL)
			{
				theApp.OpenSubscribeFuncWindow(EB_SubscribeFuncInfo(pSubscribeFuncInfo));
			}
#else
			EB_SubscribeFuncInfo pSubscribeFuncInfo;
			if (theEBAppClient.EB_GetSubscribeFuncInfo(theApp.GetFindAppSugId(),&pSubscribeFuncInfo))
			{
				theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
			}
#endif
		}
	}else
	{
		// 添加普通联系人
		eb::bigint nUGId = 0;
		const HTREEITEM hSelectItem = m_treeContacts.GetSelectedItem();
		if (hSelectItem != NULL)
		{
			const CTreeItemInfo* pItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hSelectItem);
			if (pItemInfo!=NULL && pItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
			{
				if (pItemInfo->m_sId!=const_default_group_ugid)
					nUGId = pItemInfo->m_sId;
			}else if (m_treeContacts.GetParentItem(hSelectItem)!=NULL && pItemInfo->m_sParentId!=const_default_group_ugid)
			{
				nUGId = pItemInfo->m_sParentId;
			}
		}
		CDlgContactInfo pDlgContactInfo(this);
		pDlgContactInfo.m_nUGId = nUGId;
		if (pDlgContactInfo.DoModal() == IDOK)
		{
			EB_ContactInfo pPopContactInfo;
			pPopContactInfo.m_sContact = (LPCTSTR)pDlgContactInfo.m_sContact;
			pPopContactInfo.m_nUGId = pDlgContactInfo.m_nUGId;
			pPopContactInfo.m_sName = (LPCTSTR)pDlgContactInfo.m_sName;
			pPopContactInfo.m_sPhone = (LPCTSTR)pDlgContactInfo.m_sPhone;
			pPopContactInfo.m_sCompany = (LPCTSTR)pDlgContactInfo.m_sCompany;
			pPopContactInfo.m_sJobTitle = (LPCTSTR)pDlgContactInfo.m_sJobTitle;
			pPopContactInfo.m_sUrl = (LPCTSTR)pDlgContactInfo.m_sUrl;
			pPopContactInfo.m_sTel = (LPCTSTR)pDlgContactInfo.m_sTel;
			pPopContactInfo.m_sFax = (LPCTSTR)pDlgContactInfo.m_sFax;
			pPopContactInfo.m_sEmail = (LPCTSTR)pDlgContactInfo.m_sEmail;
			pPopContactInfo.m_sAddress = (LPCTSTR)pDlgContactInfo.m_sAddress;
			pPopContactInfo.m_sDescription = (LPCTSTR)pDlgContactInfo.m_sDescription;
#ifdef USES_EBCOM_TEST
			CIEB_ContactInfo * pIEBContactInfo = new CIEB_ContactInfo(pPopContactInfo);
			CComPtr<IEB_ContactInfo> pInterface;
			pIEBContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pInterface);
			theEBClientCore->EB_EditContact(pInterface);
#else
			theEBAppClient.EB_EditContact(&pPopContactInfo);
#endif
		}
	}
}

void CDlgMyContacts::OnNMRClickTreeContacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	EB_FUNC_LOCATION nFuncLocation = EB_FUNC_LOCATION_UNKNOWN;
	theApp.ClearSubscribeSelectInfo();
	if (!theApp.IsLogonVisitor())
	{
		CNewMenu m_menu2;
		m_menu2.CreatePopupMenu();
		m_menu2.SetBitmapBackground(RGB(192,192,192));
		CNewMenu pPopupMenuMoveto;
		const HTREEITEM hSelItem = m_treeContacts.SelectHitTest();
		//const HTREEITEM hSelItem = m_treeContacts.GetSelectedItem();
		const CTreeItemInfo * pTreeItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeContacts.GetItemData(hSelItem);
		if (pTreeItemInfo==NULL)
		{
			m_menu2.InsertODMenu(-1,_T("添加联系人"),MF_BYPOSITION,EB_COMMAND_NEW_CONTACT,IDB_BITMAP_MENU_ADDUSER);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_CONTACT,_T("添加联系人"));
			m_menu2.AppendMenu(MF_SEPARATOR);
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_GROUP,_T("添加分组"));
		}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
		{
			nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER;
			theApp.m_nSelectUserId = pTreeItemInfo->m_nUserId;
			theApp.m_sSelectAccount = pTreeItemInfo->m_sAccount;
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CALL_USER,_T("打开会话"));
			m_menu2.InsertODMenu(-1,_T("添加联系人"),MF_BYPOSITION,EB_COMMAND_NEW_CONTACT,IDB_BITMAP_MENU_ADDUSER);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_CONTACT,_T("添加联系人"));
			m_menu2.AppendMenu(MF_SEPARATOR);
			// 判断聊天记录
			CString sSql;
			sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) LIMIT 1"),
				pTreeItemInfo->m_nUserId,pTreeItemInfo->m_nUserId);
			if (theApp.m_pBoUsers->select(sSql)>0)
			{
				m_menu2.InsertODMenu(-1,_T("聊天记录"),MF_BYPOSITION,EB_MSG_VIEW_MSG_RECORD,IDB_BITMAP_MENU_MSG);
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_MSG_VIEW_MSG_RECORD,_T("聊天记录"));
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_MSG_RECORD,_T("清空聊天记录"));
				m_menu2.AppendMenu(MF_SEPARATOR);
			}
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_CONTACT,_T("查看资料"));
			if (m_pGroupItemInfo.size()>1)
			{
				// 
				pPopupMenuMoveto.CreatePopupMenu();
				BoostReadLock rdlock(m_pGroupItemInfo.mutex());
				CLockMap<mycp::bigint,CTreeItemInfo::pointer>::iterator pIter = m_pGroupItemInfo.begin();
				int nIndex = 0;
				for (; pIter!=m_pGroupItemInfo.end();  pIter++)
				{
					const CTreeItemInfo::pointer& pGroupItemInfo = pIter->second;
					if (pGroupItemInfo->m_sId==const_default_group_ugid || pGroupItemInfo->m_sId==pTreeItemInfo->m_sParentId)
						continue;
					pPopupMenuMoveto.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MY_UGINFO+nIndex,pGroupItemInfo->m_sName.c_str());
					nIndex++;
				}
				m_menu2.AppendMenu(MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuMoveto.m_hMenu,_T("移动至"));
			}
			m_menu2.AppendODMenu(_T("删除联系人(&D)"),MF_BYPOSITION,EB_COMMAND_DELETE_CONTACT,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_CONTACT,_T("删除联系人"));
			m_menu2.AppendMenu(MF_SEPARATOR);
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_GROUP,_T("添加分组"));
		}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
		{
			m_menu2.InsertODMenu(-1,_T("添加联系人"),MF_BYPOSITION,EB_COMMAND_NEW_CONTACT,IDB_BITMAP_MENU_ADDUSER);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_CONTACT,_T("添加联系人"));
			m_menu2.AppendMenu(MF_SEPARATOR);
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_GROUP,_T("添加分组"));
			if (const_default_group_ugid!=pTreeItemInfo->m_sId)
			{
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_RENAME_GROUP,_T("修改分组"));
				m_menu2.AppendODMenu(_T("删除该组(&D)"),MF_BYPOSITION,EB_COMMAND_DELETE_GROUP,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_GROUP,_T("删除该组(&D)"));
			}
		}

		// 应用功能菜单
		CNewMenu pPopupMenuApps;
		if (nFuncLocation != EB_FUNC_LOCATION_UNKNOWN)
		//if (theApp.m_nSelectUserId > 0 || !theApp.m_sSelectAccount.empty())
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
				//m_menu2.AppendMenu(MF_POPUP|MF_BYPOSITION,(UINT)pPopupMenuApps.m_hMenu,_T("集成应用"));
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
}
void CDlgMyContacts::OnSubscribeFunc(UINT nID)
{
	size_t nIndex = nID-EB_COMMAND_SUBSCRIBE_FUNC;
	if (nIndex>=0 && nIndex<theApp.m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[nIndex];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
	}
}
void CDlgMyContacts::OnMyUGInfo(UINT nID)
{
	const HTREEITEM hSelItem = m_treeContacts.GetSelectedItem();
	const CTreeItemInfo * pContactItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeContacts.GetItemData(hSelItem);
	if (pContactItemInfo==NULL || pContactItemInfo->m_nItemType!=CTreeItemInfo::ITEM_TYPE_CONTACT) return;

	const size_t nSelectIndex = nID-EB_COMMAND_MY_UGINFO;
	BoostReadLock rdlock(m_pGroupItemInfo.mutex());
	CLockMap<mycp::bigint,CTreeItemInfo::pointer>::iterator pIter = m_pGroupItemInfo.begin();
	int nIndex = 0;
	for (; pIter!=m_pGroupItemInfo.end();  pIter++)
	{
		const CTreeItemInfo::pointer& pGroupItemInfo = pIter->second;
		if (pGroupItemInfo->m_sId==const_default_group_ugid || pGroupItemInfo->m_sId == pContactItemInfo->m_sParentId)
			continue;
		if (nSelectIndex==nIndex)
		{
			// 移到该组；
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_ContactInfo> pContactInfo = theEBClientCore->EB_GetContactInfo1(pContactItemInfo->m_sId);
			if (pContactInfo==NULL)
				return;
			pContactInfo->UGId = pGroupItemInfo->m_sId;
			theEBClientCore->EB_EditContact(pContactInfo);
#else
			EB_ContactInfo pContactInfo;
			if (!theEBAppClient.EB_GetContactInfo1(pContactItemInfo->m_sId,&pContactInfo))
				return;
			pContactInfo.m_nUGId = pGroupItemInfo->m_sId;
			theEBAppClient.EB_EditContact(&pContactInfo);
#endif
			break;
		}
		nIndex++;
	}

}

void CDlgMyContacts::OnContactEdit()
{
	EditItem(m_treeContacts.GetSelectedItem());
}

void CDlgMyContacts::OnContactDelete()
{
	DeleteItem(m_treeContacts.GetSelectedItem());
}

void CDlgMyContacts::OnCallUser()
{
	const HTREEITEM hSelectItem = m_treeContacts.GetSelectedItem();
	if (hSelectItem==NULL) return;
	CTreeItemInfo* pContactItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(hSelectItem);
	if (pContactItemInfo == NULL || pContactItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_CONTACT) return;
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CallAccount(pContactItemInfo->m_sAccount.c_str(),0);	// ??
#else
	theEBAppClient.EB_CallAccount(pContactItemInfo->m_sAccount.c_str(),0);	// ??
#endif
}

LRESULT CDlgMyContacts::OnTreeItemDblclk(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hDblClkItem = (HTREEITEM)wp;
	this->CallItem(hDblClkItem);
	return 0;
}
LRESULT CDlgMyContacts::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	try
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
			m_btnDeleteTrack.ShowWindow(SW_HIDE);
			m_btnEditTrack.ShowWindow(SW_HIDE);
			m_btnCallTrack.ShowWindow(SW_HIDE);
		}else if (m_btnCallTrack.GetSafeHwnd() != NULL)
		{
			const CTreeItemInfo* pContactItemInfo = (CTreeItemInfo*)m_treeContacts.GetItemData(item);
			if (pContactItemInfo == NULL)
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
			m_treeContacts.GetItemRect(item, &rectItem, TRUE);
			CRect rectHead(rectItem);
			rectHead.right = rectHead.left + m_treeContacts.GetIconWidth();
			this->ClientToScreen(rectHead);
			if (rectHead.PtInRect(pos))
			{
				if (pContactItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
				{
#ifdef USES_EBCOM_TEST
					CComPtr<IEB_ContactInfo> pContactInfo = theEBClientCore->EB_GetContactInfo1(pContactItemInfo->m_sId);
					if (pContactInfo==NULL)
						return 1;
#else
					EB_ContactInfo pContactInfo;
					if (!theEBAppClient.EB_GetContactInfo1(pContactItemInfo->m_sId,&pContactInfo))
						return 1;
#endif
					if (m_pViewContactInfo == NULL)
					{
						m_pViewContactInfo = new CDlgViewContactInfo(this);
						m_pViewContactInfo->Create(CDlgViewContactInfo::IDD,this);
					}
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
					m_pViewContactInfo->SetContactInfo(pContactInfo);
#else
					m_pViewContactInfo->SetContactInfo(&pContactInfo);
#endif
					m_pViewContactInfo->SetMoveEnter();
					//m_pViewContactInfo->ShowWindow(SW_SHOW);
					//m_pViewContactInfo->SetCheckLeave();
				}else if (m_pViewContactInfo!=NULL)// && m_pViewContactInfo->IsWindowVisible())
				{
					m_pViewContactInfo->SetMoveLeave();
					//m_pViewContactInfo->ShowWindow(SW_HIDE);
				}
			}else if (m_pViewContactInfo!=NULL)// && m_pViewContactInfo->IsWindowVisible())
			{
				m_pViewContactInfo->SetMoveLeave();
				//m_pViewContactInfo->ShowWindow(SW_HIDE);
			}
			if (pContactItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
			{
				const int nTop = rectItem.top;
				m_treeContacts.GetClientRect(&rectItem);
				const int nRight = rectItem.right;
				const int const_btn_width = m_btnCallTrack.GetImgWidth();
				m_btnEditTrack.MovePoint(nRight-const_btn_width, nTop);
				m_btnEditTrack.ShowWindow(SW_SHOW);
				m_btnEditTrack.Invalidate();
				if (pContactItemInfo->m_nUserId>0)
				{
					m_btnCallTrack.MovePoint(nRight-const_btn_width*2, nTop);
					m_btnCallTrack.ShowWindow(SW_SHOW);
					m_btnCallTrack.Invalidate();
					m_btnDeleteTrack.ShowWindow(SW_HIDE);
				}else if ((pContactItemInfo->m_nSubType&EB_CONTACT_TYPE_AUTH)==0)
				{
					m_btnDeleteTrack.MovePoint(nRight-const_btn_width*2, nTop);
					m_btnDeleteTrack.ShowWindow(SW_SHOW);
					m_btnDeleteTrack.Invalidate();
					m_btnCallTrack.ShowWindow(SW_HIDE);
				}else
				{
					m_btnDeleteTrack.ShowWindow(SW_HIDE);
					m_btnCallTrack.ShowWindow(SW_HIDE);
				}
			}else
			{
				m_btnCallTrack.ShowWindow(SW_HIDE);
				m_btnDeleteTrack.ShowWindow(SW_HIDE);
				m_btnEditTrack.ShowWindow(SW_HIDE);
			}
		}
	}catch(const std::exception&)
	{
	}catch(...)
	{}
	return 0;
}
BOOL CDlgMyContacts::IsTreeCtrlEditMessage(WPARAM KeyCode)  
{  
    // pWnd is a pointer to either an active CPropertyPage of the modal  
    // CPropertySheet or a modal CDialog object.  
    CWnd*   pWnd = this;  
#if MODAL_PROPERTYSHEET  
    pWnd = GetActivePage();  
#endif  
  
    // If the edit control of the tree view control has the input focus,  
    // sending a WM_KEYDOWN message to the edit control will dismiss the  
    // edit control.  When ENTER key was sent to the edit control, the  
    // parentwindow of the tree view control is responsible for updating  
    // the item's label in TVN_ENDLABELEDIT notification code.  
    const CWnd*  focus = GetFocus();
    const CEdit* edit  = m_treeContacts.GetEditControl();  
    if ((CEdit *) focus == edit)  
    {
		const HTREEITEM hTempItem = m_hEditGroupTempItem;
		if (KeyCode==VK_ESCAPE && hTempItem != NULL)	// **需要先设为空，保证OnTvnEndlabeleditTreeContacts不会处理
		{
			m_treeContacts.ModifyStyle(TVS_EDITLABELS,0);
			m_hEditGroupTempItem = NULL;
		}
        edit->SendMessage(WM_KEYDOWN, KeyCode);
		if (KeyCode==VK_ESCAPE && hTempItem != NULL)
		{
			const DWORD dwItemData = m_treeContacts.GetItemData(hTempItem);
			if (dwItemData == 0)	// 新建取消
			{
				m_treeContacts.DeleteItem(hTempItem);
			}else
			{
				const CTreeItemInfo* pGroupItemInfo = (CTreeItemInfo*)dwItemData;
				if (pGroupItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
				{
					CString sGroupText;
					sGroupText.Format(_T("%s(%d)"), pGroupItemInfo->m_sName.c_str(), pGroupItemInfo->m_dwItemData);
					m_treeContacts.SetItemText(pGroupItemInfo->m_hItem,sGroupText);
				}
			}
		}
		return TRUE;
	}
 //   edit  = m_treeItem.GetEditControl();  
 //   if ((CEdit *) focus == edit)  
 //   {  
 //       edit->SendMessage(WM_KEYDOWN, KeyCode);
	//	return TRUE;
	//}
    return FALSE;  
}

BOOL CDlgMyContacts::PreTranslateMessage(MSG* pMsg)
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

	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnDeleteTrack.GetSafeHwnd())
	{
		SetTimer(TIMERID_DELETE_ITEM,1,NULL);
	}
	else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnEditTrack.GetSafeHwnd())
	{
		SetTimer(TIMERID_EDIT_ITEM,1,NULL);
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeContacts.GetSafeHwnd())
	{
		CallItem(m_treeContacts.GetSelectedItem());
	}
	else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnCallTrack.GetSafeHwnd())
	{
		CallItem(m_treeContacts.GetTrackItem());
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgMyContacts::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_DELETE_ITEM == nIDEvent)
	{
		KillTimer(nIDEvent);
		DeleteItem(m_treeContacts.GetTrackItem());
	}
	else if (TIMERID_EDIT_ITEM == nIDEvent)
	{
		KillTimer(nIDEvent);
		EditItem(m_treeContacts.GetTrackItem());
	}
	else if (TIMERID_CHECK_ITEM_HOT==nIDEvent)
	{
		KillTimer(TIMERID_CHECK_ITEM_HOT);
		if (m_hCurrentHotItem!=NULL && m_hCurrentHotItem==m_treeContacts.GetTrackItem())
		{
			this->PostMessage(WM_ITEM_TRACK_HOT,(WPARAM)m_treeContacts.GetTrackItem(),(LPARAM)&m_treeContacts);
		}
	}

	__super::OnTimer(nIDEvent);
}

void CDlgMyContacts::OnTvnEndlabeleditTreeContacts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (m_hEditGroupTempItem==NULL) return;

    const CEdit* edit = m_treeContacts.GetEditControl();
	if (edit == NULL) return;

	const HTREEITEM hEditItem = pTVDispInfo->item.hItem;
	CString sNewItemText;
	edit->GetWindowText(sNewItemText);
	if (sNewItemText.IsEmpty())
	{
		CDlgMessageBox::EbMessageBox(this,_T(""),_T("分组名称不能为空：\r\n请重新输入！"),CDlgMessageBox::IMAGE_ERROR,5);
		m_treeContacts.EditLabel(hEditItem);
		return;
	}
	const CTreeItemInfo * pTreeItemInfo = hEditItem==NULL?NULL:(const CTreeItemInfo*)m_treeContacts.GetItemData(hEditItem);
	if (pTreeItemInfo == NULL)
	{
		// 添加分组
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_EditUGInfo(0,(LPCTSTR)sNewItemText);
#else
		theEBAppClient.EB_EditUGInfo(0,(LPCTSTR)sNewItemText);
#endif
		// 删除原来旧的ITEM
		m_treeContacts.DeleteItem(hEditItem);
	}else if (sNewItemText != pTreeItemInfo->m_sName.c_str())
	{
		// 修改目录
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_EditUGInfo(pTreeItemInfo->m_sId,(LPCTSTR)sNewItemText);
#else
		theEBAppClient.EB_EditUGInfo(pTreeItemInfo->m_sId,(LPCTSTR)sNewItemText);
#endif
	}else if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP)
	{
		// 分组内容没有修改
		CString sGroupText;
		sGroupText.Format(_T("%s(%d)"), pTreeItemInfo->m_sName.c_str(), pTreeItemInfo->m_dwItemData);
		m_treeContacts.SetItemText(pTreeItemInfo->m_hItem,sGroupText);
	}
	m_treeContacts.ModifyStyle(TVS_EDITLABELS,0);
	m_hEditGroupTempItem = NULL;
}
