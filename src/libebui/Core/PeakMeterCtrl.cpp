///////////////////////////////////////////////////////////////////////////////
//	File:		PeakMeterCtrl.cpp
//	Version:	1.2
//
//	Author:		Ernest Laurentin
//	E-mail:		elaurentin@sympatico.ca
//
//	Implementation of the CPeakMeterCtrl and associated classes.
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is
//	not sold for profit without the authors written consent, and
//	providing that this notice and the authors name and all copyright
//	notices remains intact.
//
//	An email letting me know how you are using it would be nice as well.
//
//	This file is provided "as is" with no expressed or implied warranty.
//	The author accepts no liability for any damage/loss of business that
//	this c++ class may cause.
//
//	Version history
//
//	1.0	- Initial release.
//	1.1	- Impoved Falloff effect to look more like Audio system
//  1.2 - Added 3 ranges of color (low, medium, high)
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mmsystem.h>
#include "SkinMemDC.h"
//#include "MemDC2.h"
#include "PeakMeterCtrl.h"

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


template <class T>
bool InRange(const T val, const T valMin, const T valMax)
{
	return ( val >= valMin && val <= valMax );
}


COLORREF LightenColor(const COLORREF crColor, BYTE byIncreaseVal)
{
	BYTE byRed = GetRValue(crColor);
	BYTE byGreen = GetGValue(crColor);
	BYTE byBlue = GetBValue(crColor);

	byRed = (byRed + byIncreaseVal) <= 255 ? BYTE(byRed + byIncreaseVal) : 255;;
	byGreen = (byGreen + byIncreaseVal)	<= 255 ? BYTE(byGreen + byIncreaseVal) : 255;
	byBlue = (byBlue + byIncreaseVal) <= 255 ? BYTE(byBlue + byIncreaseVal) : 255;

	return RGB(byRed, byGreen, byBlue);
}


COLORREF DarkenColor(const COLORREF crColor, BYTE byReduceVal)
{
	BYTE byRed = GetRValue(crColor);
	BYTE byGreen = GetGValue(crColor);
	BYTE byBlue = GetBValue(crColor);

	byRed = byRed >= byReduceVal ? BYTE(byRed - byReduceVal) : 0;
	byGreen = byGreen >= byReduceVal ? BYTE(byGreen - byReduceVal) : 0;
	byBlue = (byBlue >= byReduceVal) ? BYTE(byBlue - byReduceVal) : 0;

	return RGB(byRed, byGreen, byBlue);
}


///////////////////////////////////////////////////////////////////////////////
// PeakMeterData class members
PeakMeterData& PeakMeterData::Copy(const PeakMeterData& pm)
{
	nValue = pm.nValue;
	nFalloff = pm.nFalloff;
	nPeak = pm.nPeak;
	return *this;
}

bool PeakMeterData::IsEqual(const PeakMeterData& pm)
{
	return (nValue == pm.nValue);
}

bool PeakMeterData::IsGreater(const PeakMeterData& pm)
{
	return ( nValue > pm.nValue );
}

bool PeakMeterData::IsLower(const PeakMeterData& pm)
{
	return ( nValue < pm.nValue );
}


/////////////////////////////////////////////////////////////////////////////
// CPeakMeterCtrl

CPeakMeterCtrl::CPeakMeterCtrl()
{
	TIMECAPS timecaps = {0};
	MMRESULT mmresult = timeGetDevCaps(&timecaps, sizeof(timecaps));
	mmresult = timeBeginPeriod( timecaps.wPeriodMin );

	// Initializes all data
	InitData();
}

CPeakMeterCtrl::~CPeakMeterCtrl()
{
	TIMECAPS timecaps = {0};
	MMRESULT mmresult = timeGetDevCaps(&timecaps, sizeof(timecaps));
	mmresult = timeEndPeriod( timecaps.wPeriodMin );
}


BEGIN_MESSAGE_MAP(CPeakMeterCtrl, CWnd)
	//{{AFX_MSG_MAP(CPeakMeterCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////
// Create
BOOL CPeakMeterCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle |= WS_CHILD;
	return CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID, pContext);
}


///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CPeakMeterCtrl::PreSubclassWindow() 
{
	ResetControl();
	
	CWnd::PreSubclassWindow();
}


