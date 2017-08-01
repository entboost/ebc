#include "StdAfx.h"
#include "MixerWrap.h"
#include <windows.h>
#include <MMSystem.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <math.h>
using namespace std;

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <comdef.h>


//#define SAFE_RELEASE(punk)  \
//	if ((punk) != NULL)  \

#define EXIT_ON_ERROR(hres)  	if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  	if ((punk) != NULL)  { (punk)->Release(); (punk) = NULL; }
HRESULT getMicrophoneBoostVolumeLevel(IMMDevice *pEndptDev, IAudioVolumeLevel** ppVolumeLevel)
{
	HRESULT hr = S_OK;
	DataFlow flow;
	IDeviceTopology *pDeviceTopology = NULL;
	IConnector *pConnFrom = NULL;
	IConnector *pConnTo = NULL;
	IPart *pPartPrev = NULL;
	IPart *pPartNext = NULL;
	*ppVolumeLevel = NULL;
	//wchar_t microphoneBoostName[] = L"麦克风加强";//if your system language is English,the name is "microphone boost"
	if (pEndptDev == NULL)
	{
		EXIT_ON_ERROR(hr = E_POINTER)
	}
	// Get the endpoint device's IDeviceTopology interface.
	hr = pEndptDev->Activate(
		__uuidof(IDeviceTopology), CLSCTX_ALL, NULL,
		(void**)&pDeviceTopology);
	EXIT_ON_ERROR(hr)
		// The device topology for an endpoint device always
		// contains just one connector (connector number 0).
		hr = pDeviceTopology->GetConnector(0, &pConnFrom);
	SAFE_RELEASE(pDeviceTopology)
		EXIT_ON_ERROR(hr)
		// Make sure that this is a capture device.
		hr = pConnFrom->GetDataFlow(&flow);
	EXIT_ON_ERROR(hr)
		if (flow != Out)
		{
			// Error -- this is a rendering device.
			//EXIT_ON_ERROR(hr = AUDCLNT_E_WRONG_ENDPOINT_TYPE)
		}
		// Outer loop: Each iteration traverses the data path
		// through a device topology starting at the input
		// connector and ending at the output connector.
		while (TRUE)
		{
			BOOL bConnected;
			hr = pConnFrom->IsConnected(&bConnected);
			EXIT_ON_ERROR(hr)
				// Does this connector connect to another device?
				if (bConnected == FALSE)
				{
					// This is the end of the data path that
					// stretches from the endpoint device to the
					// system bus or external bus. Verify that
					// the connection type is Software_IO.
					ConnectorType  connType;
					hr = pConnFrom->GetType(&connType);
					EXIT_ON_ERROR(hr)
						if (connType == Software_IO)
						{
							break;  // finished
						}
						EXIT_ON_ERROR(hr = E_FAIL)
				}
				// Get the connector in the next device topology,
				// which lies on the other side of the connection.
				hr = pConnFrom->GetConnectedTo(&pConnTo);
				EXIT_ON_ERROR(hr)
					SAFE_RELEASE(pConnFrom)
					// Get the connector's IPart interface.
					hr = pConnTo->QueryInterface(
					__uuidof(IPart), (void**)&pPartPrev);
				EXIT_ON_ERROR(hr)
					SAFE_RELEASE(pConnTo)
					// Inner loop: Each iteration traverses one link in a
					// device topology and looks for input multiplexers.
					while (TRUE)
					{
						PartType parttype;
						IPartsList *pParts;
						// Follow downstream link to next part.
						hr = pPartPrev->EnumPartsOutgoing(&pParts);
						EXIT_ON_ERROR(hr)
							hr = pParts->GetPart(0, &pPartNext);
						pParts->Release();
						EXIT_ON_ERROR(hr)
							hr = pPartNext->GetPartType(&parttype);
						EXIT_ON_ERROR(hr)

							LPWSTR pName;
						if (SUCCEEDED(pPartNext->GetName(&pName)))
						{
							// Failure of the following call means only that
							// the part is not a boost (micrphone boost).
							if (wcscmp(L"麦克风加强",pName) == 0 || wcscmp(L"microphone boost",pName) == 0)
							{
								//get IAudioVolumeLevel to control volume 
								hr = pPartNext->Activate(CLSCTX_ALL, __uuidof(IAudioVolumeLevel), (void**)ppVolumeLevel);
								goto Exit;
							}
							CoTaskMemFree(pName);
						}
						GUID subType;
						pPartNext->GetSubType(&subType);
						if (parttype == Connector)
						{
							// We've reached the output connector that
							// lies at the end of this device topology.
							hr = pPartNext->QueryInterface(
								__uuidof(IConnector),
								(void**)&pConnFrom);
							EXIT_ON_ERROR(hr)
								SAFE_RELEASE(pPartPrev)
								SAFE_RELEASE(pPartNext)
								break;
						}
						SAFE_RELEASE(pPartPrev)
							pPartPrev = pPartNext;
						pPartNext = NULL;
					}
		}
Exit:
		SAFE_RELEASE(pConnFrom)
			SAFE_RELEASE(pConnTo)
			SAFE_RELEASE(pPartPrev)
			SAFE_RELEASE(pPartNext)
			return hr;
}


void MixerWrap::MicphoneBoost(bool bIsBoost)
{
	int mixerNum ;//总的混音器数量
	HMIXER     hMixer; //混音器设备句柄
	MMRESULT    mmr;//函数调用返回
	MIXERCAPS   MixerCaps; //混音器设备能力信息
	MIXERLINE   MixerLine;//线路的信息

	//获取当前系统总的混音器数量
	mixerNum= mixerGetNumDevs(); 	
	bool ifFind =false;
	for(int i=0;i<mixerNum;i++)
	{
		//打开混音器，第一个参数是记录混音器的handler,第二个参数是要打开的混音器ID
		mmr = mixerOpen(&hMixer, i, 0, 0L, MIXER_OBJECTF_MIXER);
		//UINT uMxid;
		ifFind =false;

		//取混音器id，第一个参数是混音器的handler，第二个参数记录混音器的id
		//mmr = mixerGetID((HMIXEROBJ)hMixer,&uMxid,MIXER_OBJECTF_HMIXER);

		//获取混音器能力特征，如声音控制 和录音控制
		mmr = mixerGetDevCaps((UINT)hMixer, &MixerCaps, sizeof(MixerCaps));	

		for(int j=0;j<MixerCaps.cDestinations;j++) 
			//MixerCaps.cDestinations表示此混音器设备的audio line目标的数量,
			//如一个audio line目标为“音量控制”，另一个audio line目标为“录音控制”。
		{
			memset(&MixerLine,0,sizeof(MIXERLINE));
			MixerLine.cbStruct      = sizeof(MixerLine);
			MixerLine.dwDestination = j;
			// 音量控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
			// 录音控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_WAVEIN
			//MixerLine.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

			//mixerGetLineInfo 第三个参数可以是：MIXER_GETLINEINFOF_COMPONENTTYPE，MIXER_GETLINEINFOF_SOURCE，MIXER_GETLINEINFOF_DESTINATION等
			//取录音控制
			mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine, MIXER_GETLINEINFOF_DESTINATION);
			DWORD dwConnections = MixerLine.cConnections;			
			for ( int count = 0; count < dwConnections; count++ )
			{
				MixerLine.dwSource = count;
				mmr=mixerGetLineInfo((HMIXEROBJ)hMixer,&MixerLine,MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == MixerLine.dwComponentType)
				{
					//如果是录音控制的麦克风选项，则跳出					
					ifFind =true;
					break;
				}
			}
			if (ifFind)
			{
				break;
			}
		}

		if(!ifFind)
			continue;

		//获取麦克风选项
		MIXERLINECONTROLS   MixerLineControls;
		PMIXERCONTROL       paMixerControls;

		paMixerControls = (PMIXERCONTROL)malloc(sizeof(MIXERCONTROL) * MixerLine.cControls);
		MixerLineControls.cbStruct       = sizeof(MixerLineControls);
		MixerLineControls.dwLineID       = MixerLine.dwLineID;
		MixerLineControls.cControls      = MixerLine.cControls;
		MixerLineControls.cbmxctrl       = sizeof(MIXERCONTROL);
		MixerLineControls.pamxctrl       = paMixerControls;
		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls,
			MIXER_GETLINECONTROLSF_ALL);   

	
		int u ;
		ifFind =false;
		for ( u = 0; u < MixerLine.cControls; u++)
		{				
			if (_tcscmp(paMixerControls[u].szName,_T("麦克风加强"))==0 || _tcscmp(paMixerControls[u].szName,_T("microphone boost"))==0)
			{
				ifFind =true;
				break;
			}
		}
		if (!ifFind)
		{
			continue;
		}

		MIXERCONTROL                         MixerControl;
		MixerLineControls.cbStruct    = sizeof(MixerLineControls);
		MixerLineControls.dwControlID = paMixerControls[u].dwControlID;
		MixerLineControls.cbmxctrl    = sizeof(MixerControl);
		MixerLineControls.pamxctrl    = &MixerControl;

		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYID);
		free(paMixerControls);

		MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_BOOLEAN     pMixerControlDetails_Boolean;
		pMixerControlDetails_Boolean = (PMIXERCONTROLDETAILS_BOOLEAN)malloc(1 * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Boolean[0];
		// True to turn on boost, False to turn off
		pMixerControlDetails_Boolean[0].fValue = bIsBoost;  

		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);

		free(pMixerControlDetails_Boolean);

		//关闭混音器
		mixerClose(hMixer);
		if (bIsBoost)
		{
			MicphoneMut(true);
		}

	}
	return ;
}

