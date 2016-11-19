// DlgFuncWindow.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgFuncWindow.h"
#include "DlgPrompt.h"

#define WINDOW_TITLE_HEIGHT			26	// 36
#define TIMER_REFRESH_BROWSER		0x100
#define TIMER_SET_TITLE				0x101
#define TIMER_HIDE_STATUS_MESSAGE	0x104
#define TIMER_SHOW_STATUS_MESSAGE	0x105
#define TIMER_TO_CLOSE						0x106

// CDlgFuncWindow dialog

IMPLEMENT_DYNAMIC(CDlgFuncWindow, CEbDialogBase)

CDlgFuncWindow::CDlgFuncWindow(CWnd* pParent /*=NULL*/,bool bDeleteThis)
	: CEbDialogBase(CDlgFuncWindow::IDD, pParent)
	, m_bDeleteThis(bDeleteThis)
#ifdef USES_LIBCEF
	, m_pCefBrowser(this)
#endif
{
	m_nFuncBrowserType = EB_FUNC_BROWSER_TYPE_DEFAULT;
#ifdef USES_LIBCEF
	m_nBrowserType = EB_BROWSER_TYPE_CEF;
#else
	m_nBrowserType = EB_BROWSER_TYPE_IE;
#endif
	//m_bChildMode = false;
	m_nMsgType = 0;
	m_bBroadcastMsg = false;
	m_bOpenNewClose = false;
#ifndef USES_CEF_BROADCAST
	m_pBroadcastView = NULL;
#endif
	m_nWidth = 420;
	m_nHeight = 380;
	m_bDisableContextMenu = true;
	m_bDisableScrollBar = false;
	m_bClosePrompt = true;
	m_bWinResizable = true;
	m_bDocumentComplete = false;
	m_pPanelFind = NULL;
	m_pPanelStatus = NULL;
	m_nBroadcastMsgId = 0;
	m_fZoomLevel = 0.0;
}

CDlgFuncWindow::~CDlgFuncWindow()
{
}

void CDlgFuncWindow::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_EXPLORER1, m_pExplorer);
}


