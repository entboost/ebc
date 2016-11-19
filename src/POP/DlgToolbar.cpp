// DlgToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "POP.h"
#include "DlgToolbar.h"


// CDlgToolbar dialog

IMPLEMENT_DYNAMIC(CDlgToolbar, CEbDialogBase)

CDlgToolbar::CDlgToolbar(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CDlgToolbar::IDD, pParent)
{
	m_nDataType = EB_MR_CTRL_DATA_TYPE_URL;
	m_nButtonCount = 0;
	m_hMsgWnd = NULL;
	m_pCallback = NULL;
	m_bChildMode = false;
}

CDlgToolbar::~CDlgToolbar()
{
}

void CDlgToolbar::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_1, m_btnOpen1);
	DDX_Control(pDX, IDC_BTN_2, m_btnOpen2);
	DDX_Control(pDX, IDC_BTN_3, m_btnOpen3);
	DDX_Control(pDX, IDC_BTN_4, m_btnOpen4);
}


BEGIN_MESSAGE_MAP(CDlgToolbar, CEbDialogBase)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_1, &CDlgToolbar::OnBnClickedBtn1)
	ON_BN_CLICKED(IDC_BTN_2, &CDlgToolbar::OnBnClickedBtn2)
	ON_BN_CLICKED(IDC_BTN_3, &CDlgToolbar::OnBnClickedBtn3)
	ON_BN_CLICKED(IDC_BTN_4, &CDlgToolbar::OnBnClickedBtn4)
END_MESSAGE_MAP()

