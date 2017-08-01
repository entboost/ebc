// DlgAppWindow.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgAppWindow.h"
#include "DlgPrompt.h"
#include <Mmsystem.h>

#define TIMER_CHECK_QUIT							0x101
#define TIMER_SAVE_BROWSE_TITLE				0x102
#define TIMER_REDRAW_BROWSER					0x103
#define TIMER_HIDE_STATUS_MESSAGE			0x104
#define TIMER_SHOW_STATUS_MESSAGE			0x105
#define TIMER_SHOW_DEFAULT_PAGE_ICON	0x106
#define TIMER_SHOW_SUB_ID_ICON				0x107
//#define TIMER_MOVE_WINDOW							0x108
//#define TIMER_DOWNLOAD_FAVICON		0x106
//#define TIMER_SET_TITLE				0x102
#define EBWM_PROCESS_FAVICON		(WM_USER+100)

// CDlgAppWindow dialog

IMPLEMENT_DYNAMIC(CDlgAppWindow, CPARENT_DIALOG)

CDlgAppWindow::CDlgAppWindow(CWnd* pParent /*=NULL*/)
	: CPARENT_DIALOG(CDlgAppWindow::IDD, pParent)
	, m_webUICtrl(this)
#ifdef USES_LIBCEF
	, m_pCefBrowser(this)
#endif
{
#ifdef USES_LIBCEF
	m_nBrowserType = EB_BROWSER_TYPE_CEF;
#else
	m_nBrowserType = EB_BROWSER_TYPE_IE;
#endif
	m_nUserData = 0;
	m_bSaveBrowseTitle = false;
	m_bDocumentComplete = false;
	m_tLoadOrIcoFinishedTime = 0;
	//m_dwLoadingTime = 0;
	m_nShowImageType = 0;
	m_bOpenNewClose = false;
	m_bQuit = false;
	m_bSaveBrowseInfo = false;
	m_pPanelFind = NULL;
	m_pPanelStatus = NULL;
	m_fZoomLevel = 0.0;
}

CDlgAppWindow::~CDlgAppWindow()
{
	try
	{
		m_pDownloadFaviconThread.reset();
	}catch(std::exception&)
	{}catch(...)
	{}
}

void CDlgAppWindow::DoDataExchange(CDataExchange* pDX)
{
	CPARENT_DIALOG::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_pExplorer);
	DDX_Control(pDX, IDC_STATIC_STATUS_TEXT, m_labelStatusText);
}


BEGIN_MESSAGE_MAP(CDlgAppWindow, CPARENT_DIALOG)
	//ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(EBWM_PROCESS_FAVICON, OnMsgProcessFavicon)
	ON_WM_MOUSEWHEEL()
	//ON_WM_DROPFILES()
	//ON_WM_MOVE()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgAppWindow, CPARENT_DIALOG)
	//{{AFX_EVENTSINK_MAP(CDlgFuncWindow)
	ON_EVENT(CDlgAppWindow, IDC_EXPLORER1, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//DISPID_BEFORENAVIGATE
	//DISPID_NEWWINDOW
	//DISPID_NEWWINDOW2
	
	//ON_EVENT(CDlgFuncWindow, IDC_EXPLORER1, DISPID_NEWWINDOW2, NewWindow2, VTS_PDISPATCH VTS_PBOOL)
	//ON_EVENT(CDlgAppWindow, IDC_EXPLORER1, DISPID_NEWWINDOW3, NewWindow3, VTS_PDISPATCH VTS_PBOOL VTS_I4 VTS_BSTR VTS_BSTR)
	//ON_EVENT(CDlgAppWindow,IDC_EXPLORER1,DISPID_TITLECHANGE,OnTitleChange,VTS_DISPATCH VTS_PVARIANT)
	//ON_EVENT(CDlgAppWindow,IDC_EXPLORER1,DISPID_TITLEICONCHANGE,OnTitleChange,VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CDlgAppWindow,IDC_EXPLORER1,259,OnDocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//ON_EVENT(CDlgFuncWindow,IDC_EXPLORER1,113,OnTitleChange,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// CDlgAppWindow message handlers
void CDlgAppWindow::SetCtrlColor(bool bInvalidate)
{
	if (m_pPanelFind!=NULL)
		m_pPanelFind->SetCtrlColor(bInvalidate);
}

BOOL CDlgAppWindow::OnInitDialog()
{
	CPARENT_DIALOG::OnInitDialog();

	ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	//ModifyStyle(0, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN);
	//this->EnableToolTips();

	//SetTransparentType(CEbDialogBase::TT_STATIC);
	m_pPanelFind = new CPanelFind(this);
	m_pPanelFind->Create(CPanelFind::IDD,this);
	m_pPanelFind->SetFindHandler(this);
	m_pPanelStatus = new CPanelText(this);
	m_pPanelStatus->Create(CPanelText::IDD,this);
	m_pPanelStatus->ShowWindow(SW_HIDE);
	m_pPanelStatus->SetBgColor(theDefaultFlatLineColor);

	//SetCtrlColor(false);

	//ChangeBrowserType(theApp.GetDefaultBrowserType());
	m_pFuncInfo.m_bDisableContextMenu = false;
	if (m_pFuncInfo.m_nBrowserType==EB_FUNC_BROWSER_TYPE_DEFAULT)
		m_nBrowserType = theApp.GetDefaultBrowserType();
	else if (m_pFuncInfo.m_nBrowserType==EB_FUNC_BROWSER_TYPE_IE && !theApp.GetIeException())
		m_nBrowserType = EB_BROWSER_TYPE_IE;
	else
		m_nBrowserType = EB_BROWSER_TYPE_CEF;
#ifdef USES_LIBCEF
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
	{
		CRect rect;
		this->GetClientRect(&rect);
		if (m_sToNavigateUrl.empty() || !m_sPostData.empty())
			m_pCefBrowser.Create("about:blank",rect,this->GetSafeHwnd(),0);
		else
			m_pCefBrowser.Create(libEbc::ACP2UTF8(m_sToNavigateUrl.c_str()).string(),rect,this->GetSafeHwnd(),0);
		m_pExplorer.MoveWindow(0,0,0,0);
		//m_pExplorer.EnableWindow(FALSE);
	}else
	{
		CRect rect(0,0,0,0);
		m_pCefBrowser.Create("about:blank",rect,this->GetSafeHwnd(),0);
	}
#endif

	m_pExplorer.ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	// 先显示空白页，用于刷新修改IE控件。
	//m_bDisableContextMenu = FALSE;
	m_webUICtrl.EnableContextMenu(m_pFuncInfo.m_bDisableContextMenu?0:1);
	m_webUICtrl.EnableScrollBar(m_pFuncInfo.m_bDisableScrollBar?0:1);
	m_webUICtrl.SetWebBrowser(&m_pExplorer);
	//m_pExplorer.SetFocus();
	try
	{
		m_pExplorer.Navigate(_T("about:blank"),NULL,NULL,NULL,NULL);
	}catch(...)
	{}

	m_labelStatusText.SetWindowText(_T(""));
	m_labelStatusText.ShowWindow(SW_HIDE);
	m_labelStatusText.SetTextColor(RGB(61,61,61));
	m_labelStatusText.SetAlignText(CLabelEx::Align_Left);

#ifdef USES_EBDIALOGBASE
	this->SetCircle(false);
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAppWindow::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CPARENT_DIALOG::OnOK();
}

void CDlgAppWindow::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CPARENT_DIALOG::OnCancel();
}

void CDlgAppWindow::OnDestroy()
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
	m_webUICtrl.ClearWebBrowser();
	CPARENT_DIALOG::OnDestroy();
}

void CDlgAppWindow::OnSize(UINT nType, int cx, int cy)
{
	CPARENT_DIALOG::OnSize(nType, cx, cy);

#ifdef USES_LIBCEF
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_bDocumentComplete && m_pCefBrowser.GetSafeHwnd()!=NULL)
	{
		//KillTimer(TIMER_MOVE_WINDOW);
		//SetTimer(TIMER_MOVE_WINDOW,10,NULL);
		m_pCefBrowser.MoveWindow(0,0,cx,cy);
	}
