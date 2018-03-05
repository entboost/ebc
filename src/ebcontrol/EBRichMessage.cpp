// EBRichMessage.cpp : Implementation of CEBRichMessage
#include "stdafx.h"
#include "EBRichMessage.h"
#include "atlstr.h"
#include "comutil.h"
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
//#include "../include/ebstring.h"

std::wstring theFontFamily = L"Times New Roman";

const int const_line_interval = 150;

STDMETHODIMP CEBRichMessage::GetScrollMax(LONG* pOutMax)
{
	*pOutMax = m_nScrollMax;
	return S_OK;
}


STDMETHODIMP CEBRichMessage::GetScrollMin(LONG* pOutMin)
{
	*pOutMin = const_line_interval;
	return S_OK;
}

STDMETHODIMP CEBRichMessage::GetScrollPos(LONG* pOutPos)
{
	*pOutPos = m_nScrollPos;
	return S_OK;
}

STDMETHODIMP CEBRichMessage::ScrollPageUp(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == 0)
		return S_FALSE;
	CRect rect;
	this->GetClientRect(&rect);
	int nPage = rect.Height();
	m_nScrollPos -= (nPage-1);
	if (m_nScrollPos < 0)
	{
		m_nScrollPos = 0;
	}
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBRichMessage::ScrollPageDown(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	CRect rect;
	this->GetClientRect(&rect);
	int nPage = rect.Height();
	m_nScrollPos += (nPage-1);
	if (m_nScrollPos > m_nScrollMax)
		m_nScrollPos = m_nScrollMax;
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBRichMessage::ScrollLineUp(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == 0)
		return S_FALSE;
	m_nScrollPos -= const_line_interval;
	if (m_nScrollPos < 0)
	{
		m_nScrollPos = 0;
	}
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBRichMessage::ScrollLineDown(void)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	m_nScrollPos += const_line_interval;
	if (m_nScrollPos > m_nScrollMax)
		m_nScrollPos = m_nScrollMax;
	this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBRichMessage::ScrollBy(LONG nScrollTimes)
{
	LONG nRealScrollTimes = 0;
	return ScrollBy(nScrollTimes,nRealScrollTimes);
}
HRESULT CEBRichMessage::ScrollBy(LONG nScrollTimes, LONG& pOutScrollTimes)
{
	if (nScrollTimes > 0)
	{
		// 向下拉动
		if (m_nScrollPos == m_nScrollMax)
			return S_FALSE;
		pOutScrollTimes = nScrollTimes;
		m_nScrollPos += nScrollTimes;
		if (m_nScrollPos > m_nScrollMax)
		{
			pOutScrollTimes -= (m_nScrollPos-m_nScrollMax);
			m_nScrollPos = m_nScrollMax;
		}
		this->FireViewChange();
	}else if (nScrollTimes < 0)
	{
		// 向上拉动
		if (m_nScrollPos == 0)
			return S_FALSE;
		pOutScrollTimes = nScrollTimes;
		m_nScrollPos += nScrollTimes;
		//m_nScrollPos -= (nScrollTimes*-1);
		if (m_nScrollPos < 0)
		{
			pOutScrollTimes -= m_nScrollPos;
			m_nScrollPos = 0;
		}
		this->FireViewChange();
	}
	return S_OK;
}

STDMETHODIMP CEBRichMessage::ScrollTo(LONG nScrollPos)
{
	// TODO: Add your implementation code here
	if (m_nScrollPos == nScrollPos)
		return S_FALSE;
	else if (nScrollPos < 0 && m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	else if (nScrollPos > m_nScrollMax && m_nScrollPos == m_nScrollMax)
		return S_FALSE;
	m_nScrollPos = nScrollPos;
	if (m_nScrollPos < 0 || m_nScrollPos > m_nScrollMax)
		m_nScrollPos = m_nScrollMax;
	this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::OnVBarKillFocus(void)
{
	::PostMessage(::GetParent(this->m_hWnd),WM_KILLFOCUS,(WPARAM)::GetFocus(),0);
	return S_OK;
}

void CEBRichMessage::PositionScrollBars(void)
{
	CRect windowRect;
	this->GetClientRect(&windowRect);
	CRect vBar(	windowRect.right-const_vbar_width,
				windowRect.top,
				windowRect.right,
				windowRect.bottom);
	if ( m_pSkinVBar.m_hWnd != NULL)
	{
		m_pSkinVBar.SetWindowPos(NULL,vBar.left,vBar.top,vBar.Width(),vBar.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
		if (m_pSkinVBarInterface != NULL)
			m_pSkinVBarInterface->UpdatePosition();
	}
}
void CEBRichMessage::OnSize(int cx, int cy)
{
	int nOldMax = m_nScrollMax;
	int noffset = m_nScrollMax>0?const_vbar_width:0;
	int y = 10;
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			pLine->OnSize(y, cx-noffset, cy);
			y += (pLine->GetRectF().Height+m_nLineInterval);
		}
	}
	int m_nMaxHeight = y;
	m_nScrollMax = std::max<int>(0,m_nMaxHeight-cy);
	if (m_nScrollPos > m_nScrollMax)
	{
		m_nScrollPos = m_nScrollMax;
	}
	if (!m_bReOnSize &&
		((nOldMax == 0 && m_nScrollMax > 0) ||
		(nOldMax > 0 && m_nScrollMax == 0)))
	{
		// 重新根据滚动条，计算宽度
		m_bReOnSize = true;
		OnSize(cx,cy);
		return;
	}
	m_bReOnSize = false;
	if (m_nScrollMax > 0 && !m_pSkinVBar.IsWindowVisible())
	{
		m_pSkinVBar.ShowWindow(SW_SHOW);
	}else if (m_nScrollMax == 0 && m_pSkinVBar.IsWindowVisible())
	{
		m_pSkinVBar.ShowWindow(SW_HIDE);
	}
}

// CEBRichMessage
STDMETHODIMP CEBRichMessage::SetLineInterval(SHORT nLineInterval)
{
	m_nLineInterval = nLineInterval;
	return S_OK;
}
STDMETHODIMP CEBRichMessage::SetBackgroundColor(ULONG colorRef)
{
	Gdiplus::Color pColor;
	pColor.SetFromCOLORREF(colorRef);
	m_brushBackground.SetColor(pColor);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::SetSelectedColor(ULONG colorRef)
{
	m_colorSelected.SetFromCOLORREF(colorRef);
	//m_colorSelected = Gdiplus::Color(r,g,b);
	return S_OK;
}

STDMETHODIMP CEBRichMessage::SetAlignmentFormat(SHORT nAlignmentFormat)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	m_pCurrentLine->SetAlignment((MR_ALIGNMENT)nAlignmentFormat);
	return S_OK;
}

STDMETHODIMP CEBRichMessage::SetAlignmentValue(SHORT nAlignment1, SHORT nAlignment2)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	m_pCurrentLine->SetAlignmentValue(nAlignment1,nAlignment2);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::UpdateSize(VARIANT_BOOL bRepaint)
{
	CRect rect;
	this->GetClientRect(&rect);
	OnSize(rect.Width(),rect.Height());
	if (bRepaint == VARIANT_TRUE)
		this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::ScrollToPos(LONG nScrollPos)
{
	//CRect rect;
	//GetClientRect(&rect);
	//this->OnSize(rect.Width(),rect.Height());
	//if (m_pScrollInfo.nMax <= m_pScrollInfo.nPage)
	//	return;

	if (nScrollPos <= -1)
	{
		// 移到最后
		if (m_nScrollPos == m_nScrollMax)
		{
			//this->Invalidate();
			return S_OK;
		}
		m_nScrollPos = m_nScrollMax;
	}else
	{
		if (m_nScrollPos == nScrollPos)
		{
			return S_OK;
		}
		m_nScrollPos = nScrollPos;
		if (m_nScrollPos > m_nScrollMax)
			m_nScrollPos = m_nScrollMax;
		else if (m_nScrollPos < 0)
			m_nScrollPos = 0;
	}
	this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::IsScrollBegin(VARIANT_BOOL * pVal)
{
	*pVal = (m_nScrollPos==0)?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CEBRichMessage::IsScrollEnd(VARIANT_BOOL * pVal)
{
	*pVal = (m_nScrollPos==m_nScrollMax)?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEBRichMessage::Clear(void)
{
	bool bNeedFireViewChange = !m_pList.empty();
	m_pList.clear();
	m_pCurrentLine.reset();
	if (bNeedFireViewChange)
	{
		this->FireViewChange();
		//m_pSkinVBar.Invalidate();
	}
	return S_OK;
}

STDMETHODIMP CEBRichMessage::AddLine(LONGLONG nMsgId)
{
	const UINT nClassicMode = GetPrivateProfileIntA("ebrichmessage","classic-mode",0,m_sRichSettingPath.c_str());
	m_pCurrentLine = CMrLine::create(this->m_hWnd,nMsgId);
	m_pCurrentLine->SetLineId(this->GetNextLineId());
	m_pCurrentLine->SetSelectedBkgnd(this->m_colorSelected);
	m_pCurrentLine->SetClassicMode(nClassicMode==1?true:false);
	m_pCurrentLine->SetShowReceiptFlag(m_bDisableReceipt?false:true);
	m_pList.add(m_pCurrentLine);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::DeleteLine(LONGLONG nMsgId)
{
	HRESULT result = S_FALSE;
	BoostWriteLock wtlock(m_pList.mutex());
	CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
	for (;pIter!=m_pList.end();)
	{
		const CMrLine::pointer& pLine = *pIter;
		if (pLine->GetMsgId()==nMsgId)
		{
			CLockList<CMrLine::pointer>::iterator pPrevIter = pIter;
			const bool bIsBegin = pPrevIter==m_pList.begin()?true:false;
			if (!bIsBegin)
			{
				pPrevIter--;
			}
			m_pList.erase(pIter);
			if (m_pList.empty())
			{
				return S_OK;
			}
			pIter = bIsBegin?m_pList.begin():(++pPrevIter);
			//pIter = m_pList.begin();
			result = S_OK;
			continue;
		}else if (result == S_OK)	// 已经删除所有，直接跳出
		{
			break;
		}
		pIter++;
	}
	return result;
}
STDMETHODIMP CEBRichMessage::DeleteLineItem(LONG nLineId, LONG nItemId)
{
	BoostWriteLock wtlock(m_pList.mutex());
	CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
	for (;pIter!=m_pList.end();pIter++)
	{
		const CMrLine::pointer& pLine = *pIter;
		if (pLine->GetLineId()==nLineId)
		{
			if (nItemId<=0)
			{
				m_pList.erase(pIter);
				wtlock.unlock();
				this->FireViewChange();
				return S_OK;
			}else
			{
				std::vector<CMrItem::pointer>::const_iterator pIterItem = pLine->m_pList.begin();
				for (;pIterItem!=pLine->m_pList.end(); pIterItem++)
				//for (size i=0;i<pLine->m_pList.size(); i++)
				{
					const CMrItem::pointer& pItem = *pIterItem;
					//const CMrItem::pointer& pItem = pLine->m_pList[i];
					if (pItem->GetItemId()==nItemId)
					{
						pLine->m_pList.erase(pIterItem);
						//pLine->m_pList.erase(i);
						wtlock.unlock();
						this->FireViewChange();
						return S_OK;
					}
				}
			}
		}
	}
	return S_FALSE;
}
STDMETHODIMP CEBRichMessage::ViewLine(LONGLONG nMsgId)
{
	BoostReadLock rdlock(m_pList.mutex());
	CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
	for (;pIter!=m_pList.end();pIter++)
	{
		const CMrLine::pointer& pLine = *pIter;
		if (pLine->GetMsgId()==nMsgId)
		{
			const long nScrollPos = pLine->GetRectF().GetTop();
			rdlock.unlock();
			return ScrollToPos(nScrollPos);
			//// select
			//CRect rect;
			//this->GetClientRect(&rect);
			//const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
			//PAINTSTRUCT ps;
			//HDC hDC = BeginPaint(&ps);
			//Gdiplus::Graphics m_graphics(hDC);
			//pLine->SelectAll(m_graphics,layoutRect);
			//EndPaint(&ps);
			//return S_OK;
		}
	}
	return S_FALSE;
}
STDMETHODIMP CEBRichMessage::GetLineCount(LONG * pVal)
{
	*pVal = m_pList.size();
	return S_OK;
}

STDMETHODIMP CEBRichMessage::SetFrameArc(SHORT nArcOffset, SHORT nPoloygonWidth, SHORT nPoloygonHeight)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	m_pCurrentLine->SetFrameArc(nArcOffset,nPoloygonWidth,nPoloygonHeight);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::SetFrameBorderColor(ULONG colorRef)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	Gdiplus::Color pColor;
	pColor.SetFromCOLORREF(colorRef);
	m_pCurrentLine->SetFrameBorderColor(colorRef);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::SetFrameBackGroundColor(ULONG colorRef)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	Gdiplus::Color pColor;
	pColor.SetFromCOLORREF(colorRef);
	m_pCurrentLine->SetFrameBackGroundColor(pColor);
	return S_OK;
}

STDMETHODIMP CEBRichMessage::WriteString(BSTR sString, ULONG colorRef)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);

	CString sString1(sString);
	CString sStringLower(sString1);
	sStringLower.MakeLower();
	int nfind1 = sStringLower.Find(L"http://");
	if (nfind1 < 0)
		nfind1 = sStringLower.Find(L"https://");
	if (nfind1 < 0)
		nfind1 = sStringLower.Find(L"www.");
	if (nfind1>=0)
	{
		if (nfind1 > 0)
		{
			const CComBSTR sLeftText(sString1.Left(nfind1));
			WriteString(sLeftText,colorRef);
			sString1 = sString1.Right(sString1.GetLength()-nfind1);
			//sStringLower = sStringLower.Right(sStringLower.GetLength()-nfind1);
		}
		nfind1 = -1;
		//nfind1 = sString1.FindOneOf(L" \t\r\n;,；，。");
		//bool bFindBigChar = false;
		//if (nfind1 < 0)
		{
			const size_t nLen = sString1.GetLength();	// * 判断中文
			for (size_t i=0;i<nLen;i++)
			{
				const wchar_t pChar = sString1.GetAt(i);
				if (pChar>255 || pChar==160 || pChar=='\t' || pChar=='\r' || pChar=='\n' || pChar==';' || pChar==','  || pChar==' ')	// 160=' '
				//if (pChar<0 || pChar=='\t' || pChar=='\r' || pChar=='\n' || pChar==';' || pChar==',')
				{
					nfind1 = i;
					//bFindBigChar = true;
					break;
				}				
			}
		}
		if (nfind1 > 0)
		{
			const CComBSTR sLeftText(sString1.Left(nfind1));
			WriteUrl(sLeftText,sLeftText, MR_BTN_TYPE_LDBLCLICK_OPEN|MR_BTN_TYPE_MOVE_ENTER_CB|MR_BTN_TYPE_MOVE_LEAVE_CB,RGB(0,0,255));
			sString1 = sString1.Right(sString1.GetLength()-nfind1);
			WriteString(CComBSTR(sString1),colorRef);
			return S_OK;
		}else
		{
			const CComBSTR sStringTemp(sString1);
			WriteUrl(sStringTemp,sStringTemp,MR_BTN_TYPE_LDBLCLICK_OPEN|MR_BTN_TYPE_MOVE_ENTER_CB|MR_BTN_TYPE_MOVE_LEAVE_CB,RGB(0,0,255));
			return S_OK;
		}
	}else
	{
		std::wstring sText(sString);
		std::wstring::size_type nfind = sText.find(L"\r");
		if (nfind == std::wstring::npos)
			nfind = sText.find(L"\n");
		// 回车换行处理
		while (nfind != std::wstring::npos)
		{
			const CComBSTR sLeftText(sText.substr(0,nfind).c_str());
			sText = sText.substr(nfind+1);
			nfind = sText.find(L"\r");
			if (nfind == std::wstring::npos)
				nfind = sText.find(L"\n");
			if (sLeftText.Length()>0)	// ***
				WriteText(sLeftText,colorRef);
			else
				WriteLine();			// *** 空字符串，增加一个回车即可；
			WriteLine();
		}
		if (!sText.empty())
		{
			WriteText(CComBSTR(sText.c_str()),colorRef);
		}
	}
	//this->FireViewChange();
	return S_OK;
}
inline int ParseStringW(const wchar_t * lpszString, const wchar_t * lpszInterval, std::vector<std::wstring> & pOut)
{
	const std::wstring sIn(lpszString);
	const size_t nInLen = sIn.size();
	const size_t nIntervalLen = wcslen(lpszInterval);
	pOut.clear();
	std::wstring::size_type nFindBegin = 0;
	while (nFindBegin<nInLen)
	{
		std::wstring::size_type find = sIn.find(lpszInterval,nFindBegin);
		if (find == std::wstring::npos)
		{
			pOut.push_back(sIn.substr(nFindBegin));
			break;
		}
		if (find==nFindBegin)
		{
			pOut.push_back(L"");	// 空
		}else
		{
			pOut.push_back(sIn.substr(nFindBegin, (find-nFindBegin)));
		}
		nFindBegin = find+nIntervalLen;
	}
	return (int)pOut.size();
}

const std::wstring theTextClickTab		= L"#CTRL:";		// #CTRL:BTNTYPE:PARAM:TEXT#

void CEBRichMessage::WriteText(const CMrLine::pointer& pCurrentLine, BSTR sText, ULONG colorRef)
{
	MR_ITEM_TYPE nItemType = MR_ITEM_TYPE_TEXT;
	std::wstring sTextTemp(sText);
	int nBtnType = 0;
	ULONG nItemParam = 0;
	std::wstring sItemParam;
	// 判断 theTextClickTab
	std::wstring::size_type nFind1 = sTextTemp.find(theTextClickTab);
	while (nFind1!=std::wstring::npos)
	{
		if (nItemType == MR_ITEM_TYPE_CLICK_TEXT && nFind1>0)
		{
			CMrItem::pointer pItem = CMrItem::create(pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_CLICK_TEXT,this->m_hWnd,m_nFontSize);
			pItem->SetCalllback((CMrCallback*)pCurrentLine.get());
			pItem->SetItemString(sTextTemp.substr(0,nFind1));
			pItem->SetItemParam(sItemParam);
			pItem->SetCtrlType(nBtnType);
			pItem->SetItemParam2(nItemParam);
			//pItem->SetFont(Gdiplus::Font(&fontFamily, const_font_size, FontStyleRegular, UnitPixel));
			Gdiplus::Color pColor;
			pColor.SetFromCOLORREF(colorRef);
			pItem->SetBrush(Gdiplus::SolidBrush(pColor));
			pCurrentLine->AddItem(pItem);
			sItemParam.clear();
			nItemParam = 0;
			sTextTemp = sTextTemp.substr(nFind1);	// 设到下一个
			nFind1 = 0;
			nItemType = MR_ITEM_TYPE_TEXT;
		}

		std::wstring::size_type nFind2 = sTextTemp.find(L"#",nFind1+theTextClickTab.size());
		if (nFind2==std::wstring::npos)
			break;

		if (nFind1>0)
		{
			WriteText(CComBSTR(sTextTemp.substr(0,nFind1).c_str()),colorRef);
		}
		sItemParam = sTextTemp.substr(0,nFind2);
		sItemParam = sItemParam.substr(nFind1+theTextClickTab.size());
		std::vector<std::wstring> pList;
		if (ParseStringW(sItemParam.c_str(),L":",pList)<2)
			break;

		nBtnType = _wtoi(pList[0].c_str());
		nItemParam = _wtoi(pList[1].c_str());
		sItemParam = pList.size()>=3?pList[2]:L"";
		sTextTemp = sTextTemp.substr(nFind2+1);
		nFind1 = sTextTemp.find(theTextClickTab);
		nItemType = MR_ITEM_TYPE_CLICK_TEXT;
	}

	if (sTextTemp.empty())
		return;
	CMrItem::pointer pItem = CMrItem::create(pCurrentLine->GetNextItemId(),nItemType,this->m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)pCurrentLine.get());
	pItem->SetItemString(sTextTemp);
	pItem->SetCtrlType(nBtnType);
	pItem->SetItemParam2(nItemParam);
	pItem->SetItemParam(sItemParam);
	//pItem->SetFont(Gdiplus::Font(&fontFamily, const_font_size, FontStyleRegular, UnitPixel));
	Gdiplus::Color pColor;
	pColor.SetFromCOLORREF(colorRef);
	pItem->SetBrush(Gdiplus::SolidBrush(pColor));
	pCurrentLine->AddItem(pItem);
}
STDMETHODIMP CEBRichMessage::WriteText(BSTR sText,ULONG colorRef)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	WriteText(m_pCurrentLine,sText,colorRef);
	//MR_ITEM_TYPE nItemType = MR_ITEM_TYPE_TEXT;
	//std::wstring sTextTemp(sText);
	//int nBtnType = 0;
	//ULONG nItemParam = 0;
	//std::wstring sItemParam;
	//// 判断 theTextClickTab
	//std::wstring::size_type nFind1 = sTextTemp.find(theTextClickTab);
	//while (nFind1!=std::wstring::npos)
	//{
	//	if (nItemType == MR_ITEM_TYPE_CLICK_TEXT && nFind1>0)
	//	{
	//		CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_CLICK_TEXT,this->m_hWnd,m_nFontSize);
	//		pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	//		pItem->SetItemString(sTextTemp.substr(0,nFind1));
	//		pItem->SetItemParam(sItemParam);
	//		pItem->SetCtrlType(nBtnType);
	//		pItem->SetItemParam2(nItemParam);
	//		//pItem->SetFont(Gdiplus::Font(&fontFamily, const_font_size, FontStyleRegular, UnitPixel));
	//		Gdiplus::Color pColor;
	//		pColor.SetFromCOLORREF(colorRef);
	//		pItem->SetBrush(Gdiplus::SolidBrush(pColor));
	//		m_pCurrentLine->AddItem(pItem);
	//		sItemParam.clear();
	//		nItemParam = 0;
	//		sTextTemp = sTextTemp.substr(nFind1);	// 设到下一个
	//		nFind1 = 0;
	//		nItemType = MR_ITEM_TYPE_TEXT;
	//	}

	//	std::wstring::size_type nFind2 = sTextTemp.find(L"#",nFind1+theTextClickTab.size());
	//	if (nFind2==std::wstring::npos)
	//		break;

	//	if (nFind1>0)
	//	{
	//		WriteText(CComBSTR(sTextTemp.substr(0,nFind1).c_str()),colorRef);
	//	}
	//	sItemParam = sTextTemp.substr(0,nFind2);
	//	sItemParam = sItemParam.substr(nFind1+theTextClickTab.size());
	//	std::vector<std::wstring> pList;
	//	if (ParseStringW(sItemParam.c_str(),L":",pList)<2)
	//		break;

	//	nBtnType = _wtoi(pList[0].c_str());
	//	nItemParam = _wtoi(pList[1].c_str());
	//	sItemParam = pList.size()>=3?pList[2]:L"";
	//	sTextTemp = sTextTemp.substr(nFind2+1);
	//	nFind1 = sTextTemp.find(theTextClickTab);
	//	nItemType = MR_ITEM_TYPE_CLICK_TEXT;
	//}

	//if (sTextTemp.empty())
	//	return S_OK;
	//CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),nItemType,this->m_hWnd,m_nFontSize);
	//pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	//pItem->SetItemString(sTextTemp);
	//pItem->SetCtrlType(nBtnType);
	//pItem->SetItemParam2(nItemParam);
	//pItem->SetItemParam(sItemParam);
	////pItem->SetFont(Gdiplus::Font(&fontFamily, const_font_size, FontStyleRegular, UnitPixel));
	//Gdiplus::Color pColor;
	//pColor.SetFromCOLORREF(colorRef);
	//pItem->SetBrush(Gdiplus::SolidBrush(pColor));
	//m_pCurrentLine->AddItem(pItem);
	////this->FireViewChange();
	return S_OK;
}
// 从内存加载图片，失败返回NULL
Bitmap* LoadBitmapFromMemory(const void* memory, DWORD size)
{
    Bitmap* bmp = NULL;
    IStream* stream = NULL;
    if (CreateStreamOnHGlobal(NULL, TRUE, &stream) == S_OK)
    {
        ULARGE_INTEGER uli;
        uli.QuadPart = size;
        stream->SetSize(uli);
        if (stream->Write(memory, size, NULL) == S_OK)
            bmp = new Bitmap(stream);
        stream->Release();
    }
    return bmp;
}
Bitmap* LoadBitmapFromFile(const TCHAR* file_name)
{
    Bitmap* bmp = NULL;
    HANDLE file_handle = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle != INVALID_HANDLE_VALUE)
    {
        DWORD temp = 0;
        DWORD file_size = GetFileSize(file_handle, &temp);
        if (file_size && !temp)  // 不处理大于4G的文件
        {
            // 将图片文件读到内存后，再从内存创建Bitmap
            unsigned char* buffer = new unsigned char[file_size];
            if (ReadFile(file_handle, buffer, file_size, &temp, NULL))
                bmp = LoadBitmapFromMemory(buffer, temp);
            delete [] buffer;
        }
        CloseHandle(file_handle);
    }
    return bmp;
}

STDMETHODIMP CEBRichMessage::WriteImage(BSTR sImageFilePath, LONG nLimitWidth, LONG nLimitHeight)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	std::wstring sPath(sImageFilePath);
	int nCtrlType = 0;
	ULONG nItemParam = 0;
	std::wstring sItemParam;
	std::wstring::size_type nFind1 = sPath.find(theTextClickTab);
	if (nFind1!=std::wstring::npos)
	{
		std::wstring::size_type nFind2 = sPath.find(L"#",nFind1+theTextClickTab.size());
		if (nFind2!=std::wstring::npos)
		{
			sItemParam = sPath.substr(0,nFind2);
			sItemParam = sItemParam.substr(nFind1+theTextClickTab.size());
			//::MessageBoxW(NULL,sItemParam.c_str(),L"test1",MB_OK);
			std::vector<std::wstring> pList;
			if (ParseStringW(sItemParam.c_str(),L":",pList)>=2)
			{
				nCtrlType = _wtoi(pList[0].c_str());
				nItemParam = _wtoi(pList[1].c_str());
				sItemParam = pList.size()>=3?pList[2]:L"";
			}
			sPath = sPath.substr(nFind2+1);
		}
	}

	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_IMG,this->m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	Gdiplus::Bitmap * pBitmep = LoadBitmapFromFile(sPath.c_str());
	if (pBitmep==NULL)
		pBitmep = new Gdiplus::Bitmap(sPath.c_str());

	pItem->SetImage(pBitmep,nLimitWidth,nLimitHeight);
	pItem->SetCtrlType(nCtrlType);
	pItem->SetItemParam2(nItemParam);
	pItem->SetItemParam(sItemParam);
	pItem->SetUrl(sPath, sPath);
	//pItem->SetItemString(lpszPath);
	m_pCurrentLine->AddItem(pItem);
	//this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WriteWav(BSTR sWavTitle, BSTR sWavFilePath, LONG nCtrlType)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	const std::wstring sTitle(sWavTitle);
	const std::wstring sPath(sWavFilePath);
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_WAV,this->m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetCtrlType(nCtrlType);
	pItem->SetUrl(sTitle, sPath);
	pItem->SetBrush(Gdiplus::SolidBrush(Gdiplus::Color::Green));
	m_pCurrentLine->AddItem(pItem);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WriteHICON(ULONG* hIcon, BSTR sUrl, LONG nCtrlType)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_IMG,m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetImage(Gdiplus::Bitmap::FromHICON((HICON)hIcon),0,0);
	pItem->SetCtrlType(nCtrlType);
	pItem->SetUrl(L"", sUrl);
	m_pCurrentLine->AddItem(pItem);
	//this->FireViewChange();
	return S_OK;
}