///////////////////////////////////////////////////////////////////////////////
// InitData
void CPeakMeterCtrl::InitData()
{
	const COLORREF colLime = RGB(  0,255,  0);
	const COLORREF colRed  = RGB(255,  0,  0);
	const COLORREF colYellow  = RGB(255, 255,  0);

	m_bShowGrid		= false;
	m_bShowFalloff	= true;
	m_uTimerID		= 0;		// no timer
	m_nDelay		= DELAY_10MS;
	m_nMinValue		= 60;		// Min Range 0-60
	m_nMedValue		= 80;		// Med Range 60-80
	m_nMaxValue		= 100;		// Max Range 80-100
	m_nNumBands		= BAND_DEFAULT;
	m_nLedBands		= LEDS_DEFAULT;
	m_clrBackground = GetSysColor(COLOR_3DFACE);
	m_clrNormal		= colLime;
	m_clrMedium		= colYellow;
	m_clrHigh		= colRed;
	m_nSpeed		= DEFAULT_SPEED;
	// clear vector data
	m_MeterData.clear();
}

///////////////////////////////////////////////////////////////////////////////
// ResetControl
void CPeakMeterCtrl::ResetControl()
{
	// Initialize vector
	m_MeterData.resize( m_nNumBands);

	// default data
	PeakMeterData pm;
	pm.nValue = m_nMaxValue;
	pm.nFalloff = m_nMaxValue;
	pm.nPeak  = m_nSpeed;

	for(unsigned int i=0; i < m_MeterData.size(); i++)
	{
		m_MeterData.at(i) = pm;
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetBackgroundColor
// DESCRIPTION: Set background color for this control
void CPeakMeterCtrl::SetBackgroundColor(COLORREF colorBgnd)
{
	m_clrBackground = colorBgnd;
}


///////////////////////////////////////////////////////////////////////////////
// SetBandsColor
// DESCRIPTION: Set bands color for this control
void CPeakMeterCtrl::SetBandsColor(COLORREF colorNormal, COLORREF colorMedium, COLORREF colorHigh)
{
	m_clrNormal = colorNormal;
	m_clrMedium = colorMedium;
	m_clrHigh   = colorHigh;
}


///////////////////////////////////////////////////////////////////////////////
// SetMeterBands
// DESCRIPTION: Set number of Vertical or Horizontal bands to display
// NOTES:       Obtain smooth effect by setting nHorz or nVert to "1", these
//              cannot be 0
void CPeakMeterCtrl::SetMeterBands(int nNumBands, int nLedBands)
{
	ASSERT( nNumBands>0 && nLedBands>0 );
	if (nNumBands == 0 || nLedBands == 0)
		return;

	m_nNumBands = nNumBands;
	m_nLedBands = nLedBands;

	// Reset vector
	ResetControl();
}


///////////////////////////////////////////////////////////////////////////////
// SetRangeValue
// Min: [0 - nMin[,  Med: [nMin - nMed[,  Max: [nMed - nMax]
void CPeakMeterCtrl::SetRangeValue(int nMin, int nMed, int nMax)
{
	ASSERT( nMax > nMed && nMed > nMin && nMin > 0);

	m_nMinValue	= nMin;
	m_nMedValue	= nMed;
	m_nMaxValue	= nMax;
}


///////////////////////////////////////////////////////////////////////////////
// GetRangeValue
// DESCRIPTION: Get Range value of this control
void CPeakMeterCtrl::GetRangeValue(int* lpiMin, int* lpiMed, int* lpiMax) const
{
	if (NULL != lpiMin)
		*lpiMin = m_nMinValue;
	if (NULL != lpiMed)
		*lpiMed = m_nMedValue;
	if (NULL != lpiMax)
		*lpiMax = m_nMaxValue;
}


///////////////////////////////////////////////////////////////////////////////
// SetFalloffDelay
// DESCRIPTION: Set Peak value speed before falling off
void CPeakMeterCtrl::SetFalloffDelay(int nSpeed)
{
	m_nSpeed = nSpeed;
}


///////////////////////////////////////////////////////////////////////////////
// SetFalloffEffect
// DESCRIPTION: Set falloff effect flag
void CPeakMeterCtrl::SetFalloffEffect(bool bFalloffEffect)
{
	if (m_bShowFalloff != bFalloffEffect)
	{
		m_bShowFalloff = bFalloffEffect;
		Refresh();
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetFalloffEffect
// DESCRIPTION: Read falloff effect flag
bool CPeakMeterCtrl::GetFalloffEffect() const
{
	return m_bShowFalloff;
}


///////////////////////////////////////////////////////////////////////////////
// ShowGrid
// DESCRIPTION: Request to have gridlines visible or not
void CPeakMeterCtrl::ShowGrid(bool bShowGrid)
{
	if (m_bShowGrid != bShowGrid)
	{
		m_bShowGrid = bShowGrid;
		Refresh();
	}

}


///////////////////////////////////////////////////////////////////////////////
// IsGridVisible
// DESCRIPTION: Returns if gridlines are visible
bool CPeakMeterCtrl::IsGridVisible() const
{
	return m_bShowGrid;
}


///////////////////////////////////////////////////////////////////////////////
// SetData
// DESCRIPTION: Change data value.  Use this function to change only
//              a set of values.  All bands can be changed or only 1 band.
//              Depending on the application. 
bool CPeakMeterCtrl::SetData(const int ArrayValue[], int nOffset, int nSize)
{
	ASSERT( nOffset >= 0 && ArrayValue != NULL);
	if (nOffset < 0 || ArrayValue == NULL)
		return false;

	bool bIsRunning = IsStarted();

	// Stop timer if Animation is active
	if ( bIsRunning )
		Stop();

	unsigned int nMaxSize = nOffset + nSize;
	for(unsigned int i=nOffset; i < nMaxSize; i++)
	{
		if (i < m_MeterData.size())
		{
			PeakMeterData pm = m_MeterData.at( i );
			pm.nValue = ArrayValue[i];
			if (pm.nFalloff < pm.nValue)
			{
				pm.nFalloff = pm.nValue;
				pm.nPeak    = m_nSpeed;
			}
			m_MeterData.at(i) = pm;
		}
	}

	// Auto-restart
	if ( bIsRunning )
		return Start( m_nDelay );

	Refresh();
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Refresh
// DESCRIPTION: Invalidate the control
void CPeakMeterCtrl::Refresh()
{
	if (::IsWindow( GetSafeHwnd() ))
		Invalidate();
}


///////////////////////////////////////////////////////////////////////////////
// IsStarted
// DESCRIPTION: Check if animation is active
bool CPeakMeterCtrl::IsStarted() const
{
	return (m_uTimerID > 0);
}


///////////////////////////////////////////////////////////////////////////////
// Start
// DESCRIPTION: Start the timer and animation effect
bool CPeakMeterCtrl::Start(UINT uDelay)
{
	if ( !IsStarted() )
	{
		MMRESULT mmresult;
		mmresult = timeSetEvent(uDelay, 0, PeakTimerProc, (DWORD)this, TIME_PERIODIC);
		m_uTimerID = (UINT) mmresult;
		m_nDelay   = uDelay;
	}
	return (m_uTimerID != 0);
}


///////////////////////////////////////////////////////////////////////////////
// Stop
// DESCRIPTION: Stop the timer and animation effect
bool CPeakMeterCtrl::Stop()
{
	if ( IsStarted() )
	{
		timeKillEvent( m_uTimerID );
		m_uTimerID = 0;
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// DoTimerProcessing
// DESCRIPTION: Peak Meter animation - virtual
void CPeakMeterCtrl::DoTimerProcessing(UINT uID)
{
	Refresh();

	int nDecValue  = m_nMaxValue / m_nLedBands;
	bool bNoChange = true;

	for(unsigned int i=0; i < m_MeterData.size(); i++)
	{
		PeakMeterData pm = m_MeterData.at(i);

		if (pm.nValue > 0)
		{
			pm.nValue -= ( m_nLedBands>1 ? nDecValue : m_nMaxValue*BAND_PERCENT/100);
			if (pm.nValue < 0)
				pm.nValue = 0;
			bNoChange = false;
		}

		if (pm.nPeak > 0)
		{
			pm.nPeak -= 1;
			bNoChange = false;
		}

		if (pm.nPeak == 0 && pm.nFalloff > 0)
		{
			pm.nFalloff -= (m_nLedBands>1 ? nDecValue>>1 : 5);
			if (pm.nFalloff < 0)
				pm.nFalloff = 0;
			bNoChange = false;
		}

		// re-assign PeakMeterData
		m_MeterData.at(i) = pm;
	}

	if (bNoChange) // Stop timer if no more data but do not reset ID
	{
		timeKillEvent( m_uTimerID );
	}
}


///////////////////////////////////////////////////////////////////////////////
// CPeakMeterCtrl message handlers

///////////////////////////////////////////////////////////////////////////////
// OnCreate
int CPeakMeterCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//InitControl();
	
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CPeakMeterCtrl::OnDestroy() 
{
	Stop();

	// clear vector data
	m_MeterData.clear();
	
	CWnd::OnDestroy();
}


///////////////////////////////////////////////////////////////////////////////
// OnPaint
// DESCRIPTION: Paint this control
void CPeakMeterCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	TRY
	{
		CRect rc;
		GetWindowRect( rc );
		rc.OffsetRect(-rc.left, -rc.top);
		//CMemDC2 memdc(&dc, &rc);
		CSkinMemDC memdc(&dc, &rc);
		if (memdc.m_hDC == NULL)
			return;
		memdc.SetBkMode(TRANSPARENT);
		memdc.FillSolidRect( rc, m_clrBackground);

		CPen   pen;
		if (!pen.CreatePen(PS_SOLID, 1, DarkenColor(m_clrBackground, GRID_INCREASEBY)))
			return;
		CPen* pOldPen = (CPen*) memdc.SelectObject( & pen );

		if (GetStyle() & PMS_VERTICAL)
			DrawVertBand(&memdc, rc);
		else
			DrawHorzBand(&memdc, rc);

		memdc.SelectObject( pOldPen );
		pen.DeleteObject();
		//memdc.DeleteDC();
		// Do not call CWnd::OnPaint() for painting messages
	}
	CATCH(CResourceException, e);
	{
		e->Delete();
	}
	END_CATCH

}


///////////////////////////////////////////////////////////////////////////////
// DrawVertBand
// DESCRIPTION: Draw Vertical bands - No falloff effect for vertical bands
void CPeakMeterCtrl::DrawVertBand(CDC* pDC, CRect& rcClient)
{
	TRY
	{
		int nHorzBands = (m_nLedBands>1 ? m_nLedBands : m_nMaxValue*BAND_PERCENT/100);
		int nMinHorzLimit = m_nMinValue*nHorzBands/m_nMaxValue;
		int nMedHorzLimit = m_nMedValue*nHorzBands/m_nMaxValue;
		int nMaxHorzLimit = nHorzBands;

		CSize size;
		size.cx = rcClient.Width()/nHorzBands;
		size.cy = rcClient.Height()/m_nNumBands;
		CRect rcBand( rcClient.TopLeft(), size);

		// Draw band from top
		rcBand.OffsetRect(0, rcClient.Height()-size.cy*m_nNumBands);
		int xDecal = (m_nLedBands>1 ? 1 : 0);
		int yDecal = (m_nNumBands>1 ? 1 : 0);

		for(int nVert=0; nVert < m_nNumBands; nVert++)
		{
			int nValue = m_MeterData.at(nVert).nValue;
			int nHorzLimit = nValue*nHorzBands/m_nMaxValue;

			for(int nHorz=0; nHorz < nHorzBands; nHorz++)
			{
				rcBand.DeflateRect(xDecal, yDecal, 0, yDecal);

				// Find color based on range value
				COLORREF colorRect = m_clrBackground;
				if (true == m_bShowGrid)
					colorRect = DarkenColor(m_clrBackground, GRID_INCREASEBY);

				if ( m_bShowGrid && (nHorz == nMinHorzLimit || nHorz == (nHorzBands-1)))
				{
					POINT points[2] = { 0 };
					points[0].x = rcBand.TopLeft().x + (rcBand.Width()>>1);
					points[0].y = rcBand.TopLeft().y - yDecal;
					points[1].x = points[0].x;
					points[1].y = rcBand.BottomRight().y + yDecal;
					pDC->Polyline( points, 2);
				}

				if ( nHorz < nHorzLimit)
				{
					if ( InRange<int>(nHorz, 0, nMinHorzLimit-1) )
						colorRect = m_clrNormal;
					else if ( InRange<int>(nHorz, nMinHorzLimit, nMedHorzLimit-1) )
						colorRect = m_clrMedium;
					else if ( InRange<int>(nHorz, nMedHorzLimit, nMaxHorzLimit) )
						colorRect = m_clrHigh;
				}

				pDC->FillSolidRect( rcBand, colorRect);

				rcBand.InflateRect(xDecal, yDecal, 0, yDecal);
				rcBand.OffsetRect(size.cx, 0);
			}

			// Move to Next Vertical band
			rcBand.OffsetRect(-size.cx*nHorzBands, size.cy);
		}
	}
	CATCH(CResourceException, e);
	{
		e->Delete();
	}
	END_CATCH

}


///////////////////////////////////////////////////////////////////////////////
// DrawHorzBand
// DESCRIPTION: Draw Horizontal bands - with Falloff effect
void CPeakMeterCtrl::DrawHorzBand(CDC* pDC, CRect& rcClient)
{
	TRY
	{
		int nVertBands = (m_nLedBands>1 ? m_nLedBands : m_nMaxValue*BAND_PERCENT/100);
		int nMinVertLimit = m_nMinValue*nVertBands/m_nMaxValue;
		int nMedVertLimit = m_nMedValue*nVertBands/m_nMaxValue;
		int nMaxVertLimit = nVertBands;

		CSize size;
		size.cx = rcClient.Width()/m_nNumBands;
		size.cy = rcClient.Height()/nVertBands;
		CRect rcBand( rcClient.TopLeft(), size);

		// Draw band from bottom
		rcBand.OffsetRect(0, rcClient.bottom-size.cy);
		int xDecal = (m_nNumBands>1 ? 1 : 0);
		int yDecal = (m_nLedBands>1 ? 1 : 0);

		for(int nHorz=0; nHorz < m_nNumBands; nHorz++)
		{
			int nValue = m_MeterData.at(nHorz).nValue;
			int nVertLimit = nValue*nVertBands/m_nMaxValue;
			CRect rcPrev = rcBand;

			for(int nVert=0; nVert < nVertBands; nVert++)
			{
				rcBand.DeflateRect(xDecal, yDecal, xDecal, 0);

				// Find color based on range value
				COLORREF colorRect = m_clrBackground;
				if ( true == m_bShowGrid)
					colorRect = DarkenColor(m_clrBackground, GRID_INCREASEBY);

				// Draw grid line (level) bar
				if ( m_bShowGrid && (nVert == nMinVertLimit || nVert == (nVertBands-1)))
				{
					POINT points[2] = { 0 };
					points[0].x = rcBand.TopLeft().x - xDecal;
					points[0].y = rcBand.TopLeft().y + (rcBand.Height()>>1);
					points[1].x = rcBand.BottomRight().x + xDecal;
					points[1].y = points[0].y;
					pDC->Polyline( points, 2);
				}

				if ( nVert < nVertLimit)
				{
					if ( InRange<int>(nVert, 0, nMinVertLimit-1) )
						colorRect = m_clrNormal;
					else if ( InRange<int>(nVert, nMinVertLimit, nMedVertLimit-1) )
						colorRect = m_clrMedium;
					else if ( InRange<int>(nVert, nMedVertLimit, nMaxVertLimit) )
						colorRect = m_clrHigh;
				}

				pDC->FillSolidRect( rcBand, colorRect);

				rcBand.InflateRect(xDecal, yDecal, xDecal, 0);
				rcBand.OffsetRect(0, -size.cy);
			}

			// Draw falloff effect
			if (m_bShowFalloff)
			{
				CPen pen;
				if (!pen.CreatePen(PS_SOLID, 1, DarkenColor(m_clrBackground, FALL_INCREASEBY))) return;
				CPen* pOldPen = (CPen*) pDC->SelectObject(&pen );
				int nMaxHeight = size.cy*nVertBands;
				POINT points[2] = { 0 };
				points[0].x = rcPrev.TopLeft().x + xDecal;
				points[0].y = rcPrev.BottomRight().y - m_MeterData.at(nHorz).nFalloff*nMaxHeight/m_nMaxValue;
				points[1].x = rcPrev.BottomRight().x - xDecal;
				points[1].y = points[0].y;
				pDC->Polyline( points, 2);
				pDC->SelectObject( pOldPen );
				pen.DeleteObject();
			}

			// Move to Next Horizontal band
			rcBand.OffsetRect(size.cx, size.cy*nVertBands);
		}
	}
	CATCH(CResourceException, e);
	{
		e->Delete();
	}
	END_CATCH

}

///////////////////////////////////////////////////////////////////////////////
// PeakTimerProc
// DESCRIPTION: Multimedia timer procedure - this is called from another thread
void CALLBACK CPeakMeterCtrl::PeakTimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CPeakMeterCtrl* pThis = reinterpret_cast<CPeakMeterCtrl*>( dwUser );
	ASSERT( pThis != NULL );

	pThis->DoTimerProcessing( uID );
}

