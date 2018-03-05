// DlgTools.cpp : implementation file
//

#include "stdafx.h"
#include "ebsc.h"
#include "DlgTools.h"


// CDlgTools dialog

IMPLEMENT_DYNAMIC(CDlgTools, CDialogSK)

CDlgTools::CDlgTools(CWnd* pParent /*=NULL*/)
	: CDialogSK(CDlgTools::IDD, pParent)
{
	m_pCurrentHwnd = NULL;
}

CDlgTools::~CDlgTools()
{
}

void CDlgTools::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_chkColor);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_chkText);
	DDX_Control(pDX, IDC_STATIC_RECTANGLE, m_chkRectangle);
	DDX_Control(pDX, IDC_STATIC_CIRCLE, m_chkCircle);
	DDX_Control(pDX, IDC_STATIC_ARROW, m_chkArrow);
	DDX_Control(pDX, IDC_STATIC_CANCEL, m_chkCancel);
	DDX_Control(pDX, IDC_STATIC_SAVEAS, m_chkSaveas);
	DDX_Control(pDX, IDC_BUTTON_FINISH, m_btnFinish);
	
}


BEGIN_MESSAGE_MAP(CDlgTools, CDialogSK)
	ON_BN_CLICKED(IDC_BUTTON_FINISH, &CDlgTools::OnBnClickedButtonFinish)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_STN_CLICKED(IDC_STATIC_COLOR, &CDlgTools::OnStnClickedStaticColor)
	ON_STN_CLICKED(IDC_STATIC_TEXT, &CDlgTools::OnStnClickedStaticText)
	ON_STN_CLICKED(IDC_STATIC_RECTANGLE, &CDlgTools::OnStnClickedStaticRectangle)
	ON_STN_CLICKED(IDC_STATIC_CIRCLE, &CDlgTools::OnStnClickedStaticCircle)
	ON_WM_MOUSEMOVE()
	ON_STN_CLICKED(IDC_STATIC_ARROW, &CDlgTools::OnStnClickedStaticArrow)
	ON_STN_CLICKED(IDC_STATIC_CANCEL, &CDlgTools::OnStnClickedStaticCancel)
	ON_STN_CLICKED(IDC_STATIC_SAVEAS, &CDlgTools::OnStnClickedStaticSaveas)
END_MESSAGE_MAP()


