
// ebscDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ebsc.h"
#include "ebscDlg.h"
#include "SkinMemDC.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMERID_DRAW 0x100

const int const_toolext_left = 0;//24*2;	// 
const int const_toolext_width = 6+24*3;	// 

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CebscDlg dialog




CebscDlg::CebscDlg(CWnd* pParent /*=NULL*/)
	: EBDCBASEDIALOG(CebscDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bMouseDown = NULL;
	m_bSelectFinished = FALSE;
	m_nMsgID = 0;
	m_hWindow = NULL;
	m_bMinWindow = false;
	m_hMinWindow = NULL;
	m_bShowBottom = true;
}

void CebscDlg::DoDataExchange(CDataExchange* pDX)
{
	EBDCBASEDIALOG::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CebscDlg, EBDCBASEDIALOG)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_MESSAGE(EBSC_WM_TOOLEXT_POINT, OnMessageToolextPoint)
	ON_MESSAGE(EBSC_WM_SELECT_MOVE, OnMessageSelectMove)
	ON_MESSAGE(EBSC_WM_SELECT_FINISH, OnMessageToolsFinish)
	ON_MESSAGE(EBSC_WM_TOOLS_FINISH, OnMessageToolsFinish)
	ON_MESSAGE(EBSC_WM_TOOLS_CLICK_COLOR, OnMessageToolsClieckColor)
	ON_MESSAGE(EBSC_WM_TOOLS_CLICK_TEXT, OnMessageToolsClieckText)
	ON_MESSAGE(EBSC_WM_TOOLS_CLICK_RECTANGLE, OnMessageToolsClieckRectangle)
	ON_MESSAGE(EBSC_WM_TOOLS_CLICK_CIRCLE, OnMessageToolsClieckCircle)
	ON_MESSAGE(EBSC_WM_TOOLS_CLICK_ARROW, OnMessageToolsClieckArrow)
	ON_MESSAGE(EBSC_WM_TOOLS_CLICK_CANCEL, OnMessageToolsClieckCancel)
	ON_MESSAGE(EBSC_WM_TOOLS_CLICK_SAVEAS, OnMessageToolsClieckSaveas)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CebscDlg message handlers