BEGIN_MESSAGE_MAP(CDlgFuncWindow, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgFuncWindow::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgFuncWindow::OnBnClickedButtonMax)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgFuncWindow::OnBnClickedButtonClose)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	//ON_MESSAGE(EB_COMMAND_OPEN_APP_URL, OnMsgOpenAppUrl)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgFuncWindow, CEbDialogBase)
	//{{AFX_EVENTSINK_MAP(CDlgFuncWindow)
	ON_EVENT(CDlgFuncWindow, IDC_EXPLORER1, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//DISPID_BEFORENAVIGATE
	//DISPID_NEWWINDOW
	//DISPID_NEWWINDOW2
	//DISPID_NEWWINDOW3
	//ON_EVENT(CDlgFuncWindow, IDC_EXPLORER1, DISPID_NEWWINDOW2, NewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CDlgFuncWindow, IDC_EXPLORER1, 273, NewWindow3, VTS_PDISPATCH VTS_PBOOL VTS_I4 VTS_BSTR VTS_BSTR)
	ON_EVENT(CDlgFuncWindow,IDC_EXPLORER1,259,OnDocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//ON_EVENT(CDlgFuncWindow,IDC_EXPLORER1,113,OnTitleChange,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


// CDlgFuncWindow message handlers



inline void GetHtmlString(const std::string & sTitle, const std::string& sText, std::string& pOutHtmlString)
{
	pOutHtmlString.clear();
	std::string sTextTemp(sText);
	std::transform(sTextTemp.begin(), sTextTemp.end(), sTextTemp.begin(), tolower);
	if (sTextTemp.find("<head")==std::string::npos)
	{
		pOutHtmlString = _T("<HEAD>\r\n")\
			_T("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\r\n")\
			_T("<style> \r\n")\
			_T("<!-- \r\n")\
			_T(".popups { \r\n")\
			_T("	BORDER-RIGHT: black 1px solid; PADDING-RIGHT: 5px; BORDER-TOP: black 1px solid; DISPLAY: none; PADDING-LEFT: 5px; BACKGROUND: #eee; PADDING-BOTTOM: 5px; BORDER-LEFT: black 1px solid; WIDTH: 110px; PADDING-TOP: 5px; BORDER-BOTTOM: black 1px solid; POSITION: absolute; HEIGHT: 90px \r\n")\
			_T("} \r\n")\
			_T("input, td, body, p { \r\n")\
			_T("FONT-SIZE: 14px; \r\n")\
			_T("line-height:20px;font-family: \"Arial\", \"Verdana\", \"Helvetica\", \"sans-serif\";\r\n")\
			_T("} \r\n")\
			_T("Body{ \r\n")\
			_T("Scrollbar-Face-Color:#F4F7FA; \r\n");
		pOutHtmlString += _T("BORDER-LEFT-STYLE: none;  \r\n")\
			_T("BORDER-RIGHT-STYLE: none; \r\n")\
			_T("BORDER-TOP-STYLE: none;  \r\n")\
			_T("BORDER-BOTTOM-STYLE: none;  \r\n");
		pOutHtmlString += _T("overflow:hidden;");
		pOutHtmlString += _T("} \r\n") \
			_T("a:link { \r\n")\
			_T("text-decoration: none; \r\n")\
			_T("color: #0066CC; \r\n")\
			_T("} \r\n")\
			_T("a:visited { \r\n")\
			_T("text-decoration: none;\r\n")\
			_T("color: #0066CC;\r\n")\
			_T("}\r\n")\
			_T("-->\r\n")\
			_T("</style> \r\n") \
			//_T("\r\n</HEAD>\r\n<body bgcolor=\"white\">");
			_T("\r\n<title>");
		pOutHtmlString += sTitle;
		pOutHtmlString += "</title></HEAD>\r\n";
	}
	if (sTextTemp.find("<body")==std::string::npos)
	{
		pOutHtmlString += "<body bgcolor=\"white\">";
		pOutHtmlString += sText;
		pOutHtmlString += "</body>";
	}else
	{
		pOutHtmlString += sText;
	}
}
void CDlgFuncWindow::UpdateBroadcastMsg(const std::string& sTitle, const std::string& sText, eb::bigint nMsgId)
{
	// *用户不点击，不处理
	//if (m_nBroadcastMsgId>0 && m_nBroadcastMsgId!=nMsgId)
	//{
	//	theEBAppClient.EB_AckMsg(m_nBroadcastMsgId,1);
	//}
	m_nBroadcastMsgId = nMsgId;

	m_sTitle = sTitle;
	m_sFuncUrl = sText;
#ifdef USES_CEF_BROADCAST
	if (m_bDocumentComplete)
	{
		std::string sHtmlString;
		GetHtmlString(libEbc::ACP2UTF8(sTitle.c_str()),m_sFuncUrl, sHtmlString);
		m_pCefBrowser.LoadString(sHtmlString,"http://entboost/broadcast");

		//std::string sHtmlString("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><title>");
		//sHtmlString.append(libEbc::ACP2UTF8(m_sTitle.c_str()).string());
		//sHtmlString.append("</title></HEAD><body bgcolor=\"white\">");
		//sHtmlString.append(m_sFuncUrl.string());
		//sHtmlString.append("</body></HTML>");
		//m_pCefBrowser.LoadString(sHtmlString,"http://www.entboost.com");
	}
#else
	m_pBroadcastView->ClearBlock();
	m_pBroadcastView->WriteBlock(m_sFuncUrl.c_str(),TRUE);
	m_pBroadcastView->SetBrowserFocus();
#endif
	this->Invalidate();
	this->FlashWindow(TRUE);
}
void CDlgFuncWindow::Navigate(const std::string& sUrl)
{
	if (!m_bDocumentComplete)
		m_sFuncUrl = sUrl;
	else
	{
#ifdef USES_LIBCEF
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		{
			m_pCefBrowser.LoadURL(libEbc::ACP2UTF8(sUrl.c_str()).string());
			//m_pCefBrowser.LoadURL(sUrl);
			return;
		}
#endif
		if (m_pExplorer.GetSafeHwnd()!=NULL)
			m_pExplorer.Navigate(sUrl.c_str(),NULL,NULL,NULL,NULL);
	}
}
BOOL CDlgFuncWindow::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	if (!m_bBroadcastMsg)
		ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);

	m_pPanelFind = new CPanelFind(this);
	m_pPanelFind->Create(CPanelFind::IDD,this);
	m_pPanelFind->SetFindHandler(this);
	//m_pPanelFind->SetCtrlColor(bInvalidate);
	m_pPanelStatus = new CPanelText(this);
	m_pPanelStatus->Create(CPanelText::IDD,this);
	m_pPanelStatus->ShowWindow(SW_HIDE);
	m_pPanelStatus->SetBgColor(theDefaultFlatLineColor);

	//theApp.AdaptIeVersion();
	if (m_nFuncBrowserType==EB_FUNC_BROWSER_TYPE_DEFAULT)
		m_nBrowserType = theApp.GetDefaultBrowserType();
	else if (m_nFuncBrowserType==EB_FUNC_BROWSER_TYPE_IE && !theApp.GetIeException())
		m_nBrowserType = EB_BROWSER_TYPE_IE;
	else
		m_nBrowserType = EB_BROWSER_TYPE_CEF;
#ifdef USES_LIBCEF
	//// for test
	//m_sFuncUrl = "www.entboost.com";
	//m_bDisableContextMenu = false;
#ifdef USES_CEF_BROADCAST
	if (m_bBroadcastMsg)
		m_nBrowserType = EB_BROWSER_TYPE_CEF;
#else
	if (!m_bBroadcastMsg)
#endif
	{
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		{
			CRect rect;
			this->GetClientRect(&rect);
			const int const_border_top = 0;
			const int const_border_bottom = 25;
			rect.left = 1;
			rect.right -= 1;
			rect.top = WINDOW_TITLE_HEIGHT+const_border_top;
			rect.bottom -= 1;
			if (m_bBroadcastMsg || m_sFuncUrl.empty() || !m_sPostData.empty())
				m_pCefBrowser.Create("about:blank",rect,this->GetSafeHwnd(),0);
			else// if (m_sPostData.empty())
				m_pCefBrowser.Create(libEbc::ACP2UTF8(m_sFuncUrl.c_str()).string(),rect,this->GetSafeHwnd(),0);
			m_pExplorer.MoveWindow(0,0,0,0);
		}else
		{
			CRect rect(0,0,0,0);
			m_pCefBrowser.Create("about:blank",rect,this->GetSafeHwnd(),0);
		}
	}
#endif

	this->GetDlgItem(IDC_BUTTON_NULL)->MoveWindow(0,0,0,0);
	if (!m_bWinResizable)
		m_btnMax.ShowWindow(SW_HIDE);
	if (m_bBroadcastMsg)
	{
		m_btnMin.ShowWindow(SW_HIDE);
		m_btnMax.ShowWindow(SW_HIDE);
	}else
	{
		m_btnMin.SetAutoSize(false);
		m_btnMin.SetDrawToolButtonPic(1,theDefaultBtnWhiteColor,-1,-1,-1,2);
		m_btnMin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
		m_btnMin.SetDrawPanelRgn(false);
		m_btnMin.SetToolTipText(_T("最小化"));
		m_btnMax.SetAutoSize(false);
		m_btnMax.SetDrawToolButtonPic(2,theDefaultBtnWhiteColor,-1,-1,-1,2);
		m_btnMax.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
		m_btnMax.SetDrawPanelRgn(false);
		m_btnMax.SetToolTipText(_T("最大化"));
	}
	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));


	const int m_nScreenWidth = theApp.GetScreenWidth();
	const int m_nScreenHeight = theApp.GetScreenHeight();
	CRect rect;
	if (m_bBroadcastMsg)
	{
		m_pExplorer.ShowWindow(SW_HIDE);
#ifdef USES_CEF_BROADCAST
		// *不需要处理
#else
		m_pBroadcastView = new CBroadcastView(this);
		m_pBroadcastView->m_strBgColor = _T("#ffffff");
		//m_pBroadcastView->m_strBgColor.Format(_T("#%02x%02x%02x"),GetRValue(theApp.GetBgColor2()), GetGValue(theApp.GetBgColor2()), GetBValue(theApp.GetBgColor2()));
		if ( !m_pBroadcastView->Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0x146, NULL))	// 176
		{
			delete m_pBroadcastView;
			m_pBroadcastView = NULL;
			if (theApp.GetIeException())
				CDlgMessageBox::EbMessageBox(this,"微软IE浏览器错误","请重新安装或升级IE浏览器！",CDlgMessageBox::IMAGE_WARNING,5);
			else
				AfxThrowResourceException();
			SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			return FALSE;
		}
		m_pBroadcastView->SetWindowPos(&m_btnClose,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
#endif
		SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		//m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);
		rect.right = m_nScreenWidth-2;
		rect.left = rect.right-m_nWidth;
		if (m_nMsgType==EBC_MSG_TYPE_BC_MSG)
		{
			rect.bottom = m_nScreenHeight-2-m_nHeight;
		}else if (m_nMsgType>0)
		{
			rect.bottom = m_nScreenHeight-2-m_nHeight*2;
		}else
		{
			rect.bottom = m_nScreenHeight-2;
		}
		rect.top = rect.bottom-m_nHeight;

#ifdef USES_CEF_BROADCAST
		// ** 放在 OnLoadingStateChange 处理
		//if (!m_sFuncUrl.empty())
		//	m_pCefBrowser.LoadString(m_sFuncUrl);
#else
		if (!m_sFuncUrl.empty())
			m_pBroadcastView->WriteBlock(m_sFuncUrl.c_str(),TRUE);
		m_pBroadcastView->SetBrowserFocus();
#endif
		//this->PostMessage(POP_WM_WINDOW_SIZE);
	}else
	{
		//m_pExplorer.ModifyStyle(WS_BORDER,0);
		m_pExplorer.ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
		// 先显示空白页，用于刷新修改IE控件。
		//m_bDisableContextMenu = FALSE;
		m_webUICtrl.EnableContextMenu(m_bDisableContextMenu?0:1);
		m_webUICtrl.EnableScrollBar(m_bDisableScrollBar?0:1);
		m_webUICtrl.SetWebBrowser(&m_pExplorer);
		try
		{
			m_pExplorer.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);
		}catch(...)
		{}

		//if (m_sPostData.empty())
		//	m_pExplorer.Navigate(m_sFuncUrl.c_str(),NULL,NULL,NULL,NULL);
		//else
		//{
		//	CComVariant pHeaders(L"Content-Type: application/x-www-form-urlencoded\r\n");
		//	VARIANT pPostData={0};
		//	tstring sPostData("eb_post_data=");
		//	sPostData.append(m_sPostData);
		//	GetPostData(sPostData.c_str(),&pPostData);
		//	m_pExplorer.Navigate(m_sFuncUrl.c_str(),NULL,NULL,&pPostData,&pHeaders);
		//	VariantClear(&pPostData);
		//}

		if (m_bDeleteThis)
		{
			//m_nScreenHeight += GetSystemMetrics(SM_CYCAPTION);
			if (m_bWinResizable)
				this->SetSplitterBorder();
			if (m_nWidth>0)
				rect.left = (m_nScreenWidth-m_nWidth)/2;
			else
				rect.left = 0;
			if (m_nHeight>0)
				rect.top = (m_nScreenHeight-m_nHeight)/2;
			else
				rect.top = 0;
		}else
		{
			this->GetWindowRect(&rect);
		}
		const int const_width = (m_nWidth>0?m_nWidth:m_nScreenWidth);
		const int const_height = (m_nHeight>0?m_nHeight:m_nScreenHeight);
		rect.left = (m_nScreenWidth-const_width)/2;
		rect.right = rect.left + const_width;
		rect.top = (m_nScreenHeight-const_height)/2;
		rect.bottom = rect.top + const_height;
		//rect.right = rect.left + (m_nWidth>0?m_nWidth:m_nScreenWidth);
		//rect.bottom = rect.top + (m_nHeight>0?m_nHeight:m_nScreenHeight);
	}
	this->FlashWindow(TRUE);
	this->SetWindowText(m_sTitle.c_str());
	this->MoveWindow(&rect);
	//this->SetForegroundWindow();
	this->ShowWindow(SW_SHOW);
#ifdef USES_CEF_BROADCAST
	// *
#else
	if (m_bBroadcastMsg && m_pBroadcastView!=NULL)
	{
		// **解决部分环境下，第一次弹开界面，背景不能正常刷新；
		m_pBroadcastView->Invalidate();
		//this->PostMessage(POP_WM_WINDOW_SIZE);
	}