STDMETHODIMP CEBRichMessage::WriteTime(ULONG nTimeValue, BSTR sTimeFormat)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);

	FontFamily fontFamily(theFontFamily.c_str());
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_TIME,m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetTime(nTimeValue==0?time(0):nTimeValue);
	const std::wstring sItemParam(sTimeFormat);
	if (sItemParam.empty())
		pItem->SetItemParam(L"%H:%M");
	else
		pItem->SetItemParam(sItemParam);
	pItem->SetFont(Gdiplus::Font(&fontFamily, m_nFontSize-1, FontStyleRegular, UnitPixel));
	pItem->SetBrush(Gdiplus::SolidBrush(Gdiplus::Color(127,127,127)));
	m_pCurrentLine->AddItem(pItem);
	//this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WriteUrl(BSTR sTitle, BSTR sUrl, LONG nCtrlType, ULONG colorRef)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	std::wstring sTitleTemp(sTitle);
	ULONG nItemParam = m_nDefaultUrlCbData1;
	std::wstring sItemParam(sUrl);
	if ((nCtrlType&(MR_BTN_TYPE_LCLICK_CB|MR_BTN_TYPE_LDBLCLICK_CB|MR_BTN_TYPE_RCLICK_CB|MR_BTN_TYPE_MOVE_ENTER_CB|MR_BTN_TYPE_MOVE_LEAVE_CB))!=0)
	{
		std::wstring::size_type nFind1 = sTitleTemp.find(theTextClickTab);
		if (nFind1!=std::wstring::npos)
		{
			std::wstring::size_type nFind2 = sTitleTemp.find(L"#",nFind1+theTextClickTab.size());
			if (nFind2!=std::wstring::npos)
			{
				sItemParam = sTitleTemp.substr(0,nFind2);
				sItemParam = sItemParam.substr(nFind1+theTextClickTab.size());
				std::vector<std::wstring> pList;
				if (ParseStringW(sItemParam.c_str(),L":",pList)>=2)
				{
					//nCtrlType = _wtoi(pList[0].c_str());
					nItemParam = _wtoi(pList[1].c_str());
					if (pList.size()>=3)
						sItemParam = pList[2];
					else
						sItemParam = sUrl;
				}
				sTitleTemp = sTitleTemp.substr(nFind2+1);
			}
		}
	}
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_URL,this->m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetCtrlType(nCtrlType);
	pItem->SetUrl(sTitleTemp, sUrl);
	pItem->SetItemParam2(nItemParam);
	pItem->SetItemParam(sItemParam);
	if (colorRef==-1)
		pItem->SetBrush(Gdiplus::SolidBrush(Gdiplus::Color::Blue));
	else
	{
		Gdiplus::Color pColor;
		pColor.SetFromCOLORREF(colorRef);
		pItem->SetBrush(Gdiplus::SolidBrush(pColor));
	}
	m_pCurrentLine->AddItem(pItem);
	//this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WriteLine(void)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_ENTER,m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	m_pCurrentLine->AddItem(pItem);
	//this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WriteSpace(ULONG nSpaceNumber)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_SPACE,m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetItemParam2(max(1,nSpaceNumber));
	m_pCurrentLine->AddItem(pItem);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WriteOpenFile(BSTR sTitle, BSTR sFilePath)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	FontFamily fontFamily(theFontFamily.c_str());
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_OPENFILE,m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetCtrlType(MR_BTN_TYPE_LCLICK_OPEN);
	pItem->SetUrl(sTitle, sFilePath);
	pItem->SetFont(Gdiplus::Font(&fontFamily, m_nFontSize, FontStyleUnderline, UnitPixel));
	pItem->SetBrush(Gdiplus::SolidBrush(Gdiplus::Color::Blue));
	m_pCurrentLine->AddItem(pItem);
	//this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WriteOpenDir(BSTR sTitle, BSTR sFilePath)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	FontFamily fontFamily(theFontFamily.c_str());
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_OPENDIR,m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetCtrlType(MR_BTN_TYPE_LCLICK_OPEN);
	pItem->SetUrl(sTitle, sFilePath);
	pItem->SetFont(Gdiplus::Font(&fontFamily, m_nFontSize, FontStyleUnderline, UnitPixel));
	pItem->SetBrush(Gdiplus::SolidBrush(Gdiplus::Color::Blue));
	m_pCurrentLine->AddItem(pItem);
	//this->FireViewChange();
	return S_OK;
}
STDMETHODIMP CEBRichMessage::WritePic(LONG nPicType, LONG x, LONG y, LONG w, LONG h, ULONG colorRef, LONG nCtrlType, ULONG nData1)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	FontFamily fontFamily(theFontFamily.c_str());
	CMrItem::pointer pItem = CMrItem::create(m_pCurrentLine->GetNextItemId(),MR_ITEM_TYPE_PIC,m_hWnd,m_nFontSize);
	pItem->SetCalllback((CMrCallback*)m_pCurrentLine.get());
	pItem->SetCtrlType(nCtrlType);
	pItem->SetItemParam2(nData1);
	pItem->SetPicInfo(nPicType, RectF(x,y,w,h));
	pItem->SetFont(Gdiplus::Font(&fontFamily, m_nFontSize, FontStyleUnderline, UnitPixel));
	switch (nPicType)
	{
	case 1:	// 画圆
		{
			Gdiplus::Color pColor;
			pColor.SetFromCOLORREF(colorRef);
			pItem->SetBrush(Gdiplus::SolidBrush(pColor));
		}break;
	case 2:	// 画水平线条
		{
			Gdiplus::Color pColor;
			pColor.SetFromCOLORREF(colorRef);
			pItem->SetPen(Gdiplus::Pen(pColor,h));
		}break;
	default:
		break;
	}
		m_pCurrentLine->AddItem(pItem);
	return S_OK;
}

