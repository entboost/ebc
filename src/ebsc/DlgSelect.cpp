// DlgSelect.cpp : implementation file
//

#include "stdafx.h"
#include "ebsc.h"
#include "DlgSelect.h"
#include "SkinMemDC.h"
#include <CGCBase/cgcSmartString.h>
//#include "../include/ebstring.h"
//un

std::wstring theFontFamily;	// "Times New Roman"
const std::wstring theFontFamily0 = L"微软雅黑";
const std::wstring theFontFamily1 = L"宋体";

#define CORNER_SIZE 2
#define IDC_STA_SPLITTER_LEFT	0x15001
#define IDC_STA_SPLITTER_TOP	0x15002
#define IDC_STA_SPLITTER_RIGHT	0x15003
#define IDC_STA_SPLITTER_BOTTOM 0x15004

#define POP_WM_WINDOW_SIZE (WM_USER+301)

// CDlgSelect dialog
//#define TIMERID_DRAW 100

IMPLEMENT_DYNAMIC(CDlgSelect, CDialogSK)

CDlgSelect::CDlgSelect(CWnd* pParent /*=NULL*/)
	: CDialogSK(CDlgSelect::IDD, pParent)
{
	m_nDrawType = CDrawInfo::DRAW_NULL;
	m_fDrawPointWidth = 1.0;
	m_nFontSize = 14;
	m_bMouseDown = NULL;
	m_colorTools = Gdiplus::Color(255,237,28,36);
}

CDlgSelect::~CDlgSelect()
{
}

void CDlgSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_editText);
}


BEGIN_MESSAGE_MAP(CDlgSelect, CDialogSK)
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(POP_WM_WINDOW_SIZE, OnMessageWindowResize)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT_TEXT, &CDlgSelect::OnEnChangeEditText)
END_MESSAGE_MAP()


// CDlgSelect message handlers
//#define THE_TRANSPARENNT_COLOR RGB(153, 153, 153)
BOOL CALLBACK MyEnumFonts(CONST LOGFONTW* lplf, CONST TEXTMETRICW *lptm,DWORD dwType,LPARAM lparam)
{
	const wchar_t * lpszFine = (const wchar_t*)lparam;
	const std::wstring tempFontName(lplf->lfFaceName);
	if(tempFontName.find(lpszFine)!=std::wstring::npos)
	{
		theFontFamily = lpszFine;
		return false;
	}
	return true;
}
BOOL CDlgSelect::OnInitDialog()
{
	CDialogSK::OnInitDialog();
	this->SetTransparentType(TT_EDIT);
	//this->SetTransparent(100);
	//this->SetTransparentColor(THE_TRANSPARENNT_COLOR);

	CPaintDC dc((CWnd*)this);
	::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily1.c_str());	// 
	::EnumFontsW(dc.m_hDC,NULL,(FONTENUMPROCW)MyEnumFonts,(LPARAM)theFontFamily0.c_str());

	SetEditFont();
	//LOGFONT pLogFont;
	//memset(&pLogFont,0,sizeof(pLogFont));
	//pLogFont.lfCharSet = GB2312_CHARSET;
	//pLogFont.lfWeight = FW_NORMAL;	// 正常，标准
	//pLogFont.lfWidth = -9;			// -7=14 -8=16
	//if (theFontFamily==theFontFamily0)
	//	pLogFont.lfHeight = 20;
	//else
	//	pLogFont.lfHeight = 16;
	//USES_CONVERSION;
	//strcpy(pLogFont.lfFaceName,W2A_ACP(theFontFamily.c_str()));
	//CFont* pFont = new CFont(); 
	//pFont->CreateFontIndirect(&pLogFont);
	//m_editText.SetFont(pFont);
	m_editText.SetTextColor(m_colorTools.ToCOLORREF());

	//m_editText.SetTextColor(RGB(237,28,36));

	//COLORREF colorSplitter = RGB(0,174,255);
	m_wndSplitterLeft.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_LEFT);
	m_wndSplitterLeft.SetStyle(SPS_VERTICAL);
	m_wndSplitterLeft.SetTransparent(true);
	//m_wndSplitterLeft.SetColor(colorSplitter,colorSplitter);
	m_wndSplitterTop.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_TOP);
	m_wndSplitterTop.SetStyle(SPS_HORIZONTAL);
	m_wndSplitterTop.SetTransparent(true);
	//m_wndSplitterTop.SetColor(colorSplitter,colorSplitter);
	m_wndSplitterRight.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_RIGHT);
	m_wndSplitterRight.SetStyle(SPS_VERTICAL);
	m_wndSplitterRight.SetTransparent(true);
	//m_wndSplitterRight.SetColor(colorSplitter,colorSplitter);
	m_wndSplitterBottom.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,1,1), this, IDC_STA_SPLITTER_BOTTOM);
	m_wndSplitterBottom.SetStyle(SPS_HORIZONTAL);
	m_wndSplitterBottom.SetTransparent(true);
	////m_wndSplitterBottom.SetColor(colorSplitter,colorSplitter);

	//SetTimer(TIMERID_DRAW,500,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgSelect::SetEditFont(void)
{
	LOGFONT pLogFont;
	memset(&pLogFont,0,sizeof(pLogFont));
	pLogFont.lfCharSet = GB2312_CHARSET;
	pLogFont.lfWeight = FW_NORMAL;	// 正常，标准
	if (m_nFontSize<=14)
	{
		pLogFont.lfWidth = -7;			// -7=14
		if (theFontFamily==theFontFamily0)
			pLogFont.lfHeight = 20;
		else
			pLogFont.lfHeight = 16;
	}else if (m_nFontSize<=17)
	{
		pLogFont.lfWidth = -8;			// -8=17
		if (theFontFamily==theFontFamily0)
			pLogFont.lfHeight = 23;
		else
			pLogFont.lfHeight = 19;
	}else
	{
		pLogFont.lfWidth = -10;			// -10=20
		if (theFontFamily==theFontFamily0)
			pLogFont.lfHeight = 27;
		else
			pLogFont.lfHeight = 23;
	}
	USES_CONVERSION;
	strcpy(pLogFont.lfFaceName,W2A_ACP(theFontFamily.c_str()));
	CFont* pFont = new CFont(); 
	pFont->CreateFontIndirect(&pLogFont);
	m_editText.SetFont(pFont);
}

