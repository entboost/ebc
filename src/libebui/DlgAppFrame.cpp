// DlgAppFrame.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgAppFrame.h"
#include "DlgAppWindow.h"

#define TIMERID_CHECK_MOUSE_POS			0x130
#define TIMERID_MOVE_WINDOW_SIZE		0x131

#define FRAME_BTN_ID_MIN		101
#define FRAME_BTN_ID_MAX		200

const int const_frame_tool = 28;
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

void CAppWindowInfo::SetAppUrl(const std::string& sUrl)
{
	if (m_sAppUrl != sUrl)
	{
		m_sAppUrl = sUrl;
		if (m_pAppWindow!=NULL)
		{
			m_pAppWindow->Navigate(sUrl);
		}
	}
}

CString CAppWindowInfo::GetLocationURL(void)
{
	return m_pAppWindow==NULL?m_sAppUrl.c_str():m_pAppWindow->GetLocationURL();
}
eb::bigint CAppWindowInfo::GetSubscribeId(void) const
{
	return m_pAppWindow==NULL?0:m_pAppWindow->m_pFuncInfo.m_nSubscribeId;
}

BOOL CAppWindowInfo::Create(LPCTSTR lpszCaption, const RECT& rect, CWnd* pParent, UINT nID, const EB_SubscribeFuncInfo& pFuncInfo, bool nOpenNewClose, int nLeft)
{
	AFX_SWITCH_INSTANCED();
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

		if (!m_btn.Create(sWindowText,WS_CHILD|WS_VISIBLE,rect,pParent,nID))
			return FALSE;
		m_btn.SetFont(pParent->GetFont());
		m_btn.SetToolTipText(lpszCaption);
		m_btn.SetTextHotMove(false);
		m_btn.SetDrawPanelRgn(false);
		m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetBgTitle0(),theApp.GetBgColor3());
		m_btn.SetNorTextColor(theApp.GetTitleTextColor());
		m_btn.SetHotTextColor(theApp.GetTitleTextColor());

		//if (m_pDialog.get()!=NULL)
		//{
		//	const bool bGrayImage = m_pDialog->GetFromIsOffLineState();
		//	m_btn.SetDrawImage(false,bGrayImage,const_cast<Gdiplus::Image*>(m_pDialog->GetFromImage()),Gdiplus::Rect(4,4,28,28));
		//	m_btn.SetTextLeft(32);	// 32=4+28
		//}
	}
	if (m_close.GetSafeHwnd()==NULL)
	{
		const int const_size = 12;
		m_rectClose.right = m_rectBtn.right-3;
		m_rectClose.left = m_rectClose.right - const_size;
		m_rectClose.top = m_rectBtn.top + (m_rectBtn.Height()-const_size)/2;
		m_rectClose.bottom = m_rectClose.top+const_size;
		if (!m_close.Create(_T(""),WS_CHILD,m_rectClose,pParent,nID))
			return FALSE;
		m_close.SetToolTipText(_T("关闭"));
		m_close.SetDrawPanelRgn(false);
		//m_close.SetDrawClosePic(true,RGB(27,27,27),RGB(237,28,36),-1,-1,2);
	}
	CRect rectAppWindow;
	pParent->GetClientRect(&rectAppWindow);
	rectAppWindow.left = nLeft;
	rectAppWindow.top += m_rectBtn.Height();

	if (m_pAppWindow==NULL)
	{
		AFX_SWITCH_INSTANCED();
		m_pAppWindow = new CDlgAppWindow(pParent);
		m_pAppWindow->m_pFuncInfo = pFuncInfo;
		m_pAppWindow->m_bOpenNewClose = nOpenNewClose;
		if (!m_pAppWindow->Create(CDlgAppWindow::IDD,pParent))
		{
			delete m_pAppWindow;
			m_pAppWindow = NULL;
			return FALSE;
		}
		//pParent->ClientToScreen(&rectAppWindow);
		m_pAppWindow->MoveWindow(&rectAppWindow);
		m_pAppWindow->Navigate(m_sAppUrl);
	}
	m_pAppWindow->ShowWindow(SW_SHOW);

	return TRUE;
}
bool CAppWindowInfo::IsCheckedFocus(void)
{
	if (m_btn.GetChecked())
	{
		if (m_pAppWindow!=NULL)
		{
			m_pAppWindow->SetWebFocus();
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
			m_pAppWindow->SetWebFocus();
			//m_pAppWindow->ShowWindow(SW_HIDE);
			//m_pAppWindow->ShowWindow(SW_SHOW);
			//m_pAppWindow->PostMessage(POP_WM_WINDOW_SIZE);
			////m_pAppWindow->Invalidate();
		}
		return true;
	}
	return false;
}

