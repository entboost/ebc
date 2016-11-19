// DlgViewContactInfo.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgViewContactInfo.h"
#include "Core/SkinMemDC.h"

#define TIMER_CHECK_LEAVE	101
#define TIMER_MOVE_ENTER	102
#define TIMER_MOVE_LEAVE	103


// CDlgViewContactInfo dialog

IMPLEMENT_DYNAMIC(CDlgViewContactInfo, CEbDialogBase)

CDlgViewContactInfo::CDlgViewContactInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgViewContactInfo::IDD, pParent)
{
	m_nViewType = VIEW_UNKNOWE;
	m_bFirstMoveEnter = false;
	m_bFirstInRect = false;
}

CDlgViewContactInfo::~CDlgViewContactInfo()
{
}

void CDlgViewContactInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_LABEL1, m_staLabel1);
}


BEGIN_MESSAGE_MAP(CDlgViewContactInfo, CEbDialogBase)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgViewContactInfo message handlers

BOOL CDlgViewContactInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	//m_staLabel1.ShowWindow(SW_HIDE);
	//m_staLabel1.SetBkColor(RGB(0,0,0));
	//m_staLabel1.SetTextColor(RGB(255,255,255));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgViewContactInfo::OnPaint()
{
	if (!IsIconic())
	{
		DrawInfo();
	}
}

