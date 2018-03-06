// ebmmhandle.h file here
#ifndef __ebmmhandle_h__
#define __ebmmhandle_h__

#ifdef EBUI_EXPORTS
#define EBUI_API __declspec(dllexport)
#else
#define EBUI_API __declspec(dllimport)
#endif

#include <string>
namespace ebui
{
#ifdef WIN32
	typedef __int64				bigint;
#define eb_atoi64(a) _atoi64(a)
#define eb_sprintfi64(a,i64) sprintf(a,"%I64d",i64)
#else
	typedef long long			bigint;
#define eb_atoi64(a) atoll(a)
#define eb_sprintfi64(a,i64) sprintf(a,"%lld",i64)
#endif


} // cgc namespace


class CEBUIHandler
{
public:
	// 登录成功
	virtual void OnLogonSuccess(ebui::bigint nUserId, const char* lpszAccount) = 0;
	// 登录失败
	virtual void OnLogonError(int nErrorCode, const char* lpszErrorString="") {}	
	// 当前用户，已经在其他电脑登录；
	virtual void OnLogonAnother(void) {}	
};

#define EBUI_WM_DEFAULT_MSG_ID			(WM_USER+100)
#define EBUI_MSG_TYPE_RECEIVE_RICH		1	// 收到聊天消息
#define EBUI_MSG_TYPE_RECEIVING_FILE	2	// 收到对方发送文件
#define EBUI_MSG_TYPE_RECEIVED_FILE		3	// 收到一个文件，接收成功，或者离线文件 lparam 是否离线文件：0/1


#endif // __ebmmhandle_h__