#endif
	if (m_nBrowserType==EB_BROWSER_TYPE_IE && m_pExplorer.GetSafeHwnd()!=NULL)
	{
		m_pExplorer.MoveWindow(0,0,cx,cy);
	}
	if (m_labelStatusText.GetSafeHwnd())
		m_labelStatusText.MoveWindow(3,cy-24,cx-6,24);

	MovePanelFind(cx);
	if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
	{
		SetTimer(TIMER_SHOW_STATUS_MESSAGE,100,NULL);
	}
	//if (m_pPanelStatus!=NULL)
	//{
	//	const int const_status_height = 22;
	//	m_pPanelStatus->MoveWindow(-1,cy-const_status_height-1,300,const_status_height);
	//}
}

void CDlgAppWindow::Navigate(const std::string& sUrl, const std::string& sPostData)
{
	m_sPostData = sPostData;
	if (m_sPostData.size()>2048)
		m_sPostData = m_sPostData.substr(0,2048);
	if (!m_bDocumentComplete)
		m_sToNavigateUrl = sUrl;
	else
	{
#ifdef USES_LIBCEF
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		{
			//if (m_sToNavigateUrl==sUrl)
			//{
			//	m_pCefBrowser.Reload();
			//}else
			{
				m_sToNavigateUrl = sUrl;	// ** 记录用于 OnLoadingStateChange 判断，当前是否在加载
				m_pCefBrowser.LoadURL(libEbc::ACP2UTF8(sUrl.c_str()).string());
			}
			return;
		}
#endif
		if (m_pExplorer.GetSafeHwnd()!=NULL)
			m_pExplorer.Navigate(sUrl.c_str(),NULL,NULL,NULL,NULL);
	}
}

CString CDlgAppWindow::GetLocationURL(void) const
{
#ifdef USES_LIBCEF
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		return m_pCefBrowser.GetLocationURL().c_str();
#endif
	CDlgAppWindow* pOwner = const_cast<CDlgAppWindow*>(this);
	return pOwner->m_pExplorer.get_LocationURL();
}

void CDlgAppWindow::doRefreshOrStop(void)
{
	if (this->IsLoading())
	{
		try
		{
#ifdef USES_LIBCEF
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
			{
				m_pCefBrowser.StopLoad();
				return;
			}
#endif
			m_pExplorer.Stop();
		}catch(...)
		{
		}
	}else if (m_bDocumentComplete)
	{
		try
		{
#ifdef USES_LIBCEF
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
			{
				m_pCefBrowser.ReloadIgnoreCache();
				return;
			}
#endif
			m_pExplorer.Refresh();
		}catch(...)
		{
		}
	}

	//if (m_bDocumentComplete)
	//{
	//	//m_pExplorer.ShowWindow(SW_HIDE);
	//	//m_pExplorer.ShowWindow(SW_SHOW);
	//	//m_pExplorer.RedrawWindow();
	//	//m_pExplorer.Invalidate();
	//	//this->Invalidate();
	//	//CRect rect;
	//	//m_pExplorer.GetClientRect(&rect);
	//	//m_pExplorer.InvalidateRect(&rect,TRUE);	// FALSE

	//	//m_pExplorer.Refresh();
	//	//theApp.InvalidateParentRect(&m_pExplorer);
	//}
}
bool CDlgAppWindow::IsLoading(void) const
{
	try
	{
#ifdef USES_LIBCEF
		if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
		{
			return m_pCefBrowser.IsLoading();
		}
#endif
		const long nReadyState = const_cast<CExplorer&>(m_pExplorer).get_ReadyState();
		return (nReadyState == READYSTATE_LOADING)?true:false;
	}catch(...)
	{
	}
	return false;
}

