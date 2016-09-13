// ProgressST.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressST

CProgressST::CProgressST()
{
	// Default range of the control
	m_nLower = 0;
	m_nUpper = 100;
	CalcRange();

	// Default position
	m_nPos = 0;

	// Default step
	m_nStep = 10;
} // End of CProgressST


CProgressST::~CProgressST()
{
} // End of ~CProgressST


BEGIN_MESSAGE_MAP(CProgressST, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressST)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressST message handlers

void CProgressST::OnPaint() 
{
	PAINTSTRUCT lpPaint;

	// If there is NO bitmap loaded
	if (m_bmPattern.m_hObject == NULL) 
	{
		CProgressCtrl::OnPaint();
		return;
	}

	BeginPaint(&lpPaint);

	CWindowDC dc(this);

	DrawProgress(&dc);

	EndPaint(&lpPaint);
} // End of OnPaint


BOOL CProgressST::SetBitmap(UINT nBitmapId, BOOL bRepaint)
{
	BITMAP bm;
	BOOL bRet;

	// Detach any previuos bitmap
	m_bmPattern.Detach();

	// Default return value
	bRet = TRUE;

	// Load new bitmap
	if (nBitmapId != NULL)
	{
		bRet = GetBitmapAndPalette(nBitmapId, m_bmPattern, m_Palette);
		// If all ok
		if (bRet == TRUE)
		{
			// Get dimension
			m_bmPattern.GetBitmap(&bm);
			// Width of the bitmap
			m_nWidth = bm.bmWidth;
			// Height of the bitmap
			m_nHeight = bm.bmHeight;
		}
	}

	// Repaint control
	if (bRepaint == TRUE) Invalidate();

	return bRet;
} // End of SetBitmap


void CProgressST::DrawProgress(CDC * pDC)
{
	CRect ctrlRect;

    CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	// Select bitmap
	CBitmap* pOldBitmap = memDC.SelectObject(&m_bmPattern);

	GetClientRect(ctrlRect);

	// Create temporary DC & bitmap
	CDC tempDC;
	tempDC.CreateCompatibleDC(pDC);

	CBitmap bitmapTemp;
	bitmapTemp.CreateCompatibleBitmap(&memDC, ctrlRect.Width(), ctrlRect.Height());
	CBitmap* pOldTempBitmap = tempDC.SelectObject(&bitmapTemp);

	// Calculate control's percentage to draw
	int nPercentage;
	nPercentage = (int)((100.0/m_nRange)*(abs(m_nLower)+m_nPos));

	// Adjust rectangle to draw on screen
	float f = ((float)(ctrlRect.Width())/100)*nPercentage;
	if ((ctrlRect.left + (int)f) > ctrlRect.right)
	{
		ctrlRect.right -= 1;
	}
	else
	{
		ctrlRect.right = ctrlRect.left + (int)f;
	}
	// Leave space for border
	ctrlRect.DeflateRect(1, 1);

	// Tile the bitmap into the temporary rectangle
	TileBitmap(&tempDC, &memDC, ctrlRect);

	// Select and realize the palette
	if(pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE && m_Palette.m_hObject != NULL)
	{
		pDC->SelectPalette(&m_Palette, FALSE);
		pDC->RealizePalette();
	}

	// Copy from temporary DC to screen (only the percentage rectangle)
	if (ctrlRect.IsRectEmpty() == FALSE)
		pDC->BitBlt(2, 2, ctrlRect.Width(), ctrlRect.Height(), &tempDC, 0, 0, SRCCOPY);

	/* RFU
	CRect R;
	GetClientRect(R);
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText("Title", -1, R, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	*/

	// Restore old selected bitmaps
	tempDC.SelectObject(pOldTempBitmap);
	memDC.SelectObject(pOldBitmap);
} // End of DrawProgress


void CProgressST::TileBitmap(CDC * pDestDC, CDC* pSrcDC, CRect rect)
{
	int nHLoop;
	int nVLoop;
	int nHTiles;
	int nVTiles;

	// Calc number of horizontal tiles
	nHTiles = (rect.Width() / m_nWidth);
	if (rect.Width() % m_nWidth != 0) nHTiles++;
	// Calc number of vertical tiles
	nVTiles = (rect.Height() / m_nHeight);
	if (rect.Height() % m_nHeight != 0) nVTiles++;

	// Tile bitmap horizontally
	for (nHLoop = 0; nHLoop < nHTiles; nHLoop++)
	{
		// Tile bitmap vertically
		for (nVLoop = 0; nVLoop < nVTiles; nVLoop++)
		{
			pDestDC->BitBlt((nHLoop*m_nWidth), (nVLoop*m_nHeight), m_nWidth, m_nHeight, pSrcDC, 0, 0, SRCCOPY);
		}
	}
} // End of TileBitmap


void CProgressST::CalcRange()
{
	m_nRange = abs(m_nUpper - m_nLower);
	// Avoid divide by zero
	if (m_nRange == 0) m_nRange = 1;
} // End of CalcRange


void CProgressST::SetRange(int nLower, int nUpper)
{
	m_nLower = nLower;
	m_nUpper = nUpper;
	CalcRange();

	CProgressCtrl::SetRange(nLower, nUpper);
} // End of SetRange


int CProgressST::SetStep(int nStep)
{
	m_nStep = nStep;
	return CProgressCtrl::SetStep(nStep);
} // End Of SetStep


int CProgressST::SetPos(int nPos)
{
	// Bound checking
	if (nPos < m_nLower) nPos = m_nLower;
	if (nPos > m_nUpper) nPos = m_nUpper;

	m_nPos = nPos;
	return CProgressCtrl::SetPos(nPos);
} // End of SetPos


int CProgressST::StepIt()
{
	m_nPos += m_nStep;

	// Bound checking
	if (m_nPos > m_nUpper) m_nPos = m_nLower + (m_nPos - m_nUpper);

	return CProgressCtrl::StepIt();
} // End of StepIt


BOOL CProgressST::GetBitmapAndPalette(UINT nIDResource, CBitmap& bitmap, CPalette& pal)
{
	LPCTSTR lpszResourceName = (LPCTSTR)nIDResource;

	HBITMAP hBmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), 
		lpszResourceName, IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION);

	if (hBmp == NULL) return FALSE;

	bitmap.Attach(hBmp);

	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	bitmap.GetObject(sizeof(ds), &ds);

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << bmInfo.biBitCount;

	// Create a halftone palette if colors > 256. 
	CClientDC dc(NULL); // Desktop DC

	if(nColors > 256)
		pal.CreateHalftonePalette(&dc);
	else
	{
		// Create the palette
		RGBQUAD *pRGB = new RGBQUAD[nColors];
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		memDC.SelectObject( &bitmap );
		::GetDIBColorTable( memDC, 0, nColors, pRGB );
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;
		for (int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
			pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}
		pal.CreatePalette( pLP );
		delete[] pLP;
		delete[] pRGB;
	}
	return TRUE;
} // End of GetBitmapAndPalette


const char* CProgressST::GetVersionC()
{
  return "1.0";
} // End of GetVersionC


const int CProgressST::GetVersionI()
{
  return 10; // Divide by 10 to get actual version
} // End of GetVersionI
