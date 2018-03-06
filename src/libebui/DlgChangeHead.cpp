// DlgChangeHead.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgChangeHead.h"
#include "Core/SkinMemDC.h"
#include "DlgVideoPhoto.h"
#include "DlgSelectHead.h"

#define TIMERID_INVALIDATE_PARENT 100

// CDlgChangeHead dialog

IMPLEMENT_DYNAMIC(CDlgChangeHead, CEbDialogBase)

CDlgChangeHead::CDlgChangeHead(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgChangeHead::IDD, pParent)
	, m_pDlgSelectHead(NULL)
	, m_sGroupCode(0)
{
	m_imageHead = NULL;
}

CDlgChangeHead::~CDlgChangeHead()
{
}

void CDlgChangeHead::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_HEAD1, m_btnHead1);
	DDX_Control(pDX, IDC_BUTTON_HEAD2, m_btnHead2);
	DDX_Control(pDX, IDC_BUTTON_OPENFILE, m_btnOpenFile);
	DDX_Control(pDX, IDC_BUTTON_MY_VIDEO, m_btnMyVideo);
}


BEGIN_MESSAGE_MAP(CDlgChangeHead, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgChangeHead::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDOK, &CDlgChangeHead::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE, &CDlgChangeHead::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_MY_VIDEO, &CDlgChangeHead::OnBnClickedButtonMyVideo)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_HEAD1, &CDlgChangeHead::OnBnClickedButtonHead1)
	ON_BN_CLICKED(IDC_BUTTON_HEAD2, &CDlgChangeHead::OnBnClickedButtonHead2)
END_MESSAGE_MAP()


// CDlgChangeHead message handlers

BOOL CDlgChangeHead::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	m_btnClose.SetAutoSize(false);
	m_btnClose.Load(IDB_PNG_CLOSE);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetOkColor1(),theApp.GetHotColor(),theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetHotColor(),theApp.GetNormalColor());

	m_btnHead1.Load(IDB_PNG_TAB_86X49);
	m_btnHead1.SetChecked(true);
	m_btnHead2.Load(IDB_PNG_TAB_86X49);
	m_btnOpenFile.SetTextHotMove(false);
	m_btnOpenFile.Load(IDB_PNG_BTN_SELIMG);
	m_btnOpenFile.SetTextLeft(20);
	m_btnMyVideo.SetTextHotMove(false);
	m_btnMyVideo.Load(IDB_PNG_BTN_SELMYVIDEO);
	m_btnMyVideo.SetTextLeft(20);


	m_rectHead.left = 28;
	m_rectHead.right = m_rectHead.left + 80;
	m_rectHead.top = 151;
	m_rectHead.bottom = m_rectHead.top + 80;
	m_rectBig.left = m_rectHead.right+10;	// 间隔
	m_rectBig.right = m_rectBig.left + 120;
	m_rectBig.top = m_rectHead.top;
	m_rectBig.bottom = m_rectBig.top + 120;
	USES_CONVERSION;
	if (!m_sHeadResourceFile.empty() && PathFileExists(m_sHeadResourceFile.c_str()))
	{
		m_imageHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sHeadResourceFile.c_str()));
	}

	m_pDlgSelectHead = new CDlgSelectHead(this);
	m_pDlgSelectHead->Create(CDlgSelectHead::IDD,this);
	OnBnClickedButtonHead1();

	int nScreenWidth = 0;
	int nScreenHeight = 0;
	theApp.GetScreenSize(nScreenWidth,nScreenHeight);
	const int const_dlg_width = 432;
	const int const_dlg_height = 335;
	CRect rect;
	rect.left = nScreenWidth/2-const_dlg_width/2;
	rect.right = rect.left + const_dlg_width;
	rect.top = nScreenHeight/2-const_dlg_height/2;
	rect.bottom = rect.top + const_dlg_height;
	this->MoveWindow(&rect);
	this->SetCircle();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChangeHead::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	int btnx = 0;
	btnx = cx-m_btnClose.GetImgWidth()-2;
	m_btnClose.MovePoint(btnx, 2);

	int x = 14;
	int y = 41;
	m_btnHead1.MovePoint(x,y);
	x += m_btnHead1.GetImgWidth();
	m_btnHead2.MovePoint(x,y);
	
	x = 28;
	y = 98;
	m_btnOpenFile.MovePoint(x,y);
	x += (m_btnOpenFile.GetImgWidth()+18);
	m_btnMyVideo.MovePoint(x,y);

	y = 90;
	if (m_pDlgSelectHead!=NULL && m_pDlgSelectHead->GetSafeHwnd())
	{
		m_pDlgSelectHead->MoveWindow(1,y,cx-2,cy-y-54);	// 54=335-(90+56+135);最下面间隔
	}

	const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 80;
	const int const_btn_height = 32;//m_btnCancel.GetImgHeight();// 28;
	x = cx-const_btn_width-10;						// 最右边间隔
	const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	m_btnCancel.MovePoint(x, const_btn_y,const_btn_width,const_btn_height);
	x -= (const_btn_width+10);						// 按钮间隔
	m_btnOk.MovePoint(x, const_btn_y,const_btn_width,const_btn_height);
}

