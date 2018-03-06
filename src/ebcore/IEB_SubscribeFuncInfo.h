// IEB_SubscribeFuncInfo.h file here
#ifndef __IEB_SubscribeFuncInfo_h__
#define __IEB_SubscribeFuncInfo_h__
#include "EBUnknwn.h"
#include "ebdisp.h"

class CIEB_SubscribeFuncInfo : public IEB_SubscribeFuncInfo
	, public CEBDispatch
	, public EB_SubscribeFuncInfo
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscribeId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nSubscribeId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SubscribeId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nSubscribeId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Location( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nLocation;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Location( 
		/* [in] */ LONG newVal)
	{
		m_nLocation = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IconResId( 
		/* [retval][out] */ ULONGLONG *pVal)
	{
		*pVal = m_nIconResId;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_IconResId( 
		/* [in] */ ULONGLONG newVal)
	{
		m_nIconResId = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResFile( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sResFile.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResFile( 
		/* [in] */ BSTR newVal)
	{
		m_sResFile = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FuncName( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFunctionName.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FuncName( 
		/* [in] */ BSTR newVal)
	{
		m_sFunctionName = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FuncUrl( 
		/* [retval][out] */ BSTR *pVal)
	{
		*pVal = m_sFunctionUrl.CopyBSTR();
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FuncUrl( 
		/* [in] */ BSTR newVal)
	{
		m_sFunctionUrl = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FuncMode( 
		/* [retval][out] */ SHORT *pVal)
	{
		*pVal = (SHORT)m_nFunctionMode;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FuncMode( 
		/* [in] */ SHORT newVal)
	{
		m_nFunctionMode = (EB_FUNC_MODE)newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WindowWidth( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nWindowWidth;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_WindowWidth( 
		/* [in] */ LONG newVal)
	{
		m_nWindowWidth = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WindowHeight( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nWindowHeight;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_WindowHeight( 
		/* [in] */ LONG newVal)
	{
		m_nWindowHeight = newVal;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DisableContextMenu( 
		/* [retval][out] */ VARIANT_BOOL *pVal)
	{
		*pVal = m_bDisableContextMenu?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DisableContextMenu( 
		/* [in] */ VARIANT_BOOL newVal)
	{
		m_bDisableContextMenu = newVal==VARIANT_TRUE?true:false;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DisableScrollBar( 
		/* [retval][out] */ VARIANT_BOOL *pVal)
	{
		*pVal = m_bDisableScrollBar?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DisableScrollBar( 
		/* [in] */ VARIANT_BOOL newVal)
	{
		m_bDisableScrollBar = newVal==VARIANT_TRUE?true:false;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClosePrompt( 
		/* [retval][out] */ VARIANT_BOOL *pVal)
	{
		*pVal = m_bClosePrompt?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ClosePrompt( 
		/* [in] */ VARIANT_BOOL newVal)
	{
		m_bClosePrompt = newVal==VARIANT_TRUE?true:false;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WinResizable( 
		/* [retval][out] */ VARIANT_BOOL *pVal)
	{
		*pVal = m_bWinResizable?VARIANT_TRUE:VARIANT_FALSE;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_WinResizable( 
		/* [in] */ VARIANT_BOOL newVal)
	{
		m_bWinResizable = newVal==VARIANT_TRUE?true:false;
		return S_OK;
	}
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DisplayIndex( 
		/* [retval][out] */ LONG *pVal)
	{
		*pVal = m_nDisplayIndex;
		return S_OK;
	}
	virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DisplayIndex( 
		/* [in] */ LONG newVal)
	{
		m_nDisplayIndex = newVal;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
#ifdef IID_IEB_SubscribeFuncInfo
		if (InlineIsEqualGUID(riid, IID_IEB_SubscribeFuncInfo))
#else
		if (InlineIsEqualGUID(riid, __uuidof(IEB_SubscribeFuncInfo)))
#endif
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return CEBDispatch::QueryInterface(riid,ppvObject);
	}
private:
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ __RPC__in REFIID riid,
		/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
		/* [range][in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
	{
		if (rgszNames==NULL)
			return E_INVALIDARG;
		CComBSTR bstrName(*rgszNames);
		bstrName.ToUpper();
		if (bstrName==L"0" || bstrName == L"SUBSCRIBEID")
		{
			*rgDispId = EB_DISP_SUBSCRIBEID;
		}
		else if (bstrName==L"1" || bstrName == L"LOCATION")
		{
			*rgDispId = EB_DISP_LOCATION;
		}
		else if (bstrName==L"2" || bstrName == L"ICONRESID")
		{
			*rgDispId = EB_DISP_ICONRESID;
		}
		else if (bstrName==L"3" || bstrName == L"RESFILE")
		{
			*rgDispId = EB_DISP_RESFILE;
		}
		else if (bstrName==L"4" || bstrName == L"FUNCNAME")
		{
			*rgDispId = EB_DISP_FUNCNAME;
		}
		else if (bstrName==L"5" || bstrName == L"FUNCURL")
		{
			*rgDispId = EB_DISP_FUNCURL;
		}
		else if (bstrName==L"6" || bstrName == L"FUNCMODE")
		{
			*rgDispId = EB_DISP_FUNCMODE;
		}
		else if (bstrName==L"7" || bstrName == L"WINDOWWIDTH")
		{
			*rgDispId = EB_DISP_WINDOWWIDTH;
		}
		else if (bstrName==L"8" || bstrName == L"WINDOWHEIGHT")
		{
			*rgDispId = EB_DISP_WINDOWHEIGHT;
		}
		else if (bstrName==L"9" || bstrName == L"DISABLECONTEXTMENU")
		{
			*rgDispId = EB_DISP_DISABLECONTEXTMENU;
		}
		else if (bstrName==L"10" || bstrName == L"DISABLESCROLLBAR")
		{
			*rgDispId = EB_DISP_DISABLESCROLLBAR;
		}
		else if (bstrName==L"11" || bstrName == L"CLOSEPROMPT")
		{
			*rgDispId = EB_DISP_CLOSEPROMPT;
		}
		else if (bstrName==L"12" || bstrName == L"WINRESIZABLE")
		{
			*rgDispId = EB_DISP_WINRESIZABLE;
		}
		else if (bstrName==L"13" || bstrName == L"DISPLAYINDEX")
		{
			*rgDispId = EB_DISP_INDEX;
		}
		return S_OK;
	}
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr)
	{
		if (wFlags&DISPATCH_PROPERTYGET)
		{
			if (pVarResult==NULL)
				return E_INVALIDARG;
			VariantInit(pVarResult);
			switch (dispIdMember)
			{
			case EB_DISP_SUBSCRIBEID:
				pVarResult->vt = VT_UI8;
				return this->get_SubscribeId(&pVarResult->ullVal);
			case EB_DISP_LOCATION:
				pVarResult->vt = VT_I4;
				return this->get_Location(&pVarResult->lVal);
			case EB_DISP_ICONRESID:
				pVarResult->vt = VT_UI8;
				return this->get_IconResId(&pVarResult->ullVal);
			case EB_DISP_RESFILE:
				pVarResult->vt = VT_BSTR;
				return this->get_ResFile(&pVarResult->bstrVal);
			case EB_DISP_FUNCNAME:
				pVarResult->vt = VT_BSTR;
				return this->get_FuncName(&pVarResult->bstrVal);
			case EB_DISP_FUNCURL:
				pVarResult->vt = VT_BSTR;
				return this->get_FuncUrl(&pVarResult->bstrVal);
			case EB_DISP_FUNCMODE:
				pVarResult->vt = VT_I2;
				return this->get_FuncMode(&pVarResult->iVal);
			case EB_DISP_WINDOWWIDTH:
				pVarResult->vt = VT_I4;
				return this->get_WindowWidth(&pVarResult->lVal);
			case EB_DISP_WINDOWHEIGHT:
				pVarResult->vt = VT_I4;
				return this->get_WindowHeight(&pVarResult->lVal);
			case EB_DISP_DISABLECONTEXTMENU:
				pVarResult->vt = VT_BOOL;
				return this->get_DisableContextMenu(&pVarResult->boolVal);
			case EB_DISP_DISABLESCROLLBAR:
				pVarResult->vt = VT_BOOL;
				return this->get_DisableScrollBar(&pVarResult->boolVal);
			case EB_DISP_CLOSEPROMPT:
				pVarResult->vt = VT_BOOL;
				return this->get_ClosePrompt(&pVarResult->boolVal);
			case EB_DISP_WINRESIZABLE:
				pVarResult->vt = VT_BOOL;
				return this->get_WinResizable(&pVarResult->boolVal);
			case EB_DISP_INDEX:
				pVarResult->vt = VT_I4;
				return this->get_DisplayIndex(&pVarResult->lVal);
			default:
				break;
			}
		}
		else if (wFlags&DISPATCH_PROPERTYPUT)
		{
			if (pDispParams == NULL || pDispParams->cArgs != 1 || pDispParams->rgvarg == NULL)
				return E_INVALIDARG;
			switch (dispIdMember)
			{
			case EB_DISP_SUBSCRIBEID:
				return this->put_SubscribeId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_LOCATION:
				return this->put_Location(pDispParams->rgvarg[0].lVal);
			case EB_DISP_ICONRESID:
				return this->put_IconResId(pDispParams->rgvarg[0].ullVal);
			case EB_DISP_RESFILE:
				return this->put_ResFile(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FUNCNAME:
				return this->put_FuncName(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FUNCURL:
				return this->put_FuncUrl(pDispParams->rgvarg[0].bstrVal);
			case EB_DISP_FUNCMODE:
				return this->put_FuncMode(pDispParams->rgvarg[0].iVal);
			case EB_DISP_WINDOWWIDTH:
				return this->put_WindowWidth(pDispParams->rgvarg[0].lVal);
			case EB_DISP_WINDOWHEIGHT:
				return this->put_WindowHeight(pDispParams->rgvarg[0].lVal);
			case EB_DISP_DISABLECONTEXTMENU:
				return this->put_DisableContextMenu(pDispParams->rgvarg[0].boolVal);
			case EB_DISP_DISABLESCROLLBAR:
				return this->put_DisableScrollBar(pDispParams->rgvarg[0].boolVal);
			case EB_DISP_CLOSEPROMPT:
				return this->put_ClosePrompt(pDispParams->rgvarg[0].boolVal);
			case EB_DISP_WINRESIZABLE:
				return this->put_WinResizable(pDispParams->rgvarg[0].boolVal);
			case EB_DISP_INDEX:
				return this->put_DisplayIndex(pDispParams->rgvarg[0].lVal);
			default:
				break;
			}
		}
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return CEBDispatch::AddRef();
	}
	virtual ULONG STDMETHODCALLTYPE Release( void)
	{
		return CEBDispatch::Release();
	}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ __RPC__out UINT *pctinfo)
	{
		return CEBDispatch::GetTypeInfoCount(pctinfo);
	}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)
	{
		return CEBDispatch::GetTypeInfo(iTInfo,lcid,ppTInfo);
	}
public:
	CIEB_SubscribeFuncInfo(void)
	{
	}
	CIEB_SubscribeFuncInfo(const EB_SubscribeFuncInfo& pObj)
		: EB_SubscribeFuncInfo(pObj)
	{
	}
	virtual ~CIEB_SubscribeFuncInfo(void)
	{
	}
};

#endif // __IEB_SubscribeFuncInfo_h__
