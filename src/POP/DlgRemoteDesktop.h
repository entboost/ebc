#pragma once
//#include "videoroom.h"
#include "Core/DlgVoiceBar.h"
#include "DlgVideoControl.h"


// CDlgRemoteDesktop dialog
/*
 * 远程桌面窗口类（没有用）
 * 
*//////////////////////////////////////////////////////

class CDlgRemoteDesktop : public CDialog
{
	DECLARE_DYNAMIC(CDlgRemoteDesktop)

public:
	CDlgRemoteDesktop(CWnd* pParent = NULL,bool bDeleteThis=false);   // standard constructor
	virtual ~CDlgRemoteDesktop();

	void SetVideoInfo(eb::bigint nCallId, eb::bigint nUserId);
	//static void FFTdataProcess(char *pdata, int length, void* udata);
	void OnMove(void);
	bool Photo(const char* lspzSaveTo);
// Dialog Data
	enum { IDD = IDD_DLG_REMOTE_DESKTOP };

protected:
	bool m_bDeleteThis;
	bool m_bShow;
	CWnd m_pUserVideo;
	eb::bigint m_nCallId;
	eb::bigint m_nVideoUserId;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CDlgVideoControl * m_pVideoControl;

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