STDMETHODIMP CEBRichMessage::AddContextMenuItem(BSTR sItemName, ULONG nItemData, VARIANT_BOOL bSingleSelect)
{
	if (sItemName==NULL) return S_FALSE;
	std::vector<CContextMenuItemInfo::pointer>& pList = bSingleSelect==VARIANT_TRUE?m_pContextMenuItem1:m_pContextMenuItem2;
	if (pList.size()>10) return S_FALSE;
	pList.push_back(CContextMenuItemInfo::create(sItemName,nItemData));
	return S_OK;
}
STDMETHODIMP CEBRichMessage::ClearContextMenuItem(void)
{
	m_pContextMenuItem1.clear();
	m_pContextMenuItem2.clear();
	return S_OK;
}

STDMETHODIMP CEBRichMessage::GetSelectString(BSTR* pVal)
{
	std::wstring result;
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			const std::wstring sSelectString = pLine->GetSelectString();
			if (sSelectString.empty())
				continue;
			if (!result.empty())
				result.append(L"\r\n");
			result.append(sSelectString);
		}
	}
	*pVal = ::SysAllocString(result.c_str());
	return S_OK;
}
STDMETHODIMP CEBRichMessage::SetSelectCbData1(ULONG nData1)
{
	m_nSelectStringCbData1 = nData1;
	return S_OK;
}
STDMETHODIMP CEBRichMessage::SetDefaultUrlCbData1(ULONG nData1)
{
	m_nDefaultUrlCbData1 = nData1;
	return S_OK;
}

#define PARAMETER_DISABLE_RECEIPT 1
STDMETHODIMP CEBRichMessage::SetParameter(LONG nParameter, LONG nValue)
{
	bool bUpdateView = false;
	switch (nParameter)
	{
	case PARAMETER_DISABLE_RECEIPT:
		{
			const bool bOld = m_bDisableReceipt;
			m_bDisableReceipt = nValue==0?false:true;
			if (m_bDisableReceipt!=bOld)
			{
				bUpdateView = m_pList.empty()?false:true;
				BoostReadLock rdlock(m_pList.mutex());
				CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
				for (;pIter!=m_pList.end();pIter++)
				{
					const CMrLine::pointer& pLine = *pIter;
					pLine->SetShowReceiptFlag(m_bDisableReceipt?false:true);
				}
			}
		}break;
	default:
		break;
	}
	if (bUpdateView)
	{
		this->FireViewChange();
	}
	return S_OK;
}
STDMETHODIMP CEBRichMessage::GetParameter(LONG nParameter, LONG* pValue)
{
	switch (nParameter)
	{
	case PARAMETER_DISABLE_RECEIPT:
		*pValue = m_bDisableReceipt?1:0;
		break;
	}
	return S_OK;
}
STDMETHODIMP CEBRichMessage::FreeParameter(LONG nParameter, LONG nValue)
{
	return S_OK;
}

