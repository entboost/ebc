#include "StdAfx.h"
#include "MrDrawItem.h"

bool PtInRect(const Point& pos, const RectF& rect)
{
	CRect rect2(rect.GetLeft(),rect.GetTop(),(int)rect.GetLeft()+(int)rect.Width,(int)rect.GetTop()+(int)rect.Height);
	return rect2.PtInRect(CPoint(pos.X,pos.Y))?true:false;
}
CRect Rect2Rect(const RectF& rect)
{
	return CRect(rect.GetLeft(),rect.GetTop(),rect.GetRight(),rect.GetBottom());
}

CMrDrawItem::CMrDrawItem(MR_ITEM_TYPE nItemType, HWND pParent)
: m_nItemType(nItemType), m_pParent(pParent)
, m_nSelectStart(-1)
, m_nSelectEnd(-1)

{
}

CMrDrawItem::~CMrDrawItem(void)
{
}

//void CMrDrawItem::Move2Left(const RectF& rect, int maxwidth)
//{
//	if (rect.Width > m_rect.Width && rect.GetRight() > m_rect.GetRight())
//	{
//		m_rect = RectF(m_rect.GetLeft()+(std::max<int>(0,(rect.Width-maxwidth))),m_rect.GetTop(),m_rect.Width,m_rect.Height);
//	}
//}
void CMrDrawItem::Move2Right(const RectF& rect, int maxwidth)
{
	if (rect.Width > m_rect.Width && rect.GetRight() > m_rect.GetRight())
	{
		m_rect = RectF(m_rect.GetLeft()+(std::max<int>(0,(rect.Width-maxwidth))),m_rect.GetTop(),m_rect.Width,m_rect.Height);
	}
}

//bool CMrDrawItem::DblClick(Graphics& pGraphics, const Point& pPoint)
//{
//
//	return false;
//}

bool CMrDrawItem::GetSelected(void) const
{
	if (m_nSelectStart>=0 && m_nSelectEnd>m_nSelectStart && m_nSelectEnd<=m_sText.size())
	{
		return true;
	}else
	{
		return false;
	}
}
std::wstring CMrDrawItem::GetSelectString(void) const
{
	if (m_nSelectStart>=0 && m_nSelectEnd>m_nSelectStart && m_nSelectEnd<=m_sText.size())
	{
		return m_sText.substr(m_nSelectStart,m_nSelectEnd-m_nSelectStart);
	}else
	{
		return L"";
	}
}

bool CMrDrawItem::Onclick(const Point& pPos, const RectF& layoutRect)
{
	Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
	const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_rect.Width,m_rect.Height);
	if (PtInRect(pMovePos,rectFullLine))
	{
		//if (m_nItemType==MR_ITEM_TYPE_URL && !m_sPath.empty())
		//{
		//	// open
		//	ShellExecuteW(NULL, L"open" , m_sPath.c_str(), NULL, NULL,SW_SHOW);
		//}
		return true;
	}
	return false;
}
//bool CMrDrawItem::OnDblclick(const Point& pPos, const RectF& layoutRect)
//{
//	Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
//	const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_rect.Width,m_rect.Height);
//	if (PtInRect(pMovePos,rectFullLine))
//	{
//		return true;
//	}
//	return false;
//}
bool CMrDrawItem::OnMouseMove(const Point& pPos, const RectF& layoutRect)
{
	Point pMovePos = Point(pPos.X,pPos.Y+layoutRect.GetTop());
	const RectF rectFullLine(m_rect.GetLeft(),m_rect.GetTop(),m_rect.Width,m_rect.Height);
	if (PtInRect(pMovePos,rectFullLine))
	{
		switch(m_nItemType)
		{
		//case MR_ITEM_TYPE_DBLCLICK_TEXT:
		case MR_ITEM_TYPE_CLICK_TEXT:
		case MR_ITEM_TYPE_WAV:
		case MR_ITEM_TYPE_URL:
		case MR_ITEM_TYPE_OPENFILE:
		case MR_ITEM_TYPE_OPENDIR:
			return true;
		default:
			{
				return GetSelected();
			}break;
		}
	}
	return false;
}

