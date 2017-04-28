// DlgMsgTip.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgMsgTip.h"

#define TIMERID_DELETE_ITEM 100

// CDlgMsgTip dialog

IMPLEMENT_DYNAMIC(CDlgMsgTip, CEbDialogBase)

CDlgMsgTip::CDlgMsgTip(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMsgTip::IDD, pParent)
{

}

CDlgMsgTip::~CDlgMsgTip()
{
}

void CDlgMsgTip::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MESSAGE, m_pTreeMessage);
	DDX_Control(pDX, IDC_BUTTON_SETTING, m_btnSetting);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_DELALL, m_btnDeleteAll);
	DDX_Control(pDX, IDC_BUTTON_VIEWALL, m_btnViewAll);
}


BEGIN_MESSAGE_MAP(CDlgMsgTip, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgMsgTip::OnBnClickedButtonClose)
	ON_NOTIFY(NM_CLICK, IDC_TREE_MESSAGE, &CDlgMsgTip::OnNMClickTreeMessage)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, &CDlgMsgTip::OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_VIEWALL, &CDlgMsgTip::OnBnClickedButtonViewall)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CDlgMsgTip message handlers
void CDlgMsgTip::SetCtrlColor(void)
{
	m_btnDeleteAll.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());
	m_btnViewAll.SetDrawPanel(true,-1,theApp.GetHotColor(),theApp.GetPreColor());

	m_btnSetting.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	//m_btnDeleteAll.SetNorTextColor(theApp.GetMainColor());
	//m_btnDeleteAll.SetHotTextColor(theApp.GetHotColor2());
	//m_btnViewAll.SetNorTextColor(theApp.GetMainColor());
	//m_btnViewAll.SetHotTextColor(theApp.GetHotColor2());
}

BOOL CDlgMsgTip::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	this->SetMouseMove(FALSE);

	// TODO:  Add extra initialization here
	m_btnSetting.SetDrawTrianglePic(2,theDefaultBtnWhiteColor,-1,-1,-1,10,5);
	m_btnSetting.SetDrawPanelRgn(false);
	m_btnSetting.ShowWindow(SW_HIDE);
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.ShowWindow(SW_HIDE);
	m_btnDeleteAll.SetTextHotMove(false);
	m_btnDeleteAll.SetAutoSize(false);
	m_btnDeleteAll.SetWindowText(_T("忽略全部"));
	m_btnDeleteAll.SetNorTextColor(RGB(0,128,255));
	m_btnDeleteAll.SetHotTextColor(RGB(255,255,255));
	m_btnDeleteAll.SetPreTextColor(RGB(255,255,255));
	m_btnViewAll.SetTextHotMove(false);
	m_btnViewAll.SetAutoSize(false);
	m_btnViewAll.SetWindowText(_T("查看全部"));
	m_btnViewAll.SetNorTextColor(RGB(0,128,255));
	m_btnViewAll.SetHotTextColor(RGB(255,255,255));
	m_btnViewAll.SetPreTextColor(RGB(255,255,255));

	m_btnDeleteTrack.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_pTreeMessage, 0xffff);
	m_btnDeleteTrack.SetAutoSize(false);
	m_btnDeleteTrack.SetAutoFocus(true);
	m_btnDeleteTrack.Load(IDB_PNG_HOT_DELETE);
	m_btnDeleteTrack.SetToolTipText(_T("忽略"));

	//m_pTreeMessage.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_pTreeMessage.SetCallback(&theApp);
	//m_treeDepartment.ModifyStyle(TVS_SINGLEEXPAND,0);
	m_pTreeMessage.ModifyStyle(0, TVS_SINGLEEXPAND|TVS_TRACKSELECT);
	m_pTreeMessage.SetItemHeight(40);	// 24
	m_pTreeMessage.SetIconSize(32,32);
	m_pTreeMessage.SetIntervalLine(true,theDefaultBtnWhiteColor,theDefaultFlatBgColor);

	this->SetCtrlColor();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMsgTip::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgMsgTip::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgMsgTip::OnDestroy()
{
	m_btnDeleteTrack.DestroyWindow();
	CEbDialogBase::OnDestroy();

	// TODO: Add your message handler code here
}