void MixerWrap::MicphoneBoost_W7(float fValue)
{
	HRESULT hr=NULL; 
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
		__uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	if (hr != S_OK)
	{	
		return ;
	}

	IMMDevice *defaultDevice = NULL;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;
	if (defaultDevice==NULL)
	{
		return;
	}
	IAudioVolumeLevel* pIaudioVolumeLevel;
	getMicrophoneBoostVolumeLevel(defaultDevice,&pIaudioVolumeLevel);
	defaultDevice->Release();
	defaultDevice = NULL; 
	if (pIaudioVolumeLevel==NULL)
	{
		return ;
	}

	pIaudioVolumeLevel->SetLevel(0,fValue,NULL);//0-30
	return ;
}
void MixerWrap::GetMicphoneLevel(float &fValue)
{
	HRESULT hr=NULL; 
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
		__uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);

	if (hr != S_OK)
	{	
		return ;
	}

	IMMDevice *defaultDevice = NULL;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;
	if (defaultDevice==NULL)
	{
		return;
	}
	IAudioVolumeLevel* pIaudioVolumeLevel;
	getMicrophoneBoostVolumeLevel(defaultDevice,&pIaudioVolumeLevel);
	defaultDevice->Release();
	defaultDevice = NULL; 
	if (pIaudioVolumeLevel==NULL)
	{
		return ;
	}

	pIaudioVolumeLevel->GetLevel(0,&fValue);//0-30
	return ;
}
void MixerWrap::GetMicphoneLevelData(float &nMin, float &nMax, float &nStep)
{
	HRESULT hr=NULL; 
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
		__uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	if (hr != S_OK)
	{	
		return ;
	}

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;
	IAudioVolumeLevel* pIaudioVolumeLevel;
	getMicrophoneBoostVolumeLevel(defaultDevice,&pIaudioVolumeLevel);
	defaultDevice->Release();
	defaultDevice = NULL; 
	if (pIaudioVolumeLevel==NULL)
	{
		return ;
	}

	pIaudioVolumeLevel->GetLevelRange(0,&nMin, &nMax, &nStep);//0-30

}
void MixerWrap::MicphoneMut(BOOL bIsMut)
{
	int mixerNum ;//总的混音器数量
	HMIXER     hMixer; //混音器设备句柄
	MMRESULT    mmr;//函数调用返回
	MIXERCAPS   MixerCaps; //混音器设备能力信息
	MIXERLINE   MixerLine;//线路的信息

	//获取当前系统总的混音器数量
	mixerNum= mixerGetNumDevs(); 	
	bool ifFind =false;
	for(int i=0;i<mixerNum;i++)
	{
		//打开混音器，第一个参数是记录混音器的handler,第二个参数是要打开的混音器ID
		mmr = mixerOpen(&hMixer, i, 0, 0L, MIXER_OBJECTF_MIXER);
		//UINT uMxid;
		ifFind =false;

		//取混音器id，第一个参数是混音器的handler，第二个参数记录混音器的id
		//mmr = mixerGetID((HMIXEROBJ)hMixer,&uMxid,MIXER_OBJECTF_HMIXER);

		//获取混音器能力特征，如声音控制 和录音控制
		mmr = mixerGetDevCaps((UINT)hMixer, &MixerCaps, sizeof(MixerCaps));	

		for(int j=0;j<MixerCaps.cDestinations;j++) 
			//MixerCaps.cDestinations表示此混音器设备的audio line目标的数量,
			//如一个audio line目标为“音量控制”，另一个audio line目标为“录音控制”。
		{
			memset(&MixerLine,0,sizeof(MIXERLINE));
			MixerLine.cbStruct      = sizeof(MixerLine);
			MixerLine.dwDestination = j;
			// 音量控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
			// 录音控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_WAVEIN
			//MixerLine.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

			//mixerGetLineInfo 第三个参数可以是：MIXER_GETLINEINFOF_COMPONENTTYPE，MIXER_GETLINEINFOF_SOURCE，MIXER_GETLINEINFOF_DESTINATION等
			//取录音控制
			mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine, MIXER_GETLINEINFOF_DESTINATION);
			DWORD dwConnections = MixerLine.cConnections;			
			for ( int count = 0; count < dwConnections; count++ )
			{
				MixerLine.dwSource = count;
				mmr=mixerGetLineInfo((HMIXEROBJ)hMixer,&MixerLine,MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == MixerLine.dwComponentType)
				{
					//如果是录音控制的麦克风选项，则跳出					
					ifFind =true;
					break;
				}
			}
			if (ifFind)
			{
				break;
			}
		}

		if(!ifFind)
			continue;

		//获取麦克风选项
		MIXERLINECONTROLS   MixerLineControls;
		PMIXERCONTROL       paMixerControls;

		paMixerControls = (PMIXERCONTROL)malloc(sizeof(MIXERCONTROL) * MixerLine.cControls);
		MixerLineControls.cbStruct       = sizeof(MixerLineControls);
		MixerLineControls.dwLineID       = MixerLine.dwLineID;
		MixerLineControls.cControls      = MixerLine.cControls;
		MixerLineControls.cbmxctrl       = sizeof(MIXERCONTROL);
		MixerLineControls.pamxctrl       = paMixerControls;
		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls,
			MIXER_GETLINECONTROLSF_ALL);   

		int u ;
		ifFind =false;
		for ( u = 0; u < MixerLine.cControls; u++)
		{				
			if (_tcscmp(paMixerControls[u].szName,_T("麦克风静音"))==0 || _tcscmp(paMixerControls[u].szName,_T("静音"))==0)
			{
				ifFind =true;
				break;
			}
		}
		if (!ifFind)
		{
			continue;
		}

		MIXERCONTROL                         MixerControl;
		MixerLineControls.cbStruct    = sizeof(MixerLineControls);
		MixerLineControls.dwControlID = paMixerControls[u].dwControlID;
		MixerLineControls.cbmxctrl    = sizeof(MixerControl);
		MixerLineControls.pamxctrl    = &MixerControl;

		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYID);
		free(paMixerControls);

		MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_BOOLEAN     pMixerControlDetails_Boolean;
		pMixerControlDetails_Boolean = (PMIXERCONTROLDETAILS_BOOLEAN)malloc(1 * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Boolean[0];
		// True to turn on boost, False to turn off
		pMixerControlDetails_Boolean[0].fValue = bIsMut;  

		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);

		free(pMixerControlDetails_Boolean);

		//关闭混音器
		mixerClose(hMixer);
		

	}
	return ;
}

