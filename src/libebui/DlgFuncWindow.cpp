// DlgFuncWindow.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgFuncWindow.h"

#define WINDOW_TITLE_HEIGHT			36
#define TIMER_REFRESH_BROWSER		0x100
#define TIMER_SET_TITLE				0x101

// CDlgFuncWindow dialog

IMPLEMENT_DYNAMIC(CDlgFuncWindow, CEbDialogBase)

CDlgFuncWindow::CDlgFuncWindow(CWnd* pParent /*=NULL*/,bool bDeleteThis)
	: CEbDialogBase(CDlgFuncWindow::IDD, pParent)
	, m_bDeleteThis(bDeleteThis)
{
	m_nMsgType = 0;
	m_bBroadcastMsg = false;
	m_bOpenNewClose = false;
	m_pBroadcastView = NULL;
	m_nWidth = 420;
	m_nHeight = 380;
	m_bDisableContextMenu = true;
	m_bDisableScrollBar = false;
	m_bClosePrompt = true;
	m_bWinResizable = true;
	m_bDocumentComplete = false;
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
	ON_EVENT(CDlgFuncWindow,IDC_EXPLORER1,259,DocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//ON_EVENT(CDlgFuncWindow,IDC_EXPLORER1,113,OnTitleChange,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


// CDlgFuncWindow message handlers

HRESULT GetPostData(LPCTSTR cszPostData,LPVARIANT pvPostData)
{
	HRESULT hr;
	LPSAFEARRAY psa;
	//LPCTSTR cszPostData = "FName=Matt&Flavor=Mocha+Chip";
	UINT cElems = lstrlen(cszPostData);
	LPSTR pPostData;

	if (!pvPostData)
	{
		return E_POINTER;
	}

	VariantInit(pvPostData);

	psa = SafeArrayCreateVector(VT_UI1, 0, cElems);
	if (!psa)
	{
		return E_OUTOFMEMORY;
	}

	hr = SafeArrayAccessData(psa, (LPVOID*)&pPostData);
	memcpy(pPostData, cszPostData, cElems);
	hr = SafeArrayUnaccessData(psa);

	V_VT(pvPostData) = VT_ARRAY | VT_UI1;

	V_ARRAY(pvPostData) = psa;
	return NOERROR;
}

BOOL CDlgFuncWindow::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//if (!m_bBroadcastMsg)
	//	ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	//theApp.AdaptIeVersion();

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
		m_btnMin.Load(IDB_PNG_MIN);
		m_btnMin.SetToolTipText(_T("最小化"));
		m_btnMax.SetAutoSize(false);
		m_btnMax.Load(IDB_PNG_MAX);
		m_btnMax.SetToolTipText(_T("最大化"));
	}
	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnClose.SetToolTipText(_T("关闭"));

	const int m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度 
	const int m_nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN)+GetSystemMetrics(SM_CYCAPTION);
	CRect rect;
	if (m_bBroadcastMsg)
	{
		m_pExplorer.ShowWindow(SW_HIDE);
		m_pBroadcastView = new CBroadcastView(this);
		m_pBroadcastView->m_strBgColor.Format(_T("#%02x%02x%02x"),GetRValue(theApp.GetBgColor2()), GetGValue(theApp.GetBgColor2()), GetBValue(theApp.GetBgColor2()));
		if ( !m_pBroadcastView->Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0x146, NULL))	// 176
		{
			delete m_pBroadcastView;
			m_pBroadcastView = NULL;
			AfxThrowResourceException();
			return FALSE;
		}
		m_pBroadcastView->SetWindowPos(&m_btnClose,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
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

		m_pBroadcastView->WriteBlock(m_sFuncUrl.c_str(),TRUE);
		m_pBroadcastView->SetBrowserFocus();
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
		m_pExplorer.Navigate(_T("About:Blank"),NULL,NULL,NULL,NULL);

		//if (m_sPostData.empty())
		//	m_pExplorer.Navigate(m_sFuncUrl.c_str(),NULL,NULL,NULL,NULL);
		//else
		//{
		//	CComVariant pHeaders(L"Content-Type: application/x-www-form-urlencoded\r\n");
		//	VARIANT pPostData={0};
		//	std::string sPostData("eb_post_data=");
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
		rect.right = rect.left + (m_nWidth>0?m_nWidth:m_nScreenWidth);
		rect.bottom = rect.top + (m_nHeight>0?m_nHeight:m_nScreenHeight);
	}
	this->FlashWindow(TRUE);
	this->SetWindowText(m_sTitle.c_str());
	this->MoveWindow(&rect);
	//this->SetForegroundWindow();
	this->ShowWindow(SW_SHOW);
	if (m_bBroadcastMsg && m_pBroadcastView!=NULL)
	{
		// **解决部分环境下，第一次弹开界面，背景不能正常刷新；
		m_pBroadcastView->Invalidate();
		//this->PostMessage(POP_WM_WINDOW_SIZE);
	}
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
			pOldFuncWindow->ShowWindow(SW_HIDE);	// *必须隐藏不让用户点击；
			pOldFuncWindow->m_pEbCallInfo.reset();	// *必须设为空，否则会清掉后面窗口
			theApp.m_pCloseWnd.add(pOldFuncWindow);
			//pOldFuncWindow->DestroyWindow();
		}

		CDlgFuncWindow * pNewFuncWindow = this;
		if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			theApp.m_pTempGroupMsg.insert(m_pEbCallInfo->m_pCallInfo.m_sGroupCode,pNewFuncWindow);
		else
			theApp.m_pTempFromUserMsg.insert(m_pEbCallInfo->m_pFromAccountInfo.GetUserId(),pNewFuncWindow);
	}
	SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFuncWindow::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);
	btnx -= m_btnMin.GetImgWidth();
	if (m_bWinResizable)
	{
		m_btnMax.MovePoint(btnx, 2);
		btnx -= m_btnMin.GetImgWidth();
	}
	m_btnMin.MovePoint(btnx, 2);

	if (m_bBroadcastMsg && m_pBroadcastView != NULL && m_pBroadcastView->GetSafeHwnd()!=NULL)
	{
		const int const_border_height = 6;
		m_pBroadcastView->MoveWindow(const_border_height,WINDOW_TITLE_HEIGHT+const_border_height,cx-const_border_height*2,cy-WINDOW_TITLE_HEIGHT-const_border_height*2);
	}else if (m_pExplorer.GetSafeHwnd())
	{
		const int const_border_top = 0;
		const int const_border_bottom = 25;
		m_pExplorer.MoveWindow(1,WINDOW_TITLE_HEIGHT+const_border_top,cx-2,cy-WINDOW_TITLE_HEIGHT-(const_border_top+const_border_bottom));
	}
}

