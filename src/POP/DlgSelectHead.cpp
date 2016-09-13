// DlgSelectHead.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgSelectHead.h"


// CDlgSelectHead dialog

IMPLEMENT_DYNAMIC(CDlgSelectHead, CEbDialogBase)

CDlgSelectHead::CDlgSelectHead(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgSelectHead::IDD, pParent)
	, m_pCallback(NULL)
{

}

CDlgSelectHead::~CDlgSelectHead()
{
}

void CDlgSelectHead::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
}


#define WM_EV_CLICK_EMOTION		(WM_USER+0x111)
BEGIN_MESSAGE_MAP(CDlgSelectHead, CEbDialogBase)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_MESSAGE(WM_EV_CLICK_EMOTION, OnWmEVClickEmotion)
END_MESSAGE_MAP()


// CDlgSelectHead message handlers

BOOL CDlgSelectHead::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	this->SetMouseMove(FALSE);

	if (!m_pEmotionViewControl.CreateControl(__uuidof(EBEmotionView),NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,10,10),this,1111))
		return FALSE;
	LPUNKNOWN pEbEmotionViewUnknown = m_pEmotionViewControl.GetControlUnknown();
	if (pEbEmotionViewUnknown == NULL)
		return FALSE;
	pEbEmotionViewUnknown->QueryInterface(__uuidof(IEBEmotionView),(void**)&m_pEbEmotionView);
	if (m_pEbEmotionView==NULL)
		return FALSE;
	m_pEbEmotionView->SetBackgroundColor(GetRValue(theDefaultFlatBgColor),GetGValue(theDefaultFlatBgColor),GetBValue(theDefaultFlatBgColor));
	m_pEbEmotionView->EmotionHeight = 60;
	m_pEbEmotionView->EmotionWidth = 60;
	m_pEbEmotionView->HCount = 5;
	m_pEbEmotionView->LeftInterval = 22;
	m_pEbEmotionView->TopInterval = 22;
	m_pEbEmotionView->LineInterval = 22;

	this->SetCircle(false);
	ShowImage();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectHead::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnCancel();
}

void CDlgSelectHead::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CEbDialogBase::OnOK();
}

void CDlgSelectHead::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		// TODO: Add your message handler code here
		// Do not call CEbDialogBase::OnPaint() for painting messages
	}else
	{
		CPaintDC dc2(this);
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc2, rectClient);
		this->ClearBgDrawInfo();
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
		//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
		this->DrawPopBg(&memDC, theApp.GetMainColor(),0);

		//for (size_t i=0; i<m_pImageDrawList.size(); i++)
		//{
		//	CEBImageDrawInfo pDrawInfo = m_pImageDrawList[i];
		//}
	}
}

void CDlgSelectHead::OnLButtonDown(UINT nFlags, CPoint point)
{
	// this->change_head
	this->GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	
	CEbDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgSelectHead::ShowImage(void)
{
#ifdef USES_EBCOM_TEST
	_variant_t pEnterpriseImageList = theEBClientCore->EB_GetDefaultHeadList();
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
			if (m_pEbEmotionView!=NULL)
				m_pEbEmotionView->AddEmotion(pEmotionInfo->ResFile,pEmotionInfo->Description,pDrawInfo.m_dwUserData);
		}
	}
#else
	USES_CONVERSION;
	std::vector<EB_EmotionInfo> pEnterpriseImageList;
	theEBAppClient.EB_GetDefaultHeadList(pEnterpriseImageList);
	for (size_t i=0; i<pEnterpriseImageList.size(); i++)
	{
		EB_EmotionInfo pEmotionInfo = pEnterpriseImageList[i];
		if (pEmotionInfo.m_sResFile.empty()) continue;
		if (!::PathFileExists(pEmotionInfo.m_sResFile.c_str())) continue;

		CEBImageDrawInfo pDrawInfo(pEmotionInfo);
		pDrawInfo.m_dwUserData = i+1;
		m_pImageDrawList.push_back(pDrawInfo);
		if (m_pEbEmotionView!=NULL)
			m_pEbEmotionView->AddEmotion(A2W_ACP(pEmotionInfo.m_sResFile.c_str()),A2W_ACP(pEmotionInfo.m_sDescription.c_str()),pDrawInfo.m_dwUserData);
	}
#endif
	m_pEmotionViewControl.ShowWindow(SW_SHOW);
	//this->ShowWindow(SW_SHOW);
}

void CDlgSelectHead::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	if (m_pEmotionViewControl.GetSafeHwnd())
	{
		m_pEmotionViewControl.MoveWindow(0,0,cx,cy);
	}
}

LRESULT CDlgSelectHead::OnWmEVClickEmotion(WPARAM wParam, LPARAM lParam)
{
	DWORD dwUserData = wParam;
	for (size_t i=0; i<m_pImageDrawList.size(); i++)
	{
		CEBImageDrawInfo pImageDrawInfo = m_pImageDrawList[i];
		if (pImageDrawInfo.m_dwUserData==dwUserData)
		{
			if (m_pCallback)
				m_pCallback->OnSelectedImageInfo(pImageDrawInfo);
			//this->ShowWindow(SW_HIDE);
			break;
		}
	}
	return 0;
}
