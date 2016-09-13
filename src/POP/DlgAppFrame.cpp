// DlgAppFrame.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgAppFrame.h"
#include "DlgAppWindow.h"

#define TIMERID_CHECK_MOUSE_POS			0x130
#define TIMERID_MOVE_WINDOW_SIZE		0x131
#define TIMERID_SET_CURRENT_FOCUS		0x132

#define FRAME_BTN_ID_MIN		101
#define FRAME_BTN_ID_MAX		200

const int const_frame_tool = 29;
const int const_frame_width = 140;	// 118
const int const_frame_height = 30;

CAppWindowInfo::CAppWindowInfo(const std::string& sAppUrl)
: m_sAppUrl(sAppUrl)
, m_pAppWindow(NULL)
, m_nUserData(0)

{

}
CAppWindowInfo::~CAppWindowInfo(void)
{
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.DestroyWindow();
	if (m_close.GetSafeHwnd()!=NULL)
		m_close.DestroyWindow();

	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->DestroyWindow();
		delete m_pAppWindow;
		m_pAppWindow = NULL;
	}
}

void CAppWindowInfo::SetAppUrl(bool bOpenAppUrlInfo, const std::string& sUrl, const std::string& sPostData)
{
	if (m_sAppUrl != sUrl)
	{
		m_sAppUrl = sUrl;
	}
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->SetSaveBrowseTitle(bOpenAppUrlInfo);
		m_pAppWindow->Navigate(sUrl, sPostData);
	}
}

CString CAppWindowInfo::GetLocationURL(void) const
{
	return m_pAppWindow==NULL?m_sAppUrl.c_str():m_pAppWindow->GetLocationURL();
}

bool CAppWindowInfo::IsAboutBlank(void) const
{
	const CString sUrl = GetLocationURL();
	return sUrl==_T("about:blank");
}

eb::bigint CAppWindowInfo::GetSubscribeId(void) const
{
	return m_pAppWindow==NULL?0:m_pAppWindow->m_pFuncInfo.m_nSubscribeId;
}
eb::bigint CAppWindowInfo::GetFromSubscribeId(void) const
{
	return m_pAppWindow==NULL?0:m_pAppWindow->m_pFuncInfo.m_nFromSubscribeId;
}

BOOL CAppWindowInfo::Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID, CFont * pFont, const EB_SubscribeFuncInfo& pFuncInfo, const std::string& sPostData, bool nOpenNewClose, int nLeft, bool bSaveBrowseTitle)
{
	m_rectBtn = rect;
	if (m_btn.GetSafeHwnd()==NULL)
	{
		CString sWindowText;
		const size_t nLen = strlen(lpszCaption);
		for (size_t i=0;i<nLen;i++)
		{
			sWindowText.AppendChar(lpszCaption[i]);
			if ((i+1)<nLen && lpszCaption[i]<0)	// 中文，需要取二个
			{
				sWindowText.AppendChar(lpszCaption[++i]);
			}
			if (i>=13 && (i+1)<nLen)
			{
				sWindowText.Append(_T("..."));
				break;
			}
		}
		if (sWindowText.IsEmpty() || sWindowText==_T("about:blank"))
		{
			sWindowText = _T("新标签页");
		}
		if (!m_btn.Create(sWindowText,WS_CHILD|WS_VISIBLE,rect,pParent,nID))
			return FALSE;
		m_btn.SetToolTipText(lpszCaption);
		if (pFont!=NULL)
			m_btn.SetFont(pFont);
		//m_btn.SetFont(pParent->GetFont());
		m_btn.SetTextTop(1);
		m_btn.SetTextLeft(8);
		m_btn.SetFormat(DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		m_btn.SetTextHotMove(false);
		m_btn.SetDrawPanelRgn(false);
		m_btn.SetNorTextColor(theDefaultFlatBlackText2Color);
		m_btn.SetPreTextColor(RGB(0,0,0));	// theDefaultTextBlackColor
		//m_btn.SetHotTextColor(theDefaultFlatBlackTextColor);
		SetCtrlColor();
	}else
	{
		m_btn.SetParent(pParent);
		m_btn.SetOwner(pParent);
		m_btn.SetDlgCtrlID(nID);
		m_btn.MoveWindow(&m_rectBtn);
	}

	const int const_size = 12;
	m_rectClose.right = m_rectBtn.right-3;
	m_rectClose.left = m_rectClose.right - const_size;
	m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2;
	m_rectClose.bottom = m_rectClose.top+const_size;
	if (m_close.GetSafeHwnd()==NULL)
	{
		if (!m_close.Create(_T(""),WS_CHILD,m_rectClose,pParent,nID))
			return FALSE;
		m_close.SetToolTipText(_T("关闭"));
		m_close.SetDrawPanelRgn(false);
		//m_close.SetDrawClosePic(true,RGB(27,27,27),RGB(237,28,36),-1,-1,2);
	}else
	{
		m_close.SetParent(pParent);
		m_close.SetOwner(pParent);
		m_close.SetDlgCtrlID(nID);
		m_close.MoveWindow(&m_rectClose);
	}
	CRect rectAppWindow;
	pParent->GetClientRect(&rectAppWindow);
	rectAppWindow.left = nLeft;
	rectAppWindow.top += m_rectBtn.Height();
//#ifdef USES_NEW_UI_1220
//	rectAppWindow.top += 1;
//#endif

	if (m_pAppWindow==NULL)
	{
		m_pAppWindow = new CDlgAppWindow(pParent);
		m_pAppWindow->SetSaveBrowseTitle(bSaveBrowseTitle);
		m_pAppWindow->m_pFuncInfo = pFuncInfo;
		m_pAppWindow->m_bOpenNewClose = nOpenNewClose;
		m_pAppWindow->Navigate(m_sAppUrl, sPostData);
		if (!m_pAppWindow->Create(CDlgAppWindow::IDD,pParent))
		{
			delete m_pAppWindow;
			m_pAppWindow = NULL;
			return FALSE;
		}
	}else
	{
		m_pAppWindow->SetParent(pParent);
		m_pAppWindow->SetOwner(pParent);
	}
	m_pAppWindow->MoveWindow(&rectAppWindow);
	m_pAppWindow->ShowWindow(SW_SHOW);
	return TRUE;
}
//void CAppWindowInfo::Create(const RECT& rect, CWnd* pParent, UINT nID, CFont * pFont, int nLeft)
//{
//	m_rectBtn = rect;
//	if (m_btn.GetSafeHwnd()!=NULL)
//	{
//		m_btn.SetParent(pParent);
//		m_btn.SetOwner(pParent);
//		m_btn.SetDlgCtrlID(nID);
//		m_btn.MoveWindow(&m_rectBtn);
//	}
//
//	const int const_size = 12;
//	m_rectClose.right = m_rectBtn.right-3;
//	m_rectClose.left = m_rectClose.right - const_size;
//	m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2;
//	m_rectClose.bottom = m_rectClose.top+const_size;
//	if (m_close.GetSafeHwnd()!=NULL)
//	{
//		m_close.SetParent(pParent);
//		m_close.SetOwner(pParent);
//		m_close.SetDlgCtrlID(nID);
//		m_close.MoveWindow(&m_rectClose);
//	}
//
//	if (m_pAppWindow!=NULL)
//	{
//		CRect rectAppWindow;
//		pParent->GetClientRect(&rectAppWindow);
//		rectAppWindow.left = nLeft;
//		rectAppWindow.top += m_rectBtn.Height();
//		//#ifdef USES_NEW_UI_1220
//		//	rectAppWindow.top += 1;
//		//#endif
//		m_pAppWindow->SetParent(pParent);
//		m_pAppWindow->SetOwner(pParent);
//		m_pAppWindow->MoveWindow(&rectAppWindow);
//	}
//}

bool CAppWindowInfo::IsAppWindow(HWND hWnd) const
{
	return (m_pAppWindow!=NULL&&m_pAppWindow->GetSafeHwnd()==hWnd)?true:false;
}

bool CAppWindowInfo::IsCheckedFocus(void)
{
	if (m_btn.GetChecked())
	{
		if (m_pAppWindow!=NULL)
		{
			m_pAppWindow->SetWebFocus(true);
		}
		return true;
	}
	return false;
}
bool CAppWindowInfo::IsCheckedRefresh(void)
{
	if (m_btn.GetChecked())
	{
		if (m_pAppWindow!=NULL)
		{
			m_pAppWindow->SetWebFocus(true);
			//m_pAppWindow->ShowWindow(SW_HIDE);
			//m_pAppWindow->ShowWindow(SW_SHOW);
			//m_pAppWindow->PostMessage(POP_WM_WINDOW_SIZE);
			////m_pAppWindow->Invalidate();
		}
		return true;
	}
	return false;
}
bool CAppWindowInfo::NotifyMoveOrResizeStarted(void)
{
	if (m_btn.GetChecked())
	{
		if (m_pAppWindow!=NULL)
		{
			m_pAppWindow->NotifyMoveOrResizeStarted();
		}
		return true;
	}
	return false;
}
void CAppWindowInfo::OnLogonSuccess(void)
{
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->OnLogonSuccess();
	}
}
void CAppWindowInfo::OnOffline(int nServerState)
{
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->OnOffline(nServerState);
	}
}