//STDMETHODIMP CEBRichMessage::AddLine2(LONGLONG nMsgId, LONG nReceiptFlag)
//{
//	AddLine(nMsgId);
//	m_pCurrentLine->SetReceiptFlag(nReceiptFlag);
//	return S_OK;
//}
STDMETHODIMP CEBRichMessage::SetReceiptFlag(LONG nReceiptFlag)
{
	if (m_pCurrentLine.get()==NULL)
		AddLine(0);
	m_pCurrentLine->SetReceiptFlag(nReceiptFlag);
	UpdateReceiptData(m_pCurrentLine, m_nMsgReceipt);
	return S_OK;
}
STDMETHODIMP CEBRichMessage::SetMsgReceiptFlag(LONGLONG nMsgId, LONG nReceiptFlag)
{
	HRESULT result = S_FALSE;
	bool bUpdateView = false;
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->GetMsgId()==nMsgId)
			{
				if (pLine->SetReceiptFlag((int)nReceiptFlag))
				{
					bUpdateView = true;
					UpdateReceiptData(pLine, m_nMsgReceipt);
				}
				result = S_OK;
				continue;
			}else if (result == S_OK)	// 已经处理，直接跳出
			{
				break;
			}
		}
	}
	if (bUpdateView)
	{
		this->FireViewChange();
	}
	return result;
}
STDMETHODIMP CEBRichMessage::UpdateMsgText(LONGLONG nMsgId, LONG nLineOffset, BSTR sNewText, ULONG colorRef)
{
	HRESULT result = S_FALSE;
	bool bUpdateView = false;
	LONG nFindMsgLineIndex = 0;
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->GetMsgId()==nMsgId)
			{
				if (nLineOffset<0 || nFindMsgLineIndex==nLineOffset)
				{
					pLine->ClearItem();
					WriteText(pLine,sNewText,colorRef);
					bUpdateView = true;
					result = S_OK;
					if (nLineOffset>=0) break;
				}
				nFindMsgLineIndex++;
				continue;
			}else if (result == S_OK ||		// * 已经处理，直接跳出
				nFindMsgLineIndex>0)				// * 前面已经找到，但给的 nLineOffset 找不到；
			{
				break;
			}
		}
	}
	if (bUpdateView)
	{
		this->FireViewChange();
	}
	return result;
}
LRESULT CEBRichMessage::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	KillTimer(TIMERID_DRAW_GIF_IMAGE);
	m_pSkinVBarInterface.Release();
	m_pSkinVBar.DestroyWindow();
	return 0;
}

LRESULT CEBRichMessage::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	const UINT nIDEvent = wParam;
	if (TIMERID_CHECK_LEAVE==nIDEvent)
	{
		//if (m_pMoveEnterItem.get()==NULL)
		//	KillTimer(TIMERID_CHECK_LEAVE);
		//else
		{
			CPoint pos;
			::GetCursorPos(&pos);
			CRect rect;
			this->GetWindowRect(&rect);
			if (!rect.PtInRect(pos) && m_pMoveEnterItem.get()!=NULL)
			{
				KillTimer(TIMERID_CHECK_LEAVE);
				//if (m_pMoveEnterItem->GetSelected())
				//	MessageBoxA(NULL,_T("OnTimer"),_T("m_pMoveEnterItem selected ok"),MB_OK);
				//else
				//	MessageBoxA(NULL,_T("OnTimer"),_T("m_pMoveEnterItem selected false"),MB_OK);
				if (m_nSelectStringCbData1 !=0 && m_pMoveEnterItem->GetSelected())
				{
					BSTR sParamString = ::SysAllocString(L"");
					this->Fire_onItemMoveLeave(m_pMoveEnterLine->GetLineId(),m_pMoveEnterItem->GetItemId(),m_nSelectStringCbData1,sParamString,m_pMoveEnterLine->GetMsgId());
					::SysFreeString(sParamString);
				}else if ((m_pMoveEnterItem->GetCtrlType()&MR_BTN_TYPE_MOVE_LEAVE_CB)==MR_BTN_TYPE_MOVE_LEAVE_CB)
				{
					BSTR sParamString = ::SysAllocString(m_pMoveEnterItem->GetItemParam().c_str());
					this->Fire_onItemMoveLeave(m_pMoveEnterLine->GetLineId(),m_pMoveEnterItem->GetItemId(),m_pMoveEnterItem->GetItemParam2(),sParamString,m_pMoveEnterLine->GetMsgId());
					::SysFreeString(sParamString);
				}
				m_pMoveEnterItem.reset();
				m_pMoveEnterLine.reset();
				if (m_bEnterInFocus && ::GetFocus()!=this->m_hWnd)
				{
					SetFocus();
				}
			}
		}
	}else if (nIDEvent == TIMERID_DRAW_GIF_IMAGE)
	{
		Gdiplus::Graphics m_graphics(this->m_hWnd);
		CRect rect;
		this->GetClientRect(&rect);
		RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
		BoostReadLock rdlock(m_pList.mutex());
		DWORD dwNowTime = timeGetTime();
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->OnDrawGif(m_graphics,layoutRect,dwNowTime)==1)
				break;
		}
	}else if (TIMERID_CLICK == nIDEvent)
	{
		KillTimer(nIDEvent);
		CRect rect;
		this->GetClientRect(&rect);
		const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
		{
			CPoint pos;
			::GetCursorPos(&pos);
			ScreenToClient(&pos);
			const Point pPos = Point(pos.x,pos.y);

			const bool bIsControl = (GetKeyState(VK_CONTROL)&0x80)==0?false:true;
			//bool bInSelect = false;
			//if (!bIsControl)
			//{
			//	AUTO_RLOCK(m_pList);
			//	CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
			//	for (;pIter!=m_pList.end();pIter++)
			//	{
			//		const CMrLine::pointer& pLine = *pIter;
			//		if (pLine->InSelect(pPos,layoutRect))
			//		{
			//			bInSelect = true;
			//			break;
			//		}
			//	}
			//}
			//if (!bInSelect)
			//{
			//	PAINTSTRUCT ps;
			//	HDC hDC = BeginPaint(&ps);
			//	Gdiplus::Graphics m_graphics(hDC);
			//	// clear select
			//	BoostReadLock rdlock(m_pList.mutex());
			//	CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
			//	for (;pIter!=m_pList.end();pIter++)
			//	{
			//		const CMrLine::pointer& pLine = *pIter;
			//		pLine->ClearSelect(m_graphics,layoutRect);
			//	}
			//	EndPaint(&ps);
			//}//else
			{
				BoostReadLock rdlock(m_pList.mutex());
				CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
				for (;pIter!=m_pList.end();pIter++)
				{
					const CMrLine::pointer& pLine = *pIter;
					//if (!bInSelect && pLine->InSelect(pPos,layoutRect))
					//	bInSelect = true;

					if (pLine->PtInLine(m_pSelectBegin,layoutRect))
					{
						const CMrLine::pointer pLineTemp = pLine;
						rdlock.unlock();
						CMrItem::pointer pClickItem;
						pLineTemp->Onclick(m_pSelectBegin,layoutRect,false,&pClickItem);
						if (pClickItem.get()==NULL)
							break;
						if ((pClickItem->GetCtrlType()&MR_BTN_TYPE_LCLICK_CB)==MR_BTN_TYPE_LCLICK_CB ||
							(bIsControl && (pClickItem->GetCtrlType()&MR_BTN_TYPE_LDBLCLICK_CB)==MR_BTN_TYPE_LDBLCLICK_CB))
						{
							BSTR sParamString = ::SysAllocString(pClickItem->GetItemParam().c_str());
							this->Fire_onItemLBtnClick(pLineTemp->GetLineId(),pClickItem->GetItemId(),pClickItem->GetItemParam2(),sParamString,pLineTemp->GetMsgId());
							::SysFreeString(sParamString);
						}
						break;
					}
				}
			}
			////if (!bInSelect)
			//{
			//	PAINTSTRUCT ps;
			//	HDC hDC = BeginPaint(&ps);
			//	Gdiplus::Graphics m_graphics(hDC);
			//	// clear select
			//	BoostReadLock rdlock(m_pList.mutex());
			//	CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
			//	for (;pIter!=m_pList.end();pIter++)
			//	{
			//		const CMrLine::pointer& pLine = *pIter;
			//		pLine->ClearSelect(m_graphics,layoutRect);
			//	}
			//	EndPaint(&ps);
			//}
		}
		m_pSelectBegin.X=0;
		m_pSelectBegin.Y=0;
		m_pSelectEnd.X=0;
		m_pSelectEnd.Y=0;
	}else if (TIMERID_SELECT==nIDEvent)
	{
		KillTimer(nIDEvent);
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(&ps);
		Gdiplus::Graphics m_graphics(hDC);
		//m_graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
		m_graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		CRect rect;
		this->GetClientRect(&rect);
		const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
		{
			BoostReadLock rdlock(m_pList.mutex());
			CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				pLine->Select(m_graphics,m_pSelectBegin,m_pSelectEnd,layoutRect);
			}
		}
		EndPaint(&ps);
	}
	return 0;
}

LRESULT CEBRichMessage::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return TRUE;
	return 0;
}

LRESULT CEBRichMessage::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPoint pos;
	::GetCursorPos(&pos);
	ScreenToClient(&pos);
	m_pSelectBegin = Point(pos.x,pos.y);
	m_bMouseDown = true;
	SetCapture();
	return 0;
}

LRESULT CEBRichMessage::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPoint pos;
	::GetCursorPos(&pos);
	ScreenToClient(&pos);
	CRect rect;
	this->GetClientRect(&rect);
	const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
	if (m_bMouseDown)
	{
		if (pos.y>rect.bottom+10)
		{
			// down
			LONG nRealScrollTimes = 0;
			ScrollBy(20,nRealScrollTimes);
			m_pSelectBegin.Y -= nRealScrollTimes;
		}else if (pos.y<rect.top-10)
		{
			// down
			LONG nRealScrollTimes = 0;
			ScrollBy(-20,nRealScrollTimes);
			m_pSelectBegin.Y -= nRealScrollTimes;
		}else if (abs(pos.x-m_pSelectEnd.X)>=5 || abs(pos.y-m_pSelectEnd.Y)>=8)
		{
			m_pSelectEnd = Point(pos.x,pos.y);
			SetTimer(TIMERID_SELECT,10,NULL);
		}
	//}else if (m_pSelectBegin.X==0)
	//{
	//	Point pPos(pos.x,pos.y);
	//	boost::mutex::scoped_lock lock(m_pList.mutex());
	//	CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
	//	for (;pIter!=m_pList.end();pIter++)
	//	{
	//		const CMrLine::pointer& pLine = *pIter;
	//		if (pLine->OnMouseMove(pPos, layoutRect))
	//		{
	//			SetCursor(LoadCursor(NULL,IDC_HAND));
	//			return 0;
	//		}
	//	}
	//	SetCursor(LoadCursor(NULL,IDC_IBEAM));
	//	return 0;
	}
	return 0;
}

LRESULT CEBRichMessage::OnMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	const short zDelta = HIWORD(wParam);
	if ((GetKeyState(VK_CONTROL)&0x80)==0x80)
	{
		BOOL bHandle = TRUE;
		if (zDelta>0)
			OnFontSize1(0,0,0,bHandle);
		else
			OnFontSize2(0,0,0,bHandle);
	}else
	{
		const int nScrollTimes = (-1*zDelta);
		ScrollBy(nScrollTimes);
		if (m_pSkinVBarInterface != NULL)
			m_pSkinVBarInterface->UpdatePosition();
	}
	return 0;
}