LRESULT CDlgSelect::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STA_SPLITTER_LEFT)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				CSplitterControl::ChangeWidth(this, -pHdr->delta, CW_RIGHTALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
			}
		}else if (wParam == IDC_STA_SPLITTER_TOP)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				CSplitterControl::ChangeHeight(this, -pHdr->delta, CW_BOTTOMALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
			}
		}else if (wParam == IDC_STA_SPLITTER_RIGHT)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				CSplitterControl::ChangeWidth(this, pHdr->delta, CW_LEFTALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
			}
		}else if (wParam == IDC_STA_SPLITTER_BOTTOM)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			if (pHdr->delta != 0)
			{
				CSplitterControl::ChangeHeight(this, pHdr->delta, CW_TOPALIGN);
				this->PostMessage(POP_WM_WINDOW_SIZE);
			}
		}
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}
LRESULT CDlgSelect::OnMessageWindowResize(WPARAM wParam, LPARAM lParam)
{
	//this->Invalidate();
	this->GetParent()->PostMessage(EBSC_WM_SELECT_MOVE);
	return 0;
}

void CDlgSelect::OnMove(int x, int y)
{
	CDialogSK::OnMove(x, y);
	this->Invalidate();
	this->GetParent()->PostMessage(EBSC_WM_SELECT_MOVE);
}

void CDlgSelect::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogSK::OnPaint() for painting messages

	//CRect rect;
	//CPaintDC dc(this);
	//GetClientRect(rect);
	//dc.FillSolidRect(rect,THE_TRANSPARENNT_COLOR); //设置为绿色背景
	//CDialog::OnPaint();
	//return;

	Draw();
}