bool CMrDrawItem::ClearSelect(void)
{
	//bool result = m_rectSelect.IsEmptyArea()?false:true;
	bool result = m_rectSelect.IsEmptyArea()?false:true;
	m_nSelectStart = -1;
	m_nSelectEnd = -1;
	m_rectSelect = RectF();
	m_rectOldSelect = RectF(); 
	//pDrawItem->InvalidateRect(layoutRect);
	return result;
}
void CMrDrawItem::SelectAll(const RectF& layoutRect)
{
	m_nSelectStart = 0;
	m_nSelectEnd = m_sText.size();
	m_rectSelect = m_rect;
}
bool CMrDrawItem::SelectAll(const Point& pPos,const RectF& layoutRect)
{
	Point pSelectStart = Point(pPos.X,pPos.Y+layoutRect.GetTop());
	const RectF rectFullLine(m_rect.GetLeft()-10,m_rect.GetTop(),m_rect.Width+20,m_rect.Height);
	if (PtInRect(pSelectStart,rectFullLine))
	{
		m_nSelectStart = 0;
		m_nSelectEnd = m_sText.size();
		m_rectSelect = m_rect;
		return true;
	}
	return false;
}
bool CMrDrawItem::InSelect(const Point& pPos,const RectF& layoutRect) const
{
	if (!GetSelected()) return false;//m_nSelectStart==-1 || m_nSelectEnd==-1) return false;
	Point pSelectStart = Point(pPos.X,pPos.Y+layoutRect.GetTop());
	const RectF rectFullLine(m_rectSelect.GetLeft()-10,m_rect.GetTop(),m_rectSelect.Width+20,m_rect.Height);
	//const RectF rectFullLine(m_rect.GetLeft()-10,m_rect.GetTop(),m_rect.Width+20,m_rect.Height);
	if (PtInRect(pSelectStart,rectFullLine))
	{
		return true;
	}
	return false;
}

//void GetTextBounds(const Font& font,const StringFormat* strFormat,const std::wstring& szText,const PointF & point,RectF& pOutBound)
//{
//    GraphicsPath path;
//    FontFamily fontfamily;
//    font.GetFamily(&fontfamily);
//    path.AddString(szText.c_str(),-1,&fontfamily,font.GetStyle(),font.GetSize(),point,strFormat);
//    path.GetBounds(&pOutBound);
//    //return SizeF(rcBound.Width,rcBound.Height);
//}