#endif
	//SetCircle();

	if (m_pEbCallInfo.get()!=NULL)
	{
		CDlgFuncWindow * pOldFuncWindow = NULL;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			theApp.m_pTempGroupMsg.find(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pOldFuncWindow,true);
		else
			theApp.m_pTempFromUserMsg.find(m_pEbCallInfo->m_pFromAccountInfo.GetUserId(),pOldFuncWindow,true);
		// **必须放在前面
		if (pOldFuncWindow!=NULL)
		{
			pOldFuncWindow->ShowWindow(SW_HIDE);		// *必须隐藏不让用户点击；
			pOldFuncWindow->m_pEbCallInfo.reset();	// *必须设为空，否则会清掉后面窗口
			theApp.m_pCloseWnd.add(pOldFuncWindow);
		}

		CDlgFuncWindow * pNewFuncWindow = this;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			theApp.m_pTempGroupMsg.insert(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pNewFuncWindow);
		else
			theApp.m_pTempFromUserMsg.insert(m_pEbCallInfo->m_pFromAccountInfo.GetUserId(),pNewFuncWindow);
	}
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFuncWindow::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	btnx -= const_minbtn_width;
	if (m_bWinResizable)
	{
		if (m_btnMax.GetSafeHwnd()!=NULL)
			m_btnMax.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
		btnx -= const_minbtn_width;
	}
	if (m_btnMin.GetSafeHwnd()!=NULL)
		m_btnMin.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

#ifdef USES_CEF_BROADCAST
	if (false)	// ** 由下面 m_nBrowserType==EB_BROWSER_TYPE_CEF 处理
	{
		//const int const_border_height = 6;
		//m_pBroadcastView->MoveWindow(const_border_height,WINDOW_TITLE_HEIGHT+const_border_height,cx-const_border_height*2,cy-WINDOW_TITLE_HEIGHT-const_border_height*2);
#else
	if (m_bBroadcastMsg && m_pBroadcastView != NULL && m_pBroadcastView->GetSafeHwnd()!=NULL)
	{
		const int const_border_height = 6;
		m_pBroadcastView->MoveWindow(const_border_height,WINDOW_TITLE_HEIGHT+const_border_height,cx-const_border_height*2,cy-WINDOW_TITLE_HEIGHT-const_border_height*2);
#endif
#ifdef USES_LIBCEF
	}else if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd())
	{
		const int const_border_top = 0;
		const int const_border_bottom = 2;
		m_pCefBrowser.MoveWindow(1,WINDOW_TITLE_HEIGHT+const_border_top,cx-2,cy-WINDOW_TITLE_HEIGHT-(const_border_top+const_border_bottom));
		SetTimer(TIMER_REFRESH_BROWSER,10,NULL);
#endif
	}else if (m_nBrowserType==EB_BROWSER_TYPE_IE && m_pExplorer.GetSafeHwnd())
	{
		const int const_border_top = 0;
		const int const_border_bottom = 2;
		m_pExplorer.MoveWindow(1,WINDOW_TITLE_HEIGHT+const_border_top,cx-2,cy-WINDOW_TITLE_HEIGHT-(const_border_top+const_border_bottom));
	}

	MovePanelFind(cx);
	if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
	{
		SetTimer(TIMER_SHOW_STATUS_MESSAGE,100,NULL);
	}
}

void CDlgFuncWindow::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_bClosePrompt &&				// 配置关闭提示
		!m_bBroadcastMsg &&				// 广播信息不提示
		CDlgMessageBox::EbDoModal(this,_T("关闭窗口"),_T("关闭确认：\r\n确定关闭当前应用窗口吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
	{
		return;
	}
	const CEBCCallInfo::pointer pEbCallInfo = m_pEbCallInfo;
	m_pEbCallInfo.reset();
	if (pEbCallInfo.get()!=NULL)
	{
		if (pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
		{
			if (!theApp.m_pTempGroupMsg.remove(pEbCallInfo->m_pCallInfo.m_sGroupCode))
				return;
		}else
		{
			if (!theApp.m_pTempFromUserMsg.remove(pEbCallInfo->m_pFromAccountInfo.GetUserId()))
				return;
		}
	}

	if (m_bDeleteThis)
	{
		if (this->IsWindowVisible())	// **窗口隐藏，说明已经另外有线程关闭；
		{
			this->ShowWindow(SW_HIDE);
			this->DestroyWindow();
		}
	}else
		this->EndDialog(IDOK);
	//CEbDialogBase::OnClose();	// 不能退出窗口
}

void CDlgFuncWindow::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgFuncWindow::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgFuncWindow::OnPaint()
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
		this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultBtnWhiteColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
		//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, WINDOW_TITLE_HEIGHT, 2);

		//if (theApp.GetColorGradient())
		//	this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,theDefaultBgColorTitleLight1,false));
		//else
		//	this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		//this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);

		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleRegular, UnitPixel);
		//const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		Gdiplus::PointF pointTitle(5,3);	// 7,7
		USES_CONVERSION;
		graphics.DrawString(A2W_ACP(m_sTitle.c_str()),-1,&fontEbTitle,pointTitle,&brushEbTitle);
		//if (m_pExplorer.GetSafeHwnd())
		//{
		//	//m_pExplorer.Invalidate();
		//	m_pExplorer.ShowWindow(SW_HIDE);
		//	m_pExplorer.ShowWindow(SW_SHOW);
		//}
		//if (!sDescription.empty())
		//{
		//	pointTitle.Y += 20;
		//	graphics.DrawString(A2W_ACP(sDescription.c_str()),-1,&fontEbDescription,pointTitle,&brushEbTitle);
		//}
	}
}

void CDlgFuncWindow::OnDestroy()
{
#ifdef USES_LIBCEF
	m_pCefBrowser.Destroy();
#endif
	if (m_pPanelFind!=NULL)
	{
		m_pPanelFind->DestroyWindow();
		delete m_pPanelFind;
		m_pPanelFind = NULL;
	}
	if (m_pPanelStatus!=NULL)
	{
		m_pPanelStatus->DestroyWindow();
		delete m_pPanelStatus;
		m_pPanelStatus = NULL;
	}

	try
	{
		//if (m_pEbCallInfo.get()!=NULL)
		//{
		//	if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
		//	{
		//		if (!theApp.m_pTempGroupMsg.remove(m_pEbCallInfo->m_pCallInfo.m_sGroupCode))
		//			return;
		//	}else
		//	{
		//		if (!theApp.m_pTempFromUserMsg.remove(m_pEbCallInfo->m_pFromAccountInfo.GetUserId()))
		//			return;
		//	}
		//	m_pEbCallInfo.reset();
		//}
		CEbDialogBase::OnDestroy();
		m_webUICtrl.ClearWebBrowser();
#ifdef USES_CEF_BROADCAST
		// *不需要处理
#else
		if (m_pBroadcastView!=NULL)
		{
			m_pBroadcastView->DestroyWindow();
			delete m_pBroadcastView;
			m_pBroadcastView = NULL;
		}
#endif
		if (m_bDeleteThis)
			delete this;
	}catch(std::exception&)
	{
	}catch(...)
	{}
}

void CDlgFuncWindow::OnBnClickedButtonMin()
{
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CDlgFuncWindow::OnBnClickedButtonMax()
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
		if (theRestoreRect.Width()>0 && theRestoreRect.Height()>0)	// **有可能本身窗口已经是最大化，所以不需要动作
		{
			MoveWindow(&theRestoreRect);
		}
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
	if (!m_bBroadcastMsg)
		SetTimer(TIMER_REFRESH_BROWSER,10,NULL);
	//PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}

void CDlgFuncWindow::DoClose(bool bTimerToClose)
{
	if (m_nBroadcastMsgId>0)
	{
		theEBAppClient.EB_AckMsg(m_nBroadcastMsgId,1);
		m_nBroadcastMsgId = 0;
	}
	if (bTimerToClose)
	{
		SetTimer(TIMER_TO_CLOSE,50,NULL);
	}else
	{
		this->PostMessage(WM_CLOSE, 0, 0);
	}
}
void CDlgFuncWindow::OnBnClickedButtonClose()
{
	DoClose(false);
}

void CDlgFuncWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_bWinResizable)
		OnBnClickedButtonMax();
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}

