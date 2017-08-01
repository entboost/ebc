// Mixer.cpp: implementation of the CMixer class.
//
//#include "stdwx.h"
#include "Mixer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMixer::CMixer(HWND wnd)
: m_hWnd(wnd)
, m_nParam(0)

{
	m_VolRange = 100;

	for( int i = 0 ; i < MAX_MIXER_HANDLE ; i ++ )
		m_hMixer[i] = NULL;
}

CMixer::~CMixer()
{
	for( int i = 0 ; i < MAX_MIXER_HANDLE ; i ++ )
	{
		if (m_hMixer[i] != NULL)
			mixerClose(m_hMixer[i]);
	}
}

CMixer::CMixer(const int VolRange)
{
	m_VolRange = VolRange;
}
//----------------------------设定音量---------------------------------------

bool CMixer::GetVolumeControl(HMIXER hmixer ,long componentType,long ctrlType,MIXERCONTROL* mxc) const
{
	////查询 录音控制面版中 麦克风线路的编号 
	//MIXERLINE mxl_v;
	//UINT cConnections = (UINT)mxl.cConnections;
	//UINT dwSource_v=0;
	//do
	//{
	//	mxl_v.cbStruct = sizeof(mxl_v);
	//	mxl_v.dwDestination = mxl.dwDestination;
	//	mxl_v.dwSource = dwSource_v;
	//	dwSource_v++;
	//	if (MMSYSERR_NOERROR != 
	//		mixerGetLineInfo((HMIXEROBJ)m_hmx2, 
	//		&mxl_v, 
	//		MIXER_GETLINEINFOF_SOURCE))
	//		return FALSE;
	//} while ((dwSource_v < cConnections) && 
	//	(mxl_v.dwComponentType != MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE));

	//if((dwSource_v > cConnections) || 
	//	(mxl_v.dwComponentType !=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE))
	//	return FALSE; 
	//dwSource_v--;//得到编号


	////下面实现了"强制"单选麦克风线路为录音源输入
	////MIXERCONTROL mxc;
	//MIXERLINECONTROLS mxlc;
	//mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	//mxlc.dwLineID = mxl.dwLineID;
	//mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
	////MIXERCONTROL_CONTROLTYPE_MUTE;
	//mxlc.cControls = 1;
	//mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	//mxlc.pamxctrl = mxc; //为了得到相应的mxc.dwControlID，
	////锁定mxlc.dwLineID和mxlc.dwControlType指定的线路控制
	////目前将对录音目标单元的录音音源线路做单选（多路复用）
	//if (::mixerGetLineControls((HMIXEROBJ)m_hmx2,
	//	&mxlc,
	//	MIXER_OBJECTF_HMIXER |
	//	MIXER_GETLINECONTROLSF_ONEBYTYPE)
	//	!= MMSYSERR_NOERROR) return -1; 

	////根据mxc.dwControlID检索线路，
	//MIXERCONTROLDETAILS_BOOLEAN mxcdMute[8];
	//MIXERCONTROLDETAILS mxcd;
	//mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	//mxcd.dwControlID = mxc.dwControlID;//在上面的&mxc得到
	//mxcd.cChannels = 1;
	//mxcd.cMultipleItems =mxc.cMultipleItems;//多路因子数目
	//mxcd.cbDetails = sizeof(*mxcdMute);
	//mxcd.paDetails =&mxcdMute;//存放检索结果
	//if (::mixerGetControlDetails((HMIXEROBJ)m_hmx2,
	//	&mxcd,
	//	MIXER_OBJECTF_HMIXER |
	//	MIXER_GETCONTROLDETAILSF_VALUE)
	//	!= MMSYSERR_NOERROR) return -1;

	////做相应单选改变
	//int j;
	//for(j=0;j<(int)mxc.cMultipleItems;j++)
	//	mxcdMute[j].fValue=false;
	//mxcdMute[dwSource_v].fValue=true; 

	////重新设置dwControlID锁定的线路控制
	//if (::mixerSetControlDetails((HMIXEROBJ)hmixer,
	//	&mxcd,
	//	MIXER_OBJECTF_HMIXER |
	//	MIXER_GETCONTROLDETAILSF_VALUE)
	//	!= MMSYSERR_NOERROR) return -1; 
//}//for(i;i<devnum;i++)
//return TRUE;
//}
//

	MIXERLINECONTROLS mxlc;
	MIXERLINE mxl;
	bool exist = false;
	memset(&mxl, 0, sizeof(MIXERLINE));
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = componentType;

	if (/*componentType == MIXERLINE_COMPONENTTYPE_SRC_DIGITAL ||
		*/componentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
	{
		//mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		// add by yhz
		MMRESULT mr = mixerGetLineInfo((HMIXEROBJ)hmixer, &mxl, MIXER_OBJECTF_HMIXER|MIXER_GETLINEINFOF_COMPONENTTYPE);
		if (mr |= MMSYSERR_NOERROR)
			return false;
		exist = true;

		////获取录音麦克风设备
		//mxl.dwComponentType   =   MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		////   得到录制总线中的连接数  
		//MMRESULT mr = mixerGetLineInfo(   (HMIXEROBJ)hmixer,   &mxl,  
		//	MIXER_OBJECTF_HMIXER   |   MIXER_GETLINEINFOF_COMPONENTTYPE   );  

		//if (mr |= MMSYSERR_NOERROR)
		//	return false;

		////   将连接数保存  
		//DWORD   dwConnections   =   mxl.cConnections;  
		////   准备获取麦克风设备的ID  
		//DWORD   dwLineID   =   0;  
		//for   (   DWORD   i   =   0;   i   <   dwConnections;   i++   )  
		//{  
		//	//   枚举每一个设备，当Source的ID等于当前的迭代记数  
		//	mxl.dwSource   =   i;  
		//	//   根据SourceID获得连接的信息  
		//	mr   =   mixerGetLineInfo(   (HMIXEROBJ)hmixer,   &mxl,  
		//		MIXER_OBJECTF_HMIXER   |   MIXER_GETLINEINFOF_SOURCE   );  
		//	//   判断函数执行错误  
		//	if   (   mr   !=   0   )  
		//	{  
		//		// add by yhz
		//		continue;
		//		//break;  
		//	}  
		//	//   如果当前设备类型是麦克风，则跳出循环。  
		//	if   (   mxl.dwComponentType   ==  
		//		MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE   )  
		//	{  
		//		exist = true;
		//		break;  
		//	}  
		//}
	}else if(!mixerGetLineInfo((HMIXEROBJ)hmixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
	{
		exist = true;
	}

	if (exist)
	{
		mxlc.cbStruct = sizeof(mxlc);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = ctrlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = mxc;
		if(mixerGetLineControls((HMIXEROBJ)hmixer,&mxlc,MIXER_GETLINECONTROLSF_ONEBYTYPE))
			return 0;
		else
			return 1;
	}
	return 0;
}
//---------------------------------------------------------------------------
long CMixer::GetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc) const
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	mxcd.hwndOwner = 0;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbDetails = sizeof(mxcdMute);
	mxcd.paDetails = &mxcdMute;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	if (mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd,MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))
		return 0;
		//return -1;
	return mxcdMute.fValue;
}

