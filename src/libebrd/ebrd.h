// ebrd.h file here
// 版权所有：深圳恩布网络科技有限公司 2012－2015
// 版    本：1.0.0.0

#ifndef __ebrd_h__
#define __ebrd_h__

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "ebrdhandle.h"

//EXPIMP_TEMPLATE template class EBRD_API std::string;
//EXPIMP_TEMPLATE template class EBRD_API std::vector<int>;

namespace entboost {

class EBRD_API Cebrd
{
public:
	typedef boost::shared_ptr<Cebrd> pointer;
	static Cebrd::pointer create(void)
	{
		return Cebrd::pointer(new Cebrd());
	}
	Cebrd(void);
	virtual ~Cebrd(void);
	int Start(const char* address, unsigned int bindPort);
	bool IsStart(void) const;
	void SetCallBack(CEBRDCallBack * pCallBack);
	void SetMsgHwnd(HWND pHwnd);
	void Stop(void);

	bool Register(mycp::bigint nRoomId, mycp::bigint nSourceId, mycp::bigint nParam, bool bIsGroup);
	mycp::bigint GetRoomId(void) const;
	mycp::bigint GetSourceId(void) const;
	void UnRegister(void);
	bool AddSink(mycp::bigint nDestId, HWND hWndParent);
	void DelSink(mycp::bigint nDestId);
	void DelAllSink(void);

	bool MoveSinkWindow(mycp::bigint nDestId, int x, int y, int width, int height, BOOL bRepaint=TRUE);
	// int nMouseFPS: 0: disable 1-10:enable
	bool ControlSinkDesktop(mycp::bigint nDestId, int nMouseFPS);
	// return MouseFPS: 0: disable
	int ControlSinkDesktop(mycp::bigint nDestId) const;

	// quant: 1-20
	// fps: 1-10
	bool StartLocalDesktop(int quant, int fps);
	int GetLocalDesktopFPS(void) const;
	// mycp::bigint nDestId : 0 disable
	void EnableControlLocalDesktop(mycp::bigint nDestId);
	mycp::bigint EnableControlLocalDesktop(void) const;
	void StopLocalDesktop(void);

	//bool SendData(const unsigned char* pSendData, mycp::uint32 nDataSize, mycp::uint32 nTimestamp, mycp::uint8 nDataType=SOTP_RTP_NAK_DATA_AUDIO, mycp::uint8 nNAKType=SOTP_RTP_NAK_REQUEST_1);
private:
	void * m_handle;
};

} // namespace entboost

#endif //__ebrd_h__