#define WINDOW_TITLE_HEIGHT			36
void CDlgMsgTip::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作区矩形中居中
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// 绘制图标
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		//if (theApp.GetColorGradient())
		{
			this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT-1,1.0,false,false,0,theDefaultFlatBgColor));
			this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
			this->AddBgDrawInfo(CEbBackDrawInfo(-30,1.0,false,false,0,theDefaultFlatLineColor));
			this->AddBgDrawInfo(CEbBackDrawInfo(-29,1.0,false,false,0,theDefaultFlatBgColor));
			this->DrawPopBg(&memDC, theApp.GetFlatBgColor(), 1);
		//}else
		//{
		//	if (theApp.GetColorGradient())
		//		this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,theDefaultBgColorTitleLight1,false));
		//	else
		//		this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,1.0,false));
		//	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		//	this->DrawPopBg(&memDC, theApp.GetMainColor());
		}
		Gdiplus::Graphics graphics(memDC.m_hDC);

		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleRegular, UnitPixel);
		//const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(32,32,32));
		Gdiplus::PointF pointTitle(7,7);
		USES_CONVERSION;
		graphics.DrawString(L"消息提醒",-1,&fontEbTitle,pointTitle,&brushEbTitle);
	}
}

void CDlgMsgTip::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TIMERID_DELETE_ITEM == nIDEvent)
	{
		KillTimer(nIDEvent);
		DeleteItem(m_pTreeMessage.GetTrackItem());
	}
	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgMsgTip::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnSetting.GetSafeHwnd())
		m_btnSetting.MoveWindow(btnx,1,const_minbtn_width,const_minbtn_height);

	if (m_pTreeMessage.GetSafeHwnd()!=NULL)
	{
		CRect rect;
		rect.left = 1;
		rect.right = cx-1;
		rect.top = WINDOW_TITLE_HEIGHT;
		rect.bottom = cy-30;	//24+6;
		m_pTreeMessage.MoveWindow(&rect);
	}

	const int const_btn_width = 55;
	const int const_btn_height = 19;
	m_rectDeleteAll.left = 10;
	m_rectDeleteAll.right = m_rectDeleteAll.left + const_btn_width;
	m_rectDeleteAll.top = cy-24;
	m_rectDeleteAll.bottom = m_rectDeleteAll.top + const_btn_height;
	if (m_btnDeleteAll.GetSafeHwnd()!=NULL)
		m_btnDeleteAll.MoveWindow(&m_rectDeleteAll);
	m_rectViewAll.left = cx-const_btn_width-10;
	m_rectViewAll.right = m_rectViewAll.left + const_btn_width;
	m_rectViewAll.top = m_rectDeleteAll.top;
	m_rectViewAll.bottom = m_rectDeleteAll.bottom;
	if (m_btnViewAll.GetSafeHwnd()!=NULL)
		m_btnViewAll.MoveWindow(&m_rectViewAll);

	//m_btnDeleteAll.MovePoint(10,cy-24,const_btn_width,const_btn_height);
	//m_btnViewAll.MovePoint(cx-const_btn_width-10,cy-24,const_btn_width,const_btn_height);
}

void CDlgMsgTip::OnBnClickedButtonClose()
{
	this->ShowWindow(SW_HIDE);
}