bool CMrDrawItem::Select(Graphics& pGraphics, const Gdiplus::Font* pFont, const Point& pSelectStart1, const Point& pSelectEnd1, const RectF& layoutRect)
{
	Point pSelectStart = Point(pSelectStart1.X,pSelectStart1.Y+layoutRect.GetTop());
	Point pSelectEnd = Point(pSelectEnd1.X,pSelectEnd1.Y+layoutRect.GetTop());
	const RectF rectFullLine1(layoutRect.GetLeft()-10,m_rect.GetTop(),layoutRect.Width+10,m_rect.Height);	// 全行
	const RectF rectFullLine2(m_rect.GetLeft()-10,m_rect.GetTop(),m_rect.Width+20,m_rect.Height);			// 文本行
	bool bChangeStartEndPos = false;
	if (PtInRect(pSelectStart,rectFullLine1) && PtInRect(pSelectEnd,rectFullLine1))
	{
		// 二点在本行
		if (pSelectStart.X < pSelectEnd.X)
		{
			//pSelectStart = pSelectStart1;
			//pSelectEnd = pSelectEnd1;
		}else
		{
			// 交换开始、结束点
			pSelectStart = Point(pSelectEnd1.X,pSelectEnd1.Y+layoutRect.GetTop());;
			pSelectEnd = Point(pSelectStart1.X,pSelectStart1.Y+layoutRect.GetTop());;
			bChangeStartEndPos = true;
		}
	}else if (PtInRect(pSelectStart,rectFullLine1))
	{
		// 开始点在本行
		if (pSelectEnd.Y<m_rect.GetTop())
		{
			// 交换开始、结束点
			pSelectStart = Point(pSelectEnd1.X,pSelectEnd1.Y+layoutRect.GetTop());;
			pSelectEnd = Point(pSelectStart1.X,pSelectStart1.Y+layoutRect.GetTop());;
			bChangeStartEndPos = true;
		}else
		{
			//pSelectStart = pSelectStart1;
			//pSelectEnd = pSelectEnd1;
		}
	}else if (PtInRect(pSelectEnd,rectFullLine1))
	{
		// 结束点在本行
		if (pSelectStart.Y<m_rect.GetTop())
		{
			//pSelectStart = pSelectStart1;
			//pSelectEnd = pSelectEnd1;
		}else
		{
			// 交换开始、结束点
			pSelectStart = Point(pSelectEnd1.X,pSelectEnd1.Y+layoutRect.GetTop());;
			pSelectEnd = Point(pSelectStart1.X,pSelectStart1.Y+layoutRect.GetTop());;
			bChangeStartEndPos = true;
		}
	}else if ((pSelectStart.Y<m_rect.GetTop()&&pSelectEnd.Y>m_rect.GetBottom()) ||
		(pSelectEnd.Y<m_rect.GetTop()&&pSelectStart.Y>m_rect.GetBottom()))
	{
		// 二点都不在本行
		// 一上，一下，取整行
		//if (m_nSelectStart==0 && m_nSelectEnd==m_sText.size())
		//{
		//	// 之前已经取整行，不需要刷新界面
		//	return false;
		//}
		m_nSelectStart = 0;
		m_nSelectEnd = m_sText.size();
		m_rectSelect = m_rect;
		//m_rectOldSelect = m_rect;
		return true;
	}else
	{
		const bool result = m_rectSelect.IsEmptyArea()?false:true;
		if ((GetKeyState(VK_CONTROL)&0x80)!=0x80)
		{
			m_nSelectStart = -1;
			m_nSelectEnd = -1;
			m_rectSelect = RectF();
			m_rectOldSelect = RectF();
		}
		return result;
	}

	/*
	if (!PtInRect(pSelectStart,GetRect()))
	{
		// 判断是否整行选择
		if (pSelectStart.X > m_rect.GetLeft() && pSelectStart.X < m_rect.GetRight()			// 开始点在行中间位置
			&& pSelectEnd.X > m_rect.GetLeft() && pSelectEnd.X < m_rect.GetRight()			// 结束点在行中间位置
			&& ((pSelectStart.Y<m_rect.GetTop()&&pSelectEnd.Y>m_rect.GetBottom()) || (pSelectEnd.Y<m_rect.GetTop()&&pSelectStart.Y>m_rect.GetBottom()))) // 开始，结束二点在上下位置	
		{
			m_nSelectStart = 0;
			m_nSelectEnd = m_sText.size();
			m_rectSelect = m_rect;
			m_rectOldSelect = m_rect;
			return true;
		}

		m_nSelectStart = -1;
		m_nSelectEnd = -1;
		m_rectSelect = RectF();
		m_rectOldSelect = RectF(); 
		return false;
	}
	*/
	//const Gdiplus::Font font(L"Timers New Roman",10);
	//const FontFamily fontFamily(theFontFamily.c_str());
	//const Gdiplus::Font font(&fontFamily, 13, FontStyleRegular, UnitPixel);
	//Gdiplus::StringFormat format;
	//format.SetAlignment(Gdiplus::StringAlignmentNear);
	//format.SetLineAlignment(Gdiplus::StringAlignmentNear);
	//format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
	//Gdiplus::StringFormat * format = (Gdiplus::StringFormat*)StringFormat::GenericTypographic();
	//format->SetAlignment(Gdiplus::StringAlignmentNear);
	//format->SetLineAlignment(Gdiplus::StringAlignmentNear);
	////format->SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

	//RectF stringRectTemp;
	RectF stringRect;
	m_rectOldSelect = m_rectSelect;
	RectF selectRectTemp = RectF(m_rect.GetLeft(),m_rect.GetTop()-1,1,m_rect.Height);
	//RectF selectRectTemp = RectF(m_rect.GetLeft()+3,m_rect.GetTop()-1,1,m_rect.Height);

	m_nSelectStart = -1;
	m_nSelectEnd = -1;
	if (PtInRect(pSelectStart,rectFullLine2))
	{
		// 开始点在本行位置
		m_nSelectStart = 0;
		for (size_t j=1;j<=m_sText.size();j++)
		{
			//GetTextExtentPoint32(
			pGraphics.MeasureString(m_sText.c_str(), j, pFont, PointF(m_rect.GetLeft(),m_rect.GetTop()),StringFormat::GenericTypographic(), &stringRect);
			//GetTextBounds(font,format,m_sText.substr(0,j),PointF(m_rect.GetLeft(),m_rect.GetTop()),stringRect);
			if (PtInRect(Point(pSelectStart.X-4,pSelectStart.Y),stringRect))
			//if (PtInRect(pSelectStart,stringRect) && !PtInRect(Point(pSelectStart.X+8,pSelectStart.Y),stringRect))	// +8用于判断下一个字符
			{
				//REAL nlastlen = stringRectTemp.IsEmptyArea()?(stringRect.Width-6):(stringRect.GetRight()-stringRectTemp.GetRight());			// 开始选择前一字符长度
				REAL nleft = stringRect.GetRight();
				selectRectTemp = RectF(nleft,m_rect.GetTop()-1,1,m_rect.Height);		// 记录前一个选择，最右边
				m_nSelectStart = j;
				break;
			}
			//stringRectTemp = stringRect;
		}
	}else if (PtInRect(pSelectEnd,rectFullLine1))
	{
		// 开始点不在本行，结束点在本行
		// 选择本行部分内容
		m_nSelectStart = 0;
		selectRectTemp = RectF(m_rect.GetLeft(),m_rect.GetTop()-1,1,m_rect.Height);
	//}else
	//{
	//	m_nSelectStart = -1;
	//	m_nSelectEnd = -1;
	//	m_rectSelect = RectF();
	//	m_rectOldSelect = RectF(); 
	//	return false;
	}

	if (PtInRect(pSelectEnd,rectFullLine1))
	{
		// 结束点在本行
		if ((PtInRect(pSelectStart,rectFullLine2) && pSelectEnd.X>m_rect.GetRight()-3) ||
			(!PtInRect(pSelectEnd,rectFullLine2) && pSelectStart.X>m_rect.GetRight() && pSelectStart.Y<m_rect.GetTop()-3) ||
			(!PtInRect(pSelectEnd,rectFullLine2) && pSelectEnd.X>m_rect.GetRight()-3 && pSelectStart.Y<m_rect.GetTop()-3))	// 2016-05-13
		//if (pSelectEnd.X>m_rect.GetRight()-3)
		{
			// 已经选择最右边全部
			//if (m_nSelectEnd==m_sText.size())
			//{
			//	// 之前相同，不需要刷新界面
			//	return false;
			//}
			m_nSelectEnd = m_sText.size();
			m_rectSelect = RectF(selectRectTemp.GetLeft(),selectRectTemp.GetTop(),m_rect.GetRight()-selectRectTemp.GetRight(),m_rect.Height);
			//m_rectOldSelect = m_rectSelect;
			return true;
		}else
		{
			////行间距计算  
			//REAL linespace = fontFamily.GetLineSpacing(FontStyleRegular);  
			//REAL ascent = fontFamily.GetCellAscent(FontStyleRegular);  
			//REAL descent    = fontFamily.GetCellDescent(FontStyleRegular);  
			//REAL emheight   = fontFamily.GetEmHeight(FontStyleRegular);  

			//REAL    ftheight = font.GetHeight(&pGraphics);  
			//REAL    ftsize   = font.GetSize();  

			//INT16 ascentp   = ftsize * ascent / emheight;  
			//INT16 descentp  = ftsize * descent / emheight;  
			//INT16 linespacep = ftsize * linespace / emheight;  
			//INT16 gapline = linespacep - ascentp - descentp;  

			for (size_t j=m_nSelectStart;j<=m_sText.size();j++)
			{
				pGraphics.MeasureString(m_sText.c_str(), j, pFont, PointF(m_rect.GetLeft(),m_rect.GetTop()),StringFormat::GenericTypographic(),&stringRect);
				//GetTextBounds(font,format,m_sText.substr(0,j),PointF(m_rect.GetLeft(),m_rect.GetTop()),stringRect);
				if ((!bChangeStartEndPos && PtInRect(pSelectEnd,stringRect)) ||
					(bChangeStartEndPos && PtInRect(Point(pSelectEnd.X-4,pSelectEnd.Y),stringRect)))
				//if (PtInRect(pSelectEnd,stringRect) && !PtInRect(Point(pSelectEnd.X+8,pSelectEnd.Y),stringRect))	// +8用于判断下一个字符
				{
					m_nSelectEnd = j;
					if (m_nSelectEnd==m_nSelectStart)
					{
						m_nSelectStart = -1;
						m_nSelectEnd = -1;
						m_rectSelect = RectF();//selectRectTemp;
					}else
					{
						m_rectSelect = RectF(selectRectTemp.GetLeft(),selectRectTemp.GetTop(),stringRect.GetRight()-selectRectTemp.GetLeft(),selectRectTemp.Height);
						//m_rectSelect = RectF(selectRectTemp.GetLeft(),selectRectTemp.GetTop(),stringRect.GetRight()-selectRectTemp.GetLeft()-3.0,selectRectTemp.Height);
					}
					return true;
				}
			}
		}
	}else if (pSelectEnd.Y<m_rect.GetTop())
	{
		// 选择前面内容
		//if (m_nSelectEnd == m_nSelectStart)
		//{
		//	// 之前相同，不需要重新刷新界面
		//	return false;
		//}
		m_nSelectEnd = m_nSelectStart;
		m_nSelectStart = 0;
		m_rectSelect = RectF(m_rect.GetLeft(),m_rect.GetTop(),selectRectTemp.GetLeft()-m_rect.GetLeft(),m_rect.Height);
		m_rectOldSelect = m_rectSelect;
		return true;
	}else if (pSelectStart.X < m_rect.GetRight()-3 && (pSelectEnd.Y>m_rect.GetBottom() ||
		(pSelectEnd.X>m_rect.GetRight()+3 && PtInRect(pSelectEnd,rectFullLine1))))
	//}else if (pSelectStart.X < m_rect.GetRight()-3 && pSelectEnd.Y>m_rect.GetBottom())
	{
		// 选择后面内容
		//if (m_nSelectEnd == m_sText.size())
		//{
		//	// 之前相同，不需要重新刷新界面
		//	return false;
		//}
		if (m_nSelectStart<0)
			m_nSelectStart = 0;
		m_nSelectEnd = m_sText.size();
		m_rectSelect = RectF(selectRectTemp.GetLeft(),selectRectTemp.GetTop(),m_rect.GetRight()-selectRectTemp.GetRight(),m_rect.Height);
		//m_rectOldSelect = m_rectSelect;
		return true;
	}
	// 2016-05-13 add
	if (pSelectStart.X<=(rectFullLine2.GetLeft()+rectFullLine2.Width/2) && PtInRect(pSelectStart,rectFullLine1) &&
		pSelectEnd.X>=(rectFullLine2.GetRight()-rectFullLine2.Width/2) && PtInRect(pSelectEnd,rectFullLine1))
	{
		// 二个点在左右二侧，选择全行
		m_nSelectStart = 0;
		m_nSelectEnd = m_sText.size();
		m_rectSelect = m_rect;
		return true;
	}

	if (m_nSelectEnd==-1 && m_rectSelect.Width>0)	// 没有选到，之前已经选择，重新刷新
	{
		m_nSelectStart = -1;
		m_rectSelect = RectF();//selectRectTemp;
		return true;
	}
	return false;
	return true;
}

