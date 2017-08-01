// DlgMySession.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgMySession.h"
#include "DlgViewContactInfo.h"
#include "DlgRequestAddContact.h"

#define TIMERID_LOAD_CALL_RECORD	200
#define TIMERID_CHECK_ITEM_HOT		202

// CDlgMySession dialog

IMPLEMENT_DYNAMIC(CDlgMySession, CDialog)

CDlgMySession::CDlgMySession(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMySession::IDD, pParent)
{
	m_pViewContactInfo = NULL;
	m_hCurrentHotItem = NULL;
	m_bReloadRecordData = false;
}

CDlgMySession::~CDlgMySession()
{
}

void CDlgMySession::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SESSION, m_treeSession);
}


BEGIN_MESSAGE_MAP(CDlgMySession, CDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_SESSION, OnNMRClickTreeSession)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDblclk)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_COMMAND(EB_COMMAND_DELETE_SESSION, OnSessionDeleteSes)
	ON_MESSAGE(EB_COMMAND_DELETE_SESSION, OnMsgDeleteSes)
	ON_MESSAGE(EB_COMMAND_NEW_CONTACT, OnMsgNewContact)
	ON_COMMAND(EB_COMMAND_CLEAR_SESSION, OnSessionClearSes)
	ON_WM_DESTROY()
	ON_COMMAND(EB_COMMAND_CALL_USER, OnCallUser)
	ON_COMMAND(EB_MSG_VIEW_MSG_RECORD, OnViewMsgRecord)
	//ON_COMMAND(EB_COMMAND_DELETE_MSG_RECORD, OnDeleteMsgRecord)
	ON_COMMAND(EB_COMMAND_VIEW_GROUP_SHARE, &CDlgMySession::OnGroupShare)
	ON_COMMAND_RANGE(EB_COMMAND_SUBSCRIBE_FUNC,EB_COMMAND_SUBSCRIBE_FUNC+0x20,OnSubscribeFunc)

END_MESSAGE_MAP()


// CDlgMySession message handlers
void CDlgMySession::SetCtrlColor(void)
{
	if (m_pViewContactInfo!=NULL && m_pViewContactInfo->GetSafeHwnd()!=NULL)
		m_pViewContactInfo->SetCtrlColor();

	//if (theApp.GetColorFlat())
	//	m_treeSession.SetBkGradients(theApp.GetFlatBgColor(),theApp.GetFlatBgColor());
	//else
	//	m_treeSession.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());
}
BOOL CDlgMySession::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnDeleteTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSession, 0xffff);
	m_btnDeleteTrack.SetAutoSize(false);
	m_btnDeleteTrack.SetAutoFocus(true);
	m_btnDeleteTrack.Load(IDB_PNG_HOT_DELETE);
	m_btnDeleteTrack.SetToolTipText(_T("删除会话记录"));
	m_btnAddContact.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSession, 0xffff);
	m_btnAddContact.SetAutoSize(false);
	m_btnAddContact.SetAutoFocus(true);
	m_btnAddContact.Load(IDB_PNG_HOT_NEW);
	//if (theApp.GetAuthContact())
	//	m_btnAddContact.SetToolTipText(_T("点击申请添加好友"));
	//else
	//	m_btnAddContact.SetToolTipText(_T("添加到我的联系人"));
	m_btnCallTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeSession, 0xffff);
	m_btnCallTrack.SetAutoSize(false);
	m_btnCallTrack.SetAutoFocus(true);
	m_btnCallTrack.Load(IDB_PNG_HOT_CALL);
	m_btnCallTrack.SetToolTipText(_T("打开会话"));
	m_treeSession.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeSession.SetBkGradients(theDefaultFlatBgColor,theDefaultFlatBgColor);
	//m_treeSession.SetIntervalLine(true,theDefaultBtnWhiteColor,theDefaultFlatBgColor);
	m_treeSession.SetCallback((CTreeCallback*)this);
	m_treeSession.ModifyStyle(TVS_SHOWSELALWAYS, TVS_SINGLEEXPAND);
	//m_treeSession.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeSession.SetItemHeight(40);
	m_treeSession.SetIconSize(32,32);
	//m_treeSession.SetItemIcon( theApp.GetIconCon() );

	SetCtrlColor();

	if (!theApp.IsLogonVisitor())
	{
		SetTimer(TIMERID_LOAD_CALL_RECORD,10,NULL);
	}

	//SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMySession::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_treeSession.GetSafeHwnd())
		m_treeSession.MoveWindow(0, 0, cx, cy);
}
void CDlgMySession::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}

void CDlgMySession::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}


bool CDlgMySession::GetItemImage(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem,Gdiplus::Image*& pImage1,Gdiplus::Image*& pImage2,Gdiplus::Image*& pImage3,int& pState) const
{
	CCallRecordInfo::pointer pCallRecordInfo;
	if (!m_pCallRecordInfo.find(hItem,pCallRecordInfo))
		return false;

	//if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
	//{
	//	// 广播消息，验证消息
	//	pImage1 = theApp.m_imageDefaultContact->Clone();
	//	return true;
	//}else
	//{
	//	return theApp.GetItemImage(pTreeCtrl,hItem,pImage1,pImage2,pState);
	//}

	if (pCallRecordInfo->m_sGroupCode>0)
	{
#ifdef USES_EBCOM_TEST
		short nGroupType = theEBClientCore->EB_GetGroupType(pCallRecordInfo->m_sGroupCode);
		if (nGroupType >= 0)
#else
		EB_GROUP_TYPE nGroupType = EB_GROUP_TYPE_DEPARTMENT;
		if (theEBAppClient.EB_GetGroupType(pCallRecordInfo->m_sGroupCode,nGroupType))
#endif
		{
			switch (nGroupType)
			{
			case EB_GROUP_TYPE_DEPARTMENT:
				pImage1 = theApp.m_imageDefaultDepartment->Clone();
				return true;
			case EB_GROUP_TYPE_PROJECT:
				pImage1 = theApp.m_imageDefaultProject->Clone();
				return true;
			case EB_GROUP_TYPE_GROUP:
				pImage1 = theApp.m_imageDefaultGroup->Clone();
				return true;
			default:
				break;
			}
		}
		// 默认讨论组
		pImage1 = theApp.m_imageDefaultTempGroup->Clone();
		return true;
	}

	if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
	{
		// 广播消息，验证消息
		pImage1 = theApp.m_imageDefaultContact->Clone();
		return true;
	}else if (pCallRecordInfo->m_sMemberCode==0)
	{
		pImage1 = theApp.m_imageDefaultContact->Clone();
		return true;
	}else
	{
		//return theApp.GetItemImage(pTreeCtrl,hItem,pImage1,pImage2,pState);
		CEBString sImagePath;
		bool bIsMemberAccount = false;
		EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pCallRecordInfo->m_sMemberCode);
		if (pMemberInfo = NULL)
			pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId2(pCallRecordInfo->m_nFromUserId);
		if (pMemberInfo != NULL)
		{
			bIsMemberAccount = true;
			pOutLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
			const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
			if (PathFileExists(sMemberHeadFile.c_str()))
			{
				sImagePath = sMemberHeadFile;
			}
		}
#else
		EB_MemberInfo pMemberInfo;
		if (theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,pCallRecordInfo->m_sMemberCode) ||
			theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,pCallRecordInfo->m_nFromUserId))
		{
			bIsMemberAccount = true;
			pOutLineState = pMemberInfo.m_nLineState;
			if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
			{
				sImagePath = pMemberInfo.m_sHeadResourceFile;
			}
		}