void CDlgToolbar::SetCtrlColor(bool bInvalidate)
{
	//m_btnOpen1.SetDrawPanel(true,theDefaultBtnWhiteColor,theApp.GetBgColor(),theApp.GetMainColor());
	//m_btnOpen1.SetDrawLine(5,1,0,theDefaultFlatLine2Color,theApp.GetMainColor(),theApp.GetPreColor());
	//m_btnOpen2.SetDrawLine(5,1,0,theDefaultFlatLine2Color,theApp.GetMainColor(),theApp.GetPreColor());
	//m_btnOpen3.SetDrawLine(5,1,0,theDefaultFlatLine2Color,theApp.GetMainColor(),theApp.GetPreColor());
	//m_btnOpen4.SetDrawLine(5,1,0,theDefaultFlatLine2Color,theApp.GetMainColor(),theApp.GetPreColor());
	m_btnOpen1.SetHotTextColor(theApp.GetMainColor());
	m_btnOpen1.SetPreTextColor(theApp.GetPreColor());
	m_btnOpen2.SetHotTextColor(theApp.GetMainColor());
	m_btnOpen2.SetPreTextColor(theApp.GetPreColor());
	m_btnOpen3.SetHotTextColor(theApp.GetMainColor());
	m_btnOpen3.SetPreTextColor(theApp.GetPreColor());
	m_btnOpen4.SetHotTextColor(theApp.GetMainColor());
	m_btnOpen4.SetPreTextColor(theApp.GetPreColor());

	if (bInvalidate)
		this->Invalidate();
}
int CDlgToolbar::SetMoveEnterData(EB_MR_CTRL_DATA_TYPE nDataType, const std::string& sString, bool& pOutChangeData)
{
	pOutChangeData = m_nDataType != nDataType || m_sMoveEnterString != sString;
	if (!pOutChangeData) return 4;
	m_nDataType = nDataType;
	m_sMoveEnterString = sString;
	m_nButtonCount = 4;
	switch (m_nDataType)
	{
	case EB_MR_CTRL_DATA_TYPE_URL:
		{
			if (!theApp.GetHideMainFrame()) 
			{
				m_btnOpen1.SetWindowText(_T("工作台"));
				m_btnOpen1.SetToolTipText(_T("在工作台中打开链接"));
				m_btnOpen1.ShowWindow(SW_SHOW);
			}else
			{
				m_nButtonCount -= 1;
				m_btnOpen1.ShowWindow(SW_HIDE);
			}
			m_btnOpen2.SetWindowText(_T("浏览器"));
			m_btnOpen2.SetToolTipText(_T("在浏览器中打开链接"));
			m_btnOpen2.ShowWindow(SW_SHOW);
			if (m_bChildMode)
			{
				m_btnOpen3.SetWindowText(_T("右侧窗口"));
				m_btnOpen3.SetToolTipText(_T("在聊天右侧窗口中打开链接"));
				m_btnOpen3.ShowWindow(SW_SHOW);
			}else
			{
				m_nButtonCount -= 1;
				m_btnOpen3.ShowWindow(SW_HIDE);
			}
			m_btnOpen4.SetWindowText(_T("复制"));
			m_btnOpen4.SetToolTipText(_T("复制链接地址"));
			m_btnOpen4.ShowWindow(SW_SHOW);
		}break;
	case EB_MR_CTRL_DATA_TYPE_IMAGE:
		{
			m_btnOpen1.SetWindowText(_T("打开"));
			m_btnOpen1.SetToolTipText(_T("打开图片"));
			m_btnOpen2.SetWindowText(_T("打开目录"));
			m_btnOpen2.SetToolTipText(_T("打开图片文件存放目录"));
			m_btnOpen3.SetWindowText(_T("复制"));
			m_btnOpen3.SetToolTipText(_T("复制图片到粘贴板"));
			m_btnOpen4.SetWindowText(_T("另存为..."));
			m_btnOpen4.SetToolTipText(_T("图片另存为..."));
			m_btnOpen1.ShowWindow(SW_SHOW);
			m_btnOpen2.ShowWindow(SW_SHOW);
			m_btnOpen3.ShowWindow(SW_SHOW);
			m_btnOpen4.ShowWindow(SW_SHOW);
		}break;
	case EB_MR_CTRL_DATA_TYPE_FILE:
		{
			m_btnOpen1.SetWindowText(_T("打开"));
			m_btnOpen1.SetToolTipText(_T("打开文件"));
			m_btnOpen2.SetWindowText(_T("打开目录"));
			m_btnOpen2.SetToolTipText(_T("打开文件存放目录"));
			m_btnOpen3.SetWindowText(_T("复制"));
			m_btnOpen3.SetToolTipText(_T("复制文件到粘贴板"));
			m_btnOpen4.SetWindowText(_T("另存为..."));
			m_btnOpen4.SetToolTipText(_T("文件另存为..."));
			m_btnOpen1.ShowWindow(SW_SHOW);
			m_btnOpen2.ShowWindow(SW_SHOW);
			m_btnOpen3.ShowWindow(SW_SHOW);
			m_btnOpen4.ShowWindow(SW_SHOW);
		}break;
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			m_nButtonCount = 2;
			m_btnOpen3.SetWindowText(_T("复制"));
			m_btnOpen3.SetToolTipText(_T("复制选择文本到粘贴板"));
			m_btnOpen4.SetWindowText(_T("另存为..."));
			m_btnOpen4.SetToolTipText(_T("选择文本另存到文件"));
			m_btnOpen1.ShowWindow(SW_HIDE);
			m_btnOpen2.ShowWindow(SW_HIDE);
			m_btnOpen3.ShowWindow(SW_SHOW);
			m_btnOpen4.ShowWindow(SW_SHOW);

			if (theApp.BuildChatCopySubscribeFuncList()>0)
			{
				for (size_t i=0;i<theApp.m_pChatCopySubscribeFuncList.size();i++)
				{
					const EB_SubscribeFuncInfo & pSubscribeFuncInfo = theApp.m_pChatCopySubscribeFuncList[i];
					if (i==0)
					{
						m_nButtonCount++;
						m_btnOpen1.SetWindowText(pSubscribeFuncInfo.m_sFunctionName.c_str());
						CString sTipText;
						sTipText.Format(_T("选择文本提交%s"),pSubscribeFuncInfo.m_sFunctionName.c_str());
						m_btnOpen1.SetToolTipText(sTipText);
						m_btnOpen1.ShowWindow(SW_SHOW);
					}
					else if (i==1)
					{
						m_nButtonCount++;
						m_btnOpen2.SetWindowText(pSubscribeFuncInfo.m_sFunctionName.c_str());
						CString sTipText;
						sTipText.Format(_T("选择文本提交%s"),pSubscribeFuncInfo.m_sFunctionName.c_str());
						m_btnOpen2.SetToolTipText(sTipText);
						m_btnOpen2.ShowWindow(SW_SHOW);
					}
					else
					{
						break;
					}
				}
			}

		}break;
	default:
		break;
	}
	m_btnOpen1.Invalidate();
	m_btnOpen2.Invalidate();
	m_btnOpen3.Invalidate();
	m_btnOpen4.Invalidate();
	theApp.InvalidateParentRect(&m_btnOpen1);
	theApp.InvalidateParentRect(&m_btnOpen2);
	theApp.InvalidateParentRect(&m_btnOpen3);
	theApp.InvalidateParentRect(&m_btnOpen4);
	return m_nButtonCount;
}
void CDlgToolbar::SetMsgHwnd(HWND hWnd, CToolbarCallback* pCallback)
{
	m_hMsgWnd = hWnd;
	m_pCallback = pCallback;
}
void CDlgToolbar::HideReset(void)
{
	if (this->GetSafeHwnd()!=NULL)
	{
		ShowWindow(SW_HIDE);
		m_nDataType = EB_MR_CTRL_DATA_TYPE_URL;
		m_sMoveEnterString.clear();
	}
}
void CDlgToolbar::SetChildMode(bool bChildMode)
{
	m_bChildMode = bChildMode;
}

