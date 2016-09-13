// EbTranEdit.cpp : implementation file
//

#include "stdafx.h"
//#include "test.h"
#include "EbTranEdit.h"


// CEbTranEdit

IMPLEMENT_DYNAMIC(CEbTranEdit, CEdit)

CEbTranEdit::CEbTranEdit()
{
	//m_mousedown = FALSE;
}

CEbTranEdit::~CEbTranEdit()
{
}


BEGIN_MESSAGE_MAP(CEbTranEdit, CEdit)
	//ON_WM_CTLCOLOR()
	//ON_CONTROL_REFLECT(EN_CHANGE, &CEbTranEdit::OnEnChange)
	//ON_WM_ERASEBKGND()
	//ON_WM_GETDLGCODE()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONUP()
	//ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CEbTranEdit::OnEnKillfocus)
END_MESSAGE_MAP()



// CEbTranEdit message handlers


//
//HBRUSH CEbTranEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	pDC->SetBkMode(TRANSPARENT);
//	return (HBRUSH)::GetStockObject(NULL_BRUSH);
//	return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
//
//	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
//
//	// TODO:  Change any attributes of the DC here
//
//	// TODO:  Return a different brush if the default is not desired
//	return hbr;
//}

void CEbTranEdit::RedrawParent(void)
{
	CRect rect;
	this->GetWindowRect(&rect);
	if (this->GetParent()->GetParent()==NULL)
	{
		this->GetParent()->ScreenToClient(&rect);
		this->GetParent()->InvalidateRect(&rect);
	}else// if (this->GetParent()->GetParent()->GetParent()==NULL)
	{
		this->GetParent()->GetParent()->ScreenToClient(&rect);
		this->GetParent()->GetParent()->InvalidateRect(&rect);
	//}else if (this->GetParent()->GetParent()->GetParent()->GetParent()==NULL)
	//{
	//	this->GetParent()->GetParent()->GetParent()->ScreenToClient(&rect);
	//	this->GetParent()->GetParent()->GetParent()->InvalidateRect(&rect);
	//}else
	//{
	//	this->GetParent()->GetParent()->GetParent()->GetParent()->ScreenToClient(&rect);
	//	this->GetParent()->GetParent()->GetParent()->GetParent()->InvalidateRect(&rect);
	}
}
//
//void CEbTranEdit::OnEnChange()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CEdit::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//	RedrawParent();
//}
//
//BOOL CEbTranEdit::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: Add your message handler code here and/or call default
//	//return TRUE;
//	return CEdit::OnEraseBkgnd(pDC);
//}
//
//UINT CEbTranEdit::OnGetDlgCode()
//{
//	// TODO: Add your message handler code here and/or call default
//	// Ñ¡Ôñ±ä»¯
//	RedrawParent();
//	return CEdit::OnGetDlgCode();
//}
//
//void CEbTranEdit::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//	m_mousedown = TRUE;
//	SetCapture();
//	CEdit::OnLButtonDown(nFlags, point);
//}
//
//void CEbTranEdit::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//	if(m_mousedown)
//		ReleaseCapture();
//	m_mousedown = FALSE;
//	CEdit::OnLButtonUp(nFlags, point);
//}
//
//void CEbTranEdit::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//	if(m_mousedown)
//	{
//		RedrawParent();
//	}
//	CEdit::OnMouseMove(nFlags, point);
//}

void CEbTranEdit::OnEnKillfocus()
{
	RedrawParent();
}