//---------------------------------------------------------------------------
unsigned long CMixer::GetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc) const
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol;
	vol.dwValue=0;
	mxcd.hwndOwner = 0;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;
	if(mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))
		return -1;
	return vol.dwValue;
}

//---------------------------------------------------------------------------
bool CMixer::SetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc, bool mute)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	mxcdMute.fValue=mute;
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(mxcdMute);
	mxcd.paDetails = &mxcdMute;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	if (mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))
		return false;
	return true;
}

//---------------------------------------------------------------------------

bool CMixer::SetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc, long volume)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol;
	vol.dwValue = volume;
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;
	return (mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE)) == 0;
}

long CMixer::GetComponentType(MixerDevice dev) const
{
	switch (dev)
	{
	case WAVEOUT:
		return MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	case SYNTHESIZER:
		return MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
	//case WAVEIN:
	//	return MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	//case DIGITAL:
	//	return MIXERLINE_COMPONENTTYPE_SRC_DIGITAL;
	case MICROPHONE:
		//return MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;    //麦克风音量
		return MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;		// cd 音量
		// device=MIXERLINE_COMPONENTTYPE_SRC_LINE; break;         //PC 扬声器音量

	case STEREOMIX:
		return MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER;	// ??
	default:
		return MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	}

}