void MixerWrap::StereoMix(BOOL bMute)
{
	int mixerNum ;//总的混音器数量
	HMIXER     hMixer; //混音器设备句柄
	MMRESULT    mmr;//函数调用返回
	MIXERCAPS   MixerCaps; //混音器设备能力信息
	MIXERLINE   MixerLine;//线路的信息

	//获取当前系统总的混音器数量
	mixerNum= mixerGetNumDevs(); 	
	bool ifFind =false;
	for(int i=0;i<mixerNum;i++)
	{
		//打开混音器，第一个参数是记录混音器的handler,第二个参数是要打开的混音器ID
		mmr = mixerOpen(&hMixer, i, 0, 0L, MIXER_OBJECTF_MIXER);
		//UINT uMxid;
		ifFind =false;

		//取混音器id，第一个参数是混音器的handler，第二个参数记录混音器的id
		//mmr = mixerGetID((HMIXEROBJ)hMixer,&uMxid,MIXER_OBJECTF_HMIXER);

		//获取混音器能力特征，如声音控制 和录音控制
		mmr = mixerGetDevCaps((UINT)hMixer, &MixerCaps, sizeof(MixerCaps));	

		for(int j=0;j<MixerCaps.cDestinations;j++) 
			//MixerCaps.cDestinations表示此混音器设备的audio line目标的数量,
			//如一个audio line目标为“音量控制”，另一个audio line目标为“录音控制”。
		{
			memset(&MixerLine,0,sizeof(MIXERLINE));
			MixerLine.cbStruct      = sizeof(MixerLine);
			MixerLine.dwDestination = j;
			// 音量控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
			// 录音控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_WAVEIN
			//MixerLine.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

			//mixerGetLineInfo 第三个参数可以是：MIXER_GETLINEINFOF_COMPONENTTYPE，MIXER_GETLINEINFOF_SOURCE，MIXER_GETLINEINFOF_DESTINATION等
			//取录音控制
			mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine, MIXER_GETLINEINFOF_DESTINATION);
			DWORD dwConnections = MixerLine.cConnections;			
			for ( int count = 0; count < dwConnections; count++ )
			{
				MixerLine.dwSource = count;
				mmr=mixerGetLineInfo((HMIXEROBJ)hMixer,&MixerLine,MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);
				//_tprintf(_T("MixerLine.dwComponentType:%d,MixerLine.szName:%s,\r\n"),MixerLine.dwComponentType,MixerLine.szName);			

				if (_tcscmp(MixerLine.szName,_T("立体声混音"))==0/*MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC == MixerLine.dwComponentType &&*/ )
				{
					//如果是立体声混音选项，则跳出	
					//_tprintf(_T("MixerLine.szName:%s,result:%d\r\n"),MixerLine.szName,wcscmp(MixerLine.szName,_T("立体声混音")));			

					ifFind =true;
					break;
				}
			}
			if (ifFind)
			{
				break;
			}
		}

		if(!ifFind)
			continue;

		//_tprintf(_T("ifFind%d\r\n"),ifFind);			

		//获取麦克风选项
		MIXERLINECONTROLS   MixerLineControls;
		PMIXERCONTROL       paMixerControls;

		paMixerControls = (PMIXERCONTROL)malloc(sizeof(MIXERCONTROL) * MixerLine.cControls);
		MixerLineControls.cbStruct       = sizeof(MixerLineControls);
		MixerLineControls.dwLineID       = MixerLine.dwLineID;
		MixerLineControls.cControls      = MixerLine.cControls;
		MixerLineControls.cbmxctrl       = sizeof(MIXERCONTROL);
		MixerLineControls.pamxctrl       = paMixerControls;
		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls,MIXER_GETLINECONTROLSF_ALL);   

		int u ;
		ifFind =false;
		for ( u = 0; u < MixerLine.cControls; u++)
		{		
			//_tprintf(_T("MixerLine.cControls:%d,MpaMixerControls[u].szName:%s,\r\n"),MixerLine.cControls,paMixerControls[u].szName);			
			if (_tcscmp(paMixerControls[u].szName,_T("立体声混音静音"))==0 || 
				_tcscmp(paMixerControls[u].szName,_T("静音"))==0 ||
				_tcscmp(paMixerControls[u].szName,_T("立体声混音"))==0 )
			{
				ifFind =true;
				break;
			}
		}
		if (!ifFind)
		{
			continue;
		}

		MIXERCONTROL                         MixerControl;
		MixerLineControls.cbStruct    = sizeof(MixerLineControls);
		MixerLineControls.dwControlID = paMixerControls[u].dwControlID;
		MixerLineControls.cbmxctrl    = sizeof(MixerControl);
		MixerLineControls.pamxctrl    = &MixerControl;

		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYID);
		free(paMixerControls);

		MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_BOOLEAN     pMixerControlDetails_Boolean;
		pMixerControlDetails_Boolean = (PMIXERCONTROLDETAILS_BOOLEAN)malloc(1 * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Boolean[0];
		// True to turn on boost, False to turn off
		pMixerControlDetails_Boolean[0].fValue = bMute?false:true;  

		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);

		free(pMixerControlDetails_Boolean);

		//关闭混音器
		mixerClose(hMixer);


	}
	return ;

}


void MixerWrap::StereoMixVolume(int nVolume)
{
	int mixerNum ;//总的混音器数量
	HMIXER     hMixer; //混音器设备句柄
	MMRESULT    mmr;//函数调用返回
	MIXERCAPS   MixerCaps; //混音器设备能力信息
	MIXERLINE   MixerLine;//线路的信息

	//获取当前系统总的混音器数量
	mixerNum= mixerGetNumDevs(); 	
	bool ifFind =false;
	for(int i=0;i<mixerNum;i++)
	{
		//打开混音器，第一个参数是记录混音器的handler,第二个参数是要打开的混音器ID
		mmr = mixerOpen(&hMixer, i, 0, 0L, MIXER_OBJECTF_MIXER);
		//UINT uMxid;
		ifFind =false;

		//取混音器id，第一个参数是混音器的handler，第二个参数记录混音器的id
		//mmr = mixerGetID((HMIXEROBJ)hMixer,&uMxid,MIXER_OBJECTF_HMIXER);

		//获取混音器能力特征，如声音控制 和录音控制
		mmr = mixerGetDevCaps((UINT)hMixer, &MixerCaps, sizeof(MixerCaps));	

		for(int j=0;j<MixerCaps.cDestinations;j++) 
			//MixerCaps.cDestinations表示此混音器设备的audio line目标的数量,
			//如一个audio line目标为“音量控制”，另一个audio line目标为“录音控制”。
		{
			memset(&MixerLine,0,sizeof(MIXERLINE));
			MixerLine.cbStruct      = sizeof(MixerLine);
			MixerLine.dwDestination = j;
			// 音量控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
			// 录音控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_WAVEIN
			//MixerLine.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

			//mixerGetLineInfo 第三个参数可以是：MIXER_GETLINEINFOF_COMPONENTTYPE，MIXER_GETLINEINFOF_SOURCE，MIXER_GETLINEINFOF_DESTINATION等
			//取录音控制
			mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine, MIXER_GETLINEINFOF_DESTINATION);
			DWORD dwConnections = MixerLine.cConnections;			
			for ( int count = 0; count < dwConnections; count++ )
			{
				MixerLine.dwSource = count;
				mmr=mixerGetLineInfo((HMIXEROBJ)hMixer,&MixerLine,MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);
				//_tprintf(_T("MixerLine.dwComponentType:%d,MixerLine.szName:%s,\r\n"),MixerLine.dwComponentType,MixerLine.szName);			

				if (_tcscmp(MixerLine.szName,_T("立体声混音"))==0/*MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC == MixerLine.dwComponentType &&*/ )
				{
					//如果是立体声混音选项，则跳出	
					//_tprintf(_T("MixerLine.szName:%s,result:%d\r\n"),MixerLine.szName,wcscmp(MixerLine.szName,_T("立体声混音")));			

					ifFind =true;
					break;
				}
			}
			if (ifFind)
			{
				break;
			}
		}

		if(!ifFind)
			continue;

		//_tprintf(_T("ifFind%d\r\n"),ifFind);			

		//获取麦克风选项
		MIXERLINECONTROLS   MixerLineControls;
		PMIXERCONTROL       paMixerControls;

		paMixerControls = (PMIXERCONTROL)malloc(sizeof(MIXERCONTROL) * MixerLine.cControls);
		MixerLineControls.cbStruct       = sizeof(MixerLineControls);
		MixerLineControls.dwLineID       = MixerLine.dwLineID;
		MixerLineControls.cControls      = MixerLine.cControls;
		MixerLineControls.cbmxctrl       = sizeof(MIXERCONTROL);
		MixerLineControls.pamxctrl       = paMixerControls;
		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls,MIXER_GETLINECONTROLSF_ALL);   

		int u ;
		ifFind =false;
		for ( u = 0; u < MixerLine.cControls; u++)
		{		
			//_tprintf(_T("MixerLine.cControls:%d,MpaMixerControls[u].szName:%s,\r\n"),MixerLine.cControls,paMixerControls[u].szName);			
			
			//wchar_t temp[50];	
			//swprintf(temp,L"Name:%s",paMixerControls[u].szName);
			//_bstr_t strStr = temp;			
		//	::MessageBox(NULL,(LPCWSTR)strStr,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);

			if (_tcsstr(paMixerControls[u].szName,_T("立体声混音"))!=0)// || _tcscmp(paMixerControls[u].szName,_T("立体声混音音量"))==0 )
			{
				ifFind =true;
				break;
			}
		}
		if (!ifFind)
		{
			continue;
		}

		MIXERCONTROL                         MixerControl;
		MixerLineControls.cbStruct    = sizeof(MixerLineControls);
		MixerLineControls.dwControlID = paMixerControls[u].dwControlID;
		MixerLineControls.cbmxctrl    = sizeof(MixerControl);
		MixerLineControls.pamxctrl    = &MixerControl;

		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYID);
		free(paMixerControls);




		/*MIXERCONTROLDETAILS   mxcd; 
		MIXERCONTROLDETAILS_UNSIGNED   vol;
		vol.dwValue   =   nVolume; 
		mxcd.hwndOwner   =   0; 
		mxcd.dwControlID   =   MixerControl.dwControlID; 
		mxcd.cbStruct   =   sizeof(mxcd); 
		mxcd.cbDetails   =   sizeof(vol); 
		mxcd.paDetails   =   &vol; 
		mxcd.cChannels   =   1; 
		mmr =mixerSetControlDetails((HMIXEROBJ)hMixer,   &mxcd,   MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE);
		if(mmr) 
			return   ; 
		return   ; */




		MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_UNSIGNED     pMixerControlDetails_Unsigned;
		pMixerControlDetails_Unsigned = (PMIXERCONTROLDETAILS_UNSIGNED)malloc(1 * sizeof(MIXERCONTROLDETAILS_UNSIGNED));

	

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Unsigned[0];		
		pMixerControlDetails_Unsigned->dwValue= nVolume;  

		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);

		free(pMixerControlDetails_Unsigned);

		//关闭混音器
		mixerClose(hMixer);


	}
	return ;
}