LRESULT CEBRichMessage::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default

	if(m_bMouseDown)
	{
		ReleaseCapture();
		m_bMouseDown = false;
		CPoint pos;
		::GetCursorPos(&pos);
		ScreenToClient(&pos);
		m_pSelectEnd = Point(pos.x,pos.y);
		CRect rect;
		this->GetClientRect(&rect);
		const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
		if (m_pSelectBegin.X==m_pSelectEnd.X && m_pSelectBegin.Y==m_pSelectEnd.Y)
		{
			// 点击
			const bool bIsControl = (GetKeyState(VK_CONTROL)&0x80)==0?false:true;
			if (!bIsControl)
			{
				if (m_pMoveEnterItem.get()!=NULL)
				{
					KillTimer(TIMERID_CHECK_LEAVE);
					if (m_nSelectStringCbData1!=0 && m_pMoveEnterItem->GetSelected())
					{
						BSTR sParamString = ::SysAllocString(L"");
						this->Fire_onItemMoveLeave(m_pMoveEnterLine->GetLineId(),m_pMoveEnterItem->GetItemId(),m_nSelectStringCbData1,sParamString,m_pMoveEnterLine->GetMsgId());
						::SysFreeString(sParamString);
					}else if ((m_pMoveEnterItem->GetCtrlType()&MR_BTN_TYPE_MOVE_LEAVE_CB)==MR_BTN_TYPE_MOVE_LEAVE_CB)
					{
						BSTR sParamString = ::SysAllocString(m_pMoveEnterItem->GetItemParam().c_str());
						this->Fire_onItemMoveLeave(m_pMoveEnterLine->GetLineId(),m_pMoveEnterItem->GetItemId(),m_pMoveEnterItem->GetItemParam2(),sParamString,m_pMoveEnterLine->GetMsgId());
						::SysFreeString(sParamString);
					}
					m_pMoveEnterItem.reset();
					m_pMoveEnterLine.reset();
				}
				PAINTSTRUCT ps;
				HDC hDC = BeginPaint(&ps);
				Gdiplus::Graphics m_graphics(hDC);
				// clear select
				BoostReadLock rdlock(m_pList.mutex());
				CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
				for (;pIter!=m_pList.end();pIter++)
				{
					const CMrLine::pointer& pLine = *pIter;
					pLine->ClearSelect(m_graphics,layoutRect);
				}
				EndPaint(&ps);
			}

			SetTimer(TIMERID_CLICK,100,NULL);	// 使用TIMER，解决程序挂死问题
		}else
		{
			//boost::mutex::scoped_lock lock(m_pList.mutex());
			//CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
			//for (;pIter!=m_pList.end();pIter++)
			//{
			//	const CMrLine::pointer& pLine = *pIter;
			//	pLine->Select(m_graphics,m_pSelectBegin,m_pSelectEnd,layoutRect);
			//}
			//lock.unlock();
			m_pSelectBegin.X=0;
			m_pSelectBegin.Y=0;
			m_pSelectEnd.X=0;
			m_pSelectEnd.Y=0;
		}
		this->SetFocus();
	}
	return 0;
}

bool CEBRichMessage::IsSingleSelected(mr::bigint* pOutMsgId) const
{
	bool result = false;
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->GetSelected())
			{
				if (result)
				{
					if (pOutMsgId != NULL)
						*pOutMsgId = 0;
					return false;
				}
				result = true;
				if (pOutMsgId != NULL)
					*pOutMsgId = pLine->GetMsgId();
			}
		}
	}
	return result;
}

bool CEBRichMessage::IsImgClicked(CMrItem::pointer& pOutImgItem) const
{
	CRect rect;
	this->GetClientRect(&rect);
	const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
	bool bInImgSelect = false;
	{
		CPoint pos;
		::GetCursorPos(&pos);
		ScreenToClient(&pos);
		const Point pPos = Point(pos.x,pos.y);

		AUTO_CONST_RLOCK(m_pList);
		CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->InImgSelect(pPos,layoutRect,pOutImgItem))
			{
				bInImgSelect = true;
				break;
			}
		}
	}

	//if (!bInSelect)
	return bInImgSelect;
}

int CEBRichMessage::IsSelected(void) const
{
	int result = 0;
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->GetSelected())
			{
				if (result)
				{
					return 2;
				}
				result = 1;
			}
		}
	}
	return result;
}

bool CEBRichMessage::IsSelected(bool bCheckMsgId) const
{
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->GetSelected())
			{
				if (bCheckMsgId)
				{
					if (pLine->GetMsgId()==0)
						continue;
				}
				return true;
			}
		}
	}
	return false;
}

void CEBRichMessage::Copy(void)
{
	if (m_pClickImgItem.get()!=NULL)
	{
		// 复制图片；
		if ( OpenClipboard() )
		{
			EmptyClipboard();

			// 复制当前选择图片到张粘板
			if (m_pClickImgItem->GetImage()!=NULL)
			{
				Gdiplus::Bitmap * pImage = const_cast<Gdiplus::Bitmap*>(m_pClickImgItem->GetImage());
				// **下面支持复制 WORD QQ 等其他程序；
				PAINTSTRUCT ps;
				HDC hDC = BeginPaint(&ps);
				HDC dcMem = ::CreateCompatibleDC(hDC);
				HBITMAP bitmap = ::CreateCompatibleBitmap(hDC,pImage->GetWidth(),pImage->GetHeight());
				SelectObject(dcMem,(HGDIOBJ)bitmap);
				Gdiplus::Graphics graphics(dcMem);
				graphics.DrawImage(pImage,0,0,pImage->GetWidth(),pImage->GetHeight());
				SetClipboardData(CF_BITMAP,bitmap);
				CloseClipboard();
				EndPaint(&ps);
				DeleteObject(bitmap);
				DeleteDC(dcMem);

				// ** 不支持复制到 WORD QQ等其他程序；
				//Gdiplus::Bitmap * pImage = const_cast<Gdiplus::Bitmap*>(m_pClickImgItem->GetImage());
				//HBITMAP pBitmap = NULL;
				//pImage->GetHBITMAP(Color(150,255,255,255),&pBitmap);
				//if (pBitmap!=NULL)
				//{
				//	SetClipboardData(CF_BITMAP,pBitmap);
				//}
				//CloseClipboard();
			}else
			{
				Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromFile(m_pClickImgItem->GetPath().c_str(),FALSE);
				if (pImage!=NULL)
				{
					// **下面支持复制 WORD QQ 等其他程序；
					PAINTSTRUCT ps;
					HDC hDC = BeginPaint(&ps);
					HDC dcMem = ::CreateCompatibleDC(hDC);
					HBITMAP bitmap = ::CreateCompatibleBitmap(hDC,pImage->GetWidth(),pImage->GetHeight());
					SelectObject(dcMem,(HGDIOBJ)bitmap);
					Gdiplus::Graphics graphics(dcMem);
					graphics.DrawImage(pImage,0,0,pImage->GetWidth(),pImage->GetHeight());
					SetClipboardData(CF_BITMAP,bitmap);
					CloseClipboard();
					EndPaint(&ps);
					DeleteObject(bitmap);
					DeleteDC(dcMem);
					delete pImage;
				}else
				{
					CloseClipboard();
				}

				// ** 不支持复制到 WORD QQ等其他程序；
				//HBITMAP pBitmap = NULL;
				//pImage->GetHBITMAP(Color(150,255,255,255),&pBitmap);
				//if (pBitmap!=NULL)
				//{
				//	SetClipboardData(CF_BITMAP,pBitmap);
				//	CloseClipboard();
				//}
				//delete pImage;
				//CloseClipboard();
			}
		}
		m_pClickImgItem.reset();
		return;
	}
	std::wstring result;
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			const std::wstring sSelectString = pLine->GetSelectString();
			if (sSelectString.empty())
				continue;
			if (!result.empty())
				result.append(L"\r\n");
			result.append(sSelectString);
		}
	}
	if (!result.empty())
	{
		if( OpenClipboard() )
		{
			EmptyClipboard();
			USES_CONVERSION;
			const std::string sText = W2A_ACP(result.c_str());
			HGLOBAL clipbuffer;
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, sText.size()+1);
			char * buffer = (char*)GlobalLock(clipbuffer);
			strcpy(buffer, sText.c_str());
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT,clipbuffer);
			CloseClipboard();
			//GlobalFree(clipbuffer);	// 不需要处理，需要会有异常
		}
		BSTR sSelectString = ::SysAllocString(result.c_str());
		this->Fire_onContextMenuCopy(sSelectString);
		::SysFreeString(sSelectString);
	}
}
void CEBRichMessage::Delete(void)
{
	std::wstring result;
	LONG nIndex = 0;
	CLockMap<mr::bigint,bool> pList;
	{
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->GetMsgId()>0 && pLine->GetSelected() && !pList.exist(pLine->GetMsgId()))
			{
				pList.insert(pLine->GetMsgId(),true);
				this->Fire_onContextMenuDelete(pLine->GetMsgId(),nIndex++);
			}
		}
	}
	if (nIndex>0)
	{
		this->Fire_onContextMenuDelete(0,nIndex);
	}
}

void CEBRichMessage::SelectAll(void)
{
	CRect rect;
	this->GetClientRect(&rect);
	const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(&ps);
	Gdiplus::Graphics m_graphics(hDC);
	{
		CPoint pos;
		::GetCursorPos(&pos);
		ScreenToClient(&pos);
		const Point pPos = Point(pos.x,pos.y);

		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			pLine->SelectAll(m_graphics,layoutRect);
		}
	}
	EndPaint(&ps);
}
int   GetCodecClsid(const   WCHAR*   format,   CLSID*   pClsid)
{
	UINT     num   =   0;                     //   number   of   image   encoders
	UINT     size   =   0;                   //   size   of   the   image   encoder   array   in   bytes

	Gdiplus::ImageCodecInfo*   pImageCodecInfo   =   NULL;

	Gdiplus::GetImageEncodersSize(&num,   &size);
	if(size   ==   0)
		return   -1;     //   Failure

	pImageCodecInfo   =   (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo   ==   NULL)
		return   -1;     //   Failure

	Gdiplus::GetImageEncoders(num,   size,   pImageCodecInfo);
	for(UINT   j   =   0;   j   <   num;   ++j)
	{
		if(   wcscmp(pImageCodecInfo[j].MimeType,   format)   ==   0   )
		{
			*pClsid   =   pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return   j;     //   Success
		}        
	}   //   for
	free(pImageCodecInfo);
	return   -1;     //   Failure

}   //   GetCodecClsid
void CEBRichMessage::SaveAs(void)
{
	TCHAR szFileName[MAX_PATH*2] = {0} ; 
	const time_t tNow = time(0);
	const struct tm *newtime = localtime(&tNow);
	if (m_pClickImgItem.get()!=NULL)
	{
		_stprintf(szFileName,L"EB图片%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);

		OPENFILENAME ofn={0};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = m_hWnd;
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = sizeof(szFileName);
		ofn.lpstrFilter = L"PNG Image (*.png)\0*.png\0JPEG Image (*.jpg)\0*.jpg\0BMP Image (*.bmp)\0*.bmp\0\0";
		ofn.lpstrDefExt = L"png";
		ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
		if (GetSaveFileName(&ofn)==TRUE ) 
		{
			std::wstring sExt(ofn.lpstrFile);
			sExt = sExt.substr(sExt.size()-3);
			std::transform(sExt.begin(), sExt.end(), sExt.begin(), tolower);
			CLSID                           codecClsid;
			if (sExt==L"png")
				GetCodecClsid(L"image/png",   &codecClsid);
			else if (sExt==L"bmp")
				GetCodecClsid(L"image/bmp",   &codecClsid);
			else
				GetCodecClsid(L"image/jpeg",   &codecClsid);

			if (m_pClickImgItem->GetImage()!=NULL)
			{
				Gdiplus::Bitmap * pImage = const_cast<Gdiplus::Bitmap*>(m_pClickImgItem->GetImage());
				pImage->Save(ofn.lpstrFile,&codecClsid);
			}else
			{
				Gdiplus::Image   image(m_pClickImgItem->GetPath().c_str());
				image.Save(ofn.lpstrFile, &codecClsid);
				//CopyFile(m_pClickImgItem->GetPath().c_str(),ofn.lpstrFile,FALSE);
			}
			m_pClickImgItem.reset();
			return;
		}
	}else
	{
		std::wstring result;
		{
			BoostReadLock rdlock(m_pList.mutex());
			CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				const std::wstring sSelectString = pLine->GetSelectString();
				if (sSelectString.empty())
					continue;
				if (!result.empty())
					result.append(L"\r\n");
				result.append(sSelectString);
			}
		}
		if (result.empty())
		{
			return;
		}
		// 
		_stprintf(szFileName,L"EB文本%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);

		OPENFILENAME ofn={0};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = m_hWnd;
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = sizeof(szFileName);
		ofn.lpstrFilter = L"文本文件(*.txt)\0\0";
		ofn.lpstrDefExt = L"txt";   
		ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
		if (GetSaveFileName(&ofn)==TRUE ) 
		{
			FILE * f = _tfopen(ofn.lpstrFile,L"w+");
			if (f!=NULL)
			{
				USES_CONVERSION;
				const std::string sText = W2A_ACP(result.c_str());
				fwrite(sText.c_str(),1,sText.size(),f);
				fclose(f);
				// open it
				ShellExecuteW(NULL, L"open", ofn.lpstrFile, NULL, ofn.lpstrFile,SW_SHOW);
			}
		}
	}
}

LRESULT CEBRichMessage::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	if ((GetKeyState(VK_CONTROL)&0x80)==0x80)
	{
		switch(  wParam  )  
		{  
		case  'A':	//0x41:  //selete  all
			SelectAll();
			break;
		case  'F':	//0x46:  //ctrl+f  
		case  'V':	//0x56:  //Ctrl  +  V:  
		case  'X':	//0x58:  //  cut  
			break;
		case  'C':	//0x43:  //  copy  
			{
				Copy();
			}break;
		case  'Z':	//0x5A:  //undo  
		case  'Y':	//0x59:  //redo  
			{
			}break;
		case 'S':
			{
				SaveAs();
			}break;
		case VK_ADD:
			{
				BOOL bHandle = TRUE;
				OnFontSize1(0,0,0,bHandle);
			}break;
		case VK_SUBTRACT:
			{
				BOOL bHandle = TRUE;
				OnFontSize2(0,0,0,bHandle);
			}break;
		default:
			break;
		}
	}

	return 0;
}

