// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXER_H__7369BAA5_012E_467E_AD1E_A383E4474C58__INCLUDED_)
#define AFX_MIXER_H__7369BAA5_012E_467E_AD1E_A383E4474C58__INCLUDED_
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers

#include "windows.h"
#include <mmsystem.h>

#pragma comment (lib, "winmm.lib")
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_MIXER_HANDLE 5
//#define MAX_MIXER_HANDLE 7

class CMixer
{
public:
	CMixer(HWND wnd = NULL);
	CMixer(const int VolRange);
	virtual ~CMixer();
	enum MixerDevice{
		SPEAKERS=0,
		WAVEOUT,
		//WAVEIN,
		//DIGITAL,
		SYNTHESIZER,
		MICROPHONE,
		STEREOMIX,	// 立体声混音

		UNKNOWN	= 0xff
	};

	void SetHwnd(HWND wnd) {m_hWnd = wnd;}
	HWND GetHwnd(void) const {return m_hWnd;}

	bool GetMute(MixerDevice dev);   //检查设备是否静音
	bool SetMute(MixerDevice dev, bool vol);     //设置设备静音
	bool SetVolume(MixerDevice dev, long vol);   //设置设备的音量
	unsigned long GetVolume(MixerDevice dev);    //得到设备的音量dev=0主音量，1WAVE ,2MIDI ,3 LINE IN

	bool GetMaxMin(MixerDevice dev, long & min, long & max);
	long GetVolRange(void) const {return m_VolRange;}
	bool SelectSteteoMix(int SteteoMix = 0);	// 0:mic only; 1:waveout only; 2:both

	MixerDevice GetMixerDevice(HMIXER hMixer) const;

	void SetParam(UINT param) {m_nParam = param;}
	UINT GetParam(void) const {return m_nParam;}

private:
	bool GetVolumeControl(HMIXER hmixer ,long componentType,long ctrlType,MIXERCONTROL* mxc) const;
	bool SetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc, long volume);
	bool SetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc, bool mute);
	unsigned long GetVolumeValue(HMIXER hmixer ,MIXERCONTROL *mxc) const;
	long GetMuteValue(HMIXER hmixer ,MIXERCONTROL *mxc) const;

	long GetComponentType(MixerDevice dev) const;
	bool OpenMixer(MixerDevice dev);
	void CloseMixer(MixerDevice dev);

	HWND m_hWnd;
	long m_VolRange ;
	HMIXER    m_hMixer[MAX_MIXER_HANDLE];

	UINT m_nParam;
};

#endif // !defined(AFX_MIXER_H__7369BAA5_012E_467E_AD1E_A383E4474C58__INCLUDED_)