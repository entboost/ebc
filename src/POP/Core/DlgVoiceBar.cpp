// DlgVoiceBar.cpp : implementation file
//

#include "stdafx.h"
//#include "herb.h"
//#include "herbDlg.h"
#include "DlgVoiceBar.h"


IMPLEMENT_DYNAMIC(CDlgVoiceBar, CDialogSK)
CDlgVoiceBar::CDlgVoiceBar(CWnd* pParent /*=NULL*/)
	: CDialogSK(CDlgVoiceBar::IDD, pParent)
{
	m_tReceiveVoiceTime = 0;
	m_nLastVoiceValue = 0;
	m_nSamCount = 0;
}

CDlgVoiceBar::~CDlgVoiceBar()
{
}

void CDlgVoiceBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VOICE_BAR, m_PeakMeter1);
}

BEGIN_MESSAGE_MAP(CDlgVoiceBar, CDialogSK)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgVoiceBar message handlers
void CDlgVoiceBar::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
//	OnOK();
}

void CDlgVoiceBar::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
//	OnCancel();
}

void CDlgVoiceBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogSK::OnSize(nType, cx, cy);
	if (m_PeakMeter1.GetSafeHwnd())
		m_PeakMeter1.MoveWindow(0, 0, cx, cy);
}

#define THE_LEDBANDS 10
#define THE_TRANSPARENNT_COLOR RGB(128, 0, 255)
BOOL CDlgVoiceBar::OnInitDialog()
{
	CDialogSK::OnInitDialog();
	this->EnableEasyMove(FALSE);
	this->SetTransparentColor(THE_TRANSPARENNT_COLOR);

	m_PeakMeter1.SetBackgroundColor(THE_TRANSPARENNT_COLOR);
	m_PeakMeter1.SetMeterBands(8, THE_LEDBANDS);
	m_PeakMeter1.Start(1000/18); // 18 fps
	m_PeakMeter1.SetFalloffEffect( false );
	SetTimer(1000, 1000, NULL);		// 定时检查是否有声音

	//SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgVoiceBar::SetVoiceValue(int nValue)
{
	if (m_PeakMeter1.GetSafeHwnd())
	{
		if (nValue != 0 && nValue == m_nLastVoiceValue)
		{
			m_nSamCount++;
			if (m_nSamCount >= 10)
			{
				m_nSamCount = 0;
				return false;
			}
		}else
		{
			m_nSamCount = 0;
		}
		m_nLastVoiceValue = nValue;
		m_tReceiveVoiceTime = time(0);
		nValue /= 2;
		int ArrayData[THE_LEDBANDS] = { 0 };
		for(int i=0; i < THE_LEDBANDS; i++)
		{
			int nRandom = nValue==0?0:rand();
			ArrayData[i] = nValue-(nRandom%20);
		}
		m_PeakMeter1.SetData(ArrayData, 0, THE_LEDBANDS);
		return true;
	}
	return false;
}


void CDlgVoiceBar::OnDestroy()
{
	CDialogSK::OnDestroy();
	m_PeakMeter1.Stop();
}

void CDlgVoiceBar::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1000)
	{
		if (m_tReceiveVoiceTime > 0 && time(0)-m_tReceiveVoiceTime>1)
		{
			SetVoiceValue(0);
			m_tReceiveVoiceTime = 0;
		}
	}

	CDialogSK::OnTimer(nIDEvent);
}