LRESULT CEBRichMessage::OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPoint pos;
	::GetCursorPos(&pos);
	ScreenToClient(&pos);
	CRect rect;
	this->GetClientRect(&rect);
	const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
	if (m_pSelectBegin.X==0)
	{
		Point pPos(pos.x,pos.y);
		{
			BoostReadLock rdlock(m_pList.mutex());
			CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				CMrItem::pointer pInPtItem;
				const int ret = pLine->OnMouseMove(pPos, layoutRect,&pInPtItem);
				if (ret==-1)	// 不在位置
					continue;
				const CMrLine::pointer pLineTemp = pLine;
				rdlock.unlock();

				if (pInPtItem.get()!=NULL && pInPtItem.get()!=m_pMoveEnterItem.get())
				{
					if (m_pMoveEnterItem.get()!=NULL && m_nSelectStringCbData1!=0 && m_pMoveEnterItem->GetSelected())
					{
						BSTR sParamString = ::SysAllocString(L"");
						this->Fire_onItemMoveLeave(pLineTemp->GetLineId(),m_pMoveEnterItem->GetItemId(),m_nSelectStringCbData1,sParamString,pLineTemp->GetMsgId());
						::SysFreeString(sParamString);
					}else if (m_pMoveEnterItem.get()!=NULL && (m_pMoveEnterItem->GetCtrlType()&MR_BTN_TYPE_MOVE_LEAVE_CB)==MR_BTN_TYPE_MOVE_LEAVE_CB)
					{
						BSTR sParamString = ::SysAllocString(m_pMoveEnterItem->GetItemParam().c_str());
						this->Fire_onItemMoveLeave(pLineTemp->GetLineId(),m_pMoveEnterItem->GetItemId(),m_pMoveEnterItem->GetItemParam2(),sParamString,pLineTemp->GetMsgId());
						::SysFreeString(sParamString);
						if (m_bEnterInFocus && ::GetFocus()!=this->m_hWnd)
						{
							SetFocus();
						}
					}
					if (m_nSelectStringCbData1 !=0 && ret==0 && pLineTemp->InSelect(pPos,layoutRect))
					{
						//std::wstring result;
						//{
						//	BoostReadLock rdlock2(m_pList.mutex());
						//	CLockList<CMrLine::pointer>::iterator pIter2 = m_pList.begin();
						//	for (;pIter2!=m_pList.end();pIter2++)
						//	{
						//		CMrLine::pointer pLine = *pIter2;
						//		const std::wstring sSelectString = pLine->GetSelectString();
						//		if (sSelectString.empty())
						//			continue;
						//		if (!result.empty())
						//			result.append(L"\r\n");
						//		result.append(sSelectString);
						//	}
						//}
						//if (!result.empty())
						{
							m_bEnterInFocus = ::GetFocus()==this->m_hWnd?true:false;
							BSTR sParamString = ::SysAllocString(L"");
							//BSTR sParamString = ::SysAllocString(result.c_str());
							CPoint posTemp(pos.x,pos.y);
							this->ClientToScreen(&posTemp);
							const int nItemId = pInPtItem->GetItemId();	// ?
							this->Fire_onItemMoveEnter(pLineTemp->GetLineId(),nItemId,m_nSelectStringCbData1,sParamString,pLineTemp->GetMsgId(),MAKELONG(posTemp.x,posTemp.y));
							::SysFreeString(sParamString);
							if (m_bEnterInFocus)
							{
								SetFocus();
							}
						}
					}else if ((pInPtItem->GetCtrlType()&MR_BTN_TYPE_MOVE_ENTER_CB)==MR_BTN_TYPE_MOVE_ENTER_CB)
					{
						m_bEnterInFocus = ::GetFocus()==this->m_hWnd?true:false;
						BSTR sParamString = ::SysAllocString(pInPtItem->GetItemParam().c_str());
						CPoint posTemp(pos.x,pos.y);
						this->ClientToScreen(&posTemp);
						this->Fire_onItemMoveEnter(pLineTemp->GetLineId(),pInPtItem->GetItemId(),pInPtItem->GetItemParam2(),sParamString,pLineTemp->GetMsgId(),MAKELONG(posTemp.x,posTemp.y));
						::SysFreeString(sParamString);
						if (m_bEnterInFocus)
						{
							SetFocus();
						}
					}
					m_pMoveEnterLine = pLineTemp;
					m_pMoveEnterItem = pInPtItem;
					KillTimer(TIMERID_CHECK_LEAVE);
					SetTimer(TIMERID_CHECK_LEAVE,1000,NULL);
				}
				if (ret==0)		// 在位置，不处理
				{
					//if (m_bIsTrackPopupMenu)
					//	SetCursor(LoadCursor(NULL,IDC_IBEAM));
					//else
					SetCursor(LoadCursor(NULL,IDC_ARROW));
					return 0;
					//break;
				}
				SetCursor(LoadCursor(NULL,IDC_HAND));
				return 0;
			}
		}
		if (m_pMoveEnterItem.get()!=NULL)
		{
			KillTimer(TIMERID_CHECK_LEAVE);
			//if (m_pMoveEnterItem->GetSelected())
			//	MessageBoxA(NULL,_T("OnSetCursor"),_T("m_pMoveEnterItem selected false"),MB_OK);
			//else 
			//	MessageBoxA(NULL,_T("OnSetCursor"),_T("m_pMoveEnterItem selected false"),MB_OK);
			if (m_nSelectStringCbData1!=0 && m_pMoveEnterItem->GetSelected())
			{
				BSTR sParamString = ::SysAllocString(L"");
				this->Fire_onItemMoveLeave(m_pMoveEnterLine->GetLineId(),m_pMoveEnterItem->GetItemId(),m_nSelectStringCbData1,sParamString,m_pMoveEnterLine->GetMsgId());
				::SysFreeString(sParamString);
			}else if ((m_pMoveEnterItem->GetCtrlType()&MR_BTN_TYPE_MOVE_LEAVE_CB)==MR_BTN_TYPE_MOVE_LEAVE_CB)
			{
				BSTR sParamString = ::SysAllocString(m_pMoveEnterItem->GetItemParam().c_str());
				this->Fire_onItemMoveLeave(m_pMoveEnterLine->GetLineId(),m_pMoveEnterItem->GetItemId(),m_pMoveEnterItem->GetItemParam2(),sParamString,m_pMoveEnterLine->GetMsgId());
				::SysFreeString(sParamString);
			}
			m_pMoveEnterItem.reset();
			m_pMoveEnterLine.reset();
			if (m_bEnterInFocus && ::GetFocus()!=this->m_hWnd)
			{
				SetFocus();
			}
		}

		SetCursor(LoadCursor(NULL,IDC_IBEAM));
		return 0;
	}
	return 0;
}
#define USES_RECEIPT_VERSION2
LRESULT CEBRichMessage::OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	//MessageBoxA(NULL,_T("menu"),_T("abc"),MB_OK);

	{
		CPoint pos;
		::GetCursorPos(&pos);
		ScreenToClient(&pos);
		const Point pPos = Point(pos.x,pos.y);
		CRect rect;
		this->GetClientRect(&rect);
		const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());

		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->PtInLine(pPos,layoutRect))
			{
				const CMrLine::pointer pLineTemp = pLine;
				rdlock.unlock();
				CMrItem::pointer pClickItem;
				pLineTemp->Onclick(pPos,layoutRect,true,&pClickItem);
				if (pClickItem.get()!=NULL && (pClickItem->GetCtrlType()&MR_BTN_TYPE_RCLICK_CB)==MR_BTN_TYPE_RCLICK_CB)
				{
					BSTR sParamString = ::SysAllocString(pClickItem->GetItemParam().c_str());
					this->Fire_onItemRBtnClick(pLineTemp->GetLineId(),pClickItem->GetItemId(),pClickItem->GetItemParam2(),sParamString,pLineTemp->GetMsgId());
					::SysFreeString(sParamString);
					return 0;
				}
			}
		}
	}

	// 消息回执
	if (m_hMenu2==NULL)
	{
		m_hMenu2 = CreatePopupMenu();
#ifdef USES_RECEIPT_VERSION2
		AppendMenu(m_hMenu2, MF_STRING, MENU_ID_RECEIPT2, L"显示所有消息回执");
		AppendMenu(m_hMenu2, MF_STRING, MENU_ID_RECEIPT1, L"只显示未接收回执");
		AppendMenu(m_hMenu2, MF_STRING, MENU_ID_RECEIPT0, L"隐藏所有消息回执");
#else
		AppendMenu(m_hMenu2, MF_STRING, MENU_ID_RECEIPT2, L"显示已接收图标");
		AppendMenu(m_hMenu2, MF_STRING, MENU_ID_RECEIPT1, L"显示未接收图标");
#endif
	}