void CDlgAppWindow::goBack(void)
{
	if (m_bDocumentComplete)
	{
		try
		{
#ifdef USES_LIBCEF
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
			{
				m_pCefBrowser.GoBack();
				return;
			}
#endif
			m_pExplorer.GoBack();
		}catch(...)
		{
		}
	}
}
void CDlgAppWindow::goForward(void)
{
	if (m_bDocumentComplete)
	{
		try
		{
#ifdef USES_LIBCEF
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
			{
				m_pCefBrowser.GoForward();
				return;
			}
#endif
			m_pExplorer.GoForward();
		}catch(...)
		{
		}
	}
}
void CDlgAppWindow::SetWebFocus(bool bRefresh)
{
#ifdef USES_LIBCEF
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
	{
		m_pCefBrowser.SetFocus();
		if (bRefresh)
			SetTimer(TIMER_REDRAW_BROWSER,10,NULL);
		//HWND hWnd = m_pCefBrowser.GetSafeHwnd();
		//if (hWnd!=NULL)
		//{
		//	::ShowWindow(hWnd,SW_HIDE);
		//	::ShowWindow(hWnd,SW_SHOW);
		//}
		return;
	}
#endif
	if (m_pExplorer.GetSafeHwnd()!=NULL)
		m_pExplorer.SetFocus();
}
void CDlgAppWindow::NotifyMoveOrResizeStarted(void)
{
#ifdef USES_LIBCEF
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
	{
		m_pCefBrowser.NotifyMoveOrResizeStarted();
		//::SetWindowPos(m_pCefBrowser.GetSafeHwnd(),wndTopMost.GetSafeHwnd(),0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
#endif
}

void CDlgAppWindow::OnLogonSuccess(void)
{
#ifdef USES_LIBCEF
	if (EB_BROWSER_TYPE_CEF==m_nBrowserType && m_pCefBrowser.CanExecuteJavaScript("entboost_on_user_logon_success"))
	{
		char lpszJSCommand[64];
		sprintf(lpszJSCommand,"entboost_on_user_logon_success(%lld);", theApp.GetLogonUserId());
		m_pCefBrowser.ExecuteJavaScript(lpszJSCommand,"",0);
	}
#endif
}
void CDlgAppWindow::OnOffline(int nServerState)
{
#ifdef USES_LIBCEF
	if (EB_BROWSER_TYPE_CEF==m_nBrowserType && m_pCefBrowser.CanExecuteJavaScript("entboost_on_user_off_line"))
	{
		char lpszJSCommand[64];
		sprintf(lpszJSCommand,"entboost_on_user_off_line(%lld,%d);", theApp.GetLogonUserId(),nServerState);
		m_pCefBrowser.ExecuteJavaScript(lpszJSCommand,"",0);
	}
#endif
}

void CDlgAppWindow::InvalidateRect2(LPRECT lpRect, BOOL bErase)
{
#ifdef USES_LIBCEF
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
	{
		m_pCefBrowser.InvalidateRect(lpRect);
		return;
	}
#endif
	if (m_pExplorer.GetSafeHwnd()!=0)
		m_pExplorer.InvalidateRect(lpRect,bErase);
}
void CDlgAppWindow::ChangeBrowserType(EB_BROWSER_TYPE nNewBrowserType)
{
#ifdef USES_LIBCEF
	if (nNewBrowserType!=m_nBrowserType)
	{
		std::string sUrl;
		if (EB_BROWSER_TYPE_CEF==m_nBrowserType)
		{
			sUrl = m_pCefBrowser.GetLocationURL();
			if (theApp.GetIeException()) return;
		}else
			sUrl = m_pExplorer.get_LocationURL();
		m_nBrowserType = nNewBrowserType;
		if (sUrl.empty())
			sUrl = "about:blank";
		CRect rect;
		this->GetClientRect(&rect);
		if (EB_BROWSER_TYPE_CEF==m_nBrowserType)
		{
			m_pCefBrowser.Create(sUrl,rect,this->GetSafeHwnd(),0);
			m_pCefBrowser.MoveWindow(rect.left,rect.top,rect.Width(),rect.Height());
			m_pExplorer.Navigate("about:blank",NULL,NULL,NULL,NULL);
			m_pExplorer.MoveWindow(0,0,0,0);
		}else
		{
			m_pExplorer.Navigate(sUrl.c_str(),NULL,NULL,NULL,NULL);
			m_pExplorer.MoveWindow(&rect);
			m_pCefBrowser.LoadURL("about:blank");
			m_pCefBrowser.MoveWindow(0,0,0,0);
		}
	}
#endif
}
void CDlgAppWindow::changeBrowserType(void)
{
#ifdef USES_LIBCEF
	if (EB_BROWSER_TYPE_CEF==m_nBrowserType)
		ChangeBrowserType(EB_BROWSER_TYPE_IE);
	else
		ChangeBrowserType(EB_BROWSER_TYPE_CEF);
#endif
}
bool CDlgAppWindow::canSaveHistory(void) const
{
	const CString sUrl = GetLocationURL();
	return (!sUrl.IsEmpty()&& sUrl!="about:blank")?true:false;

}
void CDlgAppWindow::saveHistory(void)
{
	KillTimer(TIMER_SAVE_BROWSE_TITLE);
	m_bSaveBrowseInfo = false;
	SetTimer(TIMER_SAVE_BROWSE_TITLE,1000,NULL);
}

void CDlgAppWindow::OnMove(void)
{
	KillTimer(TIMER_REDRAW_BROWSER);
	SetTimer(TIMER_REDRAW_BROWSER,20,NULL);

	//if (m_pPanelFind!=NULL)
	//	m_pPanelFind->OnMove();
}
//void CDlgAppWindow::OnShowHide(bool bShow)
//{
//	this->ShowWindow(bShow?SW_SHOW:SW_HIDE);
//	if (m_pPanelFind!=NULL)
//		m_pPanelFind->OnShowHide(bShow);
//}


void CDlgAppWindow::OnStatusTextChange(BSTR sStatusText)
{
	//if (EB_BROWSER_TYPE_IE==m_nBrowserType)
	//	OnStatusMessage(sStatusText);

	//CString m_sStatusText(sStatusText);
	//if (m_sStatusText.Find(".ico")>0)
	//	int i=0;

	//if (!m_bQuit)
	//{
	//	static CString theCurrentStatusText;
	//	CString m_sStatusText(sStatusText);
	//	if (theCurrentStatusText==m_sStatusText)
	//		return;
	//	theCurrentStatusText = m_sStatusText;
	//	if (m_sStatusText==_T("完成") || m_sStatusText.IsEmpty())
	//	{
	//		return;
	//		m_sStatusText = _T("");
	//	}
	//	m_labelStatusText.ShowWindow(SW_HIDE);
	//	m_labelStatusText.SetWindowText(m_sStatusText);
	//	m_labelStatusText.ShowWindow(m_sStatusText.IsEmpty()?SW_HIDE:SW_SHOW);
	//	//theApp.InvalidateParentRect(&m_labelStatusText);
	//	//m_labelStatusText.Invalidate();
	//}

}
//#include <UrlHist.h>  // IUrlHistoryStg2 
//#include <shlobj.h>   // CLSID_CUrlHistory, SHAddToRecentDocs
//#include <atlbase.h>  // USES_CONVERSION;
//void GetIEHistory()
//{
//	USES_CONVERSION;//Unicode转Ansi用
//	CoInitialize(NULL); //初始化
//
//	IUrlHistoryStg2* pUrlHistoryStg2 = NULL;
//	HRESULT hr = CoCreateInstance(CLSID_CUrlHistory,
//		NULL, CLSCTX_INPROC, IID_IUrlHistoryStg2,
//		(void**)&pUrlHistoryStg2);
//
//	/**//*if (SUCCEEDED(hr))
//		{
//		hr = pUrlHistoryStg2->ClearHistory();
//		pUrlHistoryStg2->Release();
//		}*/
//
//	IEnumSTATURL* pEnumURL;
//	hr = pUrlHistoryStg2->EnumUrls(&pEnumURL);
//
//	STATURL suURL;
//	ULONG pceltFetched;
//	suURL.cbSize = sizeof(suURL);
//	hr = pEnumURL->Reset();
//
//	while((hr = pEnumURL->Next(1, &suURL, &pceltFetched)) == S_OK)
//	{
//		//hr = pUrlHistoryStg2->DeleteUrl(suURL.pwcsUrl, 0);
//		const tstring sUrl(W2T(suURL.pwcsUrl));
//		if (suURL.pwcsTitle!=0 && sUrl.substr(0,4)=="http")
//			int i=0;
//		printf("%s ", W2T(suURL.pwcsUrl));
//	}
//
//	pEnumURL->Release();
//	pUrlHistoryStg2->Release();
//	CoUninitialize();
//}

const CString& CDlgAppWindow::GetFaviconFile(void) const
{
	if (m_nShowImageType==1)
			return theApp.GetDefaultPageImageFile();
	if (m_nShowImageType==0 || m_sFaviconFile.IsEmpty())
			return theApp.GetDefaultLoadingImageFile();
	return m_sFaviconFile;
}
void CDlgAppWindow::DoDownloadFavicon(void)
{
	try
	{
		if (!m_sDownloadFaviconDomainName.IsEmpty() && !m_sDownloadFaviconFaviconUrl.IsEmpty())
		{
			const CString sDomainName(m_sDownloadFaviconDomainName);
			const CString sFaviconUrl(m_sDownloadFaviconFaviconUrl);
			m_sDownloadFaviconDomainName = "";
			m_sDownloadFaviconFaviconUrl = "";

			CString sFaviconFile;
			sFaviconFile.Format(_T("%s\\favicon-%s.ico"),theApp.GetAppImgTempPath(),sDomainName);
			const CString sImageTempIniPath = theApp.GetAppImgTempPath()+_T("\\favicon.ini");
			char lpszOldLastModified[64];
			memset(lpszOldLastModified,0,64);
			if (PathFileExists(sFaviconFile))
			{
				m_sFaviconFile = sFaviconFile;
				m_tLoadOrIcoFinishedTime = time(0);
				m_nShowImageType = 3;
				this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)(CWnd*)this,(LPARAM)0);
				GetPrivateProfileString(_T("last_modified"),sDomainName,_T(""),lpszOldLastModified,64,sImageTempIniPath);
			}

			CString sNewLastModified;
			if (DownloadHttpFile(sFaviconUrl,sFaviconFile,lpszOldLastModified,sNewLastModified))
			{
				m_sFaviconFile = sFaviconFile;
				m_tLoadOrIcoFinishedTime = time(0);
				m_nShowImageType = 3;
				this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)(CWnd*)this,(LPARAM)0);
				WritePrivateProfileString(_T("last_modified"),sDomainName,sNewLastModified,sImageTempIniPath);
			}
		}
		if (!m_bQuit && m_nShowImageType!=3)
		{
			m_nShowImageType = 1;
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)(CWnd*)this,(LPARAM)0);
		}
		m_pDownloadFaviconThread.reset();
	}catch(std::exception&)
	{}catch(...)
	{}
}

