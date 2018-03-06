// PanelSearch.cpp : implementation file
//

#include "stdafx.h"
#include "EBUIApp.h"
#include "PanelSearch.h"


// CPanelSearch dialog

IMPLEMENT_DYNAMIC(CPanelSearch, CEbDialogBase)

CPanelSearch::CPanelSearch(CWnd* pParent /*=NULL*/)
	: CEbDialogBase(CPanelSearch::IDD, pParent)
{

}

CPanelSearch::~CPanelSearch()
{
}

void CPanelSearch::DoDataExchange(CDataExchange* pDX)
{
	CEbDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_GOBACK, m_btnGoBack);
	DDX_Control(pDX, IDC_BUTTON_GOFORWARD, m_btnGoForward);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_BUTTON_OPTIONS, m_btnOptions);
}


BEGIN_MESSAGE_MAP(CPanelSearch, CEbDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPanelSearch::OnBnClickedButtonRefresh)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_OPTIONS, &CPanelSearch::OnBnClickedButtonOptions)
	ON_BN_CLICKED(IDC_BUTTON_GOBACK, &CPanelSearch::OnBnClickedButtonGoback)
	ON_BN_CLICKED(IDC_BUTTON_GOFORWARD, &CPanelSearch::OnBnClickedButtonGoforward)
	ON_WM_DESTROY()
	ON_COMMAND(EB_COMMAND_CLEAR_IE_TEMPFILE, &OnCmdClearIeTempFile)
	ON_COMMAND(EB_COMMAND_CLEAR_HISROTY, &OnCmdClearHistory)
END_MESSAGE_MAP()


// CPanelSearch message handlers

void CPanelSearch::OnBnClickedButtonRefresh()
{
	this->GetParent()->PostMessage(EB_COMMAND_REFRESH_URL);
}

void CPanelSearch::OnSize(UINT nType, int cx, int cy)
{
	CEbDialogBase::OnSize(nType, cx, cy);
	int x = 0;
	m_btnGoBack.MovePoint(x,0);
	x += 18+2;
	m_btnGoForward.MovePoint(x,0);
	x += 18+2;
	m_btnRefresh.MovePoint(x,0);
	x += 18+2;
	m_btnOptions.MovePoint(x,0);
}

BOOL CPanelSearch::OnInitDialog()
{
	CEbDialogBase::OnInitDialog();
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW,0);

	//this->SetBgColor(RGB(255,255,255));
	m_btnGoBack.Load(IDB_PNG_BTN_GOBACK72X18);
	m_btnGoBack.SetWindowText(_T(""));
	m_btnGoBack.SetToolTipText(_T("转到上一页"));
	m_btnGoForward.Load(IDB_PNG_BTN_GOFORWARD72X18);
	m_btnGoForward.SetWindowText(_T(""));
	m_btnGoForward.SetToolTipText(_T("转到下一页"));
	m_btnRefresh.Load(IDB_PNG_BTN_REFRESH72X18);
	m_btnRefresh.SetWindowText(_T(""));
	m_btnRefresh.SetToolTipText(_T("刷新（F5）"));
	m_btnOptions.Load(IDB_PNG_BTN_ARROWDOWN72X18);
	m_btnOptions.SetWindowText(_T(""));
	m_btnOptions.SetToolTipText(_T("选项"));

	this->SetCircle(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelSearch::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	CRect rectClient;
	this->GetClientRect(&rectClient);
	CSkinMemDC memDC(&dc, rectClient);
	this->ClearBgDrawInfo();
	this->DrawPopBg(&memDC, RGB(255,255,255),0);
}

void CPanelSearch::OnBnClickedButtonOptions()
{
	if (m_pOptionsMenu.GetSafeHmenu()==NULL)
	{
		m_pOptionsMenu.CreatePopupMenu();
		m_pOptionsMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CLEAR_IE_TEMPFILE,_T("清除IE历史记录"));
		m_pOptionsMenu.AppendMenu(MF_BYCOMMAND,EB_COMMAND_CLEAR_HISROTY,_T("清除浏览记录"));
	}
	CString sSql;
	sSql.Format(_T("SELECT last_time FROM url_record_t LIMIT 1;"));
	const bool bExistHistoryUrl = theApp.m_pBoUsers->select(sSql)>0?true:false;
	m_pOptionsMenu.EnableMenuItem(EB_COMMAND_CLEAR_HISROTY,bExistHistoryUrl?MF_BYCOMMAND|MF_ENABLED:MF_BYCOMMAND|MF_GRAYED);

	CPoint point;
	GetCursorPos(&point);
	m_pOptionsMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,point.x,point.y,this);
}

void CPanelSearch::OnBnClickedButtonGoback()
{
	this->GetParent()->PostMessage(EB_COMMAND_GO_BACK);
}

void CPanelSearch::OnBnClickedButtonGoforward()
{
	this->GetParent()->PostMessage(EB_COMMAND_GO_FORWARD);
}

