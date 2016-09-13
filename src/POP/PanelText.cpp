// PanelText.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "PanelText.h"

#define TIMERID_AUTO_CLOSE 101

// CPanelText dialog

IMPLEMENT_DYNAMIC(CPanelText, CEbDialogBase)

CPanelText::CPanelText(CWnd* pParent /*=NULL*/, int nFontSize)
	: CEbDialogBase(CPanelText::IDD, pParent)
{
	m_colorBg = theDefaultFlatBgColor;
	m_nBorder = 1;
	m_pPoint.X = 1;
	m_pPoint.Y = 2;
	const FontFamily fontFamily(theFontFamily.c_str());
	m_pFont = new Gdiplus::Font(&fontFamily, nFontSize, FontStyleRegular, UnitPixel);
	m_pBrush = new Gdiplus::SolidBrush(Gdiplus::Color(96,96,96));
	//m_nAutoClose = 0;
}

CPanelText::~CPanelText()
{
	delete m_pFont;
	m_pFont = NULL;
	delete m_pBrush;
	m_pBrush = NULL;
}

void CPanelText::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelText, CEbDialogBase)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPanelText message handlers

BOOL CPanelText::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetMouseMove(FALSE);

	// TODO:  Add extra initialization here

	this->SetCircle(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelText::OnPaint()
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
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,m_colorBg));
		this->DrawPopBg(&memDC, m_colorBg,m_nBorder,0,0,theDefaultFlatLineColor);
		Gdiplus::Graphics graphics(memDC.m_hDC);

		if (!m_sDrawText.empty())
		{
			graphics.DrawString(m_sDrawText.c_str(),-1,m_pFont,m_pPoint,m_pBrush);
		}
	}
}

void CPanelText::SetBgColor(COLORREF bg)
{
	m_colorBg = bg;
}
void CPanelText::SetBorder(int nBorder)
{
	m_nBorder = nBorder;
}

void CPanelText::SetDrawPos(int x, int y)
{
	m_pPoint.X = x;
	m_pPoint.Y = y;
}

void CPanelText::SetDrawText(const wchar_t* lpszText, bool bShow, int nAutoClose)
{
	if (lpszText==NULL || wcslen(lpszText)==0)
	{
		m_sDrawText.clear();
		//if (this->IsWindowVisible())
		//	this->ShowWindow(SW_HIDE);
	}else
	{
		m_sDrawText = lpszText;
		if (bShow)
		{
			KillTimer(TIMERID_AUTO_CLOSE);
			//m_nAutoClose = nAutoClose;
			if (nAutoClose > 0)
			{
				SetTimer(TIMERID_AUTO_CLOSE,nAutoClose*1000,NULL);
			}
			if (!this->IsWindowVisible())
				this->ShowWindow(SW_SHOW);
			else
				this->Invalidate();

		}else if (!this->IsWindowVisible())
		{
			this->ShowWindow(SW_HIDE);
		}
	}
}
int CPanelText::MeasureTextWidth(const wchar_t* lpszText, int nMaxWidth)
{
	CPaintDC dc(this);
	Gdiplus::Graphics graphics(dc.m_hDC);
	Gdiplus::RectF layoutLastLineRect(0,0,nMaxWidth,22);
	Gdiplus::RectF stringLineRectTemp;
	graphics.MeasureString(lpszText, -1, m_pFont, layoutLastLineRect, StringFormat::GenericDefault(), &stringLineRectTemp);
	return (int)stringLineRectTemp.Width;
}


void CPanelText::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CPanelText::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CPanelText::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_AUTO_CLOSE == nIDEvent)
	{
		KillTimer(nIDEvent);
		this->ShowWindow(SW_HIDE);
		return;
	}
	CEbDialogBase::OnTimer(nIDEvent);
}