void MixerWrap::OutPutMicphoneVolume(int nVolume)
{
	int mixerNum ;//总的混音器数量
	HMIXER     hMixer; //混音器设备句柄
	MMRESULT    mmr;//函数调用返回
	MIXERCAPS   MixerCaps; //混音器设备能力信息
	MIXERLINE   MixerLine;//线路的信息

	//获取当前系统总的混音器数量
	mixerNum= mixerGetNumDevs(); 	
	bool ifFind =false;
	for(int i=0;i<mixerNum;i++)
	{
		//打开混音器，第一个参数是记录混音器的handler,第二个参数是要打开的混音器ID
		mmr = mixerOpen(&hMixer, i, 0, 0L, MIXER_OBJECTF_MIXER);
		//UINT uMxid;

		//取混音器id，第一个参数是混音器的handler，第二个参数记录混音器的id
		//mmr = mixerGetID((HMIXEROBJ)hMixer,&uMxid,MIXER_OBJECTF_HMIXER);

		//获取混音器能力特征，如声音控制 和录音控制
		mmr = mixerGetDevCaps((UINT)hMixer, &MixerCaps, sizeof(MixerCaps));	

		for(int j=0;j<MixerCaps.cDestinations;j++) 
			//MixerCaps.cDestinations表示此混音器设备的audio line目标的数量,
			//如一个audio line目标为“音量控制”，另一个audio line目标为“录音控制”。
		{
			memset(&MixerLine,0,sizeof(MIXERLINE));
			MixerLine.cbStruct      = sizeof(MixerLine);
			MixerLine.dwDestination = j;
			// 音量控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
			// 录音控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_WAVEIN
			//MixerLine.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

			//mixerGetLineInfo 第三个参数可以是：MIXER_GETLINEINFOF_COMPONENTTYPE，MIXER_GETLINEINFOF_SOURCE，MIXER_GETLINEINFOF_DESTINATION等
			//取录音控制
			mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine, MIXER_GETLINEINFOF_DESTINATION);
			DWORD dwConnections = MixerLine.cConnections;			
			for ( int count = 0; count < dwConnections; count++ )
			{
				MixerLine.dwSource = count;
				mmr=mixerGetLineInfo((HMIXEROBJ)hMixer,&MixerLine,MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == MixerLine.dwComponentType)
				{
					//如果是录音控制的麦克风选项，则跳出					
					ifFind =true;
					break;
				}
			}
			if (ifFind)
			{
				break;
			}
		}

		if(!ifFind)
			continue;

		//获取麦克风选项
		MIXERLINECONTROLS   MixerLineControls;
		PMIXERCONTROL       paMixerControls;

		paMixerControls = (PMIXERCONTROL)malloc(sizeof(MIXERCONTROL) * MixerLine.cControls);
		MixerLineControls.cbStruct       = sizeof(MixerLineControls);
		MixerLineControls.dwLineID       = MixerLine.dwLineID;
		MixerLineControls.cControls      = MixerLine.cControls;
		MixerLineControls.cbmxctrl       = sizeof(MIXERCONTROL);
		MixerLineControls.pamxctrl       = paMixerControls;
		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls,
			MIXER_GETLINECONTROLSF_ALL);   

		int u ;
		ifFind =false;
		for ( u = 0; u < MixerLine.cControls; u++)
		{				
			if (_tcscmp(paMixerControls[u].szName,_T("麦克风音量"))==0 || _tcscmp(paMixerControls[u].szName,_T("静音"))==0)
			{
				ifFind =true;
				break;
			}
		}
		if (!ifFind)
		{
			continue;
		}

		MIXERCONTROL                         MixerControl;
		MixerLineControls.cbStruct    = sizeof(MixerLineControls);
		MixerLineControls.dwControlID = paMixerControls[u].dwControlID;
		MixerLineControls.cbmxctrl    = sizeof(MixerControl);
		MixerLineControls.pamxctrl    = &MixerControl;

		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYID);
		free(paMixerControls);

		MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_UNSIGNED     pMixerControlDetails_Unsigned;
		pMixerControlDetails_Unsigned = (PMIXERCONTROLDETAILS_UNSIGNED)malloc(1 * sizeof(MIXERCONTROLDETAILS_UNSIGNED));

	

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Unsigned[0];		
		pMixerControlDetails_Unsigned->dwValue= nVolume;  
		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);
		free(pMixerControlDetails_Unsigned);

		/*MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_BOOLEAN     pMixerControlDetails_Boolean;
		pMixerControlDetails_Boolean = (PMIXERCONTROLDETAILS_BOOLEAN)malloc(1 * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Boolean[0];
		pMixerControlDetails_Boolean[0].fValue = bIsMut;  

		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);
		free(pMixerControlDetails_Boolean);*/

		//关闭混音器
		mixerClose(hMixer);


	}
	return ;
}



