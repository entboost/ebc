// DlgVideoPhoto.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgVideoPhoto.h"
#include "Core/SkinMemDC.h"

const int const_video_left = 20;
const int const_video_top = 60;
//const int const_video_width = 320;
//const int const_video_height = 240;
const int const_head_width = 80;
const int const_head_height = 80;

// CDlgVideoPhoto dialog

IMPLEMENT_DYNAMIC(CDlgVideoPhoto, CEbDialogBase)

CDlgVideoPhoto::CDlgVideoPhoto(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgVideoPhoto::IDD, pParent)
{
	m_imageHead = NULL;
	m_pMyVideo = NULL;

}

CDlgVideoPhoto::~CDlgVideoPhoto()
{
}

void CDlgVideoPhoto::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_PHOTO, m_btnPhoto);
}


BEGIN_MESSAGE_MAP(CDlgVideoPhoto, CEbDialogBase)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgVideoPhoto::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_PHOTO, &CDlgVideoPhoto::OnBnClickedButtonPhoto)
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CDlgVideoPhoto message handlers

BOOL CDlgVideoPhoto::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	m_btnClose.SetAutoSize(false);
	m_btnClose.SetDrawToolButtonPic(4,theDefaultBtnWhiteColor,-1,-1,-1,2);
	m_btnClose.SetDrawPanel(true,theApp.GetMainColor(),theDefaultBtnCloseColor,theDefaultBtnCloseColor);
	m_btnClose.SetDrawPanelRgn(false);
	m_btnClose.SetToolTipText(_T("关闭"));
	m_btnPhoto.SetAutoSize(false);
	m_btnPhoto.Load(IDB_PNG_71X32G);
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

	m_rectBig.left = const_video_left;
	m_rectBig.right = m_rectBig.left + const_video_width;
	m_rectBig.top = const_video_top;
	m_rectBig.bottom = m_rectBig.top + const_video_height;
	m_rectHead.left = m_rectBig.right+15;
	m_rectHead.right = m_rectHead.left + const_head_width;
	m_rectHead.bottom = m_rectBig.bottom;
	m_rectHead.top = m_rectHead.bottom-const_head_height;

	const int const_dlg_width = 55+const_video_width+const_head_width;
	const int const_dlg_height = 60+const_video_height+const_head_height;
	CRect rect;
	this->GetWindowRect(&rect);
	rect.right = rect.left + const_dlg_width;
	rect.bottom = rect.top + const_dlg_height;
	this->MoveWindow(&rect);
	this->SetCircle();

	OpenLocalVideo();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgVideoPhoto::OnPaint()
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

void CDlgVideoPhoto::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_minbtn_width = 24;
	const int const_minbtn_height = 22;
	int btnx = 0;
	btnx = cx-const_minbtn_width-2;
	if (m_btnClose.GetSafeHwnd()!=NULL)
		m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

	int x = cx-theDefaultBtnWidth-theDefaultBtnRightIntever;					// 最右边间隔
	const int const_btn_y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;	// 最下面间隔
	m_btnCancel.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	x -= (theDefaultBtnWidth+theDefaultBtnIntever);
	m_btnOk.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);
	x -= (theDefaultBtnWidth+theDefaultBtnIntever);
	//x -= (m_btnPhoto.GetImgWidth()+theDefaultBtnIntever);
	m_btnPhoto.MovePoint(x,const_btn_y,theDefaultBtnWidth,theDefaultBtnHeight);

	//const int const_btn_width = 81;//m_btnCancel.GetImgWidth();// 80;
	//const int const_btn_height = 32;//m_btnCancel.GetImgHeight();// 28;
	//const int const_btn_intever = 10;				// 按钮间隔
	//int x = cx-const_btn_width-10;					// 最右边间隔
	//const int const_btn_y = cy-const_btn_height-10;	// 最下面间隔
	//m_btnCancel.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	//x -= (const_btn_width+const_btn_intever);
	//m_btnOk.MovePoint(x,const_btn_y,const_btn_width,const_btn_height);
	//x -= (m_btnPhoto.GetImgWidth()+const_btn_intever);
	//m_btnPhoto.MovePoint(x, const_btn_y);
}

void CDlgVideoPhoto::OnDestroy()
{
	CEbDialogBase::OnDestroy();
	if (m_imageHead)
	{
		delete m_imageHead;
		m_imageHead = NULL;
	}
	CloseVideo();
}

