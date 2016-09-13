// DlgShrinkageBar.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgShrinkageBar.h"


// CDlgShrinkageBar dialog

IMPLEMENT_DYNAMIC(CDlgShrinkageBar, CEbDialogBase)

CDlgShrinkageBar::CDlgShrinkageBar(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgShrinkageBar::IDD, pParent)
{
	m_colorBg = -1;
}

CDlgShrinkageBar::~CDlgShrinkageBar()
{
}

void CDlgShrinkageBar::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShrinkageBar, CEbDialogBase)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CDlgShrinkageBar::SetBgColor(COLORREF colorBg)
{
	if (m_colorBg!=colorBg)
	{
		m_colorBg = colorBg;
		this->Invalidate();
	}
}

// CDlgShrinkageBar message handlers

void CDlgShrinkageBar::OnPaint()
{
	if (IsIconic())
	{

	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false));
		const COLORREF colorBg = m_colorBg==-1?theApp.GetMainColor():m_colorBg;
		this->DrawPopBg(&memDC,colorBg,0);
	}
}

BOOL CDlgShrinkageBar::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW,0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
