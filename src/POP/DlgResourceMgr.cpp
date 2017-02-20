// DlgResourceMgr.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgResourceMgr.h"

//static TCHAR BASED_CODE theAllFilesFilter[] = _T("All Files (*.*)|*.*||");

// CDlgResourceMgr dialog

IMPLEMENT_DYNAMIC(CDlgResourceMgr, CEbDialogBase)

CDlgResourceMgr::CDlgResourceMgr(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgResourceMgr::IDD, pParent)
{
	m_bChildMode = false;
	m_nManagerType = RES_MANAGER_GROUP;
	m_nSelectType = RES_SELECT_ALLRES;
	m_bPostCloseMsg = false;
	m_hNewDirTempItem = NULL;
	m_sMoveResId = 0;
	m_sAccount = 0;
	m_sGroupCode = 0;
}

CDlgResourceMgr::~CDlgResourceMgr()
{
}

void CDlgResourceMgr::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_ALLRES, m_btnAllRes);
	DDX_Control(pDX, IDC_BUTTON_ALLFILE, m_btnAllFile);
	DDX_Control(pDX, IDC_BUTTON_ALLNOTE, m_btnAllNote);
	DDX_Control(pDX, IDC_BUTTON_ALL_NOTPARENT, m_btnAllNotParent);
	DDX_Control(pDX, IDC_BUTTON_NEWDIR, m_btnNewDir);
	DDX_Control(pDX, IDC_BUTTON_NEWFILE, m_btnNewFile);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btnRefresh);

	DDX_Control(pDX, IDC_TREE_DIR, m_treeDir);
	DDX_Control(pDX, IDC_TREE_ITEM, m_treeItem);
}


BEGIN_MESSAGE_MAP(CDlgResourceMgr, CEbDialogBase)
	//ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgResourceMgr::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgResourceMgr::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgResourceMgr::OnBnClickedButtonClose)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIR, &CDlgResourceMgr::OnTvnSelchangedTreeDir)
	ON_MESSAGE(WM_ITEM_SEL_CHANGED, OnTreeItemSelChanged)
	ON_MESSAGE(WM_ITEM_DOUBLE_CLICK, OnTreeItemDblclk)
	ON_MESSAGE(WM_ITEM_TRACK_HOT, OnTreeItemTrackHot)

	ON_NOTIFY(NM_RCLICK, IDC_TREE_DIR, &CDlgResourceMgr::OnNMRClickTreeDir)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, &CDlgResourceMgr::OnNMRClickTreeItem)
	ON_COMMAND(EB_COMMAND_NEW_DIR_RES, OnNewDirRes)
	ON_COMMAND(EB_COMMAND_NEW_SUBDIR_RES, OnNewSubDirRes)
	ON_COMMAND(EB_COMMAND_NEW_FILE_RES, OnNewFileRes)
	ON_COMMAND(EB_COMMAND_NEW_NOTE_RES, OnNewNoteRes)
	ON_COMMAND(EB_COMMAND_DOWNLOAD_FILE, OnDownloadFile)
	ON_COMMAND(EB_COMMAND_ONLINE_VIEW, OnOnlineView)
	ON_COMMAND(EB_COMMAND_DELETE_DIR_RES, OnDeleteDirRes)
	ON_COMMAND(EB_COMMAND_EDIT_DIR_RES, OnEditDirRes)
	ON_COMMAND(EB_COMMAND_DELETE_ITEM_RES, OnDeleteItemRes)
	ON_MESSAGE(EB_COMMAND_DELETE_ITEM_RES, OnMsgDeleteItemRes)
	ON_COMMAND_RANGE(EB_COMMAND_MOVE_2_DIR0,EB_COMMAND_MOVE_2_DIRX, OnMove2Dir)

	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDlgResourceMgr::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_ALLFILE, &CDlgResourceMgr::OnBnClickedButtonAllfile)
	ON_BN_CLICKED(IDC_BUTTON_ALLNOTE, &CDlgResourceMgr::OnBnClickedButtonAllnote)
	ON_BN_CLICKED(IDC_BUTTON_ALLRES, &CDlgResourceMgr::OnBnClickedButtonAllres)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_DIR, &CDlgResourceMgr::OnTvnEndlabeleditTreeDir)
	ON_BN_CLICKED(IDC_BUTTON_ALL_NOTPARENT, &CDlgResourceMgr::OnBnClickedButtonAllNotparent)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_NEWDIR, &CDlgResourceMgr::OnBnClickedButtonNewdir)
	ON_BN_CLICKED(IDC_BUTTON_NEWFILE, &CDlgResourceMgr::OnBnClickedButtonNewfile)
END_MESSAGE_MAP()

void CDlgResourceMgr::SetCtrlColor(bool bInvalidate)
{
	m_btnNewDir.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetHotColor());
	m_btnRefresh.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetHotColor());
	m_btnNewFile.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetNormalColor(),theApp.GetHotColor());

	m_btnMin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnMax.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);

	//m_treeDir.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());
	//m_treeItem.SetBkGradients(theApp.GetBgColor1(),theApp.GetBgColor1());

	if (bInvalidate)
		this->Invalidate();
}

// CDlgResourceMgr message handlers

BOOL CDlgResourceMgr::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	if (m_bChildMode)
	{
		ModifyStyle(WS_POPUP, WS_CHILD);
		this->SetMouseMove(FALSE);
	}else
	{
		ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
		this->SetSplitterBorder();
	}
	this->EnableToolTips(TRUE);
	//this->SetToolTipText(IDC_TREE_DIR,_T("点击编辑，回车保存！"));	// *使用 TVS_SINGLEEXPAND 后不支持
	this->SetToolTipText(IDC_TREE_ITEM,_T("支持鼠标拖拉文件上传"));

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
	//m_btnAllRes.SetAutoSize(false);
	//m_btnAllRes.Load(IDB_PNG_89X35);
	//m_btnAllFile.SetAutoSize(false);
	//m_btnAllFile.Load(IDB_PNG_89X35);
	//m_btnAllNote.SetAutoSize(false);
	//m_btnAllNote.Load(IDB_PNG_89X35);
	//m_btnAllNotParent.SetAutoSize(false);
	//m_btnAllNotParent.Load(IDB_PNG_89X35);

	m_btnNewDir.SetToolTipText(_T("新建根目录"));
	m_btnNewDir.SetTextHotMove(false);
	m_btnNewDir.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnNewDir.SetPreTextColor(theDefaultBtnWhiteColor);
	m_btnRefresh.SetToolTipText(_T("刷新"));
	m_btnRefresh.SetTextHotMove(false);
	m_btnRefresh.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnRefresh.SetPreTextColor(theDefaultBtnWhiteColor);
	m_btnNewFile.SetToolTipText(_T("上传文件到云盘中"));
	m_btnNewFile.SetTextHotMove(false);
	m_btnNewFile.SetHotTextColor(theDefaultBtnWhiteColor);
	m_btnNewFile.SetPreTextColor(theDefaultBtnWhiteColor);
	//m_btnNewFile.Load(IDB_PNG_68X27G);
	m_btnAllRes.ShowWindow(SW_HIDE);
	m_btnAllFile.ShowWindow(SW_HIDE);
	m_btnAllNote.ShowWindow(SW_HIDE);
	m_btnAllNotParent.ShowWindow(SW_HIDE);

	m_treeDir.SetCallback((CTreeCallback*)&theApp);
	//m_treeDir.ModifyStyle(0, TVS_SHOWSELALWAYS);	// *
	m_treeDir.ModifyStyle(0, TVS_SINGLEEXPAND|TVS_SHOWSELALWAYS);	// *
	//m_treeDir.SetTreeOpenClosedBmp(IDB_TREE_OPENED, IDB_TREE_CLOSED);
	m_treeDir.SetItemHeight(28);
	m_treeDir.SetIconSize(24,24);
	m_treeDir.SetBkMode(VividTree::BK_MODE_GRADIENT);

	SetCtrlColor();
	if (m_bChildMode)
	{
		m_btnMin.ShowWindow(SW_HIDE);
		m_btnMax.ShowWindow(SW_HIDE);
		m_btnClose.ShowWindow(SW_HIDE);
	}
	//float h,s,l;
	//::RGBtoHSL(GetRValue(theApp.GetMainColor()), GetGValue(theApp.GetMainColor()), GetBValue(theApp.GetMainColor()),&h,&s,&l);
	//COLORREF color2 = ::HSLtoRGB(h, s, 0.85);	// 内框 RGB(196,234,247)
	//m_treeDir.SetBkColor(color2);
	//m_treeDir.SetItemIcon( theApp.GetIconDep() );
	//CString sName = _T("所有云文件");
	//HTREEITEM hItem = m_treeDir.InsertItem(sName);
	//m_pAllFileItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hItem);
	//m_pAllFileItemInfo->m_sName = sName;
	//m_treeDir.SetItemData(hItem,(DWORD)m_pAllFileItemInfo.get());
	//sName = _T("所有云笔记");
	//hItem = m_treeDir.InsertItem(sName);
	//m_pAllNoteItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hItem);
	//m_pAllNoteItemInfo->m_sName = sName;
	//m_treeDir.SetItemData(hItem,(DWORD)m_pAllNoteItemInfo.get());

	m_btnHotNewSubDir.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeDir, 0xffff);
	m_btnHotNewSubDir.Load(IDB_PNG_HOT_NEWS);
	m_btnHotNewSubDir.SetToolTipText(_T("新建子目录"));
	m_btnHotNewSubDir.SetAutoFocus(true);
	m_btnHotDeleteItem.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeItem, 0xffff);
	m_btnHotDeleteItem.Load(IDB_PNG_HOT_DELETE);
	m_btnHotDeleteItem.SetToolTipText(_T("删除"));
	m_btnHotDeleteItem.SetAutoFocus(true);
	m_btnHotDownload.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), &m_treeItem, 0xffff);
	m_btnHotDownload.Load(IDB_PNG_BTN_HOTDOWN);
	m_btnHotDownload.SetToolTipText(_T("下载"));
	m_btnHotDownload.SetAutoFocus(true);

	m_treeItem.SetCallback((CTreeCallback*)&theApp);
	m_treeItem.ModifyStyle(TVS_SHOWSELALWAYS, TVS_SINGLEEXPAND);
	//m_treeItem.SetBkMode(VividTree::BK_MODE_GRADIENT);
	m_treeItem.SetItemHeight(40);
	m_treeItem.SetIconSize(32,32);
	//m_treeItem.SetItemIcon( theApp.GetIconDep() );
	m_treeItem.SetIntervalLine(true,theDefaultBtnWhiteColor,theDefaultFlatBgColor);

	//m_treeDir.ModifyStyle(TVS_SHOWSELALWAYS, TVS_SINGLEEXPAND);
	m_treeDir.SetBkGradients(theDefaultFlatBgColor,theDefaultFlatBgColor);
	//m_treeItem.SetBkGradients(theDefaultFlatBgColor,theDefaultFlatBgColor);

	this->SetWindowText(_T("云盘"));
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgResourceMgr::OnTimer(UINT_PTR nIDEvent)
{

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgResourceMgr::OnSize(UINT nType, int cx, int cy)
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

	const int const_interval = 1;
	const int const_btn_width = 60;
	const int const_btn_height = 24;
	const int const_dir_width = 180;
	int x = 10;
	int y = m_bChildMode?8:50;
	m_btnNewDir.MovePoint(x,y,const_btn_width,const_btn_height);
	x += (const_btn_width+5);				// 按钮间隔
	m_btnNewFile.MovePoint(x,y,const_btn_width,const_btn_height);
	x = cx-const_btn_width-10;			// 最右边间隔
	m_btnRefresh.MovePoint(x,y,const_btn_width,const_btn_height);
	//m_btnAllRes.MovePoint(x,y);
	//x += (const_btn_width+const_interval);
	//m_btnAllFile.MovePoint(x,y);
	//x += (const_btn_width+const_interval);
	//m_btnAllNote.MovePoint(x,y);
	//x += (const_btn_width+const_interval);
	//m_btnAllNotParent.MovePoint(x,y);
	//x += (const_btn_width+const_interval);
	//m_btnRefresh.MovePoint(x,y);
	//y += (m_btnRefresh.GetImgHeight()+const_interval);
	y = m_bChildMode?39:81;
	CRect rect;
	if (m_treeDir.GetSafeHwnd())
	{
		rect.left = const_interval;
		rect.right = rect.left + const_dir_width;
		rect.top = y;
		rect.bottom = cy-20;				// 最下面间隔
		m_treeDir.MoveWindow(&rect);
	}
	if (m_treeItem.GetSafeHwnd())
	{
		rect.left = rect.right+1;			// 中间间隔
		rect.right = cx-const_interval;
		m_treeItem.MoveWindow(&rect);
	}

}

