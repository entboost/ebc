#pragma once

//template<class T>
//class CProxy_IEBVRoomEvents :
//	public IConnectionPointImpl<T, &__uuidof(_IEBVRoomEvents)>
//{
//public:
//	HRESULT Fire_onVideoStream( LONGLONG nUserId,  BYTE * pVideoData,  ULONG nVideoSize,  ULONG nUserData)
//	{
//		HRESULT hr = S_OK;
//		T * pThis = static_cast<T *>(this);
//		int cConnections = m_vec.GetSize();
//
//		for (int iConnection = 0; iConnection < cConnections; iConnection++)
//		{
//			pThis->Lock();
//			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
//			pThis->Unlock();
//
//			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);
//
//			if (pConnection)
//			{
//				CComVariant avarParams[4];
//				avarParams[3] = nUserId;
//				avarParams[3].vt = VT_I8;
//				avarParams[2].byref = pVideoData;
//				avarParams[2].vt = VT_UI1|VT_BYREF;
//				avarParams[1] = nVideoSize;
//				avarParams[1].vt = VT_UI4;
//				avarParams[0] = nUserData;
//				avarParams[0].vt = VT_UI4;
//				CComVariant varResult;
//
//				DISPPARAMS params = { avarParams, NULL, 4, 0 };
//				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
//			}
//		}
//		return hr;
//	}
//	HRESULT Fire_onAudioStream( LONGLONG nUserId,  BYTE * pAudioData,  ULONG nAudioSize,  ULONG nUserData)
//	{
//		HRESULT hr = S_OK;
//		T * pThis = static_cast<T *>(this);
//		int cConnections = m_vec.GetSize();
//
//		for (int iConnection = 0; iConnection < cConnections; iConnection++)
//		{
//			pThis->Lock();
//			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
//			pThis->Unlock();
//
//			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);
//
//			if (pConnection)
//			{
//				CComVariant avarParams[4];
//				avarParams[3] = nUserId;
//				avarParams[3].vt = VT_I8;
//				avarParams[2].byref = pAudioData;
//				avarParams[2].vt = VT_UI1|VT_BYREF;
//				avarParams[1] = nAudioSize;
//				avarParams[1].vt = VT_UI4;
//				avarParams[0] = nUserData;
//				avarParams[0].vt = VT_UI4;
//				CComVariant varResult;
//
//				DISPPARAMS params = { avarParams, NULL, 4, 0 };
//				hr = pConnection->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
//			}
//		}
//		return hr;
//	}
//};
//