void CAppWindowInfo::MoveSize(int left, int cx, int cy)
{
	if (m_pAppWindow!=NULL)
	{
//#ifdef USES_NEW_UI_1220
//		CRect rectAppWindow(left,m_rectBtn.Height()+1,cx,cy);
//#else
		CRect rectAppWindow(left,m_rectBtn.Height(),cx,cy);
//#endif
		m_pAppWindow->MoveWindow(&rectAppWindow);
	}
}
void CAppWindowInfo::SetCtrlColor(void)
{
	if (m_btn.GetSafeHwnd()!=NULL)
	{
#ifdef USES_NEW_UI_1220
		m_btn.SetHotTextColor(theApp.GetNormalColor());
		const int nLineWidth = m_rectBtn.Width()-6;
		m_btn.SetDrawLine(2,nLineWidth,2,theDefaultFlatLineColor,theApp.GetHotColor(),theApp.GetMainColor(),-1,1,theDefaultFlatLineColor);
		m_btn.SetDrawLineNorExt(0,1);
		m_btn.SetDrawPanel(true,theDefaultFlatBgColor,theDefaultBtnWhiteColor,theDefaultBtnWhiteColor);
#else
		m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetBgTitle0(),theApp.GetBgColor3());
#endif
	}
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->SetCtrlColor(true);
	}
}

void CAppWindowInfo::SetFunInfo(const EB_SubscribeFuncInfo& pFuncInfo, bool bOpenNewClose)
{
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.SetWindowText(pFuncInfo.m_sFunctionName.c_str());
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->m_pFuncInfo = pFuncInfo;
		m_pAppWindow->m_bOpenNewClose = bOpenNewClose;
	}
}

//void CAppWindowInfo::RefreshWindow(void)
//{
//	if (m_pAppWindow!=NULL)
//	{
//		// ***解决WEB控件背景不能刷新问题
//		//m_pAppWindow->RedrawWindow();	// **
//		//m_pAppWindow->Refresh();
//	}
//}
void CAppWindowInfo::CheckMousePos(const POINT& point){
	if (m_btn.GetSafeHwnd()==NULL || m_close.GetSafeHwnd()==NULL)
		return;
	if (m_rectBtn.PtInRect(point))
	{
		if (m_rectClose.PtInRect(point))
		{
			//if (m_btn.GetChecked())
			//	m_close.SetDrawClosePic(true,theApp.GetBgColor3(),-1,-1,-1,2);
			//else
			//	m_close.SetDrawClosePic(true,theApp.GetBgTitle0(),-1,-1,-1,2);
			m_close.SetDrawClosePic(true,RGB(250,250,250),-1,-1,-1,2);
			m_close.SetDrawPanel(true,theDefaultBtnCloseColor);
		}else
		{
			m_close.SetDrawClosePic(true,RGB(27,27,27),-1,-1,-1,2);
#ifdef USES_NEW_UI_1220
			m_close.SetDrawPanel(true,theDefaultBtnWhiteColor);
#else
			if (m_btn.GetChecked())
				m_close.SetDrawPanel(true,theApp.GetBgColor3());
			else
				m_close.SetDrawPanel(true,theApp.GetBgTitle0());
#endif
		}
		m_close.ShowWindow(SW_SHOW);
		m_close.Invalidate();
	}else if (m_close.IsWindowVisible())
	{
		m_close.ShowWindow(SW_HIDE);
	}
}
void CAppWindowInfo::SendClose(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->PostMessage(WM_CLOSE, 0, 0);
}

void CAppWindowInfo::ShowHide(bool bShowAndChecked)
{
	if (m_pAppWindow!=NULL)
	{
		//m_pAppWindow->OnShowHide(bShowAndChecked);
		m_pAppWindow->ShowWindow(bShowAndChecked?SW_SHOW:SW_HIDE);
		if (bShowAndChecked)
			m_pAppWindow->SetWebFocus(true);
	}
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		if (m_btn.GetChecked()!=bShowAndChecked)
		{
			m_btn.SetChecked(bShowAndChecked);
			m_btn.Invalidate();
		}
	}
}
void CAppWindowInfo::SetTitle(const CString& sTitle)
{
	if (m_btn.GetSafeHwnd()!=NULL)
	{
		CString sTooltipText;
		if (sTitle.IsEmpty() || sTitle==_T("about:blank"))
		{
			sTooltipText = _T("新标签页");
			if (m_pAppWindow==NULL || m_pAppWindow->m_pFuncInfo.m_nSubscribeId==0 || m_pAppWindow->m_pFuncInfo.m_sFunctionName.empty())
				m_btn.SetWindowText(sTooltipText);
		//}else if (m_pAppWindow!=NULL && m_pAppWindow->m_pFuncInfo.m_nSubscribeId>0 && !m_pAppWindow->m_pFuncInfo.m_sFunctionName.empty())
		//{
		//	//m_btn.SetWindowText(m_pAppWindow->m_pFuncInfo.m_sFunctionName.c_str());
		//	sTooltipText.Format(_T("%s[%s]"),m_pAppWindow->m_pFuncInfo.m_sFunctionName.c_str(),sTitle);
		}else
		{
			CString sWindowText;
			const size_t nLen = sTitle.GetLength();
			for (size_t i=0;i<nLen;i++)
			{
				sWindowText.AppendChar(sTitle.GetAt(i));
				if ((i+1)<nLen && sTitle.GetAt(i)<0)	// 中文，需要取二个
				{
					sWindowText.AppendChar(sTitle.GetAt(++i));
				}
				if (i>=13 && (i+1)<nLen)
				{
					sWindowText.Append(_T("..."));
					break;
				}
			}
			m_btn.SetWindowText(sWindowText);
			sTooltipText = sTitle;
		}
		if (sTooltipText.GetLength()>1024)
			sTooltipText = sTooltipText.Left(1024);
		m_btn.SetToolTipText(sTooltipText);
		//m_btn.Invalidate();
	}
}
void CAppWindowInfo::SetIco(void)
{
	if (m_pAppWindow==NULL)
		return;

	//const CString sFaviconFile = theApp.GetAppPath() + "\\img\\loading.gif";	// for test
	const CString sFaviconFile = m_pAppWindow->GetFaviconFile();
	if (PathFileExists(sFaviconFile))
	{
		USES_CONVERSION;
		Gdiplus::Image* pImage = libEbc::LoadImageFromFile(sFaviconFile);
		if (pImage == NULL)
			pImage = Gdiplus::Image::FromFile(A2W_ACP(sFaviconFile));
		if (pImage!=0)
		{
			m_btn.SetDrawImage(true,false,pImage,Gdiplus::Rect(4,7,16,16));
			m_btn.SetTextLeft(24);
			//m_btn.SetTextLeft(20);	// 20=4+18-2
		}
	}else
	{
			m_btn.SetDrawImage(true,false,NULL,Gdiplus::Rect(4,7,16,16));
			m_btn.SetTextLeft(8);
	}
}

void CAppWindowInfo::doRefresh(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->doRefresh();
}
void CAppWindowInfo::goBack(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->goBack();
}
void CAppWindowInfo::goForward(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->goForward();
}
void CAppWindowInfo::changeBrowserType(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->changeBrowserType();
}

EB_BROWSER_TYPE CAppWindowInfo::queryBrowserType(void) const
{
	return m_pAppWindow!=NULL?m_pAppWindow->GetBrowserType():EB_BROWSER_TYPE_CEF;
}
bool CAppWindowInfo::canSaveHistory(void) const
{
	if (m_pAppWindow!=NULL)
		return m_pAppWindow->canSaveHistory();
	return false;
}
void CAppWindowInfo::saveHistory(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->saveHistory();
}

// CDlgAppFrame dialog

IMPLEMENT_DYNAMIC(CDlgAppFrame, CEbDialogBase)