LRESULT CDlgAppWindow::OnMsgProcessFavicon(WPARAM wParam, LPARAM lParam)
{
	const bool bOnDocumentComplete = wParam==1?true:false;
	// <link rel="shortcut icon" href="/favicon.ico" />
	// <link href="https://kryptoners.b0.upaiyun.com/assets/favicon-6523a7e532a9862be505b7657fd7ddb7.ico" rel="shortcut icon" type="image/vnd.microsoft.icon" />

	// http://www.entboost.com/favicon.ico
	CString sLocationUrl;
	CString sFaviconUrl;
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
	{
		if (lParam==0) return 0;
		const char * lpszFaviconUrl = (const char*)lParam;
		sFaviconUrl = lpszFaviconUrl;
		delete[] lpszFaviconUrl;
		sLocationUrl = m_pCefBrowser.GetLocationURL().c_str();
	}else
		sLocationUrl = m_pExplorer.get_LocationURL();
	CString sDomainName(sLocationUrl);
	int nFind = sDomainName.Find("://");
	CString sHttp;
	if (nFind>0)
	{
		sHttp = sDomainName.Left(nFind+3);	// http:// or https://
		sDomainName = sDomainName.Mid(nFind+3);
		nFind = sDomainName.Find("/");
		if (nFind>0)
		{
			sDomainName = sDomainName.Mid(0,nFind);
		}
		if (m_nBrowserType==EB_BROWSER_TYPE_IE)
		{
			sFaviconUrl = m_webUICtrl.GetFaviconUrl();
			if (sFaviconUrl.IsEmpty())
			{
				if (bOnDocumentComplete)
					return 0;
				sFaviconUrl.Format(_T("%s%s/favicon.ico"),sHttp,sDomainName);
			}else if (sFaviconUrl.Left(1)=="/")
			{
				CString sTemp(sFaviconUrl);
				sFaviconUrl.Format(_T("%s%s%s"),sHttp,sDomainName,sTemp);
			}
		}
		// 
		if (m_pDownloadFaviconThread.get()!=NULL) return 0;
		m_sDownloadFaviconDomainName = sDomainName;
		m_sDownloadFaviconFaviconUrl = sFaviconUrl;
		m_pDownloadFaviconThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CDlgAppWindow::DoDownloadFavicon, this)));

		//SetTimer(TIMER_DOWNLOAD_FAVICON,10,NULL);
		//CString sFaviconFile;
		//sFaviconFile.Format(_T("%s\\favicon-%s.ico"),theApp.GetAppImgTempPath(),sDomainName);
		//const CString sImageTempIniPath = theApp.GetAppImgTempPath()+_T("\\favicon.ini");
		//char lpszOldLastModified[64];
		//memset(lpszOldLastModified,0,64);
		//if (PathFileExists(sFaviconFile))
		//{
		//	m_sFaviconFile = sFaviconFile;
		//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)this,(LPARAM)0);
		//	GetPrivateProfileString(_T("last_modified"),sDomainName,_T(""),lpszOldLastModified,64,sImageTempIniPath);
		//}

		//CString sNewLastModified;
		//if (DownloadHttpFile(sFaviconUrl,sFaviconFile,lpszOldLastModified,sNewLastModified))
		//{
		//	m_sFaviconFile = sFaviconFile;
		//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)this,(LPARAM)0);
		//	WritePrivateProfileString(_T("last_modified"),sDomainName,sNewLastModified,sImageTempIniPath);
		//}
	}
	return 0;
}

void CDlgAppWindow::OnTitleChange(BSTR sTitle)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_IE) return;
	if (!m_bQuit)
	{
		//static bool theGet = false;
		//if (!theGet)
		//{
		//	theGet = true;
		//	GetIEHistory();
		//}

		const CString m_sTitle(sTitle);
		if (m_sCurrentTitle!=m_sTitle)
		{
			m_sCurrentTitle = m_sTitle;
			char * lpszBuffer = new char[m_sTitle.GetLength()+1];
			strcpy(lpszBuffer,m_sTitle);
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_TITLE,(WPARAM)(CWnd*)this,(LPARAM)lpszBuffer);

			const CString sLocationUrl(m_pExplorer.get_LocationURL());
			if (m_sCurrentTitle!="about:blank" && (!m_bSaveBrowseInfo || sLocationUrl.Right(1)==_T("/")))
			{
				this->PostMessage(EBWM_PROCESS_FAVICON,0,0);
				if (m_bSaveBrowseTitle && m_pFuncInfo.m_nSubscribeId==0)
				{
					KillTimer(TIMER_SAVE_BROWSE_TITLE);
					SetTimer(TIMER_SAVE_BROWSE_TITLE,5000,NULL);
				}
			}
		}
	}
}
void CDlgAppWindow::OnNewWindow3(IDispatch **ppDisp, VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{
	// dwFlags=NWMF_USERINITED+NWMF_FIRST
	if (m_nBrowserType!=EB_BROWSER_TYPE_IE) return;
	const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
	if (!bControl)
	{
		//const CString csURLContext(bstrUrlContext);
		*Cancel = VARIANT_TRUE;
		const CString csURL(bstrUrl);
		COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(csURL, "");
		pOpenAppUrlInfo->m_nOpenParam = 0;
		pOpenAppUrlInfo->m_hwndFrom = this->GetSafeHwnd();
		this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
	}
}
#ifdef USES_LIBCEF
void CDlgAppWindow::MovePanelFind(int cx)
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
		rect.top = -2;
		rect.bottom = rect.top + DEFAULT_PANEL_FIND_HEIGHT;
		m_pPanelFind->MoveWindow(&rect);
	}
}
bool CDlgAppWindow::OnJSDialog(const wchar_t* sOriginUrl, const wchar_t* sAcceptLang, cef_jsdialog_type_t dialog_type, const wchar_t* sMessageText,const wchar_t* sDefaultPromptText,bool& pOutSuccess,std::string& pOutUserInput)
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
			//const tstring sTemp = libEbc::UTF82ACP(sText);
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
void CDlgAppWindow::OnFindResult(int identifier,int count,const CefRect& selectionRect,int activeMatchOrdinal,bool finalUpdate)
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
bool CDlgAppWindow::OnExecuteMenu(int nCommandId, const wchar_t* lpszUrl)
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

bool CDlgAppWindow::OnCefClose(void)
{
	//if (m_pFuncInfo.m_bClosePrompt &&				// 配置关闭提示
	//	CDlgMessageBox::EbDoModal(this,_T("关闭窗口"),_T("关闭确认：\r\n确定关闭当前应用窗口吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
	//{
	//	return true;
	//}
	return false;
}

bool CDlgAppWindow::OnBeforePopup(const wchar_t* sTargetUrl)
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
		pOpenAppUrlInfo->m_nOpenParam = 0;
		pOpenAppUrlInfo->m_hwndFrom = this->GetSafeHwnd();
		this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
	}
	return true;
}

bool CDlgAppWindow::OnBeforeBrowse(const wchar_t* sUrl)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return false;
	bool bCancel = false;
	OnBeforeNavigate(sUrl,&bCancel);
	if (sUrl!=NULL && !bCancel && (GetKeyState(VK_CONTROL)&0x80)==0x80)
	{
		const CString csURL(sUrl);
		if (csURL!=m_sToNavigateUrl.c_str())
		{
			bCancel = true;
			COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(csURL,"");
			pOpenAppUrlInfo->m_nOpenParam = 0;
			pOpenAppUrlInfo->m_hwndFrom = this->GetSafeHwnd();
			this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
		}
	}
	return bCancel;
}

void CDlgAppWindow::OnAddressChange(const wchar_t* sUrl)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return;

	if (this->IsWindowVisible())	// 当前窗口显示才发送消息
	{
		const CString sUrlTemp(sUrl);
		char * lpszBuffer = new char[sUrlTemp.GetLength()+1];
		strcpy(lpszBuffer,sUrlTemp);
		const LPARAM nSearchFocus = 0;
		//const LPARAM nSearchFocus = (sUrlTemp=="about:blank")?1:0;
		this->GetParent()->PostMessage(EB_COMMAND_CHANGE_APP_URL,(WPARAM)lpszBuffer,nSearchFocus);
	}
}
void CDlgAppWindow::OnTitleChange(const wchar_t* sTitle, const wchar_t* sUrl)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return;
	if (!m_bQuit)
	{
		const CString m_sTitle(sTitle);
		if (!m_sPostData.empty() && m_sTitle=="about:blank")
			return;
		if (m_sCurrentTitle!=m_sTitle)
		{
			m_sCurrentTitle = m_sTitle;
			char * lpszBuffer = new char[m_sTitle.GetLength()+1];
			strcpy(lpszBuffer,m_sTitle);
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_TITLE,(WPARAM)(CWnd*)this,(LPARAM)lpszBuffer);

			const CString sLocationUrl(sUrl);
			if (m_sCurrentTitle!="about:blank" && (!m_bSaveBrowseInfo || sLocationUrl.Right(1)==_T("/")))
			{
				//this->PostMessage(EBWM_PROCESS_FAVICON,0,0);
				//m_pCefBrowser.GetL
				if (m_bSaveBrowseTitle && m_pFuncInfo.m_nSubscribeId==0)
				{
					m_bSaveBrowseTitle = false;
					KillTimer(TIMER_SAVE_BROWSE_TITLE);
					SetTimer(TIMER_SAVE_BROWSE_TITLE,500,NULL);	// 5000
				}
			}
		}
	}
}

