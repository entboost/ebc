// DlgShrinkageBar.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgShrinkageBar.h"


// CDlgShrinkageBar dialog

IMPLEMENT_DYNAMIC(CDlgShrinkageBar, CEbDialogBase)

CDlgShrinkageBar::CDlgShrinkageBar(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgShrinkageBar::IDD, pParent)
{

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
		this->DrawPopBg(&memDC,theApp.GetMainColor(),0);
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
