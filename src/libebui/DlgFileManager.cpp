// DlgFileManager.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgFileManager.h"
#include "PanelFiles.h"
#include "Core/SkinMemDC.h"

#define TIMERID_LOAD_FILE_TRANED	100
#define TIMERID_DELETE_TRACK_FILE	101

// CDlgFileManager dialog

IMPLEMENT_DYNAMIC(CDlgFileManager, CEbDialogBase)

CDlgFileManager::CDlgFileManager(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgFileManager::IDD, pParent)
	, m_pPanFiles(NULL)
{

}

CDlgFileManager::~CDlgFileManager()
{
}

void CDlgFileManager::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_TRANING, m_btnTraning);
	DDX_Control(pDX, IDC_BUTTON_TRANED, m_btnTraned);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_TREE_TRANED, m_treeTraned);
}


BEGIN_MESSAGE_MAP(CDlgFileManager, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgFileManager::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgFileManager::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgFileManager::OnBnClickedButtonClose)
	ON_WM_PAINT()
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_BUTTON_TRANING, &CDlgFileManager::OnBnClickedButtonTraning)
	ON_BN_CLICKED(IDC_BUTTON_TRANED, &CDlgFileManager::OnBnClickedButtonTraned)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDlgFileManager::OnBnClickedButtonRefresh)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDblclk)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)

END_MESSAGE_MAP()

void CDlgFileManager::SetCtrlColor(void)
{
	m_btnTraning.SetDrawPanel(true,theApp.GetBgTitle0(),theApp.GetBgColor2(),theApp.GetBgColor2());
	m_btnTraned.SetDrawPanel(true,theApp.GetBgTitle0(),theApp.GetBgColor2(),theApp.GetBgColor2());
	m_treeTraned.SetBkGradients(theApp.GetBgColor2(),theApp.GetBgColor2());
	m_btnRefresh.SetNorTextColor(RGB(0,128,255));
	m_btnRefresh.SetHotTextColor(RGB(0,128,255));
	//m_btnRefresh.SetNorTextColor(theApp.GetHotColor2());
	//m_btnRefresh.SetHotTextColor(theApp.GetHotColor2());
	////m_btnRefresh.SetTextHotMove(false);
	////m_btnRefresh.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
}

// CDlgFileManager message handlers

BOOL CDlgFileManager::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	//SetTransparentType(CEbDialogBase::TT_DLG);
	this->SetSplitterBorder();

	m_btnMin.SetAutoSize(false);
	m_btnMin.Load(IDB_PNG_MIN);
	m_btnMax.SetAutoSize(false);
	m_btnMax.Load(IDB_PNG_MAX);
	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);

	m_btnTraning.SetAutoSize(false);
	//m_btnTraning.Load(IDB_PNG_TAB_75X30);
	m_btnTraning.SetDrawPanelRgn(false);
	m_btnTraned.SetAutoSize(false);
	m_btnTraned.SetDrawPanelRgn(false);
	//m_btnTraned.Load(IDB_PNG_TAB_75X30);

	// IDB_PNG_HOT_DELETE
	// IDB_PNG_HOT_OPENFILE
	// IDB_PNG_HOT_OPENDIR
	m_btnHotDeleteFile.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeTraned, 0xffff);
	m_btnHotDeleteFile.Load(IDB_PNG_HOT_DELETE);
	m_btnHotDeleteFile.SetToolTipText(_T("删除文件"));
	m_btnHotDeleteFile.SetAutoFocus(true);
	m_btnHotOpenFile.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeTraned, 0xffff);
	m_btnHotOpenFile.Load(IDB_PNG_HOT_OPENFILE);
	m_btnHotOpenFile.SetToolTipText(_T("打开文件"));
	m_btnHotOpenFile.SetAutoFocus(true);
	m_btnHotOpenDir.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeTraned, 0xffff);
	m_btnHotOpenDir.Load(IDB_PNG_HOT_OPENDIR);
	m_btnHotOpenDir.SetToolTipText(_T("打开目录"));
	m_btnHotOpenDir.SetAutoFocus(true);
	//m_btnHotDelete.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeTraned, 0xffff);
	//m_btnHotDelete.Load(IDB_PNG_HOT_OPENDIR);
	//m_btnHotDelete.SetToolTipText(_T("删除记录和文件"));
	//m_btnHotDelete.SetAutoFocus(true);

	m_treeTraned.SetCallback((CTreeCallback*)this);
	m_treeTraned.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeTraned.SetItemHeight(40);
	m_treeTraned.SetIconSize(32,32);
	//m_treeTraned.SetRootIcon( theApp.GetIconCon() );
	//m_treeTraned.SetItemIcon( theApp.GetIconCon() );
	m_treeTraned.ShowWindow(SW_HIDE);

	m_pPanFiles = new CPanelFiles(this);
	m_pPanFiles->SetCircle(false);
	//m_pPanFiles->m_pChatRoom = m_pChatRoom;
	m_pPanFiles->Create(CPanelFiles::IDD, this);
	m_pPanFiles->ShowWindow(SW_SHOW);

	SetCtrlColor();

	OnBnClickedButtonTraned();
	OnBnClickedButtonRefresh();

	this->SetWindowText(_T("文件传输"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFileManager::OnDestroy()
{
	m_pItemItemInfo.clear();
	CEbDialogBase::OnDestroy();

	if (m_pPanFiles)
	{
		m_pPanFiles->DestroyWindow();
		delete m_pPanFiles;
		m_pPanFiles = NULL;
	}
}

void CDlgFileManager::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	const int const_minbtn_width = m_btnMin.GetImgWidth();
	const int const_minbtn_height = m_btnMin.GetImgHeight();
	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	m_btnMax.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;
	m_btnMin.MovePoint(btnx, 2);
	btnx -= const_minbtn_width;

	const int const_left_intever	= 2;
	const int const_tab_width = 75;//m_btnTraning.GetImgWidth();
	const int const_tab_height = 30;//m_btnTraning.GetImgHeight();

	int x = const_left_intever;
	int y = 42;
	m_btnTraning.MovePoint(x, y,const_tab_width,const_tab_height);
	x += const_tab_width;
	m_btnTraned.MovePoint(x, y,const_tab_width,const_tab_height);
	x = cx-const_tab_width-const_left_intever;
	m_btnRefresh.MovePoint(x,y,const_tab_width,const_tab_height);
	y += const_tab_height;
	CRect rect(const_left_intever,y,cx-const_left_intever,cy-22);	// 22
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->MoveWindow(&rect);
	}
	if (m_treeTraned.GetSafeHwnd())
	{
		m_treeTraned.MoveWindow(&rect);
	}
}