//void CMrDrawItem::Select(int nStart, int nEnd)
//{
//	if (nStart>=0 && nStart<m_sText.size())
//		m_nSelectStart = nStart;
//	if (nEnd == -1)
//		m_nSelectEnd = m_sText.size()-1;
//	else if (nEnd>nStart && nEnd<m_sText.size())
//		m_nSelectEnd = nEnd;
//}

bool CMrDrawItem::Draw(Graphics& pGraphics, const Gdiplus::Font* pFont, const Brush* pBrush, const RectF& layoutRect)
{
	//if (!m_rectSelect.IsEmptyArea() && m_rectSelect.Width==m_rectOldSelect.Width)
	//{
	//	// 没有改变
	//	return false;
	//}
	pGraphics.DrawString(m_sText.c_str(),-1,pFont,PointF(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop()),StringFormat::GenericTypographic(),pBrush);
	//Gdiplus::StringFormat * format = (Gdiplus::StringFormat*)StringFormat::GenericTypographic();
	////format->SetAlignment(Gdiplus::StringAlignmentNear);
	////format->SetLineAlignment(Gdiplus::StringAlignmentNear);
	////format->SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
	//pGraphics.DrawString(m_sText.c_str(),-1,pFont,PointF(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop()),format,pBrush);
	//pGraphics.DrawString(m_sText.c_str(),-1,pFont,PointF(m_rect.GetLeft(),m_rect.GetTop()-layoutRect.GetTop()),pBrush);
	if (!m_rectSelect.IsEmptyArea())
	{
		// 过来透明背景
		Gdiplus::SolidBrush brushSelectBack(Color(150,0,128,255));
		RectF rectSelect(m_rectSelect.GetLeft(),m_rectSelect.GetTop()-layoutRect.GetTop(),m_rectSelect.Width,m_rectSelect.Height+1);
		pGraphics.FillRectangle(&brushSelectBack,rectSelect);
	}
	return true;

	////pGraphics->DrawString(m_sText.c_str(),-1,pFont,PointF(m_rect.GetLeft(),m_rect.GetTop()),pBrush);
	////pGraphics->DrawString(m_sText.c_str(),-1,pFont,GetRect(),pStringFormat,pBrush);
	//if (m_nSelectStart >= 0)
	//{
	//	//Gdiplus::Font fontWrite(L"Timers New Roman",10);
	//	Gdiplus::SolidBrush brush(Gdiplus::Color::LightGray);
	//	Gdiplus::SolidBrush brushSelectBack(Color(128,0,0,200));
	//	//HatchBrush brushSelectBack(HatchStyleBackwardDiagonal,Gdiplus::Color::Blue);
	//	Gdiplus::SolidBrush brushSelectString(Gdiplus::Color::White);

	//	// 计算未选择，选择，几个显示区域
	//	if (m_nSelectStart>0)
	//	{
	//		pGraphics->FillRectangle(&brushSelectBack,m_rectSelect);
	//		pGraphics->DrawString(m_sText.c_str(),-1,pFont,PointF(m_rect.GetLeft(),m_rect.GetTop()),pBrush);
	//		//pGraphics->DrawString(m_sText.substr(m_nSelectStart).c_str(),m_nSelectEnd-m_nSelectStart,pFont,PointF(m_rectSelect.GetLeft()-1,m_rectSelect.GetTop()),&brushSelectString);
	//		return true;
	//		pGraphics->DrawString(m_sText.c_str(),m_nSelectStart,pFont,PointF(m_rect.GetLeft(),m_rect.GetTop()),pBrush);
	//		pGraphics->Flush();
	//		pGraphics->FillRectangle(&brushSelectBack,m_rectSelect);
	//		pGraphics->DrawString(m_sText.substr(m_nSelectStart).c_str(),m_nSelectEnd-m_nSelectStart,pFont,PointF(m_rectSelect.GetLeft()-1,m_rectSelect.GetTop()),&brushSelectString);
	//		pGraphics->DrawString(m_sText.substr(m_nSelectEnd).c_str(),-1,pFont,PointF(m_rectSelect.GetRight(),m_rectSelect.GetTop()),pBrush);
	//		pGraphics->Flush();
	//	}else
	//	{

	//	}
	//}else
	//{
	//	pGraphics->DrawString(m_sText.c_str(),-1,pFont,PointF(m_rect.GetLeft(),m_rect.GetTop()),pBrush);
	//	//pGraphics->DrawString(m_sText.c_str(),-1,pFont,GetRect(),pStringFormat,pBrush);
	//}
	//return true;
}

