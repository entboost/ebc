// DlgToolExt.cpp : implementation file
//

#include "stdafx.h"
#include "ebsc.h"
#include "DlgToolExt.h"

#define IDC_BUTTON_POINT1               0x2001
#define IDC_BUTTON_POINT2               0x2002
#define IDC_BUTTON_POINT3               0x2003
#define IDC_BUTTON_POINT4               0x2004

// CDlgToolExt dialog

IMPLEMENT_DYNAMIC(CDlgToolExt, CDialogSK)

CDlgToolExt::CDlgToolExt(CWnd* pParent /*=NULL*/)
	: CDialogSK(CDlgToolExt::IDD, pParent)
{
	m_nDrawType = CDrawInfo::DRAW_NULL;
	m_nPointType = 0;
}

CDlgToolExt::~CDlgToolExt()
{
}

void CDlgToolExt::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgToolExt, CDialogSK)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_POINT1, &CDlgToolExt::OnBnClickedPoint1)
	//ON_BN_DOUBLECLICKED(IDC_BUTTON_POINT1, &CDlgToolExt::OnBnClickedPoint1)
	ON_BN_CLICKED(IDC_BUTTON_POINT2, &CDlgToolExt::OnBnClickedPoint2)
	//ON_BN_DOUBLECLICKED(IDC_BUTTON_POINT2, &CDlgToolExt::OnBnClickedPoint2)
	ON_BN_CLICKED(IDC_BUTTON_POINT3, &CDlgToolExt::OnBnClickedPoint3)
	ON_BN_CLICKED(IDC_BUTTON_POINT4, &CDlgToolExt::OnBnClickedPoint4)
	//ON_BN_DOUBLECLICKED(IDC_BUTTON_POINT3, &CDlgToolExt::OnBnClickedPoint3)
END_MESSAGE_MAP()


// CDlgToolExt message handlers

void CDlgToolExt::OnSize(UINT nType, int cx, int cy)
{
	CDialogSK::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CDlgToolExt::OnPaint()
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

BOOL CDlgToolExt::OnInitDialog()
{
	CDialogSK::OnInitDialog();
	this->EnableEasyMove(FALSE);
	this->EnableToolTips();

	const COLORREF theDefaultColor			= RGB(0,0,255);
	const COLORREF theDefaultFlatLine2Color			= RGB(192,192,192);

	m_btnPoint1.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), this, IDC_BUTTON_POINT1);
	m_btnPoint1.SetDlgCtrlID(IDC_BUTTON_POINT1);
	m_btnPoint1.SetDrawToolButtonPic(10,theDefaultColor,-1,-1,-1,1,1);
	m_btnPoint1.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color, theDefaultFlatLine2Color);
	m_btnPoint1.SetChecked(true);
	m_btnPoint2.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), this, IDC_BUTTON_POINT2);
	m_btnPoint2.SetDlgCtrlID(IDC_BUTTON_POINT2);
	m_btnPoint2.SetDrawToolButtonPic(10,theDefaultColor,-1,-1,-1,2,2);
	m_btnPoint2.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color, theDefaultFlatLine2Color);
	m_btnPoint3.Create(_T(""),WS_CHILD|WS_VISIBLE, CRect(0,0,1,1), this, IDC_BUTTON_POINT3);
	m_btnPoint3.SetDlgCtrlID(IDC_BUTTON_POINT3);
	m_btnPoint3.SetDrawToolButtonPic(10,theDefaultColor,-1,-1,-1,4,4);
	m_btnPoint3.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color, theDefaultFlatLine2Color);
	m_btnPoint4.SetToolTipText(_T("ÊµÐÄ"));
	m_btnPoint4.Create(_T(""),WS_CHILD, CRect(0,0,1,1), this, IDC_BUTTON_POINT4);
	m_btnPoint4.SetDlgCtrlID(IDC_BUTTON_POINT4);
	m_btnPoint4.SetDrawToolButtonPic(10,theDefaultColor,-1,-1,-1,8,8);
	m_btnPoint4.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color, theDefaultFlatLine2Color);

	const int const_top_inteval = 4;
	const int const_inteval = 2;
	//const int const_btn_width1 = 48;//m_btnFinish.GetImgWidth();
	const int const_btn_width = 22;

	int x = 4;
	m_btnPoint1.MoveWindow(x,const_top_inteval,const_btn_width,20);
	m_btnPoint1.ShowWindow(SW_SHOW);
	x += (const_btn_width+const_inteval);
	m_btnPoint2.MoveWindow(x,const_top_inteval,const_btn_width,20);
	m_btnPoint2.ShowWindow(SW_SHOW);
	x += (const_btn_width+const_inteval);
	m_btnPoint3.MoveWindow(x,const_top_inteval,const_btn_width,20);
	m_btnPoint3.ShowWindow(SW_SHOW);
	x += (const_btn_width+const_inteval);
	m_btnPoint4.MoveWindow(x,const_top_inteval,const_btn_width,20);
	m_btnPoint4.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgToolExt::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogSK::OnOK();
}

