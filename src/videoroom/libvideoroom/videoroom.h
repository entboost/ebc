// videoroom.h file here
#ifndef __videoroom_h__
#define __videoroom_h__

#ifdef VIDEOROOM_EXPORTS
#define VIDEOROOM_API __declspec(dllexport)
#else
#define VIDEOROOM_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include "videoroomdefine.h"

class VIDEOROOM_API Cvideoroom
{
public:
	Cvideoroom(void);
	virtual ~Cvideoroom(void);
	static void VR_GetVideoDevices(std::vector<std::string>& pOutDevices);

	int VR_Init(vr::bigint nlocalid, vr::VideoStreamCallBack cb);
	bool VR_IsInited(void) const;
	//void setlocalid(vr::bigint nlocalid);
	void VR_UnInit(void);

	void VR_SetVideoServer(const char* sAddress, unsigned short nPort);
	void VR_SetAudioServer(const char* sAddress, unsigned short nPort);

	// VideoQuality:0-10; default 3
	bool VR_InitVideoCapture(unsigned short nvideoindex,int nVideoQuality=3);
	bool VR_InitAudioCapture(void);
	int VR_GetVideoType(void) const;
	
	void VR_SetVideoSend(bool bSend);
	void VR_SetAudioSend(bool bSend);
	bool VR_GetVideoSend(void) const;
	bool VR_GetAudioSend(void) const;

	void VR_StartVideoCapture(void);
	void VR_StopVideoCapture(void);
	void VR_StartAudioCapture(void);
	void VR_StopAudioCapture(void);

	// 我看谁
	void VR_AddMeLooks(vr::bigint nuserid);
	void VR_DelMeLook(vr::bigint nuserid);
	// 谁看我
	void VR_AddLookMes(vr::bigint nuserid);
	void VR_DelLookMes(vr::bigint nuserid);

	void VR_AddVideoStream(vr::bigint nuserid, unsigned long udata, bool bLocalVideo);
	void VR_DelVideoStream(vr::bigint nuserid);
	// 本地音量条
	void VR_SetLocalAudiolStream(vr::AudioStreamCallBack cb, void * udata);
	//void VR_SetLocalAudiolStream(vr::bigint nuserid, AudioStreamCallBack cb, void * udata);
	void VR_AddAudioStream(vr::bigint nuserid, vr::AudioStreamCallBack cb, void * udata);
	void VR_DelAudioStream(vr::bigint nuserid);
	void VR_PauseAudioStream(vr::bigint nuserid);
	void VR_PlayAudioStream (vr::bigint nuserid);

private:
	void * m_handle;
};

class VIDEOROOM_API Cvideowindow
{
public:
	Cvideowindow(HWND hwnd, 
		int nRGB,
		int videoWidth,
		int VideoHeight,
		int left = 0, 
		int top = 0,
		int right = 0,
		int bottom = 0);
	virtual ~Cvideowindow(void);

	HWND VW_GetParentWnd(void) const;
	HRESULT	VW_BuildGraph(bool bDefaultRender = false);
	HRESULT	VW_StartGraph(void);
	HRESULT VW_StopGraph(void);
	HRESULT	VW_ReleaseGraph(void);

	void VW_MoveWindow( int left, int top, int right, int bottom);
	int VW_DrawVideo(void * videodata, int len );
	void VW_ShowVideoWindow(bool show);
	//void NotifyOwnerMessage(HWND hWnd, long uMsg,LONG_PTR wParam,LONG_PTR lParam);
	//HRESULT ProcessGraphNotify(long *lEventCode, LONG_PTR *lParam1, LONG_PTR *lParam2);

	bool VW_SaveBitmapFile(const char * sSaveToFile);
private:
	void * m_handle;
};

#endif // __videoroom_h__