void CDlgSelect::Draw(void)
{
	CPaintDC dc2(this);
	dc2.SetBkMode(TRANSPARENT);
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	Gdiplus::Graphics graphics(memDC.m_hDC);

	this->ClientToScreen(&rectClient);
	// 画背景图
	graphics.DrawImage(theApp.m_imageScreen,0,0,rectClient.left,rectClient.top,rectClient.Width(),rectClient.Height(),Gdiplus::Unit::UnitPixel);
	// 画边框
	Gdiplus::Color colorBorder(255,86,190,245);
	Gdiplus::Pen penBorder(colorBorder,const_border_width);
	graphics.DrawRectangle(&penBorder,0,0,rectClient.Width(),rectClient.Height());

	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);	// 消除锯齿
	// 画已经画好图形
	for (size_t i=0;i<m_pDrawList.size();i++)
	{
		Draw(graphics,m_pDrawList[i]);
	}

	// 画当前图形
	if (m_bMouseDown)
	{
		CRect rect;
		rect.left = min(m_pSelectBegin.X,m_pSelectEnd.X);
		rect.top = min(m_pSelectBegin.Y,m_pSelectEnd.Y);
		rect.right = max(m_pSelectBegin.X,m_pSelectEnd.X);
		rect.bottom = max(m_pSelectBegin.Y,m_pSelectEnd.Y);
		this->ScreenToClient(&rect);
		Gdiplus::Rect rectLayout(rect.left,rect.top,rect.Width(),rect.Height());
		Draw(graphics,CDrawInfo(m_nDrawType,m_fDrawPointWidth,m_pSelectBegin,m_pSelectEnd,m_colorTools));
	}
	//switch (m_nDrawType)
	//{
	//case CDrawInfo::DRAW_RECTANGLE:
	//	{
	//		CRect rect;
	//		rect.left = min(m_pSelectBegin.X,m_pSelectEnd.X);
	//		rect.top = min(m_pSelectBegin.Y,m_pSelectEnd.Y);
	//		rect.right = max(m_pSelectBegin.X,m_pSelectEnd.X);
	//		rect.bottom = max(m_pSelectBegin.Y,m_pSelectEnd.Y);
	//		this->ScreenToClient(&rect);
	//		Gdiplus::Rect rectLayout(rect.left,rect.top,rect.Width(),rect.Height());
	//		Draw(graphics,CDrawInfo(m_nDrawType,rectLayout);
	//		graphics.DrawRectangle(&pentools,rectLayout);
	//	}break;
	//default:
	//	break;
	//}
}
void CDlgSelect::Draw(Gdiplus::Graphics& graphics,const CDrawInfo& pDrawInfo)
{
	//Gdiplus::Color colorTools(255,237,28,36);
	const bool bFillSolid = pDrawInfo.m_fWidth==0.0?true:false;
	const double fWidth = pDrawInfo.m_fWidth==0.0?1.0:pDrawInfo.m_fWidth;
	Gdiplus::Pen penTools(pDrawInfo.m_color,fWidth);	// 1.0
	switch (pDrawInfo.m_nDrawType)
	{
	case CDrawInfo::DRAW_TEXT:
		{
			if (pDrawInfo.m_sText.empty())
			{
				//m_editText.SetWindowText(_T(""));
				//m_editText.MoveWindow(pDrawInfo.m_point1.X,pDrawInfo.m_point1.Y,30,26);
				//m_editText.ShowWindow(SW_SHOW);
			}else
			{
				const Gdiplus::FontFamily fontFamily(theFontFamily.c_str());
				const Gdiplus::Font fontEbTitle(&fontFamily, (int)pDrawInfo.m_fWidth, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::SolidBrush brushEbTitle(pDrawInfo.m_color);
				Gdiplus::StringFormat formatAdText;
				formatAdText.SetAlignment(Gdiplus::StringAlignmentNear);
				formatAdText.SetLineAlignment(Gdiplus::StringAlignmentNear);
				graphics.DrawString(pDrawInfo.m_sText.c_str(),-1,&fontEbTitle,pDrawInfo.GetRectF(),&formatAdText,&brushEbTitle);
			}
		}break;
	case CDrawInfo::DRAW_RECTANGLE:
		{
			graphics.DrawRectangle(&penTools,pDrawInfo.GetRect());
			if (bFillSolid)
			{
				Gdiplus::SolidBrush brush(pDrawInfo.m_color);
				graphics.FillRectangle(&brush,pDrawInfo.GetRect());
			}
		}break;
	case CDrawInfo::DRAW_CIRCLE:
		{
			graphics.DrawEllipse(&penTools,pDrawInfo.GetRect());
			if (bFillSolid)
			{
				Gdiplus::SolidBrush brush(pDrawInfo.m_color);
				graphics.FillEllipse(&brush,pDrawInfo.GetRect());
			}
		}break;
	case CDrawInfo::DRAW_ARROW:
		{
			//Gdiplus::GraphicsPath gp;         
			//gp.AddLine(0, 0, 0, 4);
			//gp.AddLine(0, 4, 4, 0);
			//gp.AddLine(0, 4,-4, 0);
			//Gdiplus::CustomLineCap clc(NULL, &gp);
			//penTools.SetCustomStartCap(&clc);
			//graphics.DrawLine(&penTools, pDrawInfo.m_point1,pDrawInfo.m_point2);

			////Gdiplus::Pen pen(colorTools, 1);
			//penTools.SetStartCap(Gdiplus::LineCapRoundAnchor);
			penTools.SetEndCap(Gdiplus::LineCapArrowAnchor);
			graphics.DrawLine(&penTools, pDrawInfo.m_point1,pDrawInfo.m_point2);
		}break;
	default:
		break;
	}
}

void CDlgSelect::OnTimer(UINT_PTR nIDEvent)
{
	//if (TIMERID_SELECT_COLOR==nIDEvent)
	//{
	//	KillTimer(TIMERID_SELECT_COLOR);
	//	CColorDialog dlg(m_colorTools.ToCOLORREF(),CC_FULLOPEN,this);
	//	if (dlg.DoModal()==IDOK)
	//	{
	//		m_colorTools.SetFromCOLORREF(dlg.GetColor());
	//		m_editText.SetTextColor(dlg.GetColor());
	//	}
	//}
	//if (TIMERID_DRAW == nIDEvent)
	//{
	//	// 这里只处理界面移动，不处理界面缩放
	//	static CRect theLastRect;
	//	CRect rect;
	//	this->GetWindowRect(&rect);
	//	if (theLastRect.left != rect.left || theLastRect.top != rect.top)
	//	{
	//		this->Invalidate();
	//		theLastRect = rect;
	//	}
	//}
	CDialogSK::OnTimer(nIDEvent);
}

BOOL CDlgSelect::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;	// 不刷新背景，不会闪
	return CDialogSK::OnEraseBkgnd(pDC);
}

