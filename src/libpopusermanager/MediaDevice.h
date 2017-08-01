#ifndef __MediaDevice_h_
#define __MediaDevice_h_

#include "Mixer.h"
#include "MixerWrap.h"
#define  WIN_XP    2
#define  WIN_2003  3
#define  WIN_VISTA 4
#define  WIN_7     5
#define  WIN_2008  6
#define  WIN_8     8
int GetOSVer()  
{  
	OSVERSIONINFO   osver;     
	osver.dwOSVersionInfoSize   =   sizeof(OSVERSIONINFO);     
	GetVersionEx(&osver);     
	if(osver.dwPlatformId == 2)  
	{  
		if(osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)   //xp
		{  
			return WIN_XP;  
		}  
		if(osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)  //2003
		{  
			return WIN_2003;  
		}  
		if(osver.dwMajorVersion ==  6 && (osver.dwMinorVersion == 0 || osver.dwMinorVersion == 1))  //vista or 2008
		{  
			return WIN_VISTA;  
		}  
		if(osver.dwMajorVersion ==  6 && osver.dwMinorVersion == 1 )  //win7
		{  
			return WIN_7;  
		}  
		if(osver.dwMajorVersion ==  6 && osver.dwMinorVersion == 2)  //win8
		{  
			return WIN_8;  
		}  
	}  
	return 0;  
} 

void GetSystemVolume(BOOL &bMute, float &dVolumeValue) //获取主音量的值
{
	const int nOSVer = GetOSVer();
	if( nOSVer == WIN_XP)
	{
		CMixer mixer;
		bMute = mixer.GetMute(CMixer::SPEAKERS)?TRUE:FALSE;
		dVolumeValue = (float)(mixer.GetVolume(CMixer::SPEAKERS)*1.0);
	}
	else if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::GetSpeakerValue_W72(bMute,dVolumeValue); 
	}
}

void SetSystemVolume(BOOL bMute, float dVolumeValue) //设置主音量的值
{
	const int nOSVer = GetOSVer();
	if( nOSVer == WIN_XP)
	{
		CMixer mixer;
		mixer.SetMute(CMixer::SPEAKERS, bMute?true:false);
		if (dVolumeValue>=0.0)
			mixer.SetVolume(CMixer::SPEAKERS,(long)dVolumeValue);
	}
	else if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::SetSpeakerValue_W72(bMute,dVolumeValue);
	}
}

void GetMicrophoneValue(BOOL &bMute,float &dVolumeValue) //获取麦克风音量
{
	const int nOSVer = GetOSVer();
	if( nOSVer == WIN_XP)
	{
		CMixer mixer;
		bMute = mixer.GetMute(CMixer::MICROPHONE)?true:false;
		dVolumeValue = (float)mixer.GetVolume(CMixer::MICROPHONE);
	}
	else if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::GetMicphone_W7(bMute,dVolumeValue);
	}
}

void SetMicrophoneValue(BOOL bMute,float dVolumeValue) //设置麦克风音量
{
	const int nOSVer = GetOSVer();
	if( nOSVer == WIN_XP)
	{
		CMixer mixer;
		mixer.SetMute(CMixer::MICROPHONE, bMute?true:false);
		if (dVolumeValue>=0.0)
			mixer.SetVolume(CMixer::MICROPHONE,(long)dVolumeValue);
		//MixerWrap::MicphoneMut(bMute);
	}
	else if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::MicphoneMut_W7(bMute, dVolumeValue);
	}
}

bool GetMicroBoostData(float &fMin, float &fMax, float &fStep) //获取麦克风增强等级的最小值，最大值和增量
{
	const int nOSVer = GetOSVer();
	if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::GetMicphoneLevelData(fMin, fMax,fStep);
		return true;
	}
	return false;
}

bool GetMicroBoostLevel(float &dMicroBoostLevel) //获取麦克风增强等级
{
	const int nOSVer = GetOSVer();
	if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::GetMicphoneLevel(dMicroBoostLevel);
		return true;
	}
	return false;
}

void SetMicroBoostLevel(float dMicroBoostLevel) //设置麦克风增强等级
{
	const int nOSVer = GetOSVer();
	if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::MicphoneBoost_W7(dMicroBoostLevel);
	}else
	{
		// XP: 大于0.0为启用，小于0.0为禁用
		MixerWrap::MicphoneBoost(dMicroBoostLevel>0.0?true:false);
	}
}

void SetStereoMix(BOOL bMute) //设置混音静音，TRUE=混音静音；FALSE＝混音开声
{
	const int nOSVer = GetOSVer();
	if( nOSVer == WIN_XP)
	{
		MixerWrap::StereoMix(bMute);
	}
	else if(nOSVer == WIN_7|| nOSVer == WIN_8 || nOSVer == WIN_VISTA)
	{
		MixerWrap::StereoMix_W7(bMute);
	}
}

#endif // __MediaDevice_h_