void CDlgToolExt::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogSK::OnCancel();
}

void CDlgToolExt::Reset(void)
{
	m_nDrawType = CDrawInfo::DRAW_NULL;
	m_nPointType = 0;
	UpdatePointCtrl();
}

bool CDlgToolExt::SetDrawType(CDrawInfo::DRAW_TYPE nDrawType)
{
	if (m_nDrawType==nDrawType) return false;
	m_nDrawType = nDrawType;
	switch (m_nDrawType)
	{
	case CDrawInfo::DRAW_RECTANGLE:
	case CDrawInfo::DRAW_CIRCLE:
		{
			if (m_btnPoint4.GetSafeHwnd()!=NULL)
				m_btnPoint4.ShowWindow(SW_SHOW);
			return true;
		}break;
	case CDrawInfo::DRAW_ARROW:
		{

		}break;
	default:
		break;
	}
	if (m_btnPoint4.GetSafeHwnd()!=NULL)
		m_btnPoint4.ShowWindow(SW_HIDE);
	return true;
}

void CDlgToolExt::OnDestroy()
{
	CDialogSK::OnDestroy();

	m_btnPoint1.DestroyWindow();
	m_btnPoint2.DestroyWindow();
	m_btnPoint3.DestroyWindow();
	m_btnPoint4.DestroyWindow();
}
inline void InvalidateParentRect(const CWnd* pWnd)
{
	CRect rect;
	pWnd->GetWindowRect(&rect);
	pWnd->GetParent()->ScreenToClient(&rect);
	pWnd->GetParent()->InvalidateRect(&rect);
}
void CDlgToolExt::UpdatePointCtrl(void)
{
	//if ((m_btnPoint1.GetChecked() && m_nPointType!=0) ||
	//	(!m_btnPoint1.GetChecked() && m_nPointType==0))
	//{
	//	m_btnPoint1.SetChecked(m_nPointType==0?true:false);
	//	m_btnPoint1.Invalidate();
	//	InvalidateParentRect(&m_btnPoint1);
	//}
	//if ((m_btnPoint2.GetChecked() && m_nPointType!=1) ||
	//	(!m_btnPoint2.GetChecked() && m_nPointType==1))
	//{
	//	m_btnPoint2.SetChecked(m_nPointType==1?true:false);
	//	m_btnPoint2.Invalidate();
	//	InvalidateParentRect(&m_btnPoint2);
	//}
	//if ((m_btnPoint3.GetChecked() && m_nPointType!=2) ||
	//	(!m_btnPoint3.GetChecked() && m_nPointType==2))
	//{
	//	m_btnPoint3.SetChecked(m_nPointType==2?true:false);
	//	m_btnPoint3.Invalidate();
	//	InvalidateParentRect(&m_btnPoint3);
	//}

	m_btnPoint1.SetChecked(m_nPointType==0?true:false);
	m_btnPoint1.Invalidate();
	InvalidateParentRect(&m_btnPoint1);
	m_btnPoint2.SetChecked(m_nPointType==1?true:false);
	m_btnPoint2.Invalidate();
	InvalidateParentRect(&m_btnPoint2);
	m_btnPoint3.SetChecked(m_nPointType==2?true:false);
	m_btnPoint3.Invalidate();
	InvalidateParentRect(&m_btnPoint3);
	m_btnPoint4.SetChecked(m_nPointType==3?true:false);
	m_btnPoint4.Invalidate();
	InvalidateParentRect(&m_btnPoint4);
}

void CDlgToolExt::OnBnClickedPoint1()
{
	m_nPointType = 0;
	UpdatePointCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLEXT_POINT,m_nPointType);
}

void CDlgToolExt::OnBnClickedPoint2()
{
	m_nPointType = 1;
	UpdatePointCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLEXT_POINT,m_nPointType);
}
void CDlgToolExt::OnBnClickedPoint3()
{
	m_nPointType = 2;
	UpdatePointCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLEXT_POINT,m_nPointType);
}
void CDlgToolExt::OnBnClickedPoint4()
{
	m_nPointType = 3;
	UpdatePointCtrl();
	this->GetParent()->PostMessage(EBSC_WM_TOOLEXT_POINT,m_nPointType);
}
