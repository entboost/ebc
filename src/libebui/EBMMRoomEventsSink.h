// EBMMRoomEventsSink.h file here
#ifndef __EBMMRoomEventsSink_h__
#define __EBMMRoomEventsSink_h__

#define EBMM_ROOM_CALLBACK_FIRST_VIDEO_FPS	1
#define EBMM_ROOM_CALLBACK_VOICE_VOLUME		2

//#ifdef USES_EBCOM_TEST
const _ATL_FUNC_INFO onVideoCallbackInfo			= {CC_STDCALL, VT_EMPTY, 5, { VT_I8,VT_I4,VT_UI4,VT_UI4,VT_UI4 } };

class CEBMMRoomEventsSink
 : public IDispEventSimpleImpl<1, CEBMMRoomEventsSink,&__uuidof(_IEBMMRoomEvents)>
{
public:
	CEBMMRoomEventsSink()
	{
	}
	virtual ~CEBMMRoomEventsSink(){};
	virtual HRESULT GetFuncInfoFromId(const IID& /*iid*/, DISPID dispidMember, LCID /*lcid*/, _ATL_FUNC_INFO& info)
	{
		if (dispidMember==1)
		{
			info = onVideoCallbackInfo;
			return S_OK;
		}
		return E_NOTIMPL;
	}

	BEGIN_SINK_MAP(CEBMMRoomEventsSink)
		// ATL宏映射事件函数
		SINK_ENTRY_EX(1, __uuidof(_IEBMMRoomEvents), 1, onVideoCallback)
		//SINK_ENTRY_INFO(1, __uuidof(_IEBMMRoomEvents), 1, onVideoCallback, &onVideoCallbackInfo)	// ?其实要不要这个无所谓
	END_SINK_MAP()
	HRESULT _stdcall onVideoCallback(LONGLONG nUserId, LONG nCallbackType, ULONG lParam, ULONG wParam, ULONG nUserData)
	{
		Fire_onVideoCallback(nUserId, nCallbackType,lParam, wParam, nUserData);
		return S_OK;
	}
private:
	virtual void Fire_onVideoCallback(LONGLONG nUserId, LONG nCallbackType, ULONG lParam, ULONG wParam, ULONG nUserData) {}
};


//#endif

#endif // __EBMMRoomEventsSink_h__