// CDlgToolbar message handlers

BOOL CDlgToolbar::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here

	this->GetDlgItem(IDC_BUTTON_NULL)->MoveWindow(0,0,0,0);
	m_btnOpen1.ShowWindow(SW_SHOW);
	m_btnOpen1.SetAutoSize(false);
	m_btnOpen1.SetTextHotMove(true);
	m_btnOpen1.SetDrawPanelRgn(false);
	m_btnOpen1.SetNorTextColor(theDefaultFlatBlackTextColor);
	m_btnOpen2.ShowWindow(SW_SHOW);
	m_btnOpen2.SetAutoSize(false);
	m_btnOpen2.SetTextHotMove(true);
	m_btnOpen2.SetDrawPanelRgn(false);
	m_btnOpen2.SetNorTextColor(theDefaultFlatBlackTextColor);
	m_btnOpen3.ShowWindow(SW_SHOW);
	m_btnOpen3.SetAutoSize(false);
	m_btnOpen3.SetTextHotMove(true);
	m_btnOpen3.SetDrawPanelRgn(false);
	m_btnOpen3.SetNorTextColor(theDefaultFlatBlackTextColor);
	m_btnOpen4.ShowWindow(SW_SHOW);
	m_btnOpen4.SetAutoSize(false);
	m_btnOpen4.SetTextHotMove(true);
	m_btnOpen4.SetDrawPanelRgn(false);
	m_btnOpen4.SetNorTextColor(theDefaultFlatBlackTextColor);

	SetCtrlColor(false);

	//const int POS_DLG_LOGIN_WIDTH = 80;
	//const int POS_DLG_LOGIN_HEIGHT = 42;
	//CRect rectClient;
	//this->GetWindowRect(&rectClient);
	//rectClient.right = rectClient.left + POS_DLG_LOGIN_WIDTH;
	//rectClient.bottom = rectClient.top + POS_DLG_LOGIN_HEIGHT;
	//this->MoveWindow(&rectClient);
	SetCircle();

	SetDlgChildFont(theDefaultDialogFontSize,theFontFace.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgToolbar::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CEbDialogBase::OnClose();
}

void CDlgToolbar::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CEbDialogBase::OnTimer(nIDEvent);
}
const int const_left_interval = 8;
const int const_top_interval = 8;
const int const_btn_width1 = 55;
const int const_btn_width2 = 55;
const int const_btn_height = 20;

void CDlgToolbar::OnPaint()
{
	if (IsIconic())
	{
		//CPaintDC dc(this); // 用于绘制的设备上下文

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// 使图标在工作区矩形中居中
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// 绘制图标
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rectClient;
		this->GetClientRect(&rectClient);
		CSkinMemDC memDC(&dc, rectClient);
		this->ClearBgDrawInfo();
		this->AddBgDrawInfo(CEbBackDrawInfo(0,1.0,false,false,0,theDefaultBtnWhiteColor));
		this->DrawPopBg(&memDC, theApp.GetMainColor(),1,0,0,theDefaultFlatLineColor);

		//Gdiplus::Pen pen(Gdiplus::Color(64,64,64));
		//Gdiplus::Graphics graphics(memDC.m_hDC);
		//if (m_nButtonCount>=2)
		//{
		//	const int x = const_left_interval+const_btn_width1*1+1;
		//	graphics.DrawLine(&pen,x,const_top_interval+2,x,const_top_interval+12);
		//}
		//if (m_nButtonCount>=3)
		//{
		//	const int x = const_left_interval+const_btn_width1*2+1;
		//	graphics.DrawLine(&pen,x,const_top_interval+2,x,const_top_interval+12);
		//}
		//if (m_nButtonCount>=4)
		//{
		//	const int x = const_left_interval+const_btn_width1*3+1;
		//	graphics.DrawLine(&pen,x,const_top_interval+2,x,const_top_interval+12);
		//}
	}
}