void CDlgResourceMgr::OnDestroy()
{
	m_pItemItemInfo.clear();
	CEbDialogBase::OnDestroy();

	delete this;
}

void CDlgResourceMgr::OnBnClickedButtonMin()
{
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgResourceMgr::OnBnClickedButtonMax()
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

void CDlgResourceMgr::OnBnClickedButtonClose()
{
	if (m_bChildMode)
	{
		this->PostMessage(WM_CLOSE, 0, 0);
		return;
	}
	if (!m_bPostCloseMsg)
	{
		m_bPostCloseMsg = true;
		this->PostMessage(WM_CLOSE, 0, 0);
	}
}

void CDlgResourceMgr::OnPaint()
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
		if (!m_bChildMode)
		{
			this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		}
		this->AddBgDrawInfo(CEbBackDrawInfo(38,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,181,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,1,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,0,theDefaultFlatBgColor));	// *
		this->AddBgDrawInfo(CEbBackDrawInfo(-20,1.0,false,false,0,theDefaultFlatLineColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(-19,1.0,false,false,0,theDefaultFlatBgColor));
		if (m_bChildMode)
		{
			this->DrawPopBg(&memDC, theApp.GetMainColor(), 0);
			return;
		}
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
		//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,theDefaultBgColorTitleHeight,2);

		//if (theApp.GetColorGradient())
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
		//else
		//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(38,theDefaultBgColorBgLight1,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(1,theDefaultBgColorBgLight3,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false,true,181));	// 1+180
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false,true,1));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false,true,0));
		//this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);
		//Gdiplus::Image * pImage;
		//if (libEbc::ImageFromIDResource(IDB_PNG_IMG1_CLOUD,_T("png"),pImage))
		//{
		//	graphics.DrawImage(pImage,PointF(10,12));
		//	delete pImage;
		//}
		//if (libEbc::ImageFromIDResource(IDB_PNG_IMG2_CLOUD,_T("png"),pImage))
		//{
		//	graphics.DrawImage(pImage,PointF(2,31));
		//	delete pImage;
		//}
		graphics.DrawImage(theApp.m_image1Cloud,PointF(10,12));
		graphics.DrawImage(theApp.m_image2Cloud,PointF(2,31));
		CString sWindowText;
		if (m_nManagerType == RES_MANAGER_GROUP)
			sWindowText.Format(_T("%s 群共享"),m_sName.c_str());
		else
			sWindowText.Format(_T("个人云盘"));
		USES_CONVERSION;
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		const Gdiplus::PointF pointTitle(40,11);
		graphics.DrawString(A2W_ACP(sWindowText),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	}
}

void CDlgResourceMgr::OnClose()
{
	if (m_bChildMode)
	{
		CEbDialogBase::OnClose();
	}else
	{
		if (m_sAccount>0)
			theApp.m_pResourceMgr.remove(m_sAccount);
		if (m_sGroupCode>0)
			theApp.m_pResourceMgr.remove(m_sGroupCode);
		theApp.m_pCloseWnd.add(this);
		//CEbDialogBase::OnClose();	// 这个关闭太慢
		this->EndDialog(IDOK);
	}
}

void CDlgResourceMgr::LoadGroupResource(eb::bigint sGroupCode)
{
	m_nManagerType = RES_MANAGER_GROUP;
	m_sGroupCode = sGroupCode;
#ifdef USES_EBCOM_TEST
	const CEBString sGroupName = theEBClientCore->EB_GetGroupName(m_sGroupCode).GetBSTR();
	if (sGroupName.empty()) return;
	m_sName = sGroupName.c_str();
	theEBClientCore->EB_LoadRes(m_sGroupCode);
#else
	tstring sGroupName;
	if (!theEBAppClient.EB_GetGroupName(m_sGroupCode,sGroupName)) return;
	m_sName = sGroupName.c_str();
	theEBAppClient.EB_LoadRes(m_sGroupCode);
#endif
	if (!m_bChildMode)
	{
		CString sWindowText;
		sWindowText.Format(_T("%s 群共享"),m_sName.c_str());
		this->SetWindowText(sWindowText);
		this->ShowWindow(SW_SHOW);
		this->SetForegroundWindow();
	}
}
void CDlgResourceMgr::LoadMyResource(void)
{
	m_nManagerType = RES_MANAGER_MY;
	m_sAccount = theApp.GetLogonUserId();
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_LoadRes(0);
#else
	theEBAppClient.EB_LoadRes();
#endif
	this->SetWindowText(_T("个人云盘"));
	this->ShowWindow(SW_SHOW);
	this->SetForegroundWindow();
}