bool CMixer::OpenMixer(MixerDevice dev)
{
	if (m_hMixer[dev] == NULL)
	{
		DWORD dwOpen = 0;
		if (m_hWnd)
			dwOpen |= CALLBACK_WINDOW;

		if(mixerOpen(&m_hMixer[dev], 0, (DWORD)m_hWnd, 0, dwOpen)) return false;
	}
	return true;
}

void CMixer::CloseMixer(MixerDevice dev)
{
	if (m_hMixer[dev] != NULL)
	{
		mixerClose(m_hMixer[dev]);
		m_hMixer[dev] = NULL;
	}
}

//---------------------------------------------------------------------------
unsigned long /*WINAPI */CMixer::GetVolume(MixerDevice dev)//得到设备的音量dev=0主音量，1WAVE ,2MIDI ,3 LINE IN
{

	unsigned rt=0;
	MIXERCONTROL volCtrl;

	if (!OpenMixer(dev)) return -1;

	long device = GetComponentType(dev);
	if(!GetVolumeControl(m_hMixer[dev],device,MIXERCONTROL_CONTROLTYPE_VOLUME,&volCtrl))
	{
		return -1;
	}
	rt=GetVolumeValue(m_hMixer[dev],&volCtrl)*m_VolRange /volCtrl.Bounds.lMaximum;
	return rt;
}

bool CMixer::GetMaxMin(MixerDevice dev, long & min, long & max)
{
	bool rc=false;
	MIXERCONTROL volCtrl;
	long device = GetComponentType(dev);

	if (!OpenMixer(dev)) return false;

	if(GetVolumeControl(m_hMixer[dev],device,MIXERCONTROL_CONTROLTYPE_VOLUME,&volCtrl))
	{
		min = volCtrl.Bounds.lMinimum;
		max = volCtrl.Bounds.lMaximum;
		rc = true;
	}
	return rc;
}

bool CMixer::SelectSteteoMix(int SteteoMix)
{
	if (SteteoMix == 0)
	{
		SetMute(CMixer::MICROPHONE, false);
		//SetMute(CMixer::DIGITAL, false);
		SetMute(CMixer::WAVEOUT, true);
	}else if (SteteoMix == 1)
	{
		SetMute(CMixer::MICROPHONE, true);
		//SetMute(CMixer::DIGITAL, true);
		SetMute(CMixer::WAVEOUT, false);
	}else
	{
		SetMute(CMixer::MICROPHONE, false);
		//SetMute(CMixer::DIGITAL, false);
		SetMute(CMixer::WAVEOUT, false);
	}
	return true;

	bool rc=false;
	//MIXERCONTROL volCtrl;
	//MixerDevice dev = STEREOMIX;
	//long device = GetComponentType(dev);

	//if (m_hMixer[STEREOMIX] == NULL)
	//	if(mixerOpen(&m_hMixer[dev], 0, 0, 0, 0)) return false;

	//if(GetVolumeControl(m_hMixer[dev],device,MIXERCONTROL_CONTROLTYPE_ONOFF,&volCtrl))
	//	if(SetMuteValue(m_hMixer[dev],&volCtrl,(bool)false))
	//		rc=true;

	return rc;
}

