#pragma once
#ifndef __RDWnd_h__
#define __RDWnd_h__

#include "rdevent.h"
#include "DesktopWindow.h"
#include <boost/thread.hpp>
#include <stl/locklist.h>
#include "XvidProxy.h"

#define RD_MIN_FPS		1
#define RD_MAX_FPS		10
#define RD_DEFAULT_FPS	5

class CRDWnd
{
private:
	CLockList<rdpStruct>	m_listRtpDatas;
	CRDEventHandler *		m_eventHandler;

	bool			m_bKilled;
	boost::thread * m_pGetDesktopThread;
	boost::thread * m_pProcessMsgThread;
	CXvidProxy m_xvid;
	CDesktopWindow m_desktopWindow;
	int m_fps;			// RD_DEFAULT_FPS
	CPoint m_ptLast;
	INT64 m_bEnableControl;
	//bool m_bWaitShiftUp;
	//bool m_bWaitControlUp;
	bool m_bWaitAltUp;
public:
	bool StartLocalDesktop(int quant, int fps, CRDEventHandler* pHandler);
	bool IsLocalDesktopStart(void) const;
	int GetLocalDesktopFPS(void) const {return m_fps;}
	void StopLocalDesktop(void);
	void AddRdpMessage(INT64 nDestId, const rdpStruct& rdpMessage);
	void EnableControl(INT64 bEnableControl) {m_bEnableControl = bEnableControl;}
	INT64 EnableControl(void) const {return m_bEnableControl;}

	void doGetDesktopInfo(void);
	void doProcessMessage(void);

public:
	CRDWnd(void);
	virtual ~CRDWnd(void);
};

#endif // __RDWnd_h__