void CDlgSelect::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	this->GetParent()->PostMessage(WM_CLOSE);
	//CDialogSK::OnCancel();
}

void CDlgSelect::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogSK::OnOK();
}

void CDlgSelect::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_nDrawType != CDrawInfo::DRAW_NULL)
	{
		CPoint pos;
		::GetCursorPos(&pos);
		this->ScreenToClient(&pos);
		m_pSelectBegin = Gdiplus::Point(pos.x,pos.y);

		if (m_nDrawType == CDrawInfo::DRAW_TEXT)
		{
			AddDrawTextInfo(true);
			m_editText.SetWindowText(_T(""));
			m_editText.MoveWindow(m_pSelectBegin.X,m_pSelectBegin.Y,30,26);
			m_editText.ShowWindow(SW_SHOW);
		}

		//theLastDrawEndPoint = m_pSelectBegin;
		m_bMouseDown = TRUE;
		SetCapture();
	}

	CDialogSK::OnLButtonDown(nFlags, point);
}
void CDlgSelect::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	if (m_nDrawType == CDrawInfo::DRAW_NULL)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		return;
	}else
	{
		if (m_bMouseDown)
		{
			// 这里显示选择区域
			CPoint pos;
			::GetCursorPos(&pos);
			this->ScreenToClient(&pos);
			m_pSelectEnd = Gdiplus::Point(pos.x,pos.y);
			this->Invalidate();

			//CRect rect;
			//rect.left = min(m_pSelectBegin.X,m_pSelectEnd.X);
			//rect.top = min(m_pSelectBegin.Y,m_pSelectEnd.Y);
			//rect.right = max(m_pSelectBegin.X,m_pSelectEnd.X);
			//rect.bottom = max(m_pSelectBegin.Y,m_pSelectEnd.Y);
			//m_pDlgSelect.MoveWindow(&rect);
			//if (!m_pDlgSelect.IsWindowVisible())
			//	m_pDlgSelect.ShowWindow(SW_SHOW);
		}

	}

	CDialogSK::OnMouseMove(nFlags, point);
}
void CDlgSelect::AddDrawTextInfo(bool bReset)
{
	if (m_nDrawType == CDrawInfo::DRAW_TEXT)
	{
		CString sText;
		this->m_editText.GetWindowText(sText);
		if (!sText.IsEmpty())
		{
			CRect rect;
			m_editText.GetWindowRect(&rect);
			this->ScreenToClient(&rect);
			USES_CONVERSION;
			CDrawInfo pDrawInfo = CDrawInfo(m_nDrawType,(double)m_nFontSize,Gdiplus::Point(rect.left,rect.top),Gdiplus::Point(rect.right,rect.bottom),m_colorTools);
			pDrawInfo.m_sText = T2W(sText);
			m_pDrawList.push_back(pDrawInfo);
			//m_editText.SetWindowText(_T(""));
			//m_editText.ShowWindow(SW_HIDE);
		}
		if (bReset)
		{
			m_editText.SetWindowText(_T(""));
			m_editText.ShowWindow(SW_HIDE);
		}
	}
}