void MixerWrap::InPutMicphoneVolume(int nVolume)
{
	int mixerNum ;//总的混音器数量
	HMIXER     hMixer; //混音器设备句柄
	MMRESULT    mmr;//函数调用返回
	MIXERCAPS   MixerCaps; //混音器设备能力信息
	MIXERLINE   MixerLine;//线路的信息

	//获取当前系统总的混音器数量
	mixerNum= mixerGetNumDevs(); 	
	bool ifFind =false;
	for(int i=1;i<mixerNum;i++)
	{
		//打开混音器，第一个参数是记录混音器的handler,第二个参数是要打开的混音器ID
		mmr = mixerOpen(&hMixer, i, 0, 0L, MIXER_OBJECTF_MIXER);
		//UINT uMxid;

		//取混音器id，第一个参数是混音器的handler，第二个参数记录混音器的id
		//mmr = mixerGetID((HMIXEROBJ)hMixer,&uMxid,MIXER_OBJECTF_HMIXER);

		//获取混音器能力特征，如声音控制 和录音控制
		mmr = mixerGetDevCaps((UINT)hMixer, &MixerCaps, sizeof(MixerCaps));	

		for(int j=0;j<MixerCaps.cDestinations;j++) 
			//MixerCaps.cDestinations表示此混音器设备的audio line目标的数量,
			//如一个audio line目标为“音量控制”，另一个audio line目标为“录音控制”。
		{
			memset(&MixerLine,0,sizeof(MIXERLINE));
			MixerLine.cbStruct      = sizeof(MixerLine);
			MixerLine.dwDestination = j;
			// 音量控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
			// 录音控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_WAVEIN
			//MixerLine.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

			//mixerGetLineInfo 第三个参数可以是：MIXER_GETLINEINFOF_COMPONENTTYPE，MIXER_GETLINEINFOF_SOURCE，MIXER_GETLINEINFOF_DESTINATION等
			//取录音控制
			mmr = mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine, MIXER_GETLINEINFOF_DESTINATION);
			DWORD dwConnections = MixerLine.cConnections;			
			for ( int count = 0; count < dwConnections; count++ )
			{
				MixerLine.dwSource = count;
				mmr=mixerGetLineInfo((HMIXEROBJ)hMixer,&MixerLine,MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);
				if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == MixerLine.dwComponentType)
				{
					//如果是录音控制的麦克风选项，则跳出					
					ifFind =true;
					break;
				}
			}
			if (ifFind)
			{
				break;
			}
		}

		if(!ifFind)
			continue;

		//获取麦克风选项
		MIXERLINECONTROLS   MixerLineControls;
		PMIXERCONTROL       paMixerControls;

		paMixerControls = (PMIXERCONTROL)malloc(sizeof(MIXERCONTROL) * MixerLine.cControls);
		MixerLineControls.cbStruct       = sizeof(MixerLineControls);
		MixerLineControls.dwLineID       = MixerLine.dwLineID;
		MixerLineControls.cControls      = MixerLine.cControls;
		MixerLineControls.cbmxctrl       = sizeof(MIXERCONTROL);
		MixerLineControls.pamxctrl       = paMixerControls;
		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls,
			MIXER_GETLINECONTROLSF_ALL);   

		int u ;
		ifFind =false;
		for ( u = 0; u < MixerLine.cControls; u++)
		{				
			if (_tcscmp(paMixerControls[u].szName,_T("麦克风输入音量"))==0 || _tcscmp(paMixerControls[u].szName,_T("麦克风音量"))==0)
			{
				ifFind =true;
				break;
			}
		}
		if (!ifFind)
		{
			continue;
		}

		MIXERCONTROL                         MixerControl;
		MixerLineControls.cbStruct    = sizeof(MixerLineControls);
		MixerLineControls.dwControlID = paMixerControls[u].dwControlID;
		MixerLineControls.cbmxctrl    = sizeof(MixerControl);
		MixerLineControls.pamxctrl    = &MixerControl;

		mmr = mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYID);
		free(paMixerControls);

		MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_UNSIGNED     pMixerControlDetails_Unsigned;
		pMixerControlDetails_Unsigned = (PMIXERCONTROLDETAILS_UNSIGNED)malloc(1 * sizeof(MIXERCONTROLDETAILS_UNSIGNED));

	

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Unsigned[0];		
		pMixerControlDetails_Unsigned->dwValue= nVolume;  
		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);
		free(pMixerControlDetails_Unsigned);

		/*MIXERCONTROLDETAILS                    MixerControlDetails;
		PMIXERCONTROLDETAILS_BOOLEAN     pMixerControlDetails_Boolean;
		pMixerControlDetails_Boolean = (PMIXERCONTROLDETAILS_BOOLEAN)malloc(1 * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

		memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
		MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
		MixerControlDetails.dwControlID    = MixerControl.dwControlID;		
		MixerControlDetails.cChannels      =1;	
		MixerControlDetails.cMultipleItems =0;	
		MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		MixerControlDetails.paDetails      = &pMixerControlDetails_Boolean[0];
		pMixerControlDetails_Boolean[0].fValue = bIsMut;  

		mmr = mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);
		free(pMixerControlDetails_Boolean);*/

		//关闭混音器
		mixerClose(hMixer);


	}
	return ;
}
bool MixerWrap::GetSpeakerValue_W7(BOOL &bMute,float &fVolume)
{
	bool result = false;
	IMMDeviceEnumerator* pEnumerator;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice=NULL;
	IPropertyStore *pProperties=NULL;
	IAudioEndpointVolume *pVolumeAPI=NULL;

	IAudioAutoGainControl* pAutoGain=NULL;
	UINT deviceCount = 0;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
	if (hr != S_OK)
	{
		printf("CoCreateInstance Failed!/n");
		return false;
	}
	hr = pEnumerator->EnumAudioEndpoints(/*eCapture*/ eRender, DEVICE_STATE_ACTIVE, &pCollection);
	//hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATEMASK_ALL , &pCollection );  
	if (hr != S_OK)
	{
		printf("EnumAudioEndpoints Failed!/n");
		goto releasepEnumerator;
	}

	hr = pCollection->GetCount(&deviceCount);
	if (hr != S_OK)
	{
		printf("GetCount Failed!/n");
		goto releasepCollection;
	}
	

	for (UINT dev=0;dev<deviceCount;dev++)
	{
		pDevice = NULL;
		hr = pCollection->Item(dev,&pDevice);
		if (hr == S_OK)
		{
			hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
     
			if (hr == S_OK)
			{				
				PROPVARIANT varName;
				PropVariantInit(&varName);			
				hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);
							
				if (hr == S_OK )
				{

					//::MessageBox(NULL,varName.pwszVal,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
				
					//if (_tcsstr(varName.pszVal,_T("扬声器"))!=NULL)
					if (wcsstr(varName.pwszVal,L"扬声器")!=NULL)
					{

						//hr=pDevice->Activate(__uuidof(IAudioAutoGainControl),CLSCTX_ALL,NULL,(void **)(&pAutoGain));
						hr=pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));
						if (hr==S_OK)
						{			
							pVolumeAPI->GetMute(&bMute);
							result = true;
							float fVolumeTemp;
							hr = pVolumeAPI->GetMasterVolumeLevelScalar(&fVolumeTemp);
							if (FAILED(hr))
								fVolume = -1;
							else
								fVolume = (MAX_VOL*fVolumeTemp + 0.5);

							//UINT chanelCount = 0;
							//hr =pVolumeAPI->GetChannelCount(&chanelCount);
							//for (int i=0;i<chanelCount;i++)
							//{
							//	float fVolumeTemp;
							//	pVolumeAPI->GetMasterVolumeLevelScalar(&fVolumeTemp);
							//	if(fabsf(fVolumeTemp -0) > 0.00001)
							//		fVolume = MAX_VOL * fVolumeTemp + 0.5;
							//}					
							SAFE_RELEASE(pVolumeAPI);
						}     
					}
				}
				SAFE_RELEASE(pProperties);
			}
			SAFE_RELEASE(pDevice);
		}
	}
releasepCollection:
	SAFE_RELEASE(pCollection);
releasepEnumerator:
	SAFE_RELEASE(pEnumerator);
	return result;
}
bool MixerWrap::GetSpeakerValue_W72(BOOL &bMute,float &fVolume)
{
	bool result = false;
	IMMDeviceEnumerator* pEnumerator;
	IMMDevice *pDevice=NULL;
	IAudioEndpointVolume *pVolumeAPI=NULL;

	//IAudioAutoGainControl* pAutoGain=NULL;
	//GUID m_guidMyContext;
 //   CoCreateGuid(&m_guidMyContext);
	UINT deviceCount = 0;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
	if (hr != S_OK)
	{
		printf("CoCreateInstance Failed!/n");
		return false;
	}
    // Get default audio-rendering device.
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if (hr != S_OK)
	{
		goto releasepEnumerator;
	}

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
                           CLSCTX_ALL, NULL, (void**)&pVolumeAPI);
	if (hr != S_OK)
	{
		goto releasepEnumerator;
	}

	result = true;
	pVolumeAPI->GetMute(&bMute);
	float fVolumeTemp;
	pVolumeAPI->GetMasterVolumeLevelScalar(&fVolumeTemp);
	fVolume = MAX_VOL * fVolumeTemp + 0.5;
	SAFE_RELEASE(pVolumeAPI);

releasepEnumerator:
	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pDevice);
	return result;
}