void CMrDrawItem::InvalidateRect(const RectF& layoutRect)
{
	if (this->m_pParent)
	{
		//if (false)
		if (!m_rectSelect.IsEmptyArea())
		{
			//m_pParent->InvalidateRect(&RectF2Rect(m_rectOldSelect));
			//m_pParent->InvalidateRect(&RectF2Rect(m_rectSelect));
			//return;

			if (m_rectOldSelect.IsEmptyArea() ||
				(PtInRect(Point(m_rectOldSelect.GetLeft(),m_rectOldSelect.GetTop()),m_rectSelect) &&
				PtInRect(Point(m_rectOldSelect.GetRight()-1,m_rectOldSelect.GetBottom()-1),m_rectSelect)))
				//m_rectSelect.Width>=m_rectOldSelect.Width)
			{
				CRect rect(m_rectSelect.GetLeft(),m_rectSelect.GetTop()-layoutRect.GetTop(),m_rectSelect.GetRight(),m_rectSelect.GetBottom()-layoutRect.GetTop());
				::InvalidateRect(m_pParent,&rect,FALSE);
			}else
			{
				CRect rect(m_rect.GetLeft()-1,m_rect.GetTop()-1-layoutRect.GetTop(),m_rect.GetRight()+1,m_rect.GetBottom()+1);
				::InvalidateRect(m_pParent,&rect,FALSE);
				// 显示部分，但移动太快，会有问题，以后再优化
				//RectF rectUnion;
				//RectF::Union(rectUnion,m_rectOldSelect,m_rectSelect);
				//CRect rect(rectUnion.GetLeft()-1,rectUnion.GetTop()-1,rectUnion.GetRight()+1,rectUnion.GetBottom()+1);
				//m_pParent->InvalidateRect(&rect);
			}
		}else
		{
			CRect rect(m_rect.GetLeft()-1,m_rect.GetTop()-1-layoutRect.GetTop(),m_rect.GetRight()+1,m_rect.GetBottom()+1);
			::InvalidateRect(m_pParent,&rect,FALSE);
		}
	}
}