void CPanelSearch::OnDestroy()
{
	if (m_pOptionsMenu.GetSafeHmenu()!=NULL)
		m_pOptionsMenu.DestroyMenu();

	CEbDialogBase::OnDestroy();
}
bool DeleteDirectory(const char* strDirName,bool bDeleteRootDir)
{
	CFileFind tempFind;
	char strTempFileFind[MAX_PATH];
	sprintf(strTempFileFind,"%s\\*.*", strDirName);
	BOOL IsFinded = tempFind.FindFile(strTempFileFind);
	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFile();
		if (!tempFind.IsDots()) 
		{
			if (tempFind.IsDirectory())
			{
				if (!DeleteDirectory(tempFind.GetFilePath(),true))
					continue;//return false;
			}
			else
			{
				if (!DeleteFile(tempFind.GetFilePath()))
					continue;
			}
		}
	}
	tempFind.Close();
	if (!bDeleteRootDir) return true;

	if(!RemoveDirectory(strDirName))
	{
		return false;
	}
	return true;
}
//#include "wininet.h"
//#include "UrlHist.h"
//
////BOOL DeleteUrlCache(DEL_CACHE_TYPE type);
////BOOL EmptyDirectory(LPCTSTR szPath, BOOL bDeleteDesktopIni = FALSE, BOOL bWipeIndexDat = FALSE);
////清空一个目录BOOL IsWindowsNT();
////判断系统BOOL IsWindows2k();BOOL GetUserSid(PSID* ppSid);
////void GetSidString(PSID pSid, LPTSTR szBuffer);
////BOOL GetOldSD(HKEY hKey, LPCTSTR pszSubKey, BYTE** pSD);
////BOOL CreateNewSD(PSID pSid, SECURITY_DESCRIPTOR* pSD, PACL* ppDacl);
////BOOL RegSetPrivilege(HKEY hKey, LPCTSTR pszSubKey, SECURITY_DESCRIPTOR* pSD, BOOL bRecursive);
////BOOL WipeFile(LPCTSTR szDir, LPCTSTR szFile); 
//BOOL DelTempFiles(void)
//{
//	BOOL bResult = FALSE;
//	BOOL bDone = FALSE;
//	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;
//	DWORD dwTrySize, dwEntrySize = 4096;	// start buffer size
//	HANDLE hCacheDir = NULL;
//	DWORD dwError = ERROR_INSUFFICIENT_BUFFER;
//	do {
//		switch (dwError)
//		{
//			// need a bigger buffer 
//		case ERROR_INSUFFICIENT_BUFFER:
//			delete [] lpCacheEntry;
//			lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
//			lpCacheEntry->dwStructSize = dwEntrySize;
//			dwTrySize = dwEntrySize;
//			BOOL bSuccess;
//			if (hCacheDir == NULL)
//				bSuccess = (hCacheDir = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwTrySize)) != NULL;
//			else
//				bSuccess = FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize);
//			if (bSuccess)
//				dwError = ERROR_SUCCESS;
//			else
//			{
//				dwError = GetLastError();
//				dwEntrySize = dwTrySize;
//				// use new size returned
//			}break;
//			// we are done 
//		case ERROR_NO_MORE_ITEMS:
//			bDone = TRUE;
//			bResult = TRUE;
//			break;
//			// we have got an entry 
//		case ERROR_SUCCESS: // don''''''''''''''''t delete cookie entry 
//			if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
//				DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName); // get ready for next entry
//			dwTrySize = dwEntrySize;
//			if (FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize))
//				dwError = ERROR_SUCCESS;
//			else
//			{
//				dwError = GetLastError();
//				dwEntrySize = dwTrySize; // use new size returned 
//			}break; // unknown error 
//		default:
//			bDone = TRUE;
//			break;
//		}
//		if (bDone)
//		{
//			delete []lpCacheEntry;
//			if (hCacheDir)
//				FindCloseUrlCache(hCacheDir);
//		}
//	} while (!bDone);
//	return bResult;
//} 

void CPanelSearch::OnCmdClearIeTempFile()
{
	// 清除IE临时文件
	//其中ClearMyTracksByProcess 可进行选择设置 ：
	//Temporary Internet Files  （Internet临时文件）
	//RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 8
	//Cookies
	//RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 2
	//History (历史记录)
	//RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 1
	// Form. Data （表单数据）
	//RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 16
	// Passwords (密码）
	//RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 32
	// Delete All  （全部删除）
	//RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 255
	//
	//Delete All - "Also delete files and settings stored by add-ons"
	//
	//    RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 4351

	ShellExecute(NULL, "open", "rundll32.exe", " InetCpl.cpl,ClearMyTracksByProcess 255", "", SW_HIDE);

	//// 1、清除IE临时文件
	//TCHAR szPath[MAX_PATH];
	//if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_INTERNET_CACHE, FALSE))
	//{
	//	//得到临时目录，并清空它.
	//	DeleteDirectory(szPath,true);
	//}
	////DelTempFiles();
	//// 2、Cookie的清除
	//if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_COOKIES, FALSE))
	//{
	//	//得到目录，并清空 
	//	DeleteDirectory(szPath,true);
	//}

}

void CPanelSearch::OnCmdClearHistory()
{
	CString sSql;
	sSql.Format(_T("DELETE FROM url_record_t;"));
	theApp.m_pBoUsers->execute(sSql);
	//DeleteDirectory(theApp.GetAppImgTempPath(),false);
}
