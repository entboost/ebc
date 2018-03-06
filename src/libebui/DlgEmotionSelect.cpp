// DlgEmotionSelect.cpp : implementation file
//
#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgEmotionSelect.h"
#include <Mmsystem.h>

const int const_emotion_width = 32;
const int const_emotion_height = 32;
const int const_emotion_top_inteval = 1;	// 上边间隔
const int const_emotion_h_inteval = 1;		// 左右二边间隔
const int const_emotion_h_count = 12;		// 水平数量
const int const_emotion_v_count = 6;		// 垂直数量
const int const_emotion_inteval = 1;		// 格线间隔

// CDlgEmotionSelect dialog

IMPLEMENT_DYNAMIC(CDlgEmotionSelect, CEbDialogBase)

CDlgEmotionSelect::CDlgEmotionSelect(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgEmotionSelect::IDD, pParent)
{
	m_pCallback = NULL;
}

CDlgEmotionSelect::~CDlgEmotionSelect()
{
}

void CDlgEmotionSelect::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
}

#define WM_EV_CLICK_EMOTION		(WM_USER+0x111)
BEGIN_MESSAGE_MAP(CDlgEmotionSelect, CEbDialogBase)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_EV_CLICK_EMOTION, OnWmEVClickEmotion)
	//ON_MESSAGE(WM_EV_KILL_FOCUS, OnWmEVKillFocus)
END_MESSAGE_MAP()


// CDlgEmotionSelect message handlers

BOOL CDlgEmotionSelect::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	//this->SetMouseMove(FALSE);

	if (!m_pEmotionViewControl.CreateControl(__uuidof(EBEmotionView),NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,10,10),this,1111))
		return FALSE;
	LPUNKNOWN pEbEmotionViewUnknown = m_pEmotionViewControl.GetControlUnknown();
	if (pEbEmotionViewUnknown == NULL)
		return FALSE;
	pEbEmotionViewUnknown->QueryInterface(__uuidof(IEBEmotionView),(void**)&m_pEbEmotionView);
	if (m_pEbEmotionView==NULL)
		return FALSE;
	m_pEbEmotionView->EmotionHeight = 24;
	m_pEbEmotionView->EmotionWidth = 24;
	m_pEbEmotionView->LineInterval = 8;
	SetCtrlColor();

	//
	//m_pEmotionView.Create(CRect(0,0,0,0),this);
	//m_pEmotionView.SetBkColor(theApp.GetMainColor());
	//m_pEmotionView.SetScrollBarBmp(IDB_VERTICAL_SCROLLBAR_UPARROW, IDB_VERTICAL_SCROLLBAR_SPAN, IDB_VERTICAL_SCROLLBAR_DOWNARROW,
	//	IDB_VERTICAL_SCROLLBAR_THUMB, IDB_VERTICAL_SCROLLBAR_TOP, IDB_VERTICAL_SCROLLBAR_BOTTOM );
	ShowImage();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
bool CDlgEmotionSelect::GetImageResourceId(DWORD dwUserData,eb::bigint& pOutResourceId)
{
	for (size_t i=0; i<m_pImageDrawList.size(); i++)
	{
		CEBImageDrawInfo pImageDrawInfo = m_pImageDrawList[i];
		if (pImageDrawInfo.m_dwUserData == dwUserData)
		{
			pOutResourceId = pImageDrawInfo.m_sResId;
			return true;
		}
	}
	return false;
}
bool CDlgEmotionSelect::GetImageDrawInfo(DWORD dwUserData,CEBImageDrawInfo& pOutImageInfo)
{
	for (size_t i=0; i<m_pImageDrawList.size(); i++)
	{
		CEBImageDrawInfo pImageDrawInfo = m_pImageDrawList[i];
		if (pImageDrawInfo.m_dwUserData == dwUserData)
		{
			pOutImageInfo = pImageDrawInfo;
			return true;
		}
	}
	return false;
}