#endif
		if (bIsMemberAccount)
		{
			switch (pOutLineState)
			{
			case EB_LINE_STATE_UNKNOWN:
			case EB_LINE_STATE_OFFLINE:
				pState = 0;
				break;
			case EB_LINE_STATE_ONLINE_NEW:
				break;
			case EB_LINE_STATE_BUSY:
				{
					pImage2 = theApp.m_imageStateBusy->Clone();
				}break;
			case EB_LINE_STATE_AWAY:
				{
					pImage2 = theApp.m_imageStateAway->Clone();
				}break;
			default:
				break;
			}
			if (!sImagePath.empty())
			{
				pImage1 = libEbc::LoadImageFromFile(sImagePath.c_str());
				if (pImage1==NULL)
				{
					USES_CONVERSION;
					pImage1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
				}
				return true;
			}else
			{
				pImage1 = theApp.m_imageDefaultMember->Clone();
				return true;
			}
		}else
		{
			// contact
			if (pCallRecordInfo->m_nFromUserId>0)
			{
				EB_ContactInfo pContactInfo;
				theEBAppClient.EB_GetContactInfo2(pCallRecordInfo->m_nFromUserId,&pContactInfo);
				pOutLineState = pContactInfo.m_nLineState;
			}else if (!theApp.GetAuthContact())
			{
				pOutLineState = EB_LINE_STATE_ONLINE_NEW;
			}

			switch (pOutLineState)
			{
			case EB_LINE_STATE_UNKNOWN:
			case EB_LINE_STATE_OFFLINE:
				pState = 0;	// 用灰色
				break;
			default:
				break;
			}
			//pState = 0;	// 非成员，全部用灰色
			pImage1 = theApp.m_imageDefaultContact->Clone();
		}
		return true;
	}
	return false;
}
COLORREF CDlgMySession::GetItemTextColor(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, COLORREF colorDefault) const
{
	CCallRecordInfo::pointer pCallRecordInfo;
	if (!m_pCallRecordInfo.find(hItem,pCallRecordInfo))
		return colorDefault;
	return pCallRecordInfo->m_bRead?colorDefault:RGB(255,0,128);
}

//HICON CDlgMySession::GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
//{
//	CCallRecordInfo::pointer pCallRecordInfo;
//	if (!m_pCallRecordInfo.find(hItem,pCallRecordInfo))
//		return NULL;
//	if (pCallRecordInfo->m_sGroupCode==0)
//		return theApp.GetIconCon();
//	else
//		return theApp.GetIconDep();
//}
//
//int CDlgMySession::GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, HICON& pOutIcon) const
//{
//	return 1;
//}

int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   time_t t1 = (time_t)lParam1;
   time_t t2 = (time_t)lParam2;
   if (t1 == t2) return 0;
   return t1>t2?-1:1;
}

CCallRecordInfo::pointer CDlgMySession::GetCallRecordInfo(eb::bigint sDepCode, eb::bigint sAccount,int nMsgType) const
//CCallRecordInfo::pointer CDlgMySession::GetCallRecordInfo(eb::bigint sDepCode, eb::bigint sAccount,bool bSysMsg) const
{
	BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_pCallRecordInfo.mutex()));
	//boost::mutex::scoped_lock lock(const_cast<boost::mutex&>(m_pCallRecordInfo.mutex()));
	CLockMap<HTREEITEM, CCallRecordInfo::pointer>::const_iterator pIter = m_pCallRecordInfo.begin();
	for (; pIter!=m_pCallRecordInfo.end(); pIter++)
	{
		const CCallRecordInfo::pointer& pCallRecordInfo = pIter->second;
		if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
		//if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0 &&
		//	(pCallRecordInfo->m_sGroupCode>0 || pCallRecordInfo->m_nFromUserId>0))
		{
			// 验证消息
			if (nMsgType>0)
			////if (bSysMsg)
			{
				if ((sDepCode>0 && sDepCode==pCallRecordInfo->m_sGroupCode) ||
					(sAccount>0 && sAccount==pCallRecordInfo->m_nFromUserId) ||
					(nMsgType==EBC_MSG_TYPE_BC_MSG && pCallRecordInfo->m_nFromType==EBC_MSG_TYPE_BC_MSG))	// 消息广播只显示当前一条
				{
					return pCallRecordInfo;
				}
			}
			continue;
		}
		if (sDepCode>0)
		{
			// 查询群组
			if (pCallRecordInfo->m_sGroupCode==sDepCode)
			{
				return pCallRecordInfo;
			}
		}else
		{
			// 查询帐号
			if (pCallRecordInfo->m_sGroupCode==0 && pCallRecordInfo->m_nFromUserId==sAccount)
			{
				return pCallRecordInfo;
			}
		}
	}
	return NullCallRecordInfo;
}