BOOL CebscDlg::OnInitDialog()
{
	EBDCBASEDIALOG::OnInitDialog();
	//this->SetTransparentType(TT_DLG);
	//this->SetTransparent(120);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	//m_sScreenImageFile

	m_pDlgSelect.Create(CDlgSelect::IDD,this);
	m_pDlgTools.Create(CDlgTools::IDD,this);
	m_pDlgToolExt.Create(CDlgToolExt::IDD,this);
	this->MoveWindow(0,0,theApp.m_pDesktopWindow.GetWidth(),theApp.m_pDesktopWindow.GetHeight());
	this->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	//this->SetActiveWindow();
	//this->SetForegroundWindow();
	
	//m_pDlgSelect.MoveWindow(0,0,theApp.m_pDesktopWindow.GetWidth(),theApp.m_pDesktopWindow.GetHeight());

	////Gdiplus::Bitmap::from
	//BITMAPINFO bmpinfo;
	//bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//bmpinfo.bmiHeader.biWidth = theApp.m_pDesktopWindow.GetWidth();
	//bmpinfo.bmiHeader.biHeight = theApp.m_pDesktopWindow.GetHeight();
	//bmpinfo.bmiHeader.biPlanes = 1;
	//bmpinfo.bmiHeader.biBitCount = 32;
	//bmpinfo.bmiHeader.biCompression = BI_RGB;
	//bmpinfo.bmiHeader.biSizeImage = 0;
	//bmpinfo.bmiHeader.biXPelsPerMeter = 0;//100;
	//bmpinfo.bmiHeader.biYPelsPerMeter = 0;//100;
	//bmpinfo.bmiHeader.biClrUsed = 0;
	//bmpinfo.bmiHeader.biClrImportant = 0;
	//m_imageScreen = Gdiplus::Bitmap::FromBITMAPINFO(&bmpinfo,(void*)m_pDesktopWindow.GetScreenData());

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon


	// TODO: Add extra initialization here
	//SetTimer(TIMERID_DRAW,10,NULL);
	SetWindowText(theWindowName);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CebscDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		EBDCBASEDIALOG::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CebscDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// Center icon in client rectangle
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// Draw the icon
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		Draw();
		//EBDCBASEDIALOG::OnPaint();
	}
}
void CebscDlg::Draw(void)
{
	CPaintDC dc2(this);
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	Gdiplus::Graphics graphics(memDC.m_hDC);

	Gdiplus::RectF drawClient(0,0,theApp.m_pDesktopWindow.GetWidth(),theApp.m_pDesktopWindow.GetHeight());
	graphics.DrawImage(theApp.m_imageScreen,drawClient);
	Gdiplus::SolidBrush brushSelectBack(Gdiplus::Color(120,0,0,0));
	//Gdiplus::SolidBrush brushSelectBack(Gdiplus::Color(100,153,153,153));
	//if (m_bMouseDown)
	//{
	//	Gdiplus::Rect rectleft(0,0,m_pSelectBegin.X,rectClient.Height());
	//	Gdiplus::Rect recttop(0,0,rectClient.Width(),m_pSelectBegin.Y);	// ?
	//	Gdiplus::Rect rectright(m_pSelectEnd.X,0,rectClient.Width()-m_pSelectEnd.X,rectClient.Height());	// ?
	//	Gdiplus::Rect rectbottom(0,m_pSelectEnd.Y,rectClient.Width(),rectClient.Height()-m_pSelectEnd.Y);	// ?
	//	graphics.FillRectangle(&brushSelectBack,rectleft);
	//	graphics.FillRectangle(&brushSelectBack,recttop);
	//	graphics.FillRectangle(&brushSelectBack,rectright);
	//	graphics.FillRectangle(&brushSelectBack,rectbottom);
	//}else
	{
		graphics.FillRectangle(&brushSelectBack,drawClient);
	}

	//if (m_pDlgSelect.IsWindowVisible())
	//{
	//	Gdiplus::SolidBrush brushSelectBack2(Gdiplus::Color(250,0,0,0));
	//	CRect rect;
	//	m_pDlgSelect.GetClientRect(&rect);
	//	this->ClientToScreen(&rect);
	//	Gdiplus::RectF drawClient2(rect.left,rect.top-20,100,20);
	//	graphics.FillRectangle(&brushSelectBack2,drawClient2);

	//	const Gdiplus::FontFamily fontFamily(L"宋体");
	//	const Gdiplus::Font fontEbTitle(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	//	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	//	Gdiplus::StringFormat formatAdText;
	//	formatAdText.SetAlignment(Gdiplus::StringAlignmentNear);
	//	formatAdText.SetLineAlignment(Gdiplus::StringAlignmentNear);
	//	graphics.DrawString(L"adsfadsfadsf",-1,&fontEbTitle,drawClient2,&formatAdText,&brushEbTitle);
	//}

}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CebscDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CebscDlg::OnDestroy()
{
	EBDCBASEDIALOG::OnDestroy();

	m_pDlgSelect.DestroyWindow();
	m_pDlgTools.DestroyWindow();
	m_pDlgToolExt.DestroyWindow();
}

void CebscDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pos;
	::GetCursorPos(&pos);
	m_pSelectBegin = Gdiplus::Point(pos.x,pos.y);
	theLastDrawEndPoint = m_pSelectBegin;
	m_bMouseDown = TRUE;
	SetCapture();
	m_bSelectFinished = FALSE;
	m_pDlgSelect.ShowWindow(SW_HIDE);
	m_pDlgSelect.Reset();
	m_pDlgTools.ShowWindow(SW_HIDE);
	m_pDlgTools.Reset();
	m_pDlgToolExt.ShowWindow(SW_HIDE);
	m_pDlgToolExt.Reset();

	EBDCBASEDIALOG::OnLButtonDown(nFlags, point);
}
void CebscDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bMouseDown)
	{
		// 这里显示选择区域
		CPoint pos;
		::GetCursorPos(&pos);
		m_pSelectEnd = Gdiplus::Point(pos.x,pos.y);

		CRect rect;
		rect.left = min(m_pSelectBegin.X,m_pSelectEnd.X);
		rect.top = min(m_pSelectBegin.Y,m_pSelectEnd.Y);
		rect.right = max(m_pSelectBegin.X,m_pSelectEnd.X);
		rect.bottom = max(m_pSelectBegin.Y,m_pSelectEnd.Y);
		m_pDlgSelect.MoveWindow(&rect);
		if (!m_pDlgSelect.IsWindowVisible())
			m_pDlgSelect.ShowWindow(SW_SHOW);
	}

	EBDCBASEDIALOG::OnMouseMove(nFlags, point);
}
void CebscDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		ReleaseCapture();
		CPoint pos;
		::GetCursorPos(&pos);
		if (pos.x != m_pSelectBegin.X && pos.y != m_pSelectBegin.Y)
		{
			m_bSelectFinished = TRUE;
			MoveTools();
		}
	}

	EBDCBASEDIALOG::OnLButtonUp(nFlags, point);
}
void CebscDlg::MoveTools(void)
{
	if (m_bSelectFinished)
	{
		const int const_tools_width = 61+24*7;	// 
		const int const_tools_height = 28;
		const int const_tools_inteval = 3;
		CRect rect;
		m_pDlgSelect.GetWindowRect(&rect);
		if (rect.bottom+const_tools_height >= theApp.m_pDesktopWindow.GetHeight())
		{
			rect.bottom = rect.top - const_tools_inteval;
			rect.top = rect.bottom - const_tools_height;
			rect.left = rect.right - const_tools_width;
			m_bShowBottom = false;
		}else
		{
			rect.top = rect.bottom + const_tools_inteval;
			rect.bottom = rect.top + const_tools_height;
			rect.left = rect.right - const_tools_width;
			m_bShowBottom = true;
		}

		m_pDlgTools.MoveWindow(&rect);
		if (!m_pDlgTools.IsWindowVisible())
		{
			//m_pDlgTools.SetCircle(true);
			m_pDlgTools.ShowWindow(SW_SHOW);
		}
		m_pDlgTools.SetFocus();

		if (m_pDlgToolExt.GetSafeHwnd()!=NULL && m_pDlgToolExt.IsWindowVisible())
		{
			CRect rectExt;
			rectExt.left = rect.left+const_toolext_left;
			rectExt.right = rectExt.left+const_toolext_width;
			if (m_pDlgSelect.GetDrawType()==CDrawInfo::DRAW_RECTANGLE ||
				m_pDlgSelect.GetDrawType()==CDrawInfo::DRAW_CIRCLE)
			{
				rectExt.right += 24;
			}
			if (m_bShowBottom)
			{
				rectExt.top = rect.bottom+2;
				rectExt.bottom = rectExt.top+rect.Height();
			}else
			{
				rectExt.bottom = rect.top-2;
				rectExt.top = rectExt.bottom-rect.Height();
			}
			m_pDlgToolExt.MoveWindow(&rectExt);
			m_pDlgToolExt.SetFocus();
		}
	}
}

void CebscDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TIMERID_DRAW == nIDEvent)
	{
		if (m_bMouseDown)
		{
			//if (theLastDrawEndPoint.X != m_pSelectEnd.X || theLastDrawEndPoint.Y != m_pSelectEnd.Y)
			//{
			//	const int const_offset = 10;
			//	CRect rect;
			//	rect.left = min(m_pSelectBegin.X-const_offset,m_pSelectEnd.X-const_offset);
			//	rect.top = min(m_pSelectBegin.Y-const_offset,m_pSelectEnd.Y-const_offset);
			//	rect.right = max(m_pSelectBegin.X+const_offset,m_pSelectEnd.X+const_offset);
			//	rect.bottom = max(m_pSelectBegin.Y+const_offset,m_pSelectEnd.Y+const_offset);
			//	this->InvalidateRect(&rect);
			//	theLastDrawEndPoint = m_pSelectEnd;
			//}
		}
	}

	EBDCBASEDIALOG::OnTimer(nIDEvent);
}