void CDlgFileManager::OnBnClickedButtonMin()
{
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgFileManager::OnBnClickedButtonMax()
{
	int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	//int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);

	static CRect theRestoreRect;
	CRect rect;
	GetWindowRect(&rect);
	if (rect.Width() == m_nScreenWidth)
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("最大化"));
		m_btnMax.Load(IDB_PNG_MAX);
		MoveWindow(&theRestoreRect);
	}else
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
		m_btnMax.Load(IDB_PNG_RESTORE);
		theRestoreRect = rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + m_nScreenWidth;
		rect.bottom = rect.top + m_nScreenHeight;
		MoveWindow(&rect);
	}
}

void CDlgFileManager::OnBnClickedButtonClose()
{
	this->PostMessage(WM_CLOSE, 0, 0);
}

void CDlgFileManager::OnPaint()
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
		if (theApp.GetColorGradient())
			this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
		else
			this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(30,theDefaultBgColorTitleLight0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);

		CString sWindowText;
		sWindowText.Format(_T("文件传输"));
		USES_CONVERSION;
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		const Gdiplus::PointF pointTitle(13,13);
		graphics.DrawString(A2W_ACP(sWindowText),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	}
}

void CDlgFileManager::OnClose()
{
	CEbDialogBase::OnClose();
}

#ifdef USES_EBCOM_TEST
void CDlgFileManager::OnSendingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->OnSendingFile(pCrFileInfo);
		this->Invalidate();
		//OnBnClickedButtonFiles();
	}
}
#else
void CDlgFileManager::OnSendingFile(const CCrFileInfo * pCrFileInfo)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->OnSendingFile(pCrFileInfo);
		this->Invalidate();
		//OnBnClickedButtonFiles();
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgFileManager::OnReceivingFile(IEB_ChatFileInfo* pCrFileInfo)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->OnReceivingFile(pCrFileInfo);
		this->Invalidate();
		//OnBnClickedButtonFiles();
	}
}
#else
void CDlgFileManager::OnReceivingFile(const CCrFileInfo * pCrFileInfo)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->OnReceivingFile(pCrFileInfo);
		this->Invalidate();
		//OnBnClickedButtonFiles();
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgFileManager::OnReceivedFile(IEB_ChatFileInfo* pCrFileInfo)
{
	DeleteDlgTranFile(pCrFileInfo->MsgId);
}
#else
void CDlgFileManager::OnReceivedFile(const CCrFileInfo * pCrFileInfo)
{
	DeleteDlgTranFile(pCrFileInfo->m_nMsgId);
}
#endif