void CDlgChangeHead::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgChangeHead::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作矩形中居中
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
		DrawInfo();
	}
}

void CDlgChangeHead::DrawInfo(void)
{
	CPaintDC dc2(this);
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	if (theApp.GetColorGradient())
		this->AddBgDrawInfo(CEbBackDrawInfo(90,theDefaultBgColorTitleLight1,true));
	else
		this->AddBgDrawInfo(CEbBackDrawInfo(90,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(56,theDefaultBgColorBgLight3,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(135,theDefaultBgColorBgLight1,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);
	//Gdiplus::Image * pImage;
	//if (libEbc::ImageFromIDResource(IDB_PNG_IMG1_PEOPLE,_T("png"),pImage))
	//{
	//	graphics.DrawImage(pImage,PointF(10,7));
	//	delete pImage;
	//}
	graphics.DrawImage(theApp.m_image1People,PointF(10,7));
	// 写标题
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
	//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
	const Gdiplus::PointF pointTitle(40,13);
	graphics.DrawString(L"更换头像",-1,&fontEbTitle,pointTitle,&brushEbTitle);

	if (m_imageHead!=NULL)
	{
		graphics.DrawImage(m_imageHead, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
		graphics.DrawImage(m_imageHead, m_rectBig.left, m_rectBig.top, m_rectBig.Width(), m_rectBig.Height());
	}

}

void CDlgChangeHead::OnBnClickedOk()
{
	if (m_sNewHeadFile.empty() || m_sNewHeadFile==m_sHeadResourceFile) return;

	unsigned int nFileSize = 0;
	FILE * f = fopen(m_sNewHeadFile.c_str(), "rb");
	if (f == NULL)
	{
		return;
	}
#ifdef WIN32
	_fseeki64(f, 0, SEEK_END);
	nFileSize = _ftelli64(f);
#else
	fseeko(f, 0, SEEK_END);
	nFileSize = ftello(f);
#endif
	fclose(f);
	if (nFileSize>500*1024)	// 500KB
	{
		CDlgMessageBox::EbMessageBox(this,"头像文件超过500KB：",_T("请重新选择小于 500KB 图片文件后再试！"),CDlgMessageBox::IMAGE_WARNING,5);
		return;
	}

#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_SetMyGroupHeadFile(m_sGroupCode,m_sNewHeadFile.c_str());
#else
	theEBAppClient.EB_SetMyGroupHeadFile(m_sGroupCode,m_sNewHeadFile.c_str());
#endif
	m_sNewHeadFile.clear();
	//SetTimer(TIMERID_INVALIDATE_PARENT,10*1000,NULL);
}

void CDlgChangeHead::OnDestroy()
{
	if (m_imageHead!=NULL)
	{
		delete m_imageHead;
		m_imageHead = NULL;
	}
	if (m_pDlgSelectHead!=NULL)
	{
		m_pDlgSelectHead->DestroyWindow();
		delete m_pDlgSelectHead;
		m_pDlgSelectHead = NULL;
	}

	CEbDialogBase::OnDestroy();
}

static TCHAR BASED_CODE theHeadFilter[] = _T("Image Files (*.png;*jpg;*.bmp;*.ico)|*.png; *jpg; *.jpeg; *.bmp; *.ico|")
_T("PNG (*.png)|*.png|")
_T("JPEG (*.jpg;*jpeg)|*.jpg; *.jpeg|")
_T("BMP (*.bmp)|*.bmp|")
_T("ICO (*.ico)|*.ico|")
_T("All Files (*.*)|*.*||");

void CDlgChangeHead::OnBnClickedButtonOpenfile()
{
	AFX_SWITCH_INSTANCED();
	CFileDialog dlg(TRUE, 0, NULL, OFN_READONLY|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, theHeadFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		m_sNewHeadFile = dlg.GetPathName();
		if (m_imageHead)
			delete m_imageHead;
		USES_CONVERSION;
		m_imageHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sNewHeadFile.c_str()));
		this->Invalidate();
	}
}

void CDlgChangeHead::OnBnClickedButtonMyVideo()
{
	char lpszBuffer[260];
#ifdef USES_EBCOM_TEST
	CEBString sAppDataTempPath = theEBClientCore->EB_AppDataTempPath.GetBSTR();
#else
	CEBString sAppDataTempPath = theEBAppClient.EB_GetAppDataTempPath();
#endif
	sprintf(lpszBuffer,"%s\\DH%lld.jpg",sAppDataTempPath.c_str(),m_sGroupCode);
	tstring sBmpPhotoFile = lpszBuffer;

	AFX_SWITCH_INSTANCED();
	CDlgVideoPhoto pdlg;
	pdlg.m_sSavePhotoFile = lpszBuffer;
	if (pdlg.DoModal()==IDOK)
	{
		m_sNewHeadFile = pdlg.m_sSavePhotoFile;
		if (m_imageHead)
			delete m_imageHead;
		USES_CONVERSION;
		m_imageHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sNewHeadFile.c_str()));
		this->Invalidate();
	}
}