LRESULT CebscDlg::OnMessageToolextPoint(WPARAM wParam, LPARAM lParam)
{
	const int nPointType = (int)wParam;
	m_pDlgSelect.SetPointType(nPointType);
	return 0;
}

LRESULT CebscDlg::OnMessageSelectMove(WPARAM wParam, LPARAM lParam)
{
	MoveTools();
	return 0;
}
LRESULT CebscDlg::OnMessageToolsFinish(WPARAM wParam, LPARAM lParam)
{
	if( OpenClipboard() )
	{
		EmptyClipboard();

		// 复制当前选择界面屏幕到张粘板
		CDesktopWindow pDesktopWindow;
		if (!pDesktopWindow.Create())
		{
			CloseClipboard();
			return FALSE;
		}
		Gdiplus::Image * m_imageScreen = Gdiplus::Bitmap::FromHBITMAP(pDesktopWindow.CopyScreen(),NULL);

		CRect rect;
		m_pDlgSelect.GetClientRect(&rect);
		m_pDlgSelect.ClientToScreen(&rect);
		rect.left += const_border_width;
		rect.right -= const_border_width;
		rect.top += const_border_width;
		rect.bottom -= const_border_width;
		CPaintDC dc2(this);
		CDC		dcMem;
		dcMem.CreateCompatibleDC(&dc2);
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&dc2, rect.Width(), rect.Height());
		CBitmap * pOldBitmap = dcMem.SelectObject(&bitmap);
		Gdiplus::Graphics graphics(dcMem.m_hDC);
		graphics.DrawImage(m_imageScreen,0,0,rect.left,rect.top,rect.Width(),rect.Height(),Gdiplus::Unit::UnitPixel);
		SetClipboardData(CF_BITMAP,bitmap.m_hObject);
		CloseClipboard();
		dcMem.SelectObject(pOldBitmap);
		bitmap.DeleteObject();
		dcMem.DeleteDC();
		delete m_imageScreen;
		pDesktopWindow.Release();

		if ((m_hWindow != NULL || !m_sWindowText.empty()) && m_nMsgID > WM_USER)
		{
			// 截图成功
			if (m_bMinWindow && m_hMinWindow!=NULL)
			{
				::ShowWindow(m_hMinWindow,SW_SHOW);
				//::ShowWindow(m_hMinWindow,SW_RESTORE);
				//::SetForegroundWindow(m_hMinWindow);
				//::SetActiveWindow(m_hMinWindow);
			}

			this->ShowWindow(SW_HIDE);
			HWND l_hWnd = m_hWindow;
			if (l_hWnd == NULL)
				::FindWindow(NULL, m_sWindowText.c_str());
			if (l_hWnd!=NULL)
			{
				if (TRUE == ::IsIconic(l_hWnd))
				{
					::ShowWindow(l_hWnd,SW_RESTORE); //还原窗口
					//::ShowWindow(l_hWnd,SW_SHOWNORMAL); //还原窗口
				}
				::SetForegroundWindow(l_hWnd); //使窗口在最前方
				::PostMessage(l_hWnd,m_nMsgID,0,0);
				//// Ctrl+V
				////Send   control   键
				//BYTE     pbKeyState   [256];
				//GetKeyboardState   ((LPBYTE)&pbKeyState);
				//pbKeyState[VK_CONTROL]   |=   0x80;
				//SetKeyboardState   ((LPBYTE)&pbKeyState);
				////Send   'v '   键
				////UINT wParam = 0x56;
				////::keybd_event(wParam,0,0,0);				//按下v
				////::keybd_event(wParam,0,KEYEVENTF_KEYUP, 0);	//放开v
				//::SendMessageA (l_hWnd, WM_KEYDOWN, 0x56, 0);
				//::SendMessageA (l_hWnd, WM_KEYUP, 0x56, 0);
			}
		}

	}
	this->PostMessage(WM_CLOSE);
	return 0;
}
void CebscDlg::UpdateToolExt(void)
{
	const bool ret = m_pDlgToolExt.SetDrawType(m_pDlgSelect.GetDrawType());

	switch (m_pDlgSelect.GetDrawType())
	{
	case CDrawInfo::DRAW_RECTANGLE:
	case CDrawInfo::DRAW_CIRCLE:
	case CDrawInfo::DRAW_ARROW:
	case CDrawInfo::DRAW_TEXT:
		{
			if (ret || !m_pDlgToolExt.IsWindowVisible())
			{
				CRect rectTool;
				m_pDlgTools.GetWindowRect(&rectTool);
				CRect rectExt;
				rectExt.left = rectTool.left+const_toolext_left;
				rectExt.right = rectExt.left+const_toolext_width;
				if (m_pDlgSelect.GetDrawType()==CDrawInfo::DRAW_RECTANGLE ||
					m_pDlgSelect.GetDrawType()==CDrawInfo::DRAW_CIRCLE)
				{
					rectExt.right += 24;
				}
				if (m_bShowBottom)
				{
					rectExt.top = rectTool.bottom+2;
					rectExt.bottom = rectExt.top+rectTool.Height();
				}else
				{
					rectExt.bottom = rectTool.top-2;
					rectExt.top = rectExt.bottom-rectTool.Height();
				}
				m_pDlgToolExt.MoveWindow(&rectExt);
				m_pDlgToolExt.ShowWindow(SW_SHOW);
			}
		}break;
	default:
		m_pDlgToolExt.ShowWindow(SW_HIDE);
		break;
	}
}

