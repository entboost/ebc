#pragma once

#include "Core/CDialogSK.h"
//#include "ImageEx.h"
// CDlgVideoControl dialog

//#define VIDEO_CONTRON_CLICKED	0x4002
class CVideoControlCallback
{
public:
	virtual void OnControlClick(int nControlId,BOOL bChecked) = 0;
};

class CDlgVideoControl : public CDialogSK
{
	DECLARE_DYNAMIC(CDlgVideoControl)

public:
	CDlgVideoControl(CWnd* pParent = NULL,CVideoControlCallback* pCallback=NULL);   // standard constructor
	virtual ~CDlgVideoControl();

	BOOL GetVideoChecked(void) const {return m_staVideo.GetCheck();}
	BOOL GetSpeekChecked(void) const {return m_staSpeek.GetCheck();}
// Dialog Data
	enum { IDD = IDD_DLG_VIDEOCONTROL };
protected:
	CVideoControlCallback* m_pCallback;
	CImageEx theBtnImage;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CLabelEx m_staVideo;
	CLabelEx m_staSpeek;
	//CBmpButton m_StaZoom;
	//CBmpButton m_StaMin;
	//CBmpButton m_StaPicture;
	//CBmpButton m_StaClose;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnStnClickedStaticVideo();
	afx_msg void OnStnClickedStaticSpeek();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	virtual void OnOK();
};
