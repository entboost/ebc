// DlgIncomingCall.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgIncomingCall.h"


// CDlgIncomingCall dialog

IMPLEMENT_DYNAMIC(CDlgIncomingCall, CEbDialogBase)

CDlgIncomingCall::CDlgIncomingCall(CWnd* pParent /*=NULL*/, bool bDeleteThis)
	: CEbDialogBase(CDlgIncomingCall::IDD, pParent)
	//, m_bMulti(false)
	, m_bDeleteThis(bDeleteThis)
{

}

CDlgIncomingCall::~CDlgIncomingCall()
{
}

void CDlgIncomingCall::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_ACCEPT, m_btnAccept);
	DDX_Control(pDX, IDC_BUTTON_REJECT, m_btnReject);
}


BEGIN_MESSAGE_MAP(CDlgIncomingCall, CEbDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_ACCEPT, &CDlgIncomingCall::OnBnClickedButtonAccept)
	ON_BN_CLICKED(IDC_BUTTON_REJECT, &CDlgIncomingCall::OnBnClickedButtonReject)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgIncomingCall message handlers


void CDlgIncomingCall::OnBnClickedButtonAccept()
{
	// TODO: Add your control notification handler code here
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CallAnswer(m_pCallInfo.GetCallId(),VARIANT_TRUE);
#else
	theEBAppClient.EB_CallAnswer(m_pCallInfo.GetCallId(),true);
#endif
	//CEbDialogBase::OnOK();
}

void CDlgIncomingCall::OnBnClickedButtonReject()
{
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CallAnswer(m_pCallInfo.GetCallId(),VARIANT_FALSE);
#else
	theEBAppClient.EB_CallAnswer(m_pCallInfo.GetCallId(),false);
#endif
	//CEbDialogBase::OnCancel();
}

BOOL CDlgIncomingCall::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	m_btnAccept.SetToolTipText(_T("接受"));
	m_btnReject.SetToolTipText(_T("拒绝"));
	m_btnAccept.SetTextHotMove(false);
	m_btnReject.SetTextHotMove(false);
	m_btnAccept.SetDrawPanel(true,theApp.GetNormalColor(),theApp.GetHotColor(),theApp.GetPreColor());
	m_btnReject.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetNormalColor(),theApp.GetPreColor());

	//CString sWindowText;
	//if (m_bMulti)
	//	sWindowText.Format(_T("%s邀请你加入多人会话..."), m_sFromAccount.c_str());
	//else
	//	sWindowText.Format(_T("收到%s一个呼叫请求..."), m_sFromAccount.c_str());
	//this->SetWindowText(sWindowText);

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgIncomingCall::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
#ifdef USES_EBCOM_TEST
	theEBClientCore->EB_CallAnswer(m_pCallInfo.GetCallId(),VARIANT_FALSE);
#else
	theEBAppClient.EB_CallAnswer(m_pCallInfo.GetCallId(),false);
#endif
	//CEbDialogBase::OnCancel();
}

void CDlgIncomingCall::OnDestroy()
{
	CEbDialogBase::OnDestroy();

	// TODO: Add your message handler code here
	if (m_bDeleteThis)
		delete this;
}