CDlgAppFrame::CDlgAppFrame(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgAppFrame::IDD, pParent)
{
	for (int i=FRAME_BTN_ID_MIN;i<=FRAME_BTN_ID_MAX; i++)
		m_pIndexIdList.add(i);

	m_bShowedToolbar = true;
}

CDlgAppFrame::~CDlgAppFrame()
{
}

void CDlgAppFrame::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON_MIN, m_btnMin);
	//DDX_Control(pDX, IDC_BUTTON_MAX, m_btnMax);
	//DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_FRAME, m_btnSwitchFrame);
	DDX_Control(pDX, IDC_BUTTON_SWITCH_TOOLBAR, m_btnSwitchToolbar);
	DDX_Control(pDX, IDC_BUTTON_NEW_WEB, m_btnNewWeb);
}


BEGIN_MESSAGE_MAP(CDlgAppFrame, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_FRAME, &CDlgAppFrame::OnBnClickedButtonSwitchFrame)
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND_RANGE(FRAME_BTN_ID_MIN, FRAME_BTN_ID_MAX, OnFrameBtnClicked)
	ON_COMMAND_RANGE(EB_COMMAND_MAINFRAME_FUNC,EB_COMMAND_MAINFRAME_FUNC+0x20,OnMainFrameFunc)
	ON_MESSAGE(EB_COMMAND_RAME_WND_CLOSE, OnFrameWndClose)
	ON_MESSAGE(EB_COMMAND_RAME_WND_TITLE, OnFrameWndTitle)
	//ON_MESSAGE(EB_COMMAND_RAME_WND_OPEN, OnFrameWndOpen)
	ON_MESSAGE(EB_COMMAND_RAME_WND_VIEW_ICO, OnFrameViewIco)
	ON_MESSAGE(EB_COMMAND_OPEN_APP_URL, OnMsgOpenAppUrl)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_UP, OnMessageMoveUp)
	ON_MESSAGE(EB_COMMAND_RAME_WND_MOVE_DOWN, OnMessageMoveDown)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_TOOLBAR, &CDlgAppFrame::OnBnClickedButtonSwitchToolbar)
	ON_BN_CLICKED(IDC_BUTTON_NEW_WEB, &CDlgAppFrame::OnBnClickedButtonNewWeb)
	//ON_BN_CLICKED(IDC_BUTTON_MIN, &CDlgAppFrame::OnBnClickedButtonMin)
	//ON_BN_CLICKED(IDC_BUTTON_MAX, &CDlgAppFrame::OnBnClickedButtonMax)
	//ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgAppFrame::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CDlgAppFrame message handlers
void CDlgAppFrame::SetCtrlColor(void)
{
//#ifdef USES_SUPPORT_UI_STYLE
//	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
//	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT || theApp.GetHideMainFrame())
//#else
//	if (theApp.GetHideMainFrame())
//#endif
//	{
//		m_btnMin.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetHotColor(),theApp.GetPreColor());
//		m_btnMax.SetDrawPanel(true,theDefaultFlatBgColor,theApp.GetHotColor(),theApp.GetPreColor());
//		m_btnClose.SetDrawPanel(true,theDefaultFlatBgColor,theDefaultBtnCloseColor,theDefaultBtnCloseColor);
//	//}else
//	//{
//	//	m_btnMin.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
//	//	m_btnMax.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
//	//	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
//	}

#ifdef USES_NEW_UI_1220
	m_btnSwitchFrame.SetDrawPanel(true,theDefaultFlatBgColor);
#else
	m_btnSwitchFrame.SetDrawPanel(true,theApp.GetMainColor());
#endif
	m_btnSwitchToolbar.SetDrawPanel(true,theApp.GetMainColor());
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		pAppWindowInfo->SetCtrlColor();
	}
}
void CDlgAppFrame::OnMove(void)
{
	BoostReadLock rdlock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer& pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			CDlgAppWindow* pDlgAppWindow = pFrameWndInfo->GetAppWindow();
			if (pDlgAppWindow!=NULL)
				pDlgAppWindow->OnMove();
			return;
		}
	}
}

BOOL CDlgAppFrame::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);
	SetMouseMove(FALSE);
	//SetTransparentType(0);
	//theApp.AdaptIeVersion();

	m_btnSwitchFrame.SetDrawPanelRgn(false);
	m_btnSwitchFrame.SetWindowText(_T(""));
	m_btnSwitchFrame.SetToolTipText(_T("切换到主面板"));
#ifdef USES_NEW_UI_1220
	m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultFlatLineColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnSwitchFrame.ShowWindow(SW_HIDE);
#else
	m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultBtnWhiteColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
#endif
	m_btnSwitchToolbar.SetDrawPanelRgn(true);
	m_btnSwitchToolbar.SetWindowText(_T(""));
	m_btnSwitchToolbar.SetToolTipText(_T("隐藏应用导航栏"));
	m_btnSwitchToolbar.SetDrawTrianglePic(3,theDefaultBtnWhiteColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnNewWeb.SetDrawPanelRgn(false);
	m_btnNewWeb.SetWindowText(_T(""));
	m_btnNewWeb.SetToolTipText(_T("打开新标签页"));
#ifdef USES_NEW_UI_1220
	m_btnNewWeb.SetDrawNewPic(true,theDefaultFlatLineColor,-1,-1,-1,2,11);
#else
	m_btnNewWeb.SetDrawNewPic(true,theDefaultBtnWhiteColor,-1,-1,-1,2,11);
#endif
	m_btnNewWeb.ShowWindow(SW_SHOW);
//
//#ifdef USES_SUPPORT_UI_STYLE
//	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
//	const bool bForceShowBlack = nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT?true:false;
//#else
//	const bool bForceShowBlack = false;
//#endif
//	m_btnMin.SetAutoSize(false);
//	m_btnMin.SetDrawToolButtonPic(1,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
//	m_btnMin.SetDrawPanelRgn(false);
//	m_btnMin.SetToolTipText(_T("最小化"));
//	m_btnMax.SetAutoSize(false);
//	m_btnMax.SetDrawToolButtonPic(2,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
//	m_btnMax.SetDrawPanelRgn(false);
//	m_btnMax.SetToolTipText(_T("最大化"));
//	m_btnClose.SetAutoSize(false);
//	m_btnClose.SetDrawToolButtonPic(4,((bForceShowBlack||theApp.GetHideMainFrame())?theDefaultFlatBlackText2Color:theDefaultBtnWhiteColor),theDefaultBtnWhiteColor,-1,-1,2);
//	m_btnClose.SetDrawPanelRgn(false);
//	m_btnClose.SetToolTipText(_T("关闭"));
//#ifdef USES_SUPPORT_UI_STYLE
//	if (nDefaultUIStyleType!=EB_UI_STYLE_TYPE_CHAT)
//#else
//	if (!theApp.GetHideMainFrame())
//#endif
//	{
//		m_btnMin.ShowWindow(SW_HIDE);
//		m_btnMax.ShowWindow(SW_HIDE);
//		m_btnClose.ShowWindow(SW_HIDE);
//	}

	SetCtrlColor();
	this->SetCircle(false);

	SetTimer(TIMERID_CHECK_MOUSE_POS,50,NULL);
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAppFrame::OnDestroy()
{
	m_pMainFuncButtonList.clear();
	m_pList.clear();
	CEbDialogBase::OnDestroy();
}

void CDlgAppFrame::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgAppFrame::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

//void CDlgAppFrame::RefreshWindow(void)
//{
//	{
//		// ???
//		BoostReadLock rdlock(m_pList.mutex());
//		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
//		for (; pIter!=m_pList.end(); pIter++)
//		{
//			const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
//			pFrameWndInfo->RefreshWindow();
//			break;	// ???
//		}
//	}
//}
//void CDlgAppFrame::RefreshBtnShow(void)
//{
//#ifdef USES_SUPPORT_UI_STYLE
//	SetCtrlColor();
//	const EB_UI_STYLE_TYPE nDefaultUIStyleType = theApp.GetDefaultUIStyleType();
//	if (nDefaultUIStyleType==EB_UI_STYLE_TYPE_CHAT)
//	{
//		m_btnMin.ShowWindow(SW_SHOW);
//		m_btnMax.ShowWindow(SW_SHOW);
//		m_btnClose.ShowWindow(SW_SHOW);
//	}else
//	{
//		m_btnMin.ShowWindow(SW_HIDE);
//		m_btnMax.ShowWindow(SW_HIDE);
//		m_btnClose.ShowWindow(SW_HIDE);
//	}
//#endif
//}

void CDlgAppFrame::doRefresh(void)
{
	this->SetFocus();
	BoostReadLock rdlock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			pFrameWndInfo->doRefresh();
			return;
		}
	}
}
void CDlgAppFrame::goBack(void)
{
	this->SetFocus();
	BoostReadLock rdlock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			pFrameWndInfo->goBack();
			return;
		}
	}
}
void CDlgAppFrame::goForward(void)
{
	this->SetFocus();
	BoostReadLock rdlock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			pFrameWndInfo->goForward();
			return;
		}
	}
}
void CDlgAppFrame::changeBrowserType(void)
{
	this->SetFocus();
	BoostReadLock rdlock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			pFrameWndInfo->changeBrowserType();
			return;
		}
	}
}
EB_BROWSER_TYPE CDlgAppFrame::queryBrowserType(void) const
{
	AUTO_CONST_RLOCK(m_pList);
	CLockList<CAppWindowInfo::pointer>::const_iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			return pFrameWndInfo->queryBrowserType();
		}
	}
	return EB_BROWSER_TYPE_CEF;
}
bool CDlgAppFrame::canSaveHistory(void) const
{
	AUTO_CONST_RLOCK(m_pList);
	CLockList<CAppWindowInfo::pointer>::const_iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			return pFrameWndInfo->canSaveHistory();
		}
	}
	return false;
}
void CDlgAppFrame::saveHistory(void)
{
	AUTO_CONST_RLOCK(m_pList);
	CLockList<CAppWindowInfo::pointer>::const_iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			pFrameWndInfo->saveHistory();
		}
	}
}
void CDlgAppFrame::SetCurrentFocus(void)
{
	SetTimer(TIMERID_SET_CURRENT_FOCUS,10,NULL);
	//BoostReadLock rdlock(m_pList.mutex());
	//CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	//for (; pIter!=m_pList.end(); pIter++)
	//{
	//	const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
	//	if (pFrameWndInfo->IsCheckedFocus())
	//		return;
	//}
}

