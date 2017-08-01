// DlgFileManager.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
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
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
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
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CDlgFileManager::OnEnChangeEditSearch)

END_MESSAGE_MAP()

void CDlgFileManager::SetCtrlColor(bool bInvalidate)
{
	m_btnTraning.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetNormalColor());
	m_btnTraned.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetNormalColor());
	m_btnRefresh.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetPreColor());
	m_editSearch.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	//m_treeTraned.SetBkGradients(theApp.GetBgColor2(),theApp.GetBgColor2());

	m_btnMin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnMax.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);

	if (m_pPanFiles!=NULL && m_pPanFiles->GetSafeHwnd()!=NULL)
		m_pPanFiles->SetCtrlColor();

	//m_btnRefresh.SetNorTextColor(RGB(0,128,255));
	//m_btnRefresh.SetHotTextColor(RGB(0,128,255));
	//m_btnRefresh.SetNorTextColor(theApp.GetHotColor2());
	//m_btnRefresh.SetHotTextColor(theApp.GetHotColor2());
	////m_btnRefresh.SetTextHotMove(false);
	////m_btnRefresh.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	if (bInvalidate)
		this->Invalidate();
}

// CDlgFileManager message handlers

BOOL CDlgFileManager::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	//SetTransparentType(CEbDialogBase::TT_DLG);
	this->SetSplitterBorder();
	this->EnableToolTips();
	this->SetToolTipText(IDC_EDIT_SEARCH,_T("搜索接收文件"));

	m_btnMin.SetAutoSize(false);
	m_btnMin.SetDrawToolButtonPic(1,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMin.SetDrawPanelRgn(false);
	m_btnMin.SetToolTipText(_T("最小化"));
	m_btnMax.SetAutoSize(false);
	m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnMax.SetDrawPanelRgn(false);
	m_btnMax.SetToolTipText(_T("最大化"));
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));

	m_btnTraning.SetAutoSize(false);
	m_btnTraning.SetTextHotMove(false);
	m_btnTraning.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnTraning.SetPreTextColor(theDefaultBtnWhiteColor);
	m_btnTraned.SetAutoSize(false);
	m_btnTraned.SetTextHotMove(false);
	m_btnTraned.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnTraned.SetPreTextColor(theDefaultBtnWhiteColor);
	//m_btnTraned.Load(IDB_PNG_TAB_75X30);

	m_btnRefresh.SetTextHotMove(false);
	m_btnRefresh.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnRefresh.SetPreTextColor(theDefaultBtnWhiteColor);

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

	m_editSearch.SetPromptText(_T("搜索文件"));

	m_treeTraned.SetCallback((CTreeCallback*)&theApp);
	m_treeTraned.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeTraned.SetItemHeight(40);
	m_treeTraned.SetIconSize(32,32);
	//m_treeTraned.SetRootIcon( theApp.GetIconCon() );
	//m_treeTraned.SetItemIcon( theApp.GetIconCon() );
	m_treeTraned.ShowWindow(SW_HIDE);
	m_treeTraned.SetBkGradients(theDefaultBtnWhiteColor,theDefaultBtnWhiteColor);
	m_treeTraned.SetIntervalLine(true,theDefaultBtnWhiteColor,theDefaultFlatBgColor);

	m_pPanFiles = new CPanelFiles(this);
	m_pPanFiles->SetCircle(false);
	//m_pPanFiles->m_pChatRoom = m_pChatRoom;
	m_pPanFiles->Create(CPanelFiles::IDD, this);
	m_pPanFiles->ShowWindow(SW_SHOW);

	SetCtrlColor(false);

	OnBnClickedButtonTraned();
	OnBnClickedButtonRefresh();

	this->SetWindowText(_T("文件管理"));
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
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
	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMax.GetSafeHwnd()!=NULL)
		m_btnMax.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_btnMin.GetSafeHwnd()!=NULL)
		m_btnMin.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	const int const_left_intever	= 1;
	const int const_tab_width1 = 60;
	const int const_tab_width2 = 36;
	const int const_tab_height = 22;

	int x = 10;//const_left_intever+5;
	int y = 50;	// 42
	m_btnTraning.MovePoint(x, y,const_tab_width1,const_tab_height);
	x += const_tab_width1+5;
	m_btnTraned.MovePoint(x, y,const_tab_width1,const_tab_height);
	if (m_editSearch.GetSafeHwnd())
	{
		x += const_tab_width1+5;
		m_editSearch.MoveWindow(x,y,120,22);
	}
	x = cx-const_tab_width2-const_left_intever-5;
	m_btnRefresh.MovePoint(x,y,const_tab_width2,const_tab_height);
	y = 80;
	//y += 30;
	CRect rect(const_left_intever,y,cx-const_left_intever,cy-20);
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
		m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
		MoveWindow(&theRestoreRect);
	}else
	{
		this->SetToolTipText(IDC_BUTTON_MAX,_T("向下还原"));
		m_btnMax.SetDrawToolButtonPic(3,theDefaultBtnWhiteColor,-1,-1,-1,2);
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
		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(37,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));	// *
		this->AddBgDrawInfo(CEbBackDrawInfo(-20,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(-19,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
		//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,theDefaultBgColorTitleHeight,2);

		//this->ClearBgDrawInfo();
		//if (theApp.GetColorGradient())
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
		//else
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(29,theDefaultBgColorBgLight1,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(1,theDefaultBgColorBgLight3,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
		//this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);

		CString sWindowText;
		sWindowText.Format(_T("文件传输"));
		USES_CONVERSION;
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		const Gdiplus::PointF pointTitle(12,12);
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
	m_editSearch.ShowWindow(SW_HIDE);
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
	m_editSearch.ShowWindow(SW_SHOW);
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
		CString sSearchString;
		m_editSearch.GetWindowText(sSearchString);

		CString sSql;
		if (sSearchString.IsEmpty())
		{
			sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_text FROM msg_record_t ")\
				_T("WHERE msg_type=%d AND from_uid<>%lld ORDER BY msg_time desc"),MRT_FILE,theApp.GetLogonUserId());	// 不查找本端发送文件
		}else if (libEbc::IsFullNumber(sSearchString,sSearchString.GetLength()))
		{
			const eb::bigint nUserId = eb_atoi64(sSearchString);
			sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_text FROM msg_record_t ")\
				_T("WHERE msg_type=%d AND from_uid<>%lld AND (from_uid=%lld OR msg_text LIKE '%%%s%%' OR from_name LIKE '%%%s%%') ORDER BY msg_time desc"),MRT_FILE,theApp.GetLogonUserId(),
				nUserId,sSearchString,sSearchString);	// 不查找本端发送文件
		}else
		{
			tstring sSearchStringTemp(sSearchString);
			CSqliteCdbc::escape_string_in(sSearchStringTemp);
			const tstring sSearchStringUtf8(libEbc::ACP2UTF8(sSearchStringTemp.c_str()));
			sSql.Format(_T("select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_text FROM msg_record_t ")\
				_T("WHERE msg_type=%d AND from_uid<>%lld AND (msg_text LIKE '%%%s%%' OR from_name LIKE '%%%s%%') ORDER BY msg_time desc"),MRT_FILE,theApp.GetLogonUserId(),
				sSearchStringUtf8.c_str(),sSearchStringUtf8.c_str());	// 不查找本端发送文件
		}

		USES_CONVERSION;
		//CString sMsgTime;
		//time_t nLocalMsgTime = 0;
		int nCookie = 0;
		theApp.m_pBoUsers->select(sSql, nCookie);
		cgcValueInfo::pointer pRecord = theApp.m_pBoUsers->first(nCookie);
		while (pRecord.get()!=NULL)
		{
			const tstring sMsgTime(pRecord->getVector()[0]->getStrValue());
			time_t nMsgTime = 0;
			libEbc::ChangeTime(sMsgTime.c_str(),nMsgTime);
			const eb::bigint sMsgId = pRecord->getVector()[1]->getBigIntValue();
			const tstring soffTime(pRecord->getVector()[2]->getStrValue());
			const eb::bigint sFromAccount = pRecord->getVector()[3]->getBigIntValue();
			const tstring sFromName(libEbc::UTF82ACP(pRecord->getVector()[4]->getStrValue().c_str()));
			//const eb::bigint sToAccount = pRecord->getVector()[5]->getBigIntValue();
			//const tstring sToName(libEbc::UTF82ACP(pRecord->getVector()[6]->getStrValue().c_str()));
			const int nPrivate = pRecord->getVector()[7]->getIntValue();
			const tstring sMsgText(libEbc::UTF82ACP(pRecord->getVector()[8]->getStrValue().c_str()));

			const tstring sFileName(libEbc::GetFileName(sMsgText));
			CTime pTime(nMsgTime);
			CString sItemName;
			int nSubType = 0;
			if (::PathFileExists(sMsgText.c_str()))
			{
				if (sFromName.empty())
					sItemName.Format(_T(" %s\n %s"),sFileName.c_str(),pTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S")));
				else
					sItemName.Format(_T(" %s\n %s - %s"),sFileName.c_str(),pTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S")),sFromName.c_str());
			}else
			{
				nSubType = -1;
				if (sFromName.empty())
					sItemName.Format(_T(" %s（文件不存在）\n %s"),sFileName.c_str(),pTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S")));
				else
					sItemName.Format(_T(" %s（文件不存在）\n %s - %s"),sFileName.c_str(),pTime.FormatGmt(_T("%Y-%m-%d %H:%M:%S")),sFromName.c_str());
			}
			CTreeItemInfo::pointer pTreeItemInfo;
			if (!m_pItemItemInfo.find(sMsgId,pTreeItemInfo))
			{
				HTREEITEM hItem = m_treeTraned.InsertItem(sItemName);
				pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_FILE,hItem);
				pTreeItemInfo->m_sId = sMsgId;
				pTreeItemInfo->m_nUserId  = sFromAccount;
				pTreeItemInfo->m_nBigId  = nMsgTime;
				m_pItemItemInfo.insert(sMsgId,pTreeItemInfo);
				m_treeTraned.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
			}else
			{
				m_treeTraned.SetItemText(pTreeItemInfo->m_hItem,sItemName);
			}
			pTreeItemInfo->m_nSubType = nSubType;
			pTreeItemInfo->m_sName = sMsgText;	// 文件路径
			pRecord = theApp.m_pBoUsers->next(nCookie);
		}
		theApp.m_pBoUsers->reset(nCookie);
		//if (!m_pItemItemInfo.empty())
		//	m_treeTraned.Sort(

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

//HICON CDlgFileManager::GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
//{
//	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return NULL;
//	SHFILEINFO sfi; 
//	ZeroMemory(&sfi,sizeof(sfi)); 
//	DWORD ret = SHGetFileInfo(pTreeItemInfo->m_sName.c_str(),
//		FILE_ATTRIBUTE_NORMAL, 
//		&sfi, 
//		sizeof(sfi), 
//		SHGFI_USEFILEATTRIBUTES|SHGFI_ICON);
//	if (ret == 1)
//	{
//		return sfi.hIcon;
//	}
//	return NULL;
//}
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
		m_btnHotDeleteFile.MovePoint(rect.right-const_btn_width, rect.top);
		m_btnHotDeleteFile.ShowWindow(SW_SHOW);
		m_btnHotDeleteFile.Invalidate();
		m_btnHotOpenDir.MovePoint(rect.right-const_btn_width*2, rect.top);
		m_btnHotOpenDir.ShowWindow(SW_SHOW);
		m_btnHotOpenDir.Invalidate();
		const CTreeItemInfo* pTreeItemInfo = item==NULL?NULL:(const CTreeItemInfo*)m_treeTraned.GetItemData(item);
		if (pTreeItemInfo != NULL && PathFileExists(pTreeItemInfo->m_sName.c_str()))
		{
			m_btnHotOpenFile.MovePoint(rect.right-const_btn_width*3, rect.top);
			m_btnHotOpenFile.ShowWindow(SW_SHOW);
			m_btnHotOpenFile.Invalidate();
		}else
		{
			m_btnHotOpenFile.ShowWindow(SW_HIDE);
		}
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
		const BOOL bFileExist = ::PathFileExists(pTreeItemInfo->m_sName.c_str());
		CString sText;
		if (bFileExist)
		{
			if (pTreeItemInfo->m_nUserId==pTreeItemInfo->m_sId)
				sText.Format(_T("确定删除文件：\r\n%s 和下载记录吗？"),pTreeItemInfo->m_sName.c_str());
			else
				sText.Format(_T("确定删除文件：\r\n%s 和聊天记录吗？"),pTreeItemInfo->m_sName.c_str());
		}
		if (!bFileExist || CDlgMessageBox::EbDoModal(this,"删除文件",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
		{
			if (bFileExist)
				DeleteFile(pTreeItemInfo->m_sName.c_str());
			m_treeTraned.DeleteItem(hItem);
			m_pItemItemInfo.remove(pTreeItemInfo->m_sId);

			CString sSql;
			sSql.Format(_T("DELETE FROM msg_record_t WHERE msg_id=%lld"),pTreeItemInfo->m_sId);
			theApp.m_pBoUsers->execute(sSql);
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
			const tstring m_sPath(pTreeItemInfo->m_sName);
			if (::PathFileExists(m_sPath.c_str()))
			{
				tstring sPath = "/select, \""+m_sPath;
				sPath += "\"";
				ShellExecute(NULL, NULL, "explorer", sPath.c_str(), NULL, SW_SHOW);
			}else
			{
				const std::string::size_type find = m_sPath.rfind('\\');
				if (find!=std::wstring::npos)
				{
					const tstring sPath = m_sPath.substr(0,find+1);
					ShellExecute(NULL, "open", sPath.c_str(), NULL, sPath.c_str(), SW_SHOW);
				}
			}
			//const tstring sPath = "/select, "+pTreeItemInfo->m_sName;
			//ShellExecute(NULL, NULL, _T("explorer.exe"), sPath.c_str(), NULL, SW_SHOW);
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
void CDlgFileManager::OnEnChangeEditSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEbDialogBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	static CString theSearchString;
	CString sString;
	m_editSearch.GetWindowText(sString);
	if (sString==theSearchString)
		return;
	theSearchString = sString;
	if (this->IsWindowVisible())	// 避免关闭窗口后，重新打开；
	{
		OnBnClickedButtonRefresh();
	}
}