void CDlgMsgTip::AddSubMsgTip(eb::bigint nId, eb::bigint nSubId, const CString& sMsgTip, const CString& sParam, eb::bigint nMsgId)
{
	CString sMsgTipTemp(sMsgTip);
	sMsgTipTemp.Replace(_T("&"),_T("&&"));
	CTreeItemInfo::pointer pItemInfo;
	if (m_pIdItemInfo.find(nId,pItemInfo))
	{
		if (pItemInfo->m_sParentId>0 && pItemInfo->m_sParentId!=nMsgId)
		{
			const eb::bigint nMsgId = pItemInfo->m_sParentId;
			theEBAppClient.EB_AckMsg(nMsgId,1);
		}
		m_pTreeMessage.SetItemText(pItemInfo->m_hItem,sMsgTipTemp);
		pItemInfo->m_nBigId = nSubId;
		pItemInfo->m_sName = (LPCTSTR)sParam;
		pItemInfo->m_sParentId = nMsgId;
	}else
	{
		HTREEITEM hEmpItem = m_pTreeMessage.InsertItem(sMsgTipTemp);
		pItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_SUBMSG,hEmpItem);
		pItemInfo->m_sId = nId;
		pItemInfo->m_nBigId = nSubId;
		pItemInfo->m_sName = (LPCTSTR)sParam;
		pItemInfo->m_sParentId = nMsgId;
		m_pIdItemInfo.insert(nId, pItemInfo);
		m_pTreeMessage.SetItemData(hEmpItem, (DWORD)pItemInfo.get());
	}
}
void CDlgMsgTip::AddEmailMsgTip(eb::bigint nId, eb::bigint nEmailSubId, const CString& sMsgTip, const CString& sParam)
{
	CString sMsgTipTemp(sMsgTip);
	sMsgTipTemp.Replace(_T("&"),_T("&&"));
	CTreeItemInfo::pointer pItemInfo;
	if (m_pIdItemInfo.find(nId,pItemInfo))
	{
		m_pTreeMessage.SetItemText(pItemInfo->m_hItem,sMsgTipTemp);
	}else
	{
		HTREEITEM hEmpItem = m_pTreeMessage.InsertItem(sMsgTipTemp);
		pItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_EMAIL,hEmpItem);
		pItemInfo->m_sId = nId;
		pItemInfo->m_nBigId = nEmailSubId;
		pItemInfo->m_sName = (LPCTSTR)sParam;
		m_pIdItemInfo.insert(nId, pItemInfo);
		m_pTreeMessage.SetItemData(hEmpItem, (DWORD)pItemInfo.get());
	}
}
void CDlgMsgTip::DelEmailMsgTip(eb::bigint nId)
{
	CTreeItemInfo::pointer pItemInfo;
	if (m_pIdItemInfo.find(nId,pItemInfo,true))
	{
		m_pTreeMessage.DeleteItem(pItemInfo->m_hItem);
	}

	if (IsMsgEmpty())
		this->ShowWindow(SW_HIDE);
	else
		this->ShowMsgTip();
}

void CDlgMsgTip::AddMsgTip(eb::bigint nGroupId, eb::bigint nFromUserId, const CString& sMsgTip)
{
	CString sMsgTipTemp(sMsgTip);
	sMsgTipTemp.Replace(_T("&"),_T("&&"));
	bool bNewMsg = false;
	if (nGroupId>0)
	{
		CTreeItemInfo::pointer pItemInfo;
		if (m_pGroupItemInfo.find(nGroupId,pItemInfo))
		{
			m_pTreeMessage.SetItemText(pItemInfo->m_hItem,sMsgTipTemp);
		}else
		{
			HTREEITEM hEmpItem = m_pTreeMessage.InsertItem(sMsgTipTemp);
			pItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_GROUP,hEmpItem);
			pItemInfo->m_sId = 1;
			pItemInfo->m_nSubType = -2;
			pItemInfo->m_sGroupCode = nGroupId;
			pItemInfo->m_nUserId = nFromUserId;
			m_pGroupItemInfo.insert(nGroupId, pItemInfo);
			m_pTreeMessage.SetItemData(hEmpItem, (DWORD)pItemInfo.get());
			bNewMsg = true;
		}
	}else
	{
		CTreeItemInfo::pointer pItemInfo;
		if (m_pUserItemInfo.find(nFromUserId,pItemInfo))
		{
			m_pTreeMessage.SetItemText(pItemInfo->m_hItem,sMsgTipTemp);
		}else
		{
			HTREEITEM hEmpItem = m_pTreeMessage.InsertItem(sMsgTipTemp);
			pItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_CONTACT,hEmpItem);
			pItemInfo->m_nUserId = nFromUserId;
			pItemInfo->m_dwItemData = EB_LINE_STATE_ONLINE_NEW;
			m_pUserItemInfo.insert(nFromUserId, pItemInfo);
			m_pTreeMessage.SetItemData(hEmpItem, (DWORD)pItemInfo.get());
			bNewMsg = true;
		}
	}
	if (bNewMsg && this->IsWindowVisible())
	{
		ShowMsgTip();
	}
}
void CDlgMsgTip::DelMsgTip(eb::bigint nGroupId, eb::bigint nFromUserId)
{
	try
	{
		if (nGroupId>0)
		{
			CTreeItemInfo::pointer pItemInfo;
			if (!m_pGroupItemInfo.find(nGroupId,pItemInfo,true))
				return;
			m_pTreeMessage.DeleteItem(pItemInfo->m_hItem);
		}else
		{
			CTreeItemInfo::pointer pItemInfo;
			if (!m_pUserItemInfo.find(nFromUserId,pItemInfo,true))
				return;
			m_pTreeMessage.DeleteItem(pItemInfo->m_hItem);
		}
		if (IsMsgEmpty())
			this->ShowWindow(SW_HIDE);
		else
			this->ShowMsgTip();
		return;
	}catch(const std::exception&)
	{
	}catch(...)
	{
	}
	//if (theApp.GetLogonUserId()==74)
	//	theApp.LogMessage("CDlgMsgTip::DelMsgTip exception\r\n");
}

