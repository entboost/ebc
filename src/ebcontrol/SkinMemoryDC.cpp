#include "stdafx.h"
#include "SkinMemoryDC.h"
CSkinMemoryDC::CSkinMemoryDC(HDC pDC, const RECT& rcBounds)
: m_pOwnerDC(NULL)
{
	m_pOwnerDC = CreateCompatibleDC(pDC);
	if (m_pOwnerDC != NULL)
	{
		m_rcBounds = rcBounds;
		m_bitmap = CreateCompatibleBitmap(pDC, Width(), Height());
		m_oldBitmap = (HBITMAP)SelectObject(m_pOwnerDC,m_bitmap);
		m_pDC = pDC;
	}else
	{
		m_bitmap = NULL;
		m_oldBitmap = NULL;
		m_pDC = NULL;
	}
}
CSkinMemoryDC::~CSkinMemoryDC(void)
{
	if (m_pDC != NULL)
	{
		BitBlt(m_pDC,m_rcBounds.left, m_rcBounds.top, Width(), Height(), m_pOwnerDC, m_rcBounds.left, m_rcBounds.top, SRCCOPY);
		SelectObject(m_pOwnerDC,m_oldBitmap);
	}
	if (m_bitmap != NULL)
	{
		DeleteObject(m_bitmap);
		m_bitmap = NULL;
	}
	if (m_pOwnerDC)
	{
		DeleteDC(m_pOwnerDC);
		m_pOwnerDC = NULL;
	}
}