void CDlgChangeHead::OnTimer(UINT_PTR nIDEvent)
{
	//if (TIMERID_INVALIDATE_PARENT==nIDEvent)
	//{
	//	
	//}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgChangeHead::OnBnClickedButtonHead1()
{
	m_btnOpenFile.ShowWindow(SW_HIDE);
	m_btnMyVideo.ShowWindow(SW_HIDE);
	m_pDlgSelectHead->ShowWindow(SW_SHOW);
	m_btnHead1.SetChecked(true);
	m_btnHead1.Invalidate();
	m_btnHead2.SetChecked(false);
	m_btnHead2.Invalidate();
	theApp.InvalidateParentRect(&m_btnHead2);
}

void CDlgChangeHead::OnBnClickedButtonHead2()
{
	m_btnOpenFile.ShowWindow(SW_SHOW);
	m_btnMyVideo.ShowWindow(SW_SHOW);
	m_pDlgSelectHead->ShowWindow(SW_HIDE);
	m_btnHead1.SetChecked(false);
	m_btnHead1.Invalidate();
	theApp.InvalidateParentRect(&m_btnHead1);
	m_btnHead2.SetChecked(true);
	m_btnHead2.Invalidate();
}
//
//void CDlgChangeHead::OnSelectedImageInfo(const CEBImageDrawInfo& pSelectedImageInfo)
//{
//#ifdef USES_EBCOM_TEST
//	theEBClientCore->EB_SetMyGroupHeadRes(m_sGroupCode,pSelectedImageInfo.m_sResId);
//#else
//	theEBAppClient.EB_SetMyGroupHeadRes(m_sGroupCode,pSelectedImageInfo.m_sResId);
//#endif
//	this->ShowWindow(SW_HIDE);
//	if (m_pCallback)
//		m_pCallback->OnSelectedImageInfo(pImageDrawInfo);
//}

void CDlgChangeHead::SetCallback(CImageSelectCallback* pCallback)
{
	m_pDlgSelectHead->SetCallback(pCallback);
}
