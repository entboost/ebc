// DlgMessageBox.cpp : implementation file
//
#include "stdafx.h"
#include "POP.h"
#include "DlgMessageBox.h"

#define WINDOW_TITLE_HEIGHT			30
#define TIMERID_AUTO_CLOSE 101

// CDlgMessageBox dialog

IMPLEMENT_DYNAMIC(CDlgMessageBox, CEbDialogBase)

CDlgMessageBox::CDlgMessageBox(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMessageBox::IDD, pParent)
{
	m_bDeleteThis = TRUE;
	m_nImageType = IMAGE_NULL;
	m_nAutoClose = 0;
	m_bCloseOnlyMode = false;
}

CDlgMessageBox::~CDlgMessageBox()
{
}

void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CDlgMessageBox, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDOK, &CDlgMessageBox::OnBnClickedOk)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgMessageBox::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CDlgMessageBox message handlers

void CDlgMessageBox::OnDestroy()
{
	CEbDialogBase::OnDestroy();
	if (m_bDeleteThis)
		delete this;
}

CDlgMessageBox* CDlgMessageBox::EbMessageBox(CWnd * pParent,LPCTSTR lpszTitle,LPCTSTR lpszText, IMAGE_TYPE nImageType, int nAutoClose)
{
	CDlgMessageBox * pDlgMessageBox = new CDlgMessageBox(pParent);
	pDlgMessageBox->m_bDeleteThis = true;
	pDlgMessageBox->m_sTitle = lpszTitle;
	pDlgMessageBox->m_sDescription = lpszText;
	pDlgMessageBox->m_nImageType = nImageType;
	pDlgMessageBox->m_nAutoClose = nAutoClose;
	pDlgMessageBox->m_bCloseOnlyMode = true;
	pDlgMessageBox->Create(CDlgMessageBox::IDD,pParent);
	pDlgMessageBox->ShowWindow(SW_SHOW);
	return pDlgMessageBox;
}
int CDlgMessageBox::EbDoModal(CWnd * pParent,LPCTSTR lpszTitle,LPCTSTR lpszText, IMAGE_TYPE nImageType, bool bCloseOnlyMode, int nAutoClose)
{
	CDlgMessageBox dlg(pParent);
	dlg.m_bDeleteThis = false;
	dlg.m_sTitle = lpszTitle;
	dlg.m_sDescription = lpszText;
	dlg.m_nImageType = nImageType;
	dlg.m_nAutoClose = nAutoClose;
	dlg.m_bCloseOnlyMode = bCloseOnlyMode;
	return dlg.DoModal();
}

void CDlgMessageBox::GetScreenSize(int & pWidth, int & pHeight)
{
	pWidth = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	//int m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN); //屏幕高度
	pHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	pHeight += GetSystemMetrics(SM_CYCAPTION);
}