void CDlgAppWindow::OnFaviconURLChange(const wchar_t* sFaviconURL, const wchar_t* sUrl)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return;
	if (m_pFuncInfo.m_nSubscribeId>0)
	{
		return;	// 应用不使用HTTP favicon
	}
	m_nShowImageType = 2;
	KillTimer(TIMER_SHOW_DEFAULT_PAGE_ICON);
	CString sFaviconURLTemp(sFaviconURL);
	char * lpszBuffer = new char[sFaviconURLTemp.GetLength()+1];
	strcpy(lpszBuffer,sFaviconURLTemp);
	this->PostMessage(EBWM_PROCESS_FAVICON,1,(LPARAM)lpszBuffer);
}

void CDlgAppWindow::OnStatusMessage(const wchar_t* sValue)
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
void CDlgAppWindow::OnLoadError(int nErrorCode,const wchar_t* sErrorText, const wchar_t* sFailedUrl)
{
  //if (errorCode == ERR_ABORTED)
	m_bSaveBrowseTitle = false;
	const WPARAM nShowRefreshOrStop = 1;	// 1=show refresh; 2=show stop
	this->GetParent()->PostMessage(EB_COMMAND_SHOW_REFRESH_OR_STOP,nShowRefreshOrStop,(WPARAM)(CWnd*)this);
	//m_pCefBrowser.StopLoad();
}
void CDlgAppWindow::OnLoadingStateChange(const wchar_t* sUrl, bool bIsLoading, bool bCanGoBack, bool bCanGoForward)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_CEF) return;

	if (m_sToNavigateUrl!="about:blank")
	{
		const WPARAM nShowRefreshOrStop = bIsLoading?2:1;	// 1=show refresh; 2=show stop
		this->GetParent()->PostMessage(EB_COMMAND_SHOW_REFRESH_OR_STOP,nShowRefreshOrStop,(WPARAM)(CWnd*)this);
	}

	if (bIsLoading)
	{
		// 0,1=S
		if ((m_tLoadOrIcoFinishedTime==0 || (m_tLoadOrIcoFinishedTime+1)<time(0)) && !m_sToNavigateUrl.empty() && m_sToNavigateUrl!="about:blank")
		{
			//m_dwLoadingTime = timeGetTime();
			m_nShowImageType = 0;
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)(CWnd*)this,(LPARAM)0);
		}
	}else if (!m_sToNavigateUrl.empty() && m_sToNavigateUrl!="about:blank")
	{
		m_tLoadOrIcoFinishedTime = time(0);
		bool bFindSubIdImageFile = (m_pFuncInfo.m_nSubscribeId>0 && !m_sFaviconFile.IsEmpty())?true:false;
		if (m_pFuncInfo.m_nSubscribeId>0 && m_sFaviconFile.IsEmpty())
		{
			if (::PathFileExists(m_pFuncInfo.m_sResFile.c_str()))
			{
				bFindSubIdImageFile = true;
				m_sFaviconFile = m_pFuncInfo.m_sResFile.c_str();
			}else if (::PathFileExists(theApp.m_sDefaultAppImage.c_str()))
			{
				bFindSubIdImageFile = true;
				m_sFaviconFile = theApp.m_sDefaultAppImage.c_str();
			}
		}
		if (!m_sFaviconFile.IsEmpty())
		{
			m_nShowImageType = 3;
			if (bFindSubIdImageFile)
			{
				KillTimer(TIMER_SHOW_SUB_ID_ICON);
				SetTimer(TIMER_SHOW_SUB_ID_ICON,500,NULL);
			}else// if (m_dwLoadingTime==0 || (timeGetTime()-m_dwLoadingTime)>500)
			{
				this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)(CWnd*)this,(LPARAM)0);
			//}else
			//{
			//	KillTimer(TIMER_SHOW_SUB_ID_ICON);
			//	SetTimer(TIMER_SHOW_SUB_ID_ICON,500-(timeGetTime()-m_dwLoadingTime),NULL);
			}
		}else if (m_nShowImageType==0)
		{
			KillTimer(TIMER_SHOW_DEFAULT_PAGE_ICON);
			SetTimer(TIMER_SHOW_DEFAULT_PAGE_ICON,1000,NULL);
		}
	}
	//if (!bIsLoading)
	{
		if (!m_bDocumentComplete)
		{
			if (m_pCefBrowser.GetSafeHwnd()==NULL) return;
			m_bDocumentComplete = true;
			//KillTimer(TIMER_MOVE_WINDOW);
			//SetTimer(TIMER_MOVE_WINDOW,100,NULL);
			CRect rect;
			this->GetClientRect(&rect);
			m_pCefBrowser.MoveWindow(0,0,rect.Width(),rect.Height());
			m_pCefBrowser.SetFocus();

			//if (!m_sToNavigateUrl.empty() && m_sToNavigateUrl!="about:blank")
			//	m_pCefBrowser.LoadURL(libEbc::ACP2UTF8(m_sToNavigateUrl.c_str()).string());

			//if (!m_sToNavigateUrl.empty())
			//{
			//	m_pCefBrowser.LoadURL(m_sToNavigateUrl);
			//	m_sToNavigateUrl.clear();
			//}
		}
		if (!m_sPostData.empty())
		{
			tstring sPostData("eb_post_data=");
			sPostData.append(m_sPostData);
			m_pCefBrowser.PostData(libEbc::ACP2UTF8(m_sToNavigateUrl.c_str()).string(),sPostData);
			m_sPostData.clear();
		}

		//if (!m_bQuit)// && m_sFaviconFile.IsEmpty())
		//{
		//	this->PostMessage(EBWM_PROCESS_FAVICON,1,0);
		//}
	}
}
bool CDlgAppWindow::OnKeyEvent(const wchar_t* sUrl, CefEventHandle os_event)
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
				case VK_UP:
					theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
					return TRUE;
				case VK_DOWN:
					theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
					return TRUE;
				case VK_NUMPAD0:
				case VK_NUMPAD1:
				case VK_NUMPAD2:
				case VK_NUMPAD3:
				case VK_NUMPAD4:
				case VK_NUMPAD5:
				case VK_NUMPAD6:
				case VK_NUMPAD7:
				case VK_NUMPAD8:
				case VK_NUMPAD9:
					theApp.GetMainWnd()->PostMessage(EB_COMMAND_RAME_WND_MOVE_OFFSET, (WPARAM)(os_event->wParam-VK_NUMPAD0), 0);
					return TRUE;
				case VK_LEFT:
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP,0,0);
					return TRUE;
				case VK_RIGHT:
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN,0,0);
					return TRUE;
				case VK_TAB:
					if (GetKeyState(VK_SHIFT)&0x80)
						this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP,0,0);
					else
						this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN,0,0);
					return TRUE;
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

void CDlgAppWindow::OnFileReceiving(const CefDownloadInfo::pointer& pDownloadInfo)
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
bool CDlgAppWindow::OnFilePercent(const CefDownloadInfo::pointer& pDownloadInfo, double fPercent, int nCurrentSpeed, int nTranSeconds)
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
void CDlgAppWindow::OnFileCencel(const CefDownloadInfo::pointer& pDownloadInfo)
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
void CDlgAppWindow::OnFileReceived(const CefDownloadInfo::pointer& pDownloadInfo)
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

void CDlgAppWindow::OnFindClose(void)
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
void CDlgAppWindow::OnFindText(const char* lpszFindText, bool bFindUp, bool bMatchCase, bool bFindNext)
{
	m_pCefBrowser.Find(0,libEbc::ACP2UTF8(lpszFindText).c_str(),!bFindUp,bMatchCase,bFindNext);
}
#endif

