// DlgAppWindow.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgAppWindow.h"

#define TIMER_CHECK_QUIT			0x101
#define TIMER_SAVE_BROWSE_TITLE		0x102
//#define TIMER_SET_TITLE				0x102
#define EBWM_PROCESS_FAVICON		(WM_USER+100)

// CDlgAppWindow dialog

IMPLEMENT_DYNAMIC(CDlgAppWindow, CEbDialogBase)

CDlgAppWindow::CDlgAppWindow(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgAppWindow::IDD, pParent)
	, m_webUICtrl(this)
{
	m_bDocumentComplete = false;
	m_bOpenNewClose = false;
	m_bQuit = false;
	m_bSaveBrowseInfo = false;
}

CDlgAppWindow::~CDlgAppWindow()
{
}

void CDlgAppWindow::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_pExplorer);
	DDX_Control(pDX, IDC_STATIC_STATUS_TEXT, m_labelStatusText);
}


BEGIN_MESSAGE_MAP(CDlgAppWindow, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(EBWM_PROCESS_FAVICON, OnMsgProcessFavicon)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgAppWindow, CEbDialogBase)
	//{{AFX_EVENTSINK_MAP(CDlgFuncWindow)
	ON_EVENT(CDlgAppWindow, IDC_EXPLORER1, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//DISPID_BEFORENAVIGATE
	//DISPID_NEWWINDOW
	//DISPID_NEWWINDOW2
	
	//ON_EVENT(CDlgFuncWindow, IDC_EXPLORER1, DISPID_NEWWINDOW2, NewWindow2, VTS_PDISPATCH VTS_PBOOL)
	//ON_EVENT(CDlgAppWindow, IDC_EXPLORER1, DISPID_NEWWINDOW3, NewWindow3, VTS_PDISPATCH VTS_PBOOL VTS_I4 VTS_BSTR VTS_BSTR)
	//ON_EVENT(CDlgAppWindow,IDC_EXPLORER1,DISPID_TITLECHANGE,OnTitleChange,VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CDlgAppWindow,IDC_EXPLORER1,259,OnDocumentComplete,VTS_DISPATCH VTS_PVARIANT)
	//ON_EVENT(CDlgFuncWindow,IDC_EXPLORER1,113,OnTitleChange,VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// CDlgAppWindow message handlers

BOOL CDlgAppWindow::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	//SetTransparentType(CEbDialogBase::TT_STATIC);

	m_pExplorer.ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	// 先显示空白页，用于刷新修改IE控件。
	//m_bDisableContextMenu = FALSE;
	m_webUICtrl.EnableContextMenu(m_pFuncInfo.m_bDisableContextMenu?0:1);
	m_webUICtrl.EnableScrollBar(m_pFuncInfo.m_bDisableScrollBar?0:1);
	m_webUICtrl.SetWebBrowser(&m_pExplorer);
	//m_pExplorer.SetFocus();
	m_pExplorer.Navigate(_T("about:blank"),NULL,NULL,NULL,NULL);

	m_labelStatusText.SetWindowText(_T(""));
	m_labelStatusText.ShowWindow(SW_HIDE);
	m_labelStatusText.SetTextColor(RGB(61,61,61));
	m_labelStatusText.SetAlignText(CLabelEx::Align_Left);

	this->SetCircle(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAppWindow::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgAppWindow::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgAppWindow::OnDestroy()
{
	m_webUICtrl.ClearWebBrowser();
	CEbDialogBase::OnDestroy();
}

void CDlgAppWindow::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	if (m_pExplorer.GetSafeHwnd()!=NULL)
	{
		m_pExplorer.MoveWindow(0,0,cx,cy);
	}
	if (m_labelStatusText.GetSafeHwnd())
		m_labelStatusText.MoveWindow(3,cy-24,cx-6,24);
}

void CDlgAppWindow::Navigate(const std::string& sUrl)
{
	if (!m_bDocumentComplete)
		m_sToNavigateUrl = sUrl;
	else if (m_pExplorer.GetSafeHwnd()!=NULL)
		m_pExplorer.Navigate(sUrl.c_str(),NULL,NULL,NULL,NULL);
}

CString CDlgAppWindow::GetLocationURL(void)
{
	return m_pExplorer.get_LocationURL();
}

void CDlgAppWindow::doRefresh(void)
{
	if (m_bDocumentComplete)
	{
		try
		{
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
void CDlgAppWindow::goBack(void)
{
	if (m_bDocumentComplete)
	{
		try
		{
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
			m_pExplorer.GoForward();
		}catch(...)
		{
		}
	}
}
void CDlgAppWindow::SetWebFocus(void)
{
	m_pExplorer.SetFocus();
}

void CDlgAppWindow::InvalidateRect2(LPRECT lpRect, BOOL bErase)
{
	if (m_pExplorer.GetSafeHwnd()!=0)
		m_pExplorer.InvalidateRect(lpRect,bErase);
}

void CDlgAppWindow::OnLogonSuccess(void)
{
ExecuteJavaScript
}

void CDlgAppWindow::OnStatusTextChange(BSTR sStatusText)
{
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
//		const std::string sUrl(W2T(suURL.pwcsUrl));
//		if (suURL.pwcsTitle!=0 && sUrl.substr(0,4)=="http")
//			int i=0;
//		printf("%s ", W2T(suURL.pwcsUrl));
//	}
//
//	pEnumURL->Release();
//	pUrlHistoryStg2->Release();
//	CoUninitialize();
//}
LRESULT CDlgAppWindow::OnMsgProcessFavicon(WPARAM wParam, LPARAM lParam)
{
	const bool bOnDocumentComplete = wParam==1?true:false;
	// <link rel="shortcut icon" href="/favicon.ico" />
	// <link href="https://kryptoners.b0.upaiyun.com/assets/favicon-6523a7e532a9862be505b7657fd7ddb7.ico" rel="shortcut icon" type="image/vnd.microsoft.icon" />

	// http://www.entboost.com/favicon.ico
	const CString sLocationUrl(m_pExplorer.get_LocationURL());
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
		CString sFaviconUrl(m_webUICtrl.GetFaviconUrl());
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
		// 
		CString sFaviconFile;
		sFaviconFile.Format(_T("%s\\favicon-%s.ico"),theApp.GetAppImgTempPath(),sDomainName);
		const CString sImageTempIniPath = theApp.GetAppImgTempPath()+_T("\\favicon.ini");
		char lpszOldLastModified[64];
		memset(lpszOldLastModified,0,64);
		if (PathFileExists(sFaviconFile))
		{
			m_sFaviconFile = sFaviconFile;
			this->GetParent()->PostMessage(EBWM_FRAME_VIEW_ICO,(WPARAM)this,(LPARAM)0);
			GetPrivateProfileString(_T("last_modified"),sDomainName,_T(""),lpszOldLastModified,64,sImageTempIniPath);
		}
		CString sNewLastModified;
		if (DownloadHttpFile(sFaviconUrl,sFaviconFile,lpszOldLastModified,sNewLastModified))
		{
			m_sFaviconFile = sFaviconFile;
			this->GetParent()->PostMessage(EBWM_FRAME_VIEW_ICO,(WPARAM)this,(LPARAM)0);
			WritePrivateProfileString(_T("last_modified"),sDomainName,sNewLastModified,sImageTempIniPath);
		}
	}
	return 0;
}

void CDlgAppWindow::OnTitleChange(BSTR sTitle)
{
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
			this->GetParent()->PostMessage(EBWM_FRAME_WND_TITLE,(WPARAM)this,(LPARAM)lpszBuffer);

			const CString sLocationUrl(m_pExplorer.get_LocationURL());
			if (m_sCurrentTitle!="about:blank" && (!m_bSaveBrowseInfo || sLocationUrl.Right(1)==_T("/")))
			{
				this->PostMessage(EBWM_PROCESS_FAVICON,0,0);
				if (m_pFuncInfo.m_nSubscribeId==0)
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
	const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
	if (!bControl)
	{
		//const CString csURLContext(bstrUrlContext);
		const CString csURL(bstrUrl);
		char * lpszBuffer = new char[csURL.GetLength()+1];
		strcpy(lpszBuffer,csURL);
		*Cancel = VARIANT_TRUE;
		this->GetParent()->PostMessage(EBWM_FRAME_WND_OPEN,(WPARAM)lpszBuffer,(LPARAM)0);
	}
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
void CDlgAppWindow::OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL)
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
			this->PostMessage(WM_CLOSE, 0, 0);
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
			this->PostMessage(WM_CLOSE, 0, 0);
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
				this->PostMessage(WM_CLOSE, 0, 0);
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
				this->PostMessage(WM_CLOSE, 0, 0);
		}
		return;
	}
	pos = csURL.Find(theClose, 0);
	if( pos != -1 )
	{
		*bCancel = VARIANT_TRUE;	
		this->PostMessage(WM_CLOSE, 0, 0);
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
		int nParam = 0;
		if (nDisableContextMenu==1)
			nParam |= 0x1;
		if (nDisableScrollBar==1)
			nParam |= 0x2;
		if (nClosePrompt==1)
			nParam |= 0x4;
		if (nOpenNewClose==1)
			nParam |= 0x8;
		char * lpszBuffer = new char[sWindowUrl.GetLength()+1];
		strcpy(lpszBuffer,sWindowUrl);
		this->GetParent()->PostMessage(EBWM_FRAME_WND_OPEN,(WPARAM)lpszBuffer,(LPARAM)nParam);
		//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		//CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		////pFuncWindow->m_sTitle = lpszTitle;
		//pFuncWindow->m_sFuncUrl = (LPCTSTR)sWindowUrl;
		//pFuncWindow->m_nWidth = nWidth;
		//pFuncWindow->m_nHeight = nHeight;
		//pFuncWindow->m_bDisableContextMenu = nDisableContextMenu==1?true:false;
		//pFuncWindow->m_bDisableScrollBar = nDisableScrollBar==1?true:false;
		//pFuncWindow->m_bClosePrompt = nClosePrompt==1?true:false;
		//pFuncWindow->m_bOpenNewClose = nOpenNewClose==1?true:false;
		//pFuncWindow->m_bWinResizable = bWinResizable==1?true:false;
		//pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
		//pFuncWindow->ShowWindow(SW_SHOW);
		if (m_bOpenNewClose)
			this->PostMessage(WM_CLOSE, 0, 0);
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
		char * lpszBuffer = new char[sWindowUrl.GetLength()+1];
		strcpy(lpszBuffer,sWindowUrl);
		this->GetParent()->PostMessage(EBWM_FRAME_WND_OPEN,(WPARAM)lpszBuffer,(LPARAM)0);

		//CWnd * pParent = CWnd::FromHandle(::GetDesktopWindow());
		//CDlgFuncWindow * pFuncWindow = new CDlgFuncWindow(pParent,true);
		//pFuncWindow->m_sFuncUrl = sWindowUrl;
		//pFuncWindow->m_nWidth = nWidth;
		//pFuncWindow->m_nHeight = nHeight;
		//pFuncWindow->m_bDisableContextMenu = true;
		//pFuncWindow->m_bDisableScrollBar = true;
		//pFuncWindow->m_bClosePrompt = false;
		//pFuncWindow->Create(CDlgFuncWindow::IDD,pParent);
		//pFuncWindow->ShowWindow(SW_SHOW);
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
//	//this->GetParent()->SendMessage(EBWM_FRAME_WND_OPEN,(WPARAM)(LPCTSTR)csURL,(LPARAM)0);
//}
void CDlgAppWindow::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, VARIANT_BOOL FAR* Cancel) 
{
	OnBeforeNavigate(Cancel,(CString)URL->bstrVal);
}
//void CDlgAppWindow::OnTitleChange(LPDISPATCH pDisp, VARIANT* pTitle)
//{
//	//if (!m_bQuit)
//	//{
//	//	const CString m_sTitle(pTitle->bstrVal);
//	//	char * lpszBuffer = new char[m_sTitle.GetLength()+1];
//	//	strcpy(lpszBuffer,m_sTitle);
//	//	this->GetParent()->PostMessage(EBWM_FRAME_WND_TITLE,(WPARAM)this,(LPARAM)lpszBuffer);
//	//}
//}
void CDlgAppWindow::OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if (!m_bDocumentComplete)
	{
		m_bDocumentComplete = true;
		m_pExplorer.SetFocus();

		if (!m_sToNavigateUrl.empty())
		{
			m_pExplorer.Navigate(m_sToNavigateUrl.c_str(),NULL,NULL,NULL,NULL);
			m_sToNavigateUrl.clear();
		}
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
	if (m_pFuncInfo.m_bClosePrompt &&				// 配置关闭提示
		CDlgMessageBox::EbDoModal(this,_T("你确定退出当前应用窗口吗？"),_T(""),CDlgMessageBox::IMAGE_QUESTION)!=IDOK)
	{
		return;
	}
	m_bQuit = true;
	if (m_bDocumentComplete)
	{
		// ** 解决某些页面，未加载完成（忙，如正在加载flash等），退出窗口会报异常问题；
		m_pExplorer.Navigate(_T("about:blank"),NULL,NULL,NULL,NULL);
		SetTimer(TIMER_CHECK_QUIT,100,NULL);
	}else
	{
		this->GetParent()->PostMessage(EBWM_FRAME_WND_CLOSE,(WPARAM)this,0);
	}
	//CEbDialogBase::OnClose();
}

void SaveUrlRecord(const char* sUrl, const char* sTitle)
{
	std::string sUrlTemp(sUrl);
	std::string sTitleTemp(sTitle);
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
	if (TIMER_SAVE_BROWSE_TITLE==nIDEvent)
	{
		KillTimer(TIMER_SAVE_BROWSE_TITLE);
		const CString sLocationUrl(m_pExplorer.get_LocationURL());
		if (sLocationUrl!="about:blank" && (!m_bSaveBrowseInfo || sLocationUrl.Right(1)==_T("/")))
		{
			m_bSaveBrowseInfo = true;
			SaveUrlRecord(sLocationUrl,m_sCurrentTitle);
		}
	}else if (TIMER_CHECK_QUIT==nIDEvent)
	{
		static int theQuitIndex = 0;
		if ((theQuitIndex++)>50 || m_pExplorer.get_LocationURL()==_T("about:blank"))	// 最长5秒超时，或重新加载 about:blank
		{
			KillTimer(nIDEvent);
			this->GetParent()->PostMessage(EBWM_FRAME_WND_CLOSE,(WPARAM)this,0);
			return;
		}
	//}else if (TIMER_SET_TITLE==nIDEvent)
	//{
	//	KillTimer(TIMER_SET_TITLE);
	//	if (!m_webUICtrl.GetTitle().IsEmpty())
	//	{
	//		const CString m_sTitle = m_webUICtrl.GetTitle();
	//		char * lpszBuffer = new char[m_sTitle.GetLength()+1];
	//		strcpy(lpszBuffer,m_sTitle);
	//		this->GetParent()->PostMessage(EBWM_FRAME_WND_TITLE,(WPARAM)this,(LPARAM)lpszBuffer);
	//	}
	}
	CEbDialogBase::OnTimer(nIDEvent);
}

#include <afxinet.h>
bool CDlgAppWindow::DownloadHttpFile(const CString& strUrl,const CString& strFile,const CString& strOldLastModified,CString& pOutNewLastModified)
{
	char     *pBuf   = NULL ;
	int      nBufLen = 0 ;
	TRY
	{
		// connection
		CInternetSession   sess ;
		sess.SetOption (INTERNET_OPTION_CONNECT_TIMEOUT, 10 * 1000) ;
		sess.SetOption (INTERNET_OPTION_CONNECT_BACKOFF, 1000) ;
		sess.SetOption (INTERNET_OPTION_CONNECT_RETRIES, 1) ;

		DWORD       dwFlag = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD ;
		CHttpFile   * pF = (CHttpFile*)sess.OpenURL(strUrl, 1, dwFlag); ASSERT(pF);
		if (!pF)
		{
			return false;
		}
		//{AfxThrowInternetException(1);}

		// connection status
		CString      str ;
		//pF->QueryInfo (HTTP_QUERY_STATUS_CODE, str) ;

		pF->QueryInfo (HTTP_QUERY_STATUS_CODE, str) ;
		if (str != _T("200"))
		{
			pF->Close() ;
			delete pF ;
			return false;
			//AfxThrowInternetException(1);
		}

		// confirm update
		pF->QueryInfo (HTTP_QUERY_LAST_MODIFIED, str);
		pOutNewLastModified = str;
		if (strOldLastModified==pOutNewLastModified)
		{
			pF->Close() ;
			delete pF ;
			return false;
		}

		/* pF->QueryInfo (HTTP_QUERY_LAST_MODIFIED, str) ;
		if (!pPara->pThis->df_Notify (GetMsg_CheckTime(), &WM_DLF_CHECKTIME(&pPara->strFileURL, &str)))
		{
		pF->Close() ;
		delete pF ;
		AfxThrowInternetException(1);
		}*/

		// start download
		pF->QueryInfo (HTTP_QUERY_CONTENT_LENGTH, str) ; // file's length
		//UINT msg = pUpdateDlg->GetMsg_Progress();
		//::SendMessage(pUpdateDlg->m_hWnd, msg, (WPARAM)pTask, 0);
		//pPara->pThis->df_Notify (GetMsg_StartDownload(), &pPara->strFileURL) ;

		nBufLen = _ttoi(str);
		if (nBufLen > 0)
		{
			// know file's size
			//pUpdateDlg->m_nFileSize = nBufLen;
			//pTask->nTotal = nBufLen;
			char    * p = (pBuf = new char[nBufLen+8]) ;
			ZeroMemory (p, nBufLen+8) ;

			int     nLen = nBufLen;
			while (true)
			{
				// download 8K every
				//DWORD tickNow = GetTickCount();
				int   n = pF->Read (p, (nLen < 2*1024) ? nLen : 2*1024) ;
				if (n <= 0)
					break ; // success exit
				p += n ; nLen -= n ;
			}

			// interrupted
			if (nLen != 0)
			{
				delete[] pBuf; pBuf=NULL;
				nBufLen = 0 ;
			}
		}
		/*else
		{
		// don't know file's size, save context to a temp file.
		bstr_t   strFile = QueryTempFilePath() ;
		CFile    outFile (strFile, CFile::modeCreate|CFile::modeReadWrite) ;
		int      n = (int)pF->GetLength() ;
		while (IsWindow(pUpdateDlg->GetSafeHwnd()) && n)
		{
		char   * pa = new char[n] ;
		n = pF->Read (pa, n) ;
		outFile.Write (pa, n) ;
		//pPara->pThis->df_Notify (GetMsg_Progress(), &WM_DLF_PROGRESS(&pPara->strFileURL, (int)outFile.GetLength(), 0)) ;
		n = (int)pF->GetLength() ;
		delete[] pa ;
		}
		outFile.Close() ;

		// success
		if (n == 0)
		{
		DWORD   dw ;
		if (::InternetQueryDataAvailable ((HINTERNET)(*pF), &dw, 0, 0) && (dw == 0))
		{
		//LoadFileToBuffer (strFile, pBuf, nBufLen) ;
		}
		}
		::DeleteFile(strFile) ;
		}*/

		pF->Close() ;
		delete pF ;
	}
	CATCH_ALL(e) {}
	END_CATCH_ALL

    if (pBuf)
    {
		FILE * f = fopen(strFile,"wb");
		if (f!=NULL)
		{
			fwrite(pBuf,1,nBufLen,f);
			fclose(f);
			delete[] pBuf;
			return true;
		}
        delete[] pBuf;
	}
	return false;
}