bool MixerWrap::SetSpeakerValue_W7(BOOL bMute, float fVolume) //设置喇叭音量
{
	bool result = false;
	IMMDeviceEnumerator* pEnumerator;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice=NULL;
	IPropertyStore *pProperties=NULL;
	IAudioEndpointVolume *pVolumeAPI=NULL;

	IAudioAutoGainControl* pAutoGain=NULL;
	GUID m_guidMyContext;
	UINT deviceCount = 0;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
	if (hr != S_OK)
	{
		printf("CoCreateInstance Failed!/n");
		return false;
	}
	hr = pEnumerator->EnumAudioEndpoints(/*eCapture*/ eRender, DEVICE_STATE_ACTIVE, &pCollection);
	//hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATEMASK_ALL , &pCollection );  
	if (hr != S_OK)
	{
		printf("EnumAudioEndpoints Failed!/n");
		goto releasepEnumerator;
	}

	hr = pCollection->GetCount(&deviceCount);
	if (hr != S_OK)
	{
		printf("GetCount Failed!/n");
		goto releasepCollection;
	}


	for (UINT dev=0;dev<deviceCount;dev++)
	{
		pDevice = NULL;
		hr = pCollection->Item(dev,&pDevice);
		if (hr == S_OK)
		{
			hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);

			if (hr == S_OK)
			{				
				PROPVARIANT varName;
				PropVariantInit(&varName);			
				hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);

				if (hr == S_OK )
				{

					//::MessageBox(NULL,varName.pwszVal,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);

					//if (_tcsstr(varName.pszVal,_T("扬声器"))!=NULL)
					if (wcsstr(varName.pwszVal,L"扬声器")!=NULL)
					{

						//hr=pDevice->Activate(__uuidof(IAudioAutoGainControl),CLSCTX_ALL,NULL,(void **)(&pAutoGain));
						hr=pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));
						if (hr==S_OK)
						{			
							result = true;
							pVolumeAPI->SetMute(bMute,&m_guidMyContext);
							if (fVolume>=0.0)
								pVolumeAPI->SetMasterVolumeLevelScalar((float)fVolume / MAX_VOL, &m_guidMyContext);
							//UINT chanelCount = 0;
							//hr =pVolumeAPI->GetChannelCount(&chanelCount);
							//for (int i=0;i<chanelCount;i++)
							//{
							//	if((int)fVolume < 0)
							//		fVolume = 0;
							//	if((int)fVolume > MAX_VOL)
							//		fVolume = MAX_VOL;
							//	pVolumeAPI->SetMasterVolumeLevelScalar((float)fVolume / MAX_VOL, &m_guidMyContext);
							//}					
							SAFE_RELEASE(pVolumeAPI);
						}     
					}
				}
				SAFE_RELEASE(pProperties);
			}
			SAFE_RELEASE(pDevice);
		}
	}
releasepCollection:
	SAFE_RELEASE(pCollection);
releasepEnumerator:
	SAFE_RELEASE(pEnumerator);
	return result;
}
bool MixerWrap::SetSpeakerValue_W72(BOOL bMute, float fVolume)
{
	bool result = false;
	IMMDeviceEnumerator* pEnumerator;
	IMMDevice *pDevice=NULL;
	IAudioEndpointVolume *pVolumeAPI=NULL;

	//IAudioAutoGainControl* pAutoGain=NULL;
	GUID m_guidMyContext;
    CoCreateGuid(&m_guidMyContext);
	UINT deviceCount = 0;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
	if (hr != S_OK)
	{
		printf("CoCreateInstance Failed!/n");
		return false;
	}
    // Get default audio-rendering device.
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if (hr != S_OK)
	{
		goto releasepEnumerator;
	}

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
                           CLSCTX_ALL, NULL, (void**)&pVolumeAPI);
	if (hr != S_OK)
	{
		goto releasepEnumerator;
	}

	result = true;
	pVolumeAPI->SetMute(bMute,&m_guidMyContext);
	if (fVolume>=0.0)
		pVolumeAPI->SetMasterVolumeLevelScalar((float)fVolume / MAX_VOL, &m_guidMyContext);
	SAFE_RELEASE(pVolumeAPI);

releasepEnumerator:
	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pDevice);
	return result;
}

//bool MixerWrap::MutSpeaker_W7(bool bMute,float fVolume)
//{
//
//	//::MessageBox(NULL,_T("MutSpeaker_W7"),_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
//
//	IMMDeviceEnumerator* pEnumerator;
//	IMMDeviceCollection *pCollection = NULL;
//	IMMDevice *pDevice=NULL;
//	IPropertyStore *pProperties=NULL;
//	IAudioEndpointVolume *pVolumeAPI=NULL;
//
//	IAudioAutoGainControl* pAutoGain=NULL;
//	UINT deviceCount = 0;
//	CoInitializeEx(NULL, COINIT_MULTITHREADED);
//	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
//	if (hr != S_OK)
//	{
//		printf("CoCreateInstance Failed!/n");
//		return 0;
//	}
//	hr = pEnumerator->EnumAudioEndpoints(/*eCapture*/ eRender, DEVICE_STATE_ACTIVE, &pCollection);
//	//hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATEMASK_ALL , &pCollection );  
//	if (hr != S_OK)
//	{
//		printf("EnumAudioEndpoints Failed!/n");
//		goto releasepEnumerator;
//	}
//
//	hr = pCollection->GetCount(&deviceCount);
//	if (hr != S_OK)
//	{
//		printf("GetCount Failed!/n");
//		goto releasepCollection;
//	}
//	
//
//	for (UINT dev=0;dev<deviceCount;dev++)
//	{
//		pDevice = NULL;
//		hr = pCollection->Item(dev,&pDevice);
//		if (hr == S_OK)
//		{
//			hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
//     
//			if (hr == S_OK)
//			{				
//				PROPVARIANT varName;
//				PropVariantInit(&varName);			
//				hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);
//
//			/*	char temp[20];	
//				sprintf_s(temp,"%d\0",hr);
//				_bstr_t strStr = temp;
//				LPCSTR lpctStrName = strStr;
//				::MessageBox(NULL,(LPCWSTR)lpctStrName,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);*/
//							
//				if (hr == S_OK )
//				{
//
//					//::MessageBox(NULL,varName.pwszVal,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
//				
//					if (wcsstr(varName.pwszVal,_T("扬声器"))!=NULL)
//					{
//
//						//hr=pDevice->Activate(__uuidof(IAudioAutoGainControl),CLSCTX_ALL,NULL,(void **)(&pAutoGain));
//						hr=pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));
//						if (hr==S_OK)
//						{			
//							
//							UINT chanelCount = 0;
//							hr =pVolumeAPI->GetChannelCount(&chanelCount);
//							for (int i=0;i<chanelCount;i++)
//							{
//								float fLevel =0;
//								float minLevel =0;
//								float maxLevel =0;
//								float stepLevel =0;
//								pVolumeAPI->GetVolumeRange(&minLevel,&maxLevel,&stepLevel);
//
//								float fVolumeTemp;
//								pVolumeAPI->GetMasterVolumeLevelScalar(&fVolumeTemp);
//								if(fVolume != NULL)
//									fVolume = MAX_VOL * fVolumeTemp + 0.5;
//
//								
//								pVolumeAPI->SetChannelVolumeLevelScalar(i,fVolume,NULL);
//								//int test=55;
//							}
//							hr = pVolumeAPI->SetMute(bMute,NULL);						
//							SAFE_RELEASE(pVolumeAPI);
//						}     
//					}
//				}
//				SAFE_RELEASE(pProperties);
//			}
//			SAFE_RELEASE(pDevice);
//		}
//	}
//releasepCollection:
//	SAFE_RELEASE(pCollection);
//releasepEnumerator:
//	SAFE_RELEASE(pEnumerator);
//	return 0;
//
//}
bool MixerWrap::GetMicphone_W7(BOOL &bMute,float &pdwVolume)
{
	bool reslt = false;
	IMMDeviceEnumerator* pEnumerator;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice=NULL;
	IPropertyStore *pProperties=NULL;
	IAudioEndpointVolume *pVolumeAPI=NULL;
	UINT deviceCount = 0;
	//GUID m_guidMyContext;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
	if (hr != S_OK)
	{
		//printf("CoCreateInstance Failed!/n");
		return false;
	}
	hr = pEnumerator->EnumAudioEndpoints(eCapture /*eRender*/, DEVICE_STATE_ACTIVE, &pCollection);
	//hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATEMASK_ALL , &pCollection );  
	if (hr != S_OK)
	{
	//	printf("EnumAudioEndpoints Failed!/n");
		goto releasepEnumerator;
	}
	//::MessageBox(NULL,_T("MicphoneMut_W7"),_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
	hr = pCollection->GetCount(&deviceCount);
	if (hr != S_OK)
	{
		//printf("GetCount Failed!/n");
		goto releasepCollection;
	}
	for (UINT dev=0;dev<deviceCount;dev++)
	{
		pDevice = NULL;
		hr = pCollection->Item(dev,&pDevice);
		if (hr == S_OK)
		{
			hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
			if (hr == S_OK)
			{			
				PROPVARIANT varName;
				PropVariantInit(&varName);
				//hr = pProperties->GetValue(PKEY_Device_BusTypeGuid, &varName);
				hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);
				
				if (hr == S_OK )
				{
					//if (_tcsstr(varName.pszVal,_T("麦克风"))!=NULL)
					if (wcsstr(varName.pwszVal,L"麦克风")!=NULL)
					{
						hr=pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));
						if (hr==S_OK)
						{				
							reslt = true;
							pVolumeAPI->GetMute(&bMute);
							float fVolume;
							pVolumeAPI->GetMasterVolumeLevelScalar(&fVolume);
							pdwVolume = MAX_VOL * fVolume + 0.5;
							SAFE_RELEASE(pVolumeAPI);
						}     
					}
				}
				SAFE_RELEASE(pProperties);
			}
			SAFE_RELEASE(pDevice);
		}
		if (reslt) break;
	}