void CDlgSelect::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		ReleaseCapture();

		if (m_nDrawType == CDrawInfo::DRAW_TEXT)
		{
			//AddDrawTextInfo();
		}else
		{
			CPoint pos;
			::GetCursorPos(&pos);
			this->ScreenToClient(&pos);
			if (pos.x != m_pSelectBegin.X && pos.y != m_pSelectBegin.Y)
			{
				//CRect rect;
				//rect.left = min(m_pSelectBegin.X,m_pSelectEnd.X);
				//rect.top = min(m_pSelectBegin.Y,m_pSelectEnd.Y);
				//rect.right = max(m_pSelectBegin.X,m_pSelectEnd.X);
				//rect.bottom = max(m_pSelectBegin.Y,m_pSelectEnd.Y);
				//this->ScreenToClient(&rect);
				//Gdiplus::Rect rectLayout(rect.left,rect.top,rect.Width(),rect.Height());
				m_pDrawList.push_back(CDrawInfo(m_nDrawType,m_fDrawPointWidth,m_pSelectBegin,m_pSelectEnd,m_colorTools));
				//m_pDrawList.push_back(CDrawInfo(m_nDrawType,rectLayout));
				//m_bSelectFinished = TRUE;
				//MoveTools();
			}
		}
	}

	CDialogSK::OnLButtonUp(nFlags, point);
}

void CDlgSelect::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->GetParent()->PostMessage(EBSC_WM_SELECT_FINISH);
	CDialogSK::OnLButtonDblClk(nFlags, point);
}


void CDlgSelect::OnSize(UINT nType, int cx, int cy)
{
	CDialogSK::OnSize(nType, cx, cy);

	const int const_max_len = 1300;
	const int const_min_len = 20;
	if (m_wndSplitterLeft.GetSafeHwnd())
	{
		m_wndSplitterLeft.MoveWindow(0,0,const_border_width,cy);
		m_wndSplitterLeft.SetRange(-const_max_len, cx-const_min_len);
	}
	if (m_wndSplitterTop.GetSafeHwnd())
	{
		m_wndSplitterTop.MoveWindow(0,0,cx,const_border_width);
		m_wndSplitterTop.SetRange(-const_max_len, cy-const_min_len);
	}
	if (m_wndSplitterRight.GetSafeHwnd())
	{
		m_wndSplitterRight.MoveWindow(cx-const_border_width,0,const_border_width,cy);
		m_wndSplitterRight.SetRange(const_min_len, const_max_len);
	}
	if (m_wndSplitterBottom.GetSafeHwnd())
	{
		m_wndSplitterBottom.MoveWindow(0,cy-const_border_width,cx,const_border_width);
		m_wndSplitterBottom.SetRange(const_min_len, const_max_len);
	}
	//this->PostMessage(POP_WM_WINDOW_SIZE);
}

