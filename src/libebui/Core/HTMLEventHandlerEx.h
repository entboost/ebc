// HTMLEventHandlerEx.h file here
#ifndef __HTMLEventHandlerEx_h__
#define __HTMLEventHandlerEx_h__


class CHTMLEventHandlerEx
	: public IDispatchEx
{
private:
	LONG m_uRefCount; 

public:
	CHTMLEventHandlerEx(void)
		: m_uRefCount(1)
	{
	}     
	virtual ~CHTMLEventHandlerEx(void) {}
protected:
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
		/* [out] */ UINT *pctinfo){
			return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo **ppTInfo){
			return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ REFIID riid,
		/* [size_is][in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ DISPID *rgDispId){
			*rgDispId=1;
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
		/* [out] */ UINT *puArgErr){
			if(dispIdMember==1)
			{
				//MessageBox(0,"Hello World","Hello",0); //place your code here
			}
			return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID classid, void** intf) {
		if (classid == IID_IDispatch)
		{
			*intf = (IDispatch*)this;
		}else if (classid == IID_IDispatchEx)
		{
			*intf = (IDispatchEx*)this;
		}else
			return E_NOINTERFACE;
		AddRef();
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef() {
		InterlockedIncrement(&m_uRefCount);
		return m_uRefCount;
	}

	virtual ULONG STDMETHODCALLTYPE Release() {
		InterlockedDecrement(&m_uRefCount);
		ULONG uRefCount = m_uRefCount;
		if (m_uRefCount == 0)
			delete this;
		return uRefCount;
	}

	virtual HRESULT STDMETHODCALLTYPE GetDispID( 
		/* [in] */ __RPC__in BSTR bstrName,
		/* [in] */ DWORD grfdex,
		/* [out] */ __RPC__out DISPID *pid)
	{
		*pid = 1;
		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE InvokeEx( 
		/* [in] */ DISPID id,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [in] */ DISPPARAMS *pdp,
		/* [out] */ VARIANT *pvarRes,
		/* [out] */ EXCEPINFO *pei,
		/* [unique][in] */ IServiceProvider *pspCaller)
	{
		//CPoint point;
		//::GetCursorPos(&point);
		//AfxGetMainWnd()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	

		//AfxMessageBox(_T("一个网页事件触发."));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DeleteMemberByName( 
		/* [in] */ __RPC__in BSTR bstrName,
		/* [in] */ DWORD grfdex)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DeleteMemberByDispID( 
		/* [in] */ DISPID id)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetMemberProperties( 
		/* [in] */ DISPID id,
		/* [in] */ DWORD grfdexFetch,
		/* [out] */ __RPC__out DWORD *pgrfdex)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetMemberName( 
		/* [in] */ DISPID id,
		/* [out] */ __RPC__deref_out_opt BSTR *pbstrName) 
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetNextDispID( 
		/* [in] */ DWORD grfdex,
		/* [in] */ DISPID id,
		/* [out] */ __RPC__out DISPID *pid)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetNameSpaceParent( 
		/* [out] */ __RPC__deref_out_opt IUnknown **ppunk) 
	{
		return S_OK;
	}

};
#endif // __HTMLEventHandlerEx_h__
