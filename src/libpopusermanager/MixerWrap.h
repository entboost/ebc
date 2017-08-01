#pragma once
#define MAX_VOL 100
class MixerWrap
{
public:
	MixerWrap(void){};
public:
	virtual ~MixerWrap(void){};

	static void MicphoneBoost(bool bIsBoost);	// 不支持WIN7
	
	static void MicphoneMut(BOOL bIsMut);		// 设置静音，支持WIN7
	static void StereoMix(BOOL bMute);			// 设置混音
	static bool StereoMix_W7(BOOL bMute, float fVolume=-1.0);	// 设置混音 小于0.0不设置音量
	static bool MicphoneMut_W7(BOOL bIsMut,float fVolume=-1.0); //设置麦克风音量  小于0.0不设置音量
	static bool GetMicphone_W7(BOOL &bMute,float &pdwVolume); //获取麦克风音量
	static void MicphoneBoost_W7(float fValue);//设置麦克风增强等级 0-30；支持WIN7
	static void GetMicphoneLevel(float &fValue); //获取麦克风增强等级 0-30；支持WIN7
	static void GetMicphoneLevelData(float &nMin, float &nMax, float &nStep); //获取麦克风最小值，最大值和增量
	static bool GetSpeakerValue_W7(BOOL &bMute, float &fVolume); //获取喇叭音量
	static bool GetSpeakerValue_W72(BOOL &bMute, float &fVolume); //获取喇叭音量
	static bool SetSpeakerValue_W7(BOOL bMute, float fVolume=-1.0); //设置喇叭静音和音量，小于0.0不设置音量
	static bool SetSpeakerValue_W72(BOOL bMute, float fVolume=-1.0); //设置喇叭静音和音量，小于0.0不设置音量

		
	//static bool MutSpeaker_W7(bool bMute,float fVolume);

	//立体声混音音量调节
	static void StereoMixVolume(int nVolume);		// 不支持WIN7
	static void OutPutMicphoneVolume(int nVolume);
	static void InPutMicphoneVolume(int nVolume);


	static int  SelectRecordIn(DWORD  dwSrcType);
	static int  SelectRecordInByName(const TCHAR*  szSrcName);
	

};
