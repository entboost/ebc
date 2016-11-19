// DlgChangeHead.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
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
	m_imageHead1 = NULL;
	m_imageHead2 = NULL;
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
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnOk.SetTextHotMove(false);
	m_btnCancel.SetTextHotMove(false);
	m_btnOk.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnOk.SetNorTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetHotTextColor(theDefaultBtnWhiteColor); 
	m_btnOk.SetPreTextColor(theDefaultBtnWhiteColor); 
	m_btnCancel.SetDrawLine(5,1,0,theDefaultFlatLineColor,theApp.GetNormalColor());
	m_btnCancel.SetDrawPanel(true,theDefaultBtnWhiteColor);
	m_btnCancel.SetHotTextColor(theApp.GetNormalColor()); 
	m_btnCancel.SetPreTextColor(theApp.GetNormalColor()); 

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
	m_rectHead.right = m_rectHead.left + 120;
	m_rectHead.top = 151;
	m_rectHead.bottom = m_rectHead.top + 120;
	m_rectBig.left = m_rectHead.right+20;	// 间隔
	m_rectBig.right = m_rectBig.left + 120;
	m_rectBig.top = m_rectHead.top;
	m_rectBig.bottom = m_rectBig.top + 120;
	USES_CONVERSION;
	if (!m_sHeadResourceFile.empty() && PathFileExists(m_sHeadResourceFile.c_str()))
	{
		m_imageHead1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sHeadResourceFile.c_str()));
	}

	m_pDlgSelectHead = new CDlgSelectHead(this);
	m_pDlgSelectHead->Create(CDlgSelectHead::IDD,this);
	OnBnClickedButtonHead1();

	const int nScreenWidth = theApp.GetScreenWidth();
	const int nScreenHeight = theApp.GetScreenHeight();
	const int const_dlg_width = 432;
	const int const_dlg_height = 335;
	CRect rect;
	rect.left = nScreenWidth/2-const_dlg_width/2;
	rect.right = rect.left + const_dlg_width;
	rect.top = nScreenHeight/2-const_dlg_height/2;
	rect.bottom = rect.top + const_dlg_height;
	this->MoveWindow(&rect);
	this->SetCircle();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChangeHead::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

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
		m_pDlgSelectHead->MoveWindow(1,y,cx-2,cy-y-44);	// 54=335-(90+56+135);最下面间隔
	}

	x = cx-theDefaultBtnWidth-theDefaultBtnRightIntever;					// 最右边间隔
	const int const_btn_y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;	// 最下面间隔
	m_btnCancel.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	x -= (theDefaultBtnWidth+theDefaultBtnIntever);
	m_btnOk.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);

	//const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 80;
	//const int const_btn_height = 32;//m_btnCancel.GetImgHeight();// 28;
	//x = cx-const_btn_width-10;						// 最右边间隔
	//const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	//m_btnCancel.MovePoint(x, const_btn_y,const_btn_width,const_btn_height);
	//x -= (const_btn_width+10);						// 按钮间隔
	//m_btnOk.MovePoint(x, const_btn_y,const_btn_width,const_btn_height);
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
	this->AddBgDrawInfo(CEbBackDrawInfo(90,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1, 90, 2);

	//if (theApp.GetColorGradient())
	//	this->AddBgDrawInfo(CEbBackDrawInfo(90,theDefaultBgColorTitleLight1,true));
	//else
	//	this->AddBgDrawInfo(CEbBackDrawInfo(90,1.0,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(56,theDefaultBgColorBgLight3,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(135,theDefaultBgColorBgLight1,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//this->DrawPopBg(&memDC, theApp.GetMainColor());
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
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	const Gdiplus::PointF pointTitle(40,11);
	graphics.DrawString(L"更换头像",-1,&fontEbTitle,pointTitle,&brushEbTitle);

	if (m_imageHead1!=NULL)
	{
		const Gdiplus::Font fontHeadString(&fontFamily, 12, FontStyleRegular, UnitPixel);
		Gdiplus::SolidBrush brushHeadString(Gdiplus::Color(237,28,36));

		int nDrawHeadStringY = m_rectHead.bottom;
		if (m_imageHead2!=NULL)
		{
			// 选择图片
			if (m_imageHead1->GetWidth()>m_imageHead1->GetHeight())
			{
				const int nDrawHeight = (m_rectHead.Width()*m_imageHead1->GetHeight())/m_imageHead1->GetWidth();
				graphics.DrawImage(m_imageHead1, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), nDrawHeight);
				Gdiplus::Pen pen(Gdiplus::Color(237,28,36));
				int x = m_rectHead.left+(m_rectHead.Width()-nDrawHeight)/2;
				int y = m_rectHead.top;
				graphics.DrawLine(&pen,x,y,x,y+nDrawHeight);
				x += nDrawHeight;
				graphics.DrawLine(&pen,x,y,x,y+nDrawHeight);
				nDrawHeadStringY = y+nDrawHeight;
			}else if (m_imageHead1->GetWidth()<m_imageHead1->GetHeight())
			{
				const int nDrawWidth = (m_rectHead.Height()*m_imageHead1->GetWidth())/m_imageHead1->GetHeight();
				graphics.DrawImage(m_imageHead1, m_rectHead.left, m_rectHead.top, nDrawWidth, m_rectHead.Height());
				Gdiplus::Pen pen(Gdiplus::Color(237,28,36));
				int x = m_rectHead.left;
				int y = m_rectHead.top;
				//int y = m_rectHead.top+(m_rectHead.Height()-nDrawWidth)/2;
				graphics.DrawLine(&pen,x,y,x+nDrawWidth,y);
				y += nDrawWidth;
				graphics.DrawLine(&pen,x,y,x+nDrawWidth,y);
			}else
			{
				graphics.DrawImage(m_imageHead1, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
			}
			graphics.DrawImage(m_imageHead2, m_rectBig.left, m_rectBig.top, m_rectBig.Width(), m_rectBig.Height());
		}else
		{
			// 不是选择图片
			graphics.DrawImage(m_imageHead1, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
			graphics.DrawImage(m_imageHead1, m_rectBig.left, m_rectBig.top, m_rectBig.Width(), m_rectBig.Height());
		}
		graphics.DrawString(L"图片文件：",-1,&fontHeadString,Gdiplus::PointF(m_rectHead.left,nDrawHeadStringY),&brushHeadString);
		graphics.DrawString(L"头像效果：",-1,&fontHeadString,Gdiplus::PointF(m_rectBig.left,m_rectBig.bottom),&brushHeadString);
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
		CDlgMessageBox::EbMessageBox(this,"",_T("头像文件超过 500KB：\r\n请选择小于 500KB 图片文件后重试！"),CDlgMessageBox::IMAGE_WARNING,5);
		return;
	}

	SetHeadResorceFile(m_sNewHeadFile);
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_SetMyGroupHeadFile(m_sGroupCode,m_sNewHeadFile.c_str());
#else
	theEBAppClient.EB_SetMyGroupHeadFile(m_sGroupCode,m_sNewHeadFile.c_str());
#endif
	m_sNewHeadFile.clear();
	this->GetParent()->PostMessage(EB_COMMAND_CHANGE_HEAD);
	this->Invalidate();
	//SetTimer(TIMERID_INVALIDATE_PARENT,10*1000,NULL);
}

void CDlgChangeHead::OnDestroy()
{
	if (m_imageHead1!=NULL)
	{
		delete m_imageHead1;
		m_imageHead1 = NULL;
	}
	if (m_imageHead2!=NULL)
	{
		delete m_imageHead2;
		m_imageHead2 = NULL;
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
	CFileDialog dlg(TRUE, 0, NULL, OFN_READONLY|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, theHeadFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		const tstring sSelectImageFile = dlg.GetPathName();
#ifdef USES_EBCOM_TEST
		const CEBString sAppDataTempPath = theEBClientCore->EB_AppDataTempPath.GetBSTR();
#else
		const CEBString sAppDataTempPath = theEBAppClient.EB_GetAppDataTempPath();
#endif
		char lpszBuffer[260];
		sprintf(lpszBuffer,"%s\\%08d%06d.bmp",sAppDataTempPath.c_str(),(int)time(0)%100000000,rand()%1000000);
		const tstring sBmpPhotoFile(lpszBuffer);
		// ** 截取图片，中间位置
		//HGDIOBJ hHead = NULL;
		//int nNewImageWidth = 0 ;
		//{
		//	CImageEx pImage;
		//	if (!pImage.LoadFilename(sSelectImageFile.c_str()))
		//	{
		//		CDlgMessageBox::EbDoModal(this,_T(""),_T("选择图片截图失败：\r\n请重试！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		//		return;
		//	}
		//	const int nImageWidth = pImage.GetWidth();
		//	const int nImageHeight = pImage.GetHeight();
		//	if (nImageWidth>nImageHeight)
		//	{
		//		const int x = (nImageWidth-nImageHeight)/2;
		//		hHead = pImage.Copy(x,0,nImageHeight,nImageHeight);
		//		nNewImageWidth = nImageHeight;
		//	}else if (nImageWidth<nImageHeight)
		//	{
		//		const int x = (nImageWidth-nImageHeight)/2;
		//		hHead = pImage.Copy(0,0,nImageWidth,nImageWidth);
		//		nNewImageWidth = nImageWidth;
		//	}else
		//	{
		//		hHead = pImage.Copy(0,0,nImageHeight,nImageHeight);
		//		nNewImageWidth = nImageWidth;
		//	}
		//	if (hHead==NULL)
		//	{
		//		CDlgMessageBox::EbDoModal(this,_T(""),_T("图片截图失败：\r\n请重试！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		//		return;
		//	}
		//}
		// 
		if (m_imageHead1)
		{
			delete m_imageHead1;
			m_imageHead1 = NULL;
		}
		if (m_imageHead2)
		{
			delete m_imageHead2;
			m_imageHead2 = NULL;
		}
		USES_CONVERSION;
		int nNewImageWidth = 0;
		{
			Bitmap* b1 = Bitmap::FromFile(T2W(sSelectImageFile.c_str()));
			const int nImageWidth = b1->GetWidth();
			const int nImageHeight = b1->GetHeight();
			Bitmap* b2 = NULL;
			if (nImageWidth>nImageHeight)
			{
				const int x = (nImageWidth-nImageHeight)/2;
				b2 = b1->Clone(x,0,nImageHeight,nImageHeight,PixelFormat24bppRGB);
				nNewImageWidth = nImageHeight;
			}else if (nImageWidth<nImageHeight)
			{
				b2 = b1->Clone(0,0,nImageWidth,nImageWidth,PixelFormat24bppRGB);
				nNewImageWidth = nImageWidth;
			}else
			{
				b2 = b1->Clone(0,0,nImageWidth,nImageWidth,PixelFormat24bppRGB);
				nNewImageWidth = nImageWidth;
			}
			delete b1;
			CLSID                           codecClsid;
			libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
			b2->Save(T2W(sBmpPhotoFile.c_str()),&codecClsid);
			delete b2;
		}

		//// 保存 bmp
		//{
		//	Bitmap* b = Bitmap::FromHBITMAP((HBITMAP)hHead,NULL);
		//	CLSID                           codecClsid;
		//	libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
		//	Gdiplus::Status stat = b->Save(T2W(sBmpPhotoFile.c_str()),&codecClsid);
		//	delete b;
		//	DeleteObject(hHead);
		//}
		if (m_sNewHeadFile.empty())
		{
			sprintf(lpszBuffer,"%s\\%08d%06d.jpg",sAppDataTempPath.c_str(),(int)time(0)%100000000,rand()%1000000);
			m_sNewHeadFile = lpszBuffer;
		}else
		{
			DeleteFile(m_sNewHeadFile.c_str());
		}
		// ** BMP 转成 jpg
		if (nNewImageWidth>2048)
			libEbc::bmp_2_jpg(A2W_ACP(sBmpPhotoFile.c_str()),A2W_ACP(m_sNewHeadFile.c_str()),60);
		else if (nNewImageWidth>1024)
			libEbc::bmp_2_jpg(A2W_ACP(sBmpPhotoFile.c_str()),A2W_ACP(m_sNewHeadFile.c_str()),80);
		else if (nNewImageWidth>512)
			libEbc::bmp_2_jpg(A2W_ACP(sBmpPhotoFile.c_str()),A2W_ACP(m_sNewHeadFile.c_str()),90);
		else
			libEbc::bmp_2_jpg(A2W_ACP(sBmpPhotoFile.c_str()),A2W_ACP(m_sNewHeadFile.c_str()),100);
		DeleteFile(sBmpPhotoFile.c_str());
		if (!::PathFileExists(m_sNewHeadFile.c_str()))
		{
			CDlgMessageBox::EbDoModal(this,_T(""),_T("图片转换失败：\r\n请重试！"),CDlgMessageBox::IMAGE_ERROR,true,5);
			return;
		}
		m_imageHead1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(sSelectImageFile.c_str()));
		m_imageHead2 = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sNewHeadFile.c_str()));
		this->Invalidate();
#ifdef _DEBUG
		//CopyFile(m_sNewHeadFile.c_str(),"d:\\tn\\entboost\\test.jpg",FALSE);
#endif
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
	CDlgVideoPhoto pdlg;
	pdlg.m_sSavePhotoFile = lpszBuffer;
	if (pdlg.DoModal()==IDOK)
	{
		m_sNewHeadFile = pdlg.m_sSavePhotoFile;
		if (m_imageHead2)
		{
			delete m_imageHead2;
			m_imageHead2 = NULL;
		}
		if (m_imageHead1)
			delete m_imageHead1;
		USES_CONVERSION;
		m_imageHead1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sNewHeadFile.c_str()));
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

void CDlgChangeHead::SetHeadResorceFile(const tstring& sHeadResourceFile)
{
	m_sHeadResourceFile = sHeadResourceFile;
	if (m_imageHead1!=NULL)
	{
		delete m_imageHead1;
		USES_CONVERSION;
		m_imageHead1 = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sHeadResourceFile.c_str()));
	}
}