void CDlgMySession::InsertCallRecord(const CCallRecordInfo::pointer& pCallRecordInfo, bool bInsertNew)
//void CDlgMySession::InsertCallRecord(const CCallRecordInfo::pointer& pCallRecordInfo,bool bSysMsg, bool bInsertNew)
{
	CCallRecordInfo::pointer pOldCallInfo = GetCallRecordInfo(pCallRecordInfo->m_sGroupCode,pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_nFromType);
	if (pOldCallInfo.get()!=NULL)
	{
		if (bInsertNew)
		{
			m_treeSession.DeleteItem(pOldCallInfo->m_hItem);
			m_pCallRecordInfo.remove(pOldCallInfo->m_hItem);
		}
		if (pCallRecordInfo->m_sGroupCode>0 || !is_visitor_uid(pCallRecordInfo->m_nFromUserId))
		{
			if (bInsertNew)
			{
				CString sSql;
				sSql.Format(_T("DELETE FROM call_record_t WHERE call_id=%lld"),pOldCallInfo->m_sCallId);
				theApp.m_pBoUsers->execute(sSql);
			}
			//CString sSql;
			//if (bInsertNew)
			//	sSql.Format(_T("DELETE FROM call_record_t WHERE call_id=%lld"),pOldCallInfo->m_sCallId);
			//else
			//	sSql.Format(_T("DELETE FROM call_record_t WHERE call_id=%lld"),pCallRecordInfo->m_sCallId);
			//theApp.m_pBoUsers->execute(sSql);
		}
		if (!bInsertNew)
			return;
	}

	// 获取日期字符串
	const CTime pNow = CTime::GetCurrentTime();
	const CTime pTime(pCallRecordInfo->m_tTime);
	CString sCallTime;
	if (pNow.GetYear()==pTime.GetYear() && pNow.GetMonth()==pTime.GetMonth() && pNow.GetDay()==pTime.GetDay())
	{
		// today
		sCallTime = pTime.Format(_T("%H:%M"));
	}else if (pNow.GetYear()==pTime.GetYear() && pNow.GetMonth()==pTime.GetMonth() && pNow.GetDay()==pTime.GetDay()+1)
	{
		// 昨天
		//sCallTime =_T("昨天");
		sCallTime = pTime.Format(_T("昨天 %H:%M"));
	}else if (pNow.GetYear()==pTime.GetYear())
	{
		// 今年
		sCallTime = pTime.Format(_T("%m-%d"));
	}else
	{
		sCallTime = pTime.Format(_T("%Y-%m-%d"));
	}
	CString sItemText;
	if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
	//if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0 &&
	//	(pCallRecordInfo->m_sGroupCode>0 || pCallRecordInfo->m_nFromUserId>0))
	{
		// 验证消息
		switch (pCallRecordInfo->m_nFromType)
		{
		case EBC_MSG_TYPE_BC_MSG:				// 广播消息
			sItemText.Format(_T("广播消息\n%s"),pCallRecordInfo->m_sGroupName.c_str());
			break;
		case EBC_MSG_TYPE_REQUEST_JOIN_2_GROUP:		// 请求加入群组
			sItemText.Format(_T("验证消息\n%s申请加入群组%lld"),pCallRecordInfo->m_sFromAccount.c_str(),pCallRecordInfo->m_sGroupCode);
			break;
		case EBC_MSG_TYPE_INVITE_JOIN_2_GROUP:	// 邀请加入群组
			sItemText.Format(_T("验证消息\n%s邀请加入群组%lld"),pCallRecordInfo->m_sFromAccount.c_str(),pCallRecordInfo->m_sGroupCode);
			break;
		case EBC_MSG_TYPE_REQUEST_ADD_CONTACT:
			sItemText.Format(_T("验证消息\n%s邀请加为好友"),pCallRecordInfo->m_sFromAccount.c_str());
			break;
		//case EBC_MSG_TYPE_ACCERPT_ADD_CONTACT:
		//	sItemText.Format(_T("通知消息\n%s成功加为好友"),pCallRecordInfo->m_sFromAccount.c_str());
		//	break;
		case EBC_MSG_TYPE_REJECT_ADD_CONTACT:
			sItemText.Format(_T("通知消息\n%s拒绝加为好友"),pCallRecordInfo->m_sFromAccount.c_str());
			break;
		default:
			sItemText.Format(_T("验证消息\n帐号：%s 群组：%lld"),pCallRecordInfo->m_sFromAccount.c_str(),pCallRecordInfo->m_sGroupCode);
			break;
		}
	}else if (pCallRecordInfo->m_sGroupCode==0)
	{
//#ifdef _DEBUG
//		sItemText.Format(_T("%s - %lld\n%s"),pCallRecordInfo->m_sFromName.c_str(),pCallRecordInfo->m_nFromUserId,sCallTime);
//#else
		sItemText.Format(_T("%s [%s]"),pCallRecordInfo->m_sFromName.c_str(),sCallTime);
//#endif
	}else
	{
		sItemText.Format(_T("%s [%s]"),pCallRecordInfo->m_sGroupName.c_str(),sCallTime);
	}
	HTREEITEM hItem = m_treeSession.InsertItem(sItemText);
	m_treeSession.SetItemData(hItem,(DWORD)pCallRecordInfo->m_tTime);
	pCallRecordInfo->m_hItem = hItem;
	m_pCallRecordInfo.insert(hItem,pCallRecordInfo);

	// 排序
	TVSORTCB tvs;
	tvs.hParent = TVI_ROOT;
	tvs.lpfnCompare = MyCompareProc;
	tvs.lParam = (LPARAM)0;
	m_treeSession.SortChildrenCB(&tvs);
}

void CDlgMySession::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TIMERID_LOAD_CALL_RECORD == nIDEvent)
	{
		KillTimer(TIMERID_LOAD_CALL_RECORD);
		const CString sSql = _T("SELECT call_id,Datetime(call_time,'localtime'),dep_code,dep_name,emp_code,from_uid,from_phone,from_account,from_name,from_type,company,title,tel,email FROM call_record_t ORDER BY call_time desc LIMIT 50");
		int nCookie = 0;
		const int nCount = theApp.m_pBoUsers->select(sSql, nCookie);
		m_bReloadRecordData = nCount==50?true:false;
		cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
		while (pRecord.get()!=NULL)
		{
			CCallRecordInfo::pointer pCallRecordInfo = CCallRecordInfo::create();
			pCallRecordInfo->m_sCallId = pRecord->getVector()[0]->getBigIntValue();
			//pCallRecordInfo->m_sCallId = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[0]->v.varcharVal);
			const tstring sCallTime = pRecord->getVector()[1]->getStrValue();
			time_t nCallTime = 0;
			libEbc::ChangeTime(sCallTime.c_str(),nCallTime);
			pCallRecordInfo->m_tTime = nCallTime;
			pCallRecordInfo->m_sGroupCode = pRecord->getVector()[2]->getBigIntValue();
			//pCallRecordInfo->m_sGroupCode = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[2]->v.varcharVal);
			pCallRecordInfo->m_sGroupName = libEbc::UTF82ACP(pRecord->getVector()[3]->getStrValue().c_str());
			pCallRecordInfo->m_sMemberCode = pRecord->getVector()[4]->getBigIntValue();
			//pCallRecordInfo->m_sMemberCode = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[4]->v.varcharVal);
			pCallRecordInfo->m_nFromUserId = pRecord->getVector()[5]->getBigIntValue();
			pCallRecordInfo->m_nFromPhone = pRecord->getVector()[6]->getBigIntValue();
			pCallRecordInfo->m_sFromAccount = pRecord->getVector()[7]->getStrValue();
			pCallRecordInfo->m_sFromName = libEbc::UTF82ACP(pRecord->getVector()[8]->getStrValue().c_str());
			pCallRecordInfo->m_nFromType = pRecord->getVector()[9]->getIntValue();
			pCallRecordInfo->m_sCompany = libEbc::UTF82ACP(pRecord->getVector()[10]->getStrValue().c_str());
			pCallRecordInfo->m_sTitle = libEbc::UTF82ACP(pRecord->getVector()[11]->getStrValue().c_str());
			//pCallRecordInfo->m_sPhone = BODB_BUFFER_TEXT(pResltSet->rsvalues[i]->fieldvalues[12]->v.varcharVal);
			pCallRecordInfo->m_sTel = pRecord->getVector()[12]->getStrValue();
			pCallRecordInfo->m_sEmail = pRecord->getVector()[13]->getStrValue();
			InsertCallRecord(pCallRecordInfo,false);
			pRecord = theApp.m_pBoUsers->next(nCookie);
		}
		theApp.m_pBoUsers->reset(nCookie);
	}else if (TIMERID_CHECK_ITEM_HOT==nIDEvent)
	{
		KillTimer(TIMERID_CHECK_ITEM_HOT);
		if (m_hCurrentHotItem!=NULL && m_hCurrentHotItem==m_treeSession.GetTrackItem())
		{
			this->PostMessage(WM_ITEM_TRACK_HOT,(WPARAM)m_treeSession.GetTrackItem(),(LPARAM)&m_treeSession);
		}
	}
	__super::OnTimer(nIDEvent);
}