void CDlgIncomingCall::OnPaint()
{
	if (!IsIconic())
	{
		//CEbDialogBase::OnPaint();
		DrawInfo();
	}
}
void CDlgIncomingCall::DrawInfo(void)
{
	//CPaintDC dc(this); // 用于绘制的设备上下文
	//this->DrawPopBg(&dc, theApp.GetMainColor());
	//HDC m_hdcMemory = dc.m_hDC;

	//// 写字
	//CFont pNewFontAd;
	//pNewFontAd.CreatePointFont(110, _T("宋体"));//创建显示文本的字体
	//HGDIOBJ pOldFond = SelectObject(m_hdcMemory, pNewFontAd.m_hObject);
	//::SetBkMode(m_hdcMemory, TRANSPARENT);
	//SetTextColor(m_hdcMemory, RGB(0, 0, 0));
	//int textx = 15;
	//int texty = 15;
	CPaintDC dc2(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	this->AddBgDrawInfo(CEbBackDrawInfo(85,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,85,2);
	//if (theApp.GetColorGradient())
	//	this->AddBgDrawInfo(CEbBackDrawInfo(85,0.7,true));
	//else
	//	this->AddBgDrawInfo(CEbBackDrawInfo(85,1.0,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(35,0.98,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(90,0.999,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,0.96,false));
	//this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);

	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	const Gdiplus::Font fontText(&fontFamily, 12, FontStyleRegular, UnitPixel);
	Gdiplus::SolidBrush brushEbText(Gdiplus::Color(32,32,32));
	if (m_pCallInfo.m_sGroupCode==0)
		graphics.DrawString(L"新会话请求来自",-1,&fontTitle,PointF(12,12),&brushEbTitle);
	else
		graphics.DrawString(L"多人会话请求来自",-1,&fontTitle,PointF(12,12),&brushEbTitle);
	//graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
	//const Gdiplus::PointF pointTitle(25,7);
	const Gdiplus::PointF pointTitle(103,43);

	const int const_image_x = 13;
	const int const_image_y = 40;
	const int const_image_size = 80;
	CString sOutText;
	Image * pImage = NULL;
	switch (m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType)
	{
	case EB_ACCOUNT_TYPE_VISITOR:
		pImage = theApp.m_imageDefaultVisitor->Clone();
		break;
	case EB_ACCOUNT_TYPE_IN_ENT:
		pImage = theApp.m_imageDefaultMember->Clone();
		break;
	default:
		pImage = theApp.m_imageDefaultContact->Clone();
		break;
	}
	graphics.DrawImage(pImage, const_image_x, const_image_y, const_image_size,const_image_size);
	delete pImage;
	if (m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType==EB_ACCOUNT_TYPE_VISITOR)
		sOutText.Format(_T("游客%s)"),m_pFromAccountInfo.GetAccount().c_str());
	else
		sOutText.Format(_T("%s(%s)"),m_pFromAccountInfo.m_pFromCardInfo.m_sName.c_str(),m_pFromAccountInfo.GetAccount().c_str());
	graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,pointTitle,&brushEbTitle);
	int x = pointTitle.X;
	int y = 65;
	//sOutText.Format(_T("%s"),m_pFromAccountInfo.m_pFromCardInfo.m_sDescription.c_str());
	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
	y += 30;
	if (m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_VISITOR)
		sOutText.Format(_T("地址：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sAddress.c_str());
	else if (m_pFromAccountInfo.m_pFromCardInfo.m_nAccountType == EB_ACCOUNT_TYPE_USER)
	{
		sOutText.Format(_T("群组：%s\t职位：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sGroupName.c_str(),m_pFromAccountInfo.m_pFromCardInfo.m_sTitle.c_str());
	}else
	{
		sOutText.Format(_T("部门：%s\t职位：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sGroupName.c_str(),m_pFromAccountInfo.m_pFromCardInfo.m_sTitle.c_str());
	}
	graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
	y += 30;
	x = 14;
	sOutText.Format(_T("公司：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sEnterprise.c_str());
	graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
	y += 30;
	sOutText.Format(_T("手机：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sPhone.c_str());
	graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
	x += 187;
	sOutText.Format(_T("邮箱：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sEmail.c_str());
	graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
	y += 30;
	x = 14;
	sOutText.Format(_T("电话：%s"), m_pFromAccountInfo.m_pFromCardInfo.m_sTel.c_str());
	graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
	//x += 187;
	//sOutText.Format(_T("传真：%s"), m_pFromAccountInfo.m_pFromCardInfo..m_sFax.c_str());
	//graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
}

void CDlgIncomingCall::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	const int const_btn_intever = 40;
	int x = (cx-theDefaultBtnWidth*2-const_btn_intever)/2;
	int y = cy-theDefaultBtnHeight-theDefaultBtnBottomIntever;		// 最下面间隔
	m_btnAccept.MovePoint(x,y,theDefaultBtnWidth,theDefaultBtnHeight);
	x += (theDefaultBtnWidth+const_btn_intever);
	m_btnReject.MovePoint(x,y,theDefaultBtnWidth,theDefaultBtnHeight);
}
