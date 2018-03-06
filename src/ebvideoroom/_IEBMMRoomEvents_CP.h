#pragma once

template<class T>
class CProxy_IEBMMRoomEvents :
	public IConnectionPointImpl<T, &__uuidof(_IEBMMRoomEvents)>
{
public:
	HRESULT Fire_onVideoCallBack( LONGLONG nUserId,  LONG nCallBackType,  ULONG lParam,  ULONG wParam,  ULONG nUserData)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant avarParams[5];
				avarParams[4] = nUserId;
				avarParams[4].vt = VT_I8;
				avarParams[3] = nCallBackType;
				avarParams[3].vt = VT_I4;
				avarParams[2] = lParam;
				avarParams[2].vt = VT_UI4;
				avarParams[1] = wParam;
				avarParams[1].vt = VT_UI4;
				avarParams[0] = nUserData;
				avarParams[0].vt = VT_UI4;
				CComVariant varResult;

				DISPPARAMS params = { avarParams, NULL, 5, 0 };
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};

