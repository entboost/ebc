// PanelFind.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "PanelFind.h"
#define TIMER_FIRE_ONFIND	101

// CPanelFind dialog

IMPLEMENT_DYNAMIC(CPanelFind, CEbDialogBase)

CPanelFind::CPanelFind(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPanelFind::IDD, pParent)
{
	m_pFindHandler = NULL;
	m_bFindUp = false;
	m_dwUserData = 0;
	m_nFindResultCountTemp = -1;
	m_nFindResultIndexTemp = -1;
	m_pPanelText = NULL;
}

CPanelFind::~CPanelFind()
{
}

void CPanelFind::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FINDTEXT, m_editFindText);
	DDX_Control(pDX, IDC_BUTTON_FINDUP, m_btnFindUp);
	DDX_Control(pDX, IDC_BUTTON_FINDDOWN, m_btnFindDown);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}


BEGIN_MESSAGE_MAP(CPanelFind, CEbDialogBase)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_FINDUP, &CPanelFind::OnBnClickedButtonFindup)
	ON_BN_CLICKED(IDC_BUTTON_FINDDOWN, &CPanelFind::OnBnClickedButtonFinddown)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CPanelFind::OnBnClickedButtonClose)
	ON_EN_CHANGE(IDC_EDIT_FINDTEXT, &CPanelFind::OnEnChangeEditFindtext)
	ON_WM_TIMER()
	ON_WM_MOVE()
END_MESSAGE_MAP()

void CPanelFind::SetCtrlColor(bool bInvalidate)
{
	m_editFindText.SetRectangleColor(theApp.GetHotColor(),theDefaultFlatLineColor);
	m_btnFindUp.SetDrawLine(5,1,0,theDefaultFlatLineColor,theApp.GetHotColor());
	m_btnFindDown.SetDrawLine(5,1,0,theDefaultFlatLineColor,theApp.GetHotColor());

	if (bInvalidate)
		this->Invalidate();
}
// CPanelFind message handlers
const int const_text_width = 200;

BOOL CPanelFind::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetMouseMove(FALSE);

	m_pPanelText = new CPanelText(this);
	m_pPanelText->Create(CPanelText::IDD,this);
	m_pPanelText->ShowWindow(SW_HIDE);

	m_editFindText.SetPromptText(_T("在当前页查找"));
	m_editFindText.LimitText(32);

	int y = 9;
	int x = 8;
	const int const_height = 22;
	const int const_btn_width = 22;
	m_editFindText.MoveWindow(x, y, const_text_width, const_height);
	x += const_text_width;
	m_btnFindUp.MoveWindow(x, y, const_btn_width, const_btn_width);
	x += const_btn_width;
	m_btnFindDown.MoveWindow(x, y, const_btn_width, const_btn_width);
	x += const_btn_width;
	m_btnClose.MoveWindow(x, y, const_btn_width, const_btn_width);


	m_btnFindUp.SetWindowText(_T(""));
	m_btnFindUp.SetToolTipText(_T("上一个 Ctrl + B"));
	m_btnFindUp.SetDrawTrianglePic(1,theDefaultFlatBlackText2Color,theDefaultFlatBlackText2Color,theDefaultFlatBlackText2Color,-1,10,5);
	//m_btnFindUp.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
	m_btnFindUp.SetDrawPanelRgn(false);
	//m_btnFindUp.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnFindDown.SetWindowText(_T(""));
	m_btnFindDown.SetToolTipText(_T("下一个 Ctrl + N"));
	m_btnFindDown.SetDrawTrianglePic(2,theDefaultFlatBlackText2Color,theDefaultFlatBlackText2Color,theDefaultFlatBlackText2Color,-1,10,5);
	//m_btnFindDown.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
	m_btnFindDown.SetDrawPanelRgn(false);
	//m_btnFindDown.SetDrawPanel(true,theApp.GetMainColor(),theApp.GetHotColor(),theApp.GetPreColor());

	m_btnClose.SetWindowText(_T(""));
	m_btnClose.SetToolTipText(_T("关闭查找栏 Esc"));
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultFlatBlackText2Color,theDefaultBtnCloseColor,-1,-1,2);
	//m_btnClose.SetDrawClosePic(true,RGB(250,250,250),-1,-1,-1,2);
	//m_btnClose.SetDrawPanel(true,theDefaultBtnCloseColor);

	SetCtrlColor(false);

	//this->SetCircle(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelFind::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CPanelFind::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CPanelFind::OnPaint()
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
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,1,theDefaultFlatBlackText2Color));
		//this->AddBgDrawInfo(CEbBackDrawInfo(1,1.0,false,false,0,theDefaultFlatBlackText2Color));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,true,-1,theDefaultFlatBlackText2Color));
		//this->DrawPopBg(&memDC, theApp.GetMainColor(),0);
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(),1,0,0,theDefaultFlatLine2Color);
	}
}

void CPanelFind::OnDestroy()
{
	if (m_pPanelText!=NULL)
	{
		m_pPanelText->DestroyWindow();
		delete m_pPanelText;
		m_pPanelText = NULL;
	}
	CEbDialogBase::OnDestroy();
}

void CPanelFind::SetFindFocus(void)
{
	m_editFindText.SetFocus();
}