void CDlgVideoPhoto::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgVideoPhoto::DrawInfo(void)
{
	CPaintDC dc2(this);
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,theDefaultBgColorTitleHeight,2);

	//if (theApp.GetColorGradient())
	//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight1,true));
	//else
	//	this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,1.0,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(235,theDefaultBgColorBgLight1,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight3,false));
	//this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);

	// 写标题
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontEbTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	const Gdiplus::PointF pointTitle(12,12);
	graphics.DrawString(L"自拍头像",-1,&fontEbTitle,pointTitle,&brushEbTitle);
	if (m_imageHead!=NULL)
	{
		graphics.DrawImage(m_imageHead, m_rectHead.left, m_rectHead.top, m_rectHead.Width(), m_rectHead.Height());
	}
	Gdiplus::Pen pen(Gdiplus::Color(237,28,36));
	int x = const_video_left+(const_video_width-const_video_height)/2;
	int y = const_video_top;
	graphics.DrawLine(&pen,x,y,x,y-2);	// 左上
	x += const_video_height;
	graphics.DrawLine(&pen,x,y,x,y-2);	// 右上
	y += const_video_height;
	graphics.DrawLine(&pen,x,y,x,y+2);	// 右下
	x -= const_video_height;
	graphics.DrawLine(&pen,x,y,x,y+2);	// 左下
}

void CDlgVideoPhoto::OnBnClickedButtonPhoto()
{
	if (m_pMyVideo != NULL && m_pMyVideo->GetSafeHwnd()!=NULL)
	{
		if (m_imageHead!=NULL)
		{
			delete m_imageHead;
			m_imageHead = NULL;
		}

#ifdef USES_EBCOM_TEST
		const CEBString sAppDataTempPath = theEBClientCore->EB_AppDataTempPath.GetBSTR();
#else
		const CEBString sAppDataTempPath = theEBAppClient.EB_GetAppDataTempPath();
#endif

		char lpszBuffer[260];
		sprintf(lpszBuffer,"%s\\%08d%06d.bmp",sAppDataTempPath.c_str(),(int)time(0)%100000000,rand()%1000000);
		const tstring sBmpPhotoFile(lpszBuffer);
		// ** 拍照
		m_pVideoRoom->SaveVideoBitmapFile(theApp.GetLogonUserId(),sBmpPhotoFile.c_str());
		// ** 截图 240*240
		//HGDIOBJ hHead = NULL;
		//{
		//	CImageEx pImage;
		//	if (!pImage.LoadFilename(sBmpPhotoFile.c_str()))
		//	{
		//		CDlgMessageBox::EbDoModal(this,_T(""),_T("拍照失败：\r\n请检查设备后重试！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		//		return;
		//	}
		//	const int x = (const_video_width-const_video_height)/2;
		//	hHead = pImage.Copy(x,0,const_video_height,const_video_height);
		//	if (hHead==NULL)
		//	{
		//		CDlgMessageBox::EbDoModal(this,_T(""),_T("截图失败：\r\n请重试！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		//		return;
		//	}
		//}
		if (m_sSavePhotoFile.empty())
		{
			sprintf(lpszBuffer,"%s\\%08d%06d.jpg",sAppDataTempPath.c_str(),(int)time(0)%100000000,rand()%1000000);
			m_sSavePhotoFile = lpszBuffer;
		}else
		{
			DeleteFile(m_sSavePhotoFile.c_str());
		}
		USES_CONVERSION;
		// ** 截图 240*240
		{
			Bitmap* b1 = Bitmap::FromFile(T2W(sBmpPhotoFile.c_str()));
			const int x = (const_video_width-const_video_height)/2;
			Bitmap* b2 = b1->Clone(x,0,const_video_height,const_video_height,PixelFormat24bppRGB);
			delete b1;
			CLSID                           codecClsid;
			libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
			b2->Save(T2W(sBmpPhotoFile.c_str()),&codecClsid);
			delete b2;
		}
		//{
		//	Bitmap* b = Bitmap::FromHBITMAP((HBITMAP)hHead,NULL);
		//	CLSID                           codecClsid;
		//	libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
		//	b->Save(T2W(sBmpPhotoFile.c_str()),&codecClsid);
		//	delete b;
		//	DeleteObject(hHead);
		//}
		// ** BMP 转成 png
		libEbc::bmp_2_jpg(A2W_ACP(sBmpPhotoFile.c_str()),A2W_ACP(m_sSavePhotoFile.c_str()),100);
		DeleteFile(sBmpPhotoFile.c_str());
		if (PathFileExists(m_sSavePhotoFile.c_str()))
		{
			m_imageHead = libEbc::LoadImageFromFile(m_sSavePhotoFile.c_str());
			if (m_imageHead==NULL)
				m_imageHead = new Gdiplus::Image((const WCHAR*)A2W_ACP(m_sSavePhotoFile.c_str()));
		}
		this->Invalidate();
#ifdef _DEBUG
		//CopyFile(m_sSavePhotoFile.c_str(),"D:\\tn\\entboost\\test.jpg",FALSE);
#endif
	}
}
void CDlgVideoPhoto::Fire_onVideoCallback(LONGLONG nUserId, LONG nCallbackType, ULONG lParam, ULONG wParam, ULONG nUserData)
{
	if (nUserData == 0) return;
	//((CDlgVideo*)nUserData)->DrawVideo(pVideoData, nVideoSize);
}