releasepCollection:
	SAFE_RELEASE(pCollection);
releasepEnumerator:
	SAFE_RELEASE(pEnumerator);
	return reslt;
}
bool MixerWrap::StereoMix_W7(BOOL bMute, float fVolume)
{
	bool result = false;
	IMMDeviceEnumerator* pEnumerator;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice=NULL;
	IPropertyStore *pProperties=NULL;
	IAudioEndpointVolume *pVolumeAPI=NULL;
	UINT deviceCount = 0;
	GUID m_guidMyContext;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
	if (hr != S_OK)
	{
		//printf("CoCreateInstance Failed!/n");
		return false;
	}
	hr = pEnumerator->EnumAudioEndpoints(eCapture /*eRender*/, DEVICE_STATE_ACTIVE, &pCollection);
	//hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATEMASK_ALL , &pCollection );  
	if (hr != S_OK)
	{
	//	printf("EnumAudioEndpoints Failed!/n");
		goto releasepEnumerator;
	}
	//::MessageBox(NULL,_T("MicphoneMut_W7"),_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
	hr = pCollection->GetCount(&deviceCount);
	if (hr != S_OK)
	{
		//printf("GetCount Failed!/n");
		goto releasepCollection;
	}
	for (UINT dev=0;dev<deviceCount;dev++)
	{
		pDevice = NULL;
		hr = pCollection->Item(dev,&pDevice);
		if (hr == S_OK)
		{
			hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
			if (hr == S_OK)
			{			
				PROPVARIANT varName;
				PropVariantInit(&varName);
				//hr = pProperties->GetValue(PKEY_Device_BusTypeGuid, &varName);
				hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);
				
				if (hr == S_OK )
				{
					//if (wcsstr(varName.pwszVal,_T("Line In"))!=NULL)
  				    //if (wcsstr(varName.pwszVal,_T("线路输入"))!=NULL)
					//::MessageBox(NULL,varName.pwszVal,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
					//if (_tcsstr(varName.pszVal,_T("立体声混音"))!=NULL)
					if (wcsstr(varName.pwszVal,L"立体声混音")!=NULL)
					{
						hr=pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));
						if (hr==S_OK)
						{				
							result = true;
							hr = pVolumeAPI->SetMute(bMute,NULL);
							if (fVolume>=0.0)
								pVolumeAPI->SetMasterVolumeLevelScalar((float)fVolume / MAX_VOL,&m_guidMyContext);
							SAFE_RELEASE(pVolumeAPI);
						}     
					}
				}
				SAFE_RELEASE(pProperties);
			}
			SAFE_RELEASE(pDevice);
		}
		if (result) break;
	}

releasepCollection:
	SAFE_RELEASE(pCollection);
releasepEnumerator:
	SAFE_RELEASE(pEnumerator);
	return result;
}
bool MixerWrap::MicphoneMut_W7(BOOL bMute,float fVolume)
{
	bool result = false;
	IMMDeviceEnumerator* pEnumerator;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice=NULL;
	IPropertyStore *pProperties=NULL;
	IAudioEndpointVolume *pVolumeAPI=NULL;
	UINT deviceCount = 0;
	GUID m_guidMyContext;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr=CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
	if (hr != S_OK)
	{
		//printf("CoCreateInstance Failed!/n");
		return false;
	}
	hr = pEnumerator->EnumAudioEndpoints(eCapture /*eRender*/, DEVICE_STATE_ACTIVE, &pCollection);
	//hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATEMASK_ALL , &pCollection );  
	if (hr != S_OK)
	{
	//	printf("EnumAudioEndpoints Failed!/n");
		goto releasepEnumerator;
	}
	//::MessageBox(NULL,_T("MicphoneMut_W7"),_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
	hr = pCollection->GetCount(&deviceCount);
	if (hr != S_OK)
	{
		//printf("GetCount Failed!/n");
		goto releasepCollection;
	}
	for (UINT dev=0;dev<deviceCount;dev++)
	{
		pDevice = NULL;
		hr = pCollection->Item(dev,&pDevice);
		if (hr == S_OK)
		{
			hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
			if (hr == S_OK)
			{			
				PROPVARIANT varName;
				PropVariantInit(&varName);
				//hr = pProperties->GetValue(PKEY_Device_BusTypeGuid, &varName);
				hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);
				
				if (hr == S_OK )
				{
					//if (wcsstr(varName.pwszVal,_T("Line In"))!=NULL)
  				    //if (wcsstr(varName.pwszVal,_T("线路输入"))!=NULL)
					//::MessageBox(NULL,varName.pwszVal,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);
					//if (_tcsstr(varName.pszVal,_T("麦克风"))!=NULL)
					if (wcsstr(varName.pwszVal,L"麦克风")!=NULL)
					{
						hr=pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));
						if (hr==S_OK)
						{			
							result = true;
							hr = pVolumeAPI->SetMute(bMute,NULL);
						//	hr = pVolumeAPI->SetChannelVolumeLevelScalar(0,fVolume,NULL);
							if (fVolume>=0.0)
								pVolumeAPI->SetMasterVolumeLevelScalar((float)fVolume / MAX_VOL,&m_guidMyContext);
							SAFE_RELEASE(pVolumeAPI);
						}     
					}
				}
				SAFE_RELEASE(pProperties);
			}
			SAFE_RELEASE(pDevice);
		}
		if (result) break;
	}

releasepCollection:
	SAFE_RELEASE(pCollection);
releasepEnumerator:
	SAFE_RELEASE(pEnumerator);
	return result;
}


int   MixerWrap::SelectRecordIn(DWORD  dwSrcType)
{
	try{ 
		HMIXER     m_hmx; //混音器设备句柄
		MMRESULT      mmr = mixerOpen(&m_hmx, 1, 0, 0L, MIXER_OBJECTF_MIXER);

		MIXERLINE   mxl; 
		mxl.cbStruct             =   sizeof(mxl); 
		mxl.dwComponentType   =   MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 
		mixerGetLineInfo((HMIXEROBJ)m_hmx,   &mxl,MIXER_GETLINEINFOF_COMPONENTTYPE); 

		LPMIXERCONTROL   pmxctrl   ; 
		DWORD   cbmxctrls   =   sizeof(*pmxctrl)   *   (UINT)mxl.cControls; 
		pmxctrl   =   (LPMIXERCONTROL)LocalAlloc(LPTR,   cbmxctrls); 
		MIXERLINECONTROLS   mxlctrl={sizeof(mxlctrl),mxl.dwLineID,0,mxl.cControls,sizeof(MIXERCONTROL),pmxctrl}; 
		mixerGetLineControls((HMIXEROBJ)m_hmx,   &mxlctrl,MIXER_GETLINECONTROLSF_ALL); 
		
		DWORD   i; 
		for(i=0;   i   <   mxl.cControls;   i++) 
			if   (MIXERCONTROL_CT_CLASS_LIST   ==   (pmxctrl[i].dwControlType&MIXERCONTROL_CT_CLASS_MASK)) 
				break; 

		//wchar_t temp[50];	
		//swprintf(temp,L"mxl.cControls:%d ",mxl.cControls);
		//_bstr_t strStr = temp;			
		//::MessageBox(NULL,(LPCWSTR)strStr,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);

		if   (i   <   mxl.cControls) 
		{ 
			BOOL   bOneItemOnly   =   FALSE; 
			switch   (pmxctrl[i].dwControlType) 
			{ 
			case   MIXERCONTROL_CONTROLTYPE_MUX: 
			case   MIXERCONTROL_CONTROLTYPE_SINGLESELECT: 
				bOneItemOnly   =   TRUE; 
			} 
			DWORD   cChannels   =   mxl.cChannels,   cMultipleItems   =   0; 
			if   (MIXERCONTROL_CONTROLF_UNIFORM   &   pmxctrl[i].fdwControl) 
				cChannels   =   1; 
			if   (MIXERCONTROL_CONTROLF_MULTIPLE   &   pmxctrl[i].fdwControl) 
				cMultipleItems   =   pmxctrl[i].cMultipleItems; 

			//   Get   the   text   description   of   each   item 
			LPMIXERCONTROLDETAILS_LISTTEXT   plisttext   =(LPMIXERCONTROLDETAILS_LISTTEXT) 
				malloc(cChannels   *   cMultipleItems   *   sizeof(MIXERCONTROLDETAILS_LISTTEXT)); 
			MIXERCONTROLDETAILS   mxcd   =   {sizeof(mxcd),   pmxctrl[i].dwControlID,cChannels,(HWND)cMultipleItems,   sizeof(MIXERCONTROLDETAILS_LISTTEXT),(LPVOID)   plisttext}; 
			mixerGetControlDetails((HMIXEROBJ)m_hmx,   &mxcd,MIXER_GETCONTROLDETAILSF_LISTTEXT); 
			LPMIXERCONTROLDETAILS_BOOLEAN   plistbool   =(LPMIXERCONTROLDETAILS_BOOLEAN) 
				malloc(cChannels   *   cMultipleItems   *   sizeof(MIXERCONTROLDETAILS_BOOLEAN)); 
			mxcd.cbDetails   =   sizeof(MIXERCONTROLDETAILS_BOOLEAN); 
			mxcd.paDetails   =   plistbool; 
			mixerGetControlDetails((HMIXEROBJ)m_hmx,   &mxcd,MIXER_GETCONTROLDETAILSF_VALUE); 

			//wchar_t temp[50];	
			//swprintf(temp,L"cMultipleItems:%d ",cMultipleItems);
			//_bstr_t strStr = temp;			
			//::MessageBox(NULL,(LPCWSTR)strStr,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);


			for   (DWORD   j=0;   j <cMultipleItems;   j   =   j   +   cChannels) 
			{ 
				//wchar_t temp[50];	
				//swprintf(temp,L"Name:%s,P1:%d P2:%d,Type:%d",plisttext[j].szName,plisttext[j].dwParam1,plisttext[j].dwParam2,dwSrcType);
				//_bstr_t strStr = temp;			
				//::MessageBox(NULL,(LPCWSTR)strStr,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);

				if   (plisttext[j].dwParam2==dwSrcType||plisttext[j].dwParam1==dwSrcType) 
				{ 
					plistbool[j].fValue   =   plistbool[j+   cChannels   -   1].fValue   =   1; 
				} 
				else   if   (bOneItemOnly) 
					plistbool[j].fValue   =   plistbool[j+   cChannels   -   1].fValue   =   0; 
			} 
			mixerSetControlDetails((HMIXEROBJ)m_hmx,   &mxcd,MIXER_GETCONTROLDETAILSF_VALUE); 
			// free(pmxctrl); 
			free(plisttext); 
			free(plistbool); 
		} 
		else 
			free(pmxctrl); 
		return   TRUE; 
	} 
	catch(...) 
	{ 
		return   FALSE; 
	} 
}