void CDlgToolbar::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);

	int x = const_left_interval;
	int y = const_top_interval;
	const int const_btn_interval = 1;
	switch (m_nDataType)
	{
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			const size_t nChatCopyFuncSize = theApp.m_pChatCopySubscribeFuncList.size();
			if (nChatCopyFuncSize>=1)
			{
				m_btnOpen1.MovePoint(x,y,const_btn_width2,const_btn_height);
				x += (const_btn_width2+const_btn_interval);
			}
			if (nChatCopyFuncSize>=2)
			{
				m_btnOpen2.MovePoint(x,y,const_btn_width2,const_btn_height);
				x += (const_btn_width2+const_btn_interval);
			}
			m_btnOpen3.MovePoint(x,y,const_btn_width2,const_btn_height);
			x += (const_btn_width2+const_btn_interval);
			m_btnOpen4.MovePoint(x,y,const_btn_width2,const_btn_height);

		}break;
	default:
		{
			const bool bHideBtn1 = (theApp.GetHideMainFrame()&&m_nDataType==EB_MR_CTRL_DATA_TYPE_URL)?true:false;
			const bool bHideBtn3 = (!m_bChildMode&&m_nDataType==EB_MR_CTRL_DATA_TYPE_URL)?true:false;
			if (!bHideBtn1)
			{
				m_btnOpen1.MovePoint(x,y,const_btn_width1,const_btn_height);
				x += (const_btn_width1+const_btn_interval);
			}
			m_btnOpen2.MovePoint(x,y,const_btn_width1,const_btn_height);
			x += (const_btn_width1+const_btn_interval);
			if (!bHideBtn3)
			{
				m_btnOpen3.MovePoint(x,y,const_btn_width2,const_btn_height);
				x += (const_btn_width2+const_btn_interval);
			}
			m_btnOpen4.MovePoint(x,y,const_btn_width2,const_btn_height);
		}break;
	}
}

