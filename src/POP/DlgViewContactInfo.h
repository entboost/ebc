#pragma once
#include "afxwin.h"

// CDlgViewContactInfo dialog

class CDlgViewContactInfo : public CEbDialogBase
{
	DECLARE_DYNAMIC(CDlgViewContactInfo)

public:
	CDlgViewContactInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgViewContactInfo();

	enum VIEW_TYPE
	{
		VIEW_UNKNOWE
		, VIEW_ENTERPRISE
		, VIEW_GROUP
		, VIEW_MEMBER
		, VIEW_CONTACT
	};
	VIEW_TYPE GetViewType(void) const {return m_nViewType;}
	bool GetFirstInRect(void) const {return m_bFirstInRect;}
	void HideReset(void);
	void SetCheckLeave(void);
	void SetMoveEnter(void);
	void SetMoveLeave(void);
#ifdef USES_EBCOM_TEST
	void SetEnterpriseInfo(IEB_EnterpriseInfo* pEnterpriseInfo);
	void SetDepartemntInfo(IEB_GroupInfo* pGroupInfo,IEB_EnterpriseInfo* pEnterpriseInfo);
	void SetEmployeeInfo(IEB_MemberInfo* pMemberInfo,IEB_GroupInfo* pGroupInfo);
	void SetContactInfo(IEB_ContactInfo* pContactInfo);
#else
	void SetEnterpriseInfo(const EB_EnterpriseInfo* pEnterpriseInfo);
	void SetDepartemntInfo(const EB_GroupInfo* pGroupInfo,const EB_EnterpriseInfo* pEnterpriseInfo);
	void SetEmployeeInfo(const EB_MemberInfo* pMemberInfo,const EB_GroupInfo* pGroupInfo);
	void SetContactInfo(const EB_ContactInfo* pContactInfo);
#endif

// Dialog Data
	enum { IDD = IDD_DLG_VIEW_CONTACTINFO };

protected:
	VIEW_TYPE m_nViewType;
	bool m_bFirstMoveEnter;
	bool m_bFirstInRect;
	EB_MemberInfo m_pMemberInfo;
	EB_GroupInfo m_pGroupInfo;
	EB_EnterpriseInfo m_pEnterpriseInfo;
	EB_ContactInfo m_pContactInfo;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CLabelEx m_staLabel1;

	void DrawInfo(void);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
