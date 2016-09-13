#if !defined(AFX_TRACKCONTROL_H__06A38C29_E713_4289_97B5_EC17CA3DAED0__INCLUDED_)
#define AFX_TRACKCONTROL_H__06A38C29_E713_4289_97B5_EC17CA3DAED0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrackControl.h : header file
//

template<class BaseClass>
/////////////////////////////////////////////////////////////////////////////
// CTrackControl window

class CTrackControl : public BaseClass
{
// Construction
public:
	CTrackControl()
	{
		m_bTracking=m_bHover=FALSE;
	}
	virtual ~CTrackControl() {}
	BOOLEAN IsHover()
	{
		return m_bHover;
	}
	
// Implementation
public:
	virtual void OnHoverEnter()=0;
	virtual void OnHoverLeave()=0;
	
	// Generated message map functions
protected:
	virtual LRESULT WindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam)
	{
		LRESULT nResult=BaseClass::WindowProc(nMessage,wParam,lParam);
		switch(nMessage)
		{
		case WM_MOUSEMOVE:
			{
				if (!m_bTracking)
				{
					TRACKMOUSEEVENT Tme;
					Tme.cbSize = sizeof(Tme);
					Tme.hwndTrack = GetSafeHwnd();
					Tme.dwFlags = TME_LEAVE|TME_HOVER;
					Tme.dwHoverTime = 1;
					if (_TrackMouseEvent(&Tme))
						m_bTracking=TRUE;
				}
				break;				
			}
		case WM_MOUSEHOVER:
			m_bHover=TRUE;
			OnHoverEnter();
			break;
		case WM_MOUSELEAVE:
			m_bTracking=m_bHover=FALSE;
			OnHoverLeave();
			break;
		}
		return nResult;
	}

protected:
	BOOLEAN m_bTracking;
	BOOLEAN m_bHover;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKCONTROL_H__06A38C29_E713_4289_97B5_EC17CA3DAED0__INCLUDED_)
