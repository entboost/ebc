// ebmm.h file here
// 版权所有：深圳恩布网络科技有限公司 2012－2017
// 版    本：2.0.0.0

#ifndef __ebmm_h__
#define __ebmm_h__

#include <QWidget>
#include <CGCBase/cgcSmartString.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "ebmmhandle.h"

namespace entboost {

class EBMM_API Cebmm
{
public:
	typedef boost::shared_ptr<Cebmm> pointer;
	Cebmm(void);
	virtual ~Cebmm(void);
    static Cebmm::pointer create(void);
	int Start(const char* sAudioAddress, const char* sVideoAddress, unsigned int bindPort);
	bool IsStart(void) const;
	void SetCallBack(FPMMCallBack pCallBack, void* pUserData);
	void Stop(void);

	bool Register(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam, bool bIsGroup);
	mycp::bigint GetRoomId(void) const;
	mycp::bigint GetSourceId(void) const;
	void UnRegister(void);
	bool AddAudioSink(mycp::bigint nDestId);
	bool IsAudioSink(mycp::bigint nDestId) const;
    bool AddVideoSink(mycp::bigint nDestId, QWidget *hVideoWndParent);
	bool IsVideoSink(mycp::bigint nDestId) const;
	void DelAudioSink(mycp::bigint nDestId);
	void DelVideoSink(mycp::bigint nDestId);
	void DelDestSink(mycp::bigint nDestId);
	void DelAllSink(void);

    /// P2PStatus 0=not-p2p 1=pop-trying 2=p2p-ok
	int GetAudioP2PStatus(void) const;
	int GetVideoP2PStatus(void) const;

    static void GetVideoDevices(std::vector<QString> &pOutDevices);
	int GetVideoDeviceSize(void) const;
    /// nVideoQuality: 0（节省带宽）-10（高清）: 默认3，标清
	bool InitVideoCapture(int nVideoDeviceIndex, int nVideoQuality=3);
	bool InitAudioCapture(void);

    void StartVideoCapture(QWidget *localVideoWndParent);
	void StopVideoCapture(void); 
    void StartAudioCapture(void);
	void StopAudioCapture(void);

	bool StartVideoGraph(mycp::bigint nUserId);
	bool MoveVideoWindow(mycp::bigint nUserId, int x, int y, int width, int height);
	bool ShowVideoWindow(mycp::bigint nUserId, bool bShow);
    bool SaveVideoBitmapFile(mycp::bigint nUserId, const QString &sSaveToFile);

    /// default true
	void SetLocalVideoSend(bool bSend);
	bool GetLocalVideoSend(void) const;
	void SetLocalAudioSend(bool bSend);
	bool GetLocalAudioSend(void) const;

    /// default true
	void SetLocalVideoPlay(bool bPlay);
	bool GetLocalVideoPlay(void) const;
	void SetLocalAudioPlay(bool bPlay);
	bool GetLocalAudioPlay(void) const;
	void SetSinkVideoPlay(mycp::bigint nUserId, bool bPlay);
	bool GetSinkVideoPlay(mycp::bigint nUserId) const;
	void SetSinkAudioPlay(mycp::bigint nUserId, bool bPlay);
	bool GetSinkAudioPlay(mycp::bigint nUserId) const;

private:
	void * m_handle;
};

} // namespace entboost

#endif //__ebmm_h__