LRESULT CDlgMySession::OnTreeItemDblclk(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hDblClkItem = (HTREEITEM)wp;
	CallItem(hDblClkItem);
	return 0;
}
LRESULT CDlgMySession::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
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
		m_btnCallTrack.ShowWindow(SW_HIDE);
		m_btnAddContact.ShowWindow(SW_HIDE);
	}else if (m_btnCallTrack.GetSafeHwnd() != NULL)
	{
		if (m_pViewContactInfo == NULL)
		{
			m_pViewContactInfo = new CDlgViewContactInfo(this);
			m_pViewContactInfo->Create(CDlgViewContactInfo::IDD,this);
		}

		CPoint pos;
		GetCursorPos(&pos);
		SetTimer(TIMERID_CHECK_ITEM_HOT,1000,NULL);
		CRect rectItem;
		m_treeSession.GetItemRect(item, &rectItem, TRUE);
		CRect rectHead(rectItem);
		rectHead.right = rectHead.left + m_treeSession.GetIconWidth();
		this->ClientToScreen(rectHead);
		const BOOL bPtInHeadRect = rectHead.PtInRect(pos);
		if (bPtInHeadRect)
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
			//m_pViewContactInfo->ShowWindow(SW_HIDE);
			m_pViewContactInfo->SetCircle();
		}
		//bool bCanCall = false;
		bool bAddMyContact = false;
		CCallRecordInfo::pointer pCallRecordInfo;
		if (m_pCallRecordInfo.find(item,pCallRecordInfo))
		{
			pCallRecordInfo->m_bRead = true;
			if (pCallRecordInfo.get() != NULL && pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
			{
				// 系统消息
				if (m_pViewContactInfo!=0)// && m_pViewContactInfo->IsWindowVisible())
				{
					m_pViewContactInfo->SetMoveLeave();
					//m_pViewContactInfo->HideReset();
				}
				m_btnDeleteTrack.ShowWindow(SW_HIDE);
				m_btnCallTrack.ShowWindow(SW_HIDE);
				m_btnAddContact.ShowWindow(SW_HIDE);
				return 0;
			}
			if (bPtInHeadRect)
			{
#ifdef USES_EBCOM_TEST
				if (pCallRecordInfo->m_sGroupCode==0)
				{
					if (pCallRecordInfo->m_sMemberCode==0 || pCallRecordInfo->m_nFromType != EB_ACCOUNT_TYPE_IN_ENT)
					{
						CComPtr<IEB_ContactInfo> pContactInfo = theEBClientCore->EB_GetContactInfo2(pCallRecordInfo->m_nFromUserId);
						if (pContactInfo!=NULL)
						{
							m_pViewContactInfo->SetContactInfo(pContactInfo);
						}else
						{
							bAddMyContact = true;
							EB_ContactInfo pPopContactInfo;
							pPopContactInfo.m_sContact = pCallRecordInfo->m_sFromAccount;
							pPopContactInfo.m_nContactUserId = pCallRecordInfo->m_nFromUserId;
							pPopContactInfo.m_sGroupName = pCallRecordInfo->m_sGroupName;
							pPopContactInfo.m_sName = pCallRecordInfo->m_sFromName;
							pPopContactInfo.m_sCompany = pCallRecordInfo->m_sCompany;
							pPopContactInfo.m_sJobTitle = pCallRecordInfo->m_sTitle;
							if (pCallRecordInfo->m_nFromPhone!=0)
							{
								char sPhoneString[24];
								sprintf(sPhoneString,"%lld",pCallRecordInfo->m_nFromPhone);
								pPopContactInfo.m_sPhone = sPhoneString;
							}
							pPopContactInfo.m_sTel = pCallRecordInfo->m_sTel;
							pPopContactInfo.m_sEmail = pCallRecordInfo->m_sEmail;
							CIEB_ContactInfo * pIEBContactInfo = new CIEB_ContactInfo(pPopContactInfo);
							pIEBContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pContactInfo);
							m_pViewContactInfo->SetContactInfo(pContactInfo);
						}
						//if (bPtInHeadRect)
						//	m_pViewContactInfo->ShowWindow(SW_SHOW);
						//else if (m_pViewContactInfo->IsWindowVisible())
						//	m_pViewContactInfo->ShowWindow(SW_HIDE);
					}else
					{
						CComPtr<IEB_MemberInfo> pMemberInfo = theEBClientCore->EB_GetMemberInfoByMemberCode(pCallRecordInfo->m_sMemberCode);
						if (pMemberInfo = NULL)
							pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId2(pCallRecordInfo->m_nFromUserId);
						if (pMemberInfo != NULL)
						{
							CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pMemberInfo->GroupCode);
							m_pViewContactInfo->SetEmployeeInfo(pMemberInfo,pGroupInfo);
							//if (bPtInHeadRect)
							//	m_pViewContactInfo->ShowWindow(SW_SHOW);
							//else if (m_pViewContactInfo->IsWindowVisible())
							//	m_pViewContactInfo->ShowWindow(SW_HIDE);
						}else
						{
							bAddMyContact = true;
							//if (m_pViewContactInfo->IsWindowVisible())
							//	m_pViewContactInfo->ShowWindow(SW_HIDE);
						}
					}
				}else
				{
					CComPtr<IEB_GroupInfo> pGroupInfo = theEBClientCore->EB_GetGroupInfo(pCallRecordInfo->m_sGroupCode);
					if (pGroupInfo != NULL)
					{
						CComPtr<IEB_EnterpriseInfo> pEnterpriseInfo;
						if (pGroupInfo->EnterpriseCode > 0)
							pEnterpriseInfo = theEBClientCore->EB_GetEnterpriseInfo(pGroupInfo->EnterpriseCode);
						m_pViewContactInfo->SetDepartemntInfo(pGroupInfo,pEnterpriseInfo);
					}else
					{
						EB_ContactInfo pPopContactInfo;
						pPopContactInfo.m_sContact = pCallRecordInfo->m_sFromAccount;
						pPopContactInfo.m_nContactUserId = pCallRecordInfo->m_nFromUserId;
						pPopContactInfo.m_sGroupName = pCallRecordInfo->m_sGroupName;
						pPopContactInfo.m_sName = pCallRecordInfo->m_sFromName;
						pPopContactInfo.m_sCompany = pCallRecordInfo->m_sCompany;
						pPopContactInfo.m_sJobTitle = pCallRecordInfo->m_sTitle;
						if (pCallRecordInfo->m_nFromPhone!=0)
						{
							char sPhoneString[24];
							sprintf(sPhoneString,"%lld",pCallRecordInfo->m_nFromPhone);
							pPopContactInfo.m_sPhone = sPhoneString;
						}
						pPopContactInfo.m_sTel = pCallRecordInfo->m_sTel;
						pPopContactInfo.m_sEmail = pCallRecordInfo->m_sEmail;
						CIEB_ContactInfo * pIEBContactInfo = new CIEB_ContactInfo(pPopContactInfo);
						CComPtr<IEB_ContactInfo> pContactInfo;
						pIEBContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pContactInfo);
						m_pViewContactInfo->SetContactInfo(pContactInfo);
					}
					//if (bPtInHeadRect && !m_pViewContactInfo->IsWindowVisible())
					//	m_pViewContactInfo->ShowWindow(SW_SHOW);
					//else if (m_pViewContactInfo->IsWindowVisible())
					//	m_pViewContactInfo->ShowWindow(SW_HIDE);
				}