void CDlgFuncWindow::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_TO_CLOSE:
		{
			KillTimer(TIMER_TO_CLOSE);
			this->PostMessage(WM_CLOSE, 0, 0);
			return;
		}break;
	case TIMER_HIDE_STATUS_MESSAGE:
		{
			KillTimer(nIDEvent);
			if (m_pPanelStatus->IsWindowVisible())
				m_pPanelStatus->ShowWindow(SW_HIDE);
		}break;
	case TIMER_SHOW_STATUS_MESSAGE:
		{
			KillTimer(nIDEvent);
			if (m_sStatusMessage.empty())
			{
				if (m_pPanelStatus->IsWindowVisible())
					m_pPanelStatus->ShowWindow(SW_HIDE);
			}else
			{
				CRect rect;
				this->GetClientRect(&rect);
				const int nStringWidth = m_pPanelStatus->MeasureTextWidth(m_sStatusMessage.c_str(),rect.Width());
				const int const_status_height = 22;
				CRect rectStatus(0,rect.Height()-const_status_height,nStringWidth+2,rect.Height());
				CPoint pos;
				::GetCursorPos(&pos);
				this->ScreenToClient(&pos);
				if (rectStatus.PtInRect(pos))
				{
					rectStatus.bottom = pos.y-12;
					rectStatus.top = rectStatus.bottom-const_status_height;
				}
				m_pPanelStatus->MoveWindow(&rectStatus);
				//m_pPanelStatus->MoveWindow(0,rect.Height()-const_status_height,nStringWidth+2,const_status_height);
				m_pPanelStatus->SetDrawText(m_sStatusMessage.c_str());
			}
		}break;
	case TIMER_SET_TITLE:
		{
			KillTimer(TIMER_SET_TITLE);
			if (!m_webUICtrl.GetTitle().IsEmpty())
			{
				this->m_sTitle = (LPCTSTR)m_webUICtrl.GetTitle();
				this->SetWindowText(m_sTitle.c_str());

				CRect rectClient;
				this->GetClientRect(&rectClient);
				// ** 可以刷新
				rectClient.bottom = rectClient.top + WINDOW_TITLE_HEIGHT;
				this->InvalidateRect(&rectClient);
			}
		}break;
	case TIMER_REFRESH_BROWSER:
		{
			KillTimer(TIMER_REFRESH_BROWSER);
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF)

				if (!m_bBroadcastMsg && m_nBrowserType==EB_BROWSER_TYPE_IE && m_pExplorer.GetSafeHwnd())
				{
					const long nReadyState = m_pExplorer.get_ReadyState();
					if (nReadyState == READYSTATE_COMPLETE)
					{
						m_pExplorer.Refresh();
					}
#ifdef USES_LIBCEF
				}else if (!m_bBroadcastMsg && m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd())
				{
					HWND hWnd = m_pCefBrowser.GetSafeHwnd();
					if (hWnd!=NULL)
					{
						::ShowWindow(hWnd,SW_HIDE);
						::ShowWindow(hWnd,SW_SHOW);
					}
#endif
				}
		}break;
	default:
		break;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