void CPanelFind::Cef_OnFindResult(int count,int activeMatchOrdinal,bool finalUpdate)
{
	if (count>=0)
		m_nFindResultCountTemp = count;
	if (activeMatchOrdinal>=0)
		m_nFindResultIndexTemp = activeMatchOrdinal;
	if (finalUpdate)
	{
		SetFindResult(m_nFindResultIndexTemp,m_nFindResultCountTemp);
	}
}

void CPanelFind::SetFindResult(int nCurrentIndex, int nFindCount)
{
	if (m_pPanelText==NULL || m_pPanelText->GetSafeHwnd()==NULL) return;

	if (nCurrentIndex<0 || nFindCount<0)
	{
		m_pPanelText->ShowWindow(SW_HIDE);
		m_pPanelText->SetDrawText(NULL);
		return;
	}
	OnMove();
	if (nCurrentIndex==0 || nFindCount==0)
		m_pPanelText->SetBgColor(RGB(255,115,21));
	else
		m_pPanelText->SetBgColor(theDefaultFlatBgColor);
	wchar_t lpszBuffer[64];
	_swprintf(lpszBuffer,L"第%d条，共%d条",nCurrentIndex,nFindCount);
	m_pPanelText->SetDrawText(lpszBuffer);
	m_editFindText.SetFocus();
}
void CPanelFind::OnMove(void)
{
	if (m_pPanelText!=NULL && m_pPanelText->GetSafeHwnd()!=NULL)
	{
		CRect rectFindText;
		m_editFindText.GetWindowRect(&rectFindText);
		this->ScreenToClient(&rectFindText);
		rectFindText.right = rectFindText.left + const_text_width;

		CRect rectPanelText(&rectFindText);
		if (m_nFindResultCountTemp>=100)
		{
			rectFindText.right -= 107;
		}else if (m_nFindResultCountTemp>=10)
		{
			rectFindText.right -= 94;
		}else if (m_nFindResultCountTemp>=0)
		{
			rectFindText.right -= 80;
		}
		m_editFindText.MoveWindow(&rectFindText);
		rectPanelText.left = rectFindText.right;
		m_pPanelText->MoveWindow(&rectPanelText);
	}
}
void CPanelFind::OnShowHide(bool bShow)
{
	if (bShow && m_nFindResultCountTemp>=0)
	{
		this->ShowWindow(SW_SHOW);
		if (m_pPanelText!=NULL && m_pPanelText->GetSafeHwnd()!=NULL)
			m_pPanelText->ShowWindow(SW_SHOW);
	}else
	{
		this->ShowWindow(SW_HIDE);
		if (m_pPanelText!=NULL && m_pPanelText->GetSafeHwnd()!=NULL)
			m_pPanelText->ShowWindow(SW_HIDE);
	}
}

void CPanelFind::OnBnClickedButtonFindup()
{
	m_bFindUp = true;
	Fire_OnFind();
}

void CPanelFind::Fire_OnFind(void)
{
	if (m_pFindHandler!=NULL)
	{
		CString sFindText;
		m_editFindText.GetWindowText(sFindText);
		if (!sFindText.IsEmpty())
		{
			const bool bFindNext = !m_sCurentFindText.IsEmpty() && m_sCurentFindText==sFindText;
			m_pFindHandler->OnFindText(sFindText,m_bFindUp,false, bFindNext);
			m_sCurentFindText = sFindText;
		}else if (m_pPanelText!=NULL && m_pPanelText->IsWindowVisible())
		{
			Fire_OnClose();
			//m_pPanelText->ShowWindow(SW_HIDE);
		}
	}
}
void CPanelFind::Fire_OnClose(void)
{
	if (m_pFindHandler!=NULL)
		m_pFindHandler->OnFindClose();
	m_nFindResultCountTemp = -1;
	m_bFindUp = false;
	m_sCurentFindText = _T("");
	m_editFindText.SetWindowText(_T(""));
	m_pPanelText->ShowWindow(SW_HIDE);
	OnMove();
}

void CPanelFind::OnBnClickedButtonFinddown()
{
	m_bFindUp = false;
	Fire_OnFind();
}

void CPanelFind::OnBnClickedButtonClose()
{
	this->ShowWindow(SW_HIDE);
	Fire_OnClose();
}

void CPanelFind::OnEnChangeEditFindtext()
{
	KillTimer(TIMER_FIRE_ONFIND);
	SetTimer(TIMER_FIRE_ONFIND,400,NULL);
}

void CPanelFind::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==TIMER_FIRE_ONFIND)
	{
		KillTimer(TIMER_FIRE_ONFIND);
		Fire_OnFind();
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CPanelFind::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);
	OnMove();
}

BOOL CPanelFind::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		const bool bControl = (GetKeyState(VK_CONTROL)&0x80)==0x80?true:false;
		if (bControl)
		{
			if (pMsg->wParam=='B')
			{
				m_bFindUp = true;
				Fire_OnFind();
				return TRUE;
			}else if (pMsg->wParam=='N')
			{
				m_bFindUp = false;
				Fire_OnFind();
				return TRUE;
			}
		}else if (pMsg->wParam==VK_ESCAPE)
		{
			OnBnClickedButtonClose();
			return TRUE;
		}
	}
	return CEbDialogBase::PreTranslateMessage(pMsg);
}