#else
				if (pCallRecordInfo->m_sGroupCode==0)
				{
					if (pCallRecordInfo->m_sMemberCode==0 || pCallRecordInfo->m_nFromType != EB_ACCOUNT_TYPE_IN_ENT)
					{
						EB_ContactInfo pContactInfo;
						if (theEBAppClient.EB_GetContactInfo2(pCallRecordInfo->m_nFromUserId,&pContactInfo))
						{
							m_pViewContactInfo->SetContactInfo(&pContactInfo);
						}else
						{
							bAddMyContact = true;
							pContactInfo.m_nContactUserId = pCallRecordInfo->m_nFromUserId;
							pContactInfo.m_sContact = pCallRecordInfo->m_sFromAccount;
							pContactInfo.m_sName = pCallRecordInfo->m_sFromName;
							pContactInfo.m_sGroupName = pCallRecordInfo->m_sGroupName;
							pContactInfo.m_sCompany = pCallRecordInfo->m_sCompany;
							pContactInfo.m_sJobTitle = pCallRecordInfo->m_sTitle;
							if (pCallRecordInfo->m_nFromPhone!=0)
							{
								char sPhoneString[24];
								sprintf(sPhoneString,"%lld",pCallRecordInfo->m_nFromPhone);
								pContactInfo.m_sPhone = sPhoneString;
							}
							pContactInfo.m_sTel = pCallRecordInfo->m_sTel;
							pContactInfo.m_sEmail = pCallRecordInfo->m_sEmail;
							m_pViewContactInfo->SetContactInfo(&pContactInfo);
						}
						//if (bPtInHeadRect && !m_pViewContactInfo->IsWindowVisible())
						//	m_pViewContactInfo->ShowWindow(SW_SHOW);
						//else if (m_pViewContactInfo->IsWindowVisible())
						//	m_pViewContactInfo->ShowWindow(SW_HIDE);
					}else
					{
						EB_MemberInfo pMemberInfo;
						if (theEBAppClient.EB_GetMemberInfoByMemberCode(&pMemberInfo,pCallRecordInfo->m_sMemberCode) ||
							theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,pCallRecordInfo->m_nFromUserId))
						{
							EB_GroupInfo pGroupInfo;
							theEBAppClient.EB_GetGroupInfo(pMemberInfo.m_sGroupCode,&pGroupInfo);
							m_pViewContactInfo->SetEmployeeInfo(&pMemberInfo,&pGroupInfo);
						}else
						{
							bAddMyContact = true;
							EB_ContactInfo pContactInfo;
							pContactInfo.m_nContactUserId = pCallRecordInfo->m_nFromUserId;
							pContactInfo.m_sContact = pCallRecordInfo->m_sFromAccount;
							pContactInfo.m_sName = pCallRecordInfo->m_sFromName;
							pContactInfo.m_sGroupName = pCallRecordInfo->m_sGroupName;
							pContactInfo.m_sCompany = pCallRecordInfo->m_sCompany;
							pContactInfo.m_sJobTitle = pCallRecordInfo->m_sTitle;
							if (pCallRecordInfo->m_nFromPhone!=0)
							{
								char sPhoneString[24];
								sprintf(sPhoneString,"%lld",pCallRecordInfo->m_nFromPhone);
								pContactInfo.m_sPhone = sPhoneString;
							}
							pContactInfo.m_sTel = pCallRecordInfo->m_sTel;
							pContactInfo.m_sEmail = pCallRecordInfo->m_sEmail;
							m_pViewContactInfo->SetContactInfo(&pContactInfo);
						}
						//if (bPtInHeadRect && !m_pViewContactInfo->IsWindowVisible())
						//	m_pViewContactInfo->ShowWindow(SW_SHOW);
						//else if (m_pViewContactInfo->IsWindowVisible())
						//	m_pViewContactInfo->ShowWindow(SW_HIDE);
					}
				}else
				{
					EB_GroupInfo pGroupInfo;
					if (theEBAppClient.EB_GetGroupInfo(pCallRecordInfo->m_sGroupCode,&pGroupInfo))
					{
						EB_EnterpriseInfo pEnterpriseInfo;
						if (pGroupInfo.m_sEnterpriseCode>0)
							theEBAppClient.EB_GetEnterpriseInfo(&pEnterpriseInfo,pGroupInfo.m_sEnterpriseCode);
						m_pViewContactInfo->SetDepartemntInfo(&pGroupInfo,&pEnterpriseInfo);
					}
				}
