#pragma once
#include "BitmapManager.h"
#include "rdevent.h"
#include <stl/locklist.h>

// CScrollStatic

#define DEFAULT_SCROLL_STATIC_ID	0x123001

class CScrollStatic : public CStatic
{
	DECLARE_DYNAMIC(CScrollStatic)
private:
	CLockList<rdpStruct>	m_listRtpDatas;
	int m_nScrollBarWidth;
	CRemoteEventHandler* m_pCallback;
	void* m_pUserData;

public:
	CScrollStatic();
	virtual ~CScrollStatic();

	CBitmapManager m_bitmapManager;

	void SetCallback(CRemoteEventHandler* pCallback, void * pUserData);
	bool CreateWnd(CWnd * pParentWnd);
	//void SetMouseMoveFPS(int fps);
	bool EnableControl(int nMouseFPS);
	int EnableControl(void) const;
	bool IsWndValidate(void) const;
	void SetScrollBarWidth(int scrollBarWidth) {m_nScrollBarWidth = scrollBarWidth;}
	int GetScrollBarWidth(void) const {return m_nScrollBarWidth;}

	int GetHScrollBarPos(void) const;
	int GetVScrollBarPos(void) const;

	void UpdateScreenInfo(void);
	void MoveParentFullSize(void);
	void drawScreen(void);

	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = DEFAULT_SCROLL_STATIC_ID);

protected:
	CScrollBar	m_hbar;
	CScrollBar	m_vbar;
	int m_nMouseFPS;
	int m_nWaitMouseMoveCount;
	CPoint m_ptLast;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnStnClicked();
	afx_msg void OnStnDblclick();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode(void);
};


