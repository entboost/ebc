#pragma once

#include "Core/HTMLWnd.h"

//
// CBroadcastView

class CBroadcastViewCallback
{
public:
	virtual void OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL) =0;
};

class CBroadcastView : public CHTMLWnd
{
	DECLARE_DYNAMIC(CBroadcastView)

public:
	CBroadcastView(CBroadcastViewCallback* pCallback=NULL);
	virtual ~CBroadcastView();
	virtual void OnBeforeNavigate(VARIANT_BOOL* bCancel,LPCTSTR szURL);

	CString m_strBgColor;
	CString m_strBackground;
	//void SetOverflowHidden(bool bHidden = true);
	//void SetScallBarZoom(bool bZoom = true);

protected:
	CBroadcastViewCallback* m_pCallback;
	virtual void OnBeforeCreateBody();

protected:
	DECLARE_MESSAGE_MAP()
};