CMixer::MixerDevice CMixer::GetMixerDevice(HMIXER hMixer) const
{
	for( int i = 0 ; i < MAX_MIXER_HANDLE ; i++ )
	{
		if (m_hMixer[i] == hMixer)
			return (MixerDevice)i;
	}

	return UNKNOWN;
}

//---------------------------------------------------------------------------

bool /*WINAPI*/ CMixer::SetVolume(MixerDevice dev,long vol)//设置设备的音量
{
	// dev =0,1,2   分别表示主音量,波形,MIDI ,LINE IN
	// vol=0-m_VolRange      表示音量的大小 , 设置与返回音量的值用的是百分比，即音量从0 - m_VolRange ，而不是设备的绝对值
	// retrun false 表示设置音量的大小的操作不成功
	// retrun true 表示设置音量的大小的操作成功

	bool rc=false;
	MIXERCONTROL volCtrl;
	long device = GetComponentType(dev);

	if (!OpenMixer(dev)) return false;

	if(GetVolumeControl(m_hMixer[dev],device,MIXERCONTROL_CONTROLTYPE_VOLUME,&volCtrl))
	{
		vol=vol*volCtrl.Bounds.lMaximum/m_VolRange ;
		if(SetVolumeValue(m_hMixer[dev],&volCtrl,vol))
			rc=true;
	}
	return rc;
}

//---------------------------------------------------------------------------
#include <stdio.h>
bool /*WINAPI*/ CMixer::SetMute(MixerDevice dev,/*long*/bool vol)//设置设备静音
{
	// dev =0,1,2   分别表示主音量,波形,MIDI ,LINE IN
	// vol=0,1      分别表示取消静音,设置静音
	// retrun false 表示取消或设置静音操作不成功
	// retrun true 表示取消或设置静音操作成功

	bool rc=false;
	MIXERCONTROL volCtrl;
	long device = GetComponentType(dev);

	//if (dev == MICROPHONE)
	//{
	//	long device2 = MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED;
	//	while (device2 < MIXERLINE_COMPONENTTYPE_SRC_LAST)
	//	{
	//		if (!OpenMixer(dev)) return false;

	//		if(GetVolumeControl(m_hMixer[dev],device2,MIXERCONTROL_CONTROLTYPE_MUTE,&volCtrl))
	//		{
	//			if(SetMuteValue(m_hMixer[dev],&volCtrl,(bool)vol))
	//			{
	//				rc=true;
	//				int i = device2 - MIXERLINE_COMPONENTTYPE_SRC_FIRST;
	//				char buffer[20];
	//				sprintf(buffer, "set mute = %d", i);
	//				MessageBoxA(NULL, buffer, buffer, MB_OK);
	//				//break;
	//			}
	//		}

	//		device2 += 1;
	//	}
	//}

	if (!OpenMixer(dev)) return false;

	if(GetVolumeControl(m_hMixer[dev],device,MIXERCONTROL_CONTROLTYPE_MUTE,&volCtrl))
		if(SetMuteValue(m_hMixer[dev],&volCtrl,(bool)vol))
			rc=true;

	//if (!rc)
	//	CloseMixer(dev);

	return rc;
}

//---------------------------------------------------------------------------

bool /*WINAPI */CMixer::GetMute(MixerDevice dev)//检查设备是否静音
{
	//dev =0,1,2 分别表示主音量，波形,MIDI ,LINE IN
	// retrun false 表示没有静音
	// retrun true 表示静音
	bool rc=false;
	MIXERCONTROL volCtrl;
	long device = GetComponentType(dev);

	if (!OpenMixer(dev)) return false;

	if(GetVolumeControl(m_hMixer[dev],device,MIXERCONTROL_CONTROLTYPE_MUTE,&volCtrl))
		rc=GetMuteValue(m_hMixer[dev],&volCtrl)==0?true:false;
	return rc;
}