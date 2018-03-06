// DlgViewContactInfo.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "DlgViewContactInfo.h"
#include "Core/SkinMemDC.h"


// CDlgViewContactInfo dialog

IMPLEMENT_DYNAMIC(CDlgViewContactInfo, CEbDialogBase)

CDlgViewContactInfo::CDlgViewContactInfo(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgViewContactInfo::IDD, pParent)
{
	m_nViewType = VIEW_UNKNOWE;
}

CDlgViewContactInfo::~CDlgViewContactInfo()
{
}

void CDlgViewContactInfo::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgViewContactInfo, CEbDialogBase)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgViewContactInfo message handlers

BOOL CDlgViewContactInfo::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here

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
	if (theApp.GetColorGradient())
		this->AddBgDrawInfo(CEbBackDrawInfo(60,theDefaultBgColorTitleLight0,true));
	else
		this->AddBgDrawInfo(CEbBackDrawInfo(60,1.0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(30,theDefaultBgColorBgLight2,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(60,theDefaultBgColorBgLight0,false));
	this->AddBgDrawInfo(CEbBackDrawInfo(0,theDefaultBgColorBgLight2,false));
	this->DrawPopBg(&memDC, theApp.GetMainColor());
	Gdiplus::Graphics graphics(memDC.m_hDC);

	USES_CONVERSION;
	const FontFamily fontFamily(theFontFamily.c_str());
	const Gdiplus::Font fontTitle(&fontFamily, 13, FontStyleBold, UnitPixel);
	const Gdiplus::Font fontText(&fontFamily, 12, FontStyleRegular, UnitPixel);
	Gdiplus::Color colorTitle;
	colorTitle.SetFromCOLORREF(theApp.GetTitleTextColor());
	Gdiplus::SolidBrush brushEbTitle(colorTitle);
	//Gdiplus::SolidBrush brushEbTitle(Gdiplus::Color(38,38,38));
	//graphics.DrawImage(theApp.m_imageEbIcon, Gdiplus::RectF(3,3,20,20));
	//const Gdiplus::PointF pointTitle(25,7);
	const Gdiplus::PointF pointTitle(100,12);

	const int const_image_x = 10;
	const int const_image_y = 10;
	const int const_image_size = 80;
	CString sOutText;
	switch (m_nViewType)
	{
	case VIEW_CONTACT:
		{
			// 联系人（好友）
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
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			sOutText.Format(_T("%s\t%s"),m_pContactInfo.m_sCompany.c_str(),m_pContactInfo.m_sJobTitle.c_str());
			//sOutText.Format(_T("分组：%s"),m_pContactInfo.m_sGroup.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x = 14;
			y += 30;
			sOutText.Format(_T("手机：%s"),m_pContactInfo.m_sPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x += 148;
			sOutText.Format(_T("邮箱：%s"),m_pContactInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x = 14;
			y += 30;
			sOutText.Format(_T("电话：%s"),m_pContactInfo.m_sTel.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x += 148;
			sOutText.Format(_T("传真：%s"),m_pContactInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x = 14;
			y += 30;
			sOutText.Format(_T("地址：%s"),m_pContactInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
		}break;
	case VIEW_MEMBER:
		{
			// 群成员
			const tstring sHeadResourceFile = m_pMemberInfo.m_sHeadResourceFile;
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
			//sOutText.Format(_T("%s(%s)"),m_pMemberInfo.m_sUserName.c_str(),m_pMemberInfo.m_sMemberAccount.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,pointTitle,&brushEbTitle);
			int x = pointTitle.X;
			int y = 35;
			sOutText.Format(_T("%s"), m_pMemberInfo.m_sDescription.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			sOutText.Format(_T("%s\t%s"), m_pGroupInfo.m_sGroupName.c_str(),m_pMemberInfo.m_sJobTitle.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			
			// 公司名称
			y += 30;
			x = 14;
			sOutText.Format(_T("手机：%s"), m_pMemberInfo.m_sCellPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x += 148;
			sOutText.Format(_T("邮箱：%s"), m_pMemberInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			x = 14;
			sOutText.Format(_T("电话：%s"), m_pMemberInfo.m_sWorkPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x += 148;
			sOutText.Format(_T("传真：%s"), m_pMemberInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x = 14;
			y += 30;
			sOutText.Format(_T("地址：%s"), m_pMemberInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
		}break;
	case VIEW_GROUP:
		{
			Image * pImage = NULL;
			if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_DEPARTMENT)
			{
				pImage = theApp.m_imageDefaultDepartment->Clone();
				sOutText.Format(_T("%s - 部门:%lld"), m_pGroupInfo.m_sGroupName.c_str(),m_pGroupInfo.m_sGroupCode);
			}else if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_PROJECT)
			{
				pImage = theApp.m_imageDefaultProject->Clone();
				sOutText.Format(_T("%s - 项目组:%lld"), m_pGroupInfo.m_sGroupName.c_str(),m_pGroupInfo.m_sGroupCode);
			}else if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_GROUP)
			{
				pImage = theApp.m_imageDefaultGroup->Clone();
				sOutText.Format(_T("%s - 群组:%lld"), m_pGroupInfo.m_sGroupName.c_str(),m_pGroupInfo.m_sGroupCode);
			}else// if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_TEMP)
			{
				pImage = theApp.m_imageDefaultTempGroup->Clone();
				sOutText.Format(_T("%s - 讨论组"), m_pGroupInfo.m_sGroupName.c_str());
			}
			graphics.DrawImage(pImage, const_image_x, const_image_y, const_image_size,const_image_size);
			delete pImage;
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontTitle,pointTitle,&brushEbTitle);
			int x = pointTitle.X;
			int y = 35;
			sOutText.Format(_T("%s"), m_pGroupInfo.m_sDescription.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			if (m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_DEPARTMENT ||
				m_pGroupInfo.m_nGroupType==EB_GROUP_TYPE_PROJECT)
			{
				sOutText.Format(_T("%s"), m_pEnterpriseInfo.m_sEnterpriseName.c_str());
				graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			}
			y += 30;
			x = 14;
			sOutText.Format(_T("网站：%s"), m_pGroupInfo.m_sUrl.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x += 148;
			sOutText.Format(_T("邮箱：%s"), m_pGroupInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			x = 14;
			sOutText.Format(_T("电话：%s"), m_pGroupInfo.m_sPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x += 148;
			sOutText.Format(_T("传真：%s"), m_pGroupInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			x = 14;
			sOutText.Format(_T("地址：%s"), m_pGroupInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
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
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			sOutText.Format(_T("%s"), m_pEnterpriseInfo.m_sDescription.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x = 14;
			y += 30;
			sOutText.Format(_T("电话：%s"), m_pEnterpriseInfo.m_sPhone.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x += 148;
			sOutText.Format(_T("邮箱：%s"), m_pEnterpriseInfo.m_sEmail.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			x = 14;
			y += 30;
			sOutText.Format(_T("传真：%s"), m_pEnterpriseInfo.m_sFax.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
			y += 30;
			sOutText.Format(_T("地址：%s"), m_pEnterpriseInfo.m_sAddress.c_str());
			graphics.DrawString(A2W_ACP(sOutText),-1,&fontText,PointF(x,y),&brushEbTitle);
		}break;
	default:
		break;
	}

	//SelectObject(m_hdcMemory, pOldFond);
}

void CDlgViewContactInfo::HideReset(void)
{
	this->ShowWindow(SW_HIDE);
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
	m_nViewType = VIEW_GROUP;
	m_pGroupInfo = pGroupInfo;
	m_pEnterpriseInfo = pEnterpriseInfo;
	this->Invalidate();
}
#else
void CDlgViewContactInfo::SetDepartemntInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo)
{
	m_nViewType = VIEW_GROUP;
	m_pGroupInfo = pGroupInfo;
	m_pEnterpriseInfo = pEnterpriseInfo;
	this->Invalidate();
}
#endif

#ifdef USES_EBCOM_TEST
void CDlgViewContactInfo::SetEmployeeInfo(IEB_MemberInfo* pMemberInfo,IEB_GroupInfo* pGroupInfo)
{
	m_nViewType = VIEW_MEMBER;
	m_pGroupInfo = pGroupInfo;
	m_pMemberInfo = pMemberInfo;
	this->Invalidate();
}
#else
void CDlgViewContactInfo::SetEmployeeInfo(const EB_MemberInfo* pMemberInfo,const EB_GroupInfo* pGroupInfo)
{
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
	m_nViewType = VIEW_CONTACT;
	m_pContactInfo = pContactInfo;
	this->Invalidate();
}
#endif
