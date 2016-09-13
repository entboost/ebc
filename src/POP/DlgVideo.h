#pragma once
//#include "videoroom.h"
#include "Core/DlgVoiceBar.h"
#include "DlgVideoControl.h"

const int const_video_width = 320;
const int const_video_height = 240;

// CDlgVideo dialog
#define FFT_LEN  1024

class CDlgVideo : public CDialog
	, public CVideoControlCallback
{
	DECLARE_DYNAMIC(CDlgVideo)

public:
	CDlgVideo(CWnd* pParent = NULL,bool bDeleteThis=false);   // standard constructor
	virtual ~CDlgVideo();

	void SetVideoInfo(eb::bigint nCallId, eb::bigint nUserId);
	void SetVoicePos(int nVoicePos);
	void OnMove(void);
	//void BuildVideoWindow(SHORT videoType);
	void ShowVideo(void);
	//bool Photo(const char* lspzSaveTo);
// Dialog Data
	enum { IDD = IDD_DLG_VIDEO };

protected:
	bool m_bDeleteThis;
	bool m_bShow;
	CWnd m_pUserVideo;
	eb::bigint m_nCallId;
	eb::bigint m_nVideoUserId;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CDlgVoiceBar *	m_pVoiceBar;
	CDlgVideoControl * m_pVideoControl;

	int		m_nVoicePos;
	virtual void OnControlClick(int nControlId,BOOL bChecked);

	DECLARE_MESSAGE_MAP()
	//LRESULT OnMsgVideoControlClicked(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMove(int x, int y);
};