int   MixerWrap::SelectRecordInByName(const TCHAR*  szSrcName)
{
	try{ 
		HMIXER     m_hmx; //混音器设备句柄
		MMRESULT      mmr = mixerOpen(&m_hmx, 1, 0, 0L, MIXER_OBJECTF_MIXER);

		MIXERLINE   mxl; 
		mxl.cbStruct             =   sizeof(mxl); 
		mxl.dwComponentType   =   MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 
		mixerGetLineInfo((HMIXEROBJ)m_hmx,   &mxl,MIXER_GETLINEINFOF_COMPONENTTYPE); 

		LPMIXERCONTROL   pmxctrl   ; 
		DWORD   cbmxctrls   =   sizeof(*pmxctrl)   *   (UINT)mxl.cControls; 
		pmxctrl   =   (LPMIXERCONTROL)LocalAlloc(LPTR,   cbmxctrls); 
		MIXERLINECONTROLS   mxlctrl={sizeof(mxlctrl),mxl.dwLineID,0,mxl.cControls,sizeof(MIXERCONTROL),pmxctrl}; 
		mixerGetLineControls((HMIXEROBJ)m_hmx,   &mxlctrl,MIXER_GETLINECONTROLSF_ALL); 

		DWORD   i; 
		for(i=0;   i   <   mxl.cControls;   i++) 
			if   (MIXERCONTROL_CT_CLASS_LIST   ==   (pmxctrl[i].dwControlType&MIXERCONTROL_CT_CLASS_MASK)) 
				break; 

		//wchar_t temp[50];	
		//swprintf(temp,L"mxl.cControls:%d ",mxl.cControls);
		//_bstr_t strStr = temp;			
	//	::MessageBox(NULL,(LPCWSTR)strStr,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);

		if   (i   <   mxl.cControls) 
		{ 
			BOOL   bOneItemOnly   =   FALSE; 
			switch   (pmxctrl[i].dwControlType) 
			{ 
			case   MIXERCONTROL_CONTROLTYPE_MUX: 
			case   MIXERCONTROL_CONTROLTYPE_SINGLESELECT: 
				bOneItemOnly   =   TRUE; 
			} 
			DWORD   cChannels   =   mxl.cChannels,   cMultipleItems   =   0; 
			if   (MIXERCONTROL_CONTROLF_UNIFORM   &   pmxctrl[i].fdwControl) 
				cChannels   =   1; 
			if   (MIXERCONTROL_CONTROLF_MULTIPLE   &   pmxctrl[i].fdwControl) 
				cMultipleItems   =   pmxctrl[i].cMultipleItems; 

			//   Get   the   text   description   of   each   item 
			LPMIXERCONTROLDETAILS_LISTTEXT   plisttext   =(LPMIXERCONTROLDETAILS_LISTTEXT) 
				malloc(cChannels   *   cMultipleItems   *   sizeof(MIXERCONTROLDETAILS_LISTTEXT)); 
			MIXERCONTROLDETAILS   mxcd   =   {sizeof(mxcd),   pmxctrl[i].dwControlID,cChannels,(HWND)cMultipleItems,   sizeof(MIXERCONTROLDETAILS_LISTTEXT),(LPVOID)   plisttext}; 
			mixerGetControlDetails((HMIXEROBJ)m_hmx,   &mxcd,MIXER_GETCONTROLDETAILSF_LISTTEXT); 
			LPMIXERCONTROLDETAILS_BOOLEAN   plistbool   =(LPMIXERCONTROLDETAILS_BOOLEAN) 
				malloc(cChannels   *   cMultipleItems   *   sizeof(MIXERCONTROLDETAILS_BOOLEAN)); 
			mxcd.cbDetails   =   sizeof(MIXERCONTROLDETAILS_BOOLEAN); 
			mxcd.paDetails   =   plistbool; 
			mixerGetControlDetails((HMIXEROBJ)m_hmx,   &mxcd,MIXER_GETCONTROLDETAILSF_VALUE); 

			//wchar_t temp[50];	
			//swprintf(temp,L"cMultipleItems:%d ",cMultipleItems);
			//_bstr_t strStr = temp;			
		//	::MessageBox(NULL,(LPCWSTR)strStr,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);


			for   (DWORD   j=0;   j <cMultipleItems;   j   =   j   +   cChannels) 
			{ 
				//wchar_t temp[50];	
				//swprintf(temp,L"Name:%s,P1:%d P2:%d,Type:%s",plisttext[j].szName,plisttext[j].dwParam1,plisttext[j].dwParam2,szSrcName);
				//_bstr_t strStr = temp;			
			//	::MessageBox(NULL,(LPCWSTR)strStr,_T("提示信息"),MB_OK   |  MB_ICONINFORMATION);

				/*if   (plisttext[j].dwParam2==dwSrcType||plisttext[j].dwParam1==dwSrcType) 
				{ 
					plistbool[j].fValue   =   plistbool[j+   cChannels   -   1].fValue   =   1; 
				} 
				else   if   (bOneItemOnly) 
					plistbool[j].fValue   =   plistbool[j+   cChannels   -   1].fValue   =   0; */

				if   (_tcsstr(plisttext[j].szName,szSrcName)!=NULL ) 
				//if   (wcsstr(plisttext[j].szName,szSrcName)!=NULL ) 
				{ 
					plistbool[j].fValue   =   plistbool[j+   cChannels   -   1].fValue   =   1; 
				} 
				else   if   (bOneItemOnly) 
					plistbool[j].fValue   =   plistbool[j+   cChannels   -   1].fValue   =   0; 

				
			} 
			mixerSetControlDetails((HMIXEROBJ)m_hmx,   &mxcd,MIXER_GETCONTROLDETAILSF_VALUE); 
			// free(pmxctrl); 
			free(plisttext); 
			free(plistbool); 
		} 
		else 
			free(pmxctrl); 
		return   TRUE; 
	} 
	catch(...) 
	{ 
		return   FALSE; 
	} 
}