void CDlgEmotionSelect::ShowImage(void)
{
	if (m_pEbEmotionView!=NULL && m_pImageDrawList.empty())
	{
#ifdef USES_EBCOM_TEST
		_variant_t pEnterpriseImageList = theEBClientCore->EB_GetMyEmotionList();
		if (pEnterpriseImageList.vt!=VT_EMPTY && pEnterpriseImageList.parray != NULL)
		{
			CComSafeArray<VARIANT> m_sa(pEnterpriseImageList.parray);
			for (ULONG i=0;i<m_sa.GetCount();i++)
			{
				CComVariant var = m_sa.GetAt(i);
				if (var.vt != VT_DISPATCH)
					continue;
				CComPtr<IEB_EmotionInfo> pEmotionInfo;
				var.pdispVal->QueryInterface(__uuidof(IEB_EmotionInfo),(void**)&pEmotionInfo);
				if (pEmotionInfo == NULL) continue;

				const CEBString sResFile(pEmotionInfo->ResFile.GetBSTR());
				if (sResFile.empty()) continue;
				if (!::PathFileExists(sResFile.c_str())) continue;

				CEBImageDrawInfo pDrawInfo((EB_EmotionInfo)pEmotionInfo);
				pDrawInfo.m_dwUserData = i+1;
				m_pImageDrawList.push_back(pDrawInfo);
				m_pEbEmotionView->AddEmotion(pEmotionInfo->ResFile,pEmotionInfo->Description,pDrawInfo.m_dwUserData);
			}
		}
#else
		USES_CONVERSION;
		std::vector<EB_EmotionInfo> pEnterpriseImageList;
		theEBAppClient.EB_GetMyEmotionList(pEnterpriseImageList);
		for (size_t i=0; i<pEnterpriseImageList.size(); i++)
		{
			EB_EmotionInfo pEmotionInfo = pEnterpriseImageList[i];
			if (pEmotionInfo.m_sResFile.empty()) continue;
			if (!::PathFileExists(pEmotionInfo.m_sResFile.c_str())) continue;

			CEBImageDrawInfo pDrawInfo(pEmotionInfo);
			pDrawInfo.m_dwUserData = i+1;
			m_pImageDrawList.push_back(pDrawInfo);
			m_pEbEmotionView->AddEmotion(A2W_ACP(pEmotionInfo.m_sResFile.c_str()),A2W_ACP(pEmotionInfo.m_sDescription.c_str()),pDrawInfo.m_dwUserData);
		}
#endif
	}
	m_pEmotionViewControl.ShowWindow(SW_SHOW);
	this->ShowWindow(SW_SHOW);
}
void CDlgEmotionSelect::SetCtrlColor(void)
{
	if (m_pEbEmotionView != NULL)
	{
		m_pEbEmotionView->SetBackgroundColor(GetRValue(theApp.GetBgColor1()),GetGValue(theApp.GetBgColor1()),GetBValue(theApp.GetBgColor1()));
		//m_pEbEmotionView->SetBorderColor();
	}
}

void CDlgEmotionSelect::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_top_inteval = 52;	// 上边二边间隔
	const int const_h_inteval = 10;		// 左右二边间隔
	const int const_emotion_view_width = const_emotion_h_inteval*2+const_emotion_h_count*(const_emotion_inteval+const_emotion_width)-const_emotion_inteval;
	const int const_emotion_view_height = const_emotion_top_inteval*2+const_emotion_v_count*(const_emotion_inteval+const_emotion_height)-const_emotion_inteval;
	if (m_pEmotionViewControl.GetSafeHwnd())
	{
		m_pEmotionViewControl.MoveWindow(const_h_inteval,const_top_inteval,const_emotion_view_width+12,const_emotion_view_height,VARIANT_TRUE);
	}

}

void CDlgEmotionSelect::OnPaint()
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
		this->AddBgDrawInfo(CEbBackDrawInfo(theDefaultBgColorTitleHeight,theDefaultBgColorTitleLight0,false));
		this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor());
		Gdiplus::Graphics graphics(memDC.m_hDC);
		const FontFamily fontFamily(theFontFamily.c_str());
		const Gdiplus::Font fontEbTitle(&fontFamily, 13, FontStyleRegular, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
		//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
		graphics.DrawString(L"默认表情",-1,&fontEbTitle,PointF(13,13),&brushEbTitle);
	}
}

void CDlgEmotionSelect::OnKillFocus(CWnd* pNewWnd)
{
	CEbDialogBase::OnKillFocus(pNewWnd);
	if (pNewWnd != this && pNewWnd != &m_pEmotionViewControl)
	{
		CPoint point;
		::GetCursorPos(&point);
		CRect rect;
		this->GetWindowRect(&rect);
		if (!rect.PtInRect(point))
		{
			ShowWindow(SW_HIDE);
		}
	}
}

void CDlgEmotionSelect::OnDestroy()
{
	m_pEmotionViewControl.DestroyWindow();
	m_pImageDrawList.clear();
	CEbDialogBase::OnDestroy();

	// TODO: Add your message handler code here
}

LRESULT CDlgEmotionSelect::OnWmEVClickEmotion(WPARAM wParam, LPARAM lParam)
{
	DWORD dwUserData = wParam;
	for (size_t i=0; i<m_pImageDrawList.size(); i++)
	{
		CEBImageDrawInfo pImageDrawInfo = m_pImageDrawList[i];
		if (pImageDrawInfo.m_dwUserData==dwUserData)
		{
			if (m_pCallback)
				m_pCallback->OnSelectedImageInfo(pImageDrawInfo);
			this->ShowWindow(SW_HIDE);
			break;
		}
	}
	return 0;
}
LRESULT CDlgEmotionSelect::OnWmEVKillFocus(WPARAM wParam, LPARAM lParam)
{
	if (this->GetFocus() != this)
	{
		this->ShowWindow(SW_HIDE);
	}
	return 0;
}