void CAppWindowInfo::MoveSize(int left, int cx, int cy)
{
	if (m_pAppWindow!=NULL)
	{
		CRect rectAppWindow(left,m_rectBtn.Height(),cx,cy);
		m_pAppWindow->MoveWindow(&rectAppWindow);
	}
}
void CAppWindowInfo::SetCtrlColor(void)
{
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetBgTitle0(),theApp.GetBgColor3());
	//if (m_pAppWindow!=NULL)
	//{
	//	m_pAppWindow->SetCtrlColor();
	//}
}

//void CAppWindowInfo::SetFunInfo(const EB_SubscribeFuncInfo& pFuncInfo)
//{
//	if (m_pAppWindow!=NULL)
//	{
//		m_pAppWindow->m_pFuncInfo = pFuncInfo;
//	}
//}

//void CAppWindowInfo::RefreshWindow(void)
//{
//	if (m_pAppWindow!=NULL)
//	{
//		// ***解决WEB控件背景不能刷新问题
//		//m_pAppWindow->RedrawWindow();	// **
//		//m_pAppWindow->Refresh();
//	}
//}
void CAppWindowInfo::SendClose(void)
{
	if (m_pAppWindow!=NULL)
		m_pAppWindow->PostMessage(WM_CLOSE, 0, 0);
}

