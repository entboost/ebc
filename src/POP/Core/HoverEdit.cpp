// HoverEdit.cpp : implementation file
//

#include "stdafx.h"
#include "HoverEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverEdit

CHoverEdit::CHoverEdit()
  :	m_bFirstTime(FALSE)
  , m_bFocusSelAll(FALSE)
  //,m_strPromptText(_T("<Enter text here>"))
  , m_crPromptColor(RGB(119,121,118))	// RAL 9023 (Pearl dark gray)
  , m_crTextColor(RGB(0,0,0))
{

	m_colorHoverRectangle = RGB(0,128,255);
	//m_colorNormalRectangle = 0;
	m_colorNormalRectangle = RGB(120,176,228);
}

CHoverEdit::~CHoverEdit()
{
	m_brush.DeleteObject();
}


BEGIN_MESSAGE_MAP(CHoverEdit, CEdit)
//{{AFX_MSG_MAP(CHoverEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCPAINT()
//}}AFX_MSG_MAP
//ON_WM_PAINT()
ON_CONTROL_REFLECT(EN_KILLFOCUS, &CHoverEdit::OnEnKillfocus)
ON_CONTROL_REFLECT(EN_SETFOCUS, &CHoverEdit::OnEnSetfocus)
ON_WM_KEYDOWN()
ON_WM_LBUTTONDOWN()
ON_WM_MBUTTONDOWN()
ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoverEdit message handlers

void CHoverEdit::OnHoverEnter()
{
	Redraw();
}

void CHoverEdit::OnHoverLeave()
{
	Redraw();
}

void CHoverEdit::Redraw()
{
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE);
}

void CHoverEdit::OnNcPaint() 
{
	//DWORD nStyle = this->GetStyle();
	//if (nStyle & ES_READONLY)
	//{
	//	return;
	//}

	//m_colorHoverRectangle = RGB(0,0,128);
	//m_colorNormalRectangle = RGB(0,0,128);
	CWindowDC DC(this);
	CRect Rect;
	GetWindowRect(&Rect);
	if (IsHover() && m_colorHoverRectangle!=0)
	{
		CPen BorderPen,*pOldPen;
		BorderPen.CreatePen(PS_SOLID,1, m_colorHoverRectangle);		//±ß¿òÑÕÉ«
		pOldPen = DC.SelectObject(&BorderPen);
		DC.Rectangle(0,0,Rect.Width(),Rect.Height());
		DC.SelectObject(pOldPen);
	}else if (m_colorNormalRectangle != 0)
	{
		CPen BorderPen,*pOldPen;
		BorderPen.CreatePen(PS_SOLID,1, m_colorNormalRectangle);	//±ß¿òÑÕÉ«
		pOldPen = DC.SelectObject(&BorderPen);
		DC.Rectangle(0,0,Rect.Width(),Rect.Height());
		DC.SelectObject(pOldPen);
	}else
	{
		//DC.DrawEdge(CRect(0,0,Rect.Width(),Rect.Height()),EDGE_SUNKEN,BF_FLAT|BF_RECT);
	}
}

void CHoverEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CTrackControl<CEdit>::OnPaint() for painting messages
	//CString sText;
	//this->GetWindowText(sText);
	//if (sText.IsEmpty())
	//	dc.TextOutA(0,0,"awfasf",6);
}

BOOL CHoverEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((GetKeyState(VK_CONTROL)&0x80)==0x80 && pMsg->wParam=='A')
		{
			this->SetSel(0,-1);
		}
	}
	return CTrackControl<CEdit>::PreTranslateMessage(pMsg);
}

void CHoverEdit::PreSubclassWindow()
{
	m_brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	if (!m_strPromptText.IsEmpty())
	{
		SetWindowText(m_strPromptText);
		//SetSel(-1, 0);		// get rid of standard highlighting
	}
	CTrackControl<CEdit>::PreSubclassWindow();
}


