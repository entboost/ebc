// EBRichMessageEventsSink.h file here
#ifndef __EBRichMessageEventsSink_h__
#define __EBRichMessageEventsSink_h__

const _ATL_FUNC_INFO onContextMenuCopyInfo		= {CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };
const _ATL_FUNC_INFO onContextMenuDeleteInfo	= {CC_STDCALL, VT_EMPTY, 2, { VT_I8,VT_I4 } };
const _ATL_FUNC_INFO onContextMenuSelectInfo	= {CC_STDCALL, VT_EMPTY, 3, { VT_UI4,VT_BSTR,VT_I8 } };
//const _ATL_FUNC_INFO onItemClickInfo			= {CC_STDCALL, VT_EMPTY, 2, { VT_UI4,VT_BSTR } };

class CEBRichMessageEventsSink
 : public IDispEventSimpleImpl<1, CEBRichMessageEventsSink,&__uuidof(_IEBRichMessageEvents)>
{
public:
	HWND m_hFireWnd;
	CEBRichMessageEventsSink(void)
		: m_hFireWnd(NULL)
	{}
	virtual ~CEBRichMessageEventsSink() {}

	virtual HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID /*lcid*/, _ATL_FUNC_INFO& info)
	{
		switch (dispidMember)
		{
		case 1:
			info = onContextMenuCopyInfo;
			return S_OK;
		case 2:
			info = onContextMenuDeleteInfo;
			return S_OK;
		case 11:
		case 12:
		case 13:
		case 14:
			info = onContextMenuSelectInfo;
			return S_OK;
		default:
			break;
		}
		return E_NOTIMPL;
	}

	BEGIN_SINK_MAP(CEBRichMessageEventsSink)
		// ATLºêÓ³ÉäÊÂ¼þº¯Êý
		SINK_ENTRY_EX(1, __uuidof(_IEBRichMessageEvents), 1, onContextMenuCopy)
		SINK_ENTRY_EX(1, __uuidof(_IEBRichMessageEvents), 2, onContextMenuDelete)
		SINK_ENTRY_EX(1, __uuidof(_IEBRichMessageEvents), 11, onContextMenuSelect)
		SINK_ENTRY_EX(1, __uuidof(_IEBRichMessageEvents), 12, onItemLBtnClick)
		SINK_ENTRY_EX(1, __uuidof(_IEBRichMessageEvents), 13, onItemLBtnDblClick)
		SINK_ENTRY_EX(1, __uuidof(_IEBRichMessageEvents), 14, onItemRBtnDblClick)
	END_SINK_MAP()

	HRESULT _stdcall onContextMenuCopy(BSTR sSelectString) {
		Fire_onContextMenuCopy(CEBString(sSelectString));
		return S_OK;}
	HRESULT _stdcall onContextMenuDelete(LONGLONG nMsgId,LONG nIndex) {
		Fire_onContextMenuDelete(nMsgId,nIndex);
		return S_OK;}
	HRESULT _stdcall onContextMenuSelect(ULONG nItemData, BSTR sSelectString,LONGLONG nSelectMsgId) {
		Fire_onContextMenuSelect(nItemData,CEBString(sSelectString),nSelectMsgId);
		return S_OK;}
	HRESULT _stdcall onItemLBtnClick(ULONG nData1, BSTR sParamString, LONGLONG nSelectMsgId) {
		Fire_onItemLBtnClick(nData1, CEBString(sParamString), nSelectMsgId);
		return S_OK;}
	HRESULT _stdcall onItemLBtnDblClick(ULONG nData1, BSTR sParamString, LONGLONG nSelectMsgId) {
		Fire_onItemLBtnDblClick(nData1, CEBString(sParamString), nSelectMsgId);
		return S_OK;}
	HRESULT _stdcall onItemRBtnDblClick(ULONG nData1, BSTR sParamString, LONGLONG nSelectMsgId) {
		Fire_onItemRBtnClick(nData1, CEBString(sParamString), nSelectMsgId);
		return S_OK;}

private:
	virtual void Fire_onContextMenuCopy(const CEBString& sSelectString) {}
	virtual void Fire_onContextMenuDelete(LONGLONG nMsgId,LONG nIndex) {}
	virtual void Fire_onContextMenuSelect(ULONG nItemData, const CEBString& sSelectString,LONGLONG nSelectMsgId) {}
	virtual void Fire_onItemLBtnClick(ULONG nData1, const CEBString& sParamString, LONGLONG nSelectMsgId) {}
	virtual void Fire_onItemLBtnDblClick(ULONG nData1, const CEBString& sParamString, LONGLONG nSelectMsgId) {}
	virtual void Fire_onItemRBtnClick(ULONG nData1, const CEBString& sParamString, LONGLONG nSelectMsgId) {}
};


#endif // __EBRichMessageEventsSink_h__