void CAppWindowInfo::ShowHide(bool bShowAndChecked)
{
	if (m_pAppWindow!=NULL)
	{
		m_pAppWindow->ShowWindow(bShowAndChecked?SW_SHOW:SW_HIDE);
		if (bShowAndChecked)
			m_pAppWindow->SetWebFocus();
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
		if (m_pAppWindow!=NULL && m_pAppWindow->m_pFuncInfo.m_nSubscribeId>0 && !m_pAppWindow->m_pFuncInfo.m_sFunctionName.empty())
		{
			sTooltipText.Format(_T("%s[%s]"),m_pAppWindow->m_pFuncInfo.m_sFunctionName.c_str(),sTitle);
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
		m_btn.SetToolTipText(sTooltipText);
		//m_btn.Invalidate();
	}
}
void CAppWindowInfo::SetIco(void)
{
	if (m_pAppWindow==NULL)
		return;

	const CString sFaviconFile = m_pAppWindow->GetFaviconFile();
	if (!PathFileExists(sFaviconFile))
		return;

	USES_CONVERSION;
	Gdiplus::Image* pImage = Gdiplus::Image::FromFile(A2W_ACP(sFaviconFile));
	if (pImage!=0)
	{
		m_btn.SetDrawImage(true,false,pImage,Gdiplus::Rect(4,7,16,16));
		m_btn.SetTextLeft(20);	// 20=4+18-2
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
	ON_MESSAGE(EBWM_FRAME_WND_CLOSE, OnFrameWndClose)
	ON_MESSAGE(EBWM_FRAME_WND_TITLE, OnFrameWndTitle)
	ON_MESSAGE(EBWM_FRAME_WND_OPEN, OnFrameWndOpen)
	ON_MESSAGE(EBWM_FRAME_VIEW_ICO, OnFrameViewIco)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_TOOLBAR, &CDlgAppFrame::OnBnClickedButtonSwitchToolbar)
	ON_BN_CLICKED(IDC_BUTTON_NEW_WEB, &CDlgAppFrame::OnBnClickedButtonNewWeb)
END_MESSAGE_MAP()


// CDlgAppFrame message handlers
void CDlgAppFrame::SetCtrlColor(void)
{
	m_btnSwitchFrame.SetDrawPanel(true,theApp.GetMainColor());
	m_btnSwitchToolbar.SetDrawPanel(true,theApp.GetMainColor());
	//m_btnSwitchFrame.SetDrawPanel(true,theApp.GetMainColor(),RGB(237,28,36),theApp.GetBgColor3());
	//m_btnSwitchFrame.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetBgTitle0(),theApp.GetBgColor3());
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		pAppWindowInfo->SetCtrlColor();
	}
}
//void CDlgAppFrame::OnMove(void)
//{
//	//BoostReadLock rdlock(m_pList.mutex());
//	//CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
//	//for (; pIter!=m_pList.end(); pIter++)
//	//{
//	//	const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
//	//	//pFrameWndInfo->Move
//	//}
//
//}


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
	m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(250,250,250),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnSwitchToolbar.SetDrawPanelRgn(true);
	m_btnSwitchToolbar.SetWindowText(_T(""));
	m_btnSwitchToolbar.SetToolTipText(_T("隐藏应用导航栏"));
	m_btnSwitchToolbar.SetDrawTrianglePic(3,RGB(250,250,250),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	m_btnNewWeb.SetDrawPanelRgn(false);
	m_btnNewWeb.SetWindowText(_T(""));
	m_btnNewWeb.SetToolTipText(_T("打开新标签页"));
	m_btnNewWeb.SetDrawNewPic(true,RGB(250,250,250),-1,-1,-1,2,11);
	m_btnNewWeb.ShowWindow(SW_SHOW);

	SetCtrlColor();
	this->SetCircle(false);

	SetTimer(TIMERID_CHECK_MOUSE_POS,50,NULL);
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
void CDlgAppFrame::doRefresh(void)
{
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

void CDlgAppFrame::SetCurrentFocus(void)
{
	BoostReadLock rdlock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
		if (pFrameWndInfo->IsCheckedFocus())
			return;
	}
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
			return true;
	}
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CRect rectFuncButton;
	rectFuncButton.left = 2;
	rectFuncButton.right = rectFuncButton.left + const_func_button_size;
	rectFuncButton.top = const_frame_height+2+ m_pMainFuncButtonList.size()*(const_func_button_size+const_func_button_intever);
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
void CDlgAppFrame::AddAppUrl(const std::string& sAppUrl, const EB_SubscribeFuncInfo& pFuncInfo, bool nOpenNewClose)
{
	if (pFuncInfo.m_nSubscribeId>0)	// 订购应用，查询是否已经打开，如果是，直接显示；
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetAppUrl()==sAppUrl)
			{
				rdlock.unlock();
				pFrameWndInfo->doRefresh();
				ShowApp(pFrameWndInfo->GetUserData());
				return;
			}
		}
	}else
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
			if (pFrameWndInfo->GetAppUrl()=="about:blank")
			{
				rdlock.unlock();
				pFrameWndInfo->SetAppUrl(sAppUrl);
				return;
			}
		}
	}

	CRect rectLable;
	rectLable.left = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+m_pList.size()*const_frame_width;
	rectLable.right = rectLable.left+const_frame_width;
	rectLable.top = 0;
	rectLable.bottom = rectLable.top+const_frame_height;

	int nIndexId = 0;
	m_pIndexIdList.front(nIndexId);
	CAppWindowInfo::pointer pAppWindowInfo = CAppWindowInfo::create(sAppUrl);
	pAppWindowInfo->SetUserData(nIndexId);
	if (pAppWindowInfo->Create(pFuncInfo.m_sFunctionName.c_str(),rectLable,this,pAppWindowInfo->GetUserData(),pFuncInfo,nOpenNewClose,(m_bShowedToolbar?const_frame_tool:0)))
	{
		m_pList.add(pAppWindowInfo);
		ShowApp(pAppWindowInfo->GetUserData());

		if (m_btnNewWeb.GetSafeHwnd()!=NULL)
		{
			m_rectNewWeb.left = rectLable.right+2;
			m_rectNewWeb.right = m_rectNewWeb.left+24;
			m_rectNewWeb.top = 3;
			m_rectNewWeb.bottom = m_rectNewWeb.top+24;
			m_btnNewWeb.MoveWindow(&m_rectNewWeb);
		}
	}else
	{
		m_pIndexIdList.pushfront(nIndexId);
	}
}
bool CDlgAppFrame::IsEmpty(void) const
{
	return m_pList.empty();
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
			rectFuncButton.top = const_frame_height + 2 + i*(const_func_button_size+const_func_button_intever);
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
	int nNewWebBtnLeft = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+2;
	{
		int index = 0;
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CAppWindowInfo::pointer pFrameWndInfo = *pIter;
			CRect rectLable;
			rectLable.left = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+(index++)*const_frame_width;
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
	MoveSize(cx, cy);
}

void CDlgAppFrame::OnBnClickedButtonSwitchFrame()
{
	this->GetParent()->PostMessage(WM_RETURN_MAINFRAME);
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
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false));	// 0.45
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
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
				pAppWindowInfo->ShowHide(true);
				SendMsgChangeUrl(pAppWindowInfo);
				if (pAppWindowInfo->IsClickClose(pos))
				{
					bClose = true;
					pAppWindowInfo->SendClose();
					//return;
				}else
				{
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
		pCheckedAppWindowInfo->ShowHide(true);
		SendMsgChangeUrl(pCheckedAppWindowInfo);
	}
}
void CDlgAppFrame::ShowApp(unsigned int nWndIndex)
{
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->GetUserData()==nWndIndex)
			{
				pAppWindowInfo->ShowHide(true);
				SendMsgChangeUrl(pAppWindowInfo);
			}else
			{
				pAppWindowInfo->ShowHide(false);
			}
		}
	}
}
void CDlgAppFrame::DelApp(unsigned int nWndIndex)
{
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
				break;
			}
			pPrevFrameWndInfo = pAppWindowInfo;
		}
	}
	if (m_pList.empty())
	{
		OnBnClickedButtonSwitchFrame();
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		pPrevFrameWndInfo->ShowHide(true);
		SendMsgChangeUrl(pPrevFrameWndInfo);
	}else
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->IsChecked())
			{
				pAppWindowInfo->ShowHide(true);
				SendMsgChangeUrl(pAppWindowInfo);
				pPrevFrameWndInfo.reset();
				break;
			}else if (pPrevFrameWndInfo.get()==NULL)
			{
				pPrevFrameWndInfo = pAppWindowInfo;
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
		{
			pPrevFrameWndInfo->ShowHide(true);
			SendMsgChangeUrl(pPrevFrameWndInfo);
		}
	}
	RebuildBtnSize();
}
void CDlgAppFrame::DelApp(const CWnd* pWnd)
{
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
				break;
			}
			pPrevFrameWndInfo = pAppWindowInfo;
		}
	}
	if (m_pList.empty())
	{
		OnBnClickedButtonSwitchFrame();
		return;
	}else if (pPrevFrameWndInfo.get()!=NULL && pPrevFrameWndInfo->IsChecked())
	{
		pPrevFrameWndInfo->ShowHide(true);
		SendMsgChangeUrl(pPrevFrameWndInfo);
	}else
	{
		BoostReadLock rdLock(m_pList.mutex());
		CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			CAppWindowInfo::pointer pAppWindowInfo = *pIter;
			if (pAppWindowInfo->IsChecked())
			{
				pAppWindowInfo->ShowHide(true);
				SendMsgChangeUrl(pAppWindowInfo);
				pPrevFrameWndInfo.reset();
				break;
			}else if (pPrevFrameWndInfo.get()==NULL)
			{
				pPrevFrameWndInfo = pAppWindowInfo;
			}
		}
		if (pPrevFrameWndInfo.get()!=NULL)
		{
			pPrevFrameWndInfo->ShowHide(true);
			SendMsgChangeUrl(pPrevFrameWndInfo);
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
			if (sTitle!=_T("about:blank") || sTitle==pAppWindowInfo->GetAppUrl().c_str())
				SendMsgChangeUrl(pAppWindowInfo);
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

void CDlgAppFrame::SendMsgChangeUrl(const CAppWindowInfo::pointer& pAppWindowInfo)
{
	//if (pAppWindowInfo->GetSubscribeId()==0)
	{
		const CString sUrl(pAppWindowInfo->GetLocationURL());
		char * lpszBuffer = new char[sUrl.GetLength()+1];
		strcpy(lpszBuffer,sUrl);
		this->GetParent()->PostMessage(EB_COMMAND_CHANGE_APP_URL,(WPARAM)lpszBuffer,0);
	}
}

void CDlgAppFrame::ShowFirstWnd(void)
{
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;
		pAppWindowInfo->ShowHide(true);
		SendMsgChangeUrl(pAppWindowInfo);
		return;
	}
}
void CDlgAppFrame::RebuildBtnSize(void)
{
	int nNewWebBtnLeft = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+2;
	int nIndex = 0;
	BoostReadLock rdLock(m_pList.mutex());
	CLockList<CAppWindowInfo::pointer>::iterator pIter = m_pList.begin();
	for (; pIter!=m_pList.end(); pIter++)
	{
		CAppWindowInfo::pointer pAppWindowInfo = *pIter;

		CRect rectLable;
		rectLable.left = (m_bShowedToolbar?const_frame_tool:DEFAULT_RETURN_BTN_WIDTH)+(nIndex++)*const_frame_width;
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
void CDlgAppFrame::RefreshAppWnd(void)
{
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
						m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(61,61,61),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
					}else
					{
						m_btnSwitchFrame.SetDrawTrianglePic(3,RGB(250,250,250),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
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
							m_btnSwitchToolbar.SetDrawTrianglePic(3,RGB(61,61,61),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
						else
							m_btnSwitchToolbar.SetDrawTrianglePic(4,RGB(61,61,61),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
					}else
					{
						if (m_bShowedToolbar)
							m_btnSwitchToolbar.SetDrawTrianglePic(3,RGB(250,250,250),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
						else
							m_btnSwitchToolbar.SetDrawTrianglePic(4,RGB(250,250,250),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
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
						m_btnNewWeb.SetDrawNewPic(true,RGB(61,61,61),-1,-1,-1,2,11);
					}else
					{
						m_btnNewWeb.SetDrawNewPic(true,RGB(250,250,250),-1,-1,-1,2,11);
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

LRESULT CDlgAppFrame::OnFrameWndOpen(WPARAM wParam, LPARAM lParam)
{
	char * sUrl = (char*)wParam;
	if (sUrl==0 || strlen(sUrl)==0) return 0;
	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = sUrl;
	pSubscribeFuncInfo.m_bDisableContextMenu = (lParam&0x1)!=0;;
	pSubscribeFuncInfo.m_bDisableScrollBar = (lParam&0x2)!=0;
	pSubscribeFuncInfo.m_bClosePrompt = (lParam&0x4)!=0;
	pSubscribeFuncInfo.m_bWinResizable = true;
	const bool nOpenNewClose = (lParam&0x8)!=0;
	this->AddAppUrl(sUrl,pSubscribeFuncInfo,nOpenNewClose);
	delete[] sUrl;
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
		m_btnSwitchToolbar.SetDrawTrianglePic(4,RGB(61,61,61),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
	}else
	{
		m_btnSwitchToolbar.SetToolTipText(_T("隐藏应用导航栏"));
		m_btnSwitchToolbar.SetDrawTrianglePic(3,RGB(61,61,61),-1,-1,-1,DEFAULT_TRIANGLE_BTN_WIDTH,DEFAULT_TRIANGLE_BTN_HEIGHT);
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
		theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo);
	}
}

void CDlgAppFrame::OnBnClickedButtonNewWeb()
{
	const std::string sAppUrl("about:blank");
	EB_SubscribeFuncInfo pSubscribeFuncInfo;
	pSubscribeFuncInfo.m_sFunctionName = sAppUrl;
	pSubscribeFuncInfo.m_bClosePrompt = false;
	pSubscribeFuncInfo.m_bDisableScrollBar = false;
	pSubscribeFuncInfo.m_bDisableContextMenu = false;
	pSubscribeFuncInfo.m_bWinResizable = true;
	AddAppUrl(sAppUrl,pSubscribeFuncInfo,false);
}