bool CDlgMsgTip::IsMsgEmpty(void) const
{
	return m_pTreeMessage.GetCount()==0?true:false;
}
void CDlgMsgTip::SetCurrentTrayIconPos(void)
{
	GetCursorPos(&m_posTrayIcon);
}
void CDlgMsgTip::ShowMsgTip(void)
{
	const int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN)+GetSystemMetrics(SM_CYCAPTION);
	const int const_width = 280;
	CRect rect;
	rect.left = m_posTrayIcon.x - const_width/2;
	rect.right = rect.left + const_width;
	rect.bottom = min(m_posTrayIcon.y,m_nScreenHeight);
	if (rect.bottom<m_nScreenHeight)
		rect.bottom -= 38;
	const int nHeight = WINDOW_TITLE_HEIGHT+m_pTreeMessage.GetCount()*40+24+6;
	rect.top = rect.bottom - nHeight;
	this->MoveWindow(&rect);
	if (this->IsIconic())
		this->ShowWindow(SW_RESTORE);
	else if (!this->IsWindowVisible())
		this->ShowWindow(SW_SHOW);
	else
		return;
	this->SetWindowPos(&wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	//this->SetForegroundWindow();
}
void CDlgMsgTip::ViewAllMsg(void)
{
	OnBnClickedButtonViewall();
}
void CDlgMsgTip::CheckPosInMsgArea(void)
{
	CPoint pos;
	GetCursorPos(&pos);
	CRect rect;
	this->GetWindowRect(&rect);
	if (rect.PtInRect(pos))
		return;
	rect.left = m_posTrayIcon.x-20;
	rect.right = m_posTrayIcon.x+20;
	rect.top = m_posTrayIcon.y-20;
	rect.bottom = m_posTrayIcon.y+20;
	if (rect.PtInRect(pos))
		return;
	this->ShowWindow(SW_HIDE);
}

void CDlgMsgTip::OnNMClickTreeMessage(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CallItem(m_pTreeMessage.GetTrackItem());
}

void CDlgMsgTip::CallItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_pTreeMessage.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return;
	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_EMAIL && pTreeItemInfo->m_nBigId>0)
	{
		const eb::bigint nEmailSubId = pTreeItemInfo->m_nBigId;
		if (m_pEmailSubscribeFuncInfo.m_nSubscribeId!=nEmailSubId)
		{
			if (!theEBAppClient.EB_GetSubscribeFuncInfo(nEmailSubId,&m_pEmailSubscribeFuncInfo))
				return;
		}
		if (theApp.OpenSubscribeFuncWindow(m_pEmailSubscribeFuncInfo,"", pTreeItemInfo->m_sName, NULL))
		{
			DelEmailMsgTip(pTreeItemInfo->m_sId);
			char* lpszSubId = new char[24];
			sprintf(lpszSubId,"%lld",m_pEmailSubscribeFuncInfo.m_nSubscribeId);
			theApp.GetMainWnd()->PostMessage(EB_COMMAND_CLEAR_SUBID_UNREAD_MSG, (WPARAM)lpszSubId,1);
		}
	}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP || pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_CONTACT)
	{
		if (pTreeItemInfo->m_sGroupCode>0)
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_CallGroup(pTreeItemInfo->m_sGroupCode);
#else
			theEBAppClient.EB_CallGroup(pTreeItemInfo->m_sGroupCode);
#endif
		}else
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_CallUserId(pTreeItemInfo->m_nUserId);
#else
			theEBAppClient.EB_CallUserId(pTreeItemInfo->m_nUserId);