bool CDlgAppFrame::AddSubscribeFuncInfo(const EB_SubscribeFuncInfo & pSubscribeFuncInfo)
{
	if (pSubscribeFuncInfo.m_nIconResId==0 || !PathFileExists(pSubscribeFuncInfo.m_sResFile.c_str()))
	{
		if (theApp.m_sDefaultAppImage.empty())
			return false;
	}
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->GetFuncInfo().m_nSubscribeId==pSubscribeFuncInfo.m_nSubscribeId)
		{
			// 更新图标
			if (pFuncButtonInfo->GetResFile().empty() && !pSubscribeFuncInfo.m_sResFile.empty())
			{
				pFuncButtonInfo->UpdateResFile(pSubscribeFuncInfo.m_sResFile);
			}
			return true;
		}
	}
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CRect rectFuncButton;
	rectFuncButton.left = 2;
	rectFuncButton.right = rectFuncButton.left + const_func_button_size;
#ifdef USES_NEW_UI_1220
	rectFuncButton.top = 3+ m_pMainFuncButtonList.size()*(const_func_button_size+const_func_button_intever);
#else
	rectFuncButton.top = const_frame_height+2+ m_pMainFuncButtonList.size()*(const_func_button_size+const_func_button_intever);
#endif
	rectFuncButton.bottom = rectFuncButton.top + const_func_button_size;
	CEBFuncButton::pointer pFuncButton = CEBFuncButton::create();
	const UINT nID = EB_COMMAND_MAINFRAME_FUNC+m_pMainFuncButtonList.size();
	if (pFuncButton->Create(this,rectFuncButton,nID,pSubscribeFuncInfo,theApp.m_sDefaultAppImage))
	{
		m_pMainFuncButtonList.push_back(pFuncButton);
		return true;
	}
	return false;
}
void CDlgAppFrame::AddAppUrl(bool bSaveBrowseTitle, const std::string& sAppUrl, const std::string& sPostData, const EB_SubscribeFuncInfo& pFuncInfo, bool nOpenNewClose, int nInsertOffset)
{
	const mycp::bigint nSubscribeId = pFuncInfo.m_nFromSubscribeId>0?pFuncInfo.m_nFromSubscribeId:pFuncInfo.m_nSubscribeId;
	if (nSubscribeId>0)	// 订购应用，查询是否已经打开，如果是，直接显示；
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetSubscribeId()==nSubscribeId || pFrameWndInfo->GetFromSubscribeId()==nSubscribeId)
			{
				rdlock.unlock();
				pFrameWndInfo->SetAppUrl(bSaveBrowseTitle,sAppUrl,sPostData);
				ShowApp(pFrameWndInfo->GetUserData(),true);
				return;
			}else if (pFrameWndInfo->GetAppUrl()==sAppUrl)
			{
				rdlock.unlock();
				pFrameWndInfo->doRefresh();
				ShowApp(pFrameWndInfo->GetUserData(),true);
				return;
			}else if (pFrameWndInfo->GetAppUrl()=="about:blank")
			{
				rdlock.unlock();
				pFrameWndInfo->SetFunInfo(pFuncInfo,nOpenNewClose);
				pFrameWndInfo->SetAppUrl(bSaveBrowseTitle,sAppUrl,sPostData);
				ShowApp(pFrameWndInfo->GetUserData(),true);
				return;
			}
		}
	}else// if (nInsertOffset==-1)
	{
		int nCurrentIndex = 0;
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetAppUrl()=="about:blank" && (nInsertOffset==-1 || nCurrentIndex==nInsertOffset))
			{
				rdlock.unlock();
				pFrameWndInfo->SetAppUrl(bSaveBrowseTitle,sAppUrl,sPostData);
				if (!pFrameWndInfo->IsChecked())
				//if (nInsertOffset!=-1 && !pFrameWndInfo->IsChecked())
					ShowApp(pFrameWndInfo->GetUserData(),true);
				return;
			}
			nCurrentIndex++;
		}
	}

	CRect rectLable;
#ifdef USES_NEW_UI_1220
	rectLable.left = (m_bShowedToolbar?const_frame_tool:0)+m_pList.size()*const_frame_width;
#else
	rectLable.left = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+m_pList.size()*const_frame_width;