void CDlgSelect::Reset(void)
{
	m_nDrawType = CDrawInfo::DRAW_NULL;
	m_pDrawList.clear();
	this->EnableEasyMove(TRUE);
	m_pSelectBegin = Gdiplus::Point();
	m_pSelectEnd = Gdiplus::Point();
}
void CDlgSelect::SetPointType(int nPointType)
{
	AddDrawTextInfo(true);
	int nFontSize = 14;
	switch (nPointType)
	{
	case 0:
		m_fDrawPointWidth = 1.0;
		nFontSize = 14;
		break;
	case 1:
		m_fDrawPointWidth = 2.0;
		nFontSize = 17;
		break;
	case 2:
		m_fDrawPointWidth = 4.0;
		nFontSize = 20;
		break;
	case 3:	// 实心　圆/矩形有效
		m_fDrawPointWidth = 0.0;	// ***
		break;
	default:
		break;
	}
	if (m_nFontSize!=nFontSize)
	{
		m_nFontSize=nFontSize;
		SetEditFont();
	}
}

void CDlgSelect::SetToolsColor(void)
{
	AddDrawTextInfo(true);

	CColorDialog dlg(m_colorTools.ToCOLORREF(),CC_FULLOPEN,this);
	if (dlg.DoModal()==IDOK)
	{
		m_colorTools.SetFromCOLORREF(dlg.GetColor());
		m_editText.SetTextColor(dlg.GetColor());
	}
}
void CDlgSelect::SetToolsText(bool bChecked)
{
	if (m_nDrawType == CDrawInfo::DRAW_TEXT && !bChecked)
	{
		AddDrawTextInfo(true);
	}

	m_nDrawType = bChecked?CDrawInfo::DRAW_TEXT:CDrawInfo::DRAW_NULL;
	this->EnableEasyMove(bChecked?FALSE:TRUE);
}
void CDlgSelect::SetToolsRectangle(bool bChecked)
{
	AddDrawTextInfo(true);
	m_nDrawType = bChecked?CDrawInfo::DRAW_RECTANGLE:CDrawInfo::DRAW_NULL;
	this->EnableEasyMove(bChecked?FALSE:TRUE);
}
void CDlgSelect::SetToolsCircle(bool bChecked)
{
	AddDrawTextInfo(true);
	m_nDrawType = bChecked?CDrawInfo::DRAW_CIRCLE:CDrawInfo::DRAW_NULL;
	this->EnableEasyMove(bChecked?FALSE:TRUE);
}
void CDlgSelect::SetToolsArrow(bool bChecked)
{
	AddDrawTextInfo(true);
	m_nDrawType = bChecked?CDrawInfo::DRAW_ARROW:CDrawInfo::DRAW_NULL;
	this->EnableEasyMove(bChecked?FALSE:TRUE);
}
void CDlgSelect::SetToolsCancel(void)
{
	AddDrawTextInfo(true);
	if (!m_pDrawList.empty())
	{
		m_pDrawList.pop_back();
		this->Invalidate();
	}
}
int   GetCodecClsid(const   WCHAR*   format,   CLSID*   pClsid)
{
	UINT     num   =   0;                     //   number   of   image   encoders
	UINT     size   =   0;                   //   size   of   the   image   encoder   array   in   bytes

	Gdiplus::ImageCodecInfo*   pImageCodecInfo   =   NULL;

	Gdiplus::GetImageEncodersSize(&num,   &size);
	if(size   ==   0)
		return   -1;     //   Failure

	pImageCodecInfo   =   (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo   ==   NULL)
		return   -1;     //   Failure

	Gdiplus::GetImageEncoders(num,   size,   pImageCodecInfo);
	for(UINT   j   =   0;   j   <   num;   ++j)
	{
		if(   wcscmp(pImageCodecInfo[j].MimeType,   format)   ==   0   )
		{
			*pClsid   =   pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return   j;     //   Success
		}        
	}   //   for
	free(pImageCodecInfo);
	return   -1;     //   Failure

}   //   GetCodecClsid
#define USES_CFILEDIALOG
bool CDlgSelect::SetToolsSaveas(void)
{
	CDesktopWindow pDesktopWindow;
	if (!pDesktopWindow.Create())
	{
		return false;
	}
	Gdiplus::Image * m_imageScreen = Gdiplus::Bitmap::FromHBITMAP(pDesktopWindow.CopyScreen(),NULL);
	CRect rect;
	this->GetClientRect(&rect);
	this->ClientToScreen(&rect);
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
	Gdiplus::Bitmap * pImageSaveas = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)bitmap.m_hObject,NULL);
	bool ret = false;
	if (pImageSaveas!=NULL)
	{
		TCHAR szFileName[MAX_PATH*2] = {0} ; 
		const time_t tNow = time(0);
		const struct tm *newtime = localtime(&tNow);
		_stprintf(szFileName,_T("EB截图%04d%02d%02d%02d%02d%02d"),newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);

#ifdef USES_CFILEDIALOG
		CFileDialog dlg(FALSE,_T("png"),szFileName,OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,_T("PNG (*.png)|*.png|JPEG (*.jpg;*.jpeg)|*.jpg|BMP (*.bmp)|*.bmp||"),this,0,TRUE);
		if (dlg.DoModal()==IDOK)
#else
		OPENFILENAME ofn={0};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = this->m_hWnd;
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = sizeof(szFileName);
		ofn.lpstrFilter = "PNG (*.png)\0*.png\0JPEG (*.jpg;*.jpeg)\0*.jpg\0BMP (*.bmp)\0*.bmp\0\0";
		ofn.lpstrDefExt = "png";   
		ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
		if (GetSaveFileName(&ofn)==TRUE )
#endif
		{
#ifdef USES_CFILEDIALOG
			CString sExt(dlg.GetFileName());
#else
			CString sExt(ofn.lpstrFile);
#endif
			sExt = sExt.Right(3);
			sExt.MakeLower();
			CLSID                           codecClsid;
			if (sExt=="png")
				GetCodecClsid(L"image/png",   &codecClsid);
			else if (sExt=="bmp")
				GetCodecClsid(L"image/bmp",   &codecClsid);
			else
				GetCodecClsid(L"image/jpeg",   &codecClsid);

			USES_CONVERSION;
#ifdef USES_CFILEDIALOG
			pImageSaveas->Save(T2W(dlg.GetPathName()),&codecClsid);
#else
			pImageSaveas->Save(T2W(ofn.lpstrFile),&codecClsid);
#endif
			ret = true;
		}
		delete pImageSaveas;
	}
	dcMem.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	dcMem.DeleteDC();
	delete m_imageScreen;
	pDesktopWindow.Release();
	return ret;
}