static CString theReqAddContact = _T("eb-add-contact://");
static CString theCallAccount = _T("ebim-call-account://");
static CString theCallGroup = _T("ebim-call-group://");
static CString theOpenSubId = _T("eb-open-subid://");
//static CString theOpenUrl = _T("eb-open-url://");
static CString theOpenFile = _T("eb-open-file://");
static CString theEBWindow2Url = _T("eb-open2://");
static CString theEBWindowUrl = _T("eb-open://");
static CString theClose = _T("eb-close://");
static CString theExitApp = _T("eb-exit-app://");
static CString theLogout = _T("eb-logout://");
static CString theShowMainFrame = _T("eb-show-mainframe://");
static CString theHideMainFrame = _T("eb-hide-mainframe://");
static CString theOpenFileManager = _T("eb-open-filemanager://");
static CString theMailTo = _T("mailto:");
void CDlgFuncWindow::OnBeforeNavigate(const wchar_t* szURL, bool* pOutCancel)
{
	if (m_nBroadcastMsgId>0)
	{
		theEBAppClient.EB_AckMsg(m_nBroadcastMsgId,1);
		m_nBroadcastMsgId = 0;
	}

	CString csURL(szURL);
	if (csURL.Right(1)==_T("/"))
		csURL = csURL.Left(csURL.GetLength()-1);
	//csURL.TrimRight('/');
	//AfxMessageBox(csURL);
	int pos = csURL.Find(theCallAccount, 0); 
	if( pos != -1 )
	{
		// ebim-call-account://[to_account]
		*pOutCancel = true;	
		const CString sToAccount = csURL.Mid(pos+theCallAccount.GetLength());
#ifdef USES_EBCOM_TEST
		const bool bRet = theEBClientCore->EB_CallAccount((LPCTSTR)sToAccount,0)==S_OK?true:false;
#else
		const bool bRet = theEBAppClient.EB_CallAccount(sToAccount)==0?true:false;
#endif
		if (bRet && m_bOpenNewClose)
			DoClose(true);
		return;
	}
	pos = csURL.Find(theCallGroup, 0); 
	if( pos != -1 )
	{
		// ebim-call-group://[group_id]
		*pOutCancel = true;	
		const CString sToGroupId = csURL.Mid(pos+theCallGroup.GetLength());
		const eb::bigint nToGroupId = eb_atoi64(sToGroupId);
#ifdef USES_EBCOM_TEST
		const bool bRet = theEBClientCore->EB_CallGroup(nToGroupId)==S_OK?true:false;
#else
		const bool bRet = theEBAppClient.EB_CallGroup(nToGroupId)==0?true:false;
#endif
		if (bRet && m_bOpenNewClose)
			DoClose(true);
		return;
	}
	if (csURL.Find(theMailTo)==0)
	{
		// mailto:
		// mailto:support@entboost.com
		// mailto:support@entboost.com?subject=Hello&Body=This is a test
		*pOutCancel = true;	
		ShellExecute(NULL, "open", csURL, NULL, NULL, SW_SHOWNORMAL);
		return;
	}
	pos = csURL.Find(theOpenSubId, 0); 
	if( pos != -1 )
	{
		// eb-open-subid://[sub_id][,xxx]
		// eb-open-subid://[sub_id],0[,xxx]	// 当前窗口打开
		// eb-open-subid://[sub_id],1[,xxx]	// 新窗口打开
		*pOutCancel = true;	
		const CString sSubId = csURL.Mid(pos+theOpenSubId.GetLength());
		const eb::bigint nSubId = eb_atoi64(sSubId);
		if (nSubId>0)
		{
			bool bOpenNewWindows = true;
			CString sParameters;
			pos = sSubId.Find(",",1);
			if( pos != -1 )
			{
				sParameters = sSubId.Mid(pos+1);
				const CString sOpenFlag = sParameters.IsEmpty()?"":sParameters.Left(1);
				if (sOpenFlag==_T("0"))
				{
					bOpenNewWindows = false;
					sParameters = sParameters.Mid(1);
				}else if (sOpenFlag==_T("1"))
				{
					bOpenNewWindows = true;
					sParameters = sParameters.Mid(1);
				}
				if (!sParameters.IsEmpty() && sParameters.Left(1)==",")
				{
					sParameters = sParameters.Mid(1);
				}
			}

			if (!bOpenNewWindows)
			{
				const CString sUrl = theApp.GetSubscribeFuncUrl(nSubId,sParameters);
				if (!sUrl.IsEmpty())
				{
					Navigate((LPCTSTR)sUrl);
				}
				return;
			}
			//if( pos != -1 )
			//	sParameters = sSubId.Mid(pos+1);

			bool bRet = false;
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo = theEBClientCore->EB_GetSubscribeFuncInfo(nSubId);
			if (pSubscribeFuncInfo!=NULL)
			{
				//bRet = theApp.OpenSubscribeFuncWindow(EB_SubscribeFuncInfo(pSubscribeFuncInfo),"",(LPCTSTR)sParameters,NULL);
				bRet = true;
			}
#else
			EB_SubscribeFuncInfo pSubscribeFuncInfo;
			if (theEBAppClient.EB_GetSubscribeFuncInfo(nSubId,&pSubscribeFuncInfo))
			{
				//bRet = theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"",(LPCTSTR)sParameters,NULL);
				bRet = true;
			}
#endif
			if (bRet)
			{
				COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(nSubId, (LPCTSTR)sParameters);
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
				if (m_bOpenNewClose)
					DoClose(true);
			}
		}
		return;
	}
	//pos = csURL.Find(theOpenUrl, 0); 
	//if( pos != -1 )
	//{
	//	// eb-open-url://[url-name]
	//	*bCancel = VARIANT_TRUE;	
	//	const CString sUrl = csURL.Mid(pos+theOpenUrl.GetLength());
	//	//if (::PathFileExists(sFileName))
	//	{
	//		ShellExecute(NULL, "open", sUrl, NULL, NULL, SW_SHOW);
	//		if (m_bOpenNewClose)
	//			DoClose(true);
	//	}
	//	return;
	//}
	pos = csURL.Find(theOpenFile, 0); 
	if( pos != -1 )
	{
		// eb-open-file://[file-name]
		// eb-open-file://notepad.exe
		*pOutCancel = true;	
		const CString sFileName = csURL.Mid(pos+theOpenFile.GetLength());
		//if (::PathFileExists(sFileName))
		{
			ShellExecute(NULL, "open", sFileName, NULL, NULL, SW_SHOW);
			if (m_bOpenNewClose)
				DoClose(true);
		}
		return;
	}
	pos = csURL.Find(theClose, 0);
	if( pos != -1 )
	{
		*pOutCancel = true;	
		DoClose(true);
		return;
	}
	pos = csURL.Find(theExitApp, 0);
	if( pos != -1 )
	{
		*pOutCancel = true;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_EXIT_APP);
		return;
	}
	pos = csURL.Find(theLogout, 0);
	if( pos != -1 )
	{
		*pOutCancel = true;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_LOGOUT);
		return;
	}
	pos = csURL.Find(theShowMainFrame, 0);
	if( pos != -1 )
	{
		*pOutCancel = true;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_SHOWHIDE_MAIN,1);
		return;
	}
	pos = csURL.Find(theHideMainFrame, 0);
	if( pos != -1 )
	{
		*pOutCancel = true;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_SHOWHIDE_MAIN,0);
		return;
	}
	pos = csURL.Find(theOpenFileManager, 0);
	if( pos != -1 )
	{
		*pOutCancel = true;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_FILE_MANAGER, 0, 0);
		return;
	}
	pos = csURL.Find(theReqAddContact, 0); 
	if( pos != -1 )
	{
		*pOutCancel = true;	
		// eb-add-contact://contact_userid,description
		pos += theReqAddContact.GetLength();
		const CString sString(csURL.Mid(pos));
		eb::bigint nContactUserId = 0;
		char lpszDescription[101];
		memset(lpszDescription,0,sizeof(lpszDescription));
		const int nRet = ::sscanf(sString,"%lld,%100s",&nContactUserId,lpszDescription);
		if (nRet>=1)
		{
#ifdef USES_EBCOM_TEST
			USES_CONVERSION;
			theEBClientCore->EB_ReqAddContact(nContactUserId,A2W_ACP(lpszDescription));
#else
			theEBAppClient.EB_ReqAddContact(nContactUserId,lpszDescription);
#endif
		}
		return;
	}
	pos = csURL.Find(theEBWindow2Url, 0); 
	if( pos != -1 )
	{
		*pOutCancel = true;	
		// eb-open2://width,height,disablecontentmenu,disablescrollbar,closeprompt,opennewclose,winresizable,url
		pos += theEBWindow2Url.GetLength();
		const CString sString(csURL.Mid(pos));
		int nWidth = 0;
		int nHeight = 0;
		int nDisableContextMenu = 1;
		int nDisableScrollBar = 0;
		int nClosePrompt = 1;
		int nOpenNewClose = 0;
		int bWinResizable = 0;
		char lpszUrl[513];
		memset(lpszUrl,0,sizeof(lpszUrl));
		const int nRet = ::sscanf(sString,"%d,%d,%d,%d,%d,%d,%d,%512s",
			&nWidth,&nHeight,&nDisableContextMenu,&nDisableScrollBar,&nClosePrompt,&nOpenNewClose,&bWinResizable,lpszUrl);
		if (nRet!=8)
		{
			return;
		}
		CString sWindowUrl(lpszUrl);
		if (lpszUrl[0] == '/')
		{
			const CString sLocationURL(szURL);
			const int nFind = sLocationURL.Find('/',10);	// *10至少是http://abc后面
			if (nFind>0)
			{
				sWindowUrl = sLocationURL.Left(nFind);
			}else
			{
				sWindowUrl = sLocationURL;
			}
			sWindowUrl.Append(lpszUrl);
		}
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		//pFuncWindow->m_sTitle = lpszTitle;
		pFuncWindow->m_nFuncBrowserType = m_nFuncBrowserType;
		pFuncWindow->m_sFuncUrl = (LPCTSTR)sWindowUrl;
		pFuncWindow->m_nWidth = nWidth;
		pFuncWindow->m_nHeight = nHeight;
		pFuncWindow->m_bDisableContextMenu = nDisableContextMenu==1?true:false;
		pFuncWindow->m_bDisableScrollBar = nDisableScrollBar==1?true:false;
		pFuncWindow->m_bClosePrompt = nClosePrompt==1?true:false;
		pFuncWindow->m_bOpenNewClose = nOpenNewClose==1?true:false;
		pFuncWindow->m_bWinResizable = bWinResizable==1?true:false;
		pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
		//pFuncWindow->ShowWindow(SW_SHOW);
		if (m_bOpenNewClose)
			DoClose(true);
		return;
	}
	pos = csURL.Find(theEBWindowUrl, 0); 
	if( pos != -1 )
	{
		*pOutCancel = true;	
		// eb-open://width,height,url
		pos += theEBWindowUrl.GetLength();
		int nPo2 = csURL.Find(_T(","), pos);
		if ( nPo2 == -1 )
		{
			return;
		}
		const int nWidth = atoi(csURL.Mid(pos,nPo2-pos));
		pos = nPo2+1;
		nPo2 = csURL.Find(_T(","), pos);
		if ( nPo2 == -1 )
		{
			return;
		}
		const int nHeight = atoi(csURL.Mid(pos,nPo2-pos));
		CString sWindowUrl = csURL.Mid(nPo2+1);
		if (sWindowUrl.Left(1) == _T("/"))
		{
			const CString sWindowUrlTemp(sWindowUrl);
			const CString sLocationURL(szURL);
			const int nFind = sLocationURL.Find('/',10);	// *10至少是http://abc后面
			if (nFind>0)
			{
				sWindowUrl = sLocationURL.Left(nFind);
			}else
			{
				sWindowUrl = sLocationURL;
			}
			sWindowUrl.Append(sWindowUrlTemp);
		}
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_nFuncBrowserType = m_nFuncBrowserType;
		pFuncWindow->m_sFuncUrl = sWindowUrl;
		pFuncWindow->m_nWidth = nWidth;
		pFuncWindow->m_nHeight = nHeight;
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bDisableScrollBar = true;
		pFuncWindow->m_bClosePrompt = false;
		pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
		//pFuncWindow->ShowWindow(SW_SHOW);
		if (m_bOpenNewClose)
			DoClose(true);
		return;
	}

	// **这里会导致，同时打开二个窗口；
	//csURL.MakeLower();
	//if (csURL.Find(_T("http://"))>=0 ||
	//	csURL.Find(_T("https://"))>=0 ||
	//	csURL.Find(_T("www."))>=0)
	//{
	//	*bCancel = VARIANT_TRUE;	
	//	ShellExecute(NULL,  "open", csURL, NULL, NULL, SW_SHOW);
	//}
}
void CDlgFuncWindow::OnBeforeNavigate(VARIANT_BOOL* Cancel,LPCTSTR szURL)
{
	USES_CONVERSION;
	bool bCancel = false;
	OnBeforeNavigate(T2W(szURL),&bCancel);
	if (bCancel)
		*Cancel = VARIANT_TRUE;
}
void CDlgFuncWindow::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, VARIANT_BOOL FAR* Cancel) 
{
	bool bCancel = false;
	OnBeforeNavigate(URL->bstrVal,&bCancel);
	if (bCancel)
		*Cancel = VARIANT_TRUE;
}
//void CDlgFuncWindow::NewWindow2(IDispatch **ppDisp,
//						  VARIANT_BOOL *Cancel)
//{
//	*Cancel = VARIANT_FALSE;
//}
void CDlgFuncWindow::NewWindow3(IDispatch **ppDisp,
						  VARIANT_BOOL *Cancel,
						  DWORD dwFlags,
						  BSTR bstrUrlContext,
						  BSTR bstrUrl
						  )
{
	//CString csURL(bstrUrl);
	//csURL.MakeLower();
	//if (csURL.Find(_T("http://"))>=0 ||
	//	csURL.Find(_T("https://"))>=0 ||
	//	csURL.Find(_T("www."))>=0)
	//{
	//	*Cancel = VARIANT_TRUE;	
	//	ShellExecute(NULL,  "open", csURL, NULL, NULL, SW_SHOW);
	//}
	*Cancel = VARIANT_FALSE;
}