#ifdef USES_RECEIPT_VERSION2
	CheckMenuItem(m_hMenu2, MENU_ID_RECEIPT2, MF_BYCOMMAND | (m_nMsgReceipt==RECEIPT_SHOW_ALL?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_hMenu2, MENU_ID_RECEIPT1, MF_BYCOMMAND | (m_nMsgReceipt==RECEIPT_SHOW_UNRECEIVE_ONLY?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_hMenu2, MENU_ID_RECEIPT0, MF_BYCOMMAND | (m_nMsgReceipt==RECEIPT_HIDE_ALL?MF_CHECKED:MF_UNCHECKED));
#else
	const UINT nMsgReceipt = GetPrivateProfileIntA("ebrichmessage","msg-receipt",3,m_sRichSettingPath.c_str());
	CheckMenuItem(m_hMenu2, MENU_ID_RECEIPT1, MF_BYCOMMAND | ((nMsgReceipt&1)==1?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_hMenu2, MENU_ID_RECEIPT2, MF_BYCOMMAND | ((nMsgReceipt&2)==2?MF_CHECKED:MF_UNCHECKED));
#endif
	if (m_hMenu1==NULL)
	{
		m_hMenu1 = CreatePopupMenu();
		AppendMenu(m_hMenu1, MF_POPUP, (UINT)m_hMenu2, L"消息回执");
		AppendMenu(m_hMenu1, MF_SEPARATOR, 0, L"");
		AppendMenu(m_hMenu1, MF_STRING, MENU_ID_MODE0, L"气泡模式");
		AppendMenu(m_hMenu1, MF_STRING, MENU_ID_MODE1, L"经典模式");
		AppendMenu(m_hMenu1, MF_SEPARATOR, 0, L"");
		AppendMenu(m_hMenu1, MF_STRING, MENU_ID_FONT_SIZE1, L"大字体显示\tCtrl+(+)");
		AppendMenu(m_hMenu1, MF_STRING, MENU_ID_FONT_SIZE0, L"默认大小");
		AppendMenu(m_hMenu1, MF_STRING, MENU_ID_FONT_SIZE2, L"小字体显示\tCtrl+(-)");
	}
	if (m_bDisableReceipt)
		EnableMenuItem(m_hMenu1, (UINT)m_hMenu2, MF_POPUP|MF_GRAYED);
	else
		EnableMenuItem(m_hMenu1, (UINT)m_hMenu2, MF_POPUP|MF_ENABLED);
	const UINT nClassicMode = GetPrivateProfileIntA("ebrichmessage","classic-mode",0,m_sRichSettingPath.c_str());
	CheckMenuItem(m_hMenu1, MENU_ID_MODE0, MF_BYCOMMAND | (nClassicMode==0?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_hMenu1, MENU_ID_MODE1, MF_BYCOMMAND | (nClassicMode==1?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_hMenu1, MENU_ID_FONT_SIZE0, MF_BYCOMMAND | (m_nFontSize==const_font_size?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_hMenu1, MENU_ID_FONT_SIZE1, MF_BYCOMMAND | (m_nFontSize>const_font_size?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(m_hMenu1, MENU_ID_FONT_SIZE2, MF_BYCOMMAND | (m_nFontSize<const_font_size?MF_CHECKED:MF_UNCHECKED));

	if (m_hContextMenu==NULL)
	{
		m_hContextMenu = CreatePopupMenu();
		AppendMenu(m_hContextMenu, MF_STRING, MENU_ID_COPY, L"复制\tCtrl+C");
		AppendMenu(m_hContextMenu, MF_STRING, MENU_ID_DELETE, L"删除");
		AppendMenu(m_hContextMenu, MF_STRING, MENU_ID_SAVEAS, L"另存为...\tCtrl+S");
		AppendMenu(m_hContextMenu, MF_SEPARATOR, 0, L"");
		AppendMenu(m_hContextMenu, MF_STRING, MENU_ID_SELALL, L"全选\tCtrl+A");
		AppendMenu(m_hContextMenu, MF_STRING, MENU_ID_CLEANALL, L"清屏");
		if (!m_pContextMenuItem1.empty())
		{
			AppendMenu(m_hContextMenu, MF_SEPARATOR, 0, L"");
			for (size_t i=0;i<m_pContextMenuItem1.size();i++)
			{
				CContextMenuItemInfo::pointer pMenuItemInfo = m_pContextMenuItem1[i];
				AppendMenu(m_hContextMenu, MF_STRING, MENU_ID_USER_MENU_ITEM_F1+i, pMenuItemInfo->m_sItemName.c_str());
			}
		}
		if (!m_pContextMenuItem2.empty())
		{
			AppendMenu(m_hContextMenu, MF_SEPARATOR, 0, L"");
			for (size_t i=0;i<m_pContextMenuItem2.size();i++)
			{
				CContextMenuItemInfo::pointer pMenuItemInfo = m_pContextMenuItem2[i];
				AppendMenu(m_hContextMenu, MF_STRING, MENU_ID_USER_MENU_ITEM_F2+i, pMenuItemInfo->m_sItemName.c_str());
			}
		}
		AppendMenu(m_hContextMenu, MF_SEPARATOR, 0, L"");
		//AppendMenu(m_hContextMenu, MF_POPUP, (UINT)m_hMenu2, L"消息回执");
		AppendMenu(m_hContextMenu, MF_POPUP, (UINT)m_hMenu1, L"聊天显示格式");
	}
	//if (m_bDisableReceipt)
	//	EnableMenuItem(m_hContextMenu, (UINT)m_hMenu2, MF_POPUP|MF_GRAYED);
	//else
	//	EnableMenuItem(m_hContextMenu, (UINT)m_hMenu2, MF_POPUP|MF_ENABLED);

	if (IsImgClicked(m_pClickImgItem) || IsSelected(false))
	{
		EnableMenuItem(m_hContextMenu, MENU_ID_COPY, MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(m_hContextMenu, MENU_ID_SAVEAS, MF_BYCOMMAND|MF_ENABLED);
	}else
	{
		EnableMenuItem(m_hContextMenu, MENU_ID_COPY, MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(m_hContextMenu, MENU_ID_SAVEAS, MF_BYCOMMAND|MF_GRAYED);
	}
	if (IsSelected(true))
	{
		EnableMenuItem(m_hContextMenu, MENU_ID_DELETE, MF_BYCOMMAND|MF_ENABLED);
	}else
	{
		EnableMenuItem(m_hContextMenu, MENU_ID_DELETE, MF_BYCOMMAND|MF_GRAYED);
	}
	const int nSelect = IsSelected();
	if (nSelect==1)		// 单选
	{
		for (size_t i=0;i<m_pContextMenuItem1.size();i++)
		{
			EnableMenuItem(m_hContextMenu, MENU_ID_USER_MENU_ITEM_F1+i, MF_BYCOMMAND|MF_ENABLED);
		}
		for (size_t i=0;i<m_pContextMenuItem2.size();i++)
		{
			EnableMenuItem(m_hContextMenu, MENU_ID_USER_MENU_ITEM_F2+i, MF_BYCOMMAND|MF_ENABLED);
		}
	}else if (nSelect==2)	// 多选
	{
		for (size_t i=0;i<m_pContextMenuItem1.size();i++)
		{
			EnableMenuItem(m_hContextMenu, MENU_ID_USER_MENU_ITEM_F1+i, MF_BYCOMMAND|MF_GRAYED);
		}
		for (size_t i=0;i<m_pContextMenuItem2.size();i++)
		{
			EnableMenuItem(m_hContextMenu, MENU_ID_USER_MENU_ITEM_F2+i, MF_BYCOMMAND|MF_ENABLED);
		}
	}else	// 未选
	{
		for (size_t i=0;i<m_pContextMenuItem1.size();i++)
		{
			EnableMenuItem(m_hContextMenu, MENU_ID_USER_MENU_ITEM_F1+i, MF_BYCOMMAND|MF_GRAYED);
		}
		for (size_t i=0;i<m_pContextMenuItem2.size();i++)
		{
			EnableMenuItem(m_hContextMenu, MENU_ID_USER_MENU_ITEM_F2+i, MF_BYCOMMAND|MF_GRAYED);
		}
	}
	// 全选菜单
	if (m_pList.empty())
	{
		EnableMenuItem(m_hContextMenu, MENU_ID_SELALL, MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(m_hContextMenu, MENU_ID_CLEANALL, MF_BYCOMMAND|MF_GRAYED);
	}else
	{
		EnableMenuItem(m_hContextMenu, MENU_ID_SELALL, MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(m_hContextMenu, MENU_ID_CLEANALL, MF_BYCOMMAND|MF_ENABLED);
	}
	//m_bIsTrackPopupMenu = true;
	//HCURSOR hOldCursor = SetCursor(LoadCursor(NULL,IDC_HAND));
	CPoint ptDown;
	GetCursorPos(&ptDown);
	BOOL reval=TrackPopupMenu(m_hContextMenu,TPM_TOPALIGN,ptDown.x,ptDown.y,NULL,m_hWnd,NULL);
	//BOOL reval=TrackPopupMenu(hMenu,TPM_TOPALIGN | TPM_RETURNCMD,ptDown.x,ptDown.y,NULL,m_hWnd,NULL);
	//SetCursor(hOldCursor);
	//m_bIsTrackPopupMenu = false;

	//CPoint point;
	//::GetCursorPos(&point);
	//HMENU hMenu;
	//hMenu = ::LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU_SC));
	//HMENU hSubMenu;
	//hSubMenu = ::GetSubMenu(hMenu,0);
	////EnableMenuItem(hSubMenu, ID_PEILIAO_MENU, MF_BYCOMMAND|MF_GRAYED);
	////EnableMenuItem(hSubMenu, ID__FENLEI_MENU, MF_BYCOMMAND|MF_GRAYED);
	////EnableMenuItem(hSubMenu, ID__TEXING_MENU, MF_BYCOMMAND|MF_GRAYED);
	////EnableMenuItem(hSubMenu, ID__CHENLIE_MENU, MF_BYCOMMAND|MF_GRAYED);
	////EnableMenuItem(hSubMenu, ID__TISHI_MENU, MF_BYCOMMAND|MF_GRAYED);
	//int nCmd=::TrackPopupMenu(hSubMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, 0, m_hWnd, NULL);

	return 0;
}
LRESULT CEBRichMessage::OnCopy(WORD ,WORD ,HWND , BOOL& /*bHandled*/)
{
	//MessageBoxA(NULL,"ABC","",MB_OK);
	Copy();
	return 0;
}
LRESULT CEBRichMessage::OnDelete(WORD ,WORD ,HWND , BOOL& /*bHandled*/)
{
	//MessageBoxA(NULL,"ABC","",MB_OK);
	Delete();
	return 0;
}
LRESULT CEBRichMessage::OnSelectAll(WORD ,WORD ,HWND , BOOL& /*bHandled*/)
{
	SelectAll();
	return 0;
}
LRESULT CEBRichMessage::OnCleanAll(WORD ,WORD ,HWND , BOOL& /*bHandled*/)
{
	Clear();
	//BSTR sSelectString = SysAllocString(L"");
	//Fire_onContextMenuSelect(CONTEXTMENU_SELECT_ITEMDATA_CLEAN_ALL,sSelectString,0);
	//::SysFreeString(sSelectString);
	return 0;
}
LRESULT CEBRichMessage::OnSaveAs(WORD ,WORD ,HWND , BOOL& /*bHandled*/)
{
	SaveAs();
	return 0;
}
LRESULT CEBRichMessage::OnUserMenuSelect1(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	size_t nItem = id-MENU_ID_USER_MENU_ITEM_F1;
	if (nItem>=0 && nItem<m_pContextMenuItem1.size())
	{
		CContextMenuItemInfo::pointer pMenuItemInfo = m_pContextMenuItem1[nItem];
		mr::bigint nMsgId = 0;
		if (IsSingleSelected(&nMsgId))
		{
			BSTR sSelectString = NULL;
			if (GetSelectString(&sSelectString)==S_OK)
			{
				Fire_onContextMenuSelect(pMenuItemInfo->m_nItemData,sSelectString,nMsgId);
				::SysFreeString(sSelectString);
			}
		}
	}
	return 0;
}
LRESULT CEBRichMessage::OnUserMenuSelect2(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	size_t nItem = id-MENU_ID_USER_MENU_ITEM_F2;
	if (nItem>=0 && nItem<m_pContextMenuItem2.size())
	{
		CContextMenuItemInfo::pointer pMenuItemInfo = m_pContextMenuItem2[nItem];
		BSTR sSelectString = NULL;
		if (GetSelectString(&sSelectString)==S_OK)
		{
			mr::bigint nMsgId = 0;
			IsSingleSelected(&nMsgId);
			Fire_onContextMenuSelect(pMenuItemInfo->m_nItemData,sSelectString,nMsgId);
			::SysFreeString(sSelectString);
		}
	}
	return 0;
}
LRESULT CEBRichMessage::OnMode0(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	//::MessageBoxA(NULL,m_sAppPath.c_str(),m_sRichSettingPath.c_str(),MB_OK);
	const UINT nClassicMode = GetPrivateProfileIntA("ebrichmessage","classic-mode",0,m_sRichSettingPath.c_str());
	if (nClassicMode==1)
	{
		// *转成气泡模式
		WritePrivateProfileStringA("ebrichmessage","classic-mode","0",m_sRichSettingPath.c_str());
		{
			CRect rect;
			this->GetClientRect(&rect);
			const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(&ps);
			Gdiplus::Graphics m_graphics(hDC);
			AUTO_CONST_RLOCK(m_pList);
			CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				pLine->ClearSelect(m_graphics,layoutRect);
				pLine->SetClassicMode(false);
			}
			EndPaint(&ps);
		}
		UpdateSize(VARIANT_FALSE);
		if (m_nScrollMax>0)
		{
			ScrollTo(-1);
		}else
		{
			// 没有滚动条，重新刷新一次；
			this->FireViewChange();
		}
	}
	return 0;
}
LRESULT CEBRichMessage::OnMode1(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	const UINT nClassicMode = GetPrivateProfileIntA("ebrichmessage","classic-mode",0,m_sRichSettingPath.c_str());
	if (nClassicMode==0)
	{
		// *转成经典模式
		WritePrivateProfileStringA("ebrichmessage","classic-mode","1",m_sRichSettingPath.c_str());
		{
			CRect rect;
			this->GetClientRect(&rect);
			const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(&ps);
			Gdiplus::Graphics m_graphics(hDC);
			AUTO_CONST_RLOCK(m_pList);
			CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				pLine->ClearSelect(m_graphics,layoutRect);
				pLine->SetClassicMode(true);
			}
			EndPaint(&ps);
		}
		UpdateSize(VARIANT_TRUE);
	}
	return 0;
}
LRESULT CEBRichMessage::OnFontSize0(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	if (m_nFontSize!=const_font_size)
	{
		m_nFontSize = const_font_size;
		char lpszBuffer[20];
		sprintf(lpszBuffer,"%d",m_nFontSize);
		WritePrivateProfileStringA("ebrichmessage","font-size",lpszBuffer,m_sRichSettingPath.c_str());
		{
			AUTO_CONST_RLOCK(m_pList);
			CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				pLine->SetFontSize(m_nFontSize);
			}
		}
		UpdateSize(VARIANT_TRUE);
	}

	return 0;
}
LRESULT CEBRichMessage::OnFontSize1(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	if (m_nFontSize<=26)
	{
		m_nFontSize += 1;
		char lpszBuffer[20];
		sprintf(lpszBuffer,"%d",m_nFontSize);
		WritePrivateProfileStringA("ebrichmessage","font-size",lpszBuffer,m_sRichSettingPath.c_str());
		{
			AUTO_CONST_RLOCK(m_pList);
			CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				pLine->SetFontSize(m_nFontSize);
			}
		}
		UpdateSize(VARIANT_TRUE);
	}
	return 0;
}
LRESULT CEBRichMessage::OnFontSize2(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	if (m_nFontSize>8)
	{
		m_nFontSize -= 1;
		char lpszBuffer[20];
		sprintf(lpszBuffer,"%d",m_nFontSize);
		WritePrivateProfileStringA("ebrichmessage","font-size",lpszBuffer,m_sRichSettingPath.c_str());
		{
			AUTO_CONST_RLOCK(m_pList);
			CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
			for (;pIter!=m_pList.end();pIter++)
			{
				const CMrLine::pointer& pLine = *pIter;
				pLine->SetFontSize(m_nFontSize);
			}
		}
		UpdateSize(VARIANT_TRUE);
	}
	return 0;
}
LRESULT CEBRichMessage::OnReceipt0(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
#ifdef USES_RECEIPT_VERSION2
	// 隐藏所有消息回执 0
	if (m_nMsgReceipt!=RECEIPT_HIDE_ALL)
	{
		m_nMsgReceipt = RECEIPT_HIDE_ALL;
		WritePrivateProfileStringA("ebrichmessage","msg-receipt","0",m_sRichSettingPath.c_str());
		UpdateReceiptData(m_nMsgReceipt);
	}
#endif
	return 0;
}
LRESULT CEBRichMessage::OnReceipt1(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	// 显示未接收回执 0x1
#ifdef USES_RECEIPT_VERSION2
	if (m_nMsgReceipt!=RECEIPT_SHOW_UNRECEIVE_ONLY)
	{
		m_nMsgReceipt = RECEIPT_SHOW_UNRECEIVE_ONLY;
		WritePrivateProfileStringA("ebrichmessage","msg-receipt","1",m_sRichSettingPath.c_str());
		UpdateReceiptData(m_nMsgReceipt);
	}
#else
	int nMsgReceipt = (int)GetPrivateProfileIntA("ebrichmessage","msg-receipt",3,m_sRichSettingPath.c_str());
	if ((nMsgReceipt&1)==1)
	{
		nMsgReceipt &= ~1;
	}else
	{
		nMsgReceipt |= 1;
	}
	char lpszBuf[12];
	sprintf(lpszBuf,"%d",nMsgReceipt);
	WritePrivateProfileStringA("ebrichmessage","msg-receipt",lpszBuf,m_sRichSettingPath.c_str());
	UpdateReceiptData(nMsgReceipt);
#endif
	return 0;
}
LRESULT CEBRichMessage::OnReceipt2(WORD ,WORD id,HWND , BOOL& /*bHandled*/)
{
	// 显示已接收回执 0x2
#ifdef USES_RECEIPT_VERSION2
	if (m_nMsgReceipt!=RECEIPT_SHOW_ALL)
	{
		m_nMsgReceipt = RECEIPT_SHOW_ALL;
		WritePrivateProfileStringA("ebrichmessage","msg-receipt","2",m_sRichSettingPath.c_str());
		UpdateReceiptData(m_nMsgReceipt);
	}
#else
	int nMsgReceipt = (int)GetPrivateProfileIntA("ebrichmessage","msg-receipt",3,m_sRichSettingPath.c_str());
	if ((nMsgReceipt&2)==2)
	{
		nMsgReceipt &= ~2;
	}else
	{
		nMsgReceipt |= 2;
	}
	char lpszBuf[12];
	sprintf(lpszBuf,"%d",nMsgReceipt);
	WritePrivateProfileStringA("ebrichmessage","msg-receipt",lpszBuf,m_sRichSettingPath.c_str());
	UpdateReceiptData(nMsgReceipt);
#endif
	return 0;
}
void CEBRichMessage::UpdateReceiptData(int nMsgReceipt)
{
	bool bUpdateView = false;
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			int nReceiptFlag = pLine->GetReceiptFlag();
			if ((nReceiptFlag&RECEIPT_FLAG_SHOW)==0) continue;

			if (!bUpdateView)
				bUpdateView = true;
#ifdef USES_RECEIPT_VERSION2
			UpdateReceiptData(pLine, nMsgReceipt);
#else
			if ((nReceiptFlag&RECEIPT_FLAG_TRUE)==RECEIPT_FLAG_TRUE)
			{
				// 对方已接收消息回执
				if ((nMsgReceipt&2)==2)	// 显示已接收回执
					pLine->SetShowReceiptFlag(true);
				else
					pLine->SetShowReceiptFlag(false);
			}else
			{
				// 对方未接收消息回执
				if ((nMsgReceipt&1)==1)	// 显示未接收回执
					pLine->SetShowReceiptFlag(true);
				else
					pLine->SetShowReceiptFlag(false);
			}
#endif
		}
	}
	if (bUpdateView)
	{
		this->FireViewChange();
	}
}
void CEBRichMessage::UpdateReceiptData(const CMrLine::pointer& pLine, int nMsgReceipt)
{
	const int nReceiptFlag = pLine->GetReceiptFlag();
	if ((nReceiptFlag&RECEIPT_FLAG_SHOW)==0) return;
	if (nMsgReceipt==RECEIPT_HIDE_ALL || m_bDisableReceipt)
	{
		pLine->SetShowReceiptFlag(false);
	}else if (nMsgReceipt==RECEIPT_SHOW_UNRECEIVE_ONLY)
	{
		if ((nReceiptFlag&RECEIPT_FLAG_TRUE)==RECEIPT_FLAG_TRUE)
		{
			// 对方已接收消息回执
			pLine->SetShowReceiptFlag(false);
		}else
		{
			// 对方未接收消息回执
			pLine->SetShowReceiptFlag(true);
		}
	}else		// RECEIPT_SHOW_ALL
	{
		pLine->SetShowReceiptFlag(true);
	}
}

LRESULT CEBRichMessage::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	this->GetClientRect(&rect);
	const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(&ps);
	Gdiplus::Graphics m_graphics(hDC);
	if ((GetKeyState(VK_CONTROL)&0x80)!=0x80)
	{
		// clear select
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			pLine->ClearSelect(m_graphics,layoutRect);
		}
	}
	{
		CPoint pos;
		::GetCursorPos(&pos);
		ScreenToClient(&pos);
		const Point pPos = Point(pos.x,pos.y);

		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->PtInLine(m_pSelectBegin,layoutRect))
			{
				const CMrLine::pointer pLineTemp = pLine;
				rdlock.unlock();
				CMrItem::pointer pClickItem;
				pLineTemp->OnDblClick(m_graphics,pPos,layoutRect,&pClickItem);
				if (pClickItem.get()!=NULL && (pClickItem->GetCtrlType()&MR_BTN_TYPE_LDBLCLICK_CB)==MR_BTN_TYPE_LDBLCLICK_CB)
				{
					BSTR sParamString = ::SysAllocString(pClickItem->GetItemParam().c_str());
					this->Fire_onItemLBtnDblClick(pLineTemp->GetLineId(),pClickItem->GetItemId(),pClickItem->GetItemParam2(),sParamString,pLineTemp->GetMsgId());
					::SysFreeString(sParamString);
				}
				break;
			}
		}
	}
	EndPaint(&ps);
	return 0;
}

LRESULT CEBRichMessage::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	this->GetClientRect(&rect);
	const RectF layoutRect(0,m_nScrollPos,rect.Width(),rect.Height());
	bool bInSelect = false;
	{
		CPoint pos;
		::GetCursorPos(&pos);
		ScreenToClient(&pos);
		const Point pPos = Point(pos.x,pos.y);

		AUTO_RLOCK(m_pList);
		CLockList<CMrLine::pointer>::const_iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			if (pLine->InSelect(pPos,layoutRect))
			{
				bInSelect = true;
				break;
			}
		}
	}

	if (!bInSelect)
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(&ps);
		Gdiplus::Graphics m_graphics(hDC);
		// clear select
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CMrLine::pointer>::iterator pIter = m_pList.begin();
		for (;pIter!=m_pList.end();pIter++)
		{
			const CMrLine::pointer& pLine = *pIter;
			pLine->ClearSelect(m_graphics,layoutRect);
		}
		EndPaint(&ps);
	}

	return 0;
}
//LRESULT CEBRichMessage::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//{
//	// TODO: Add your message handler code here and/or call default
//	if (m_pMoveEnterItem.get()!=NULL && (m_pMoveEnterItem->GetCtrlType()&MR_BTN_TYPE_MOVE_LEAVE_CB)==MR_BTN_TYPE_MOVE_LEAVE_CB)
//	{
//		KillTimer(TIMERID_CHECK_LEAVE);
//		BSTR sParamString = ::SysAllocString(m_pMoveEnterItem->GetItemParam().c_str());
//		this->Fire_onItemMoveLeave(m_pMoveEnterItem->GetItemParam2(),sParamString,m_pMoveEnterLine->GetMsgId());
//		::SysFreeString(sParamString);
//		m_pMoveEnterItem.reset();
//		m_pMoveEnterLine.reset();
//	}
//	return 0;
//}

//CString theTooptipText("abc");
//BOOL CEBRichMessage::OnTipNotify(int /*wParam*/, LPNMHDR pNMHDR, BOOL& bHandled)
//{
//	TOOLTIPTEXT*pTTT=(TOOLTIPTEXT*)pNMHDR;
//	pTTT->lpszText=(LPTSTR)(LPCTSTR)theTooptipText;
//	//UINT uID=pNMHDR->idFrom;
//	//if(pTTT->uFlags&TTF_IDISHWND)
//	//	uID=::GetDlgCtrlID((HWND)uID);
//	//if(uID==NULL) return FALSE;
//	//CString sToolTip=m_pToolTip[uID];
//	//if (sToolTip.IsEmpty()) return FALSE;
//	//pTTT->lpszText=(LPTSTR)(LPCTSTR)sToolTip;
//	bHandled = TRUE;
//	return TRUE;
//}