void CDlgFileManager::DeleteDlgTranFile(eb::bigint nMsgId)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->DeleteDlgTranFile(nMsgId);
		this->Invalidate();
		//if (m_pPanFiles->IsEmpty())
		//{
		//	m_labelFiles.ShowWindow(SW_HIDE);
		//}
	}
}
bool CDlgFileManager::IsEmpty(void) const
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
		return m_pPanFiles->IsEmpty();
	return true;
}

#ifdef USES_EBCOM_TEST
void CDlgFileManager::SetFilePercent(IEB_ChatFilePercent* pChatRoomFilePercent)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->SetFilePercent(pChatRoomFilePercent);
	}
}
#else
void CDlgFileManager::SetFilePercent(const CChatRoomFilePercent * pChatRoomFilePercent)
{
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->SetFilePercent(pChatRoomFilePercent);
	}
}
#endif

void CDlgFileManager::OnBnClickedButtonTraning()
{
	// TODO: Add your control notification handler code here
	m_btnTraning.SetChecked(true);
	m_btnTraning.Invalidate();
	m_btnTraned.SetChecked(false);
	m_btnTraned.Invalidate();
	theApp.InvalidateParentRect(&m_btnTraned);
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->ShowWindow(SW_SHOW);
	}
	m_treeTraned.ShowWindow(SW_HIDE);
}

void CDlgFileManager::OnBnClickedButtonTraned()
{
	m_btnTraning.SetChecked(false);
	m_btnTraning.Invalidate();
	theApp.InvalidateParentRect(&m_btnTraning);
	m_btnTraned.SetChecked(true);
	m_btnTraned.Invalidate();
	if (m_pPanFiles != NULL && m_pPanFiles->GetSafeHwnd() != NULL)
	{
		m_pPanFiles->ShowWindow(SW_HIDE);
	}
	m_treeTraned.ShowWindow(SW_SHOW);
}

void CDlgFileManager::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_LOAD_FILE_TRANED==nIDEvent)
	{
		KillTimer(TIMERID_LOAD_FILE_TRANED);

		m_pItemItemInfo.clear();
		m_treeTraned.DeleteAllItems();
		CString sSql;
		sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_text FROM msg_record_t ")\
			_T("WHERE msg_type=%d AND from_uid<>%lld ORDER BY msg_time"),MRT_FILE,theApp.GetLogonUserId());	// 不查找本端发送文件

		USES_CONVERSION;
		//CString sMsgTime;
		time_t nLocalMsgTime = 0;
		int nCookie = 0;
		theApp.m_pBoUsers->select(sSql, nCookie);
		cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
		while (pRecord.get()!=NULL)
		{
			const tstring sMsgTime = pRecord->getVector()[0]->getStrValue();
			time_t nMsgTime = 0;
			libEbc::ChangeTime(sMsgTime.c_str(),nMsgTime);
			const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
			const tstring soffTime = pRecord->getVector()[2]->getStrValue();
			const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
			const tstring sFromName(libEbc::UTF82ACP(pRecord->getVector()[4]->getStrValue().c_str()));
			const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
			const tstring sToName(libEbc::UTF82ACP(pRecord->getVector()[6]->getStrValue().c_str()));
			const int nPrivate = pRecord->getVector()[7]->getIntValue();
			const tstring sMsgText(libEbc::UTF82ACP(pRecord->getVector()[8]->getStrValue().c_str()));

			const tstring sFileName = libEbc::GetFileName(sMsgText);
			CTime pTime(nMsgTime);
			CString sItemName;
			sItemName.Format(_T(" %s\n %s - %s"),sFileName.c_str(),pTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S")),sFromName.c_str());
			CTreeItemInfo::pointer pTreeItemInfo;
			if (!m_pItemItemInfo.find(sMsgId,pTreeItemInfo))
			{
				HTREEITEM hItem = m_treeTraned.InsertItem(sItemName);
				pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_FILE,hItem);
				pTreeItemInfo->m_nBigId = sMsgId;
				m_pItemItemInfo.insert(sMsgId,pTreeItemInfo);
				m_treeTraned.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
			}else
			{
				m_treeTraned.SetItemText(pTreeItemInfo->m_hItem,sItemName);
			}
			pTreeItemInfo->m_sName = sMsgText;	// 文件路径
			pRecord = theApp.m_pBoUsers->next(nCookie);
		}
		theApp.m_pBoUsers->reset(nCookie);
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}else if (TIMERID_DELETE_TRACK_FILE==nIDEvent)
	{
		KillTimer(TIMERID_DELETE_TRACK_FILE);
		DeleteItemFile(m_treeTraned.GetTrackItem());
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgFileManager::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnBnClickedButtonMax();
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}