//void CDlgFuncWindow::OnTitleChange(LPDISPATCH pDisp, VARIANT* sTitle)
//{
//	const CString sTitleString(sTitle->bstrVal);
//	this->SetWindowText(sTitleString);
//}
void CDlgFuncWindow::OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_IE) return;

	if (!m_bDocumentComplete && !m_sFuncUrl.empty())
	{
		//AfxMessageBox(m_sFuncUrl.c_str());
		m_bDocumentComplete = true;
		CString csURL(URL->bstrVal);
		csURL.MakeLower();
		//AfxMessageBox(csURL);
		if (csURL == _T("about:blank"))
		{
			if (m_sPostData.empty())
				m_pExplorer.Navigate(m_sFuncUrl.c_str(),NULL,NULL,NULL,NULL);
			else
			{
				//AfxMessageBox(m_sPostData.c_str());
				if (m_sPostData.size()>2048)
					m_sPostData = m_sPostData.substr(0,2048);
				CComVariant pHeaders(L"Content-Type: application/x-www-form-urlencoded\r\n");
				VARIANT pPostData={0};
				tstring sPostData("eb_post_data=");
				sPostData.append(m_sPostData);
				GetPostData(sPostData.c_str(),&pPostData);
				m_pExplorer.Navigate(m_sFuncUrl.c_str(),NULL,NULL,&pPostData,&pHeaders);
				VariantClear(&pPostData);
			}
		}
	}else
	{
		SetTimer(TIMER_SET_TITLE,100,NULL);
	}
}

#ifdef USES_LIBCEF
void CDlgFuncWindow::MovePanelFind(int cx)
{
	if (m_pPanelFind != NULL && m_pPanelFind->GetSafeHwnd()!=NULL)
	{
		int x = 0;
		if (m_pPanelFind->GetUserData()>0)
		{
			x = LOWORD(m_pPanelFind->GetUserData());
			const int w = HIWORD(m_pPanelFind->GetUserData());
			if (cx-x-w>DEFAULT_PANEL_FIND_WIDTH+DEFAULT_PANEL_FIND_RIGHT)
			{
				m_pPanelFind->SetUserData(0);
			}
		}
		CRect rect;
		if (m_pPanelFind->GetUserData()==0)
		{
			rect.right = cx-DEFAULT_PANEL_FIND_RIGHT;
			rect.left = rect.right - DEFAULT_PANEL_FIND_WIDTH;
		}else
		{
			rect.right = x-5;
			rect.left = rect.right - DEFAULT_PANEL_FIND_WIDTH;
		}
		rect.top = WINDOW_TITLE_HEIGHT;
		rect.bottom = rect.top + DEFAULT_PANEL_FIND_HEIGHT;
		m_pPanelFind->MoveWindow(&rect);
	}
}
bool CDlgFuncWindow::OnJSDialog(const wchar_t* sOriginUrl, const wchar_t* sAcceptLang, cef_jsdialog_type_t dialog_type, const wchar_t* sMessageText,const wchar_t* sDefaultPromptText,bool& pOutSuccess,std::string& pOutUserInput)
{
	switch (dialog_type)
	{
	case JSDIALOGTYPE_ALERT:
		{
			pOutSuccess = true;
			USES_CONVERSION;
			const CString sUrl = sOriginUrl==NULL?"":W2A(sOriginUrl);
			//CString sTitle;
			//sTitle.Format(_T("脚本 - %s"),sUrl);
			CString sText = sMessageText==NULL?"":W2A(sMessageText);
			//CDlgMessageBox::EbDoModal(this,sTitle,sText,CDlgMessageBox::IMAGE_NULL,true,0);
			CDlgMessageBox::EbDoModal(this,"",sText,CDlgMessageBox::IMAGE_NULL,true,0);
			return true;
		}break;
	case JSDIALOGTYPE_CONFIRM:
		{
			pOutSuccess = true;
			USES_CONVERSION;
			const CString sUrl = sOriginUrl==NULL?"":W2A(sOriginUrl);
			//CString sTitle;
			//sTitle.Format(_T("脚本 - %s"),sUrl);
			CString sText = sMessageText==NULL?"":W2A(sMessageText);
			if (CDlgMessageBox::EbDoModal(this,_T("确认"),sText,CDlgMessageBox::IMAGE_QUESTION,false,0)!=IDOK)
			//if (CDlgMessageBox::EbDoModal(this,sTitle,sText,CDlgMessageBox::IMAGE_QUESTION,false,0)!=IDOK)
			{
				pOutSuccess = false;
			}
			return true;
		}break;
	case JSDIALOGTYPE_PROMPT:
		{
			pOutSuccess = true;
			USES_CONVERSION;
			//const CString sUrl = sOriginUrl==NULL?"":W2A(sOriginUrl);
			CDlgPrompt pDlgPrompt;
			pDlgPrompt.m_sTitle = _T("提示");
			//pDlgPrompt.m_sTitle.Format(_T("脚本 - %s"),sUrl);
			if (sMessageText!=NULL)
				pDlgPrompt.m_sPrompt = W2A(sMessageText);
			if (sDefaultPromptText!=NULL)
				pDlgPrompt.m_sUserInput = W2A(sDefaultPromptText);
			if (pDlgPrompt.DoModal()==IDOK)
			{
				pOutUserInput = libEbc::ACP2UTF8(pDlgPrompt.m_sUserInput).c_str();
			}else
			{
				pOutSuccess = false;
			}
			return true;
		}break;
	default:
		break;
	}

	return false;
}
void CDlgFuncWindow::OnFindResult(int identifier,int count,const CefRect& selectionRect,int activeMatchOrdinal,bool finalUpdate)
{
	if (m_pPanelFind!=NULL && m_pPanelFind->IsWindowVisible())
	{
		if (activeMatchOrdinal>0)
		{
			CPoint posSelection1(selectionRect.x+1,selectionRect.y+1);
			::ClientToScreen(m_pCefBrowser.GetSafeHwnd(),&posSelection1);
			CPoint posSelection2(posSelection1.x+selectionRect.width-1,posSelection1.y);
			CRect rectPanelFind;
			m_pPanelFind->GetWindowRect(&rectPanelFind);
			if (rectPanelFind.PtInRect(posSelection1) || rectPanelFind.PtInRect(posSelection2))
			{
				m_pPanelFind->SetUserData(MAKELONG(selectionRect.x,selectionRect.width));
				CRect rect;
				this->GetClientRect(&rect);
				MovePanelFind(rect.Width());
				m_pPanelFind->Invalidate();
			}
		}
		m_pPanelFind->Cef_OnFindResult(count,activeMatchOrdinal,finalUpdate);
	}
}
bool CDlgFuncWindow::OnExecuteMenu(int nCommandId, const wchar_t * lpszUrl)
{
	switch (nCommandId)
	{
	case CEF_CLIENT_MENU_COMMAND_ID_IMAGE_SAVEAS:
		{
			if (lpszUrl==NULL || wcslen(lpszUrl)==0) return false;

			USES_CONVERSION;
			const std::string sCopyImageUrl = W2A(lpszUrl);
			std::string::size_type find = sCopyImageUrl.rfind("/");
			if (find==std::string::npos) return false;
			std::string sFileName = sCopyImageUrl.substr(find+1);
			find = sFileName.rfind(".");
			if (find!=std::string::npos)
				sFileName = sFileName.substr(0,find);

			char szFileName[MAX_PATH*2] = {0} ; 
			sprintf(szFileName,"%s",sFileName.c_str());

			OPENFILENAME ofn={0};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hWnd;
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = sizeof(szFileName);
			ofn.lpstrFilter = "PNG Image (*.png)\0*.png\0JPEG Image (*.jpg)\0*.jpg\0BMP Image (*.bmp)\0*.bmp\0\0";
			ofn.lpstrDefExt = "png";
			ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
			if (GetSaveFileName(&ofn)==TRUE ) 
			{
				CString sTempFile;
				sTempFile.Format(_T("%s\\%lld_%s"),theApp.GetAppImgTempPath(),(mycp::bigint)time(0),sFileName.c_str());
				CString sOldLastModified;
				CString sNewLastModified;
				if (!DownloadHttpFile(sCopyImageUrl.c_str(),sTempFile,sOldLastModified,sNewLastModified))
				{
					CDlgMessageBox::EbDoModal(this,"","图片另存失败：\r\n请重试！",CDlgMessageBox::IMAGE_WARNING,true,5);
					return false;
				}

				std::string sExt(ofn.lpstrFile);
				sExt = sExt.substr(sExt.size()-3);
				std::transform(sExt.begin(), sExt.end(), sExt.begin(), tolower);
				CLSID                           codecClsid;
				if (sExt=="png")
					libEbc::GetCodecClsid(L"image/png",   &codecClsid);
				else if (sExt=="bmp")
					libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
				else
					libEbc::GetCodecClsid(L"image/jpeg",   &codecClsid);

				{
					USES_CONVERSION;
					Gdiplus::Image   image(A2W(sTempFile));
					image.Save(A2W(ofn.lpstrFile), &codecClsid);
				}
				DeleteFile(sTempFile);
				return true;
			}
		}break;
	case CEF_CLIENT_MENU_COMMAND_ID_COPY_IMAGE:
		{
			if (lpszUrl==NULL || wcslen(lpszUrl)==0) return false;

			USES_CONVERSION;
			const std::string sCopyImageUrl = W2A(lpszUrl);
			//const std::string::size_type find = sCopyImageUrl.rfind(".");
			//if (find==std::string::npos) return false;
			//const std::string sExt = sCopyImageUrl.substr(find+1);
			CString sTempFile;
			sTempFile.Format(_T("%s\\%lld.ebtemp"),theApp.GetAppImgTempPath(),(mycp::bigint)time(0));
			//sTempFile.Format(_T("%s\\%lld.%s"),theApp.GetAppImgTempPath(),(mycp::bigint)time(0),sExt.c_str());

			CString sOldLastModified;
			CString sNewLastModified;
			if (DownloadHttpFile(sCopyImageUrl.c_str(),sTempFile,sOldLastModified,sNewLastModified))
			{
				if ( OpenClipboard() )
				{
					Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromFile(A2W(sTempFile),FALSE);
					if (pImage!=NULL)
					{
						EmptyClipboard();
						// **下面支持复制 WORD QQ 等其他程序；
						PAINTSTRUCT ps;
						HDC hDC = ::BeginPaint(this->GetSafeHwnd(),&ps);
						HDC dcMem = ::CreateCompatibleDC(hDC);
						HBITMAP bitmap = ::CreateCompatibleBitmap(hDC,pImage->GetWidth(),pImage->GetHeight());
						SelectObject(dcMem,(HGDIOBJ)bitmap);
						Gdiplus::Graphics graphics(dcMem);
						graphics.DrawImage(pImage,0,0,pImage->GetWidth(),pImage->GetHeight());
						SetClipboardData(CF_BITMAP,bitmap);
						CloseClipboard();
						EndPaint(&ps);
						DeleteObject(bitmap);
						DeleteDC(dcMem);
						delete pImage;
					}else
					{
						CloseClipboard();
						CDlgMessageBox::EbDoModal(this,"","复制图片失败：\r\n请重试！",CDlgMessageBox::IMAGE_WARNING,true,5);
					}
				}
				DeleteFile(sTempFile);
			}
		}break;
	case CEF_CLIENT_MENU_COMMAND_ID_FIND:
		{
			if (m_pPanelFind==NULL) break;
			if (!m_pPanelFind->IsWindowVisible())
			{
				m_pPanelFind->ShowWindow(SW_SHOW);
			}
			m_pPanelFind->SetFindFocus();
			return true;
		}break;
	default:
		break;
	}
	return false;
}
bool CDlgFuncWindow::OnBeforePopup(const wchar_t* sTargetUrl)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return false;
	const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
	const CString csURL(sTargetUrl);
	if (bControl)
	{
		ShellExecute(NULL,  "open", csURL, NULL, NULL, SW_SHOW);
	}else
	{
		COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(csURL,"");
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
		//this->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
	}
	return true;
}