#endif
			}
		}

		if (bPtInHeadRect)// && !m_pViewContactInfo->IsWindowVisible())
		{
			m_pViewContactInfo->SetMoveEnter();
			//m_pViewContactInfo->ShowWindow(SW_SHOW);
			//m_pViewContactInfo->SetCheckLeave();
		}else if (!bPtInHeadRect)// && m_pViewContactInfo->IsWindowVisible())
		{
			m_pViewContactInfo->SetMoveLeave();
			//m_pViewContactInfo->ShowWindow(SW_HIDE);
		}

		if (!bAddMyContact && pCallRecordInfo.get()!=NULL  && pCallRecordInfo->m_sGroupCode==0)
		{
			// 判断是否我的联系人帐号
#ifdef USES_EBCOM_TEST
			bAddMyContact = theEBClientCore->EB_IsMyContactAccount2(pCallRecordInfo->m_nFromUserId)?false:true;
#else
			bAddMyContact = !theEBAppClient.EB_IsMyContactAccount2(pCallRecordInfo->m_nFromUserId); 
#endif
		}

		const int nTop = rectItem.top;
		m_treeSession.GetClientRect(&rectItem);
		const int nRight = rectItem.right;
		const int const_btn_width = m_btnCallTrack.GetImgWidth();
		m_btnCallTrack.MovePoint(nRight-const_btn_width, nTop);
		m_btnCallTrack.ShowWindow(SW_SHOW);
		m_btnCallTrack.Invalidate();
		if (bAddMyContact)
		{
			if (!is_visitor_uid(pCallRecordInfo->m_nFromUserId) && theApp.GetAuthContact() &&
				(pCallRecordInfo->m_nFromUserId>0 || !pCallRecordInfo->m_sFromAccount.empty()))
			{
				m_btnAddContact.SetToolTipText(_T("点击申请添加好友"));
			}else
			{
				m_btnAddContact.SetToolTipText(_T("添加到我的联系人"));
			}
			m_btnAddContact.MovePoint(nRight-const_btn_width*2, nTop);
			m_btnAddContact.ShowWindow(SW_SHOW);
			m_btnAddContact.Invalidate();
		}else
		{
			m_btnAddContact.ShowWindow(SW_HIDE);
		}
		if (pCallRecordInfo.get()!=NULL && pCallRecordInfo->m_sGroupCode==0 && is_visitor_uid(pCallRecordInfo->m_nFromUserId))
		{
			// ** 游客增加删除按钮，方便操作
			m_btnDeleteTrack.MovePoint(nRight-const_btn_width*(bAddMyContact?3:2), nTop);
			m_btnDeleteTrack.ShowWindow(SW_SHOW);
			m_btnDeleteTrack.Invalidate();
		}else
		{
			m_btnDeleteTrack.ShowWindow(SW_HIDE);
		}
	}
	return 0;
}

BOOL CDlgMySession::PreTranslateMessage(MSG* pMsg)
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
		//DeleteItem(m_treeSession.GetTrackItem(),true);
		this->PostMessage(EB_COMMAND_DELETE_SESSION,(WPARAM)m_treeSession.GetTrackItem(),0);
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnCallTrack.GetSafeHwnd())
	{
		CallItem(m_treeSession.GetTrackItem());
	}else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_treeSession.GetSafeHwnd())
	{
		CallItem(m_treeSession.GetSelectedItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnAddContact.GetSafeHwnd())
	{
		this->PostMessage(EB_COMMAND_NEW_CONTACT,(WPARAM)m_treeSession.GetTrackItem(),0);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgMySession::DeleteItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	CCallRecordInfo::pointer pCallRecordInfo;
	if (m_pCallRecordInfo.find(hItem,pCallRecordInfo,true))
	{
		CString sSql;
		sSql.Format(_T("DELETE FROM call_record_t WHERE call_id=%lld"),pCallRecordInfo->m_sCallId);
		theApp.m_pBoUsers->execute(sSql);

		bool bIsAccount = false;
		eb::bigint sId = 0;
		tstring sName;
		if (pCallRecordInfo->m_sGroupCode>0)
		{
			sId = pCallRecordInfo->m_sGroupCode;
			sName = pCallRecordInfo->m_sGroupName;
			sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=%lld LIMIT 1"),pCallRecordInfo->m_sGroupCode);
		}else
		{
			bIsAccount = true;
			sId = pCallRecordInfo->m_nFromUserId;
			sName = pCallRecordInfo->m_sFromName;
			sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) LIMIT 1"),
				pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_nFromUserId);
		}
		m_treeSession.DeleteItem(hItem);	// *放在前面，用户感觉好一些；
		if (theApp.m_pBoUsers->select(sSql)>0)
		{
			CString sText;
			sText.Format(_T("已经删除会话记录，是否同时删除：\r\n%s(%lld) 本地聊天记录？"), sName.c_str(),sId);
			if ((bIsAccount && pCallRecordInfo->m_nFromType==EB_ACCOUNT_TYPE_VISITOR) || CDlgMessageBox::EbDoModal(this,"删除聊天记录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
			{
				theApp.DeleteDbRecord(sId,bIsAccount);
			}
		}
	}else
	{
		m_treeSession.DeleteItem(hItem);
	}
	if (m_bReloadRecordData && m_treeSession.GetCount()<20)
	{
		KillTimer(TIMERID_LOAD_CALL_RECORD);
		SetTimer(TIMERID_LOAD_CALL_RECORD,100,NULL);
	}
}

void CDlgMySession::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	CCallRecordInfo::pointer pCallRecordInfo;
	if (!m_pCallRecordInfo.find(hItem,pCallRecordInfo))
		return;

	if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
	//if (pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0 &&
	//	(pCallRecordInfo->m_sGroupCode>0 || pCallRecordInfo->m_nFromUserId>0))
	{
		// 验证消息
		const tstring sParameter = pCallRecordInfo->m_nFromType==EBC_MSG_TYPE_BC_MSG?"tab_type=bc_msg":"tab_type=sys_msg";
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo = theEBClientCore->EB_GetSubscribeFuncInfo(theApp.GetGroupMsgSugId());
		if (pSubscribeFuncInfo!=NULL)
		{
			theApp.OpenSubscribeFuncWindow(EB_SubscribeFuncInfo(pSubscribeFuncInfo),"","tab_type=sys_msg");
		}
#else
		EB_SubscribeFuncInfo pSubscribeFuncInfo;
		if (theEBAppClient.EB_GetSubscribeFuncInfo(theApp.GetGroupMsgSugId(),&pSubscribeFuncInfo))
		{
			theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"",sParameter,NULL);
		}
#endif
	}else
	{
#ifdef USES_EBCOM_TEST
		if (pCallRecordInfo->m_sGroupCode==0)
		{
			theEBClientCore->EB_CallUserId(pCallRecordInfo->m_nFromUserId,0);	// ??
		}else
		{
			theEBClientCore->EB_CallGroup(pCallRecordInfo->m_sGroupCode);
		}
#else
		if (pCallRecordInfo->m_sGroupCode==0)
		{
			theApp.m_pAutoCallFromUserIdList.remove(pCallRecordInfo->m_nFromUserId);
			theEBAppClient.EB_CallUserId(pCallRecordInfo->m_nFromUserId,0);	// ??
		}else
		{
			theApp.m_pAutoCallGroupIdList.remove(pCallRecordInfo->m_sGroupCode);
			theEBAppClient.EB_CallGroup(pCallRecordInfo->m_sGroupCode);
		}
#endif
}
}