void CDlgFuncWindow::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_bClosePrompt &&				// 配置关闭提示
		!m_bBroadcastMsg &&				// 广播信息不提示
		CDlgMessageBox::EbDoModal(this,_T("你确定退出当前应用窗口吗？"),_T(""),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
	{
		return;
	}
	if (m_bDeleteThis)
	{
		if (this->IsWindowVisible())	// **窗口隐藏，说明已经另外有线程关闭；
			this->DestroyWindow();
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
		if (theApp.GetColorGradient())
			this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,theDefaultBgColorTitleLight1,false));
		else
			this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);

		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleRegular, UnitPixel);
		//const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
		//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
		Gdiplus::PointF pointTitle(7,9);
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
	try
	{
		if (m_pEbCallInfo.get()!=NULL)
		{
			if (m_pEbCallInfo->m_pCallInfo.m_sGroupCode>0)
			{
				if (!theApp.m_pTempGroupMsg.remove(m_pEbCallInfo->m_pCallInfo.m_sGroupCode))
					return;
			}else
			{
				if (!theApp.m_pTempFromUserMsg.remove(m_pEbCallInfo->m_pFromAccountInfo.GetUserId()))
					return;
			}
			m_pEbCallInfo.reset();
		}
		CEbDialogBase::OnDestroy();
		m_webUICtrl.ClearWebBrowser();
		if (m_pBroadcastView!=NULL)
		{
			m_pBroadcastView->DestroyWindow();
			delete m_pBroadcastView;
			m_pBroadcastView = NULL;
		}
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
		m_btnMax.Load(IDB_PNG_MAX);
		if (theRestoreRect.Width()>0 && theRestoreRect.Height()>0)	// **有可能本身窗口已经是最大化，所以不需要动作
		{
			MoveWindow(&theRestoreRect);
		}
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
	if (!m_bBroadcastMsg)
		SetTimer(TIMER_REFRESH_BROWSER,10,NULL);
	//PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}

void CDlgFuncWindow::OnBnClickedButtonClose()
{
	this->PostMessage(WM_CLOSE, 0, 0);
}

void CDlgFuncWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_bWinResizable)
		OnBnClickedButtonMax();
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}