bool CDlgFuncWindow::OnBeforeBrowse(const wchar_t* sUrl)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return false;
	bool bCancel = false;
	OnBeforeNavigate(sUrl,&bCancel);
	//if (sUrl!=NULL && !bCancel && (GetKeyState(VK_CONTROL)&0x80)==0x80)
	//{
	//	bCancel = true;
	//	const CString csURL(sUrl);
	//	//ShellExecute(NULL,  "open", csURL, NULL, NULL, SW_SHOW);
	//	COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(csURL,"");
	//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
	//}
	return bCancel;
}

void CDlgFuncWindow::OnAddressChange(const wchar_t* sUrl)
{

}
void CDlgFuncWindow::OnTitleChange(const wchar_t* sTitle, const wchar_t* sUrl)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return;
	
	//SetTimer(TIMER_SET_TITLE,100,NULL);
	this->m_sTitle = CString(sTitle);
	this->SetWindowText(m_sTitle.c_str());

	CRect rectClient;
	this->GetClientRect(&rectClient);
	// ** 可以刷新
	rectClient.bottom = rectClient.top + WINDOW_TITLE_HEIGHT;
	this->InvalidateRect(&rectClient);
}
void CDlgFuncWindow::OnStatusMessage(const wchar_t* sValue)
{
	if (sValue==NULL || wcslen(sValue)==0)
	{
		if (m_pPanelStatus->IsWindowVisible())
			m_pPanelStatus->ShowWindow(SW_HIDE);
		m_sStatusMessage.clear();
	}else
	{
		m_sStatusMessage = sValue;
		KillTimer(TIMER_HIDE_STATUS_MESSAGE);
		KillTimer(TIMER_SHOW_STATUS_MESSAGE);
		SetTimer(TIMER_SHOW_STATUS_MESSAGE,100,NULL);
		SetTimer(TIMER_HIDE_STATUS_MESSAGE,10*1000,NULL);
	}
}
void CDlgFuncWindow::OnLoadingStateChange(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return;
	//if (!bIsLoading)
	{
		if (!m_bDocumentComplete)
		{
			if (m_pCefBrowser.GetSafeHwnd()==NULL) return;
			m_bDocumentComplete = true;
			CRect rect;
			this->GetClientRect(&rect);
			const int const_border_top = 0;
			const int const_border_bottom = 25;
			m_pCefBrowser.MoveWindow(1,WINDOW_TITLE_HEIGHT+const_border_top,rect.Width()-2,rect.Height()-WINDOW_TITLE_HEIGHT-(const_border_top+const_border_bottom));
			//m_pCefBrowser.MoveWindow(0,0,rect.Width(),rect.Height());
			m_pCefBrowser.SetFocus();
			if (!m_sPostData.empty())
			{
				tstring sPostData("eb_post_data=");
				sPostData.append(m_sPostData);
				//sPostData.append(libEbc::URLEncode(m_sPostData.c_str()));
				m_pCefBrowser.PostData(m_sFuncUrl,sPostData);
				m_sPostData.clear();
			}
//#ifdef USES_CEF_BROADCAST
//			if (m_bBroadcastMsg && !m_sFuncUrl.empty())
//			{
//				std::string sHtmlString;
//				GetHtmlString(libEbc::ACP2UTF8(m_sTitle.c_str()),m_sFuncUrl, sHtmlString);
//				m_pCefBrowser.LoadString(sHtmlString,"http://entboost/broadcast");
//				//m_pCefBrowser.LoadString(m_sFuncUrl,"");
//			}
//#endif
		}
	}
#ifdef USES_CEF_BROADCAST
	if (m_bDocumentComplete && !bIsLoading && m_bBroadcastMsg && !m_sFuncUrl.empty())
	{
		std::string sHtmlString;
		GetHtmlString(libEbc::ACP2UTF8(m_sTitle.c_str()),m_sFuncUrl, sHtmlString);
		m_pCefBrowser.LoadString(sHtmlString,"http://entboost/broadcast");
		m_sFuncUrl.clear();
	}
#endif


}
bool CDlgFuncWindow::OnKeyEvent(const wchar_t* sUrl, CefEventHandle os_event)
{
	switch (os_event->message)
	{
	case WM_KEYDOWN:
		{
			if (GetKeyState(VK_CONTROL)&0x80)
			{
				switch (os_event->wParam)
				{
				case VK_ADD:
					{
						m_fZoomLevel = min(CEF_DEFAULT_MAX_ZOOM_IN,m_fZoomLevel+((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET));
						m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
						ShowZoomLevel();
						return TRUE;
					}
				case VK_SUBTRACT:
					{
						m_fZoomLevel = max(CEF_DEFAULT_MAX_ZOOM_OUT,m_fZoomLevel+(((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET*2.0)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET)*-1.0));
						m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
						ShowZoomLevel();
						return TRUE;
					}
				case 'P':
					m_pCefBrowser.Print();
					break;
				case 'F':
					OnExecuteMenu(CEF_CLIENT_MENU_COMMAND_ID_FIND,NULL);
					break;
				//case VK_UP:
				//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
				//	return TRUE;
				//case VK_DOWN:
				//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
				//	return TRUE;
				//case VK_NUMPAD0:
				//case VK_NUMPAD1:
				//case VK_NUMPAD2:
				//case VK_NUMPAD3:
				//case VK_NUMPAD4:
				//case VK_NUMPAD5:
				//case VK_NUMPAD6:
				//case VK_NUMPAD7:
				//case VK_NUMPAD8:
				//case VK_NUMPAD9:
				//	theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_OFFSET, (WPARAM)(os_event->wParam-VK_NUMPAD0), 0);
				//	return TRUE;
				//case VK_LEFT:
				//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP,0,0);
				//	return TRUE;
				//case VK_RIGHT:
				//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN,0,0);
				//	return TRUE;
				default:
					break;
				}
			}
		}break;
	default:
		break;
	}
	return false;
}

void CDlgFuncWindow::OnFileReceiving(const CefDownloadInfo::pointer& pDownloadInfo)
{
	CCrFileInfo * pCrFileInfo = new CCrFileInfo();
	pCrFileInfo->m_sResId = (cr::bigint)pDownloadInfo->m_nFileId;
	pCrFileInfo->m_nMsgId = pCrFileInfo->m_sResId;
	pCrFileInfo->m_sSendFrom = pCrFileInfo->m_sResId;
	pCrFileInfo->m_nFileSize = (cr::bigint)pDownloadInfo->m_nFileSize;
	USES_CONVERSION;
	pCrFileInfo->m_sFileName = W2A(pDownloadInfo->m_sFileName.c_str());
	theApp.GetMainWnd()->SendMessage(CR_WM_RECEIVING_FILE,(WPARAM)pCrFileInfo,10);
}
bool CDlgFuncWindow::OnFilePercent(const CefDownloadInfo::pointer& pDownloadInfo, double fPercent, int nCurrentSpeed, int nTranSeconds)
{
	CChatRoomFilePercent * pChatRoomFilePercent = new CChatRoomFilePercent();
	pChatRoomFilePercent->m_sResId = (cr::bigint)pDownloadInfo->m_nFileId;
	pChatRoomFilePercent->m_nMsgId = pChatRoomFilePercent->m_sResId;
	if (fPercent<=0.0 && pDownloadInfo->m_nReceivedBytes>100)
		pChatRoomFilePercent->m_percent = (double)pDownloadInfo->m_nReceivedBytes;
	else
		pChatRoomFilePercent->m_percent = fPercent;
	pChatRoomFilePercent->m_nCurSpeed = nCurrentSpeed;
	pChatRoomFilePercent->m_nTranSeconds = nTranSeconds;
	theApp.GetMainWnd()->PostMessage(CR_WM_FILE_PERCENT,(WPARAM)pChatRoomFilePercent,10);
	if (theApp.m_pCancelFileList.exist(pDownloadInfo->m_nFileId))
		return true;
	return false;
}
void CDlgFuncWindow::OnFileCencel(const CefDownloadInfo::pointer& pDownloadInfo)
{
	CCrFileInfo * pCrFileInfo = new CCrFileInfo();
	pCrFileInfo->m_sResId = (cr::bigint)pDownloadInfo->m_nFileId;
	pCrFileInfo->m_nMsgId = pCrFileInfo->m_sResId;
	pCrFileInfo->m_sSendFrom = pCrFileInfo->m_sResId;
	//pCrFileInfo->m_nFileSize = (cr::bigint)nFileSize;
	//USES_CONVERSION;
	//pCrFileInfo->m_sFileName = W2A(sFileName);
	theApp.GetMainWnd()->PostMessage(CR_WM_CANCEL_FILE,(WPARAM)pCrFileInfo,10);
	theApp.m_pCancelFileList.remove(pDownloadInfo->m_nFileId);
}
void CDlgFuncWindow::OnFileReceived(const CefDownloadInfo::pointer& pDownloadInfo)
{
	CCrFileInfo * pCrFileInfo = new CCrFileInfo();
	pCrFileInfo->m_sResId = (cr::bigint)pDownloadInfo->m_nFileId;
	pCrFileInfo->m_nMsgId = pCrFileInfo->m_sResId;
	pCrFileInfo->m_nFileSize = (cr::bigint)pDownloadInfo->m_nFileSize;
	pCrFileInfo->m_sSendFrom = pCrFileInfo->m_sResId;
	USES_CONVERSION;
	pCrFileInfo->m_sFileName = W2A(pDownloadInfo->m_sFileName.c_str());
	theApp.GetMainWnd()->PostMessage(CR_WM_RECEIVED_FILE,(WPARAM)pCrFileInfo,10);
}
void CDlgFuncWindow::OnFindClose(void)
{
	if (m_pPanelFind->GetUserData()!=0)// && !m_pPanelFind->IsWindowVisible())
	{
		m_pPanelFind->SetUserData(0);
		CRect rect;
		this->GetClientRect(&rect);
		MovePanelFind(rect.Width());
		if (m_pPanelFind->IsWindowVisible())
			m_pPanelFind->Invalidate();
	}
	m_pCefBrowser.StopFinding(true);
	if (!m_pPanelFind->IsWindowVisible())
		m_pCefBrowser.SetFocus();
}
void CDlgFuncWindow::OnFindText(const char* lpszFindText, bool bFindUp, bool bMatchCase, bool bFindNext)
{
	m_pCefBrowser.Find(0,libEbc::ACP2UTF8(lpszFindText).c_str(),!bFindUp,bMatchCase,bFindNext);
}
#endif
//LRESULT CDlgFuncWindow::OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam)
//{
//	COpenAppUrlInfo * pOpenAppUrlInfo = (COpenAppUrlInfo*)wParam;
//	if (pOpenAppUrlInfo==0) return 0;
//	CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
//	CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
//	pFuncWindow->m_nFuncBrowserType = m_nFuncBrowserType;
//	pFuncWindow->m_sFuncUrl = pOpenAppUrlInfo->m_sUrl;
//	pFuncWindow->m_bDisableContextMenu = false;
//	pFuncWindow->m_bDisableScrollBar = false;
//	pFuncWindow->m_bClosePrompt = false;
//	pFuncWindow->m_bWinResizable = true;
//	CRect rect;
//	this->GetClientRect(&rect);
//	pFuncWindow->m_nHeight = rect.Height();
//	pFuncWindow->m_nWidth = rect.Width();
//	pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
//	delete pOpenAppUrlInfo;
//	return 0;
//}
LRESULT CDlgFuncWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOVING:
	case WM_MOVE:
		{
#ifdef USES_LIBCEF
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
			{
				m_pCefBrowser.NotifyMoveOrResizeStarted();
			}
#endif
		}break;
	case WM_ERASEBKGND:
		{
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
				return 0;
			else if (m_nBrowserType==EB_BROWSER_TYPE_IE && m_pExplorer.GetSafeHwnd()!=NULL)
				return 0;
		}break;
	default:
		break;
	}
	return __super::WindowProc(message, wParam, lParam);
}
void CDlgFuncWindow::ShowZoomLevel(void)
{
	int nZoomLevel = 100.0 + m_fZoomLevel*20.0;
	if (m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET*2.0)
		nZoomLevel += (m_fZoomLevel-CEF_DEFAULT_ZOOM_OFFSET*2.0)*m_fZoomLevel*5.0;
	//const int nZoomLevel = 100.0 + m_fZoomLevel*20.0;
	wchar_t lpszBuffer[32];
	swprintf(lpszBuffer,L"缩放：%d%%",nZoomLevel);
	this->OnStatusMessage(lpszBuffer);
	SetTimer(TIMER_HIDE_STATUS_MESSAGE,2000,NULL);
}
BOOL CDlgFuncWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
	{
		if (GetKeyState(VK_CONTROL)&0x80)
		{
			if (zDelta>=120)
			{
				// 向上滚动
				m_fZoomLevel = min(CEF_DEFAULT_MAX_ZOOM_IN,m_fZoomLevel+((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET));
				m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
				ShowZoomLevel();
			}else if (zDelta<=-120)
			{
				// 向下滚动
				m_fZoomLevel = max(CEF_DEFAULT_MAX_ZOOM_OUT,m_fZoomLevel+(((m_fZoomLevel>CEF_DEFAULT_ZOOM_OFFSET*2.0)?(CEF_DEFAULT_ZOOM_OFFSET*2.0):CEF_DEFAULT_ZOOM_OFFSET)*-1.0));
				m_pCefBrowser.SetZoomLevel(m_fZoomLevel);
				ShowZoomLevel();
				return TRUE;
			}
		}
	}
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}