#endif
	rectLable.right = rectLable.left+const_frame_width;
	rectLable.top = 0;
	rectLable.bottom = rectLable.top+const_frame_height;

	int nIndexId = 0;
	m_pIndexIdList.front(nIndexId);
	CAppWindowInfo::pointer pAppWindowInfo = CAppWindowInfo::create(sAppUrl);
	pAppWindowInfo->SetUserData(nIndexId);
	CFont * pFont = m_childFont.GetSafeHandle()==NULL?NULL:&m_childFont;
	if (pAppWindowInfo->Create(pFuncInfo.m_sFunctionName.c_str(),rectLable,this,pAppWindowInfo->GetUserData(),pFont,pFuncInfo,sPostData,nOpenNewClose,(m_bShowedToolbar?const_frame_tool:0),bSaveBrowseTitle))
	{
		bool bInsertOk = false;
		if (nInsertOffset>=0 && nInsertOffset<(int)m_pList.size())
		{
			int nCurrentIndex = 0;
			BoostWriteLock wtLock(m_pList.mutex());
			CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
			for (; pIter!=m_pList.end(); pIter++)
			{
				if ((nCurrentIndex++)==nInsertOffset)
				{
					m_pList.insert(pIter,pAppWindowInfo);
					bInsertOk = true;
					break;
				}
			}
		}
		if (bInsertOk)
		{
			RebuildBtnSize();
		}else
		{
			m_pList.add(pAppWindowInfo);
		}		
		ShowApp(pAppWindowInfo->GetUserData(),true);

		if (m_btnNewWeb.GetSafeHwnd()!=NULL)
		{
			m_rectNewWeb.left = rectLable.right+2;
			m_rectNewWeb.right = m_rectNewWeb.left+24;
			m_rectNewWeb.top = 3;
			m_rectNewWeb.bottom = m_rectNewWeb.top+24;
			m_btnNewWeb.MoveWindow(&m_rectNewWeb);
			m_btnNewWeb.Invalidate();
			theApp.InvalidateParentRect(&m_btnNewWeb);
		}
		CRect rect;
		this->GetWindowRect(&rect);
		const int nWidthOffset = m_rectNewWeb.right-rect.Width();
		if (nWidthOffset>0)
		{
			this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_ADJUST_WIDTH,(WPARAM)this->GetSafeHwnd(),(LPARAM)nWidthOffset);
		}
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)this->GetSafeHwnd(),(LPARAM)m_pList.size(false));
	}else
	{
		m_pIndexIdList.pushfront(nIndexId);
	}
}
void CDlgAppFrame::AddFromAppFrame(const CDlgAppFrame* pDlgAppFrame)
{
	const CLockList<CAppWindowInfo::pointer>& pList = pDlgAppFrame->GetList();
	if (pList.empty()) return;
	{
		const bool bOwnerEmpty = this->m_pList.empty(false);
		unsigned int nCheckedIndex = 0;
		CFont * pFont = m_childFont.GetSafeHandle()==NULL?NULL:&m_childFont;
		AUTO_CONST_RLOCK(pList);
		CLockList<CAppWindowInfo::pointer>::const_iterator pIter = pList.begin();
		for (; pIter!=pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			const_cast<CDlgAppFrame*>(pDlgAppFrame)->ResetUserData(pAppWindowInfo);

			CDlgAppWindow* pDlgAppWindow = pAppWindowInfo->GetAppWindow();
			if (pDlgAppWindow==NULL) continue;

			int nIndexId = 0;
			m_pIndexIdList.front(nIndexId);
			pAppWindowInfo->SetUserData(nIndexId);
			if (nCheckedIndex==0 && pAppWindowInfo->IsChecked())
			{
				nCheckedIndex = nIndexId;
			}

			CRect rectLable;
#ifdef USES_NEW_UI_1220
			rectLable.left = (m_bShowedToolbar?const_frame_tool:0)+m_pList.size()*const_frame_width;
#else
			rectLable.left = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+m_pList.size()*const_frame_width;
#endif
			rectLable.right = rectLable.left+const_frame_width;
			rectLable.top = 0;
			rectLable.bottom = rectLable.top+const_frame_height;
			m_pList.add(pAppWindowInfo);
			pAppWindowInfo->Create(NULL,rectLable,this,pAppWindowInfo->GetUserData(),pFont,pDlgAppWindow->m_pFuncInfo,"",false,(m_bShowedToolbar?const_frame_tool:0),false);
		}
		if (bOwnerEmpty)
		{
			if (nCheckedIndex>0)
				this->ShowApp(nCheckedIndex,false);
			else
				this->ShowFirstWnd();
		}
	}
	const_cast<CDlgAppFrame*>(pDlgAppFrame)->DelAll();

	if (m_btnNewWeb.GetSafeHwnd()!=NULL)
	{
		m_rectNewWeb.left = (m_bShowedToolbar?const_frame_tool:0)+m_pList.size()*const_frame_width+2;
		//m_rectNewWeb.left = rectLable.right+2;
		m_rectNewWeb.right = m_rectNewWeb.left+24;
		m_rectNewWeb.top = 3;
		m_rectNewWeb.bottom = m_rectNewWeb.top+24;
		m_btnNewWeb.MoveWindow(&m_rectNewWeb);
		m_btnNewWeb.Invalidate();
		theApp.InvalidateParentRect(&m_btnNewWeb);
	}
	CRect rect;
	this->GetWindowRect(&rect);
	const int nWidthOffset = m_rectNewWeb.right-rect.Width();
	if (nWidthOffset>0)
	{
		this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_ADJUST_WIDTH,(WPARAM)this->GetSafeHwnd(),(LPARAM)nWidthOffset);
	}
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)this->GetSafeHwnd(),(LPARAM)m_pList.size(false));

}

bool CDlgAppFrame::IsEmpty(void) const
{
	return m_pList.empty();
}
size_t CDlgAppFrame::GetSize(void) const
{
	return m_pList.size();
}
void CDlgAppFrame::ResetUserData(const CAppWindowInfo::pointer& pAppWindowInfo)
{
	const int nUserData = (int)pAppWindowInfo->GetUserData();
	if (nUserData>0)
	{
		pAppWindowInfo->SetUserData(0);
		m_pIndexIdList.pushfront(nUserData);
	}
}
void CDlgAppFrame::DelAll(void)
{
	if (m_pList.empty()) return;
	m_pList.clear();
	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)this->GetSafeHwnd(),(LPARAM)m_pList.size(false));
	OnBnClickedButtonSwitchFrame();
	RebuildBtnSize();
	return;
}

void CDlgAppFrame::MoveSize(int cx, int cy)
{
	if (m_btnSwitchFrame.GetSafeHwnd()!=NULL)
	{
		m_rectSwitchFrame.left = 0;	// cx-DEFAULT_RETURN_BTN_WIDTH;
		m_rectSwitchFrame.right = m_rectSwitchFrame.left+DEFAULT_RETURN_BTN_WIDTH;
		m_rectSwitchFrame.top = 0;
		m_rectSwitchFrame.bottom = const_frame_height;
		m_btnSwitchFrame.MoveWindow(&m_rectSwitchFrame);
		//m_btnSwitchFrame.MoveWindow(cx-DEFAULT_RETURN_BTN_WIDTH,0,DEFAULT_RETURN_BTN_WIDTH,const_frame_height);
	}
	if (m_btnSwitchToolbar.GetSafeHwnd()!=NULL)
	{
		m_rectSwitchToolbar.left = m_bShowedToolbar?(const_frame_tool-DEFAULT_RETURN_BTN_WIDTH):0;
		m_rectSwitchToolbar.right = m_rectSwitchToolbar.left+DEFAULT_RETURN_BTN_WIDTH;
		m_rectSwitchToolbar.top = (cy-const_frame_tool)/2;
		m_rectSwitchToolbar.bottom = m_rectSwitchToolbar.top+const_frame_tool;
		m_btnSwitchToolbar.MoveWindow(&m_rectSwitchToolbar);
	}
	for (size_t i=0;i<m_pMainFuncButtonList.size();i++)
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[i];
		if (pFuncButtonInfo->m_btn.GetSafeHwnd()!=NULL)
		{
			CRect rectFuncButton;
			rectFuncButton.left = 2;
			rectFuncButton.right = rectFuncButton.left + const_func_button_size;
#ifdef USES_NEW_UI_1220
			rectFuncButton.top = 3 + i*(const_func_button_size+const_func_button_intever);
#else
			rectFuncButton.top = const_frame_height + 2 + i*(const_func_button_size+const_func_button_intever);
#endif
			rectFuncButton.bottom = rectFuncButton.top + const_func_button_size;
			pFuncButtonInfo->m_btn.MoveWindow(&rectFuncButton);
			pFuncButtonInfo->m_btn.ShowWindow(m_bShowedToolbar?SW_SHOW:SW_HIDE);
		}
	}
	if (m_bShowedToolbar)
		MoveWindowSize(cx, cy);
	else
		SetTimer(TIMERID_MOVE_WINDOW_SIZE,10,NULL);	// 解决按钮背景刷新问题，部分网站IE控件右边滚动条位置，也会有背景问题（不影响使用）
}

void CDlgAppFrame::MoveWindowSize(int cx, int cy)
{
#ifdef USES_NEW_UI_1220
	int nNewWebBtnLeft = (m_bShowedToolbar?const_frame_tool:0)+2;
#else
	int nNewWebBtnLeft = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+2;
#endif
	{
		int index = 0;
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
			CRect rectLable;
#ifdef USES_NEW_UI_1220
			rectLable.left = (m_bShowedToolbar?const_frame_tool:0)+(index++)*const_frame_width;
#else
			rectLable.left = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+(index++)*const_frame_width;
#endif
			rectLable.right = rectLable.left+const_frame_width;
			rectLable.top = 0;
			rectLable.bottom = rectLable.top+const_frame_height;
			pFrameWndInfo->MoveBtnWindow(&rectLable);
			pFrameWndInfo->MoveSize(m_bShowedToolbar?const_frame_tool:0,cx, cy);
			pFrameWndInfo->IsCheckedFocus();
			nNewWebBtnLeft = rectLable.right+2;
		}
	}
	if (m_btnNewWeb.GetSafeHwnd()!=NULL)
	{
		m_rectNewWeb.left = nNewWebBtnLeft;
		m_rectNewWeb.right = m_rectNewWeb.left+24;
		m_rectNewWeb.top = 3;
		m_rectNewWeb.bottom = m_rectNewWeb.top+24;
		m_btnNewWeb.MoveWindow(&m_rectNewWeb);
		theApp.InvalidateParentRect(&m_btnNewWeb);
	}
}

