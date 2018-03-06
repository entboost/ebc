#ifndef _PROGRESSST_H
#define _PROGRESSST_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ProgressST.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressST window

class CProgressST : public CProgressCtrl
{
public:
	CProgressST();
	virtual ~CProgressST();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressST)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetBitmap(UINT nBitmapId = NULL, BOOL bRepaint = TRUE);

	void SetRange(int nLower, int nUpper);
	int SetPos(int nPos);
	int SetStep(int nStep);
	int StepIt( );

	static const char* GetVersionC();
	static const int GetVersionI();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProgressST)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	BOOL GetBitmapAndPalette(UINT nIDResource, CBitmap& bitmap, CPalette& pal);
	void CalcRange();
	void DrawProgress(CDC* pDC);
	void TileBitmap(CDC* pDestDC, CDC* pSrcDC, CRect rect);

	int m_nLower;
	int m_nUpper;
	int m_nRange;

	int m_nStep;

	int m_nPos;

	CBitmap m_bmPattern;
	CPalette m_Palette;
	int m_nWidth;
	int m_nHeight;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