void CDlgVideoPhoto::OpenLocalVideo(void)
{
	int nLocalVideoIndex = (int)theApp.GetProfileInt(_T("param"),_T("local-video-index"),0);
	std::vector<tstring> pVideoDevices;
#ifdef USES_EBCOM_TEST
	_variant_t pVideoDeviceList = theEBClientCore->EB_GetVideoDeviceList();
	if (pVideoDeviceList.vt!=VT_EMPTY && pVideoDeviceList.parray != NULL)
	{
		CComSafeArray<VARIANT> m_sa(pVideoDeviceList.parray);
		for (ULONG i=0;i<m_sa.GetCount();i++)
		{
			CComVariant var = m_sa.GetAt(i);
			if (var.vt != VT_BSTR)
				continue;
			const CEBString sVideoDevice(var.bstrVal);
			pVideoDevices.push_back(sVideoDevice);
		}
	}
#else
	CEBAppClient::EB_GetVideoDeviceList(pVideoDevices);
#endif
	if (pVideoDevices.empty())
	{
		CDlgMessageBox::EbDoModal(this,_T(""),_T("本地找不到视频设备：\r\n请检查设备后再试一次！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		return;
	}
	if (nLocalVideoIndex>=(int)pVideoDevices.size())
		nLocalVideoIndex = 0;
	if (!CreateVideoRoom())
	{
		CDlgMessageBox::EbDoModal(this,_T(""),_T("找不到视频聊天组件：\r\n请重新安装程序，或重新登录再试一次！"),CDlgMessageBox::IMAGE_ERROR,true,5);
		return;
	}
	if (m_pMyVideo == NULL)
	{
		// 打开本地视频
		const HRESULT hr = m_pVideoRoom->InitVideoCapture(nLocalVideoIndex,10);
		if (hr!=S_OK)
			CDlgMessageBox::EbMessageBox(this,_T(""),_T("打开视频摄像头失败：\r\n请检查摄像头是否被占用或其他硬件故障！"),CDlgMessageBox::IMAGE_ERROR,5);

		//CRect m_rectMy;
		//m_rectMy.left = 10;
		//m_rectMy.right = m_rectMy.left+const_video_width;
		//m_rectMy.top = 40;
		//m_rectMy.bottom = m_rectMy.top + const_video_height;
		m_pMyVideo = new CDlgVideo(this);
		m_pMyVideo->Create(CDlgVideo::IDD,this);
		m_pMyVideo->MoveWindow(&m_rectBig);
		m_pMyVideo->ShowWindow(SW_SHOW);
		m_pVideoRoom->StartVideoCapture((DWORD)m_pMyVideo->GetSafeHwnd());
		m_pVideoRoom->StartVideoGraph(theApp.GetLogonUserId());
	}
}

void CDlgVideoPhoto::CloseVideo(void)
{
	DispEventUnadvise(m_pVideoRoom);
	if (m_pVideoRoom != NULL)
	{
		// 关闭本地视频
		m_pVideoRoom->StopVideoCapture();
		m_pVideoRoom->StopAudioCapture();
		m_pVideoRoom->UnRegister();
		m_pVideoRoom->Stop();
		m_pVideoRoom.Release();
	}
	if (m_pMyVideo != NULL)
	{
		m_pMyVideo->DestroyWindow();
		delete m_pMyVideo;
		m_pMyVideo = NULL;
	}
}

bool CDlgVideoPhoto::CreateVideoRoom(void)
{
	if (m_pVideoRoom == NULL)
	{
		CoCreateInstance(__uuidof(EBMMRoom),NULL,CLSCTX_INPROC,__uuidof(IEBMMRoom),(void**)&m_pVideoRoom);
		if (m_pVideoRoom==NULL) return false;
		m_pVideoRoom->Start("","",0);
		m_pVideoRoom->Register(0,theApp.GetLogonUserId(),0,VARIANT_FALSE);
		m_pVideoRoom->LocalVideoSend = VARIANT_FALSE;
		m_pVideoRoom->LocalAudioSend = VARIANT_FALSE;

		// OK
		DispEventAdvise(m_pVideoRoom);
	}
	return true;
}

void CDlgVideoPhoto::OnMove(int x, int y)
{
	CEbDialogBase::OnMove(x, y);
	if (m_pMyVideo!=NULL && m_pMyVideo->GetSafeHwnd())
	{
		m_pMyVideo->OnMove();
	}
}