void CDlgAppFrame::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	//const int const_minbtn_width = 24;
	//const int const_minbtn_height = 22;
	//int btnx = 0;
	//btnx = cx-const_minbtn_width-2;
	//if (m_btnClose.GetSafeHwnd()!=NULL)
	//	m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	//btnx -= const_minbtn_width;
	//if (m_btnMax.GetSafeHwnd()!=NULL)
	//	m_btnMax.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
	//btnx -= const_minbtn_width;
	//if (m_btnMin.GetSafeHwnd()!=NULL)
	//	m_btnMin.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	MoveSize(cx, cy);
}

void CDlgAppFrame::OnBnClickedButtonSwitchFrame()
{
	this->GetParent()->PostMessage(EB_COMMAND_RETURN_MAINFRAME);
	//this->ShowWindow(SW_HIDE);
}

void CDlgAppFrame::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	//// TODO: Add your message handler code here
	//// Do not call CEbDialogBase::OnPaint() for painting messages

	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文
	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
#ifdef USES_NEW_UI_1220
		if (m_bShowedToolbar)
		{
			this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,const_frame_tool-1,theDefaultFlatBgColor));
			this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,1,theDefaultFlatLineColor));
		}
		this->AddBgDrawInfo(CEbBackDrawInfo(const_frame_height-1,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatLineColor));
#else
		this->AddBgDrawInfo(CEbBackDrawInfo(const_frame_height,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,const_frame_tool-1,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,1,theDefaultFlatLineColor));
#endif
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatLineColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
	}
}

void CDlgAppFrame::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);

	//{
	//	int index = 0;
	//	BoostReadLock rdlock(m_pList.mutex());
	//	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	//	for (; pIter!=m_pList.end(); pIter++)
	//	{
	//		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
	//		CRect rectLable;
	//		rectLable.left = (index++)*const_frame_width;
	//		rectLable.right = rectLable.left+const_frame_width;
	//		rectLable.top = 0;
	//		rectLable.bottom = rectLable.top+const_frame_height;
	//		//pFrameWndInfo->Move
	//	}
	//}
}

void CDlgAppFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgAppFrame::OnFrameBtnClicked(UINT nID)
{
	ClickFrame(nID);
}

void CDlgAppFrame::ClickFrame(unsigned int nWndIndex)
{
	CAppWindowInfo::pointer pCheckedAppWindowInfo;
	bool bClose = false;
	{
		CPoint pos;
		GetCursorPos(&pos);
		this->ScreenToClient(&pos);
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->GetUserData()==nWndIndex)
			{
				//SendMsgChangeUrl(pAppWindowInfo,false);	// true
				//pAppWindowInfo->ShowHide(true);
				if (pAppWindowInfo->IsClickClose(pos))
				{
					bClose = true;
					pAppWindowInfo->SendClose();
					//return;
				}else
				{
					SendMsgChangeUrl(pAppWindowInfo,false);	// true
					pAppWindowInfo->ShowHide(true);
					pAppWindowInfo->IsCheckedFocus();
				}
			}else
			{
				if (pAppWindowInfo->IsChecked())
					pCheckedAppWindowInfo = pAppWindowInfo;
				pAppWindowInfo->ShowHide(false);
			}
		}
	}
	if (bClose && pCheckedAppWindowInfo.get()!=NULL)
	{
		SendMsgChangeUrl(pCheckedAppWindowInfo,true);
		pCheckedAppWindowInfo->ShowHide(true);
	}
}
bool CDlgAppFrame::ShowPrev(void)
{
	if (m_pList.size()<=1) return false;
	CAppWindowInfo::pointer pHideFrameWndInfo;
	CAppWindowInfo::pointer pShowFrameWndInfo;
	CAppWindowInfo::pointer pPrevFrameWndInfo;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsChecked())
		{
			pHideFrameWndInfo = pFrameWndInfo;
			if (pPrevFrameWndInfo.get()!=NULL)	// 前面有未显示
			{
				pShowFrameWndInfo = pPrevFrameWndInfo;
				break;
			}
			continue;
		}
		pPrevFrameWndInfo = pFrameWndInfo;
		//if (pHideFrameWndInfo.get()!=NULL)	// 前面已经找到已经显示，后面有未显示
		//{
		//	pShowFrameWndInfo = pPrevFrameWndInfo;
		//	break;	// 不能退出，因为要取最后一个；
		//}
	}
	if (pHideFrameWndInfo.get()!=NULL && pPrevFrameWndInfo.get()!=NULL && pShowFrameWndInfo.get()==NULL)
	{
		pShowFrameWndInfo = pPrevFrameWndInfo;	// 这是最后一个；
	}
	if (pShowFrameWndInfo.get()!=NULL)
	{
		SendMsgChangeUrl(pShowFrameWndInfo,false);
		pShowFrameWndInfo->ShowHide(true);
		//if (m_pCallback!=NULL)
		//	m_pCallback->OnFrameWndShow(pShowFrameWndInfo,true);
		if (pHideFrameWndInfo.get()!=NULL)
			pHideFrameWndInfo->ShowHide(false);
		return true;
	}
	return false;
}
bool CDlgAppFrame::ShowNext(void)
{
	if (m_pList.size()<=1) return false;

	CAppWindowInfo::pointer pFirstFrameWndInfo;
	CAppWindowInfo::pointer pLastFrameWndInfo;
	CAppWindowInfo::pointer pHideFrameWndInfo;
	CAppWindowInfo::pointer pShowFrameWndInfo;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFirstFrameWndInfo.get()==NULL)
			pFirstFrameWndInfo = pFrameWndInfo;
		pLastFrameWndInfo = pFrameWndInfo;
		if (pFrameWndInfo->IsChecked())
		{
			pHideFrameWndInfo = pFrameWndInfo;
			continue;
		}
		pShowFrameWndInfo = pFrameWndInfo;
		if (pHideFrameWndInfo.get()!=NULL)	// 前面已经找到已经显示，后面有未显示（*前面也会正常）
		{
			break;
		}
	}
	if (pHideFrameWndInfo.get()==pLastFrameWndInfo.get())
	{
		pShowFrameWndInfo = pFirstFrameWndInfo;	// 这是第一个；
	}
	if (pShowFrameWndInfo.get()!=NULL)
	{
		SendMsgChangeUrl(pShowFrameWndInfo,false);
		pShowFrameWndInfo->ShowHide(true);
		//if (m_pCallback!=NULL)
		//	m_pCallback->OnFrameWndShow(pShowFrameWndInfo,true);
		if (pHideFrameWndInfo.get()!=NULL)
			pHideFrameWndInfo->ShowHide(false);
		return true;
	}
	return false;
	//{
	//	BoostReadLock rdLock(m_pList.mutex());
	//	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	//	for (; pIter!=m_pList.end(); pIter++)
	//	{
	//		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
	//		if (pAppWindowInfo->GetUserData()==nWndIndex)
	//		{
	//			pAppWindowInfo->ShwoHide(true);
	//			SendMsgChangeUrl(pAppWindowInfo);
	//		}else
	//		{
	//			pAppWindowInfo->ShowHide(false);
	//		}
	//	}
	//}
}
void CDlgAppFrame::ShowApp(unsigned int nWndIndex, bool bSearchFocus)
{
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->GetUserData()==nWndIndex)
			{
				SendMsgChangeUrl(pAppWindowInfo,bSearchFocus);
				pAppWindowInfo->ShowHide(true);
			}else
			{
				pAppWindowInfo->ShowHide(false);
			}
		}
	}
}
void CDlgAppFrame::DelApp(unsigned int nWndIndex)
{
	bool bCloseAboutBlank = false;
	CAppWindowInfo::pointer pPrevFrameWndInfo;
	{
		BoostWriteLock wtLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->GetUserData()==nWndIndex)
			{
				m_pIndexIdList.pushfront(pAppWindowInfo->GetUserData());
				m_pList.erase(pIter);
				if (pAppWindowInfo->IsAboutBlank())
				{
					bCloseAboutBlank = true;
				}
				break;
			}
			pPrevFrameWndInfo = pAppWindowInfo;
		}
	}
	if (!bCloseAboutBlank && m_pList.empty())
	{
		OnBnClickedButtonNewWeb();
	}

	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)this->GetSafeHwnd(),(LPARAM)m_pList.size(false));
	if (m_pList.empty())
	{
		OnBnClickedButtonSwitchFrame();
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		SendMsgChangeUrl(pPrevFrameWndInfo,false);
		pPrevFrameWndInfo->ShowHide(true);
	}else
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->IsChecked())
			{
				SendMsgChangeUrl(pAppWindowInfo,false);
				pAppWindowInfo->ShowHide(true);
				pPrevFrameWndInfo.reset();
				break;
			}else if (pPrevFrameWndInfo.get()==NULL)
			{
				pPrevFrameWndInfo = pAppWindowInfo;
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
		{
			SendMsgChangeUrl(pPrevFrameWndInfo,false);
			pPrevFrameWndInfo->ShowHide(true);
		}
	}
	RebuildBtnSize();
}
void CDlgAppFrame::DelApp(const CWnd* pWnd)
{
	bool bCloseAboutBlank = false;
	CAppWindowInfo::pointer pPrevFrameWndInfo;
	{
		BoostWriteLock wtLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->GetAppWindow()==pWnd)
			{
				m_pIndexIdList.pushfront(pAppWindowInfo->GetUserData());
				m_pList.erase(pIter);
				if (pAppWindowInfo->IsAboutBlank())
				{
					bCloseAboutBlank = true;
				}
				break;
			}
			pPrevFrameWndInfo = pAppWindowInfo;
		}
	}
	if (!bCloseAboutBlank && m_pList.empty())
	{
		OnBnClickedButtonNewWeb();
	}

	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_COUNT,(WPARAM)this->GetSafeHwnd(),(LPARAM)m_pList.size(false));
	if (m_pList.empty())
	{
		OnBnClickedButtonSwitchFrame();
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		SendMsgChangeUrl(pPrevFrameWndInfo,false);
		pPrevFrameWndInfo->ShowHide(true);
	}else
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->IsChecked())
			{
				SendMsgChangeUrl(pAppWindowInfo,false);
				pAppWindowInfo->ShowHide(true);
				pPrevFrameWndInfo.reset();
				break;
			}else if (pPrevFrameWndInfo.get()==NULL)
			{
				pPrevFrameWndInfo = pAppWindowInfo;
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
		{
			SendMsgChangeUrl(pPrevFrameWndInfo,false);
			pPrevFrameWndInfo->ShowHide(true);
		}
	}
	RebuildBtnSize();

}