void CDlgFuncWindow::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_SET_TITLE==nIDEvent)
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
	}else if (TIMER_REFRESH_BROWSER==nIDEvent)
	{
		KillTimer(TIMER_REFRESH_BROWSER);
		if (!m_bBroadcastMsg && m_pExplorer.GetSafeHwnd())
		{
			const long nReadyState = m_pExplorer.get_ReadyState();
			if (nReadyState == READYSTATE_COMPLETE)
			{
				m_pExplorer.Refresh();
			}
		}
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
void CDlgFuncWindow::OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL)
{
	CString csURL(szURL);
	if (csURL.Right(1)==_T("/"))
		csURL = csURL.Left(csURL.GetLength()-1);
	//csURL.TrimRight('/');
	//AfxMessageBox(csURL);
	int pos = csURL.Find(theCallAccount, 0); 
	if( pos != -1 )
	{
		// ebim-call-account://[to_account]
		*bCancel = VARIANT_TRUE;	
		const CString sToAccount = csURL.Mid(pos+theCallAccount.GetLength());
#ifdef USES_EBCOM_TEST
		const bool bRet = theEBClientCore->EB_CallAccount((LPCTSTR)sToAccount,0)==S_OK?true:false;
#else
		const bool bRet = theEBAppClient.EB_CallAccount(sToAccount)==0?true:false;
#endif
		if (bRet && m_bOpenNewClose)
			OnBnClickedButtonClose();
		return;
	}
	pos = csURL.Find(theCallGroup, 0); 
	if( pos != -1 )
	{
		// ebim-call-group://[group_id]
		*bCancel = VARIANT_TRUE;	
		const CString sToGroupId = csURL.Mid(pos+theCallGroup.GetLength());
		const eb::bigint nToGroupId = eb_atoi64(sToGroupId);
#ifdef USES_EBCOM_TEST
		const bool bRet = theEBClientCore->EB_CallGroup(nToGroupId)==S_OK?true:false;
#else
		const bool bRet = theEBAppClient.EB_CallGroup(nToGroupId)==0?true:false;
#endif
		if (bRet && m_bOpenNewClose)
			OnBnClickedButtonClose();
		return;
	}
	pos = csURL.Find(theOpenSubId, 0); 
	if( pos != -1 )
	{
		// eb-open-subid://[sub_id][,xxx]
		*bCancel = VARIANT_TRUE;
		const CString sSubId = csURL.Mid(pos+theOpenSubId.GetLength());
		const eb::bigint nSubId = eb_atoi64(sSubId);
		if (nSubId>0)
		{
			CString sParameters;
			pos = sSubId.Find(",",1);
			if( pos != -1 )
				sParameters = sSubId.Mid(pos+1);

			bool bRet = false;
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo = theEBClientCore->EB_GetSubscribeFuncInfo(nSubId);
			if (pSubscribeFuncInfo!=NULL)
			{
				bRet = theApp.OpenSubscribeFuncWindow(EB_SubscribeFuncInfo(pSubscribeFuncInfo),"",(LPCTSTR)sParameters);
			}
#else
			EB_SubscribeFuncInfo pSubscribeFuncInfo;
			if (theEBAppClient.EB_GetSubscribeFuncInfo(nSubId,&pSubscribeFuncInfo))
			{
				bRet = theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"",(LPCTSTR)sParameters);
			}
#endif
			if (bRet && m_bOpenNewClose)
				OnBnClickedButtonClose();
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
	//			OnBnClickedButtonClose();
	//	}
	//	return;
	//}
	pos = csURL.Find(theOpenFile, 0); 
	if( pos != -1 )
	{
		// eb-open-file://[file-name]
		// eb-open-file://notepad.exe
		*bCancel = VARIANT_TRUE;	
		const CString sFileName = csURL.Mid(pos+theOpenFile.GetLength());
		//if (::PathFileExists(sFileName))
		{
			ShellExecute(NULL, "open", sFileName, NULL, NULL, SW_SHOW);
			if (m_bOpenNewClose)
				OnBnClickedButtonClose();
		}
		return;
	}
	pos = csURL.Find(theClose, 0);
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;	
		OnBnClickedButtonClose();
		return;
	}
	pos = csURL.Find(theExitApp, 0);
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_EXIT_APP);
		return;
	}
	pos = csURL.Find(theLogout, 0);
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_LOGOUT);
		return;
	}
	pos = csURL.Find(theShowMainFrame, 0);
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_SHOWHIDE_MAIN,1);
		return;
	}
	pos = csURL.Find(theHideMainFrame, 0);
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;	
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_SHOWHIDE_MAIN,0);
		return;
	}
	pos = csURL.Find(theReqAddContact, 0); 
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;
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
		*bCancel = VARIANT_TRUE;
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
			const CString sLocationURL = m_pExplorer.get_LocationURL();
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
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		//pFuncWindow->m_sTitle = lpszTitle;
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
			OnBnClickedButtonClose();
		return;
	}
	pos = csURL.Find(theEBWindowUrl, 0); 
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;
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
			const CString sLocationURL = m_pExplorer.get_LocationURL();
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
		AFX_SWITCH_INSTANCED();
		CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		pFuncWindow->m_sFuncUrl = sWindowUrl;
		pFuncWindow->m_nWidth = nWidth;
		pFuncWindow->m_nHeight = nHeight;
		pFuncWindow->m_bDisableContextMenu = true;
		pFuncWindow->m_bDisableScrollBar = true;
		pFuncWindow->m_bClosePrompt = false;
		pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
		//pFuncWindow->ShowWindow(SW_SHOW);
		if (m_bOpenNewClose)
			OnBnClickedButtonClose();
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

void CDlgFuncWindow::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, VARIANT_BOOL FAR* Cancel) 
{
	OnBeforeNavigate(Cancel,(CString)URL->bstrVal);
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
void CDlgFuncWindow::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
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
				CComVariant pHeaders(L"Content-Type: application/x-www-form-urlencoded\r\n");
				VARIANT pPostData={0};
				std::string sPostData("eb_post_data=");
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