LRESULT CebscDlg::OnMessageToolsClieckColor(WPARAM wParam, LPARAM lParam)
{
	m_pDlgSelect.SetToolsColor();
	return 0;
}
LRESULT CebscDlg::OnMessageToolsClieckText(WPARAM wParam, LPARAM lParam)
{
	const bool bChecked = (bool)(wParam==1);
	m_pDlgSelect.SetToolsText(bChecked);
	UpdateToolExt();
	return 0;
}
LRESULT CebscDlg::OnMessageToolsClieckRectangle(WPARAM wParam, LPARAM lParam)
{
	const bool bChecked = (bool)(wParam==1);
	m_pDlgSelect.SetToolsRectangle(bChecked);
	UpdateToolExt();
	return 0;
}
LRESULT CebscDlg::OnMessageToolsClieckCircle(WPARAM wParam, LPARAM lParam)
{
	const bool bChecked = (bool)(wParam==1);
	m_pDlgSelect.SetToolsCircle(bChecked);
	UpdateToolExt();
	return 0;
}
LRESULT CebscDlg::OnMessageToolsClieckArrow(WPARAM wParam, LPARAM lParam)
{
	const bool bChecked = (bool)(wParam==1);
	m_pDlgSelect.SetToolsArrow(bChecked);
	UpdateToolExt();
	return 0;
}
LRESULT CebscDlg::OnMessageToolsClieckCancel(WPARAM wParam, LPARAM lParam)
{
	m_pDlgSelect.SetToolsCancel();
	return 0;
}
//#define USES_CFILEDIALOG
LRESULT CebscDlg::OnMessageToolsClieckSaveas(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgSelect.SetToolsSaveas())
	{
		this->PostMessage(WM_CLOSE);
	}
	return 0;