#endif
		}
		DelMsgTip(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
	}else if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_SUBMSG)
	{
		CTreeItemInfo::pointer pItemInfo;
		if (m_pIdItemInfo.find(pTreeItemInfo->m_sId,pItemInfo))
		{
			if (pItemInfo->m_sParentId>0)
			{
				const eb::bigint nMsgId = pItemInfo->m_sParentId;
				//theEBAppClient.EB_AckMsg(nMsgId,5);
				theEBAppClient.EB_AckMsg(nMsgId,1);
				pItemInfo->m_sParentId = 0;
			}
		}

		const eb::bigint nSubId = pTreeItemInfo->m_nBigId;
		if (nSubId==0)
		{
			DelEmailMsgTip(pTreeItemInfo->m_sId);
			return;
		}
		EB_SubscribeFuncInfo pSubscribeFuncInfo;
		if (!theEBAppClient.EB_GetSubscribeFuncInfo(nSubId,&pSubscribeFuncInfo))
		{
			return;
		}
		if (theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"", pTreeItemInfo->m_sName, NULL))
		{
			DelEmailMsgTip(pTreeItemInfo->m_sId);
			char* lpszSubId = new char[24];
			sprintf(lpszSubId,"%lld",pSubscribeFuncInfo.m_nSubscribeId);
			theApp.GetMainWnd()->PostMessage(EB_COMMAND_CLEAR_SUBID_UNREAD_MSG, (WPARAM)lpszSubId,1);
		}
	}
}
void CDlgMsgTip::DeleteItem(HTREEITEM hItem)
{
	if (hItem==NULL) return;
	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)m_pTreeMessage.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return;
	if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_GROUP || pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_CONTACT)
	{
		DelMsgTip(pTreeItemInfo->m_sGroupCode,pTreeItemInfo->m_nUserId);
	}else if (pTreeItemInfo->m_nItemType >= CTreeItemInfo::ITEM_TYPE_EMAIL)
	{
		DelEmailMsgTip(pTreeItemInfo->m_sId);
	}
}

LRESULT CDlgMsgTip::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	try
	{
		HTREEITEM item = (HTREEITEM)wp;
		if (item == NULL)
		{
			m_btnDeleteTrack.ShowWindow(SW_HIDE);
		}else if (m_btnDeleteTrack.GetSafeHwnd() != NULL)
		{
			CRect rectItem;
			m_pTreeMessage.GetItemRect(item, &rectItem, TRUE);
			const int nTop = rectItem.top;
			m_pTreeMessage.GetClientRect(&rectItem);
			const int nRight = rectItem.right;
			const int const_btn_width = m_btnDeleteTrack.GetImgWidth();
			m_btnDeleteTrack.MovePoint(nRight-const_btn_width, nTop);
			m_btnDeleteTrack.ShowWindow(SW_SHOW);
			m_btnDeleteTrack.Invalidate();
		}
	}catch(const std::exception&)
	{
	}catch(...)
	{}
	return 0;
}

BOOL CDlgMsgTip::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnDeleteTrack.GetSafeHwnd())
	{
		SetTimer(TIMERID_DELETE_ITEM,1,NULL);
	}

	return CEbDialogBase::PreTranslateMessage(pMsg);
}

void CDlgMsgTip::OnBnClickedButtonDelall()
{
	{
		BoostReadLock rdlock(m_pIdItemInfo.mutex());
		CLockMap<eb::bigint,CTreeItemInfo::pointer>::iterator pIter = m_pIdItemInfo.begin();
		for (; pIter!=m_pIdItemInfo.end(); pIter++)
		{
			const CTreeItemInfo::pointer& pItemInfo = pIter->second;
			if (pItemInfo->m_sParentId>0)
			{
				const eb::bigint nMsgId = pItemInfo->m_sParentId;
				theEBAppClient.EB_AckMsg(nMsgId,1);
				pItemInfo->m_sParentId = 0;
			}
		}
	}

	m_pTreeMessage.DeleteAllItems();
	m_pIdItemInfo.clear();
	m_pGroupItemInfo.clear();
	m_pUserItemInfo.clear();
	this->ShowWindow(SW_HIDE);
}

void CDlgMsgTip::OnBnClickedButtonViewall()
{
	while (true)
	{
		HTREEITEM hChildItem = m_pTreeMessage.GetChildItem(TVI_ROOT);
		if (hChildItem==NULL)
			break;
		CallItem(hChildItem);
	}
	OnBnClickedButtonDelall();
}

void CDlgMsgTip::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	if (m_rectDeleteAll.PtInRect(pos) || m_rectViewAll.PtInRect(pos))
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		return;
	}

	CEbDialogBase::OnMouseMove(nFlags, point);
}
