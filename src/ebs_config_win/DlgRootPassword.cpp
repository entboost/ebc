// DlgRootPassword.cpp : implementation file
//

#include "stdafx.h"
#include "ebs_config_win.h"
#include "DlgRootPassword.h"


// CDlgRootPassword dialog

IMPLEMENT_DYNAMIC(CDlgRootPassword, CDialog)

CDlgRootPassword::CDlgRootPassword(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRootPassword::IDD, pParent)
	, m_sRootPassword(_T(""))
{

}

CDlgRootPassword::~CDlgRootPassword()
{
}

void CDlgRootPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROOTPWD, m_sRootPassword);
}


BEGIN_MESSAGE_MAP(CDlgRootPassword, CDialog)
END_MESSAGE_MAP()


// CDlgRootPassword message handlers