HICON CDlgFileManager::GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
{
	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
	if (pTreeItemInfo == NULL) return NULL;
	SHFILEINFO sfi; 
	ZeroMemory(&sfi,sizeof(sfi)); 
	DWORD ret = SHGetFileInfo(pTreeItemInfo->m_sName.c_str(),
		FILE_ATTRIBUTE_NORMAL, 
		&sfi, 
		sizeof(sfi), 
		SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
	if (ret == 1)
	{
		return sfi.hIcon;
	}
	return NULL;
}
LRESULT CDlgFileManager::OnTreeItemDblclk(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hDblClkItem = (HTREEITEM)wp;
	const VividTree* pOwner = (const VividTree*)lp;
	OpenItemFile(hDblClkItem);
	return 0;
}
LRESULT CDlgFileManager::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	const VividTree* pTreeCtrl = (const VividTree*)lp;
	if (item == NULL)
	{
		m_btnHotDeleteFile.ShowWindow(SW_HIDE);
		m_btnHotOpenFile.ShowWindow(SW_HIDE);
		m_btnHotOpenDir.ShowWindow(SW_HIDE);
	}else if (pTreeCtrl == &m_treeTraned)
	{
		CRect rectTreeCtrl;
		pTreeCtrl->GetClientRect(&rectTreeCtrl);
		CRect rect;
		pTreeCtrl->GetItemRect(item, &rect, TRUE);
		rect.right = rectTreeCtrl.Width();
		const int const_btn_width = m_btnHotOpenFile.GetImgWidth();
		m_btnHotOpenDir.MovePoint(rect.right-const_btn_width, rect.top);
		m_btnHotOpenDir.ShowWindow(SW_SHOW);
		m_btnHotOpenDir.Invalidate();
		m_btnHotOpenFile.MovePoint(rect.right-const_btn_width*2, rect.top);
		m_btnHotOpenFile.ShowWindow(SW_SHOW);
		m_btnHotOpenFile.Invalidate();
		m_btnHotDeleteFile.MovePoint(rect.right-const_btn_width*3, rect.top);
		m_btnHotDeleteFile.ShowWindow(SW_SHOW);
		m_btnHotDeleteFile.Invalidate();
	}
	return 0;
}
void CDlgFileManager::OpenItemFile(HTREEITEM hItem)
{
	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)m_treeTraned.GetItemData(hItem);
	if (pTreeItemInfo != NULL)
	{
		ShellExecute(NULL, _T("open"), pTreeItemInfo->m_sName.c_str(), NULL, pTreeItemInfo->m_sName.c_str(),SW_SHOW);
	}

}
void CDlgFileManager::DeleteItemFile(HTREEITEM hItem)
{
	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)m_treeTraned.GetItemData(hItem);
	if (pTreeItemInfo != NULL)
	{
		if (CDlgMessageBox::EbDoModal(this,"确定要删除此文件吗？",pTreeItemInfo->m_sName.c_str(),CDlgMessageBox::IMAGE_QUESTION)==IDOK)
		{
			DeleteFile(pTreeItemInfo->m_sName.c_str());
			m_treeTraned.DeleteItem(hItem);
		}
	}
}

BOOL CDlgFileManager::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnHotOpenDir.GetSafeHwnd())
	{
		// 打开目录
		HTREEITEM hItem = m_treeTraned.GetTrackItem();
		const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)m_treeTraned.GetItemData(hItem);
		if (pTreeItemInfo != NULL)
		{
			const std::string sPath = "/select, "+pTreeItemInfo->m_sName;
			ShellExecute(NULL, NULL, _T("explorer.exe"), sPath.c_str(), NULL, SW_SHOW);
		}
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnHotOpenFile.GetSafeHwnd())
	{
		// 打开文件
		OpenItemFile(m_treeTraned.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnHotDeleteFile.GetSafeHwnd())
	{
		// 删除文件
		SetTimer(TIMERID_DELETE_TRACK_FILE,10,NULL);
	}

	return CEbDialogBase::PreTranslateMessage(pMsg);
}
void CDlgFileManager::OnBnClickedButtonRefresh()
{
	SetTimer(TIMERID_LOAD_FILE_TRANED,100,NULL);
}