static CString theDownloadResource = _T("eb-download-resource://");
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
void CDlgAppWindow::OnBeforeNavigate(const wchar_t* szURL, bool* pOutCancel)
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
		*pOutCancel = true;	
		const CString sToAccount = csURL.Mid(pos+theCallAccount.GetLength());
#ifdef USES_EBCOM_TEST
		const bool bRet = theEBClientCore->EB_CallAccount((LPCTSTR)sToAccount,0)==S_OK?true:false;
#else
		const bool bRet = theEBAppClient.EB_CallAccount(sToAccount)==0?true:false;
#endif
		if (bRet && m_bOpenNewClose)
			this->PostMessage(WM_CLOSE, 0, 0);
		return;
	}
	pos = csURL.Find(theDownloadResource, 0); 
	if( pos != -1 )
	{
		// eb-download-resource://[TYPE],[RESOURCEID]
		// eb-download-resource://[TYPE],[RESOURCEID],[FILENAME]
		*pOutCancel = true;	
		const CString sResourceInfo = csURL.Mid(pos+theDownloadResource.GetLength());
		char * lpszBuffer = new char[sResourceInfo.GetLength()+1];
		strcpy(lpszBuffer,sResourceInfo);
		theApp.GetMainWnd()->PostMessage(EB_COMMAND_DOWNLOAD_RESOURCE,(WPARAM)lpszBuffer,(LPARAM)0);
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
			this->PostMessage(WM_CLOSE, 0, 0);
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
					Navigate((LPCTSTR)sUrl,"");
				}
				return;
			}

			bool bRet = false;
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_SubscribeFuncInfo> pSubscribeFuncInfo = theEBClientCore->EB_GetSubscribeFuncInfo(nSubId);
			if (pSubscribeFuncInfo!=NULL)
			{
				//bRet = theApp.OpenSubscribeFuncWindow(EB_SubscribeFuncInfo(pSubscribeFuncInfo),"",(LPCTSTR)sParameters,this->GetSafeHwnd());
				bRet = true;
			}
#else
			EB_SubscribeFuncInfo pSubscribeFuncInfo;
			if (theEBAppClient.EB_GetSubscribeFuncInfo(nSubId,&pSubscribeFuncInfo))
			{
				//bRet = theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"",(LPCTSTR)sParameters,this->GetSafeHwnd());
				bRet = true;
			}
#endif
			if (bRet)
			{
				COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(nSubId,(LPCTSTR)sParameters);
				pOpenAppUrlInfo->m_hwndFrom = this->GetSafeHwnd();
				theApp.GetMainWnd()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
				if (m_bOpenNewClose)
					this->PostMessage(WM_CLOSE, 0, 0);
			}
		}
		return;
	}
	//pos = csURL.Find(theOpenUrl, 0); 
	//if( pos != -1 )
	//{
	//	// eb-open-url://[url-name]
	//	*pOutCancel = true;	
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
		*pOutCancel = true;	
		const CString sFileName = csURL.Mid(pos+theOpenFile.GetLength());
		//if (::PathFileExists(sFileName))
		{
			ShellExecute(NULL, "open", sFileName, NULL, NULL, SW_SHOW);
			if (m_bOpenNewClose)
				this->PostMessage(WM_CLOSE, 0, 0);
		}
		return;
	}
	pos = csURL.Find(theClose, 0);
	if( pos != -1 )
	{
		*pOutCancel = true;	
		this->PostMessage(WM_CLOSE, 0, 0);
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
			//const CString sLocationURL = m_pExplorer.get_LocationURL();
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

		int nParam = 0;
		if (nDisableContextMenu==1)
			nParam |= 0x1;
		if (nDisableScrollBar==1)
			nParam |= 0x2;
		if (nClosePrompt==1)
			nParam |= 0x4;
		if (nOpenNewClose==1)
			nParam |= 0x8;
		COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(sWindowUrl,"");
		pOpenAppUrlInfo->m_nOpenParam = nParam;
		pOpenAppUrlInfo->m_hwndFrom = this->GetSafeHwnd();
		this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
		if (m_bOpenNewClose)
			this->PostMessage(WM_CLOSE, 0, 0);
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
			//const CString sLocationURL = m_pExplorer.get_LocationURL();
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
		COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(sWindowUrl,"");
		pOpenAppUrlInfo->m_nOpenParam = 0;
		pOpenAppUrlInfo->m_hwndFrom = this->GetSafeHwnd();
		this->GetParent()->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
		if (m_bOpenNewClose)
			this->PostMessage(WM_CLOSE, 0, 0);
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
//void CDlgAppWindow::NewWindow3(IDispatch **ppDisp,
//						  VARIANT_BOOL *Cancel,
//						  DWORD dwFlags,
//						  BSTR bstrUrlContext,
//						  BSTR bstrUrl
//						  )
//{
//	//const CString csURLContext(bstrUrlContext);
//	//const CString csURL(bstrUrl);
//	////csURL.MakeLower();
//	////if (csURL.Find(_T("http://"))>=0 ||
//	////	csURL.Find(_T("https://"))>=0 ||
//	////	csURL.Find(_T("www."))>=0)
//	////{
//	////	*Cancel = VARIANT_TRUE;	
//	////	ShellExecute(NULL,  "open", csURL, NULL, NULL, SW_SHOW);
//	////}
//	//*Cancel = VARIANT_TRUE;
//	//this->GetParent()->SendMessage(EB_COMMAND_RAME_WND_OPEN,(WPARAM)(LPCTSTR)csURL,(LPARAM)0);
//}
void CDlgAppWindow::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, VARIANT_BOOL FAR* Cancel) 
{
	bool bCancel = false;
	OnBeforeNavigate(URL->bstrVal,&bCancel);
	if (bCancel)
		*Cancel = VARIANT_TRUE;
}
//void CDlgAppWindow::OnTitleChange(LPDISPATCH pDisp, VARIANT* pTitle)
//{
//	//if (!m_bQuit)
//	//{
//	//	const CString m_sTitle(pTitle->bstrVal);
//	//	char * lpszBuffer = new char[m_sTitle.GetLength()+1];
//	//	strcpy(lpszBuffer,m_sTitle);
//	//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_TITLE,(WPARAM)this,(LPARAM)lpszBuffer);
//	//}
//}
void CDlgAppWindow::OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if (m_nBrowserType!=EB_BROWSER_TYPE_IE)
		return;

	const WPARAM nShowRefreshOrStop = 1;	// 1=show refresh; 2=show stop
	this->GetParent()->PostMessage(EB_COMMAND_SHOW_REFRESH_OR_STOP,nShowRefreshOrStop,(WPARAM)(CWnd*)this);

	if (!m_bDocumentComplete)
	{
		m_bDocumentComplete = true;
		m_pExplorer.SetFocus();

		//if (!m_sToNavigateUrl.empty())
		//{
		//	m_pExplorer.Navigate(m_sToNavigateUrl.c_str(),NULL,NULL,NULL,NULL);
		//	m_sToNavigateUrl.clear();
		//}
	}
	if (!m_sToNavigateUrl.empty())
	{
		if (!m_sPostData.empty())
		{
			CComVariant pHeaders(L"Content-Type: application/x-www-form-urlencoded\r\n");
			VARIANT pPostData={0};
			tstring sPostData("eb_post_data=");
			sPostData.append(m_sPostData);
			GetPostData(sPostData.c_str(),&pPostData);
			m_pExplorer.Navigate(m_sToNavigateUrl.c_str(),NULL,NULL,&pPostData,&pHeaders);
			VariantClear(&pPostData);
		}else
		{
			m_pExplorer.Navigate(m_sToNavigateUrl.c_str(),NULL,NULL,NULL,NULL);
		}
		m_sToNavigateUrl.clear();
	}
	if (!m_bQuit)// && m_sFaviconFile.IsEmpty())
	{
		this->PostMessage(EBWM_PROCESS_FAVICON,1,0);
	}
	//if (!m_bQuit)// && this->m_pFuncInfo.m_nSubscribeId==0)
	//	SetTimer(TIMER_SET_TITLE,100,NULL);
}