void CDlgMySession::OnDestroy()
{
	__super::OnDestroy();
	m_btnDeleteTrack.DestroyWindow();
	m_btnCallTrack.DestroyWindow();
	m_btnAddContact.DestroyWindow();
	if (m_pViewContactInfo)
	{
		m_pViewContactInfo->DestroyWindow();
		delete m_pViewContactInfo;
		m_pViewContactInfo = NULL;
	}
}

void CDlgMySession::OnNMRClickTreeSession(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	EB_FUNC_LOCATION nFuncLocation = EB_FUNC_LOCATION_UNKNOWN;
	theApp.ClearSubscribeSelectInfo();
	CNewMenu m_menu2;
	m_menu2.CreatePopupMenu();
	m_menu2.SetBitmapBackground(RGB(192,192,192));
	const HTREEITEM hSelItem = m_treeSession.SelectHitTest();
	//const HTREEITEM hSelItem = m_treeSession.GetSelectedItem();
	const time_t pSessionTime = hSelItem==NULL?0:(time_t)m_treeSession.GetItemData(hSelItem);
	if (pSessionTime!=0)
	{
		CCallRecordInfo::pointer pCallRecordInfo;
		m_pCallRecordInfo.find(hSelItem,pCallRecordInfo);
		bool bNeedSeparator = false;
		if (pCallRecordInfo.get() != NULL && pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
		{
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CALL_USER,_T("打开我的消息"));
			bNeedSeparator = true;
		}
		//else
		//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CALL_USER,_T("打开会话"));
		// 判断聊天记录
		if (pCallRecordInfo.get() != NULL && pCallRecordInfo->m_sMemberCode!=-1)
		{
			theApp.m_nSelectUserId = pCallRecordInfo->m_nFromUserId;
			theApp.m_nSelectGroupId = pCallRecordInfo->m_sGroupCode;
			if (theApp.m_nSelectGroupId>0)
				nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_GROUP;
			else if (theApp.m_nSelectUserId>0)
				nFuncLocation = EB_FUNC_LOCATION_RIGHT_CLICK_MENU_USER;

			CString sSql;
			if (pCallRecordInfo->m_sGroupCode>0)
			{
				if (!theApp.GetDisableGroupSharedCloud())
				{
					bNeedSeparator = true;
					m_menu2.InsertODMenu(-1, _T("群共享"),MF_BYPOSITION,EB_COMMAND_VIEW_GROUP_SHARE,IDB_BITMAP_MENU_SHARE);
					//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_VIEW_GROUP_SHARE,_T("群共享"));
				}
				sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=%lld LIMIT 1"),pCallRecordInfo->m_sGroupCode);
			}else
			{
				sSql.Format(_T("select msg_type FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) LIMIT 1"),
					pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_nFromUserId);
			}
			if (theApp.m_pBoUsers->select(sSql)>0)
			{
				bNeedSeparator = true;
				m_menu2.InsertODMenu(-1,_T("聊天记录"),MF_BYPOSITION,EB_MSG_VIEW_MSG_RECORD,IDB_BITMAP_MENU_MSG);
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_MSG_VIEW_MSG_RECORD,_T("聊天记录"));
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_MSG_RECORD,_T("清空聊天记录"));
			}
		}
		if (bNeedSeparator)
			m_menu2.AppendMenu(MF_SEPARATOR);
		if (pCallRecordInfo.get() != NULL && pCallRecordInfo->m_sMemberCode==-1 && pCallRecordInfo->m_nFromType>0)
			m_menu2.AppendODMenu(_T("删除消息通知(&D)"),MF_BYPOSITION,EB_COMMAND_DELETE_SESSION,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_SESSION,_T("删除消息通知"));
		else
			m_menu2.AppendODMenu(_T("删除会话记录(&D)"),MF_BYPOSITION,EB_COMMAND_DELETE_SESSION,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_SESSION,_T("删除会话记录"));
	}
	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CLEAR_SESSION,_T("清空会话记录"));

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
void CDlgMySession::OnSubscribeFunc(UINT nID)
{
	size_t nIndex = nID-EB_COMMAND_SUBSCRIBE_FUNC;
	if (nIndex>=0 && nIndex<theApp.m_pSubscribeFuncList.size())
	{
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pSubscribeFuncList[nIndex];
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
	}
}