void CDlgSelect::OnEnChangeEditText()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogSK::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString sText;
	m_editText.GetWindowText(sText);
	int nMaxHCount = 1;
	int nVCount = 1;	// 找到多少个回车符号
	{
		int nStart = -1;
		while (true)
		{
			int nStartTemp = sText.Find('\r',nStart+1);
			if (nStartTemp<0)
			{
				if ((sText.GetLength()-nStart)>nMaxHCount)
					nMaxHCount = (sText.GetLength()-nStart);
				break;
			}
			if ((nStartTemp-nStart)>nMaxHCount)
				nMaxHCount = (nStartTemp-nStart);
			nStart = nStartTemp;
			nVCount++;
		}
	}
	CRect rect;
	m_editText.GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	const int nNeedWidth = nMaxHCount*(m_nFontSize<=14?10:(m_nFontSize<=17?14:16));
	const int nNeedHeight = nVCount*(m_nFontSize<=14?22:(m_nFontSize<=17?26:30));
	//const int nNeedWidth = nMaxHCount*10;
	//const int nNeedHeight = nVCount*22;
	if (rect.Width() != nNeedWidth || rect.Height()!=nNeedHeight)
	{
		rect.right = rect.left + nNeedWidth;
		rect.bottom = rect.top + nNeedHeight;
		m_editText.MoveWindow(&rect);
		//rect.DeflateRect(2, 2);
		this->InvalidateRect(rect, FALSE);  
	}
}

BOOL CDlgSelect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	//if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_editText.GetSafeHwnd())
	//{
	//	if ((GetKeyState(VK_CONTROL)&0x80)==0x80 && pMsg->wParam=='A')
	//	{
	//		//AfxMessageBox(_T("select all"));
	//		m_editText.SetSel(0,-1);
	//	}
	//}
	return CDialogSK::PreTranslateMessage(pMsg);
}