void CDlgAppWindow::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.IsInDownloadFile())
	{
		if (CDlgMessageBox::EbDoModal(this,_T("关闭窗口"),_T("正在下载文件，关闭窗口会清空下载任务：\r\n确定关闭当前窗口，并清空下载任务吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
		{
			return;
		}
	}
	else if (m_pFuncInfo.m_bClosePrompt &&				// 配置关闭提示
		CDlgMessageBox::EbDoModal(this,_T("关闭窗口"),_T("关闭确认：\r\n确定关闭当前窗口吗？"),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
	{
		return;
	}
	m_bQuit = true;
	if (m_bDocumentComplete && m_nBrowserType==EB_BROWSER_TYPE_IE)
	{
		// ** 解决某些页面，未加载完成（忙，如正在加载flash等），退出窗口会报异常问题；
		m_pExplorer.Navigate(_T("about:blank"),NULL,NULL,NULL,NULL);
		SetTimer(TIMER_CHECK_QUIT,50,NULL);
	//}else if (m_bDocumentComplete && m_nBrowserType==EB_BROWSER_TYPE_CEF)
	//{
	//	m_pCefBrowser.CloseAllBrowsers(false);
	}else
	{
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE,(WPARAM)(CWnd*)this,(LPARAM)m_nUserData);
	}
	//CPARENT_DIALOG::OnClose();
}

void SaveUrlRecord(const char* sUrl, const char* sTitle)
{
	tstring sUrlTemp(sUrl);
	tstring sTitleTemp(sTitle);
	CSqliteCdbc::escape_string_in(sUrlTemp);
	CSqliteCdbc::escape_string_in(sTitleTemp);
	sUrlTemp = libEbc::ACP2UTF8(sUrlTemp.c_str());
	sTitleTemp = libEbc::ACP2UTF8(sTitleTemp.c_str());
	CString sSql;
	sSql.Format(_T("UPDATE url_record_t SET title='%s',last_time=datetime('now') WHERE url='%s'"),sTitleTemp.c_str(),sUrlTemp.c_str());
	if (theApp.m_pBoUsers->execute(sSql)<1)
	{
		sSql.Format(_T("INSERT INTO url_record_t(url,title) VALUES('%s','%s')"),sUrlTemp.c_str(),sTitleTemp.c_str());
		theApp.m_pBoUsers->execute(sSql);
	}

}
void CDlgAppWindow::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	//case TIMER_MOVE_WINDOW:
	//	{
	//		KillTimer(TIMER_MOVE_WINDOW);
	//		CRect rect;
	//		this->GetClientRect(&rect);
	//		m_pCefBrowser.MoveWindow(0,0,rect.Width(),rect.Height());
	//		m_pCefBrowser.SetFocus();
	//	}break;
	//case TIMER_DOWNLOAD_FAVICON:
	//	{
	//		KillTimer(nIDEvent);
	//		if (m_sDownloadFaviconDomainName.IsEmpty() || m_sDownloadFaviconFaviconUrl.IsEmpty())
	//		{
	//			break;
	//		}
	//		const CString sDomainName(m_sDownloadFaviconDomainName);
	//		const CString sFaviconUrl(m_sDownloadFaviconFaviconUrl);
	//		m_sDownloadFaviconDomainName = "";
	//		m_sDownloadFaviconFaviconUrl = "";

	//		CString sFaviconFile;
	//		sFaviconFile.Format(_T("%s\\favicon-%s.ico"),theApp.GetAppImgTempPath(),sDomainName);
	//		const CString sImageTempIniPath = theApp.GetAppImgTempPath()+_T("\\favicon.ini");
	//		char lpszOldLastModified[64];
	//		memset(lpszOldLastModified,0,64);
	//		if (PathFileExists(sFaviconFile))
	//		{
	//			m_sFaviconFile = sFaviconFile;
	//			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)this,(LPARAM)0);
	//			GetPrivateProfileString(_T("last_modified"),sDomainName,_T(""),lpszOldLastModified,64,sImageTempIniPath);
	//		}

	//		CString sNewLastModified;
	//		if (DownloadHttpFile(sFaviconUrl,sFaviconFile,lpszOldLastModified,sNewLastModified))
	//		{
	//			m_sFaviconFile = sFaviconFile;
	//			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)this,(LPARAM)0);
	//			WritePrivateProfileString(_T("last_modified"),sDomainName,sNewLastModified,sImageTempIniPath);
	//		}
	//	}break;
	case TIMER_SHOW_SUB_ID_ICON:
		{
			KillTimer(nIDEvent);
			m_nShowImageType = 3;
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)(CWnd*)this,(LPARAM)0);
		}break;
	case TIMER_SHOW_DEFAULT_PAGE_ICON:
		{
			KillTimer(nIDEvent);
			m_nShowImageType = 1;
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_VIEW_ICO,(WPARAM)(CWnd*)this,(LPARAM)0);
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
				m_pPanelStatus->SetDrawText(m_sStatusMessage.c_str());
			}
		}break;
	case TIMER_SAVE_BROWSE_TITLE:
		{
			KillTimer(TIMER_SAVE_BROWSE_TITLE);
			CString sLocationUrl;
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
			{
#ifdef USES_LIBCEF
				sLocationUrl = m_pCefBrowser.GetLocationURL().c_str();
#endif
			}else
			{
				sLocationUrl = m_pExplorer.get_LocationURL();
			}
			if (sLocationUrl!="about:blank" && (!m_bSaveBrowseInfo || sLocationUrl.Right(1)==_T("/")))
			{
				m_bSaveBrowseInfo = true;
				SaveUrlRecord(sLocationUrl,m_sCurrentTitle);
			}
		}break;
	case TIMER_CHECK_QUIT:
		{
			if (m_nBrowserType==EB_BROWSER_TYPE_CEF)
			{
				KillTimer(nIDEvent);
				this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE,(WPARAM)(CWnd*)this,(LPARAM)m_nUserData);
				return;
			}else
			{
				static int theQuitIndex = 0;
				if ((theQuitIndex++)>100 || m_pExplorer.get_LocationURL()==_T("about:blank"))	// 最长5秒超时，或重新加载 about:blank
				{
					KillTimer(nIDEvent);
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE,(WPARAM)(CWnd*)this,(LPARAM)m_nUserData);
					return;
				}
			}
		}break;
	case TIMER_REDRAW_BROWSER:
		{
			KillTimer(TIMER_REDRAW_BROWSER);
#ifdef USES_LIBCEF
			HWND hWnd = m_pCefBrowser.GetSafeHwnd();
			if (hWnd!=NULL)
			{
				::ShowWindow(hWnd,SW_HIDE);
				::ShowWindow(hWnd,SW_SHOW);
			}

			//CPoint pos;
			//::GetCursorPos(&pos);

			//CefMouseEvent pEvent;
			//pEvent.x = pos.x;
			//pEvent.y = pos.y;
			//m_pCefBrowser.SendMouseMoveEvent(pEvent,false);
			//m_pCefBrowser.SendMouseMoveEvent(pEvent,true);
#endif
		}break;
	default:
		break;
	}
	//}else if (TIMER_SET_TITLE==nIDEvent)
	//{
	//	KillTimer(TIMER_SET_TITLE);
	//	if (!m_webUICtrl.GetTitle().IsEmpty())
	//	{
	//		const CString m_sTitle = m_webUICtrl.GetTitle();
	//		char * lpszBuffer = new char[m_sTitle.GetLength()+1];
	//		strcpy(lpszBuffer,m_sTitle);
	//		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_TITLE,(WPARAM)this,(LPARAM)lpszBuffer);
	//	}
	//}
	CPARENT_DIALOG::OnTimer(nIDEvent);
}

