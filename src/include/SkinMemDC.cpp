#include "stdafx.h"
#include "SkinMemDC.h"
CSkinMemDC::CSkinMemDC(CDC* pDC, const CRect& rcBounds)
: CDC()
{
	if (CreateCompatibleDC(pDC))
	{
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.Width(), rcBounds.Height());
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rcBounds = rcBounds;
	}else
	{
		m_bitmap = NULL;
		m_oldBitmap = NULL;
		m_pDC = NULL;
	}
}
CSkinMemDC::~CSkinMemDC(void)
{
	if (m_pDC)
	{
		m_pDC->BitBlt(m_rcBounds.left, m_rcBounds.top, m_rcBounds.Width(), m_rcBounds.Height(), 
			this, m_rcBounds.left, m_rcBounds.top, SRCCOPY);
		SelectObject(m_oldBitmap);
	}
	if (m_bitmap != NULL)
	{
		m_bitmap->DeleteObject();
		delete m_bitmap;
	}
}