BOOL CDlgMessageBox::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnOk.SetToolTipText(_T("确定"));
	m_btnOk.SetDrawPanelRgn(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnCancel.SetToolTipText(_T("取消"));
	m_btnCancel.SetDrawPanelRgn(false);

	if (m_bCloseOnlyMode)
	{
		m_btnClose.ShowWindow(SW_HIDE);
		m_btnOk.ShowWindow(SW_HIDE);
		m_btnCancel.SetToolTipText(_T(""));
		if (m_nAutoClose > 0)
		{
			CString sText;
			sText.Format(_T("关闭（%d）"),m_nAutoClose);
			m_btnCancel.SetWindowText(sText);
		}else
		{
			m_btnCancel.SetWindowText(_T("确定"));
			//m_btnCancel.SetWindowText(_T("关闭"));
		}
	}else if (m_nAutoClose>0)
	{
		CString sText;
		sText.Format(_T("取消（%d）"),m_nAutoClose);
		m_btnCancel.SetWindowText(sText);
	}

	float h,s,l;
	::RGBtoHSL(GetRValue(theApp.GetMainColor()), GetGValue(theApp.GetMainColor()), GetBValue(theApp.GetMainColor()),&h,&s,&l);
	const COLORREF crOk = theDefaultBtnNormal==0.0?theApp.GetMainColor():HSLtoRGB(h, s, (l+theDefaultBtnNormal));
	const COLORREF crHot = HSLtoRGB(h, s, min(theDefaultMaxBtnHot,(l+theDefaultBtnHot)));
	const COLORREF crPre = ::HSLtoRGB(h, s, min(theDefaultMaxBtnPre,(l+theDefaultBtnPre)));
	m_btnOk.SetDrawPanel(true,crOk,crHot,crPre);
	m_btnOk.SetNorTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetHotTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetPreTextColor(theDefaultBtnWhiteColor); 
	if (m_bCloseOnlyMode)
	{
		m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLine2Color,crOk);
	}else
	{
		m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLineColor,crOk);
	}
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor);
	m_btnCancel.SetHotTextColor(crOk); 
	m_btnCancel.SetPreTextColor(crOk); 

	const int const_dlg_width = 380;
	const int const_dlg_height = 201;
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	GetScreenSize(nScreenWidth, nScreenHeight);
	this->MoveWindow((nScreenWidth-const_dlg_width)/2, (nScreenHeight-const_dlg_height)/2, const_dlg_width, const_dlg_height);
	//this->MoveWindow(nScreenWidth-const_dlg_width, nScreenHeight-const_dlg_height, const_dlg_width, const_dlg_height);
	this->SetCircle();

	if (m_nAutoClose > 0)
	{
		SetTimer(TIMERID_AUTO_CLOSE,1000,NULL);
	}
	this->SetWindowText(m_sTitle);
	this->SetForegroundWindow();
	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMessageBox::OnPaint()
{
	if (!IsIconic())
	{
		DrawInfo();
	}
}