//#include <afxinet.h>
//bool CDlgAppWindow::DownloadHttpFile(const CString& strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified)
//{
//	char     *pBuf   = NULL ;
//	int      nBufLen = 0 ;
//	TRY
//	{
//		// connection
//		CInternetSession   sess ;
//		sess.SetOption (INTERNET_OPTION_CONNECT_TIMEOUT, 10 * 1000) ;
//		sess.SetOption (INTERNET_OPTION_CONNECT_BACKOFF, 1000) ;
//		sess.SetOption (INTERNET_OPTION_CONNECT_RETRIES, 1) ;
//
//		DWORD       dwFlag = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD ;
//		CHttpFile   * pF = (CHttpFile*)sess.OpenURL(strUrl, 1, dwFlag); ASSERT(pF);
//		if (!pF)
//		{
//			return false;
//		}
//		//{AfxThrowInternetException(1);}
//
//		// connection status
//		CString      str ;
//		//pF->QueryInfo (HTTP_QUERY_STATUS_CODE, str) ;
//
//		pF->QueryInfo (HTTP_QUERY_STATUS_CODE, str) ;
//		if (str != _T("200"))
//		{
//			pF->Close() ;
//			delete pF ;
//			return false;
//			//AfxThrowInternetException(1);
//		}
//
//		// confirm update
//		pF->QueryInfo (HTTP_QUERY_LAST_MODIFIED, str);
//		pOutNewLastModified = str;
//		if (strOldLastModified==pOutNewLastModified)
//		{
//			pF->Close() ;
//			delete pF ;
//			return false;
//		}
//
//		/* pF->QueryInfo (HTTP_QUERY_LAST_MODIFIED, str) ;
//		if (!pPara->pThis->df_Notify (GetMsg_CheckTime(), &WM_DLF_CHECKTIME(&pPara->strFileURL, &str)))
//		{
//		pF->Close() ;
//		delete pF ;
//		AfxThrowInternetException(1);
//		}*/
//
//		// start download
//		pF->QueryInfo (HTTP_QUERY_CONTENT_LENGTH, str) ; // file's length
//		//UINT msg = pUpdateDlg->GetMsg_Progress();
//		//::SendMessage(pUpdateDlg->m_hWnd, msg, (WPARAM)pTask, 0);
//		//pPara->pThis->df_Notify (GetMsg_StartDownload(), &pPara->strFileURL) ;
//
//		nBufLen = _ttoi(str);
//		if (nBufLen > 0)
//		{
//			// know file's size
//			//pUpdateDlg->m_nFileSize = nBufLen;
//			//pTask->nTotal = nBufLen;
//			char    * p = (pBuf = new char[nBufLen+8]) ;
//			ZeroMemory (p, nBufLen+8) ;
//
//			int     nLen = nBufLen;
//			while (true)
//			{
//				// download 8K every
//				//DWORD tickNow = GetTickCount();
//				int   n = pF->Read (p, (nLen < 2*1024) ? nLen : 2*1024) ;
//				if (n <= 0)
//					break ; // success exit
//				p += n ; nLen -= n ;
//			}
//
//			// interrupted
//			if (nLen != 0)
//			{
//				delete[] pBuf; pBuf=NULL;
//				nBufLen = 0 ;
//			}
//		}else
//		{
//			// don't know file's size, save context to a temp file.
//			FILE * f = NULL;
//			int      n = (int)pF->GetLength() ;
//			while (n>0)
//				//while (IsWindow(this->GetSafeHwnd()) && n>0)
//			{
//				char   * pa = new char[n+1];
//				n = pF->Read (pa, n) ;
//				if (f==NULL)
//				{
//					f = fopen(strFile,"wb");
//					if (f==NULL)
//					{
//						delete[] pa ;
//						break;
//					}
//				}
//				fwrite(pa,1,n,f);
//				n = (int)pF->GetLength() ;
//				delete[] pa ;
//			}
//			if (f!=NULL)
//			{
//				fclose(f);
//				pF->Close();
//				delete pF ;
//				return true;
//			}
//		}
//		/*outFile.Close() ;
//
//		// success
//		if (n == 0)
//		{
//		DWORD   dw ;
//		if (::InternetQueryDataAvailable ((HINTERNET)(*pF), &dw, 0, 0) && (dw == 0))
//		{
//		//LoadFileToBuffer (strFile, pBuf, nBufLen) ;
//		}
//		}
//		::DeleteFile(strFile) ;
//		}*/
//
//		pF->Close() ;
//		delete pF ;
//	}
//	CATCH_ALL(e) {}
//	END_CATCH_ALL
//
//    if (pBuf)
//    {
//		FILE * f = fopen(strFile,"wb");
//		if (f!=NULL)
//		{
//			fwrite(pBuf,1,nBufLen,f);
//			fclose(f);
//			delete[] pBuf;
//			return true;
//		}
//        delete[] pBuf;
//	}
//	return false;
//}
BOOL CDlgAppWindow::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
//	case WM_ERASEBKGND:
//		{
//#ifdef USES_LIBCEF
//			if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
//			{
//				if (m_pCefBrowser.GetSafeHwnd()!=NULL)
//					retunr 
//			}
//#endif
//		}break;
	case WM_KEYDOWN:
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
				}else if (pMsg->wParam==VK_LEFT)
				{
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP,0,0);
					return TRUE;
				}else if (pMsg->wParam==VK_RIGHT)
				{
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN,0,0);
					return TRUE;
				}
			}
		}break;

//	case WM_MOVING:
//	case WM_MOVE:
//		{
//#ifdef USES_LIBCEF
//			if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
//			{
//				m_pCefBrowser.NotifyMoveOrResizeStarted();
//			}
//#endif
//		}break;
//	case WM_SETFOCUS:
//		{
//#ifdef USES_LIBCEF
//			m_pCefBrowser.SetFocus();
//#endif
//		}break;
	default:
		break;
	}
	return CPARENT_DIALOG::PreTranslateMessage(pMsg);
}

LRESULT CDlgAppWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
//	case WM_MOVING:
//	case WM_MOVE:
//		{
//#ifdef USES_LIBCEF
//			if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
//			{
//				m_pCefBrowser.NotifyMoveOrResizeStarted();
//			}
//#endif
//		}break;
//	case WM_SETFOCUS:
//		{
//#ifdef USES_LIBCEF
//			m_pCefBrowser.SetFocus();
//#endif
//		}break;
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

void CDlgAppWindow::ShowZoomLevel(void)
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
BOOL CDlgAppWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
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

//void CDlgAppWindow::OnDropFiles(HDROP hDropInfo)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	__super::OnDropFiles(hDropInfo);
//}

// ** 会导致工作台没有鼠标提示tooltip
//void CDlgAppWindow::OnPaint()
//{
//	//CPaintDC dc(this); // device context for painting
//	//// TODO: Add your message handler code here
//	//// Do not call CEbDialogBase::OnPaint() for painting messages
//
//	if (IsIconic())
//	{
//		//CPaintDC dc(this); // 用于绘制的设备上下文
//	}
//	else
//	{
//		CPaintDC dc(this); // 用于绘制的设备上下文
//#ifdef USES_EBDIALOGBASE
//		//CRect rectClient;
//		//this->GetClientRect(&rectClient);
//		//CSkinMemDC memDC(&dc, rectClient);
//		//this->ClearBgDrawInfo();
//		//this->DrawPopBg(&memDC, theDefaultBtnWhiteColor,0);
//
//		if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
//		{
//			// *解决界面拖动，浏览器背景灰色问题
//			KillTimer(TIMER_REDRAW_BROWSER);
//			SetTimer(TIMER_REDRAW_BROWSER,20,NULL);
//			//m_pCefBrowser.Invalidate();
//		}
//#endif
//	}
//}

//void CDlgAppWindow::OnMove(int x, int y)
//{
//	__super::OnMove(x, y);
//	if (m_nBrowserType==EB_BROWSER_TYPE_CEF && m_pCefBrowser.GetSafeHwnd()!=NULL)
//	{
//		// *解决界面拖动，浏览器背景灰色问题
//		KillTimer(TIMER_REDRAW_BROWSER);
//		SetTimer(TIMER_REDRAW_BROWSER,20,NULL);
//		//m_pCefBrowser.Invalidate();
//	}
//}
