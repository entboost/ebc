// EBVideoDataEventsSink.h file here
#ifndef __EBVideoDataEventsSink_h__
#define __EBVideoDataEventsSink_h__

//#ifdef USES_EBCOM_TEST
#include "../include/EBUnknwn.h"
class CEBVideoDataEventsSink
	: public CEBDispatch
{
public:
	CEBVideoDataEventsSink() {}
	virtual ~CEBVideoDataEventsSink() {};

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
		switch (dispIdMember)
		{
		case 1:	// 视频数据
			{
				if (pDispParams == NULL || pDispParams->cArgs != 4 || pDispParams->rgvarg == NULL)
					return E_INVALIDARG;
				LONGLONG nUserVideoId = pDispParams->rgvarg[3].llVal;
				BYTE* pData = (BYTE*)pDispParams->rgvarg[2].byref;
				unsigned long nSize = pDispParams->rgvarg[1].ulVal;
				ULONG dwParam = pDispParams->rgvarg[0].ulVal;
				return onVideoData(nUserVideoId, pData, nSize, dwParam);
			}break;
		case 2:	// 音频数据
			{
				if (pDispParams == NULL || pDispParams->cArgs != 4 || pDispParams->rgvarg == NULL)
					return E_INVALIDARG;
				LONGLONG nUserVideoId = pDispParams->rgvarg[3].llVal;
				BYTE* pData = (BYTE*)pDispParams->rgvarg[2].byref;
				unsigned long nSize = pDispParams->rgvarg[1].ulVal;
				ULONG dwParam = pDispParams->rgvarg[0].ulVal;
				return onAudioData(nUserVideoId,pData, nSize, dwParam);
			}break;
		default:
			break;
		}
		return S_OK;
	}
	HRESULT _stdcall onVideoData(LONGLONG nUserVideoId, const BYTE* pData, DWORD nSize, DWORD dwParam)
	{
		if (pData == NULL || nSize == NULL) return E_INVALIDARG;
		Fire_onVideoData(nUserVideoId, pData, nSize, dwParam);
		return S_OK;
	}
	HRESULT _stdcall onAudioData(LONGLONG nUserVideoId,const BYTE* pData, DWORD nSize, DWORD dwParam)
	{
		if (pData == NULL || nSize == NULL) return E_INVALIDARG;
		Fire_onAudioData(nUserVideoId,pData, nSize, dwParam);
		return S_OK;
	}
private:
	virtual void Fire_onVideoData(LONGLONG nUserVideoId, const BYTE* pData, DWORD nSize, DWORD dwParam) {}
	virtual void Fire_onAudioData(LONGLONG nUserVideoId, const BYTE* pData, DWORD nSize, DWORD dwParam) {}
};

//#endif

#endif // __EBVideoDataEventsSink_h__
