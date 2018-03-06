// DlgMessageBox.cpp : implementation file
//
#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgMessageBox.h"

#define TIMERID_AUTO_CLOSE 101

// CDlgMessageBox dialog

IMPLEMENT_DYNAMIC(CDlgMessageBox, CEbDialogBase)

CDlgMessageBox::CDlgMessageBox(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgMessageBox::IDD, pParent)
{
	m_bDeleteThis = TRUE;
	m_nImageType = IMAGE_NULL;
	m_nAutoClose = 0;
}

CDlgMessageBox::~CDlgMessageBox()
{
}

void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CDlgMessageBox, CEbDialogBase)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDOK, &CDlgMessageBox::OnBnClickedOk)
	ON_WM_SIZE()
	ON_WM_TIMER()
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
	AFX_SWITCH_INSTANCED();
	CDlgMessageBox * pDlgMessageBox = new CDlgMessageBox(pParent);
	pDlgMessageBox->m_bDeleteThis = true;
	pDlgMessageBox->m_sTitle = lpszTitle;
	pDlgMessageBox->m_sDescription = lpszText;
	pDlgMessageBox->m_nImageType = nImageType;
	pDlgMessageBox->m_nAutoClose = nAutoClose;
	pDlgMessageBox->Create(CDlgMessageBox::IDD,pParent);
	pDlgMessageBox->ShowWindow(SW_SHOW);
	return pDlgMessageBox;
}
int CDlgMessageBox::EbDoModal(CWnd * pParent,LPCTSTR lpszTitle,LPCTSTR lpszText, IMAGE_TYPE nImageType)
{
	CDlgMessageBox dlg(pParent);
	dlg.m_bDeleteThis = false;
	dlg.m_sTitle = lpszTitle;
	dlg.m_sDescription = lpszText;
	dlg.m_nImageType = nImageType;
	AFX_SWITCH_INSTANCED();
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

	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	//m_btnOk.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetHotColor(),theApp.GetNormalColor());

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
		SetTimer(TIMERID_AUTO_CLOSE,m_nAutoClose*1000,NULL);
	}
	this->SetForegroundWindow();
	SetWindowPos(&CWnd::wndTopMost,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
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
	this->AddBgDrawInfo(CEbBackDrawInfo(145,theDefaultBgColorBgLight0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(1,theDefaultColorTureLight1,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);

	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());

	const int const_image_x = 68;
	const int const_image_y = 54;
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
		break;
	}

	// 写标题
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleRegular, UnitPixel);
	//Gdiplus::Color colorTitle;
	//colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	//Gdiplus::SolidBrush brushEbTitle(colorTitle);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
	const Gdiplus::PointF pointTitle(x,y);
	graphics.DrawString(A2W_ACP(m_sTitle),-1,&fontEbTitle,pointTitle,&brushEbTitle);
	// TEXT
	y += 22;
	const Gdiplus::Font fontAdText(&fontFamily, 13, FontStyleRegular, UnitPixel);
	Gdiplus::SolidBrush brushDescription(Gdiplus::Color(12,12,12));
	const int nAdLeft = 20;
	const Gdiplus::RectF rectAdText(x,y,rectClient.Width()-x-10,rectClient.Height()-y-60);
	Gdiplus::StringFormat formatAdText;
	formatAdText.SetAlignment(Gdiplus::StringAlignmentNear);
	formatAdText.SetLineAlignment(Gdiplus::StringAlignmentNear);
	graphics.DrawString(A2W_ACP(m_sDescription),-1,&fontAdText,rectAdText,&formatAdText,&brushDescription);

	//CPaintDC dc(this); // 用于绘制的设备上下文
	//this->DrawPopBg(&dc, theApp.GetMainColor());
	//HDC m_hdcMemory = dc.m_hDC;

	//// 写标题
	//CFont pNewFontTitle;
	//pNewFontTitle.CreatePointFont(115, _T("宋体"));//创建显示文本的字体
	//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontTitle.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));	// 黑色
	//TextOut(m_hdcMemory, 6, 5, m_sTitle, m_sTitle.GetLength());
	//TextOut(m_hdcMemory, 10, 30, m_sDescription, m_sDescription.GetLength());
	//SelectObject(m_hdcMemory, pOldFond);

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

	const int const_btn_width = 81;//m_btnOk.GetImgWidth();
	const int const_btn_height = 32;//m_btnOk.GetImgHeight();
	const int const_btn_intever = 20;
	int x = (cx-const_btn_width*2-const_btn_intever)/2;
	int y = cy-const_btn_height-13;		// 最下面间隔
	m_btnOk.MovePoint(x,y,const_btn_width,const_btn_height);
	x += (const_btn_width+const_btn_intever);
	m_btnCancel.MovePoint(x,y,const_btn_width,const_btn_height);

	//const int const_btn_y = cy-m_btnOk.GetImgHeight()-10;
	//int x = cx-20-m_btnOk.GetImgWidth();
	//m_btnOk.MovePoint(x, const_btn_y);

}


void CDlgMessageBox::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_AUTO_CLOSE == nIDEvent)
	{
		KillTimer(nIDEvent);
		this->OnOK();
		return;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}