#ifdef USES_EBCOM_TEST
void CDlgResourceMgr::OnResourceMove(IEB_ResourceInfo* pResourceInfo,const std::string& sOldParentResId)
{
	//const eb::bigint sResId(pResourceInfo->ResId);
	//CTreeItemInfo::pointer pTreeItemInfo;
	//if (m_pItemItemInfo.find(sResId,pTreeItemInfo,true))
	//{
	//	m_treeItem.DeleteItem(pTreeItemInfo->m_hItem);
	//}
	OnResourceInfo(pResourceInfo);
}
#else
void CDlgResourceMgr::OnResourceMove(const EB_ResourceInfo& pResourceInfo,eb::bigint nOldParentResId)
{
	//CTreeItemInfo::pointer pTreeItemInfo;
	//if (m_pItemItemInfo.find(nOldParentResId,pTreeItemInfo,true))
	////if (m_pItemItemInfo.find(pResourceInfo.m_sResId,pTreeItemInfo,true))
	//{
	//	m_treeItem.DeleteItem(pTreeItemInfo->m_hItem);
	//}
	OnResourceInfo(pResourceInfo);
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgResourceMgr::OnResourceDelete(IEB_ResourceInfo* pResourceInfo)
{
	const eb::bigint sResId(pResourceInfo->ResId);
	const short nResType = pResourceInfo->ResType;
	CTreeItemInfo::pointer pTreeItemInfo;
	if (nResType==EB_RESOURCE_DIR)
	{
		if (m_pDirItemInfo.find(sResId,pTreeItemInfo,true))
		{
			m_treeDir.DeleteItem(pTreeItemInfo->m_hItem);
		}
	}
	if (m_pItemItemInfo.find(sResId,pTreeItemInfo,false))
	{
		m_treeItem.DeleteItem(pTreeItemInfo->m_hItem);
	}
	if (m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId == sResId)
	{
		m_pSelectDirItemInfo.reset();
	}
}
#else
void CDlgResourceMgr::OnResourceDelete(const EB_ResourceInfo& pResourceInfo)
{
	CTreeItemInfo::pointer pTreeItemInfo;
	if (pResourceInfo.m_nResType==EB_RESOURCE_DIR)
	{
		if (m_pDirItemInfo.find(pResourceInfo.m_sResId,pTreeItemInfo,true))
		{
			m_treeDir.DeleteItem(pTreeItemInfo->m_hItem);
		}
	}
	if (m_pItemItemInfo.find(pResourceInfo.m_sResId,pTreeItemInfo,false))
	{
		m_treeItem.DeleteItem(pTreeItemInfo->m_hItem);
	}
	if (m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId == pResourceInfo.m_sResId)
	{
		m_pSelectDirItemInfo.reset();
	}
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgResourceMgr::OnResourceInfo(IEB_ResourceInfo* pResourceInfo)
{
	if (m_pAllResParent.get()==NULL)
	{
		HTREEITEM hItem = m_treeDir.InsertItem(_T("所有资源"));
		m_pAllResParent = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hItem);
		m_treeDir.SetItemData(hItem,(DWORD)m_pAllResParent.get());
	}

	const eb::bigint sResId(pResourceInfo->ResId);
	const eb::bigint sParentResId = pResourceInfo->ParentResId;
	const CEBString sName(pResourceInfo->Name.GetBSTR());
	SHORT nResType = pResourceInfo->ResType;
	if (nResType==(short)EB_RESOURCE_DIR)
	{
		HTREEITEM hParentItem = TVI_ROOT;
		if (sParentResId>0)
		{
			CTreeItemInfo::pointer pTreeItemInfo;
			if (!m_pDirItemInfo.find(sParentResId,pTreeItemInfo))
			{
				return;
			}
			hParentItem = pTreeItemInfo->m_hItem;
		}
		CTreeItemInfo::pointer pTreeItemInfo;
		if (!m_pDirItemInfo.find(sResId,pTreeItemInfo))
		{
			HTREEITEM hItem = m_treeDir.InsertItem(sName.c_str(),hParentItem);
			pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hItem);
			pTreeItemInfo->m_sId = sResId;
			m_pDirItemInfo.insert(sResId,pTreeItemInfo);
			m_treeDir.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
		}else
		{
			m_treeDir.SetItemText(pTreeItemInfo->m_hItem,sName.c_str());
		}
		pTreeItemInfo->m_sParentId = sParentResId;
		pTreeItemInfo->m_sName = sName;
		pTreeItemInfo->m_nSubType = (int)nResType;
	}else if ((m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId==sParentResId) ||
		m_nSelectType == RES_SELECT_ALLRES ||
		(m_nSelectType==RES_SELECT_ALLNOTDIR && sParentResId==0) ||
		(m_nSelectType==RES_SELECT_ALLFILE && nResType==EB_RESOURCE_FILE) ||
		(m_nSelectType==RES_SELECT_ALLNOTE && nResType==EB_RESOURCE_NOTE))
	{
		NewItem(pResourceInfo);
	}
}
#else
void CDlgResourceMgr::OnResourceInfo(const EB_ResourceInfo& pResourceInfo)
{
	if (m_pAllResParent.get()==NULL)
	{
		HTREEITEM hItem = m_treeDir.InsertItem(_T("所有资源"));
		m_pAllResParent = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hItem);
		m_treeDir.SetItemData(hItem,(DWORD)m_pAllResParent.get());
	}

	if (pResourceInfo.m_nResType==EB_RESOURCE_DIR)
	{
		HTREEITEM hParentItem = TVI_ROOT;
		if (pResourceInfo.m_sParentResId>0)
		{
			CTreeItemInfo::pointer pTreeItemInfo;
			if (!m_pDirItemInfo.find(pResourceInfo.m_sParentResId,pTreeItemInfo))
			{
				return;
			}
			hParentItem = pTreeItemInfo->m_hItem;
		}
		CTreeItemInfo::pointer pTreeItemInfo;
		m_pDirItemInfo.find(pResourceInfo.m_sResId,pTreeItemInfo);
		if (pTreeItemInfo.get()!=NULL && pTreeItemInfo->m_sParentId!=pResourceInfo.m_sParentResId)
		{
			// 目录更换
			m_pDirItemInfo.remove(pTreeItemInfo->m_sId);
			m_treeDir.DeleteItem(pTreeItemInfo->m_hItem);
			if (m_pItemItemInfo.find(pTreeItemInfo->m_sId,pTreeItemInfo,true))
			{
				m_treeItem.DeleteItem(pTreeItemInfo->m_hItem);
			}
			if (m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId>0 && m_pSelectDirItemInfo->m_sId != pResourceInfo.m_sParentResId)
			//if (m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId != pResourceInfo.m_sParentResId)
			{
				// 不是当前选择目录;
				return;
			}
			pTreeItemInfo.reset();
		}
		if (pTreeItemInfo.get()==NULL)
		{
			HTREEITEM hItem = m_treeDir.InsertItem(pResourceInfo.m_sName.c_str(),hParentItem);
			pTreeItemInfo = CTreeItemInfo::create(CTreeItemInfo::ITEM_TYPE_DIR,hItem);
			pTreeItemInfo->m_sId = pResourceInfo.m_sResId;
			m_pDirItemInfo.insert(pResourceInfo.m_sResId,pTreeItemInfo);
			m_treeDir.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
		}else
		{
			m_treeDir.SetItemText(pTreeItemInfo->m_hItem,pResourceInfo.m_sName.c_str());
		}
		pTreeItemInfo->m_sParentId = pResourceInfo.m_sParentResId;
		pTreeItemInfo->m_sName = pResourceInfo.m_sName;
		pTreeItemInfo->m_nSubType = (int)pResourceInfo.m_nResType;
	}else if ((m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId==pResourceInfo.m_sParentResId) ||
		m_nSelectType == RES_SELECT_ALLRES ||
		(m_nSelectType==RES_SELECT_ALLNOTDIR && pResourceInfo.m_sParentResId==0) ||
		(m_nSelectType==RES_SELECT_ALLFILE && pResourceInfo.m_nResType==EB_RESOURCE_FILE) ||
		(m_nSelectType==RES_SELECT_ALLNOTE && pResourceInfo.m_nResType==EB_RESOURCE_NOTE))
	{
		NewItem(pResourceInfo);
	}
}
#endif

LRESULT CDlgResourceMgr::OnTreeItemSelChanged(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hSelItem = (HTREEITEM)wp;
	if (hSelItem==NULL) return 0;
	const VividTree * pOwnerTree = (VividTree*)lp;
	if (pOwnerTree != &m_treeDir || m_treeDir.IsOnDeleteItem(hSelItem))
		return 0;
	const CTreeItemInfo* pTreeItemInfo = (const CTreeItemInfo*)m_treeDir.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return 0;

	m_pSelectDirItemInfo.reset();
	const bool bIsAllResParent = pTreeItemInfo == m_pAllResParent.get();
	if (bIsAllResParent)
		m_pSelectDirItemInfo = m_pAllResParent;
	else
		m_pDirItemInfo.find(pTreeItemInfo->m_sId,m_pSelectDirItemInfo);
#ifdef USES_EBCOM_TEST
	_variant_t pResourceInfoList;
	if (bIsAllResParent && m_nManagerType==RES_MANAGER_GROUP)
		pResourceInfoList = theEBClientCore->EB_GetAllRes(m_sGroupCode);
	else if (bIsAllResParent && m_nManagerType==RES_MANAGER_MY)
		pResourceInfoList = theEBClientCore->EB_GetAllRes(0);
	else
		pResourceInfoList = theEBClientCore->EB_GetDirAllRes(pTreeItemInfo->m_sId);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	if (pResourceInfoList.vt!=VT_EMPTY && pResourceInfoList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pResourceInfoList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;

			CComPtr<IEB_ResourceInfo> pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
			NewItem(pInterface);
		}
	}
#else
	std::vector<EB_ResourceInfo> pResourceInfoList;
	if (bIsAllResParent && m_nManagerType==RES_MANAGER_GROUP)
		theEBAppClient.EB_GetAllRes(pResourceInfoList,m_sGroupCode);
	else if (bIsAllResParent && m_nManagerType==RES_MANAGER_MY)
		theEBAppClient.EB_GetAllRes(pResourceInfoList);
	else
		theEBAppClient.EB_GetDirAllRes(pTreeItemInfo->m_sId,pResourceInfoList);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	for (size_t i=0; i<pResourceInfoList.size(); i++)
	{
		const EB_ResourceInfo & pResourceInfo = pResourceInfoList[i];
		if (pResourceInfo.m_nResType==EB_RESOURCE_DIR)
			OnResourceInfo(pResourceInfo);
		NewItem(pResourceInfo);
	}
#endif
	return 0;
}