// CDlgTools message handlers
COLORREF theTransparentColor = RGB(1,254,255);
BOOL CDlgTools::OnInitDialog()
{
	CDialogSK::OnInitDialog();
	this->EnableEasyMove(FALSE);
	//this->SetTransparentColor(theTransparentColor);

	CImageEx pBtnImage;
	HGDIOBJ pBtnBitmap[3];
	pBtnImage.LoadResource(IDB_BITMAP_BTN_COLOR);
	pBtnImage.Cut(3, pBtnBitmap);
	m_chkColor.SetBGBitmap((HBITMAP)pBtnBitmap[0]);
	m_chkColor.SetClickedBGBitmap((HBITMAP)pBtnBitmap[1]);
	m_chkColor.SetMouseOverBGBitmap((HBITMAP)pBtnBitmap[2]);
	m_chkColor.SetWindowText(_T(""));
	m_chkColor.SetToolTipText(_T("颜色"));
	pBtnImage.LoadResource(IDB_BITMAP_BTN_TEXT);
	pBtnImage.Cut(3, pBtnBitmap);
	m_chkText.SetBGBitmap((HBITMAP)pBtnBitmap[0]);
	m_chkText.SetClickedBGBitmap((HBITMAP)pBtnBitmap[1]);
	m_chkText.SetMouseOverBGBitmap((HBITMAP)pBtnBitmap[2]);
	m_chkText.SetWindowText(_T(""));
	m_chkText.SetToolTipText(_T("文字工具"));
	pBtnImage.LoadResource(IDB_BITMAP_BTN_RECTANGLE);
	pBtnImage.Cut(3, pBtnBitmap);
	m_chkRectangle.SetBGBitmap((HBITMAP)pBtnBitmap[0]);
	m_chkRectangle.SetClickedBGBitmap((HBITMAP)pBtnBitmap[1]);
	m_chkRectangle.SetMouseOverBGBitmap((HBITMAP)pBtnBitmap[2]);
	m_chkRectangle.SetWindowText(_T(""));
	m_chkRectangle.SetToolTipText(_T("矩形工具"));
	pBtnImage.LoadResource(IDB_BITMAP_BTN_CIRCLE);
	pBtnImage.Cut(3, pBtnBitmap);
	m_chkCircle.SetBGBitmap((HBITMAP)pBtnBitmap[0]);
	m_chkCircle.SetClickedBGBitmap((HBITMAP)pBtnBitmap[1]);
	m_chkCircle.SetMouseOverBGBitmap((HBITMAP)pBtnBitmap[2]);
	m_chkCircle.SetWindowText(_T(""));
	m_chkCircle.SetToolTipText(_T("圆形工具"));
	pBtnImage.LoadResource(IDB_BITMAP_BTN_ARROW);
	pBtnImage.Cut(3, pBtnBitmap);
	m_chkArrow.SetBGBitmap((HBITMAP)pBtnBitmap[0]);
	m_chkArrow.SetClickedBGBitmap((HBITMAP)pBtnBitmap[1]);
	m_chkArrow.SetMouseOverBGBitmap((HBITMAP)pBtnBitmap[2]);
	m_chkArrow.SetWindowText(_T(""));
	m_chkArrow.SetToolTipText(_T("箭头工具"));
	pBtnImage.LoadResource(IDB_BITMAP_BTN_CANCEL);
	pBtnImage.Cut(3, pBtnBitmap);
	m_chkCancel.SetBGBitmap((HBITMAP)pBtnBitmap[0]);
	m_chkCancel.SetClickedBGBitmap((HBITMAP)pBtnBitmap[1]);
	m_chkCancel.SetMouseOverBGBitmap((HBITMAP)pBtnBitmap[2]);
	m_chkCancel.SetWindowText(_T(""));
	m_chkCancel.SetToolTipText(_T("撤销前一个编辑"));
	pBtnImage.LoadResource(IDB_BITMAP_BTN_SAVEAS);
	pBtnImage.Cut(3, pBtnBitmap);
	m_chkSaveas.SetBGBitmap((HBITMAP)pBtnBitmap[0]);
	m_chkSaveas.SetClickedBGBitmap((HBITMAP)pBtnBitmap[1]);
	m_chkSaveas.SetMouseOverBGBitmap((HBITMAP)pBtnBitmap[2]);
	m_chkSaveas.SetWindowText(_T(""));
	m_chkSaveas.SetToolTipText(_T("保存截图"));

	m_btnFinish.SetDrawPanel(true,RGB(86,190,245),RGB(14,162,240),RGB(86,190,245));
	//m_btnFinish.Load(IDB_PNG_58X24);
	m_btnFinish.SetToolTipText(_T("完成屏幕画布截图（按ESC取消）"));

	// IDB_BITMAP_BTN_RECTANGLE

	//SetCircle(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTools::OnBnClickedButtonFinish()
{
	// TODO: Add your control notification handler code here
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_FINISH);
}

void CDlgTools::OnSize(UINT nType, int cx, int cy)
{
	CDialogSK::OnSize(nType, cx, cy);

	const int const_top_inteval = 4;
	const int const_inteval = 2;
	const int const_btn_width1 = 48;//m_btnFinish.GetImgWidth();
	//const int const_btn_height1 = m_btnFinish.GetImgHeight();
	int x = cx-const_btn_width1-4;	// 4最右边间隔
	m_btnFinish.MovePoint(x,const_top_inteval,const_btn_width1,20);
	x -= 5;
	const int const_btn_width2 = m_chkRectangle.GetImgWidth();
	x -= (const_btn_width2+const_inteval);
	m_chkSaveas.MoveWindowPoint(x,const_top_inteval);
	x -= (const_btn_width2+const_inteval);
	m_chkCancel.MoveWindowPoint(x,const_top_inteval);
	x -= (const_btn_width2+const_inteval);
	m_chkColor.MoveWindowPoint(x,const_top_inteval);
	x -= (const_btn_width2+const_inteval);
	m_chkText.MoveWindowPoint(x,const_top_inteval);
	x -= (const_btn_width2+const_inteval);
	m_chkArrow.MoveWindowPoint(x,const_top_inteval);
	x -= (const_btn_width2+const_inteval);
	m_chkCircle.MoveWindowPoint(x,const_top_inteval);
	x -= (const_btn_width2+const_inteval);
	m_chkRectangle.MoveWindowPoint(x,const_top_inteval);
	
}