void CDlgMySession::OnSessionDeleteSes()
{
	DeleteItem(m_treeSession.GetSelectedItem());
}
LRESULT CDlgMySession::OnMsgDeleteSes(WPARAM wParam, LPARAM lParam)
{
	DeleteItem((HTREEITEM)wParam);
	return 0;
}
LRESULT CDlgMySession::OnMsgNewContact(WPARAM wp, LPARAM lp)
{
	HTREEITEM hItem = (HTREEITEM)wp;
	CCallRecordInfo::pointer pCallRecordInfo;
	if (m_pCallRecordInfo.find(hItem,pCallRecordInfo))
	{
		tstring sAddress;
		//tstring sContact(pCallRecordInfo->m_sFromAccount);
		if (pCallRecordInfo->m_nFromType==EB_ACCOUNT_TYPE_VISITOR || is_visitor_uid(pCallRecordInfo->m_nFromUserId))
		{
			sAddress = pCallRecordInfo->m_sFromName;
		}else if (theApp.GetAuthContact() && (pCallRecordInfo->m_nFromUserId>0 || !pCallRecordInfo->m_sFromAccount.empty()))
		{
			// 验证好友方式
			CDlgRequestAddContact pDlg;
			pDlg.m_sHeadFilePath = theApp.GetUserHeadFilePath(pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_sFromAccount);
			CString sText;
			sText.Format(_T("%s\n%lld\n%s"),pCallRecordInfo->m_sFromName.c_str(),pCallRecordInfo->m_nFromUserId,pCallRecordInfo->m_sFromAccount.c_str());
			pDlg.m_sHeadName = (LPCTSTR)sText;
			if (pDlg.DoModal()==IDOK)
			{
				EB_ContactInfo pContactInfo;
				pContactInfo.m_nContactUserId = pCallRecordInfo->m_nFromUserId;
				pContactInfo.m_sContact = pCallRecordInfo->m_sFromAccount;
				pContactInfo.m_sDescription = (LPCTSTR)pDlg.m_sDescription;
				theEBAppClient.EB_EditContact(&pContactInfo);
			}
			return 0;
		}
		//if (sContact.empty())
		//{
		//	if ( !pCallRecordInfo->m_sEmail.empty())
		//		sContact = pCallRecordInfo->m_sEmail;
		//	else
		//	{
		//		char lpszUid[24];
		//		sprintf(lpszUid,"%lld",pCallRecordInfo->m_nFromUserId);
		//		sContact = lpszUid;
		//	}
		//}
#ifdef USES_EBCOM_TEST
		CComPtr<IEB_ContactInfo> pContactInfo = theEBClientCore->EB_GetContactInfo2(pCallRecordInfo->m_nFromUserId);
		if (pContactInfo==NULL)
		{
			CIEB_ContactInfo * pIEBContactInfo = new CIEB_ContactInfo();
			pIEBContactInfo->QueryInterface(__uuidof(IEB_ContactInfo),(void**)&pContactInfo);
		}
		pContactInfo->ContactUserId = pCallRecordInfo->m_nFromUserId;
		pContactInfo->Contact = pCallRecordInfo->m_sFromAccount.c_str();
		pContactInfo->Address = sAddress.c_str();

		CDlgContactInfo pDlgContactInfo;
		pDlgContactInfo.m_sContact = pContactInfo->Contact.GetBSTR();
		pDlgContactInfo.m_sName = pCallRecordInfo->m_sFromName.c_str();
		pDlgContactInfo.m_nUGId = pContactInfo->UGId;
		pDlgContactInfo.m_sPhone.Format(_T("%lld"),pCallRecordInfo->m_nFromPhone);
		pDlgContactInfo.m_sCompany = pCallRecordInfo->m_sCompany.c_str();
		pDlgContactInfo.m_sJobTitle = pCallRecordInfo->m_sTitle.c_str();
		pDlgContactInfo.m_sUrl = pContactInfo->Url.GetBSTR();
		pDlgContactInfo.m_sTel = pCallRecordInfo->m_sTel.c_str();
		pDlgContactInfo.m_sFax = pContactInfo->Fax.GetBSTR();
		pDlgContactInfo.m_sEmail = pCallRecordInfo->m_sEmail.c_str();
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
		theEBAppClient.EB_GetContactInfo2(pCallRecordInfo->m_nFromUserId,&pContactInfo);
		pContactInfo.m_nContactUserId = pCallRecordInfo->m_nFromUserId;
		pContactInfo.m_sContact = pCallRecordInfo->m_sFromAccount;
		pContactInfo.m_sAddress = sAddress;

		CDlgContactInfo pDlgContactInfo(this);
		pDlgContactInfo.m_sContact = pContactInfo.m_sContact.c_str();
		pDlgContactInfo.m_sName = pCallRecordInfo->m_sFromName.c_str();
		pDlgContactInfo.m_nUGId = pContactInfo.m_nUGId;
		if (pCallRecordInfo->m_nFromPhone>0)
		{
			char sPhoneString[24];
			sprintf(sPhoneString,"%lld",pCallRecordInfo->m_nFromPhone);
			pDlgContactInfo.m_sPhone = sPhoneString;
		}
		pDlgContactInfo.m_sCompany = pCallRecordInfo->m_sCompany.c_str();
		pDlgContactInfo.m_sJobTitle = pCallRecordInfo->m_sTitle.c_str();
		pDlgContactInfo.m_sUrl = pContactInfo.m_sUrl.c_str();
		pDlgContactInfo.m_sTel = pCallRecordInfo->m_sTel.c_str();
		pDlgContactInfo.m_sFax = pContactInfo.m_sFax.c_str();
		pDlgContactInfo.m_sEmail = pCallRecordInfo->m_sEmail.c_str();
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
	return 0;
}

void CDlgMySession::OnSessionClearSes()
{
	if (CDlgMessageBox::EbDoModal(this,"清空会话","是否确定清空：\r\n所有会话记录和聊天记录吗？",CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
	{
		return;
	}
	const bool bTemp = m_bReloadRecordData;
	m_bReloadRecordData = false;
	HTREEITEM hChildItem = m_treeSession.GetChildItem(TVI_ROOT);
	while (hChildItem!=NULL)
	{
		HTREEITEM hNextItem = m_treeSession.GetNextItem(hChildItem, TVGN_NEXT);
		DeleteItem(hChildItem);
		hChildItem = hNextItem;
	}
	m_bReloadRecordData = bTemp;
	if (m_bReloadRecordData && m_treeSession.GetCount()<20)
	{
		KillTimer(TIMERID_LOAD_CALL_RECORD);
		SetTimer(TIMERID_LOAD_CALL_RECORD,100,NULL);
	}
}

void CDlgMySession::OnCallUser()
{
	CallItem(m_treeSession.GetSelectedItem());
}
void CDlgMySession::OnViewMsgRecord()
{
	const HTREEITEM hSelItem = m_treeSession.GetSelectedItem();
	CCallRecordInfo::pointer pCallRecordInfo;
	if (!m_pCallRecordInfo.find(hSelItem,pCallRecordInfo))
	{
		return;
	}

	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	if (pCallRecordInfo->m_sGroupCode==0)
	{
		bIsAccount = true;
		sId = pCallRecordInfo->m_nFromUserId;
		sName = pCallRecordInfo->m_sFromName;
	}else
	{
		sId = pCallRecordInfo->m_sGroupCode;
		sName = pCallRecordInfo->m_sGroupName;
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
void CDlgMySession::DeleteMsgRecord(const CCallRecordInfo::pointer& pCallRecordInfo,bool bQuesiton)
{
	bool bIsAccount = false;
	eb::bigint sId = 0;
	tstring sName;
	if (pCallRecordInfo->m_sGroupCode==0)
	{
		bIsAccount = true;
		sId = pCallRecordInfo->m_nFromUserId;
		sName = pCallRecordInfo->m_sFromName;
	}else
	{
		sId = pCallRecordInfo->m_sGroupCode;
		sName = pCallRecordInfo->m_sGroupName;
	}

	CString sText;
	sText.Format(_T("确定清空：\r\n%s(%lld) 本地聊天记录吗？"), sName.c_str(),sId);
	if (!bQuesiton || CDlgMessageBox::EbDoModal(this,"清空聊天记录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("清空聊天记录"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
		theApp.DeleteDbRecord(sId,bIsAccount);
	}
}

void CDlgMySession::OnDeleteMsgRecord()
{
	const HTREEITEM hSelItem = m_treeSession.GetSelectedItem();
	CCallRecordInfo::pointer pCallRecordInfo;
	if (!m_pCallRecordInfo.find(hSelItem,pCallRecordInfo))
	{
		return;
	}
	DeleteMsgRecord(pCallRecordInfo,true);
}

void CDlgMySession::OnGroupShare()
{
	const HTREEITEM hSelItem = m_treeSession.GetSelectedItem();
	CCallRecordInfo::pointer pCallRecordInfo;
	if (!m_pCallRecordInfo.find(hSelItem,pCallRecordInfo))
	{
		return;
	}else if (pCallRecordInfo->m_sGroupCode==0)
	{
		return;
	}
	theApp.OpenGroupShareWindow(pCallRecordInfo->m_sGroupCode,this->GetParent());
}