///////////////////////////////////////////////////////////////////////////////
// SetPromptColor
void CHoverEdit::SetPromptColor(COLORREF crText)
{
	m_crPromptColor = crText; 
	//if (m_bFirstTime)
	//	Invalidate(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// SetPromptText
void CHoverEdit::SetPromptText(LPCTSTR lpszPrompt)
{
	CString sCurrentText;
	GetWindowText(sCurrentText);
	m_strPromptText = lpszPrompt;
	m_bFirstTime = (!sCurrentText.IsEmpty() || m_strPromptText.IsEmpty())?FALSE:TRUE;
	//m_bFirstTime = m_strPromptText.IsEmpty()?FALSE:TRUE;
	if (m_bFirstTime)
		SetWindowText(m_strPromptText);
}
///////////////////////////////////////////////////////////////////////////////
// GetWindowText
int CHoverEdit::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	if (m_bFirstTime)
	{
		if (lpszStringBuf && (nMaxCount > 0))
			lpszStringBuf[0] = _T('\0');
		return 0;
	}

	return CTrackControl<CEdit>::GetWindowText(lpszStringBuf, nMaxCount);
}

///////////////////////////////////////////////////////////////////////////////
// GetWindowText
void CHoverEdit::GetWindowText(CString& rString) const
{
	if (m_bFirstTime)
		rString = _T("");
	else
		CTrackControl<CEdit>::GetWindowText(rString);
}

///////////////////////////////////////////////////////////////////////////////
// SetWindowText
void CHoverEdit::SetWindowText(LPCTSTR lpszString)
{
	// the control is being initialized, just ignore
	if (m_bFirstTime && (lpszString[0] == _T('\0')))
		return;

	// if this is not prompt string, then no need to prompt
	if (lpszString && 
		(m_strPromptText.Compare(lpszString) != 0))
		m_bFirstTime = FALSE;

	CTrackControl<CEdit>::SetWindowText(lpszString);
}
///////////////////////////////////////////////////////////////////////////////
// DefWindowProc
LRESULT CHoverEdit::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_SETTEXT)
	{
		TCHAR *cp = (TCHAR *) lParam;

		if (!cp)
			return TRUE;

		// the control is being initialized, just ignore
		if (m_bFirstTime && (cp[0] == _T('\0')))
			return TRUE;

		// if this is not prompt string, then no need to prompt
		if (m_strPromptText.Compare(cp) != 0)
			m_bFirstTime = FALSE;
	}
	else if (message == WM_GETTEXT)
	{
		if (m_bFirstTime)
		{
			TCHAR *cp = (TCHAR *) lParam;

			if (cp && (wParam != 0))
				*cp = _T('\0');

			return 0;
		}
	}

	return CTrackControl<CEdit>::DefWindowProc(message, wParam, lParam);
	//return CEdit::DefWindowProc(message, wParam, lParam);
}
///////////////////////////////////////////////////////////////////////////////
// CtlColor
HBRUSH CHoverEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	//pDC->SetBkColor(RGB(0,0,128));
	//pDC->SetBkMode(TRANSPARENT);
	if (m_bFirstTime)
		pDC->SetTextColor(m_crPromptColor);
	else
		pDC->SetTextColor(m_crTextColor);
	return m_brush;		// setting text color will have no effect unless 
						// we return a valid brush
}

void CHoverEdit::OnEnKillfocus()
{
	if (!m_strPromptText.IsEmpty())
	{
		CString sString;
		this->GetWindowText(sString);
		if (sString.IsEmpty())
		{
			SetPromptText(m_strPromptText);
		}
	}
}

void CHoverEdit::OnEnSetfocus()
{
	//if (m_bFocusSelAll)
	//	this->SetSel(0,-1);

	//if (m_bFirstTime)
	//{
	//	//this->SetSel(0,-1);
	//}
	//CTrackControl<CEdit>::OnSetFocus();
}

void CHoverEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bFirstTime)
	{
		// key down includes keys like shift and ctrl
		m_bFirstTime = FALSE;
		SetWindowText(_T(""));
	}

	CTrackControl<CEdit>::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CHoverEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bFirstTime)
	{
		// key down includes keys like shift and ctrl
		m_bFirstTime = FALSE;
		SetWindowText(_T(""));
	}

	CTrackControl<CEdit>::OnLButtonDown(nFlags, point);
}

void CHoverEdit::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bFirstTime)
	{
		// key down includes keys like shift and ctrl
		m_bFirstTime = FALSE;
		SetWindowText(_T(""));
	}

	CTrackControl<CEdit>::OnMButtonDown(nFlags, point);
}

void CHoverEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bFirstTime)
	{
		// key down includes keys like shift and ctrl
		m_bFirstTime = FALSE;
		SetWindowText(_T(""));
	}

	CTrackControl<CEdit>::OnRButtonDown(nFlags, point);
}
