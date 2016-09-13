#pragma once
#include "afxwin.h"

#include "DlgRemoteDesktop.h"
#include "DlgRDWindows.h"

// CPanelRemoteDesktop dialog
#define MAX_RD_COUNT 4

class CPanelRemoteDesktop : public CEbDialogBase
#ifdef USES_EBCOM_TEST
	//, public CEBVideoDataEventsSink
	//, public CEBVRoomEventsSink
#endif
{
	DECLARE_DYNAMIC(CPanelRemoteDesktop)

public:
	CPanelRemoteDesktop(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelRemoteDesktop();

	EB_CallInfo m_pCallInfo;

	void ExitRD(void);
	//void OnMove(void);
	//int GetVideoCount(void) const;
	void ExitChat(bool bHangup);
	void SetCtrlColor(bool bInvalidate = true);
	bool IsInFRDRequest(void) const {return m_bInFRDRequest;}
	bool GetInDesktop(void) const {return m_nInDesktop;}

// Dialog Data
	enum { IDD = IDD_PAN_RD };

#ifdef USES_EBCOM_TEST
	void VRequestResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VAckResponse(IEB_VideoInfo* pVideoInfo,int nStateValue);
	void VideoRequest(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoAccept(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoCancel(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
	void VideoEnd(IEB_VideoInfo* pVideoInfo,IEB_UserVideoInfo* pUserVideoInfo);
#else
	void RDRequestResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void RDAckResponse(const EB_RemoteDesktopInfo* pVideoInfo,int nStateValue);
	void RDRequest(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDAccept(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDCancel(const EB_RemoteDesktopInfo* pVideoInfo);
	void RDEnd(const EB_RemoteDesktopInfo* pVideoInfo);
#endif
protected:
	CDlgRDWindows::pointer m_pRDWindows;
	eb::bigint m_nRequestUserId;
	bool m_bInVideoConference;
	bool m_bLocalOnDesktop;
	eb::bigint m_nFromUserId;
	//int m_nLocalUserId;
	boost::shared_mutex m_mutex; 
	CDlgRemoteDesktop * m_pUserVideo[MAX_RD_COUNT];
	INT64 m_nVideoUserId[MAX_RD_COUNT];
	CRect m_rectUser[MAX_RD_COUNT];
	CLockList<CDlgRemoteDesktop*> m_pCloseWnd;
	void CloseAllVideo(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CTraButton m_btnVideoOn;
	CTraButton m_btnVideoAccept;
	CTraButton m_btnVideoEnd;
	CTraButton m_btnVideoCancel;
	CLabelEx m_bEnableControl;

	bool IsMyOnVideo(void) const;
	int GetVideoIndex(INT64 nVideoUserId) const;
	int GetNullVideoIndex(void);
	//void OpenUserVideo(INT64 nVideoUserId);
	bool m_bInFRDRequest;
	bool m_nInDesktop;
	void DoVideoDisonnecte(void);
	void DoVideoConnected(void);

	// video
//#ifndef USES_EBCOM1
//	LRESULT OnMessageVRequestResponse(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVAckResponse(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoRequest(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoAccept(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoCancel(WPARAM wParam, LPARAM lParam);
//	LRESULT OnMessageVideoEnd(WPARAM wParam, LPARAM lParam);
//#endif
	//LRESULT OnMsgBuildUserVideo(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonVideoEnd();
	afx_msg void OnBnClickedButtonVideoAccept();
	afx_msg void OnBnClickedButtonVideoCancel();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonOn();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckEnableControl();
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//afx_msg void OnPaint();
};