//void CDlgResourceMgr::OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
//	const CTreeItemInfo* pTreeItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeDir.GetItemData(hSelItem);
//	if (pTreeItemInfo == NULL) return;
//
//	m_pSelectDirItemInfo.reset();
//	const bool bIsAllResParent = pTreeItemInfo == m_pAllResParent.get();
//	if (bIsAllResParent)
//		m_pSelectDirItemInfo = m_pAllResParent;
//	else
//		m_pDirItemInfo.find(pTreeItemInfo->m_sId,m_pSelectDirItemInfo);
//#ifdef USES_EBCOM_TEST
//	_variant_t pResourceInfoList;
//	if (bIsAllResParent && m_nManagerType==RES_MANAGER_GROUP)
//		pResourceInfoList = theEBClientCore->EB_GetAllRes(m_sGroupCode);
//	else if (bIsAllResParent && m_nManagerType==RES_MANAGER_MY)
//		pResourceInfoList = theEBClientCore->EB_GetAllRes(0);
//	else
//		pResourceInfoList = theEBClientCore->EB_GetDirAllRes(pTreeItemInfo->m_sId);
//	m_pItemItemInfo.clear();
//	m_treeItem.DeleteAllItems();
//	if (pResourceInfoList.vt!=VT_EMPTY && pResourceInfoList.parray != NULL)
//	{
//		CComSafeArray<VARIANT> m_sa(pResourceInfoList.parray);
//		for (ULONG i=0;i<m_sa.GetCount();i++)
//		{
//			CComVariant var = m_sa.GetAt(i);
//			if (var.vt != VT_DISPATCH)
//				continue;
//
//			CComPtr<IEB_ResourceInfo> pInterface;
//			var.pdispVal->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
//			NewItem(pInterface);
//		}
//	}
//#else
//	std::vector<EB_ResourceInfo> pResourceInfoList;
//	if (bIsAllResParent && m_nManagerType==RES_MANAGER_GROUP)
//		theEBAppClient.EB_GetAllRes(pResourceInfoList,m_sGroupCode);
//	else if (bIsAllResParent && m_nManagerType==RES_MANAGER_MY)
//		theEBAppClient.EB_GetAllRes(pResourceInfoList);
//	else
//		theEBAppClient.EB_GetDirAllRes(pTreeItemInfo->m_sId,pResourceInfoList);
//	m_pItemItemInfo.clear();
//	m_treeItem.DeleteAllItems();
//	for (size_t i=0; i<pResourceInfoList.size(); i++)
//	{
//		NewItem(pResourceInfoList[i]);
//	}
//#endif
//}

//HICON CDlgResourceMgr::GetItemIcon(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
//{
//	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo == NULL) return NULL;
//	//EB_ResourceInfo pResourceInfo;
//	//if (!theEBAppClient.EB_GetResourceInfo(pTreeItemInfo->m_sId,&pResourceInfo)) return NULL;
//
//	switch (pTreeItemInfo->m_nItemType)
//	{
//	case CTreeItemInfo::ITEM_TYPE_DIR:
//		return NULL;
//		//return theApp.GetIconDep();
//	case CTreeItemInfo::ITEM_TYPE_FILE:
//		{
//			return theApp.GetFileHIcon(pTreeItemInfo->m_sName);
//		}break;
//	case CTreeItemInfo::ITEM_TYPE_NOTE:
//		{
//		}break;
//	default:
//		break;
//	}
//	return NULL;
//}
//int CDlgResourceMgr::GetItemState(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem, HICON& pOutIcon) const
//{
//	// 正常显示
//	return 1;
//}
//bool CDlgResourceMgr::GetItemDrawOpenClose(const CTreeCtrl& pTreeCtrl,HTREEITEM hItem) const
//{
//	if (pTreeCtrl.GetSafeHwnd()==m_treeItem.GetSafeHwnd()) return false;	// ITEM不显示
//	const CTreeItemInfo * pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl.GetItemData(hItem);
//	if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) return true;
//	return false;
//}
LRESULT CDlgResourceMgr::OnTreeItemDblclk(WPARAM wp, LPARAM lp)
{
	const HTREEITEM hDblClkItem = (HTREEITEM)wp;
	const VividTree* pOwner = (const VividTree*)lp;

	if (pOwner->GetSafeHwnd() == m_treeItem.GetSafeHwnd())
	{
		const CTreeItemInfo * pTreeItemInfo = hDblClkItem==NULL?NULL:(const CTreeItemInfo*)m_treeItem.GetItemData(hDblClkItem);
		if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
		{
			CTreeItemInfo::pointer pDirTreeItemInfo;
			if (m_pDirItemInfo.find(pTreeItemInfo->m_sId,pDirTreeItemInfo))
			{
				HTREEITEM hParentItem = m_treeDir.GetParentItem(pDirTreeItemInfo->m_hItem);
				if (hParentItem!=NULL)
					m_treeDir.Expand(hParentItem,TVE_EXPAND);
				m_treeDir.SelectItem(pDirTreeItemInfo->m_hItem);
				m_treeDir.Select(pDirTreeItemInfo->m_hItem,TVGN_CARET);
			}
		}else if (pTreeItemInfo != NULL && pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE)
		{
			DownloadFile(m_treeItem.GetSelectedItem());
		}
	}

	return 0;
}
LRESULT CDlgResourceMgr::OnTreeItemTrackHot(WPARAM wp, LPARAM lp)
{
	HTREEITEM item = (HTREEITEM)wp;
	const VividTree* pTreeCtrl = (const VividTree*)lp;
	if (item == NULL)
	{
		m_btnHotNewSubDir.ShowWindow(SW_HIDE);
		m_btnHotDeleteItem.ShowWindow(SW_HIDE);
		m_btnHotDownload.ShowWindow(SW_HIDE);
	}else if (pTreeCtrl == &m_treeDir)
	{
		const CTreeItemInfo* pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl->GetItemData(item);
		if (pTreeItemInfo == m_pAllResParent.get())
		{
			m_btnHotNewSubDir.ShowWindow(SW_HIDE);
			return 1;
		}
		CRect rectTreeCtrl;
		pTreeCtrl->GetClientRect(&rectTreeCtrl);
		CRect rect;
		pTreeCtrl->GetItemRect(item, &rect, TRUE);
		rect.right = rectTreeCtrl.Width();
		const int const_btn_width = m_btnHotNewSubDir.GetImgWidth();// 24*2;
		m_btnHotNewSubDir.MovePoint(rect.right-const_btn_width, rect.top);
		m_btnHotNewSubDir.ShowWindow(SW_SHOW);
		m_btnHotNewSubDir.Invalidate();
	}else if (pTreeCtrl == &m_treeItem)
	{
		const CTreeItemInfo* pTreeItemInfo = (const CTreeItemInfo*)pTreeCtrl->GetItemData(item);
		if (pTreeItemInfo == NULL) return 1;

		//if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR)
		//{

		//}else if (m_pViewContactInfo!=NULL)
		//{
		//	m_pViewContactInfo->ShowWindow(SW_HIDE);
		//}
		//if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)
		if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE)
		{
			CRect rectTreeCtrl;
			pTreeCtrl->GetClientRect(&rectTreeCtrl);
			CRect rect;
			pTreeCtrl->GetItemRect(item, &rect, TRUE);
			rect.right = rectTreeCtrl.Width();
			//rect.right = pTreeCtrl->GetHSize();
			const int const_btn_width = m_btnHotDownload.GetImgWidth();// 24*2;
			m_btnHotDeleteItem.MovePoint(rect.right-const_btn_width, rect.top);
			m_btnHotDeleteItem.ShowWindow(SW_SHOW);
			m_btnHotDeleteItem.Invalidate();
			if (pTreeItemInfo->m_dwItemData==1)
			{
				m_btnHotDownload.MovePoint(rect.right-const_btn_width*2, rect.top);
				m_btnHotDownload.ShowWindow(SW_SHOW);
				m_btnHotDownload.Invalidate();
			}else
			{
				m_btnHotDownload.ShowWindow(SW_HIDE);
			}
		}else
		{
			m_btnHotDeleteItem.ShowWindow(SW_HIDE);
			m_btnHotDownload.ShowWindow(SW_HIDE);
		}
	}
	return 0;
}

void CDlgResourceMgr::OnNMRClickTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (!theApp.IsLogonVisitor())
	{
		CString sText;
		CNewMenu m_menu2;
		CNewMenu m_menuSub;
		m_menu2.CreatePopupMenu();
#ifdef USES_EBCOM_TEST
		//const bool bCanAddGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
#else
		const bool bCanAddGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,0);
#endif
		if ((m_nManagerType==RES_MANAGER_GROUP && bCanAddGroupRes) || (m_nManagerType==RES_MANAGER_MY))
			m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_DIR_RES,_T("新建根目录"));

		const HTREEITEM hSelItem = m_treeDir.SelectHitTest();
		//HTREEITEM hSelItem = m_treeDir.GetSelectedItem();
		const CTreeItemInfo * pTreeItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeDir.GetItemData(hSelItem);
		if (pTreeItemInfo!=NULL && pTreeItemInfo!=m_pAllResParent.get())
		{
#ifdef USES_EBCOM_TEST
			//const bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
			//const bool bCanDeleteGroupRes = theEBClientCore->GetEB_CanDeleteGroupRes(this->m_sGroupCode)?true:false;
#else
			const bool bCanEditGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,pTreeItemInfo->m_sId);
			const bool bCanDeleteGroupRes = theEBAppClient.EB_CanDeleteGroupRes(this->m_sGroupCode,pTreeItemInfo->m_sId);