void CDlgViewContactInfo::DrawInfo(void)
{
	CPaintDC dc2(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc2, rectClient);
	this->ClearBgDrawInfo();
	this->AddBgDrawInfo(CEbBackDrawInfo(60,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultFlatBgColor));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor(), 1);
	//this->DrawPopBg(&memDC, theApp.GetMainColor(), 1,60,2);
	//if (theApp.GetColorGradient())
	//	this->AddBgDrawInfo(CEbBackDrawInfo(60,theDefaultBgColorTitleLight0,true));
	//else
	//	this->AddBgDrawInfo(CEbBackDrawInfo(60,1.0,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(29,theDefaultBgColorBgLight1,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(1,theDefaultBgColorBgLight3,false));
	//this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight1,false));
	//this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);

	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontTitle(&fontFamily, 14, FontStyleBold, UnitPixel);
	const Gdiplus::Font fontText(&fontFamily, 12, FontStyleRegular, UnitPixel);
	Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(255,255,255));
	Gdiplus::SolidBrush brushEbText(Gdiplus::Color(32,32,32));
	Gdiplus::SolidBrush brushLabelBg(Gdiplus::Color(163,73,164));
	//Gdiplus::SolidBrush brushLabelBg(Gdiplus::Color(237,28,36));
	Gdiplus::SolidBrush brushLabelText(Gdiplus::Color(255,255,255));
	//graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
	//const Gdiplus::PointF pointTitle(25,7);
	const Gdiplus::PointF pointTitle(75,12);

	const int const_image_x = 8;
	const int const_image_y = 8;
	const int const_image_size = 60;
	//m_staLabel1.ShowWindow(SW_HIDE);
	CString sOutText;
	switch (m_nViewType)
	{
	case VIEW_CONTACT:
		{
			// 联系人（好友）
			if ((m_pContactInfo.m_nContactType&EB_CONTACT_TYPE_MAIL)==EB_CONTACT_TYPE_MAIL)
			{
				const int nLabel1Width = 65;
				Gdiplus::RectF rectLabel1(rectClient.Width()-nLabel1Width-3,1,nLabel1Width,17);
				graphics.FillRectangle(&brushLabelBg,rectLabel1);
				graphics.DrawString(L"邮件联系人",-1,&fontText,PointF(rectLabel1.X,rectLabel1.Y),&brushLabelText);
			}

			CEBString sImagePath;
			bool bIsMemberAccount = false;
			EB_USER_LINE_STATE pOutLineState = EB_LINE_STATE_UNKNOWN;
#ifdef USES_EBCOM_TEST
			CComPtr<IEB_MemberInfo> pMemberInfo;
			if (m_pContactInfo.m_nContactUserId>0)
				pMemberInfo = theEBClientCore->EB_GetMemberInfoByUserId2(m_pContactInfo.m_nContactUserId);
			if (pMemberInfo == NULL)
				pMemberInfo = theEBClientCore->EB_GetMemberInfoByAccount2(m_pContactInfo.m_sContact.c_str());
			if (pMemberInfo != NULL)
			{
				bIsMemberAccount = true;
				pOutLineState = (EB_USER_LINE_STATE)pMemberInfo->LineState;
				const CEBString sMemberHeadFile = pMemberInfo->HeadResourceFile.GetBSTR();
				if (PathFileExists(sMemberHeadFile.c_str()))
				{
					sImagePath = sMemberHeadFile;
				}
			}
#else
			EB_MemberInfo pMemberInfo;
			bool bFind = false;
			if (m_pContactInfo.m_nContactUserId>0)
				bFind = theEBAppClient.EB_GetMemberInfoByUserId2(&pMemberInfo,m_pContactInfo.m_nContactUserId);
			if (!bFind)
				bFind = theEBAppClient.EB_GetMemberInfoByAccount2(&pMemberInfo,m_pContactInfo.m_sContact.c_str());
			if (bFind)
			{
				bIsMemberAccount = true;
				pOutLineState = pMemberInfo.m_nLineState;
				if (PathFileExists(pMemberInfo.m_sHeadResourceFile.c_str()))
				{
					sImagePath = pMemberInfo.m_sHeadResourceFile;
				}
			}
#endif
			Image * pImage = NULL;
			if (bIsMemberAccount)
			{
				if (!sImagePath.empty())
				{
					USES_CONVERSION;
					pImage = new Gdiplus::Image((const WCHAR*)A2W_ACP(sImagePath.c_str()));
				}else
				{
					pImage = theApp.m_imageDefaultMember->Clone();
				}
			}else
			{
				pImage = theApp.m_imageDefaultContact->Clone();
			}
			graphics.DrawImage(pImage, const_image_x, const_image_y, const_image_size,const_image_size);
			delete pImage;
			if (m_pContactInfo.m_nContactUserId>0)
				sOutText.Format(_T("%s(%lld)"),m_pContactInfo.m_sName.c_str(),m_pContactInfo.m_nContactUserId);
			else
				sOutText.Format(_T("%s(%s)"),m_pContactInfo.m_sName.c_str(),m_pContactInfo.m_sContact.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,pointTitle,&brushEbTitle);
			int x = pointTitle.X;
			int y = 35;
			sOutText.Format(_T("%s"),m_pContactInfo.m_sDescription.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			sOutText.Format(_T("%s\t%s"),m_pContactInfo.m_sCompany.c_str(),m_pContactInfo.m_sJobTitle.c_str());
			//sOutText.Format(_T("分组：%s"),m_pContactInfo.m_sGroup.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x = 14;
			y += 30;
			sOutText.Format(_T("手机：%s"),m_pContactInfo.m_sPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x += 148;
			sOutText.Format(_T("邮箱：%s"),m_pContactInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x = 14;
			y += 30;
			sOutText.Format(_T("电话：%s"),m_pContactInfo.m_sTel.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x += 148;
			sOutText.Format(_T("传真：%s"),m_pContactInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x = 14;
			y += 30;
			sOutText.Format(_T("地址：%s"),m_pContactInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
		}break;
	case VIEW_MEMBER:
		{
			// 群成员
			if (m_pGroupInfo.m_nManagerUserId==m_pMemberInfo.m_nMemberUserId)
			{
				const CString sGroupManagerText = GetGroupManagerText(m_pGroupInfo.m_nGroupType);
				const int nLabel1Width = m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_GROUP?28:52;
				Gdiplus::RectF rectLabel1(rectClient.Width()-nLabel1Width-3,1,nLabel1Width,17);
				graphics.FillRectangle(&brushLabelBg,rectLabel1);
				graphics.DrawString(A2W_ACP(sGroupManagerText),-1,&fontText,PointF(rectLabel1.X,rectLabel1.Y),&brushLabelText);
			}

			const tstring sHeadResourceFile(m_pMemberInfo.m_sHeadResourceFile);
			if (!sHeadResourceFile.empty() && PathFileExists(sHeadResourceFile.c_str()))
			{
				Gdiplus::Image imageHead((const WCHAR*)A2W_ACP(sHeadResourceFile.c_str()));
				graphics.DrawImage(&imageHead, const_image_x, const_image_y, const_image_size,const_image_size);
			}else
			{
				Image * pImage = theApp.m_imageDefaultMember->Clone();
				graphics.DrawImage(pImage, const_image_x, const_image_y, const_image_size,const_image_size);
				delete pImage;
			}
			sOutText.Format(_T("%s(%lld)"),m_pMemberInfo.m_sUserName.c_str(),m_pMemberInfo.m_nMemberUserId);
			//if (m_pGroupInfo.m_nManagerUserId==m_pMemberInfo.m_nMemberUserId)
			//	sOutText.Format(_T("%s(%lld) %s"),m_pMemberInfo.m_sUserName.c_str(),m_pMemberInfo.m_nMemberUserId,GetGroupManagerText(m_pGroupInfo.m_nGroupType));
			//else
			//	sOutText.Format(_T("%s(%lld)"),m_pMemberInfo.m_sUserName.c_str(),m_pMemberInfo.m_nMemberUserId);
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,pointTitle,&brushEbTitle);
			int x = pointTitle.X;
			int y = 35;
			sOutText.Format(_T("%s"), m_pMemberInfo.m_sDescription.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			sOutText.Format(_T("%s\t%s"), m_pGroupInfo.m_sGroupName.c_str(),m_pMemberInfo.m_sJobTitle.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			
			// 公司名称
			y += 30;
			x = 14;
			sOutText.Format(_T("手机：%s"), m_pMemberInfo.m_sCellPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x += 148;
			sOutText.Format(_T("邮箱：%s"), m_pMemberInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			x = 14;
			sOutText.Format(_T("电话：%s"), m_pMemberInfo.m_sWorkPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x += 148;
			sOutText.Format(_T("传真：%s"), m_pMemberInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x = 14;
			y += 30;
			sOutText.Format(_T("地址：%s"), m_pMemberInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
		}break;
	case VIEW_GROUP:
		{
			const CString sGroupTypeString = GetGroupTypeText(m_pGroupInfo.m_nGroupType,true);
			const int nLabel1Width = m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_TEMP?40:28;
			Gdiplus::RectF rectLabel1(rectClient.Width()-nLabel1Width-3,1,nLabel1Width,17);
			graphics.FillRectangle(&brushLabelBg,rectLabel1);
			graphics.DrawString(A2W_ACP(sGroupTypeString),-1,&fontText,PointF(rectLabel1.X,rectLabel1.Y),&brushLabelText);
			//m_staLabel1.SetWindowText(sGroupTypeString);
			//m_staLabel1.ShowWindow(SW_SHOW);
			//m_staLabel1.Invalidate();

			Image * pImage = NULL;
			if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_DEPARTMENT)
			{
				pImage = theApp.m_imageDefaultDepartment->Clone();
				sOutText.Format(_T("%s(%lld)"), m_pGroupInfo.m_sGroupName.c_str(),m_pGroupInfo.m_sGroupCode);
			}else if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_PROJECT)
			{
				pImage = theApp.m_imageDefaultProject->Clone();
				sOutText.Format(_T("%s(%lld)"), m_pGroupInfo.m_sGroupName.c_str(),m_pGroupInfo.m_sGroupCode);
			}else if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_GROUP)
			{
				pImage = theApp.m_imageDefaultGroup->Clone();
				sOutText.Format(_T("%s(%lld)"), m_pGroupInfo.m_sGroupName.c_str(),m_pGroupInfo.m_sGroupCode);
			}else// if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_TEMP)
			{
				pImage = theApp.m_imageDefaultTempGroup->Clone();
				sOutText.Format(_T("%s"), m_pGroupInfo.m_sGroupName.c_str());
			}
			graphics.DrawImage(pImage, const_image_x, const_image_y, const_image_size,const_image_size);
			delete pImage;
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,pointTitle,&brushEbTitle);
			int x = pointTitle.X;
			int y = 35;
			sOutText.Format(_T("%s"), m_pGroupInfo.m_sDescription.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_DEPARTMENT ||
				m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_PROJECT)
			{
				sOutText.Format(_T("%s"), m_pEnterpriseInfo.m_sEnterpriseName.c_str());
				graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			}
			y += 30;
			x = 14;
			sOutText.Format(_T("网站：%s"), m_pGroupInfo.m_sUrl.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x += 148;
			sOutText.Format(_T("邮箱：%s"), m_pGroupInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			x = 14;
			sOutText.Format(_T("电话：%s"), m_pGroupInfo.m_sPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x += 148;
			sOutText.Format(_T("传真：%s"), m_pGroupInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			x = 14;
			sOutText.Format(_T("地址：%s"), m_pGroupInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
		}break;
	case VIEW_ENTERPRISE:
		{
			Image * pImage = theApp.m_imageDefaultOrg->Clone();;
			graphics.DrawImage(pImage, const_image_x, const_image_y, const_image_size,const_image_size);
			delete pImage;
			sOutText.Format(_T("%s"), m_pEnterpriseInfo.m_sEnterpriseName.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,pointTitle,&brushEbTitle);
			int x = pointTitle.X;
			int y = 35;
			sOutText.Format(_T("%s"), m_pEnterpriseInfo.m_sUrl.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			sOutText.Format(_T("%s"), m_pEnterpriseInfo.m_sDescription.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x = 14;
			y += 30;
			sOutText.Format(_T("电话：%s"), m_pEnterpriseInfo.m_sPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x += 148;
			sOutText.Format(_T("邮箱：%s"), m_pEnterpriseInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			x = 14;
			y += 30;
			sOutText.Format(_T("传真：%s"), m_pEnterpriseInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
			y += 30;
			sOutText.Format(_T("地址：%s"), m_pEnterpriseInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbText);
		}break;
	default:
		break;
	}

	//SelectObject(m_hdcMemory, pOldFond);
}

void CDlgViewContactInfo::HideReset(void)
{
	KillTimer(TIMER_MOVE_ENTER);
	KillTimer(TIMER_MOVE_LEAVE);
	KillTimer(TIMER_CHECK_LEAVE);
	this->ShowWindow(SW_HIDE);
	m_bFirstMoveEnter = false;
	m_bFirstInRect = false;
}

void CDlgViewContactInfo::SetCheckLeave(void)
{
	KillTimer(TIMER_MOVE_ENTER);
	KillTimer(TIMER_MOVE_LEAVE);
	KillTimer(TIMER_CHECK_LEAVE);
	SetTimer(TIMER_CHECK_LEAVE,100,NULL);
}
void CDlgViewContactInfo::SetMoveEnter(void)
{
	if (!m_bFirstMoveEnter)
	{
		m_bFirstMoveEnter = true;
		KillTimer(TIMER_MOVE_ENTER);
		if (m_nViewType==VIEW_MEMBER)
			SetTimer(TIMER_MOVE_ENTER,1000,NULL);
		else
			SetTimer(TIMER_MOVE_ENTER,2500,NULL);
	}
}
void CDlgViewContactInfo::SetMoveLeave(void)
{
	if (m_bFirstMoveEnter)
	{
		m_bFirstMoveEnter = false;
		KillTimer(TIMER_MOVE_ENTER);
	}
	if (!m_bFirstInRect)
	{
		KillTimer(TIMER_MOVE_LEAVE);
		SetTimer(TIMER_MOVE_LEAVE,600,NULL);
	}
}

#ifdef USES_EBCOM_TEST
void CDlgViewContactInfo::SetEnterpriseInfo(IEB_EnterpriseInfo* pEnterpriseInfo)
{
	m_nViewType = VIEW_ENTERPRISE;
	m_pEnterpriseInfo = pEnterpriseInfo;
	this->Invalidate();
}
#else
void CDlgViewContactInfo::SetEnterpriseInfo(const EB_EnterpriseInfo* pEnterpriseInfo)
{
	m_nViewType = VIEW_ENTERPRISE;
	m_pEnterpriseInfo = pEnterpriseInfo;
	this->Invalidate();
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgViewContactInfo::SetDepartemntInfo(IEB_GroupInfo* pGroupInfo,IEB_EnterpriseInfo* pEnterpriseInfo)
{
	//if (m_bFirstMoveEnter)
	//{
	//	// * 切换
	//	m_bFirstMoveEnter = (m_nViewType==VIEW_GROUP && m_pGroupInfo.m_sGroupCode==pGroupInfo->m_sGroupCode)?true:false;
	//}
	m_nViewType = VIEW_GROUP;
	m_pGroupInfo = pGroupInfo;
	m_pEnterpriseInfo = pEnterpriseInfo;
	this->Invalidate();
}
#else
void CDlgViewContactInfo::SetDepartemntInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo)
{
	if (m_bFirstMoveEnter)
	{
		// * 切换
		if (m_nViewType!=VIEW_GROUP || m_pGroupInfo.m_sGroupCode!=pGroupInfo->m_sGroupCode)
		{
			m_bFirstMoveEnter = false;
			KillTimer(TIMER_MOVE_ENTER);
			//this->ShowWindow(SW_HIDE);
		}
	}
	m_nViewType = VIEW_GROUP;
	m_pGroupInfo = pGroupInfo;
	m_pEnterpriseInfo = pEnterpriseInfo;
	this->Invalidate();
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgViewContactInfo::SetEmployeeInfo(IEB_MemberInfo* pMemberInfo,IEB_GroupInfo* pGroupInfo)
{
	//if (m_bFirstMoveEnter)
	//{
	//	// * 切换
	//	m_bFirstMoveEnter = (m_nViewType==VIEW_MEMBER && m_pMemberInfo.m_sMemberCode==pMemberInfo->m_sMemberCode)?true:false;
	//}
	m_nViewType = VIEW_MEMBER;
	m_pGroupInfo = pGroupInfo;
	m_pMemberInfo = pMemberInfo;
	this->Invalidate();
}
#else
void CDlgViewContactInfo::SetEmployeeInfo(const EB_MemberInfo* pMemberInfo,const EB_GroupInfo* pGroupInfo)
{
	if (m_bFirstMoveEnter)
	{
		// * 切换
		if (m_nViewType!=VIEW_MEMBER || m_pMemberInfo.m_sMemberCode!=pMemberInfo->m_sMemberCode)
		{
			m_bFirstMoveEnter = false;
			KillTimer(TIMER_MOVE_ENTER);
			//this->ShowWindow(SW_HIDE);
		}
	}
	m_nViewType = VIEW_MEMBER;
	m_pGroupInfo = pGroupInfo;
	m_pMemberInfo = pMemberInfo;
	this->Invalidate();
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgViewContactInfo::SetContactInfo(IEB_ContactInfo* pContactInfo)
{
	m_nViewType = VIEW_CONTACT;
	m_pContactInfo = pContactInfo;
	this->Invalidate();
}
#else
void CDlgViewContactInfo::SetContactInfo(const EB_ContactInfo* pContactInfo)
{
	if (m_bFirstMoveEnter)
	{
		// * 切换
		if (m_nViewType!=VIEW_CONTACT || m_pContactInfo.m_nContactId!=pContactInfo->m_nContactId)
		{
			m_bFirstMoveEnter = false;
			KillTimer(TIMER_MOVE_ENTER);
			//this->ShowWindow(SW_HIDE);
		}
	}
	m_nViewType = VIEW_CONTACT;
	m_pContactInfo = pContactInfo;
	this->Invalidate();
}
#endif

void CDlgViewContactInfo::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_MOVE_ENTER:
		{
			//KillTimer(TIMER_MOVE_ENTER);
			this->ShowWindow(SW_SHOW);
			SetCheckLeave();
		}break;
	case TIMER_MOVE_LEAVE:
		{
			if (!m_bFirstInRect)
			{
				this->HideReset();
			}else
			{
				KillTimer(TIMER_MOVE_LEAVE);
			}
		}break;
	case TIMER_CHECK_LEAVE:
		{
			CPoint pos;
			::GetCursorPos(&pos);
			CRect rect;
			this->GetWindowRect(&rect);
			if (rect.PtInRect(pos))
			{
				if (!m_bFirstInRect)
				{
					m_bFirstInRect = true;
					KillTimer(TIMER_CHECK_LEAVE);
					SetTimer(TIMER_CHECK_LEAVE,600,NULL);
				}
			}else
			{
				if (m_bFirstInRect)
				{
					this->HideReset();
				}
			}
		}break;
	default:
		break;
	}

	CEbDialogBase::OnTimer(nIDEvent);
}

void CDlgViewContactInfo::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	//if (m_staLabel1.GetSafeHwnd()!=NULL)
	//{
	//	const int const_width = 30;
	//	const int const_height = 22;
	//	m_staLabel1.MoveWindow(cx-const_width-1,1,const_width,const_height);
	//}
}
