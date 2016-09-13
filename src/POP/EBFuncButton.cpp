#include "StdAfx.h"
#include "EBFuncButton.h"

CEBFuncButton::CEBFuncButton(void)
{
}

CEBFuncButton::~CEBFuncButton(void)
{
	if (m_btn.GetSafeHwnd()!=NULL)
		m_btn.DestroyWindow();
}

bool CEBFuncButton::Create(CWnd* pParentWnd,const RECT& rect,UINT nID,const EB_SubscribeFuncInfo& pFuncInfo,const tstring& sDefaultImage)
{
	if (m_btn.GetSafeHwnd()==NULL)
	{
		if (!m_btn.Create(_T(""),WS_CHILD|WS_VISIBLE,rect,pParentWnd,nID))
			return false;
		m_pFuncInfo = pFuncInfo;
		const tstring sButtunImage(pFuncInfo.m_sResFile.empty()?sDefaultImage:pFuncInfo.m_sResFile);
		if (sButtunImage.empty())
			return false;
		CImage orgImg;
		if (orgImg.Load(sButtunImage.c_str())!=S_OK)
		{
			return false;
		}
		if (orgImg.GetWidth()>(orgImg.GetHeight()*2))
		{
			// 恩布默认风格图片
			m_btn.SetAutoSize(false);
			m_btn.Load(sButtunImage.c_str());
		}else
		{
			// 用户自定义图片
			m_btn.SetAutoSize(true);
			m_btn.SetDrawPanelRgn(false);
			m_btn.Load(sButtunImage.c_str(),orgImg.GetWidth(),orgImg.GetHeight());
			m_btn.SetDrawLine(5,1,0,-1,theDefaultButtonColor);
			//m_btn.SetDrawLine(5,1,0,-1,theDefaultFlatLine2Color);
		}
		m_btn.SetToolTipText(pFuncInfo.m_sFunctionName.c_str());
	}
	return true;
}

void CEBFuncButton::UpdateResFile(const tstring& sResFile)
{
	if (m_btn.GetSafeHwnd()!=NULL && PathFileExists(sResFile.c_str()))
	{
		if (m_pFuncInfo.m_sResFile!=sResFile)
		{
			m_pFuncInfo.m_sResFile = sResFile;
			m_btn.Load(sResFile.c_str());
			m_btn.Invalidate();
		}
	}
}
