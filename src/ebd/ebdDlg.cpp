
// ebdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ebd.h"
#include "ebdDlg.h"

#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CebdDlg dialog




CebdDlg::CebdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CebdDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CebdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CebdDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CebdDlg::OnBnClickedButton1)
	//ON_MESSAGE(EBCR_WM_RTP_FRAME, OnMessageRtpFrame)
	ON_BN_CLICKED(IDC_BUTTON2, &CebdDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CebdDlg::OnBnClickedButton3)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON4, &CebdDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CebdDlg message handlers

BOOL CebdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//AfxMessageBox(_T("OnInitDialog"));

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//AfxMessageBox(_T("aaa"));
	//theEBRD->SetMsgHwnd(this->GetSafeHwnd());
	//AfxMessageBox(_T("bbb"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CebdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CebdDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CebdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define DEFAULT_ROOM_ID 1
void CebdDlg::OnBnClickedButton1()
{
	CString sString;
	this->GetDlgItem(IDC_EDIT_SRCID)->GetWindowText(sString);
	const mycp::bigint nSourceId = cgc_atoi64(sString);
	if (nSourceId==0)
	{
		AfxMessageBox(_T("SourceId不能为空！"));
		return;
	}
	theEBRD->Register(DEFAULT_ROOM_ID, nSourceId, 10, false);
}

LRESULT CebdDlg::OnMessageRtpFrame(WPARAM wp, LPARAM lp)
{
	const EBRD_RtpFrame* pRtpFrame = (const EBRD_RtpFrame*)wp;
	// ebrd::uint32 nUserData = (ebrd::uint32)lp;

	FILE * f = fopen("f:\\test2.txt","wb");
	//FILE * f = fopen("f:\\log2.tar.gz","wb");
	if (f==NULL)
		return 0;
	fwrite(pRtpFrame->m_pRtpFrame->m_pPayload,1,pRtpFrame->m_pRtpFrame->m_pRtpHead.m_nTotleLength,f);
	fclose(f);

	//CString sText;
	//sText.Format(_T("OnMessageRtpFrame:%d,%s"),(int)pRtpFrame->m_pRtpFrame->m_pRtpHead.m_nTotleLength,pRtpFrame->m_pRtpFrame->m_pPayload);
	//AfxMessageBox(sText);
	return 0;
}


void CebdDlg::OnBnClickedButton2()
{
	CString sString;
	this->GetDlgItem(IDC_EDIT_DESTID)->GetWindowText(sString);
	const mycp::bigint nDestId = cgc_atoi64(sString);
	if (nDestId==0)
	{
		AfxMessageBox(_T("DestId不能为空！"));
		return;
	}
	theEBRD->AddSink(nDestId, this->GetSafeHwnd());
	theEBRD->ControlSinkDesktop(nDestId, 2);
}

void CebdDlg::OnBnClickedButton3()
{
	theEBRD->StartLocalDesktop(9,2);
	theEBRD->EnableControlLocalDesktop(1);
	//const mycp::uint32 tNow1 = timeGetTime();
	//Sleep(1000);
	//const mycp::uint32 tNow2 = timeGetTime();
	//int i = tNow2-tNow1;
	
	//FILE * f = fopen("f:\\test.txt","rb");
	////FILE * f = fopen("f:\\log.tar.gz","rb");
	//if (f==NULL)
	//	return;
	//unsigned char lpszBuffer[200*1024];
	//const size_t nSize = fread(lpszBuffer,1,200*1024,f);
	//fclose(f);
	//mycp::uint32 nTimestamp = timeGetTime();
	//theEBRD->SendData(lpszBuffer,nSize,nTimestamp,SOTP_RTP_NAK_DATA_AUDIO,SOTP_RTP_NAK_REQUEST_1);

	//Sleep(100);
	//nTimestamp = timeGetTime();
	//theEBRD->SendData(DEFAULT_ROOM_ID,lpszBuffer,nSize,nTimestamp,SOTP_RTP_NAK_DATA_AUDIO,SOTP_RTP_NAK_REQUEST_1);
	//theEBRD->SendData(DEFAULT_ROOM_ID,(const unsigned char*)"123",3);
}

void CebdDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CString sString;
	this->GetDlgItem(IDC_EDIT_DESTID)->GetWindowText(sString);
	const mycp::bigint nDestId = cgc_atoi64(sString);
	if (nDestId==0)
	{
		return;
	}

	theEBRD->MoveSinkWindow(nDestId, 0, 0, cx, cy, TRUE);
}

void CebdDlg::OnBnClickedButton4()
{
	theEBRD->StopLocalDesktop();
}

BOOL CebdDlg::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->hwnd == m_imageStatic.GetSafeHwnd())

	if (!this->IsIconic())
	{
		switch (pMsg->message)
		{
			//case WM_SYSKEYDOWN:
			//	{
			//		int i=0;
			//	}break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			{
				int i=0;
			}break;
		default:
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CebdDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}