#endif
			if ((m_nManagerType==RES_MANAGER_GROUP && bCanEditGroupRes) ||
				(m_nManagerType==RES_MANAGER_MY))
			{
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_SUBDIR_RES,_T("新建子目录"));
				sText.Format(_T("重命名[%s]"),pTreeItemInfo->m_sName.c_str());
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_EDIT_DIR_RES,sText);

				// **
				if (!m_pDirItemInfo.empty())
				{
					BoostReadLock rdlock(m_pDirItemInfo.mutex());
					//boost::mutex::scoped_lock lock(m_pDirItemInfo.mutex());
					CLockMap<eb::bigint,CTreeItemInfo::pointer>::const_iterator pIter = m_pDirItemInfo.begin();
					m_sMove2DirResId.clear();
					m_sMoveResId = pTreeItemInfo->m_sId;
					for (;pIter!=m_pDirItemInfo.end();pIter++)
					{
						const CTreeItemInfo::pointer pDirTreeItemInfo = pIter->second;
						if (pDirTreeItemInfo->m_sId == pTreeItemInfo->m_sParentId ||										// 当前目录
							(pTreeItemInfo->m_nSubType==EB_RESOURCE_DIR && pTreeItemInfo->m_sId==pDirTreeItemInfo->m_sId))	// 本身目录
							continue;
						if (m_menuSub.GetSafeHmenu()==NULL)
							m_menuSub.CreatePopupMenu();
						m_menuSub.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MOVE_2_DIR0+m_sMove2DirResId.size(),pDirTreeItemInfo->m_sName.c_str());
						m_sMove2DirResId.push_back(pDirTreeItemInfo->m_sId);
						if (m_sMove2DirResId.size()>=(EB_COMMAND_MOVE_2_DIRX-EB_COMMAND_MOVE_2_DIR0))
						{
							break;
						}
					}
					if (m_menuSub.GetSafeHmenu()!=NULL)
					{
						//sText.Format(_T("[%s]移到"),pTreeItemInfo->m_sName.c_str());
						//m_menu2.AppendMenu(MF_POPUP|MF_STRING,(UINT)m_menuSub.GetSafeHmenu(),sText);
						m_menu2.AppendMenu(MF_POPUP|MF_STRING,(UINT)m_menuSub.GetSafeHmenu(),_T("移到"));
					}
				}
			}
			if (m_treeDir.GetChildItem(hSelItem)==NULL && 
				((m_nManagerType==RES_MANAGER_GROUP && bCanDeleteGroupRes) ||
				(m_nManagerType==RES_MANAGER_MY)))
			{
				sText.Format(_T("删除[%s]"),pTreeItemInfo->m_sName.c_str());
				m_menu2.AppendODMenu(sText,MF_BYPOSITION,EB_COMMAND_DELETE_DIR_RES,&theApp.m_pMenuImageList,MENU_TOOLBAR_ICON_OFFSET_DELETE);
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_DIR_RES,sText);
			}
		}
		CPoint point;
		GetCursorPos(&point);
		m_menu2.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
	}
}
BOOL CDlgResourceMgr::IsTreeCtrlEditMessage(WPARAM KeyCode)  
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
    CWnd*  focus = GetFocus();  
    CEdit* edit  = m_treeDir.GetEditControl();  
    if ((CEdit *) focus == edit)  
    {
        edit->SendMessage(WM_KEYDOWN, KeyCode);
		if (KeyCode==VK_ESCAPE && m_hNewDirTempItem != NULL)
		{
			DWORD dwItemData = m_treeDir.GetItemData(m_hNewDirTempItem);
			if (dwItemData == 0)
			{
				m_treeDir.DeleteItem(m_hNewDirTempItem);
				m_hNewDirTempItem = NULL;
			}
		}
		return TRUE;
	}
    edit  = m_treeItem.GetEditControl();  
    if ((CEdit *) focus == edit)  
    {  
        edit->SendMessage(WM_KEYDOWN, KeyCode);
		return TRUE;
	}
    return FALSE;  
} 
void CDlgResourceMgr::OnNMRClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (!theApp.IsLogonVisitor())
	{
		CString sText;
		CNewMenu m_menu2;
		CNewMenu m_menuSub;
		m_menu2.CreatePopupMenu();
		m_menu2.SetBitmapBackground(RGB(192,192,192));
#ifdef USES_EBCOM_TEST
		//bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
#else
		bool bCanAddGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,0);
#endif
		if ((m_nManagerType==RES_MANAGER_GROUP && bCanAddGroupRes) || (m_nManagerType==RES_MANAGER_MY))
		{
			m_menu2.InsertODMenu(-1, _T("上传文件"),MF_BYPOSITION,EB_COMMAND_NEW_FILE_RES,IDB_BITMAP_MENU_UPLOAD);
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_FILE_RES,_T("上传文件"));
			//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_NEW_NOTE_RES,_T("添加云笔记"));
		}
		const HTREEITEM hSelItem = m_treeItem.SelectHitTest();
		//HTREEITEM hSelItem = m_treeItem.GetSelectedItem();
		const CTreeItemInfo * pTreeItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeItem.GetItemData(hSelItem);
		if (pTreeItemInfo !=NULL)
		{
#ifdef USES_EBCOM_TEST
			//bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
			//bool bCanDeleteGroupRes = theEBClientCore->GetEB_CanDeleteGroupRes(this->m_sGroupCode)?true:false;
#else
			bool bCanEditGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,pTreeItemInfo->m_sId);
			bool bCanDeleteGroupRes = theEBAppClient.EB_CanDeleteGroupRes(this->m_sGroupCode,pTreeItemInfo->m_sId);
#endif
			if (!pTreeItemInfo->m_sGroupName.empty())	// ** online-view-url
			{
				m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_ONLINE_VIEW,_T("在线浏览"));
			}

			//if (pTreeItemInfo->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE)
			//	m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DOWNLOAD_FILE,_T("下载"));
			if ((m_nManagerType==RES_MANAGER_GROUP && bCanEditGroupRes) ||
				(m_nManagerType==RES_MANAGER_MY))
			{
				if (!m_pDirItemInfo.empty())
				{
					BoostReadLock rdlock(m_pDirItemInfo.mutex());
					//boost::mutex::scoped_lock lock(m_pDirItemInfo.mutex());
					CLockMap<eb::bigint,CTreeItemInfo::pointer>::const_iterator pIter = m_pDirItemInfo.begin();
					m_sMove2DirResId.clear();
					m_sMoveResId = pTreeItemInfo->m_sId;
					for (;pIter!=m_pDirItemInfo.end();pIter++)
					{
						const CTreeItemInfo::pointer pDirTreeItemInfo = pIter->second;
						if (pDirTreeItemInfo->m_sId == pTreeItemInfo->m_sParentId ||										// 当前目录
							(pTreeItemInfo->m_nSubType==EB_RESOURCE_DIR && pTreeItemInfo->m_sId==pDirTreeItemInfo->m_sId))	// 本身目录
							continue;
						if (m_menuSub.GetSafeHmenu()==NULL)
							m_menuSub.CreatePopupMenu();
						m_menuSub.AppendMenu(MF_BYCOMMAND,EB_COMMAND_MOVE_2_DIR0+m_sMove2DirResId.size(),pDirTreeItemInfo->m_sName.c_str());
						m_sMove2DirResId.push_back(pDirTreeItemInfo->m_sId);
						if (m_sMove2DirResId.size()>=(EB_COMMAND_MOVE_2_DIRX-EB_COMMAND_MOVE_2_DIR0))
						{
							break;
						}
					}
					if (m_menuSub.GetSafeHmenu()!=NULL)
					{
						//sText.Format(_T("[%s]移到"),pTreeItemInfo->m_sName.c_str());
						//m_menu2.AppendMenu(MF_POPUP|MF_STRING,(UINT)m_menuSub.GetSafeHmenu(),sText);
						m_menu2.AppendMenu(MF_POPUP|MF_STRING,(UINT)m_menuSub.GetSafeHmenu(),_T("移到"));
					}
				}
			}
			if ((m_nManagerType==RES_MANAGER_GROUP && bCanDeleteGroupRes) || (m_nManagerType==RES_MANAGER_MY))
			{
				sText.Format(_T("删除[%s]"),pTreeItemInfo->m_sName.c_str());
				m_menu2.InsertODMenu(-1,sText,MF_BYPOSITION,EB_COMMAND_DELETE_ITEM_RES,IDB_BITMAP_MENU_DELSHARE);
				//m_menu2.AppendMenu(MF_BYCOMMAND,EB_COMMAND_DELETE_ITEM_RES,sText);
			}
		}
		CPoint point;
		GetCursorPos(&point);
		m_menu2.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
	}
}

void CDlgResourceMgr::OnNewDirRes(void)
{
#ifdef USES_EBCOM_TEST
	//bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
	//bool bCanDeleteGroupRes = theEBClientCore->GetEB_CanDeleteGroupRes(this->m_sGroupCode)?true:false;
#else
	bool bCanEditGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,0);
	//bool bCanDeleteGroupRes = theEBAppClient.EB_CanDeleteGroupRes(this->m_sGroupCode);