void CDlgAppFrame::SetAppTitle(const CWnd* pWnd, const CString& sTitle)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		if (pAppWindowInfo->GetAppWindow()==pWnd)
		{
			pAppWindowInfo->SetTitle(sTitle);
			//if (sTitle!=_T("about:blank") || sTitle==pAppWindowInfo->GetAppUrl().c_str())
			if (pAppWindowInfo->IsChecked() && (sTitle!=_T("about:blank") || sTitle==pAppWindowInfo->GetAppUrl().c_str()))
			{
				const bool bSearchFocus = sTitle=="about:blank"?true:false;
				SendMsgChangeUrl(pAppWindowInfo,bSearchFocus);
			}
			return;
		}
	}
}

void CDlgAppFrame::SetAppIco(const CWnd* pWnd)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		if (pAppWindowInfo->GetAppWindow()==pWnd)
		{
			pAppWindowInfo->SetIco();
			return;
		}
	}
}

void CDlgAppFrame::SendMsgChangeUrl(const CAppWindowInfo::pointer& pAppWindowInfo,bool bSearchFocus)
{
	//if (pAppWindowInfo->GetSubscribeId()==0)
	{
		const CString sUrl(pAppWindowInfo->GetLocationURL());
		char * lpszBuffer = new char[sUrl.GetLength()+1];
		strcpy(lpszBuffer,sUrl);
		const int nSearchFocus = (bSearchFocus || sUrl=="about:blank")?1:0;
		this->GetParent()->PostMessage(EB_COMMAND_CHANGE_APP_URL,(WPARAM)lpszBuffer,nSearchFocus);
	}
}

void CDlgAppFrame::ShowFirstWnd(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		SendMsgChangeUrl(pAppWindowInfo,false);
		pAppWindowInfo->ShowHide(true);
		return;
	}
}
void CDlgAppFrame::RebuildBtnSize(void)
{
#ifdef USES_NEW_UI_1220
	int nNewWebBtnLeft = (m_bShowedToolbar?const_frame_tool:0)+2;
#else
	int nNewWebBtnLeft = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+2;
#endif
	int nIndex = 0;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;

		CRect rectLable;
#ifdef USES_NEW_UI_1220
		rectLable.left = (m_bShowedToolbar?const_frame_tool:0)+(nIndex++)*const_frame_width;
#else
		rectLable.left = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+(nIndex++)*const_frame_width;
#endif
		rectLable.right = rectLable.left+const_frame_width;
		rectLable.top = 0;
		rectLable.bottom = rectLable.top+const_frame_height;
		pAppWindowInfo->MoveBtnWindow(&rectLable);
		nNewWebBtnLeft = rectLable.right+2;
	}

	if (m_btnNewWeb.GetSafeHwnd()!=NULL)
	{
		m_rectNewWeb.left = nNewWebBtnLeft;
		m_rectNewWeb.right = m_rectNewWeb.left+24;
		m_rectNewWeb.top = 3;
		m_rectNewWeb.bottom = m_rectNewWeb.top+24;
		m_btnNewWeb.MoveWindow(&m_rectNewWeb);
	}
}
int CDlgAppFrame::GetOffsetIndexByHwnd(HWND hHwnd) const
{
	if (hHwnd==NULL) return -1;
	int nCurrentIndex = 0;
	AUTO_CONST_RLOCK(m_pList);
	CLockList<CAppWindowInfo::pointer>::const_iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		if (pAppWindowInfo->IsAppWindow(hHwnd))
		{
			return nCurrentIndex;
		}
		nCurrentIndex++;
	}
	return -1;
}

void CDlgAppFrame::RefreshAppWnd(void)
{
	this->SetFocus();
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		if (pAppWindowInfo->IsCheckedRefresh())
		{
			return;
		}
	}
}
void CDlgAppFrame::NotifyMoveOrResizeStarted(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		pAppWindowInfo->NotifyMoveOrResizeStarted();
	}
}
void CDlgAppFrame::OnLogonSuccess(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		pAppWindowInfo->OnLogonSuccess();
	}
}
void CDlgAppFrame::OnOffline(int nServerState)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		pAppWindowInfo->OnOffline(nServerState);
	}
}

void CDlgAppFrame::CheckMousePos(const POINT& point)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		pAppWindowInfo->CheckMousePos(point);
	}
}

void CDlgAppFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMERID_SET_CURRENT_FOCUS:
		{
			KillTimer(nIDEvent);
			this->SetFocus();
			BoostReadLock rdlock(m_pList.mutex());
			CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
			for (; pIter!=m_pList.end(); pIter++)
			{
				const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
				if (pFrameWndInfo->IsCheckedFocus())
					break;
			}
		}break;
	case TIMERID_CHECK_MOUSE_POS:
		{
			CPoint pos;
			GetCursorPos(&pos);
			this->ScreenToClient(&pos);
			CheckMousePos(pos);

			if (m_btnSwitchFrame.GetSafeHwnd()!=NULL)
			{
				static bool thePtInRect = false;
				const bool bPtInRect = m_rectSwitchFrame.PtInRect(pos)?true:false;
				if (thePtInRect!=bPtInRect)
				{
					thePtInRect = bPtInRect;
					if (bPtInRect)
					{
						m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultFlatBlackText2Color,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
					}else
					{
#ifdef USES_NEW_UI_1220
						m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultFlatLineColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
#else
						m_btnSwitchFrame.SetDrawTrianglePic(3,theDefaultBtnWhiteColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
#endif
					}
					m_btnSwitchFrame.Invalidate();
				}
			}
			if (m_btnSwitchToolbar.GetSafeHwnd()!=NULL)
			{
				static bool thePtInRect = false;
				const bool bPtInRect = m_rectSwitchToolbar.PtInRect(pos)?true:false;
				if (thePtInRect!=bPtInRect)
				{
					thePtInRect = bPtInRect;
					if (bPtInRect)
					{
						if (m_bShowedToolbar)
							m_btnSwitchToolbar.SetDrawTrianglePic(3,theDefaultFlatBlackText2Color,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
						else
							m_btnSwitchToolbar.SetDrawTrianglePic(4,theDefaultFlatBlackText2Color,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
					}else
					{
						if (m_bShowedToolbar)
							m_btnSwitchToolbar.SetDrawTrianglePic(3,theDefaultBtnWhiteColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
						else
							m_btnSwitchToolbar.SetDrawTrianglePic(4,theDefaultBtnWhiteColor,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
					}
					m_btnSwitchToolbar.Invalidate();
				}
			}
			if (m_btnNewWeb.GetSafeHwnd()!=NULL)
			{
				static bool thePtInRect = false;
				const bool bPtInRect = m_rectNewWeb.PtInRect(pos)?true:false;
				if (thePtInRect!=bPtInRect)
				{
					thePtInRect = bPtInRect;
					if (bPtInRect)
					{
						m_btnNewWeb.SetDrawNewPic(true,theDefaultFlatBlackText2Color,-1,-1,-1,2,11);
					}else
					{
#ifdef USES_NEW_UI_1220
						m_btnNewWeb.SetDrawNewPic(true,theDefaultFlatLineColor,-1,-1,-1,2,11);
#else
						m_btnNewWeb.SetDrawNewPic(true,theDefaultBtnWhiteColor,-1,-1,-1,2,11);
#endif
					}
					m_btnNewWeb.Invalidate();
				}
			}
		}break;
	case TIMERID_MOVE_WINDOW_SIZE:
		{
			KillTimer(TIMERID_MOVE_WINDOW_SIZE);
			CRect rect;
			this->GetClientRect(&rect);
			MoveWindowSize(rect.Width(), rect.Height());
		}break;
	default:
		break;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}
LRESULT CDlgAppFrame::OnFrameWndClose(WPARAM wParam, LPARAM lParam)
{
	const CWnd * pWnd = (CWnd*)wParam;
	DelApp(pWnd);
	return 0;
}
LRESULT CDlgAppFrame::OnFrameWndTitle(WPARAM wParam, LPARAM lParam)
{
	const CWnd * pWnd = (CWnd*)wParam;
	char * sTitle = (char*)lParam;
	if (sTitle==0 || strlen(sTitle)==0) return 0;
	SetAppTitle(pWnd, sTitle);
	delete[] sTitle;
	return 0;
}

LRESULT CDlgAppFrame::OnFrameViewIco(WPARAM wParam, LPARAM lParam)
{
	const CWnd * pWnd = (CWnd*)wParam;
	SetAppIco(pWnd);
	return 0;
}

LRESULT CDlgAppFrame::OnMsgOpenAppUrl(WPARAM wParam, LPARAM lParam)
{
	COpenAppUrlInfo * pOpenAppUrlInfo = (COpenAppUrlInfo*)wParam;
	if (pOpenAppUrlInfo==0) return 0;
	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = pOpenAppUrlInfo->m_sUrl;
	pSubscribeFuncInfo.m_bDisableContextMenu = (pOpenAppUrlInfo->m_nOpenParam&0x1)!=0;;
	pSubscribeFuncInfo.m_bDisableScrollBar = (pOpenAppUrlInfo->m_nOpenParam&0x2)!=0;
	pSubscribeFuncInfo.m_bClosePrompt = (pOpenAppUrlInfo->m_nOpenParam&0x4)!=0;
	pSubscribeFuncInfo.m_bWinResizable = true;
	const bool nOpenNewClose = (pOpenAppUrlInfo->m_nOpenParam&0x8)!=0;
	const int nFromOffsetIndex = GetOffsetIndexByHwnd(pOpenAppUrlInfo->m_hwndFrom);
	const int nInsertOffset = nFromOffsetIndex!=-1?(nFromOffsetIndex+1):-1;
	this->AddAppUrl(pOpenAppUrlInfo->m_bSaveBrowseTitle,pOpenAppUrlInfo->m_sUrl,pOpenAppUrlInfo->m_sPostData,pSubscribeFuncInfo,nOpenNewClose,nInsertOffset);
	delete pOpenAppUrlInfo;

	//char * sUrl = (char*)wParam;
	//if (sUrl==0 || strlen(sUrl)==0) return 0;
	//EB_SubscribeFuncInfo pSubscribeFuncInfo;
	//pSubscribeFuncInfo.m_sFunctionName = sUrl;
	//pSubscribeFuncInfo.m_bDisableContextMenu = (lParam&0x1)!=0;;
	//pSubscribeFuncInfo.m_bDisableScrollBar = (lParam&0x2)!=0;
	//pSubscribeFuncInfo.m_bClosePrompt = (lParam&0x4)!=0;
	//pSubscribeFuncInfo.m_bWinResizable = true;
	//const bool nOpenNewClose = (lParam&0x8)!=0;
	//this->AddAppUrl(sUrl,pSubscribeFuncInfo,nOpenNewClose);
	//delete[] sUrl;
	return 0;
}
LRESULT CDlgAppFrame::OnMessageMoveUp(WPARAM wParam, LPARAM lParam)
{
	this->ShowPrev();
	return 0;
}
LRESULT CDlgAppFrame::OnMessageMoveDown(WPARAM wParam, LPARAM lParam)
{
	this->ShowNext();
	return 0;
}


void CDlgAppFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->PostMessage(WM_LBUTTONDBLCLK, nFlags, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDblClk(nFlags, point);
}

void CDlgAppFrame::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->PostMessage(WM_RBUTTONUP, nFlags, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnRButtonUp(nFlags, point);
}

void CDlgAppFrame::OnBnClickedButtonSwitchToolbar()
{
	if (m_bShowedToolbar)
	{
		m_btnSwitchToolbar.SetToolTipText(_T("显示应用导航栏"));
		m_btnSwitchToolbar.SetDrawTrianglePic(4,theDefaultFlatBlackText2Color,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	}else
	{
		m_btnSwitchToolbar.SetToolTipText(_T("隐藏应用导航栏"));
		m_btnSwitchToolbar.SetDrawTrianglePic(3,theDefaultFlatBlackText2Color,-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	}
	m_bShowedToolbar = !m_bShowedToolbar;
	CRect rect;
	this->GetClientRect(&rect);
	MoveSize(rect.Width(),rect.Height());
	m_btnSwitchToolbar.Invalidate();
	//if (!m_bShowedToolbar)
	//{
	//	//RefreshAppWnd();
	//	////this->PostMessage(POP_WM_WINDOW_SIZE);
	//	//rect.top = const_frame_height;
	//	//rect.right = const_frame_tool;
	//	//this->InvalidateRect(&rect,FALSE);
	//}
}

void CDlgAppFrame::OnMainFrameFunc(UINT nID)
{
	const size_t nIndex = nID-EB_COMMAND_MAINFRAME_FUNC;
	if (nIndex>=0 && nIndex<m_pMainFuncButtonList.size())
	{
		const CEBFuncButton::pointer & pFuncButtonInfo = m_pMainFuncButtonList[nIndex];
		const EB_SubscribeFuncInfo& pSubscribeFuncInfo = pFuncButtonInfo->GetFuncInfo();
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,"","",NULL);
	}
}

void CDlgAppFrame::OnBnClickedButtonNewWeb()
{
	//const tstring sAppUrl("www.baidu.com");
	const tstring sAppUrl("about:blank");
	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = sAppUrl;
	pSubscribeFuncInfo.m_bClosePrompt = false;
	pSubscribeFuncInfo.m_bDisableScrollBar = false;
	pSubscribeFuncInfo.m_bDisableContextMenu = false;
	pSubscribeFuncInfo.m_bWinResizable = true;
	AddAppUrl(false,sAppUrl,"",pSubscribeFuncInfo,false);
}
BOOL CDlgAppFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if (GetKeyState(VK_CONTROL)&0x80)
			{
				if (pMsg->wParam==VK_UP)
				{
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_UP, 0, 0);
					return TRUE;
				}else if (pMsg->wParam==VK_DOWN)
				{
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_DOWN, 0, 0);
					return TRUE;
				}else if (pMsg->wParam>=VK_NUMPAD0 && pMsg->wParam<=VK_NUMPAD9)
				{
					this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MOVE_OFFSET, (WPARAM)(pMsg->wParam-VK_NUMPAD0), 0);
					return TRUE;
				}else if (pMsg->wParam==VK_LEFT)
				{
					this->ShowPrev();
					return TRUE;
				}else if (pMsg->wParam==VK_RIGHT)
				{
					this->ShowNext();
					return TRUE;
				}
			}
		}break;
	//case WM_MOVING:
	//case WM_MOVE:
	//	{
	//		int i=0;
	//	}break;
	default:
		break;
	}

	return CEbDialogBase::PreTranslateMessage(pMsg);
}

//void CDlgAppFrame::OnBnClickedButtonMin()
//{
//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MIN,(WPARAM)(CWnd*)this,0);
//}
//
//void CDlgAppFrame::OnBnClickedButtonMax()
//{
//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MAX,(WPARAM)(CWnd*)this,0);
//}
//void CDlgAppFrame::OnBnClickedButtonClose()
//{
//	this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_MIN,(WPARAM)(CWnd*)this,0);
//	//this->GetParent()->PostMessage(EB_COMMAND_RAME_WND_CLOSE,(WPARAM)(CWnd*)this,1);
//}