void CDlgTools::OnCancel()
{
	this->GetParent()->PostMessage(WM_CLOSE);
	//CDialogSK::OnCancel();
}

void CDlgTools::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogSK::OnOK();
}

void CDlgTools::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // device context for painting
		//SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
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
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect,RGB(222,238,255));
		//dc.Rectangle(&rect);
		CDialogSK::OnPaint();
	}
}
void CDlgTools::RefreshToolCtrl(void)
{
	if (m_chkColor.GetCheck())
	{
		m_chkColor.SetCheck(FALSE);
		m_chkColor.Invalidate();
	}
	if (m_pCurrentHwnd != m_chkText.GetSafeHwnd() && m_chkText.GetCheck())
	{
		m_chkText.SetCheck(FALSE);
		m_chkText.Invalidate();
	}
	if (m_pCurrentHwnd != m_chkRectangle.GetSafeHwnd() && m_chkRectangle.GetCheck())
	{
		m_chkRectangle.SetCheck(FALSE);
		m_chkRectangle.Invalidate();
	}
	if (m_pCurrentHwnd != m_chkCircle.GetSafeHwnd() && m_chkCircle.GetCheck())
	{
		m_chkCircle.SetCheck(FALSE);
		m_chkCircle.Invalidate();
	}
	if (m_pCurrentHwnd != m_chkArrow.GetSafeHwnd() && m_chkArrow.GetCheck())
	{
		m_chkArrow.SetCheck(FALSE);
		m_chkArrow.Invalidate();
	}
	if (m_chkCancel.GetCheck())
	//if (m_pCurrentHwnd != m_chkCancel.GetSafeHwnd() && m_chkCancel.GetCheck())
	{
		m_chkCancel.SetCheck(FALSE);
		m_chkCancel.Invalidate();
	}
	if (m_chkSaveas.GetCheck())
	{
		m_chkSaveas.SetCheck(FALSE);
		m_chkSaveas.Invalidate();
	}
}
void CDlgTools::OnStnClickedStaticColor()
{
	RefreshToolCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_CLICK_COLOR);
}
void CDlgTools::OnStnClickedStaticText()
{
	m_pCurrentHwnd = m_chkText.GetSafeHwnd();
	RefreshToolCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_CLICK_TEXT,m_chkText.GetCheck());
}
void CDlgTools::OnStnClickedStaticRectangle()
{
	m_pCurrentHwnd = m_chkRectangle.GetSafeHwnd();
	RefreshToolCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_CLICK_RECTANGLE,m_chkRectangle.GetCheck());
}
void CDlgTools::OnStnClickedStaticCircle()
{
	m_pCurrentHwnd = m_chkCircle.GetSafeHwnd();
	RefreshToolCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_CLICK_CIRCLE,m_chkCircle.GetCheck());
}

void CDlgTools::OnStnClickedStaticArrow()
{
	m_pCurrentHwnd = m_chkArrow.GetSafeHwnd();
	RefreshToolCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_CLICK_ARROW,m_chkArrow.GetCheck());
}
void CDlgTools::OnStnClickedStaticCancel()
{
	//m_pCurrentHwnd = m_chkCancel.GetSafeHwnd();
	RefreshToolCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_CLICK_CANCEL);
}
void CDlgTools::OnStnClickedStaticSaveas()
{
	RefreshToolCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLS_CLICK_SAVEAS);
}

void CDlgTools::Reset(void)
{
	m_pCurrentHwnd = NULL;
	RefreshToolCtrl();
}


void CDlgTools::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	this->SetFocus();
	CDialogSK::OnMouseMove(nFlags, point);
}