void CDlgToolbar::OnBnClickedBtn1()
{
	if (m_sMoveEnterString.empty() && m_nDataType!=EB_MR_CTRL_DATA_TYPE_SELECT_STRING)
		return;

	switch (m_nDataType)
	{
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			const tstring sSelectString(m_pCallback->GetSelectString());
			if (!sSelectString.empty() && theApp.m_pChatCopySubscribeFuncList.size()>=1)
			{
				const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pChatCopySubscribeFuncList[0];
				if (theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,libEbc::ACP2UTF8(sSelectString.c_str()),"",NULL))
				{
					HideReset();
				}
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_URL:
		{
			COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(m_sMoveEnterString.string(),"");
			pOpenAppUrlInfo->m_bSaveBrowseTitle = true;
			theApp.m_pMainWnd->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
			HideReset();
		}break;
	case EB_MR_CTRL_DATA_TYPE_IMAGE:
		{
			if (::PathFileExists(m_sMoveEnterString.c_str()))
			{
				if (m_sMoveEnterString.find(".")==std::string::npos)
				{
					const CString& strSystemDir = theApp.GetSysPath();
					const CString strRundll = strSystemDir + "\\rundll32.exe";
					CString strParm;
					strParm.Format("%s//shimgvw.dll imageview_fullscreen %s", strSystemDir, m_sMoveEnterString.c_str());
					COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo((LPCTSTR)strRundll,(LPCTSTR)strParm,"");
					theApp.m_pMainWnd->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
					//ShellExecute(NULL, "Open", strRundll, strParm, NULL, SW_SHOW);
				}else
				{
					COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(m_sMoveEnterString.string(),"",m_sMoveEnterString.string());
					theApp.m_pMainWnd->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
					//ShellExecute(NULL, "open", m_sMoveEnterString.c_str(), NULL, m_sMoveEnterString.c_str(),SW_SHOW);
				}
			}else
			{
				CString sText;
				sText.Format(_T("图片文件不存在：\r\n%s"),m_sMoveEnterString.c_str());
				CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
			}
			HideReset();
		}break;
	case EB_MR_CTRL_DATA_TYPE_FILE:
		{
			if (::PathFileExists(m_sMoveEnterString.c_str()))
			{
				COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(m_sMoveEnterString.string(),"",m_sMoveEnterString.string());
				theApp.m_pMainWnd->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
				//ShellExecute(NULL, "open", m_sMoveEnterString.c_str(), NULL, m_sMoveEnterString.c_str(),SW_SHOW);
			}else
			{
				CString sText;
				sText.Format(_T("指定文件不存在：\r\n%s"),m_sMoveEnterString.c_str());
				CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
			}
			HideReset();
		}break;
	default:
		break;
	}
	this->GetDlgItem(IDC_BUTTON_NULL)->SetFocus();
}
int CopyFileToClipboard(const char szFileName[])
{
	UINT uDropEffect;
	HGLOBAL hGblEffect;
	LPDWORD lpdDropEffect;
	DROPFILES stDrop;

	HGLOBAL hGblFiles;
	LPSTR lpData;

	uDropEffect = RegisterClipboardFormat("Preferred DropEffect");
	hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,sizeof(DWORD));
	lpdDropEffect = (LPDWORD)GlobalLock(hGblEffect);
	*lpdDropEffect = DROPEFFECT_COPY;//复制; 剪贴则用DROPEFFECT_MOVE
	GlobalUnlock(hGblEffect);

	stDrop.pFiles = sizeof(DROPFILES);
	stDrop.pt.x = 0;
	stDrop.pt.y = 0;
	stDrop.fNC = FALSE;
	stDrop.fWide = FALSE;

	hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,\
		sizeof(DROPFILES)+strlen(szFileName)+2);
	lpData = (LPSTR)GlobalLock(hGblFiles);
	memcpy(lpData,&stDrop,sizeof(DROPFILES));
	strcpy(lpData+sizeof(DROPFILES),szFileName);
	GlobalUnlock(hGblFiles);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_HDROP,hGblFiles);
	SetClipboardData(uDropEffect,hGblEffect);
	CloseClipboard();

	return 1;
}
void CDlgToolbar::OnBnClickedBtn2()
{
	if (m_sMoveEnterString.empty() && m_nDataType!=EB_MR_CTRL_DATA_TYPE_SELECT_STRING)
		return;
	switch (m_nDataType)
	{
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			const tstring sSelectString(m_pCallback->GetSelectString());
			if (!sSelectString.empty() && theApp.m_pChatCopySubscribeFuncList.size()>=2)
			{
				const EB_SubscribeFuncInfo& pSubscribeFuncInfo = theApp.m_pChatCopySubscribeFuncList[1];
				if (theApp.OpenSubscribeFuncWindow(pSubscribeFuncInfo,libEbc::ACP2UTF8(sSelectString.c_str()),"",NULL))
				{
					HideReset();
				}
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_URL:
		{
			COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(m_sMoveEnterString.string(),"",m_sMoveEnterString.string());
			theApp.m_pMainWnd->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
			//ShellExecute(NULL, "open", m_sMoveEnterString.c_str(), NULL, m_sMoveEnterString.c_str(),SW_SHOW);
			HideReset();
		}break;
	case EB_MR_CTRL_DATA_TYPE_IMAGE:
	case EB_MR_CTRL_DATA_TYPE_FILE:
		{
			if (::PathFileExists(m_sMoveEnterString.c_str()))
			{
				const tstring sPath = "/select, "+m_sMoveEnterString;
				//COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo("explorer",sPath.string(),"");
				//theApp.m_pMainWnd->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
				ShellExecute(NULL, NULL, "explorer", sPath.c_str(), NULL, SW_SHOW);
			}else
			{
				const std::string::size_type find = m_sMoveEnterString.rfind('\\');
				if (find!=std::wstring::npos)
				{
					const tstring sPath = m_sMoveEnterString.substr(0,find+1);
					//COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(sPath.string(),"",sPath.string());
					//theApp.m_pMainWnd->PostMessage(EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
					ShellExecute(NULL, "open", sPath.c_str(), NULL, sPath.c_str(), SW_SHOW);
				}
			}
			HideReset();
		}break;
	default:
		break;
	}
	this->GetDlgItem(IDC_BUTTON_NULL)->SetFocus();
}
void CDlgToolbar::OnBnClickedBtn3()
{
	switch (m_nDataType)
	{
	case EB_MR_CTRL_DATA_TYPE_URL:
		{
			if (m_sMoveEnterString.empty())
				return;
			COpenAppUrlInfo * pOpenAppUrlInfo = new COpenAppUrlInfo(m_sMoveEnterString.string(),"");
			pOpenAppUrlInfo->m_bSaveBrowseTitle = true;
			::PostMessage(m_hMsgWnd,EB_COMMAND_OPEN_APP_URL,(WPARAM)pOpenAppUrlInfo,(LPARAM)0);
			HideReset();
		}break;
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			const tstring sSelectString(m_pCallback->GetSelectString());
			if( OpenClipboard() )
			{
				EmptyClipboard();
				HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, sSelectString.size()+1);
				char * buffer = (char*)GlobalLock(clipbuffer);
				strcpy(buffer, sSelectString.c_str());
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT,clipbuffer);
				CloseClipboard();
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_IMAGE:
		{
			// 复制图片；
			if (!::PathFileExists(m_sMoveEnterString.c_str()))
			{
				CString sText;
				sText.Format(_T("图片文件不存在：\r\n%s"),m_sMoveEnterString.c_str());
				CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
				break;
			}
			if ( OpenClipboard() )
			{
				USES_CONVERSION;
				Gdiplus::Bitmap * pImage = Gdiplus::Bitmap::FromFile(A2W(m_sMoveEnterString.c_str()),FALSE);
				if (pImage!=NULL)
				{
					EmptyClipboard();
					// **下面支持复制 WORD QQ 等其他程序；
					PAINTSTRUCT ps;
					HDC hDC = ::BeginPaint(this->GetSafeHwnd(),&ps);
					HDC dcMem = ::CreateCompatibleDC(hDC);
					HBITMAP bitmap = ::CreateCompatibleBitmap(hDC,pImage->GetWidth(),pImage->GetHeight());
					SelectObject(dcMem,(HGDIOBJ)bitmap);
					Gdiplus::Graphics graphics(dcMem);
					graphics.DrawImage(pImage,0,0,pImage->GetWidth(),pImage->GetHeight());
					SetClipboardData(CF_BITMAP,bitmap);
					CloseClipboard();
					EndPaint(&ps);
					DeleteObject(bitmap);
					DeleteDC(dcMem);
					delete pImage;
				}else
				{
					CloseClipboard();
					CDlgMessageBox::EbMessageBox(this,"","复制图片失败：\r\n请重试！",CDlgMessageBox::IMAGE_WARNING,5);
				}
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_FILE:
		{
			// 复制文件
			if (!::PathFileExists(m_sMoveEnterString.c_str()))
			{
				CString sText;
				sText.Format(_T("指定文件不存在：\r\n%s"),m_sMoveEnterString.c_str());
				CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
				break;
			}
			CopyFileToClipboard(m_sMoveEnterString.c_str());
		}break;
	default:
		break;
	}
	this->GetDlgItem(IDC_BUTTON_NULL)->SetFocus();
}
void CDlgToolbar::OnBnClickedBtn4()
{
	switch (m_nDataType)
	{
	case EB_MR_CTRL_DATA_TYPE_URL:
		{
			if (m_sMoveEnterString.empty())
				return;
			if( OpenClipboard() )
			{
				EmptyClipboard();
				HGLOBAL clipbuffer;
				clipbuffer = GlobalAlloc(GMEM_DDESHARE, m_sMoveEnterString.size()+1);
				char * buffer = (char*)GlobalLock(clipbuffer);
				strcpy(buffer, m_sMoveEnterString.c_str());
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT,clipbuffer);
				CloseClipboard();
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_IMAGE:
		{
			if (!::PathFileExists(m_sMoveEnterString.c_str()))
			{
				CString sText;
				sText.Format(_T("图片文件不存在：\r\n%s"),m_sMoveEnterString.c_str());
				CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
				break;
			}
			const tstring sStringTemp(m_sMoveEnterString);

			// 图片另存
			wchar_t szFileName[MAX_PATH*2] = {0} ; 
			const time_t tNow = time(0);
			const struct tm *newtime = localtime(&tNow);
			_swprintf(szFileName,L"EB图片%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);

			OPENFILENAMEW ofn={0};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hWnd;
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = sizeof(szFileName);
			ofn.lpstrFilter = L"PNG (*.png)\0*.png\0JPEG (*.jpg)\0*.jpg\0BMP (*.bmp)\0*.bmp\0\0";
			ofn.lpstrDefExt = L"png";
			ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
			if (GetSaveFileNameW(&ofn)==TRUE ) 
			{
				std::wstring sExt(ofn.lpstrFile);
				sExt = sExt.substr(sExt.size()-3);
				CLSID                           codecClsid;
				if (sExt==L"png" || sExt==L"PNG")
					libEbc::GetCodecClsid(L"image/png",   &codecClsid);
				else if (sExt==L"bmp" || sExt==L"BMP")
					libEbc::GetCodecClsid(L"image/bmp",   &codecClsid);
				else
					libEbc::GetCodecClsid(L"image/jpeg",   &codecClsid);

				USES_CONVERSION;
				Gdiplus::Image   image(A2W(sStringTemp.c_str()));
				image.Save(ofn.lpstrFile, &codecClsid);
				HideReset();
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_FILE:
		{
			if (!::PathFileExists(m_sMoveEnterString.c_str()))
			{
				CString sText;
				sText.Format(_T("指定文件不存在：\r\n%s"),m_sMoveEnterString.c_str());
				CDlgMessageBox::EbMessageBox(this,"",sText,CDlgMessageBox::IMAGE_WARNING,5);
				break;
			}
			const tstring sStringTemp(m_sMoveEnterString);
			const tstring sFileName(libEbc::GetFileName(sStringTemp));
			tstring sNameTemp;
			tstring sFileExt;
			libEbc::GetFileExt(sFileName,sNameTemp,sFileExt);

			// 文件另存
			char szFileName[MAX_PATH*2] = {0} ; 
			strcpy(szFileName,sFileName.c_str());
			char sFilter[120];
			if (sFileExt.empty())
				strcpy(sFilter,"All Files (*.*)\0*.*\0\0");
			else
			{
				std::transform(sFileExt.begin(), sFileExt.end(), sFileExt.begin(), tolower);
				tstring sFileExtUpper(sFileExt);
				std::transform(sFileExtUpper.begin(), sFileExtUpper.end(), sFileExtUpper.begin(), toupper);
				sprintf(sFilter,"%s (*.%s)\0*.%s\0All Files (*.*)\0*.*\0\0",sFileExtUpper.c_str(),sFileExt.c_str(),sFileExt.c_str());
			}

			OPENFILENAME ofn={0};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hWnd;
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = sizeof(szFileName);
			ofn.lpstrFilter = sFilter;
			ofn.lpstrDefExt = sFileExt.c_str();
			ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
			if (GetSaveFileName(&ofn)==TRUE ) 
			{
				CopyFile(sStringTemp.c_str(),ofn.lpstrFile,FALSE);
				HideReset();
			}
		}break;
	case EB_MR_CTRL_DATA_TYPE_SELECT_STRING:
		{
			const tstring sSelectString(m_pCallback->GetSelectString());
			if (sSelectString.empty())
			{
				//HideReset();
				break;
			}
			TCHAR szFileName[MAX_PATH*2] = {0} ; 
			const time_t tNow = time(0);
			const struct tm *newtime = localtime(&tNow);
			sprintf(szFileName,"EB文本%04d%02d%02d%02d%02d%02d",newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday,newtime->tm_hour,newtime->tm_min,newtime->tm_sec);

			OPENFILENAME ofn={0};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_hWnd;
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = sizeof(szFileName);
			ofn.lpstrFilter = "文本文件(*.txt)\0\0";
			ofn.lpstrDefExt = "txt";   
			ofn.Flags = OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
			if (GetSaveFileName(&ofn)==TRUE ) 
			{
				FILE * f = fopen(ofn.lpstrFile,"w");
				if (f!=NULL)
				{
					fwrite(sSelectString.c_str(),1,sSelectString.size(),f);
					fclose(f);
					// open it
					ShellExecute(NULL, "open", ofn.lpstrFile, NULL, NULL,SW_SHOW);
					HideReset();
				}
			}
		}break;
	default:
		break;
	}
	this->GetDlgItem(IDC_BUTTON_NULL)->SetFocus();
}