#endif
	if ((m_nManagerType==RES_MANAGER_GROUP && bCanEditGroupRes) || (m_nManagerType==RES_MANAGER_MY))
	{
		m_hNewDirTempItem = m_treeDir.InsertItem(_T("新建根目录"));
		m_treeDir.SetItemData(m_hNewDirTempItem,0);
		m_treeDir.ModifyStyle(0, TVS_EDITLABELS);
		m_treeDir.EditLabel(m_hNewDirTempItem);
	}else
	{
		CDlgMessageBox::EbMessageBox(this,"",_T("没有管理员权限：\r\n不能新建目录！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
	//CEdit * edit = m_treeDir.EditLabel(hNewItem);
	//CRect rect;
	//edit->GetWindowRect(&rect);
	//rect.left += 30;
	//rect.right += 30;
	//edit->MoveWindow(&rect);
	//m_treeDir.SelectItem(hNewItem);
}

void CDlgResourceMgr::NewSubDirRes(HTREEITEM hParentItem)
{
#ifdef USES_EBCOM_TEST
	//bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
	//bool bCanDeleteGroupRes = theEBClientCore->GetEB_CanDeleteGroupRes(this->m_sGroupCode)?true:false;
#else
	bool bCanEditGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,0);
	//bool bCanDeleteGroupRes = theEBAppClient.EB_CanDeleteGroupRes(this->m_sGroupCode);
#endif
	if ((m_nManagerType==RES_MANAGER_GROUP && bCanEditGroupRes) || (m_nManagerType==RES_MANAGER_MY))
	{
		m_hNewDirTempItem = m_treeDir.InsertItem(_T("新建子目录"),hParentItem);
		if (hParentItem!=NULL)
			m_treeDir.Expand(hParentItem,TVE_EXPAND);
		m_treeDir.ModifyStyle(0, TVS_EDITLABELS);
		m_treeDir.EditLabel(m_hNewDirTempItem);
		//m_treeDir.SelectItem(hNewItem);
	}else
	{
		CDlgMessageBox::EbMessageBox(this,"",_T("没有管理员权限：\r\n不能新建目录！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
}
void CDlgResourceMgr::OnNewSubDirRes(void)
{
	NewSubDirRes(m_treeDir.GetSelectedItem());
}

void CDlgResourceMgr::OnNewFileRes(void)
{
#ifdef USES_EBCOM_TEST
	//bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
	//bool bCanDeleteGroupRes = theEBClientCore->GetEB_CanDeleteGroupRes(this->m_sGroupCode)?true:false;
#else
	bool bCanEditGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,0);
	//bool bCanDeleteGroupRes = theEBAppClient.EB_CanDeleteGroupRes(this->m_sGroupCode);
#endif
	if ((m_nManagerType==RES_MANAGER_GROUP && bCanEditGroupRes) || (m_nManagerType==RES_MANAGER_MY))
	{
		const eb::bigint sParentResId = m_pSelectDirItemInfo.get()==NULL?0:m_pSelectDirItemInfo->m_sId;
		CFileDialog dlg(TRUE, 0, NULL, OFN_READONLY|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, theAllFilesFilter, this);
		const DWORD MAXFILE = 30 * 256;
		dlg.m_ofn.nMaxFile = MAXFILE;
		char* pc = new char[MAXFILE];
		dlg.m_ofn.lpstrFile = pc;
		dlg.m_ofn.lpstrFile[0] = NULL;
		if (dlg.DoModal() == IDOK)
		{
			POSITION pos = dlg.GetStartPosition(); 
			while (pos)
			{
				CString sPathName = dlg.GetNextPathName(pos);
#ifdef USES_EBCOM_TEST
				if (m_nManagerType==RES_MANAGER_GROUP)
					theEBClientCore->EB_AddFileRes((LPCTSTR)sPathName,"","",sParentResId,m_sGroupCode);
				else if (m_nManagerType==RES_MANAGER_MY)
					theEBClientCore->EB_AddFileRes((LPCTSTR)sPathName,"","",sParentResId,0);
#else
				if (m_nManagerType==RES_MANAGER_GROUP)
					theEBAppClient.EB_AddFileRes(sPathName,"","",sParentResId,m_sGroupCode);
				else if (m_nManagerType==RES_MANAGER_MY)
					theEBAppClient.EB_AddFileRes(sPathName,"","",sParentResId);
#endif
			}
		}
		delete []pc;
	}else
	{
		CDlgMessageBox::EbMessageBox(this,"",_T("没有管理员权限：\r\n不能上传文件！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
}
void CDlgResourceMgr::OnNewNoteRes(void)
{

}
void CDlgResourceMgr::OnDownloadFile(void)
{
	DownloadFile(m_treeItem.GetSelectedItem());
}
void CDlgResourceMgr::OnOnlineView(void)
{
	HTREEITEM hItem = m_treeItem.GetSelectedItem();
	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)m_treeItem.GetItemData(hItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_FILE) return;
	if (!pTreeItemInfo->m_sGroupName.empty())	// online-view-url
	{
		COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(pTreeItemInfo->m_sGroupName.c_str(),"");
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
	}
}
void CDlgResourceMgr::OnDeleteDirRes(void)
{
	HTREEITEM hSelItem = m_treeDir.GetSelectedItem();
	CTreeItemInfo* pTreeItemInfo = hSelItem==NULL?NULL:(CTreeItemInfo*)m_treeDir.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
#ifdef USES_EBCOM_TEST
	if (theEBClientCore->EB_HasSubRes(pTreeItemInfo->m_sId))
#else
	if (theEBAppClient.EB_HasSubRes(pTreeItemInfo->m_sId))
#endif
	{
		CDlgMessageBox::EbDoModal(this,_T(""),_T("不能删除非空目录：\r\n请检查后重试！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		return;
	}

	CString sText;
	sText.Format(_T("确定删除：\r\n%s 吗？"), pTreeItemInfo->m_sName.c_str());
	if (CDlgMessageBox::EbDoModal(this,"删除目录",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
	//if (MessageBox(sText, _T("删除"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
	{
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_DeleteRes(pTreeItemInfo->m_sId);
#else
		theEBAppClient.EB_DeleteRes(pTreeItemInfo->m_sId);
#endif
	}
}
void CDlgResourceMgr::OnEditDirRes(void)
{
	HTREEITEM nSelItem = m_treeDir.GetSelectedItem();
	if (nSelItem == NULL) return;
	m_treeDir.ModifyStyle(0, TVS_EDITLABELS);
	m_treeDir.EditLabel(nSelItem);
}
void CDlgResourceMgr::DeleteItemRes(HTREEITEM hSelItem)
{
	const CTreeItemInfo* pTreeItemInfo = hSelItem==NULL?NULL:(const CTreeItemInfo*)m_treeItem.GetItemData(hSelItem);
	if (pTreeItemInfo == NULL) return;
#ifdef USES_EBCOM_TEST
	//bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
	//bool bCanDeleteGroupRes = theEBClientCore->GetEB_CanDeleteGroupRes(this->m_sGroupCode)?true:false;
#else
	//bool bCanEditGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode);
	bool bCanDeleteGroupRes = theEBAppClient.EB_CanDeleteGroupRes(this->m_sGroupCode,pTreeItemInfo->m_sId);
#endif
	if ((m_nManagerType==RES_MANAGER_GROUP && bCanDeleteGroupRes) || (m_nManagerType==RES_MANAGER_MY))
	{
		if (pTreeItemInfo->m_nItemType == CTreeItemInfo::ITEM_TYPE_DIR)
		{
#ifdef USES_EBCOM_TEST
			if (theEBClientCore->EB_HasSubRes(pTreeItemInfo->m_sId))
#else
			if (theEBAppClient.EB_HasSubRes(pTreeItemInfo->m_sId))
#endif
			{
				CDlgMessageBox::EbDoModal(this,_T(""),_T("不能删除非空目录：\r\n请检查后重试！"),CDlgMessageBox::IMAGE_ERROR,true,5);
				return;
			}
		}

		CString sText;
		sText.Format(_T("确定删除：\r\n%s 吗？"), pTreeItemInfo->m_sName.c_str());
		if (CDlgMessageBox::EbDoModal(this,"删除资源",sText,CDlgMessageBox::IMAGE_QUESTION)==IDOK)
			//if (MessageBox(sText, _T("删除"), MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
		{
#ifdef USES_EBCOM_TEST
			theEBClientCore->EB_DeleteRes(pTreeItemInfo->m_sId);
#else
			theEBAppClient.EB_DeleteRes(pTreeItemInfo->m_sId);
#endif
		}
	}else
	{
		CDlgMessageBox::EbMessageBox(this,"",_T("没有管理员权限：\r\n不能删除资源！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
}

void CDlgResourceMgr::OnDeleteItemRes(void)
{
	DeleteItemRes(m_treeItem.GetSelectedItem());
}
LRESULT CDlgResourceMgr::OnMsgDeleteItemRes(WPARAM wParam, LPARAM lParam)
{
	DeleteItemRes((HTREEITEM)wParam);
	return 0;
}

void CDlgResourceMgr::OnMove2Dir(UINT nID)
{
	size_t nIndex = nID-EB_COMMAND_MOVE_2_DIR0;
	if (nIndex < 0 || nIndex>=m_sMove2DirResId.size())
	{
		return;
	}
	const eb::bigint sMove2DirResId = m_sMove2DirResId[nIndex];
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_MoveRes2Dir(m_sMoveResId,sMove2DirResId);
#else
	theEBAppClient.EB_MoveRes2Dir(m_sMoveResId,sMove2DirResId);
#endif
	m_sMove2DirResId.clear();
	m_sMoveResId = 0;
}

void CDlgResourceMgr::OnBnClickedButtonRefresh()
{
#ifdef USES_EBCOM_TEST
	if (m_nManagerType==RES_MANAGER_GROUP)
		theEBClientCore->EB_LoadRes(m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		theEBClientCore->EB_LoadRes(0);
#else
	if (m_nManagerType==RES_MANAGER_GROUP)
		theEBAppClient.EB_LoadRes(m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		theEBAppClient.EB_LoadRes();
#endif
}
CTreeItemInfo::ITEM_TYPE GetItemType(EB_RESOURCE_TYPE nResType)
{
	switch (nResType)
	{
	case EB_RESOURCE_DIR:
		return CTreeItemInfo::ITEM_TYPE_DIR;
	case EB_RESOURCE_FILE:
		return CTreeItemInfo::ITEM_TYPE_FILE;
	case EB_RESOURCE_NOTE:
		return CTreeItemInfo::ITEM_TYPE_NOTE;
	default:
		return CTreeItemInfo::ITEM_TYPE_OTHERRES;
	}
}

#ifdef USES_EBCOM_TEST
void CDlgResourceMgr::NewItem(IEB_ResourceInfo* pResourceInfo)
{
	//if (pResourceInfo.m_nResType==EB_RESOURCE_DIR)
	//{
	//}
	CEBString sTime(pResourceInfo->Time.GetBSTR());
	std::string::size_type nFindTime = sTime.find(" ");
	if (nFindTime!=std::string::npos)
	{
		sTime = sTime.substr(0,nFindTime);
	}
	const eb::bigint sResId(pResourceInfo->ResId);
	const eb::bigint sParentResId(pResourceInfo->ParentResId);
	const CEBString sName(pResourceInfo->Name.GetBSTR());
	const unsigned long nSize = pResourceInfo->Size;
	const unsigned long nDownloads = pResourceInfo->Downloads;
	const short nResType = pResourceInfo->ResType;
	CString sItemName(sName.c_str());
	if (nResType==EB_RESOURCE_FILE)
	{
		const float theOneKB = 1024.0;
		const float theOneMB = 1024*1024.0;
		CString sSize;
		if (nSize>=theOneMB)
			sSize.Format(_T("%.02fMB"),(nSize/theOneMB));
		else if (nSize>=theOneKB)
			sSize.Format(_T("%.02fKB"),(nSize/theOneKB));
		else
			sSize.Format(_T("%d字节"),nSize);
		if (m_sGroupCode==0)
		{
			sItemName.Format(_T("%s\n%s  %s  下载%d次"),sName.c_str(),sTime.c_str(),sSize,nDownloads);
		}else
		{
			const CEBString sMemberName = theEBClientCore->EB_GetMemberNameByUserId(m_sGroupCode,pResourceInfo->CreateUserId).GetBSTR();
			if (pResourceInfo->Share==EB_RESOURCE_SHARE_TEMP)
				sItemName.Format(_T("%s  （群组临时文件，上传者：%s）\n%s  %s  下载%d次"),sName.c_str(),sMemberName.c_str(),sTime.c_str(),sSize,nDownloads);
			else
				sItemName.Format(_T("%s  （上传者：%s）\n%s  %s  下载%d次"),sName.c_str(),sMemberName.c_str(),sTime.c_str(),sSize,nDownloads);
		}

		//???
		//sItemName.Format(_T("%s\n%s  %s  下载%d次"),sName.c_str(),sTime.c_str(),sSize,nDownloads);
	}else// if (nResType!=EB_RESOURCE_DIR)
		sItemName.Format(_T("%s\n%s"),sName.c_str(),sTime.c_str());

	CTreeItemInfo::pointer pTreeItemInfo;
	if (!m_pItemItemInfo.find(sResId,pTreeItemInfo))
	{
		HTREEITEM hItem = m_treeItem.InsertItem(sItemName);
		pTreeItemInfo = CTreeItemInfo::create(GetItemType((EB_RESOURCE_TYPE)nResType),hItem);
		pTreeItemInfo->m_sId = sResId;
		m_pItemItemInfo.insert(sResId,pTreeItemInfo);
		m_treeItem.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
		//m_treeItem.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
	}else
	{
		m_treeItem.SetItemText(pTreeItemInfo->m_hItem,sItemName);
	}
	pTreeItemInfo->m_sParentId = sParentResId;
	pTreeItemInfo->m_sName = sName;
	pTreeItemInfo->m_nSubType = (int)nResType;
}
#else
void CDlgResourceMgr::NewItem(const EB_ResourceInfo& pResourceInfo)
{
	//if (pResourceInfo.m_nResType==EB_RESOURCE_DIR)
	//{
	//}
	tstring sTime(pResourceInfo.m_sTime);
	std::string::size_type nFindTime = pResourceInfo.m_sTime.find(" ");
	if (nFindTime!=std::string::npos)
	{
		sTime = pResourceInfo.m_sTime.substr(0,nFindTime);
	}
	CString sItemName(pResourceInfo.m_sName.c_str());
	if (pResourceInfo.m_nResType==EB_RESOURCE_FILE)
	{
		const float theOneKB = 1024.0;
		const float theOneMB = 1024*1024.0;
		CString sSize;
		if (pResourceInfo.m_nSize>=theOneMB)
			sSize.Format(_T("%.02fMB"),(pResourceInfo.m_nSize/theOneMB));
		else if (pResourceInfo.m_nSize>=theOneKB)
			sSize.Format(_T("%.02fKB"),(pResourceInfo.m_nSize/theOneKB));
		else if (pResourceInfo.m_nSize>0)
			sSize.Format(_T("%d字节"),pResourceInfo.m_nSize);
		else
			sSize.Format(_T("文件上传中..."));
		if (m_sGroupCode==0)
		{
			if (pResourceInfo.m_nDownloads>0)
				sItemName.Format(_T("%s\n%s  %s  下载%d次"),pResourceInfo.m_sName.c_str(),sTime.c_str(),sSize,pResourceInfo.m_nDownloads);
			else
				sItemName.Format(_T("%s\n%s  %s"),pResourceInfo.m_sName.c_str(),sTime.c_str(),sSize);
		}else
		{
			tstring sMemberName;
			theEBAppClient.EB_GetMemberNameByUserId(m_sGroupCode,pResourceInfo.m_nCreateUserId,sMemberName);
			if (pResourceInfo.m_nShare==EB_RESOURCE_SHARE_TEMP)
			{
				if (!pResourceInfo.m_sDeleteTime.empty())
				{
					sItemName.Format(_T("%s到期"),pResourceInfo.m_sDeleteTime.c_str());
					sTime = sItemName;
				}
				if (pResourceInfo.m_nDownloads>0)
					sItemName.Format(_T("%s  （群临时文件，上传者：%s）\n%s  %s  下载%d次"),pResourceInfo.m_sName.c_str(),sMemberName.c_str(),sTime.c_str(),sSize,pResourceInfo.m_nDownloads);
				else
					sItemName.Format(_T("%s  （群临时文件，上传者：%s）\n%s  %s"),pResourceInfo.m_sName.c_str(),sMemberName.c_str(),sTime.c_str(),sSize);
			}else
			{
				if (pResourceInfo.m_nDownloads>0)
					sItemName.Format(_T("%s  （上传者：%s）\n%s  %s  下载%d次"),pResourceInfo.m_sName.c_str(),sMemberName.c_str(),sTime.c_str(),sSize,pResourceInfo.m_nDownloads);
				else
					sItemName.Format(_T("%s  （上传者：%s）\n%s  %s"),pResourceInfo.m_sName.c_str(),sMemberName.c_str(),sTime.c_str(),sSize);
			}
		}
	}else if (pResourceInfo.m_nResType==EB_RESOURCE_DIR)
		sItemName = pResourceInfo.m_sName.c_str();
	else
		sItemName.Format(_T("[%s]\n%s"),pResourceInfo.m_sName.c_str(),sTime.c_str());

	CTreeItemInfo::pointer pTreeItemInfo;
	m_pItemItemInfo.find(pResourceInfo.m_sResId,pTreeItemInfo);
	if (pTreeItemInfo.get()!=NULL && pTreeItemInfo->m_sParentId!=pResourceInfo.m_sParentResId)
	{
		m_pItemItemInfo.remove(pResourceInfo.m_sResId);
		m_treeItem.DeleteItem(pTreeItemInfo->m_hItem);
		if (m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId>0 && m_pSelectDirItemInfo->m_sId != pResourceInfo.m_sParentResId)
		//if (m_pSelectDirItemInfo.get()!=NULL && m_pSelectDirItemInfo->m_sId != pResourceInfo.m_sParentResId)
		{
			// 不是当前选择目录;
			return;
		}
		pTreeItemInfo.reset();
	}
	const bool bNewItem = pTreeItemInfo.get()==NULL?true:false;
	if (pTreeItemInfo.get()==NULL)
	{
		HTREEITEM hItem = m_treeItem.InsertItem(sItemName);
		pTreeItemInfo = CTreeItemInfo::create(GetItemType(pResourceInfo.m_nResType),hItem);
		pTreeItemInfo->m_sId = pResourceInfo.m_sResId;

		time_t nMsgTime = 0;
		libEbc::ChangeTime(pResourceInfo.m_sTime.c_str(),nMsgTime);
		pTreeItemInfo->m_nBigId = nMsgTime;
		//pTreeItemInfo->m_sAccount = pResourceInfo.m_sTime;
		m_pItemItemInfo.insert(pResourceInfo.m_sResId,pTreeItemInfo);
		m_treeItem.SetItemData(hItem,(DWORD)pTreeItemInfo.get());
	}else
	{
		m_treeItem.SetItemText(pTreeItemInfo->m_hItem,sItemName);
	}
	pTreeItemInfo->m_sParentId = pResourceInfo.m_sParentResId;
	pTreeItemInfo->m_sName = pResourceInfo.m_sName;
	pTreeItemInfo->m_nSubType = (int)pResourceInfo.m_nResType;
	pTreeItemInfo->m_dwItemData = pResourceInfo.m_nSize>0?1:0;
	pTreeItemInfo->m_sGroupName = pResourceInfo.m_sOnlineViewUrl;
	if (bNewItem)
		m_treeItem.Sort(TVI_ROOT, CPOPApp::TreeCmpFunc);
}
#endif

void CDlgResourceMgr::DownloadFile(HTREEITEM hItem)
{
	const CTreeItemInfo* pTreeItemInfo = hItem==NULL?NULL:(const CTreeItemInfo*)m_treeItem.GetItemData(hItem);
	if (pTreeItemInfo == NULL || pTreeItemInfo->m_nItemType != CTreeItemInfo::ITEM_TYPE_FILE) return;

	//static TCHAR BASED_CODE theAllFilesFilter[] = _T("All Files (*.*)|*.*||");
	tstring sFileName;
	tstring sFileExt;
	libEbc::GetFileExt(pTreeItemInfo->m_sName,sFileName,sFileExt);
	CFileDialog dlg(FALSE, sFileExt.c_str(), pTreeItemInfo->m_sName.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, theAllFilesFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		const CString sPathName(dlg.GetPathName());
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_DownloadFileRes(pTreeItemInfo->m_sId, (LPCTSTR)sPathName);
#else
		theEBAppClient.EB_DownloadFileRes(pTreeItemInfo->m_sId, sPathName);
#endif
	}
}

void CDlgResourceMgr::OnBnClickedButtonAllfile()
{
	m_nSelectType = RES_SELECT_ALLFILE;
#ifdef USES_EBCOM_TEST
	_variant_t pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		pResourceInfoList = theEBClientCore->EB_GetAllFileRes(m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		pResourceInfoList = theEBClientCore->EB_GetAllFileRes(0);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	if (pResourceInfoList.vt!=VT_EMPTY && pResourceInfoList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pResourceInfoList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_ResourceInfo> pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
			NewItem(pInterface);
		}
	}
#else
	std::vector<EB_ResourceInfo> pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		theEBAppClient.EB_GetAllFileRes(pResourceInfoList,m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		theEBAppClient.EB_GetAllFileRes(pResourceInfoList);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	for (size_t i=0; i<pResourceInfoList.size(); i++)
	{
		NewItem(pResourceInfoList[i]);
	}
#endif
}

void CDlgResourceMgr::OnBnClickedButtonAllnote()
{
	m_nSelectType = RES_SELECT_ALLNOTE;
#ifdef USES_EBCOM_TEST
	_variant_t pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		pResourceInfoList = theEBClientCore->EB_GetAllNoteRes(m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		pResourceInfoList = theEBClientCore->EB_GetAllNoteRes(0);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	if (pResourceInfoList.vt!=VT_EMPTY && pResourceInfoList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pResourceInfoList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_ResourceInfo> pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
			NewItem(pInterface);
		}
	}
#else
	std::vector<EB_ResourceInfo> pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		theEBAppClient.EB_GetAllNoteRes(pResourceInfoList,m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		theEBAppClient.EB_GetAllNoteRes(pResourceInfoList);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	for (size_t i=0; i<pResourceInfoList.size(); i++)
	{
		NewItem(pResourceInfoList[i]);
	}
#endif
}
void CDlgResourceMgr::OnBnClickedButtonAllres()
{
	m_nSelectType = RES_SELECT_ALLRES;
#ifdef USES_EBCOM_TEST
	_variant_t pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		pResourceInfoList = theEBClientCore->EB_GetAllRes(m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		pResourceInfoList = theEBClientCore->EB_GetAllRes(0);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	if (pResourceInfoList.vt!=VT_EMPTY && pResourceInfoList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pResourceInfoList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_ResourceInfo> pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
			NewItem(pInterface);
		}
	}
#else
	std::vector<EB_ResourceInfo> pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		theEBAppClient.EB_GetAllRes(pResourceInfoList,m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		theEBAppClient.EB_GetAllRes(pResourceInfoList);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	for (size_t i=0; i<pResourceInfoList.size(); i++)
	{
		NewItem(pResourceInfoList[i]);
	}
#endif
}
void CDlgResourceMgr::OnBnClickedButtonAllNotparent()
{
	m_nSelectType = RES_SELECT_ALLNOTDIR;
#ifdef USES_EBCOM_TEST
	_variant_t pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		pResourceInfoList = theEBClientCore->EB_GetNotDirAllRes(m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		pResourceInfoList = theEBClientCore->EB_GetNotDirAllRes(0);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	if (pResourceInfoList.vt!=VT_EMPTY && pResourceInfoList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pResourceInfoList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_DISPATCH)
				continue;
			CComPtr<IEB_ResourceInfo> pInterface;
			var.pdispVal->QueryInterface(__uuidof(IEB_ResourceInfo),(void**)&pInterface);
			NewItem(pInterface);
		}
	}
#else
	std::vector<EB_ResourceInfo> pResourceInfoList;
	if (m_nManagerType==RES_MANAGER_GROUP)
		theEBAppClient.EB_GetNotDirAllRes(pResourceInfoList,m_sGroupCode);
	else if (m_nManagerType==RES_MANAGER_MY)
		theEBAppClient.EB_GetNotDirAllRes(pResourceInfoList);
	m_pItemItemInfo.clear();
	m_treeItem.DeleteAllItems();
	for (size_t i=0; i<pResourceInfoList.size(); i++)
	{
		NewItem(pResourceInfoList[i]);
	}
#endif
}

BOOL CDlgResourceMgr::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnHotNewSubDir.GetSafeHwnd())
	{
		NewSubDirRes(m_treeDir.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnHotDownload.GetSafeHwnd())
	{
		DownloadFile(m_treeItem.GetTrackItem());
	}else if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_btnHotDeleteItem.GetSafeHwnd())
	{
		this->PostMessage(EB_COMMAND_DELETE_ITEM_RES,(WPARAM)m_treeItem.GetTrackItem());
	}

	return CEbDialogBase::PreTranslateMessage(pMsg);
}

void CDlgResourceMgr::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	IsTreeCtrlEditMessage(VK_RETURN);

	//__super::OnOK();
}

void CDlgResourceMgr::OnTvnEndlabeleditTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

    CEdit* edit = m_treeDir.GetEditControl();
	if (edit == NULL) return;

	HTREEITEM hEditItem = pTVDispInfo->item.hItem;
	CString sNewItemText;
	edit->GetWindowText(sNewItemText);
	if (sNewItemText.IsEmpty())
	{
		CDlgMessageBox::EbDoModal(this,_T(""),_T("目录名称不能为空：\r\n请重新输入！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		m_treeDir.EditLabel(hEditItem);
		return;
	}
	const CTreeItemInfo * pTreeItemInfo = hEditItem==NULL?NULL:(const CTreeItemInfo*)m_treeDir.GetItemData(hEditItem);
	if (pTreeItemInfo == NULL)
	{
		eb::bigint sParentResId = 0;
		HTREEITEM hParentItem = m_treeDir.GetParentItem(hEditItem);
		const CTreeItemInfo * pParentTreeItemInfo = hParentItem==NULL?NULL:(const CTreeItemInfo*)m_treeDir.GetItemData(hParentItem);
		if (pParentTreeItemInfo != NULL)
		{
			sParentResId = pParentTreeItemInfo->m_sId;
		}

		// 新建目录
#ifdef USES_EBCOM_TEST
		if (m_nManagerType==RES_MANAGER_GROUP)
			theEBClientCore->EB_AddDirRes((LPCTSTR)sNewItemText,sParentResId,m_sGroupCode);
		else if (m_nManagerType==RES_MANAGER_MY)
			theEBClientCore->EB_AddDirRes((LPCTSTR)sNewItemText,sParentResId,0);
#else
		if (m_nManagerType==RES_MANAGER_GROUP)
			theEBAppClient.EB_AddDirRes(sNewItemText,sParentResId,m_sGroupCode);
		else if (m_nManagerType==RES_MANAGER_MY)
			theEBAppClient.EB_AddDirRes(sNewItemText,sParentResId);
#endif
		// 删除原来旧的ITEM
		m_treeDir.DeleteItem(hEditItem);
	}else if (sNewItemText != pTreeItemInfo->m_sName.c_str())
	{
		// 修改目录
#ifdef USES_EBCOM_TEST
		theEBClientCore->EB_EditRes(pTreeItemInfo->m_sId,(LPCTSTR)sNewItemText,"");
#else
		theEBAppClient.EB_EditRes(pTreeItemInfo->m_sId,sNewItemText,"");
#endif
	}
	m_treeDir.ModifyStyle(TVS_EDITLABELS,0);
	m_hNewDirTempItem = NULL;
}

void CDlgResourceMgr::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	IsTreeCtrlEditMessage(VK_ESCAPE);
	//__super::OnCancel();
}

void CDlgResourceMgr::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (this->m_bChildMode)
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX);
	else
		OnBnClickedButtonMax();
	__super::OnLButtonDblClk(nFlags, point);
}
void CDlgResourceMgr::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (this->m_bChildMode && this->GetParent()->GetParent()!=NULL && this->GetParent()->GetParent()->GetParent()!=NULL)
		this->GetParent()->GetParent()->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgResourceMgr::OnBnClickedButtonNewdir()
{
	OnNewDirRes();
}

void CDlgResourceMgr::OnBnClickedButtonNewfile()
{
	OnNewFileRes();
}
void CDlgResourceMgr::OnDropFiles(HDROP hDropInfo)
{
#ifdef USES_EBCOM_TEST
	const bool bCanEditGroupRes = theEBClientCore->GetEB_CanEditGroupRes(this->m_sGroupCode)?true:false;
	//bool bCanDeleteGroupRes = theEBClientCore->GetEB_CanDeleteGroupRes(this->m_sGroupCode)?true:false;
#else
	const bool bCanEditGroupRes = theEBAppClient.EB_CanEditGroupRes(this->m_sGroupCode,0);
	//bool bCanDeleteGroupRes = theEBAppClient.EB_CanDeleteGroupRes(this->m_sGroupCode);
#endif
	if ((m_nManagerType==RES_MANAGER_GROUP && bCanEditGroupRes) || (m_nManagerType==RES_MANAGER_MY))
	{
		const eb::bigint sParentResId = m_pSelectDirItemInfo.get()==NULL?0:m_pSelectDirItemInfo->m_sId;
		const int nFileSum = min(12,DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0));   //得到文件个数
		for (int i=0; i<nFileSum; i++)
		{
			char lpszFile[MAX_PATH];
			memset(lpszFile,0,MAX_PATH);
			DragQueryFile(hDropInfo,i,lpszFile,MAX_PATH);         //得到文件名
#ifdef USES_EBCOM_TEST
			if (m_nManagerType==RES_MANAGER_GROUP)
				theEBClientCore->EB_AddFileRes((LPCTSTR)lpszFile,"","",sParentResId,m_sGroupCode);
			else if (m_nManagerType==RES_MANAGER_MY)
				theEBClientCore->EB_AddFileRes((LPCTSTR)lpszFile,"","",sParentResId,0);
#else
			if (m_nManagerType==RES_MANAGER_GROUP)
				theEBAppClient.EB_AddFileRes(lpszFile,"","",sParentResId,m_sGroupCode);
			else if (m_nManagerType==RES_MANAGER_MY)
				theEBAppClient.EB_AddFileRes(lpszFile,"","",sParentResId);
#endif
		}
	}else
	{
		CDlgMessageBox::EbMessageBox(this,"",_T("没有管理员权限：\r\n不能上传文件！"),CDlgMessageBox::IMAGE_ERROR,5);
	}
	CEbDialogBase::OnDropFiles(hDropInfo);
}