//
//	CDesktopWindow pDesktopWindow;
//	if (!pDesktopWindow.Create())
//	{
//		return 0;
//	}
//	Gdiplus::Image * m_imageScreen = Gdiplus::Bitmap::FromHBITMAP(pDesktopWindow.CopyScreen(),NULL);
//	CRect rect;
//	m_pDlgSelect.GetClientRect(&rect);
//	m_pDlgSelect.ClientToScreen(&rect);
//	rect.left += const_border_width;
//	rect.right -= const_border_width;
//	rect.top += const_border_width;
//	rect.bottom -= const_border_width;
//	CPaintDC dc2(this);
//	CDC		dcMem;
//	dcMem.CreateCompatibleDC(&dc2);
//	CBitmap bitmap;
//	bitmap.CreateCompatibleBitmap(&dc2, rect.Width(), rect.Height());
//	CBitmap * pOldBitmap = dcMem.SelectObject(&bitmap);
//	Gdiplus::Graphics graphics(dcMem.m_hDC);
//	graphics.DrawImage(m_imageScreen,0,0,rect.left,rect.top,rect.Width(),rect.Height(),Gdiplus::Unit::UnitPixel);
//	Gdiplus::Bitmap * pImageSaveas = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)bitmap.m_hObject,NULL);
//	if (pImageSaveas!=NULL)
//	{
//		TCHAR szFileName[MAX_PATH*2] = {0} ; 
//		const time_t tNow = time(0);
//		const struct tm *newtime = localtime(&tNow);
//		_stprintf(szFileName,_T("EB截图%04d%02d%02d%02d%02d%02d"),newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);
//
//#ifdef USES_CFILEDIALOG
//		CFileDialog dlg(FALSE,_T("png"),szFileName,OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,_T("PNG (*.png)\0*.png\0JPEG (*.jpg;*.jpeg)\0*.jpg\0BMP (*.bmp)\0*.bmp\0\0"),this,0,TRUE);
//		if (dlg.DoModal()==IDOK)
//#else
//		OPENFILENAME ofn={0};
//		ofn.lStructSize = sizeof(OPENFILENAME);
//		ofn.hwndOwner = m_pDlgSelect.GetSafeHwnd();
//		ofn.lpstrFile = szFileName;
//		ofn.nMaxFile = sizeof(szFileName);
//		ofn.lpstrFilter = "PNG (*.png)\0*.png\0JPEG (*.jpg;*.jpeg)\0*.jpg\0BMP (*.bmp)\0*.bmp\0\0";
//		ofn.lpstrDefExt = "png";   
//		ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
//		if (GetSaveFileName(&ofn)==TRUE )
//#endif
//		{
//#ifdef USES_CFILEDIALOG
//			CString sExt(dlg.GetFileName());
//#else
//			CString sExt(ofn.lpstrFile);
//#endif
//			sExt = sExt.Right(3);
//			sExt.MakeLower();
//			CLSID                           codecClsid;
//			if (sExt=="png")
//				GetCodecClsid(L"image/png",   &codecClsid);
//			else if (sExt=="bmp")
//				GetCodecClsid(L"image/bmp",   &codecClsid);
//			else
//				GetCodecClsid(L"image/jpeg",   &codecClsid);
//
//			USES_CONVERSION;
//#ifdef USES_CFILEDIALOG
//			pImageSaveas->Save(T2W(dlg.GetPathName()),&codecClsid);
//#else
//			pImageSaveas->Save(T2W(ofn.lpstrFile),&codecClsid);
//#endif
//		}
//		delete pImageSaveas;
//	}
//	dcMem.SelectObject(pOldBitmap);
//	bitmap.DeleteObject();
//	dcMem.DeleteDC();
//	delete m_imageScreen;
//	pDesktopWindow.Release();
//	return 0;
}

BOOL CebscDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return TRUE;
	return CDialogSK::OnEraseBkgnd(pDC);
}

void CebscDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// **如果右键选择区域，这里不会响应，也就是不会退出；
	//if (!m_pDlgSelect.IsWindowVisible())
	{
		this->OnCancel();
	}

	CDialogSK::OnRButtonDown(nFlags, point);
}