void CDlgMessageBox::DrawInfo(void)
{
	CPaintDC dc2(this);
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	if (m_bCloseOnlyMode)
	{
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultTextWhiteColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(-52,1.0,false,false,0,theDefaultFlatLineColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(-51,1.0,false,false,0,theDefaultFlatBgColor));
		this->AddBgDrawInfo(CEbBackDrawInfo(-52,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(-51,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	}else
	{
		this->AddBgDrawInfo(CEbBackDrawInfo(WINDOW_TITLE_HEIGHT,1.0,false));
		//this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(-52,1.0,false,false,0,theDefaultFlatLine2Color));
		//this->AddBgDrawInfo(CEbBackDrawInfo(-51,1.0,false,false,0,theDefaultFlatBgColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
		//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, theDefaultBgColorTitleHeight, 2);
	}
	Gdiplus::Graphics graphics(memDC.m_hDC);

	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());

	const int const_image_x = 38;
	const int const_image_y = m_bCloseOnlyMode?48:68;
	int x = const_image_x+10;
	int y = const_image_y;
	// image
	switch (m_nImageType)
	{
	case IMAGE_INFORMATION:
		{
			Image * pImage = NULL;
			if (libEbc::ImageFromIDResource(IDB_PNG_IMG_TIP, _T("png"), pImage))
			{
				x += pImage->GetWidth();
				graphics.DrawImage(pImage, const_image_x, const_image_y);
				delete pImage;
			}
		}break;
	case IMAGE_QUESTION:
		{
			Image * pImage = NULL;
			if (libEbc::ImageFromIDResource(IDB_PNG_IMG_QUESTION, _T("png"), pImage))
			{
				x += pImage->GetWidth();
				graphics.DrawImage(pImage, const_image_x, const_image_y);
				delete pImage;
			}
		}break;
	case IMAGE_WARNING:
		{
			Image * pImage = NULL;
			if (libEbc::ImageFromIDResource(IDB_PNG_IMG_WARNING, _T("png"), pImage))
			{
				x += pImage->GetWidth();
				graphics.DrawImage(pImage, const_image_x, const_image_y);
				delete pImage;
			}
		}break;
	case IMAGE_ERROR:
		{
			Image * pImage = NULL;
			if (libEbc::ImageFromIDResource(IDB_PNG_IMG_ERROR, _T("png"), pImage))
			{
				x += pImage->GetWidth();
				graphics.DrawImage(pImage, const_image_x, const_image_y);
				delete pImage;
			}
		}break;
	default:
		x = 10;
		break;
	}

	// 写标题
	if (m_bCloseOnlyMode)
	{
		if (!m_sTitle.IsEmpty())
		{
			const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleRegular, UnitPixel);	// 13
			Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(32,32,32));
			//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(12,12,12));
			const Gdiplus::PointF pointTitle(6,5);
			//const Gdiplus::PointF pointTitle(x,y-2);
			graphics.DrawString(A2W_ACP(m_sTitle),-1,&fontEbTitle,pointTitle,&brushEbTitle);
			//y += 22;
		//}else
		//{
		//	y -= 2;
		}
	}else
	{
		const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
		Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
		const Gdiplus::PointF pointTitle(6,5);
		//const Gdiplus::PointF pointTitle(12,12);
		graphics.DrawString(A2W_ACP(m_sTitle),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	}
	// TEXT
	//y += 22;
	const Gdiplus::Font fontAdText(&fontFamily, 13, FontStyleRegular, UnitPixel);
	Gdiplus::SolidBrush brushDescription(Gdiplus::Color(12,12,12));
	//const int nAdLeft = 20;
	const Gdiplus::RectF rectAdText(x,y,rectClient.Width()-x-10,rectClient.Height()-y-60);
	Gdiplus::StringFormat formatAdText;
	if (m_nImageType==IMAGE_NULL)
		formatAdText.SetAlignment(Gdiplus::StringAlignmentCenter);
	else
		formatAdText.SetAlignment(Gdiplus::StringAlignmentNear);
	formatAdText.SetLineAlignment(Gdiplus::StringAlignmentNear);
	graphics.DrawString(A2W_ACP(m_sDescription),-1,&fontAdText,rectAdText,&formatAdText,&brushDescription);
}

//void CDlgMessageBox::OnBnClickedOk()
//{
//	OnOK();
//}

void CDlgMessageBox::OnOK()
{
	if (m_bDeleteThis)
		this->DestroyWindow();
	else
		CEbDialogBase::OnOK();
}

void CDlgMessageBox::OnCancel()
{
	if (m_bDeleteThis)
		this->DestroyWindow();
	else
		CEbDialogBase::OnCancel();
}

void CDlgMessageBox::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	if (m_bCloseOnlyMode)
	{
		int x = (cx-theDefaultBtnWidth*2)/2-2;		// 居中
		//int x = cx-theDefaultBtnWidth-theDefaultBtnRightIntever;		// 最右边间隔
		int y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;		// 最下面间隔
		m_btnCancel.MovePoint(x,y,theDefaultBtnWidth*2,theDefaultBtnHeight);
	}else
	{
		int x = (cx-theDefaultBtnWidth*2-theDefaultBtnIntever)/2;
		int y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;		// 最下面间隔
		m_btnOk.MovePoint(x,y,theDefaultBtnWidth,theDefaultBtnHeight);
		x += (theDefaultBtnWidth+theDefaultBtnIntever);
		m_btnCancel.MovePoint(x,y,theDefaultBtnWidth,theDefaultBtnHeight);
	}

	//const int const_btn_y = cy-m_btnOk.GetImgHeight()-10;
	//int x = cx-20-m_btnOk.GetImgWidth();
	//m_btnOk.MovePoint(x, const_btn_y);

}


void CDlgMessageBox::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_AUTO_CLOSE == nIDEvent)
	{
		m_nAutoClose--;
		if (m_nAutoClose>=0)
		{
			CString sText;
			if (m_bCloseOnlyMode)
			{
				sText.Format(_T("关闭（%d）"),m_nAutoClose);
			}else
			{
				sText.Format(_T("取消（%d）"),m_nAutoClose);
			}
			m_btnCancel.SetWindowText(sText);
		}else
		{
			KillTimer(nIDEvent);
			this->OnOK();
		}
		return;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgMessageBox::OnBnClickedButtonClose()
{
	PostMessage(WM_CLOSE, 0, 0);
}
