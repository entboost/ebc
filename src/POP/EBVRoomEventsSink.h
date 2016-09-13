// EBVRoomEventsSink.h file here
#ifndef __EBVRoomEventsSink_h__
#define __EBVRoomEventsSink_h__

//#ifdef USES_EBCOM_TEST
//_ATL_FUNC_INFO onVideoStreamInfo;
const _ATL_FUNC_INFO onVideoStreamInfo			= {CC_STDCALL, VT_EMPTY, 4, { VT_I8,VT_UI1|VT_BYREF,VT_UI4,VT_UI4 } };

class CEBVRoomEventsSink
 : public IDispEventSimpleImpl<1, CEBVRoomEventsSink,&__uuidof(_IEBVRoomEvents)>
 //: public IDispEventImpl<1, CEBVRoomEventsSink,&__uuidof(_IEBVRoomEvents),&__uuidof(__ebvideoroomLib),1,0>
{
public:
	CEBVRoomEventsSink()
	{
		//onVideoStreamInfo.cc = CC_STDCALL;
		//onVideoStreamInfo.vtReturn = VT_EMPTY;
		//onVideoStreamInfo.nParams = 4;
		//onVideoStreamInfo.pVarTypes[0] = VT_I8;
		//onVideoStreamInfo.pVarTypes[1] = VT_UI1|VT_BYREF;
		//onVideoStreamInfo.pVarTypes[2] = VT_UI4;
		//onVideoStreamInfo.pVarTypes[3] = VT_UI4;
	}
	virtual ~CEBVRoomEventsSink(){};
	//static _ATL_FUNC_INFO onVideoStreamInfo2 = {CC_STDCALL, VT_EMPTY, 4, { VT_I8,VT_UI4,VT_UI1|VT_BYREF,VT_UI4 } };
	virtual HRESULT GetFuncInfoFromId(const IID& /*iid*/, DISPID dispidMember, LCID /*lcid*/, _ATL_FUNC_INFO& info)
	{
		if (dispidMember==1 || dispidMember==2)
		{
			info = onVideoStreamInfo;
			//info.nParams = 4;
			//info.pVarTypes[3] = VT_UI4;
			//info.pVarTypes[2] = VT_UI1|VT_BYREF;
			//info.pVarTypes[1] = VT_UI4;
			//info.pVarTypes[0] = VT_UI4;
			//info.vtReturn = VT_EMPTY;
			return S_OK;
		}
		return E_NOTIMPL;
	}

	BEGIN_SINK_MAP(CEBVRoomEventsSink)
		// ATL宏映射事件函数
		SINK_ENTRY_EX(1, __uuidof(_IEBVRoomEvents), 1, onVideoStream)
		//SINK_ENTRY_INFO(1, __uuidof(_IEBVRoomEvents), 1, onVideoStream, &onVideoStreamInfo)	// ?其实要不要这个无所谓
		SINK_ENTRY_EX(1, __uuidof(_IEBVRoomEvents), 2, onAudioStream)
	END_SINK_MAP()
	HRESULT _stdcall onVideoStream(LONGLONG nUserId, BYTE* pVideoData, ULONG nVideoSize, ULONG nUserData)
	{
		Fire_onVideoStream(nUserId,pVideoData,nVideoSize,nUserData);
		return S_OK;
	}
	HRESULT _stdcall onAudioStream(LONGLONG nUserId, BYTE* pAudioData, ULONG nAudioSize, ULONG nUserData)
	{
		Fire_onAudioStream(nUserId,pAudioData,nAudioSize,nUserData);
		return S_OK;
	}
private:
	virtual void Fire_onVideoStream(LONGLONG nUserId, BYTE* pVideoData, ULONG nVideoSize, ULONG nUserData) {}
	virtual void Fire_onAudioStream(LONGLONG nUserId, BYTE* pAudioData, ULONG nAudioSize, ULONG nUserData) {}
};

//#endif

#endif // __EBVRoomEventsSink_h__
