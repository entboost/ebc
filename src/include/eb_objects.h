// eb_objects.h file here
#ifndef __eb_objects_h__
#define __eb_objects_h__

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <CGCLib/CGCLib.h>
#include <CGCBase/cgcSmartString.h>
#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcparameter.h>
#include <CGCBase/cgcattachment.h>
#include <boost/thread/mutex.hpp>
#include <stl/locklist.h>
using namespace mycp;
#include "eb_defines.h"
#include "eb_object.h"
#ifdef USES_REMOTE_DESKTOP
//#define USES_RD_COM
#ifdef USES_RD_COM
#include <atlbase.h>
#include <atlcom.h>
#import "..\build\ebcore.dll"  no_namespace	// for ebclient&ebrd
#else
#include "ebrd.h"
#endif // USES_RD_COM


#endif
#ifdef USES_CHATROOM
#include "chatroom.h"
#endif
#ifdef USES_VIDEOROOM
#include "ebmm.h"
#endif

#ifdef _QT_MAKE_
#include <QFile>
#endif

#define USES_DELETE_FILE_LIST_T

namespace entboost {

class CEBFileInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CEBFileInfo> pointer;
	static CEBFileInfo::pointer create(void)
	{
		return CEBFileInfo::pointer(new CEBFileInfo());
	}
	static CEBFileInfo::pointer create(const CEBFileInfo* p)
	{
		return CEBFileInfo::pointer(new CEBFileInfo(p));
	}
#ifdef USES_DELETE_FILE_LIST_T
	mycp::bigint m_nResourceId;
#else
	mycp::bigint m_nFileListId;
#endif
	mycp::bigint m_nFromId;
	int m_nFromType;
	mycp::bigint m_nFileId;
	tstring m_sFileServer;
	int m_nFileServiceId;
	int m_nFileOffset;
	int m_nFileSize;
	int m_nUnZipSize;
        EBFileString m_sFileName;
	tstring m_sFileExt;
#ifdef USES_DELETE_FILE_LIST_T
	EB_RESOURCE_TYPE m_nResourceType;
#else
	int m_nFileType;
#endif
	tstring m_sTempFilePath;	// clear delete
	unsigned int m_nUserData;
	time_t m_tCreateTime;

	CEBFileInfo(const CEBFileInfo* p)
#ifdef USES_DELETE_FILE_LIST_T
		: m_nResourceId(0)
#else
		: m_nFileListId(0)
#endif
		, m_nFromId(0), m_nFromType(0)
		, m_nFileId(0)
		, m_nFileServiceId(0)
		, m_nFileOffset(0)
		, m_nFileSize(0), m_nUnZipSize(0)
#ifdef USES_DELETE_FILE_LIST_T
		, m_nResourceType(EB_RESOURCE_UNKNOWN)
#else
		, m_nFileType(0)
#endif
		, m_nUserData(0)
		, m_tCreateTime(0)
	{
		this->operator = (p);
	}
	CEBFileInfo(void)
#ifdef USES_DELETE_FILE_LIST_T
		: m_nResourceId(0)
#else
		: m_nFileListId(0)
#endif
		, m_nFromId(0), m_nFromType(0)
		, m_nFileId(0)
		, m_nFileServiceId(0)
		, m_nFileOffset(0)
		, m_nFileSize(0), m_nUnZipSize(0)
#ifdef USES_DELETE_FILE_LIST_T
		, m_nResourceType(EB_RESOURCE_UNKNOWN)
#else
		, m_nFileType(0)
#endif
		, m_nUserData(0)
		, m_tCreateTime(0)
	{}
	virtual ~CEBFileInfo(void)
	{
		if (!m_sTempFilePath.empty())
			remove(m_sTempFilePath.c_str());
	}
	const CEBFileInfo& operator =(const CEBFileInfo& p) {return this->operator =(&p);}
	const CEBFileInfo& operator =(const CEBFileInfo::pointer& p) {return this->operator =(p.get());}
	const CEBFileInfo& operator =(const CEBFileInfo* p)
	{
		if (p!=NULL)
		{
#ifdef USES_DELETE_FILE_LIST_T
			m_nResourceId = p->m_nResourceId;
#else
			m_nFileListId = p->m_nFileListId;
#endif
			m_nFromId = p->m_nFromId;
			m_nFromType = p->m_nFromType;
			m_nFileId = p->m_nFileId;
			m_sFileServer = p->m_sFileServer;
			m_nFileServiceId = p->m_nFileServiceId;
			m_nFileOffset = p->m_nFileOffset;
			m_nFileSize = p->m_nFileSize;
			m_nUnZipSize = p->m_nUnZipSize;
			m_sFileName = p->m_sFileName;
			m_sFileExt = p->m_sFileExt;
#ifdef USES_DELETE_FILE_LIST_T
			m_nResourceType = p->m_nResourceType;
#else
			m_nFileType = p->m_nFileType;
#endif
			m_sTempFilePath = p->m_sTempFilePath;	// clear delete
			m_nUserData = p->m_nUserData;
			m_tCreateTime = p->m_tCreateTime;
		}
		return *this;
	}
};
class CEBContentInfo
{
public:
	typedef boost::shared_ptr<CEBContentInfo> pointer;
	static CEBContentInfo::pointer create(void)
	{
		return CEBContentInfo::pointer(new CEBContentInfo());
	}
	mycp::bigint m_nContentListId;
	mycp::bigint m_nFromId;
	int m_nFromType;
	mycp::bigint m_nContentId;
	tstring m_sContent;

	CEBContentInfo(void)
		: m_nContentListId(0)
		, m_nFromId(0), m_nFromType(0)
		, m_nContentId(0)
	{}
	const CEBContentInfo& operator =(const CEBContentInfo& p) {return this->operator =(&p);}
	const CEBContentInfo& operator =(const CEBContentInfo::pointer& p) {return this->operator =(p.get());}
	const CEBContentInfo& operator =(const CEBContentInfo* p)
	{
		if (p!=NULL)
		{
			m_nContentListId = p->m_nContentListId;
			m_nFromId = p->m_nFromId;
			m_nFromType = p->m_nFromType;
			m_nContentId = p->m_nContentId;
			m_sContent = p->m_sContent;
		}
		return *this;
	}
};

class CSaveFileInfo
{
public:
	typedef boost::shared_ptr<CSaveFileInfo> pointer;
	static CSaveFileInfo::pointer create(void)
	{
		return CSaveFileInfo::pointer(new CSaveFileInfo());
	}
	bool IsOpened(void) const {return m_file==NULL?false:true;}
	bool Open(const char* lpszFilePath)
	{
		Close();
		m_nCurrentIndex = 0;
		m_file = fopen(lpszFilePath,"r+b");		// ** 先打开存在文件（2016-07-24）
		//m_file = fopen(lpszFilePath,"ab");	// ** 不能修改该行代码（加了 ZIP 后，不能 seek write）
		if (m_file==NULL)
		{
			m_file = fopen(lpszFilePath,"wb");	// ** 文件不存在，自动创建打开
			if (m_file==NULL)
			{
				return false;
			}
		}else
		{
			SeekToEnd();
//#ifdef WIN32
//			_fseeki64(m_file,0,SEEK_END);
//			m_nCurrentIndex = (unsigned int)_ftelli64(m_file);
//			//m_nCurrentIndex = (unsigned int)_ftelli64(m_file);
//#else
//			fseeko(m_file,0,SEEK_END);
//			m_nCurrentIndex = (unsigned int)ftello(m_file);
//#endif
		}
		//m_sFilePath = lpszFilePath;
		//m_sFileName = lpszFileName;
		return true;
	}
	void SeekToEnd(void)
	{
		if (m_file!=NULL)
		{
#ifdef WIN32
			_fseeki64(m_file,0,SEEK_END);
			m_nCurrentIndex = (unsigned int)_ftelli64(m_file);
#else
			fseeko(m_file,0,SEEK_END);
			m_nCurrentIndex = (unsigned int)ftello(m_file);
#endif
		}
	}
	void Close(void)
	{
		if (m_file!=NULL)
		{
			fclose(m_file);
			m_file = NULL;
		}
	}
	void Write(const char* lpszData, unsigned int nSize, bool bFlush=true)
	{
		if (m_file!=NULL)
		{
			fwrite(lpszData,1,nSize,m_file);
			if (bFlush)
				fflush(m_file);
			m_nCurrentIndex += nSize;
		}
	}
	void Seek(unsigned int nOffset)
	{
		if (m_file!=NULL && m_nCurrentIndex!=nOffset)
		{
#ifdef WIN32
			_fseeki64(m_file, nOffset, SEEK_SET);
#else
			fseeko(m_file, nOffset, SEEK_SET);
#endif
			m_nCurrentIndex = nOffset;
		}
	}
	void Flush(void)
	{
		if (m_file!=NULL)
			fflush(m_file);
	}

	unsigned int GetFileSize(void) const {return m_nCurrentIndex;}
	void SetFileId(mycp::bigint cfid) {m_nFileId = cfid;}
	mycp::bigint GetFileId(void) const {return m_nFileId;}

	CSaveFileInfo(void)
		: m_file(NULL), m_nCurrentIndex(0), m_nFileId(0)
	{
	}
	virtual ~CSaveFileInfo(void)
	{
		Close();
	}
private:
	FILE * m_file;
	unsigned int m_nCurrentIndex;
	mycp::bigint m_nFileId;
};

class CEBAreaInfo : public EB_AreaInfo
{
public:
	typedef boost::shared_ptr<CEBAreaInfo> pointer;
	static CEBAreaInfo::pointer create(void)
	{
		return CEBAreaInfo::pointer(new CEBAreaInfo());
	}
	static CEBAreaInfo::pointer create(int nParent)
	{
		return CEBAreaInfo::pointer(new CEBAreaInfo(nParent));
	}
	CEBAreaInfo(void)
		: EB_AreaInfo()
		, m_nSize(-1)
	{}
	CEBAreaInfo(int nParent)
		: EB_AreaInfo(nParent)
		, m_nSize(-1)
	{
	}
	CLockMap<int,CEBAreaInfo::pointer> m_pAreaInfoList;
	int m_nSize;
};


class CEBReqInfo
{
public:
	typedef boost::shared_ptr<CEBReqInfo> pointer;
	static CEBReqInfo::pointer create(mycp::bigint nId,mycp::bigint nUserId,mycp::bigint nFromIp,const tstring& sReqKey)
	{
		return CEBReqInfo::pointer(new CEBReqInfo(nId,nUserId,nFromIp,sReqKey));
	}

	mycp::bigint GetId(void) const {return m_nId;};
	mycp::bigint GetUserId(void) const {return m_nUserId;};
	mycp::bigint GetFromIp(void) const {return m_nFromIp;};
	const tstring& GetReqKey(void) const {return m_sReqKey;}
	time_t GetReqTime(void) const {return m_tReqTime;}

	void SetAccount(const tstring& v) {m_sAccount = v;}
	const tstring& GetAccount(void) const {return m_sAccount;}
	void SetDbPwd(const tstring& sDbPwd) {m_sDbPwd = sDbPwd;}
	const tstring& GetDbPwd(void) const {return m_sDbPwd;}
	void SetPwdSalt(const tstring& sPwdSalt) {m_sPwdSalt = sPwdSalt;}
	const tstring& GetPwdSalt(void) const {return m_sPwdSalt;}
	void SetSymmetricCipher(const tstring& v) {m_sSymmetricCipher = v;}
	const tstring& GetSymmetricCipher(void) const {return m_sSymmetricCipher;}

	void SetRegCode(int nRegCode) {m_nRegCode = nRegCode;}
	int GetRegCode(void) const {return m_nRegCode;}
	//void SetAccountState(int nAccountState) {m_nAccountState = nAccountState;}
	//int GetAccountState(void) const {return m_nAccountState;}

	void SetSmtpServer(const tstring& v) {m_sSmtpServer = v;}
	const tstring& GetSmtpServer(void) const {return m_sSmtpServer;}
	void SetSmtpPort(int nPort) {m_nSmtpPort = nPort;}
	int GetSmtpPort(void) const {return m_nSmtpPort;}
	void SetSmtpSsl(bool bSsl) {m_bSmtpSsl = bSsl;}
	bool GetSmtpSsl(void) const {return m_bSmtpSsl;}
	void SetPop3Server(const tstring& v) {m_sPop3Server = v;}
	const tstring& GetPop3Server(void) const {return m_sPop3Server;}
	//void SetPop3Port(int nPort) {m_nPop3Port = nPort;}
	//int GetPop3Port(void) const {return m_nPop3Port;}
	//void SetPop3Ssl(bool bSsl) {m_bPop3Ssl = bSsl;}
	//bool GetPop3Ssl(void) const {return m_bPop3Ssl;}

	CEBReqInfo(mycp::bigint nId,mycp::bigint nUserId,mycp::bigint nFromIp,const tstring& sReqKey)
		: m_nId(nId), m_nUserId(nUserId), m_nFromIp(nFromIp),m_sReqKey(sReqKey)
		, m_nRegCode(0)//,m_nAccountState(0)
		, m_nSmtpPort(0), m_bSmtpSsl(false)//, m_nPop3Port(0), m_bPop32Ssl(false)
	{
		m_tReqTime = time(0);
	}
private:
	mycp::bigint m_nId;
	mycp::bigint m_nUserId;
	tstring m_sAccount;
	mycp::bigint m_nFromIp;
	tstring m_sReqKey;
	tstring m_sDbPwd;
	tstring m_sPwdSalt;
	tstring m_sSymmetricCipher;
	int m_nRegCode;
	//int m_nAccountState;
	time_t m_tReqTime;
	tstring m_sSmtpServer;
	int m_nSmtpPort;
	bool m_bSmtpSsl;
	tstring m_sPop3Server;
	//int m_nPop3Port;
	//bool m_bPop32Ssl;
};

class CEBAPMsgInfo
	: public EB_APMsgInfo
	, public cgcObject
{
public:
	typedef boost::shared_ptr<CEBAPMsgInfo> pointer;
	static CEBAPMsgInfo::pointer create(void)
	{
		return CEBAPMsgInfo::pointer(new CEBAPMsgInfo());
	}
	static CEBAPMsgInfo::pointer create(const CEBAPMsgInfo * p)
	{
		return CEBAPMsgInfo::pointer(new CEBAPMsgInfo(p));
	}
	CEBAPMsgInfo(void)
		: EB_APMsgInfo()
		, m_nErrorCount(0)
	{
	}
	CEBAPMsgInfo(const CEBAPMsgInfo * p)
		: EB_APMsgInfo()
		, m_nErrorCount(0)
	{
		this->operator = (p);
	}
	const CEBAPMsgInfo& operator =(const CEBAPMsgInfo& p) {return this->operator = (&p);}
	const CEBAPMsgInfo& operator =(const CEBAPMsgInfo::pointer p) {return this->operator = (p.get());}
	const CEBAPMsgInfo& operator =(const CEBAPMsgInfo* p)
	{
		EB_APMsgInfo::operator =(p);
		if (p!=NULL)
		{
			m_nErrorCount = p->m_nErrorCount;
		}
		return *this;
	}
	int m_nErrorCount;
};

// 应用导航信息
class CEBNavigationInfo
{
public:
	typedef boost::shared_ptr<CEBNavigationInfo> pointer;
	static CEBNavigationInfo::pointer create(mycp::bigint nFuncId,mycp::bigint nNavId)
	{
		return CEBNavigationInfo::pointer(new CEBNavigationInfo(nFuncId,nNavId));
	}
	CEBNavigationInfo(mycp::bigint nFuncId, mycp::bigint nNavId)
		: m_nFuncId(nFuncId)
		, m_nNavId(nNavId)
		, m_nParentNavId(0)
		, m_nIndex(0)
		, m_nType(0)
	{
	}
	CEBNavigationInfo(void)
		: m_nFuncId(0)
		, m_nNavId(0)
		, m_nParentNavId(0)
		, m_nIndex(0)
		, m_nType(0)
	{
	}

public:
	mycp::bigint m_nFuncId;
	mycp::bigint m_nNavId;
	mycp::bigint m_nParentNavId;
	tstring m_sName;
	tstring m_sUrl;
	int m_nIndex;
	int m_nType;
	tstring m_sDescription;
	tstring m_sCreateTime;
};

// 应用功能信息
class CEBFuncInfo
{
public:
	typedef boost::shared_ptr<CEBFuncInfo> pointer;
	static CEBFuncInfo::pointer create(mycp::bigint nFuncId)
	{
		return CEBFuncInfo::pointer(new CEBFuncInfo(nFuncId));
	}
	CEBFuncInfo(mycp::bigint nFuncId)
		: m_nFuncId(nFuncId)
		, m_nAppId(0)
		, m_nOffset(0)
		, m_nLocation(0)
		, m_nFuncMode(0), m_nFuncParam(1)
		, m_nFuncType(0)
		, m_nWidth(0)
		, m_nHeight(0),m_nFuncExt(1)
		, m_nSubId(0),/*m_nSubEntId(0),*/m_nSubLevel(0),m_nDisplayIndex(0)
	{
	}
	CEBFuncInfo(void)
		: m_nFuncId(0)
		, m_nAppId(0)
		, m_nOffset(0)
		, m_nLocation(0)
		, m_nFuncMode(0),m_nFuncParam(1)
		, m_nFuncType(0)
		, m_nWidth(0)
		, m_nHeight(0),m_nFuncExt(1)
		, m_nSubId(0),/*m_nSubEntId(0),*/m_nSubLevel(0),m_nDisplayIndex(0)
	{
	}

public:
	mycp::bigint m_nFuncId;
	mycp::bigint m_nAppId;
	int m_nOffset;
	tstring m_sDescription;
	tstring m_sCreateTime;
	int m_nLocation;
	tstring m_sFuncName;
	int m_nFuncMode;
	int m_nFuncParam;
	tstring m_sFuncUrl;
	int m_nFuncType;
	int m_nWidth;
	int m_nHeight;
	int m_nFuncExt;
	// sub
	mycp::bigint m_nSubId;
	//mycp::bigint m_nSubEntId;
	int m_nSubLevel;
	tstring m_sStartTime;
	tstring m_sEndTime;
	int m_nDisplayIndex;
	tstring m_sSubDesc;
};

// 订购功能信息
class CEBSubscribeFuncInfo: public EB_SubscribeFuncInfo
{
public:
	typedef boost::shared_ptr<CEBSubscribeFuncInfo> pointer;
	static CEBSubscribeFuncInfo::pointer create(eb::bigint nSubscribeId)
	{
		return CEBSubscribeFuncInfo::pointer(new CEBSubscribeFuncInfo(nSubscribeId));
	}
	CEBSubscribeFuncInfo(void)
		: EB_SubscribeFuncInfo()
		, m_nFunctionParam(EB_FUNC_PARAM_NONE)
	{
	}
	CEBSubscribeFuncInfo(eb::bigint nSubscribeId)
		: EB_SubscribeFuncInfo(nSubscribeId)
		, m_nFunctionParam(EB_FUNC_PARAM_NONE)
	{
	}
	EB_FUNC_PARAM m_nFunctionParam;
	//tstring m_sHttpUrl;	// for ebwebum
};


class CEBEmotionInfo : public EB_EmotionInfo
{
public:
	typedef boost::shared_ptr<CEBEmotionInfo> pointer;
	static CEBEmotionInfo::pointer create(eb::bigint sResId,int nIndex,int nType,eb::bigint sAccount,eb::bigint sEntCode,EB_RESOURCE_TYPE nResType)
	{
		return CEBEmotionInfo::pointer(new CEBEmotionInfo(sResId,nIndex,nType,sAccount,sEntCode,nResType));
	}
	CEBEmotionInfo(void)
		: EB_EmotionInfo()
		, m_nCmServiceId(0), m_nEmoId(0)
	{
	}
	CEBEmotionInfo(eb::bigint sResId,int nIndex,int nType,eb::bigint sAccount,eb::bigint sEntCode,EB_RESOURCE_TYPE nResType)
		: EB_EmotionInfo(sResId,nIndex,nType,sAccount,sEntCode,nResType)
		, m_nCmServiceId(0), m_nEmoId(0)
	{
	}
	int m_nCmServiceId;
	mycp::bigint m_nEmoId;
	tstring m_sResMd5;
	tstring m_sCmServer;
	tstring m_sCmHttpServer;
	tstring m_sCmAppName;
};
const CEBEmotionInfo::pointer NullEBEmotionInfo;

/*==========================================================
系统信息
===========================================================*/
class EB_AppMsgInfo
{
public:
	mycp::bigint GetMsgId(void) const {return m_nMsgId;}
	EB_MSG_TYPE GetMsgType(void) const {return m_nMsgType;}
	void SetMsgSubType(int v) {m_nMsgSubType = v;}
	int GetMsgSubType(void) const {return m_nMsgSubType;}
	void SetMsgName(const tstring& v) {m_sMsgName = v;}
	const tstring GetMsgName(void) const {return m_sMsgName;}
	void SetMsgContent(const tstring& v) {m_sMsgContent = v;}
	const tstring& GetMsgContent(void) const {return m_sMsgContent;}

	EB_AppMsgInfo(mycp::bigint nMsgId,EB_MSG_TYPE nMsgType, const tstring& sMsgContent)
		: m_sGroupCode(0), m_nMsgId(nMsgId), m_nMsgType(nMsgType),m_nMsgSubType(0), m_sMsgContent(sMsgContent)
		, m_nFromUserId(0)
		, m_bOffSave(false)
	{
	}
	EB_AppMsgInfo(void)
		: m_sGroupCode(0),m_nMsgId(0), m_nMsgType(EB_MSG_UNKNOWN),m_nMsgSubType(0)//, m_sMsgContent(sMsgContent)
		, m_nFromUserId(0)
		, m_bOffSave(false)
	{
	}
	mycp::bigint m_sGroupCode;
	tstring m_sFromInfo;
	tstring m_sFromAccount;
	eb::bigint m_nFromUserId;
	bool m_bOffSave;
	tstring m_sMsgTime;
private:
	mycp::bigint m_nMsgId;
	EB_MSG_TYPE m_nMsgType;
	int m_nMsgSubType;
	tstring m_sMsgName;
	tstring m_sMsgContent;
};

/*==========================================================
资源扩展数据
===========================================================*/
typedef enum EB_RESOURCE_DATA
{
	EB_RESOURCE_DATA_REFRESH				= 0x01	// * 重新刷新
}EB_RESOURCE_DATA;

/*==========================================================
在线资源信息
===========================================================*/
class CEBResourceInfo : public EB_ResourceInfo
{
public:
	typedef boost::shared_ptr<CEBResourceInfo> pointer;
	static CEBResourceInfo::pointer create(void)
	{
		return CEBResourceInfo::pointer(new CEBResourceInfo());
	}
	CEBResourceInfo(void)
		: EB_ResourceInfo()
		, m_nCmServiceId(0)
		, m_nIndex(0)
		, m_nEMailContentFlag(0), m_nEMailFromUserId(0)
		, m_nItemData(0)
	{
	}
	tstring m_sCreateUserName;
	int m_nCmServiceId;
	tstring m_sCmServer;
	//tstring m_sCmHttpServer;
	tstring m_sCmAppName;
	int m_nIndex;

	// email for nFromType==EB_RESOURCE_FROM_TYPE_MCON_ID_ATT
	int m_nEMailContentFlag;
	mycp::bigint m_nEMailFromUserId;
	tstring m_sEMailFromAccount;
	tstring m_sEMailFromName;
	tstring m_sEMailSubject;
	unsigned int m_nItemData;	// 自定义数据
};

/*==========================================================
OAUTH开放验证信息
===========================================================*/
class CEBOAuthInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CEBOAuthInfo> pointer;
	static CEBOAuthInfo::pointer create(const tstring & sRequestCode)
	{
		return CEBOAuthInfo::pointer(new CEBOAuthInfo(sRequestCode));
	}
	static CEBOAuthInfo::pointer create(void)
	{
		return CEBOAuthInfo::pointer(new CEBOAuthInfo());
	}

	const tstring& GetRequestCode(void) const {return m_sRequestCode;}
	void SetSotpSessionId(const tstring& v) {m_sSotpSessionId = v;}
	const tstring& GetSotpSessionId(void) const {return m_sSotpSessionId;}
	void SetRequestTime(const time_t v) {m_tRequestTime = v;}
	const time_t GetRequestTime(void) const {return m_tRequestTime;}
	void SetOAuthKey(const tstring& v) {m_sOAuthKey = v;}
	const tstring& GetOAuthKey(void) const {return m_sOAuthKey;}

	CEBOAuthInfo(void)
		: m_tRequestTime(0)
	{
	}
	CEBOAuthInfo(const tstring & sRequestCode)
		: m_sRequestCode(sRequestCode)
	{
		m_tRequestTime = time(0);
	}
	const CEBOAuthInfo& operator = (const CEBOAuthInfo& p) {return this->operator = (&p);}
	const CEBOAuthInfo& operator = (const CEBOAuthInfo::pointer& p) {return this->operator = (p.get());}
	const CEBOAuthInfo& operator = (const CEBOAuthInfo* p)
	{
		if (p!=NULL)
		{
			m_sRequestCode = p->m_sRequestCode;
			m_sSotpSessionId = p->m_sSotpSessionId;
			m_sOAuthKey = p->m_sOAuthKey;
			m_tRequestTime = p->m_tRequestTime;
		}
		return *this;
	}
	virtual cgcObject::pointer copyNew(void) const
	{
		CEBOAuthInfo::pointer result = CEBOAuthInfo::create(m_sRequestCode);
		result->SetSotpSessionId(m_sSotpSessionId);
		result->SetOAuthKey(m_sOAuthKey);
		result->SetRequestTime(m_tRequestTime);
		return result;
	}

private:
	tstring m_sRequestCode;
	tstring m_sSotpSessionId;
	tstring m_sOAuthKey;
	time_t m_tRequestTime;
};


/*==========================================================
用户分组信息
===========================================================*/
class CEBUGInfo : public EB_UGInfo
{
public:
	typedef boost::shared_ptr<CEBUGInfo> pointer;
	static CEBUGInfo::pointer create(void)
	{
		return CEBUGInfo::pointer(new CEBUGInfo());
	}
	static CEBUGInfo::pointer create(eb::bigint nUGId,const CEBString& sGroupName)
	{
		return CEBUGInfo::pointer(new CEBUGInfo(nUGId,sGroupName));
	}
	static CEBUGInfo::pointer create(const EB_UGInfo* pUGInfo)
	{
		return CEBUGInfo::pointer(new CEBUGInfo(pUGInfo));
	}
	const CEBUGInfo& operator = (const EB_UGInfo& pUGInfo)
	{
		EB_UGInfo::operator =(pUGInfo);
		return *this;
	}
	const CEBUGInfo& operator = (const EB_UGInfo* pUGInfo)
	{
		EB_UGInfo::operator =(pUGInfo);
		return *this;
	}

	CEBUGInfo(void)
		: EB_UGInfo()
	{}
	CEBUGInfo(eb::bigint nUGId,const CEBString& sGroupName)
		: EB_UGInfo(nUGId,sGroupName)
	{}
	CEBUGInfo(const EB_UGInfo* pUGInfo)
		: EB_UGInfo(pUGInfo)
	{}
};
const CEBUGInfo::pointer EB_Null_UGInfo;

/*==========================================================
通讯录（联系人）信息
===========================================================*/
class CEBContactInfo : public EB_ContactInfo
{
public:
	typedef boost::shared_ptr<CEBContactInfo> pointer;
	static CEBContactInfo::pointer create(void)
	{
		return CEBContactInfo::pointer(new CEBContactInfo());
	}
	static CEBContactInfo::pointer create(eb::bigint nContactId)
	{
		return CEBContactInfo::pointer(new CEBContactInfo(nContactId));
	}
	//static CEBContactInfo::pointer create(const tstring& sContact)
	//{
	//	return CEBContactInfo::pointer(new CEBContactInfo(sContact));
	//}
	static CEBContactInfo::pointer create(const EB_ContactInfo* pContactInfo)
	{
		return CEBContactInfo::pointer(new CEBContactInfo(pContactInfo));
	}
	const CEBContactInfo& operator = (const EB_ContactInfo& pContactInfo)
	{
		EB_ContactInfo::operator =(pContactInfo);
		return *this;
	}
	const CEBContactInfo& operator = (const EB_ContactInfo* pContactInfo)
	{
		EB_ContactInfo::operator =(pContactInfo);
		return *this;
	}

	CEBContactInfo(void)
		: EB_ContactInfo()
		, m_nHeadVer(0)
	{}
	CEBContactInfo(eb::bigint nContactId)
		: EB_ContactInfo(nContactId)
		, m_nHeadVer(0)
	{}
	//CEBContactInfo(const tstring& sContact)
	//	: EB_ContactInfo(sContact)
	//{}
	CEBContactInfo(const EB_ContactInfo* pContactInfo)
		: EB_ContactInfo(pContactInfo)
		, m_nHeadVer(0)
	{}

	tstring m_sHeadCmServer;
	tstring m_sHeadHttpServer;
	mycp::bigint m_nHeadVer;
};
const CEBContactInfo::pointer EB_Null_ContactInfo;

/*==========================================================
电子名片
===========================================================*/
class CEBECardInfo : public EB_ECardInfo
{
public:
	typedef boost::shared_ptr<CEBECardInfo> pointer;
	static CEBECardInfo::pointer create(void)
	{
		return CEBECardInfo::pointer(new CEBECardInfo());
	}
	CEBECardInfo(void)
		: EB_ECardInfo()
	{}
};
const CEBECardInfo::pointer EB_Null_ECardInfo;

typedef enum LOGON_TYPE_EXT_DATA
{
	LOGON_TYPE_EXT_DATA_LOAD_APP_SUB_INFO		= 0x1
	, LOGON_TYPE_EXT_DATA_LOAD_CALL_LIST_INFO	= 0x2
	, LOGON_TYPE_EXT_DATA_IS_LOCAL_IP			= 0x4
	, LOGON_TYPE_EXT_DATA_LOAD_SYS_MSG			= 0x8
	, LOGON_TYPE_EXT_DATA_ALREADY_ONLINE		= 0x10
	//, LOGON_TYPE_EXT_DATA_ALREADY_OFFLINE		= 0x20
	, LOGON_TYPE_EXT_DATA_CHANGE_PWD_OFFLINE	= 0x40
}LOGON_TYPE_EXT_DATA;
class CEBLogonTypeInfo
{
public:
	typedef boost::shared_ptr<CEBLogonTypeInfo> pointer;
	static CEBLogonTypeInfo::pointer create(int nLogonType,const tstring& sSotpSessionId,const tstring& sEbSid="")
	{
		return CEBLogonTypeInfo::pointer(new CEBLogonTypeInfo(nLogonType,sSotpSessionId,sEbSid));
	}
	static CEBLogonTypeInfo::pointer create(void)
	{
		return CEBLogonTypeInfo::pointer(new CEBLogonTypeInfo());
	}
	const CEBLogonTypeInfo& operator = (const CEBLogonTypeInfo* p)
	{
		if (p!=NULL)
		{
			m_nLogonType = p->m_nLogonType;
			m_sSotpSessionId = p->m_sSotpSessionId;
			m_sEbSid = p->m_sEbSid;
			m_sOnlineKey = p->m_sOnlineKey;
			m_sAcmKey = p->m_sAcmKey;
			m_bLoadAppSubInfo = p->m_bLoadAppSubInfo;
			m_nExtData = p->m_nExtData;
			m_sRemoteAddress = p->m_sRemoteAddress;
			m_nFromIpAddress = p->m_nFromIpAddress;
			m_nHttpIpAddress1 = p->m_nHttpIpAddress1;
			m_nHttpIpAddress2 = p->m_nHttpIpAddress2;
			m_nUserSignId = p->m_nUserSignId;
			m_tCreateTime = p->GetCreateTime();
		}
		return *this;
	}
	const CEBLogonTypeInfo& operator = (const CEBLogonTypeInfo::pointer& p) {return this->operator = (p.get());}
	const CEBLogonTypeInfo& operator = (const CEBLogonTypeInfo& p) {return this->operator = (&p);}

	int m_nLogonType;
	tstring m_sSotpSessionId;
	tstring m_sEbSid;	// for EB_LOGON_TYPE_SERVER
	tstring m_sOnlineKey;
	tstring m_sAcmKey;
	bool m_bLoadAppSubInfo;
	unsigned int m_nExtData;
	tstring m_sRemoteAddress;
	unsigned long m_nFromIpAddress;
	unsigned long m_nHttpIpAddress1;
	unsigned long m_nHttpIpAddress2;
	mycp::bigint m_nUserSignId;
	//mycp::bigint m_nSSLId;
	//tstring m_sDeviceToken;
	void SetCreateTime(time_t t) {m_tCreateTime = t;}
	time_t	GetCreateTime(void) const {return m_tCreateTime;}

	bool IsLogonEmail(void) const {return (m_nLogonType&EB_LOGON_TYPE_EMAIL)==EB_LOGON_TYPE_EMAIL;}
	bool IsLogonPhone(void) const {return (m_nLogonType&EB_LOGON_TYPE_PHONE)==EB_LOGON_TYPE_PHONE;}
	bool IsLogonVisitor(void) const {return (m_nLogonType&EB_LOGON_TYPE_VISITOR)==EB_LOGON_TYPE_VISITOR;}
	bool IsLogonWeb(void) const {return (m_nLogonType&EB_LOGON_TYPE_WEB)==EB_LOGON_TYPE_WEB;}

	CEBLogonTypeInfo(int nLogonType,const tstring& sSotpSessionId,const tstring& sEbSid="")
		: m_nLogonType(nLogonType)
		, m_sSotpSessionId(sSotpSessionId)
		, m_sEbSid(sEbSid)
		, m_bLoadAppSubInfo(false)
		, m_nExtData(0)
		, m_nFromIpAddress(0),m_nHttpIpAddress1(0),m_nHttpIpAddress2(0),m_nUserSignId(0)//,m_nSSLId(0)
	{
		m_tCreateTime = time(0);
	}
	CEBLogonTypeInfo(void)
		: m_nLogonType(EB_LOGON_TYPE_UNKNOWN)
		, m_bLoadAppSubInfo(false)
		, m_nExtData(0)
		, m_nFromIpAddress(0),m_nHttpIpAddress1(0),m_nHttpIpAddress2(0),m_nUserSignId(0)//,m_nSSLId(0)
		, m_tCreateTime(0)
	{
	}
private:
	time_t	m_tCreateTime;
};
const CEBLogonTypeInfo::pointer NullEBLogonTypeInfo;

class CEBLogonTypeList
{
public:
	typedef boost::shared_ptr<CEBLogonTypeList> pointer;
	static CEBLogonTypeList::pointer create(void)
	{
		return CEBLogonTypeList::pointer(new CEBLogonTypeList());
	}
	static CEBLogonTypeList::pointer create(const CEBLogonTypeList*p)
	{
		return CEBLogonTypeList::pointer(new CEBLogonTypeList(p));
	}
	static CEBLogonTypeList::pointer create(const CEBLogonTypeList::pointer& p)
	{
		return CEBLogonTypeList::pointer(new CEBLogonTypeList(p.get()));
	}

	void SetACMKey(const tstring& sSotpSessionId, const tstring& newValue) {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		if (pLogonTypeInfo.get()!=NULL)
			pLogonTypeInfo->m_sAcmKey = newValue;
	}
	tstring GetACMKey(const tstring& sSotpSessionId) const {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?"":pLogonTypeInfo->m_sAcmKey;
	}
	void SetRemoteAddress(const tstring& sSotpSessionId, const tstring& newValue ) {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		if (pLogonTypeInfo.get()!=NULL)
			pLogonTypeInfo->m_sRemoteAddress = newValue;
	}
	tstring GetRemoteAddress(const tstring& sSotpSessionId) const {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?"":pLogonTypeInfo->m_sRemoteAddress;
	}
	void SetFromIpAddress(const tstring& sSotpSessionId, unsigned long newValue) {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		if (pLogonTypeInfo.get()!=NULL)
			pLogonTypeInfo->m_nFromIpAddress = newValue;
	}
	unsigned long GetFromIpAddress(const tstring& sSotpSessionId) const {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?0:pLogonTypeInfo->m_nFromIpAddress;
	}
	void SetUserSignId(const tstring& sSotpSessionId, mycp::bigint newValue) {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		if (pLogonTypeInfo.get()!=NULL)
			pLogonTypeInfo->m_nUserSignId = newValue;
	}
	mycp::bigint GetUserSignId(const tstring& sSotpSessionId) const {
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?0:pLogonTypeInfo->m_nUserSignId;
	}

	// 设置加载应用功能订购信息，已经设置返回false,未设置返回true
	bool SetLoadAppSubInfo(const tstring& sSotpSessionId)
	{
		BoostReadLock lock(m_pList.mutex());
		CLockList<CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		//CLockMap<int,CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			//const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
			if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			{
				if (!pLogonTypeInfo->m_bLoadAppSubInfo)
				{
					pLogonTypeInfo->m_bLoadAppSubInfo = true;
					return true;
				}
				break;
			}
		}
		return false;
	}
	unsigned int GetLogonTypeExtData(const tstring& sSotpSessionId) const
	{
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?0:pLogonTypeInfo->m_nExtData;
		//AUTO_CONST_RLOCK(m_pList);
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//for (; pIter!=m_pList.end(); pIter++)
		//{
		//	const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
		//	if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
		//	{
		//		return pLogonTypeInfo->m_nExtData;
		//	}
		//}
		//return 0;
	}
	CEBLogonTypeInfo::pointer GetLogonTypeInfo(const tstring& sSotpSessionId) const
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			//const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
			if (sSotpSessionId.empty() || pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	CEBLogonTypeInfo::pointer GetLogonTypeInfoByEbSid(const tstring& sEbSid, bool bGetEmptyFirst=false) const
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if ((bGetEmptyFirst && sEbSid.empty()) || pLogonTypeInfo->m_sEbSid == sEbSid)
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	CEBLogonTypeInfo::pointer GetLogonTypeInfoBySotpSidOrEbSid(const tstring& sSotpSessionId,const tstring& sEbSid) const
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId || pLogonTypeInfo->m_sEbSid == sEbSid)
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	CEBLogonTypeInfo::pointer GetLogonTypeInfo(const tstring& sSotpSessionId, const tstring& sEbSid) const
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (sSotpSessionId.empty() || (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId && pLogonTypeInfo->m_sEbSid==sEbSid))
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	CEBLogonTypeInfo::pointer GetLogonTypeInfo2(const tstring& sOnlineKey) const
	{
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			//const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
			if (pLogonTypeInfo->m_sOnlineKey == sOnlineKey)
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	CEBLogonTypeInfo::pointer GetLogonTypeInfo(int nLogonType) const
	{
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			//const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
			if (pLogonTypeInfo->m_nLogonType==nLogonType)
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	// *sSotpSessionId empty to get first
	CEBLogonTypeInfo::pointer GetLogonTypeInfoBySotpSessionId(int nLogonType, const tstring& sSotpSessionId) const
	{
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (pLogonTypeInfo->m_nLogonType==nLogonType && (sSotpSessionId.empty() || pLogonTypeInfo->m_sSotpSessionId==sSotpSessionId))
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	// *sEbSid empty to get first
	CEBLogonTypeInfo::pointer GetLogonTypeInfoByEbSid(int nLogonType, const tstring& sEbSid, bool bGetEmptyFirst=false) const
	{
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (pLogonTypeInfo->m_nLogonType==nLogonType && ((bGetEmptyFirst && sEbSid.empty()) || pLogonTypeInfo->m_sEbSid==sEbSid))
			{
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	CEBLogonTypeInfo::pointer SetOnlineKey(const tstring& sSotpSessionId,const tstring & sOnlineKey)
	{
		CEBLogonTypeInfo::pointer pResultLogonTypeInfo;
		AUTO_CONST_RLOCK(m_pList);
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (sSotpSessionId.empty() || pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			{
				pLogonTypeInfo->m_sOnlineKey = sOnlineKey;
				if (pResultLogonTypeInfo.get()==NULL || (pLogonTypeInfo->m_nLogonType&EB_LOGON_TYPE_SERVER)==0)
					pResultLogonTypeInfo = pLogonTypeInfo;
			}
		}
		return pResultLogonTypeInfo;
		//CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		//if (pLogonTypeInfo.get()!=NULL)
		//	pLogonTypeInfo->m_sOnlineKey = sOnlineKey;
		//return pLogonTypeInfo;
	}
	tstring GetOnlineKey(const tstring& sSotpSessionId) const
	{
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?"":pLogonTypeInfo->m_sOnlineKey;
	}

	// 返回登录类型
	int SetSotpSessionId(int nLogonType,const tstring & sSotpSessionId, const tstring& sEbSid) {
		//if (nLogonType==-1)
		//	nLogonType = nDefaultLogonType;
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(nLogonType);
		if (pLogonTypeInfo.get()==NULL)
		{
			pLogonTypeInfo = CEBLogonTypeInfo::create(nLogonType,sSotpSessionId,sEbSid);
			m_pList.add(pLogonTypeInfo);
		}else
		{
			 if (pLogonTypeInfo->m_sSotpSessionId != sSotpSessionId)
				 pLogonTypeInfo->m_sSotpSessionId = sSotpSessionId;
			 if (pLogonTypeInfo->m_sEbSid != sEbSid)
				 pLogonTypeInfo->m_sEbSid = sEbSid;
		}
		//CEBLogonTypeInfo::pointer pLogonTypeInfo;
		//if (!m_pList.find(nLogonType,pLogonTypeInfo))
		//{
		//	pLogonTypeInfo = CEBLogonTypeInfo::create(nLogonType,sSotpSessionId);
		//	m_pList.insert(nLogonType,pLogonTypeInfo);
		//}else if (pLogonTypeInfo->m_sSotpSessionId != sSotpSessionId)
		//{
		//	pLogonTypeInfo->m_sSotpSessionId = sSotpSessionId;
		//}
		return nLogonType;
	}
	void SetSotpSessionId2(int nLogonType,const tstring & sSotpSessionId,const tstring& sEbSid) {
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		{
			AUTO_CONST_RLOCK(m_pList);
			CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
			for (; pIter!=m_pList.end(); pIter++)
			{
				const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
				if (pLogonTypeInfo->m_nLogonType==nLogonType && pLogonTypeInfo->m_sSotpSessionId==sSotpSessionId)
				//if (pLogonTypeInfo->m_nLogonType==nLogonType && pLogonTypeInfo->m_sSotpSessionId==sSotpSessionId && pLogonTypeInfo->m_sEbSid.empty())
				{
					if (pLogonTypeInfo->m_sEbSid!=sEbSid)
						pLogonTypeInfo->m_sEbSid = sEbSid;
					return;
				}
				else if (pLogonTypeInfo->m_nLogonType==nLogonType && pLogonTypeInfo->m_sEbSid==sEbSid)
				{
					if (pLogonTypeInfo->m_sSotpSessionId!=sSotpSessionId)
						pLogonTypeInfo->m_sSotpSessionId = sSotpSessionId;
					return;
				}

			}
		}
		m_pList.add(CEBLogonTypeInfo::create(nLogonType,sSotpSessionId,sEbSid));
	}
	void SetSotpSessionId3(int nLogonType,const tstring & sSotpSessionId, const tstring& sEbSid) {
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		{
			AUTO_CONST_RLOCK(m_pList);
			CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
			for (; pIter!=m_pList.end(); pIter++)
			{
				const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
				if (pLogonTypeInfo->m_nLogonType==nLogonType && pLogonTypeInfo->m_sSotpSessionId==sSotpSessionId && pLogonTypeInfo->m_sEbSid==sEbSid)
				{
					return;
				}
			}
		}
		m_pList.add(CEBLogonTypeInfo::create(nLogonType,sSotpSessionId,sEbSid));
	}
	tstring GetSotpSessionId(int nLogonType) const {
		//if (nLogonType==-1)
		//	nLogonType = nDefaultLogonType;
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(nLogonType);
		if (pLogonTypeInfo.get()!=NULL)
			return pLogonTypeInfo->m_sSotpSessionId;
		//CEBLogonTypeInfo::pointer pLogonTypeInfo;
		//if (m_pList.find(nLogonType,pLogonTypeInfo))
		//	return pLogonTypeInfo->m_sSotpSessionId;
		return "";
	}
	tstring GetEbSid(int nLogonType) const {
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(nLogonType);
		if (pLogonTypeInfo.get()!=NULL)
			return pLogonTypeInfo->m_sEbSid;
		return "";
	}
	tstring GetEbSid(int nLogonType, const tstring& sSotpSessionId) const {
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfoBySotpSessionId(nLogonType,sSotpSessionId);
		if (pLogonTypeInfo.get()!=NULL)
			return pLogonTypeInfo->m_sEbSid;
		return "";
	}
	bool ExistLogonType(int nLogonType) const {
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(nLogonType);
		return pLogonTypeInfo.get()!=NULL?true:false;
	}
	bool ExistLogonTypeBySotpSessionId(int nLogonType,const tstring& sSotpSessionId) const {
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfoBySotpSessionId(nLogonType,sSotpSessionId);
		return pLogonTypeInfo.get()!=NULL?true:false;
	}
	bool ExistLogonTypeByEbSid(int nLogonType,const tstring& sEbSid, bool bGetEmptyFirst=false) const {
		nLogonType = nLogonType&EB_SID_LOGON_TYPE;
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfoByEbSid(nLogonType,sEbSid,bGetEmptyFirst);
		return pLogonTypeInfo.get()!=NULL?true:false;
	}
	// 检查是否还有IM业务登录
	tstring GetFirstImSotpSessionId(int & pOutLogonType) const
	{
		BoostReadLock lock(const_cast<boost::shared_mutex&>(m_pList.mutex()));
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if ((pLogonTypeInfo->m_nLogonType&EB_LOGON_TYPE_SERVER)!=EB_LOGON_TYPE_SERVER)
			{
				pOutLogonType = pLogonTypeInfo->m_nLogonType;
				return pLogonTypeInfo->m_sSotpSessionId;
			}
		}
		//BoostReadLock lock(const_cast<boost::shared_mutex&>(m_pList.mutex()));
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//for (; pIter!=m_pList.end(); pIter++)
		//{
		//	const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
		//	pOutLogonType = pIter->first;
		//	return pLogonTypeInfo->m_sSotpSessionId;
		//}
		return "";
	}

	tstring GetFirstSotpSessionId(int& pOutLogonType) const
	{
		BoostReadLock lock(const_cast<boost::shared_mutex&>(m_pList.mutex()));
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			pOutLogonType = pLogonTypeInfo->m_nLogonType;
			return pLogonTypeInfo->m_sSotpSessionId;
		}
		//BoostReadLock lock(const_cast<boost::shared_mutex&>(m_pList.mutex()));
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//for (; pIter!=m_pList.end(); pIter++)
		//{
		//	const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
		//	pOutLogonType = pIter->first;
		//	return pLogonTypeInfo->m_sSotpSessionId;
		//}
		return "";
	}

	// 返回登录类型
	int GetOnlineKeyLogonType(const tstring& sOnlineKey) const
	{
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo2(sOnlineKey);
		return pLogonTypeInfo.get()==NULL?EB_LOGON_TYPE_UNKNOWN:pLogonTypeInfo->m_nLogonType;
	}
	// 返回登录类型
	int GetSotpSessionIdLogonType(const tstring& sSotpSessionId) const
	{
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?EB_LOGON_TYPE_UNKNOWN:pLogonTypeInfo->m_nLogonType;
	}
	int GetEbSidLogonType(const tstring& sEbSid, bool bGetEmptyFirst=false) const
	{
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfoByEbSid(sEbSid,bGetEmptyFirst);
		return pLogonTypeInfo.get()==NULL?EB_LOGON_TYPE_UNKNOWN:pLogonTypeInfo->m_nLogonType;
	}
	int GetSotpSessionIdLogonType(const tstring& sSotpSessionId, const tstring& sEbSid) const
	{
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId,sEbSid);
		return pLogonTypeInfo.get()==NULL?EB_LOGON_TYPE_UNKNOWN:pLogonTypeInfo->m_nLogonType;
	}
	bool ExistSotpSessionId(const tstring& sSotpSessionId) const
	{
		CEBLogonTypeInfo::pointer pLogonTypeInfo = GetLogonTypeInfo(sSotpSessionId);
		return pLogonTypeInfo.get()==NULL?false:true;
	}
	// 检查是否还有IM业务登录
	bool HasImLogonType(void) const
	{
		BoostReadLock lock(const_cast<boost::shared_mutex&>(m_pList.mutex()));
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if ((pLogonTypeInfo->m_nLogonType&EB_LOGON_TYPE_SERVER)!=EB_LOGON_TYPE_SERVER)
				return true;
			//const int nLogonType = pIter->first;
			//if ((nLogonType&EB_LOGON_TYPE_SERVER)!=EB_LOGON_TYPE_SERVER)
			//{
			//	return true;
			//}
		}
		return false;
	}
	size_t ImLogonTypeSize(void) const
	{
		size_t result = 0;
		BoostReadLock lock(const_cast<boost::shared_mutex&>(m_pList.mutex()));
		CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		//CLockMap<int,CEBLogonTypeInfo::pointer>::const_iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if ((pLogonTypeInfo->m_nLogonType&EB_LOGON_TYPE_SERVER)!=EB_LOGON_TYPE_SERVER)
			{
				result++;
			}
			//const int nLogonType = pIter->first;
			//if ((nLogonType&EB_LOGON_TYPE_SERVER)!=EB_LOGON_TYPE_SERVER)
			//{
			//	result++;
			//}
		}
		return result;
	}
	// 返回登录类型
	int DelSotpSessionId(const tstring& sSotpSessionId)
	{
		BoostWriteLock lock(m_pList.mutex());
		CLockList<CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		//BoostReadLock lock(m_pList.mutex());
		//CLockMap<int,CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			{
				m_pList.erase(pIter);
				return pLogonTypeInfo->m_nLogonType;
			}
			//const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
			//if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			//{
			//	const int nLogonType = pIter->first;
			//	lock.unlock();
			//	m_pList.remove(nLogonType);
			//	//m_pList.erase(pIter);
			//	return nLogonType;
			//}
		}
		return EB_LOGON_TYPE_UNKNOWN;
	}
	CEBLogonTypeInfo::pointer DelSotpSessionId2(const tstring& sSotpSessionId)
	{
		BoostWriteLock lock(m_pList.mutex());
		CLockList<CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		//BoostReadLock lock(m_pList.mutex());
		//CLockMap<int,CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			{
				m_pList.erase(pIter);
				return pLogonTypeInfo;
			}
			//const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
			//if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			//{
			//	const int nLogonType = pIter->first;
			//	lock.unlock();
			//	m_pList.remove(nLogonType);
			//	return pLogonTypeInfo;
			//}
		}
		return NullEBLogonTypeInfo;
	}
	void DelAllSotpSessionId(const tstring& sSotpSessionId)
	{
		bool bContinue = true;
		while(bContinue)
		{
			bContinue = false;
			BoostWriteLock lock(m_pList.mutex());
			CLockList<CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
			for (; pIter!=m_pList.end(); pIter++)
			{
				const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
				if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
				{
					m_pList.erase(pIter);
					bContinue = true;
					break;
				}
			}
		}
	}
	CEBLogonTypeInfo::pointer DelSotpSessionId3(const tstring& sSotpSessionId,const tstring& sEbSid)
	{
		BoostWriteLock lock(m_pList.mutex());
		CLockList<CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId && pLogonTypeInfo->m_sEbSid==sEbSid)
			{
				m_pList.erase(pIter);
				return pLogonTypeInfo;
			}
		}
		return NullEBLogonTypeInfo;
	}
	void SetLastCallLogonType(EB_LOGON_TYPE v) {m_nLastCallLogonType = v;}
	void SetLastCallLogonType(const tstring& sSotpSessionId) {
		BoostReadLock rdlock(m_pList.mutex());
		CLockList<CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		//CLockMap<int,CEBLogonTypeInfo::pointer>::iterator pIter = m_pList.begin();
		for (; pIter!=m_pList.end(); pIter++)
		{
			const CEBLogonTypeInfo::pointer pLogonTypeInfo = *pIter;
			if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			{
				m_nLastCallLogonType = (EB_LOGON_TYPE)pLogonTypeInfo->m_nLogonType;
				break;
			}
			//int nLogonType = pIter->first;
			//const CEBLogonTypeInfo::pointer pLogonTypeInfo = pIter->second;
			//if (pLogonTypeInfo->m_sSotpSessionId == sSotpSessionId)
			//{
			//	m_nLastCallLogonType = (EB_LOGON_TYPE)nLogonType;
			//	break;
			//}
		}
		m_sLastCallSessionId = sSotpSessionId;
	}
	EB_LOGON_TYPE GetLastCallLogonType(void) const {return m_nLastCallLogonType;}
	const tstring& GetLastCallSessionId(void) const {return m_sLastCallSessionId;}
	size_t size(void) const {return m_pList.size();}
	bool empty(void) const {return m_pList.empty();}
	void clear(void) {m_pList.clear();}
	const CLockList<CEBLogonTypeInfo::pointer>& list(void) const {return m_pList;}

	CEBLogonTypeList(void)
		: m_nLastCallLogonType(EB_LOGON_TYPE_UNKNOWN)
	{}
	CEBLogonTypeList(const CEBLogonTypeList* p)
		: m_nLastCallLogonType(EB_LOGON_TYPE_UNKNOWN)
	{
		this->operator = (p);
	}
	virtual ~CEBLogonTypeList(void)
	{
		m_pList.clear(false);
	}
	const CEBLogonTypeList& operator = (const CEBLogonTypeList& p) {return this->operator = (&p);}
	const CEBLogonTypeList& operator = (const CEBLogonTypeList::pointer& p) {return this->operator = (p.get());}
	const CEBLogonTypeList& operator = (const CEBLogonTypeList* p)
	{
		if (p!=NULL)
		{
			m_nLastCallLogonType = p->GetLastCallLogonType();
			m_sLastCallSessionId= p->GetLastCallSessionId();
			//m_pList = p->list();
			const CLockList<CEBLogonTypeInfo::pointer>& pOldList = p->list();
			BoostReadLock lock(const_cast<boost::shared_mutex&>(pOldList.mutex()));
			CLockList<CEBLogonTypeInfo::pointer>::const_iterator pIter = pOldList.begin();
			for (; pIter!=pOldList.end(); pIter++)
			{
				const CEBLogonTypeInfo::pointer pOldLogonTypeInfo = *pIter;
				CEBLogonTypeInfo::pointer pNewLogonTypeInfo = CEBLogonTypeInfo::create();
				pNewLogonTypeInfo->operator = (pOldLogonTypeInfo);
				m_pList.add(pNewLogonTypeInfo,false);
			}
		}
		return *this;
	}
private:
	CLockList<CEBLogonTypeInfo::pointer> m_pList;	// logontype->
	//CLockMap<int,CEBLogonTypeInfo::pointer> m_pList;	// logontype->
	EB_LOGON_TYPE m_nLastCallLogonType;
	tstring m_sLastCallSessionId;
};

/*==========================================================
授权许可时间信息
===========================================================*/
class CEBLicenseTimeInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CEBLicenseTimeInfo> pointer;
	static CEBLicenseTimeInfo::pointer create(void)
	{
		return CEBLicenseTimeInfo::pointer(new CEBLicenseTimeInfo());
	}

	bool	m_bLicenseOk;
	time_t	m_nLicenseEndTime;
	time_t	m_nLastTime;
	int		m_nVideoConferenceCount;

	CEBLicenseTimeInfo(void)
		: m_bLicenseOk(false), m_nLicenseEndTime(0), m_nLastTime(0)
		, m_nVideoConferenceCount(0)
	{
	}
	const CEBLicenseTimeInfo& operator = (const CEBLicenseTimeInfo& p) {return this->operator =(&p);}
	const CEBLicenseTimeInfo& operator = (const CEBLicenseTimeInfo::pointer& p) {return this->operator =(p.get());}
	const CEBLicenseTimeInfo& operator = (const CEBLicenseTimeInfo* p)
	{
		if (p!=NULL)
		{
			m_bLicenseOk = p->m_bLicenseOk;
			m_nLicenseEndTime = p->m_nLicenseEndTime;
			m_nLastTime = p->m_nLastTime;
			m_nVideoConferenceCount = p->m_nVideoConferenceCount;
		}
		return *this;
	}
	virtual cgcObject::pointer copyNew(void) const
	{
		CEBLicenseTimeInfo::pointer result = CEBLicenseTimeInfo::create();
		result->m_bLicenseOk = m_bLicenseOk;
		result->m_nLicenseEndTime = m_nLicenseEndTime;
		result->m_nLastTime = m_nLastTime;
		result->m_nVideoConferenceCount = m_nVideoConferenceCount;
		return result;
	}
};

/*==========================================================
帐号信息
===========================================================*/
class CEBAccountInfo
	: public EB_AccountInfo
	, public cgcObject
{
public:
	typedef boost::shared_ptr<CEBAccountInfo> pointer;
	static CEBAccountInfo::pointer create(mycp::bigint nUserId, const CEBString& sAccount, int nLogonType = EB_LOGON_TYPE_UNKNOWN)
	{
		return CEBAccountInfo::pointer(new CEBAccountInfo(nUserId, sAccount,nLogonType));
	}
	static CEBAccountInfo::pointer create(const CEBAccountInfo::pointer& p)
	{
		return CEBAccountInfo::pointer(new CEBAccountInfo(p.get()));
	}
	static CEBAccountInfo::pointer create(void)
	{
		return CEBAccountInfo::pointer(new CEBAccountInfo());
	}
	CEBAccountInfo(void)
		: EB_AccountInfo()
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_nSetting(0),m_sDefaultEmpId(0),m_sDefaultEntId(0),m_nMyGroupInfoVer(0),m_nEntDepInfoVer(0),m_nContactInfoVer(0),m_bLicenseUser(true),m_nVideoConferenceCount(0)
		, m_nIOSSSLId(0), m_nAndroidSSLId(0)//m_nAndroidPushType(EB_ANDROID_PUSH_TYPE_UNKNOWN)
		//, m_nLastCallLogonType(EB_LOGON_TYPE_UNKNOWN)
		, m_nUserSignId(0)//,m_nFromIpAddress(0)
		, m_nOnlineId(0)
		//, m_nTrafficId(0)//,m_nOnlineDate(0)
		//, m_nDataYearDay(-1),m_nDataOff(0),m_nDataUp(0),m_nDataDown(0),m_nDataRes(0),m_nResEntId(0)
#ifdef USES_CHATROOM
		,m_bLocalP2POk(false),m_bRemoteP2POk(false)
#endif
		, m_tLastApiCallTime(0),m_nLastApiCallSign(EB_SIGN_UNKNOWN)
		//, m_tLastRequestReceiveMailTime(0), m_tLastReceiveNewMailTime(0)
		//, m_tNextRequestReceiveMailTime(0)
		, m_tLastRequestReceiveMailTime(0), m_nWaitNextReceiveMailTime(0)
	{
		m_dwAccountData = 0;
		m_tOnlineTime = time(0);
	}
	CEBAccountInfo(const CEBAccountInfo* p)
		: EB_AccountInfo()
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_nSetting(0),m_sDefaultEmpId(0),m_sDefaultEntId(0),m_nMyGroupInfoVer(0),m_nEntDepInfoVer(0),m_nContactInfoVer(0),m_bLicenseUser(true),m_nVideoConferenceCount(0)
		, m_nIOSSSLId(0), m_nAndroidSSLId(0)//m_nAndroidPushType(EB_ANDROID_PUSH_TYPE_UNKNOWN)
		//, m_nLastCallLogonType(EB_LOGON_TYPE_UNKNOWN)
		, m_nUserSignId(0)//,m_nFromIpAddress(0)
		, m_nOnlineId(0)
		//, m_nTrafficId(0)//,m_nOnlineDate(0)
		//, m_nDataYearDay(-1),m_nDataOff(0),m_nDataUp(0),m_nDataDown(0),m_nDataRes(0),m_nResEntId(0)
#ifdef USES_CHATROOM
		,m_bLocalP2POk(false),m_bRemoteP2POk(false)
#endif
		, m_tLastApiCallTime(0),m_nLastApiCallSign(EB_SIGN_UNKNOWN)
		//, m_tLastRequestReceiveMailTime(0), m_tLastReceiveNewMailTime(0)
		//, m_tNextRequestReceiveMailTime(0)
		, m_tLastRequestReceiveMailTime(0), m_nWaitNextReceiveMailTime(0)
	{
		m_dwAccountData = 0;
		m_tOnlineTime = time(0);
		this->operator = (p);
	}
	CEBAccountInfo(mycp::bigint nUserId, const CEBString& sAccount, int nLogonType)
		: EB_AccountInfo(nUserId,sAccount,nLogonType)
		, m_nLineState(EB_LINE_STATE_UNKNOWN)
		, m_nSetting(0),m_sDefaultEmpId(0),m_sDefaultEntId(0),m_nMyGroupInfoVer(0),m_nEntDepInfoVer(0),m_nContactInfoVer(0),m_bLicenseUser(true),m_nVideoConferenceCount(0)
		, m_nIOSSSLId(0), m_nAndroidSSLId(0)//m_nAndroidPushType(EB_ANDROID_PUSH_TYPE_UNKNOWN)
		//, m_nLastCallLogonType(EB_LOGON_TYPE_UNKNOWN)
		, m_nUserSignId(0)//,m_nFromIpAddress(0)
		, m_nOnlineId(0)
		//, m_nTrafficId(0)//,m_nOnlineDate(0)
		//, m_nDataYearDay(-1),m_nDataOff(0),m_nDataUp(0),m_nDataDown(0),m_nDataRes(0),m_nResEntId(0)
#ifdef USES_CHATROOM
		,m_bLocalP2POk(false),m_bRemoteP2POk(false)
#endif
		, m_tLastApiCallTime(0),m_nLastApiCallSign(EB_SIGN_UNKNOWN)
		//, m_tLastRequestReceiveMailTime(0), m_tLastReceiveNewMailTime(0)
		//, m_tNextRequestReceiveMailTime(0)
		, m_tLastRequestReceiveMailTime(0), m_nWaitNextReceiveMailTime(0)
	{
		m_dwAccountData = 0;
		m_tOnlineTime = time(0);
	}
	virtual ~CEBAccountInfo(void)
	{
		//printf("**** virtual ~CEBAccountInfo(void)...\n");
		m_pLogonList.clear();
		m_pServiceInfo.reset();
		m_pGroupList.clear(false);
		m_pEntGroupList.clear(false);
#ifdef USES_CHATROOM
		m_pLocalChatRoom.reset();
		m_pRemoteChatRoom.reset();
#endif
		//printf("**** virtual ~CEBAccountInfo(void)ok\n");
	}
	const CEBAccountInfo& operator = (const CEBAccountInfo::pointer& p) {return this->operator = (p.get());}
	const CEBAccountInfo& operator = (const CEBAccountInfo& p) {return this->operator = (&p);}
	const CEBAccountInfo& operator = (const CEBAccountInfo* p)
	{
		if (p!=NULL)
		{
			EB_AccountInfo::operator = (p);
			m_pGroupList = p->m_pGroupList;
			m_pEntGroupList = p->m_pEntGroupList;
			m_sFromInfo = p->m_sFromInfo;
			m_dwAccountData = p->m_dwAccountData;
			m_pCallList = p->m_pCallList;
			m_pOffCallList = p->m_pOffCallList;
			m_pLogonList = p->m_pLogonList;
			theSubFuncIdList = p->theSubFuncIdList;
			theMsgList = p->theMsgList;
			theLoadMemberGroupList = p->theLoadMemberGroupList;

			m_tLastApiCallTime = p->m_tLastApiCallTime;
			m_nLastApiCallSign = p->m_nLastApiCallSign;
			m_tLastRequestReceiveMailTime = p->m_tLastRequestReceiveMailTime;
			m_nWaitNextReceiveMailTime = p->m_nWaitNextReceiveMailTime;

			m_tOnlineTime = p->GetOnlineTime();
			m_nLineState = p->GetLineState();
			m_nSetting = p->GetSetting();
			m_sDefaultEmpId = p->GetDefaultEmpId();
			m_sDefaultEntId = p->GetDefaultEntId();
			m_nMyGroupInfoVer = p->GetMyGroupInfoVer();
			m_nEntDepInfoVer = p->GetEntDepInfoVer();
			m_nContactInfoVer = p->GetContactInfoVer();
			m_bLicenseUser = p->GetLicenseUser();
			m_nVideoConferenceCount = p->GetVCCount();
			m_nAccessToken = p->GetAccessToken();
			m_nOnlineKey = p->GetOnlineKey();
			m_sUmServer = p->GetUmServer();
			m_sUmAppName = p->GetUmAppName();
			m_nIOSSSLId = p->GetIOSSSLId();
			m_sIOSDeviceToken = p->GetIOSDeviceToken();
			//m_nAndroidPushType = p->GetAndroidPushType();
			m_nAndroidSSLId = p->GetAndroidSSLId();
			m_sAndroidDeviceToken = p->GetAndroidDeviceToken();
			m_sPwdSalt = p->GetPwdSalt();
			m_sSymmetricCipher = p->GetSymmetricCipher();
			//if (m_pServiceInfo.get()!=NULL)
			//	result->SetServiceInfo(m_pServiceInfo->copyNew();
			m_sLocalAddress = p->GetLocalAddress();
			m_sRemoteAddress = p->GetRemoteAddress();
			m_nUserSignId = p->GetUserSignId();
			m_nOnlineId = p->GetOnlineId();
		}
		return *this;
	}
	virtual cgcObject::pointer copyNew(void) const
	{
		CEBAccountInfo::pointer result = CEBAccountInfo::pointer(new CEBAccountInfo());
		((EB_AccountInfo*)result.get())->operator = ((const EB_AccountInfo*)this);
		result->m_pGroupList.operator = (m_pGroupList);
		result->m_pEntGroupList.operator = (m_pEntGroupList);
		result->m_sFromInfo = m_sFromInfo;
		result->m_dwAccountData = m_dwAccountData;
		result->m_pCallList.operator = (m_pCallList);
		result->m_pOffCallList.operator = (m_pOffCallList);
		result->m_pLogonList = m_pLogonList;
		result->theSubFuncIdList.operator = (theSubFuncIdList);
		result->theMsgList.operator = (theMsgList);
		result->theLoadMemberGroupList.operator = (theLoadMemberGroupList);

		result->m_tLastApiCallTime = m_tLastApiCallTime;
		result->m_nLastApiCallSign = m_nLastApiCallSign;
		result->m_tLastRequestReceiveMailTime = m_tLastRequestReceiveMailTime;
		result->m_nWaitNextReceiveMailTime = m_nWaitNextReceiveMailTime;

		result->SetOnlineTime(m_tOnlineTime);
		result->SetLineState(m_nLineState);
		result->SetSetting(m_nSetting);
		result->SetDefaultEmpId(m_sDefaultEmpId);
		result->SetDefaultEntId(m_sDefaultEntId);
		result->SetMyGroupInfoVer(m_nMyGroupInfoVer);
		result->SetEntDepInfoVer(m_nEntDepInfoVer);
		result->SetContactInfoVer(m_nContactInfoVer);
		result->SetLicenseUser(m_bLicenseUser);
		result->SetVCCount(m_nVideoConferenceCount);
		result->SetAccessToken(m_nAccessToken);
		result->SetOnlineKey(m_nOnlineKey);
		result->SetUmServer(m_sUmServer);
		result->SetUmAppName(m_sUmAppName);
		result->SetIOSSSLId(m_nIOSSSLId);
		result->SetIOSDeviceToken(m_sIOSDeviceToken);
		//result->SetAndroidPushType(m_nAndroidPushType);
		result->SetAndroidSSLId(m_nAndroidSSLId);
		result->SetAndroidDeviceToken(m_sAndroidDeviceToken);
		result->SetPwdSalt(m_sPwdSalt);
		result->SetSymmetricCipher(m_sSymmetricCipher);
#ifdef USES_OBJECT_COPYNEW
		if (m_pServiceInfo.get()!=NULL)
			result->SetServiceInfo(m_pServiceInfo->copyNew());
#endif
		result->SetLocalAddress(m_sLocalAddress);
		result->SetRemoteAddress(m_sRemoteAddress);
		result->SetUserSignId(m_nUserSignId);
		result->SetOnlineId(m_nOnlineId);
		return result;
	}

	void SetOnlineTime(time_t value) {m_tOnlineTime = value;}
	time_t GetOnlineTime(void) const {return m_tOnlineTime;}
	void SetLineState(EB_USER_LINE_STATE value) {m_nLineState = value;}
	EB_USER_LINE_STATE GetLineState(void) const {return m_nLineState;}
	bool IsUnknownLineState(void) const {return m_nLineState == EB_LINE_STATE_UNKNOWN;}
	bool IsOnLineState(void) const {return m_nLineState == EB_LINE_STATE_ONLINE_NEW;}
	bool IsFullOnLineState(void) const {return m_nLineState >= EB_LINE_STATE_BUSY;}
	bool IsOffLineState(void) const {return m_nLineState == EB_LINE_STATE_OFFLINE;}
	bool IsFullOffLineState(void) const {return m_nLineState <= EB_LINE_STATE_OFFLINE;}
	//void SetDescription(const tstring& v) {m_sDescription = v;}
	//const tstring& GetDescription(void) const {return m_sDescription;}
	void SetSetting(int v) {m_nSetting = v;}
	int GetSetting(void) const {return m_nSetting;}
	void SetDefaultEmpId(mycp::bigint v) {m_sDefaultEmpId = v;}
	mycp::bigint GetDefaultEmpId(void) const {return m_sDefaultEmpId;}
	void SetDefaultEntId(mycp::bigint v) {m_sDefaultEntId = v;}
	mycp::bigint GetDefaultEntId(void) const {return m_sDefaultEntId;}
	void SetMyGroupInfoVer(mycp::bigint v) {m_nMyGroupInfoVer = v;}
	mycp::bigint GetMyGroupInfoVer(void) const {return m_nMyGroupInfoVer;}
	void SetEntDepInfoVer(mycp::bigint v) {m_nEntDepInfoVer = v;}
	mycp::bigint GetEntDepInfoVer(void) const {return m_nEntDepInfoVer;}
	void SetContactInfoVer(mycp::bigint v) {m_nContactInfoVer = v;}
	mycp::bigint GetContactInfoVer(void) const {return m_nContactInfoVer;}
	void SetLicenseUser(bool v) {m_bLicenseUser = v;}
	bool GetLicenseUser(void) const {return m_bLicenseUser;}
	void SetVCCount(int v) {m_nVideoConferenceCount = v;}
	int GetVCCount(void) const {return m_nVideoConferenceCount;}
	
	CLockMap<mycp::bigint,int> m_pGroupList;		// groupid->grouptype
	CLockMap<mycp::bigint,int> m_pEntGroupList;	// groupid->grouptype

	//void SetACMKey(const tstring & value) {m_sACMKey = value;}
	//const tstring & GetACMKey(void) const {return m_sACMKey;}
	const tstring GetACMKey(const tstring& sSotpSessionId) const {return m_pLogonList.GetACMKey(sSotpSessionId);}
	void SetUmServer(const tstring& v) {m_sUmServer = v;}
	const tstring& GetUmServer(void) const {return m_sUmServer;}
	void SetUmAppName(const tstring& v) {m_sUmAppName = v;}
	const tstring& GetUmAppName(void) const {return m_sUmAppName;}
	void SetIOSDeviceToken(const tstring& v) {m_sIOSDeviceToken = v;}
	const tstring& GetIOSDeviceToken(void) const {return m_sIOSDeviceToken;}
	void SetAndroidDeviceToken(const tstring& v) {m_sAndroidDeviceToken = v;}
	const tstring& GetAndroidDeviceToken(void) const {return m_sAndroidDeviceToken;}
	void SetPwdSalt(const tstring& v) {m_sPwdSalt = v;}
	const tstring& GetPwdSalt(void) const {return m_sPwdSalt;}
	void SetSymmetricCipher(const tstring& v) {m_sSymmetricCipher = v;}
	const tstring& GetSymmetricCipher(void) const {return m_sSymmetricCipher;}

	void SetIOSSSLId(mycp::bigint v) {m_nIOSSSLId = v;}
	mycp::bigint GetIOSSSLId(void) const {return m_nIOSSSLId;}
	void SetAndroidSSLId(mycp::bigint v) {m_nAndroidSSLId = v;}
	mycp::bigint GetAndroidSSLId(void) const {return m_nAndroidSSLId;}
	//void SetAndroidPushType(EB_ANDROID_PUSH_TYPE v) {m_nAndroidPushType = v;}
	//EB_ANDROID_PUSH_TYPE GetAndroidPushType(void) const {return m_nAndroidPushType;}
	
	bool IsLogonEmail(void) const {return (m_nLogonType&EB_LOGON_TYPE_EMAIL)==EB_LOGON_TYPE_EMAIL;}
	bool IsLogonPhone(void) const {return (m_nLogonType&EB_LOGON_TYPE_PHONE)==EB_LOGON_TYPE_PHONE;}
	bool IsLogonVisitor(void) const {return (m_nLogonType&EB_LOGON_TYPE_VISITOR)==EB_LOGON_TYPE_VISITOR;}
	bool IsLogonWeb(void) const {return (m_nLogonType&EB_LOGON_TYPE_WEB)==EB_LOGON_TYPE_WEB;}

	void SetServiceInfo(const cgcObject::pointer & value) {m_pServiceInfo = value;}
	cgcObject::pointer GetServiceInfo(void) const {return m_pServiceInfo;}

	void SetOnlineKey(const tstring & value) {m_nOnlineKey = value;}
	const tstring & GetOnlineKey(void) const {return m_nOnlineKey;}
	void SetAccessToken(const tstring & value) {m_nAccessToken = value;}
	const tstring & GetAccessToken(void) const {return m_nAccessToken;}

	//void SetSotpSessionId(const tstring & value) {m_sSotpSessionId = value;}
	//const tstring & GetSotpSessionId(void) const {return m_sSotpSessionId;}
	void SetLocalAddress(const tstring & value) {m_sLocalAddress = value;}
	const tstring & GetLocalAddress(void) const {return m_sLocalAddress;}
	void SetRemoteAddress(const tstring & value) {m_sRemoteAddress = value;}
	const tstring & GetRemoteAddress(void) const {return m_sRemoteAddress;}
	void SetUserSignId(mycp::bigint v) {m_nUserSignId = v;}
	mycp::bigint GetUserSignId(void) const {return m_nUserSignId;}
	//void SetFromIpAddress(unsigned long v) {m_nFromIpAddress = v;}
	//unsigned long GetFromIpAddress(void) const {return m_nFromIpAddress;}
	void SetOnlineId(mycp::bigint v) {m_nOnlineId = v;}
	mycp::bigint GetOnlineId(void) const {return m_nOnlineId;}
	//void SetTrafficId(mycp::bigint v) {m_nTrafficId = v;}
	//mycp::bigint GetTrafficId(void) const {return m_nTrafficId;}
	////void SetOnlineDate(int v) {m_nOnlineDate = v;}
	////int GetOnlineDate(void) const {return m_nOnlineDate;}
	//void SetDataYearDay(int v) {m_nDataYearDay = v;}
	//int GetDataYearDay(void) const {return m_nDataYearDay;}
	//void AddDataOff(mycp::bigint v) {m_nDataOff += v;}
	//void SetDataOff(mycp::bigint v) {m_nDataOff = v;}
	//mycp::bigint GetDataOff(void) const {return m_nDataOff;}
	//void AddDataUp(mycp::bigint v) {m_nDataUp += v;}
	//void SetDataUp(mycp::bigint v) {m_nDataUp = v;}
	//mycp::bigint GetDataUp(void) const {return m_nDataUp;}
	//void AddDataDown(mycp::bigint v) {m_nDataDown += v;}
	//void SetDataDown(mycp::bigint v) {m_nDataDown = v;}
	//mycp::bigint GetDataDown(void) const {return m_nDataDown;}
	////void AddDataRes(mycp::bigint v) {m_nDataRes += v;}
	//void SetDataRes(mycp::bigint res,mycp::bigint nEntId) {m_nDataRes = res;m_nResEntId=nEntId;}
	//mycp::bigint GetDataRes(void) const {return m_nDataRes;}
	//mycp::bigint GetResEntId(void) const {return m_nResEntId;}

	// 1:audio 2:audio&video
	//int m_nVideoType;	// from
	//int m_nVideoUserId;

	//CEBECardInfo::pointer m_pFromCardInfo;
	//EB_ACCOUNT_TYPE GetAccountType(void) const {return m_pFromCardInfo.get()==NULL?EB_ACCOUNT_TYPE_VISITOR:m_pFromCardInfo->m_nAccountType;}
	//tstring GetEmpCode(void) const {return m_pFromCardInfo.get()==NULL?"":m_pFromCardInfo->m_sMemberCode;}
	//tstring GetName(void) const {return m_pFromCardInfo.get()==NULL?"":m_pFromCardInfo->m_sName;}
	//tstring GetDepartment(void) const {return m_pFromCardInfo.get()==NULL?"":m_pFromCardInfo->m_sGroupName;}
	//tstring GetEnterprise(void) const {return m_pFromCardInfo.get()==NULL?"":m_pFromCardInfo->m_sEnterprise;}
	tstring m_sFromInfo;		// 用于保存离线
	unsigned int m_dwAccountData;	// 自定义数据
	CLockMap<mycp::bigint, bool>	m_pCallList;	// callid-> 记录用户对应的callid
	CLockMap<mycp::bigint, bool>	m_pOffCallList;	// callid->
	//CLockMap<int,CEBLogonTypeInfo::pointer> m_pSotpSessionIdList;	// logontype->
	CEBLogonTypeList m_pLogonList;
	CLockMap<mycp::bigint,mycp::bigint> theSubFuncIdList;				// subid->funcid
	CLockMap<mycp::bigint,int> theMsgList;				// msgid->type 0:off-file 
	CLockMap<mycp::bigint,time_t> theLoadMemberGroupList;				// group_id->

#ifdef USES_CHATROOM
	boost::shared_mutex m_mutexLocalChatRoom; 
	Cchatroom::pointer m_pLocalChatRoom;
	bool m_bLocalP2POk;
	boost::shared_mutex m_mutexRemoteChatRoom; 
	Cchatroom::pointer m_pRemoteChatRoom;
	bool m_bRemoteP2POk;
#endif

	unsigned long m_tLastApiCallTime;
	EB_CALL_SIGN m_nLastApiCallSign;
	//time_t m_tNextRequestReceiveMailTime;
	//time_t m_tLastRequestReceiveMailTime;
	//time_t m_tLastReceiveNewMailTime;
	time_t m_tLastRequestReceiveMailTime;
	int m_nWaitNextReceiveMailTime;	// 单位：S

private:
	time_t			m_tOnlineTime;
	EB_USER_LINE_STATE m_nLineState;
	//tstring		m_sDescription;
	unsigned int	m_nSetting;
	mycp::bigint		m_sDefaultEmpId;
	mycp::bigint		m_sDefaultEntId;
	mycp::bigint		m_nMyGroupInfoVer;
	mycp::bigint		m_nEntDepInfoVer;
	mycp::bigint		m_nContactInfoVer;
	bool			m_bLicenseUser;	// 是否授权用户 default true;
	int				m_nVideoConferenceCount;

	tstring		m_nAccessToken;
	tstring		m_nOnlineKey;
	//tstring		m_sACMKey;
	tstring		m_sUmServer;
	tstring		m_sUmAppName;
	mycp::bigint		m_nIOSSSLId;
	tstring		m_sIOSDeviceToken;
	mycp::bigint		m_nAndroidSSLId;
	//EB_ANDROID_PUSH_TYPE m_nAndroidPushType;
	tstring		m_sAndroidDeviceToken;
	tstring		m_sPwdSalt;
	tstring		m_sSymmetricCipher;

	cgcObject::pointer m_pServiceInfo;
	//tstring		m_sSotpSessionId;
	//EB_LOGON_TYPE m_nLastCallLogonType;
	tstring		m_sLocalAddress;
	tstring		m_sRemoteAddress;
	mycp::bigint		m_nUserSignId;
	//unsigned long	m_nFromIpAddress;
	mycp::bigint		m_nOnlineId;
	//mycp::bigint		m_nTrafficId;
	////int				m_nOnlineDate;	// YYYYMMDD
	//int				m_nDataYearDay;	// 0,365
	//mycp::bigint		m_nDataOff;		// 离线数据流量
	//mycp::bigint		m_nDataUp;		// 上传（发送）数据流量
	//mycp::bigint		m_nDataDown;	// 下载（接收）数据流量
	//mycp::bigint		m_nDataRes;		// 云盘数据流量
	//mycp::bigint		m_nResEntId;	// 云盘所属企业ID
};
const CEBAccountInfo::pointer EB_Null_AccountInfo;


/*==========================================================
系统信息
===========================================================*/
class CEBAppMsgInfo
	: public EB_AppMsgInfo
	, public cgcObject
{
public:
	typedef boost::shared_ptr<CEBAppMsgInfo> pointer;
	static CEBAppMsgInfo::pointer create(void)
	{
		return CEBAppMsgInfo::pointer(new CEBAppMsgInfo());
	}
	static CEBAppMsgInfo::pointer create(mycp::bigint nMsgId,EB_MSG_TYPE nMsgType, const tstring& sMsgContent)
	{
		return CEBAppMsgInfo::pointer(new CEBAppMsgInfo(nMsgId,nMsgType,sMsgContent));
	}

	CEBAppMsgInfo(mycp::bigint nMsgId,EB_MSG_TYPE nMsgType, const tstring& sMsgContent)
		: EB_AppMsgInfo(nMsgId,nMsgType,sMsgContent)
	{
	}
	CEBAppMsgInfo(void)
		: EB_AppMsgInfo()
	{
	}
	CEBAccountInfo::pointer m_pFromAccountInfo;
	CEBAccountInfo::pointer m_pToAccountInfo;
};

/*==========================================================
群组（部门）成员信息
===========================================================*/
#define EB_MEMBER_DATA_SEARCH_INFO			0x1
class CEBMemberInfo : public EB_MemberInfo
{
public:
	typedef boost::shared_ptr<CEBMemberInfo> pointer;
	static CEBMemberInfo::pointer create(void)
	{
		return CEBMemberInfo::pointer(new CEBMemberInfo());
	}
	static CEBMemberInfo::pointer create(mycp::bigint sGroupCode, mycp::bigint nMemberUserId)
	{
		return CEBMemberInfo::pointer(new CEBMemberInfo(sGroupCode,nMemberUserId));
	}
	static CEBMemberInfo::pointer create(const EB_MemberInfo * pMemberInfo)
	{
		return CEBMemberInfo::pointer(new CEBMemberInfo(pMemberInfo));
	}
	void operator =(const CEBMemberInfo * pMemberInfo)
	{
		EB_MemberInfo::operator =(pMemberInfo);
		if (pMemberInfo)
		{
			m_sUmServer = pMemberInfo->m_sUmServer;
			m_sUmAppName = pMemberInfo->m_sUmAppName;
		}
	}
	bool IsOfflineState(void) const {return m_nLineState==EB_LINE_STATE_UNKNOWN || m_nLineState==EB_LINE_STATE_OFFLINE;}
	bool IsFullOnineState(void) const {return m_nLineState>=EB_LINE_STATE_BUSY;}

	CEBAccountInfo::pointer GetAccountInfo(void) const
	{
		CEBAccountInfo::pointer pEmpAccount = CEBAccountInfo::create(m_nMemberUserId,m_sMemberAccount);
		GetAccountInfo(pEmpAccount);
		return pEmpAccount;
	}
	void GetAccountInfo(CEBAccountInfo::pointer & pOutAccountInfo) const
	{
		if (pOutAccountInfo.get() != NULL)
		{
			pOutAccountInfo->m_pFromCardInfo.m_sName = m_sUserName;
			pOutAccountInfo->m_pFromCardInfo.m_sPhone = m_sCellPhone;
			pOutAccountInfo->m_pFromCardInfo.m_sTel = m_sWorkPhone;
			pOutAccountInfo->m_pFromCardInfo.m_sEmail = m_sEmail;
			pOutAccountInfo->m_pFromCardInfo.m_sTitle = m_sJobTitle;
			pOutAccountInfo->SetLineState(m_nLineState);
			pOutAccountInfo->SetUmServer(m_sUmServer);
			pOutAccountInfo->SetUmAppName(m_sUmAppName);
		}
	}

	CEBMemberInfo(void)
		: EB_MemberInfo()
		//, m_bDeleteFlag(false)
		, m_nMemberData(0)
	{}
	CEBMemberInfo(mycp::bigint sGroupCode, mycp::bigint nMemberUserId)
		: EB_MemberInfo(sGroupCode,nMemberUserId)
		//, m_bDeleteFlag(false)
		, m_nMemberData(0)
	{}
	CEBMemberInfo(const EB_MemberInfo * pMemberInfo)
		: EB_MemberInfo()
		//, m_bDeleteFlag(false)
		, m_nMemberData(0)
	{
		EB_MemberInfo::operator =(pMemberInfo);
	}

	unsigned int m_nMemberData;
	//tstring m_sHeadMd5;
	tstring m_sHeadCmServer;
	tstring m_sHeadHttpServer;
	tstring m_sUmServer;
	tstring m_sUmAppName;
	//bool m_bDeleteFlag;
};
const CEBMemberInfo::pointer EB_Null_MemberInfo;

#define EB_GROUP_DATA_LOAD_MEMBER			0x1
#define EB_GROUP_DATA_LOAD_MEMBER_STATE		0x2
#define EB_GROUP_DATA_NEED_SAVE2LOCAL		0x4
#define EB_GROUP_DATA_DELETE				0x8

class CEBGroupInfo : public EB_GroupInfo
{
public:
	typedef boost::shared_ptr<CEBGroupInfo> pointer;
	static CEBGroupInfo::pointer create(void)
	{
		return CEBGroupInfo::pointer(new CEBGroupInfo());
	}
	static CEBGroupInfo::pointer create(mycp::bigint sEntCode, mycp::bigint sGroupCode)
	{
		return CEBGroupInfo::pointer(new CEBGroupInfo(sEntCode,sGroupCode));
	}
	const CEBGroupInfo& operator =(const EB_GroupInfo& pGroupInfo)
	{
		EB_GroupInfo::operator =(pGroupInfo);
		return *this;
	}
	CLockMap<mycp::bigint, CEBMemberInfo::pointer> m_pMemberList;

	CEBGroupInfo(void)
		: EB_GroupInfo()
		, m_nGroupData(0)
		, m_nServerGroupVer(0)
		, m_nOnlineSize(0)
	{
	}
	CEBGroupInfo(mycp::bigint sEntCode, mycp::bigint sGroupCode)
		: EB_GroupInfo(sEntCode, sGroupCode)
		, m_nGroupData(0)
		, m_nServerGroupVer(0)
		, m_nOnlineSize(0)
	{}
	unsigned int m_nGroupData;
	eb::bigint m_nServerGroupVer;
	int m_nOnlineSize;
};
const CEBGroupInfo::pointer EB_Null_GroupInfo;

/*==========================================================
企业信息
===========================================================*/
class CEBEnterpriseInfo : public EB_EnterpriseInfo
{
public:
	typedef boost::shared_ptr<CEBEnterpriseInfo> pointer;
	static CEBEnterpriseInfo::pointer create(void)
	{
		return CEBEnterpriseInfo::pointer(new CEBEnterpriseInfo());
	}
	static CEBEnterpriseInfo::pointer create(mycp::bigint sEntCode, const tstring& sEntName)
	{
		return CEBEnterpriseInfo::pointer(new CEBEnterpriseInfo(sEntCode,sEntName));
	}
	const CEBEnterpriseInfo& operator =(const EB_EnterpriseInfo& pEnterpriseInfo)
	{
		EB_EnterpriseInfo::operator =(pEnterpriseInfo);
		return *this;
	}
	CEBEnterpriseInfo(void)
		: EB_EnterpriseInfo()
	{}
	CEBEnterpriseInfo(mycp::bigint sEntCode, const tstring& sEntName)
		: EB_EnterpriseInfo(sEntCode,sEntName)
	{}
};
const CEBEnterpriseInfo::pointer EB_Null_EnterpriseInfo;

class CPOPMsgItem
{
public:
	typedef boost::shared_ptr<CPOPMsgItem> pointer;
	enum MsgItemType
	{
		MIT_TEXT
		, MIT_CHARFORMAT
		, MIT_OBJECT
	};

	static CPOPMsgItem::pointer create(const tstring & text)
	{
		return CPOPMsgItem::pointer(new CPOPMsgItem(text));
	}
	static CPOPMsgItem::pointer create(const char * text)
	{
		return CPOPMsgItem::pointer(new CPOPMsgItem(text));
	}
	static CPOPMsgItem::pointer create(char * data, unsigned long size, MsgItemType type = MIT_OBJECT)
	{
		return CPOPMsgItem::pointer(new CPOPMsgItem(data, size, type));
	}

	CPOPMsgItem(const tstring & text)
		: m_type(MIT_TEXT)
		, m_text(text)
		, m_data(NULL), m_size(0)
	{}
	CPOPMsgItem(const char * text)
		: m_type(MIT_TEXT)
		, m_text(text)
		, m_data(NULL), m_size(0)
	{}
	CPOPMsgItem(char * data, unsigned long size, MsgItemType type = MIT_OBJECT)
		: m_type(type)
		, m_text(_T(""))
		, m_data(data), m_size(size)
	{}
	virtual ~CPOPMsgItem(void)
	{
		if (m_data)
		{
			delete []m_data;
		}
	}

	MsgItemType	GetType(void) const {return m_type;}
	void SetText(const char * value) {m_text = value;}
	const tstring & GetText(void) const {return m_text;}
	const char * GetData(void) const {return m_data;}
	unsigned long GetSize(void) const {return m_size;}
	void UpdateData(char * data, unsigned long size)
	{
		if (m_size == size && m_data == NULL)
			m_data = data;
	}

private:
	MsgItemType		m_type;
	tstring			m_text;	// text or object filename
	char *			m_data;
	unsigned long	m_size;
};
const CPOPMsgItem::pointer NullPOPMsgItem;

class CPopParameterList
	: public cgcParameterMap
{
public:
	typedef boost::shared_ptr<CPopParameterList> pointer;
	static CPopParameterList::pointer create(void)
	{
		return CPopParameterList::pointer(new CPopParameterList());
	}
	void SetParameter(const cgcParameter::pointer & pParameter, bool bSetForce = true)
	{
		if (pParameter.get() != NULL)
		{
			if (!bSetForce && pParameter->getType() == cgcValueInfo::TYPE_STRING && pParameter->empty())
				return;
			cgcParameterMap::insert(pParameter->getName(), pParameter);
		}
	}
        CPopParameterList(void) {

        }
};
const CPopParameterList NullPopParameterList;

class CPOPSotpResponseInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CPOPSotpResponseInfo> pointer;
	static CPOPSotpResponseInfo::pointer create(unsigned long nCallId, unsigned long nCallSign, int nResultValue)
	{
		return CPOPSotpResponseInfo::pointer(new CPOPSotpResponseInfo(nCallId, nCallSign, nResultValue));
	}
	CPOPSotpResponseInfo(unsigned long nCallId, unsigned long nCallSign, int nResultValue)
		: m_nCallId(nCallId)
		, m_nCallSign(nCallSign)
		, m_nResultValue(nResultValue)
	{
	}

	unsigned long GetCallId(void) const {return m_nCallId;}
	unsigned long GetCallSign(void) const {return m_nCallSign;}
	int GetResultValue(void) const {return m_nResultValue;}
	time_t GetResponseTime(void) const {return m_nResponseTime;}
	CPopParameterList m_pResponseList;
	cgcAttachment::pointer m_pAttachMent;
	virtual cgcObject::pointer copyNew(void) const
	{
		CPOPSotpResponseInfo::pointer result = CPOPSotpResponseInfo::create(m_nCallId,m_nCallSign,m_nResultValue);
		// ???
		return result;
	}
private:
	unsigned long	m_nCallId;
	unsigned long	m_nCallSign;
	int				m_nResultValue;
	time_t			m_nResponseTime;
};
class CPOPSotpRequestInfo
{
public:
	typedef boost::shared_ptr<CPOPSotpRequestInfo> pointer;
	static CPOPSotpRequestInfo::pointer create(unsigned long nCallId, int nRequestType = EB_REQ_TYPE_UNKNOWN)
	{
		return CPOPSotpRequestInfo::pointer(new CPOPSotpRequestInfo(nCallId, nRequestType));
	}
	CPOPSotpRequestInfo(unsigned long nCallId, int nRequestType)
		: m_nCallId(nCallId)
		, m_nRequestType(nRequestType)
	{
		m_nRequestTime = time(0);
		m_nRequestTimeout = 20;
		m_nResponseTime = 0;
	}
	virtual ~CPOPSotpRequestInfo(void)
	{
		m_pAttachList.clear();
	}

	void SetCallId(unsigned long value) {m_nCallId = value;}
	unsigned long GetCallId(void) const {return m_nCallId;}
	int GetRequestType(void) const {return m_nRequestType;}
	void SetRequestTime(time_t value = time(0)) {m_nRequestTime = value;}
	time_t GetRequestTime(void) const {return m_nRequestTime;}
	void SetRequestTimeout(int v) {m_nRequestTimeout = v;}
	int	GetRequestTimeout(void) const {return m_nRequestTimeout;}
	void SetResponseTime(time_t value = time(0)) {m_nResponseTime = value;}
	time_t GetResponseTime(void) const {return m_nResponseTime;}
	CPopParameterList m_pRequestList;
	//void SetRequestInfo(cgcObject::pointer value) {m_nReqeustInfo = value;}
	//cgcObject::pointer GetRequestInfo(void) const {return m_nReqeustInfo;}
	//cgcObject::pointer m_pRichMsg;
	CLockMap<mycp::bigint, cgcAttachment::pointer> m_pAttachList;	// index->
	//CPopParameterList m_pResponseList;
	mycp::cgcValueInfo::pointer m_pResponseInfo;
private:
	unsigned long	m_nCallId;
	int				m_nRequestType;
	time_t			m_nRequestTime;
	int				m_nRequestTimeout;
	time_t			m_nResponseTime;
	//cgcObject::pointer m_nReqeustInfo;
};
const CPOPSotpRequestInfo::pointer NullPOPSotpRequestInfo;

class CPOPLogonInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CPOPLogonInfo> pointer;
	static CPOPLogonInfo::pointer create(int nLogonType)
	{
		return CPOPLogonInfo::pointer(new CPOPLogonInfo(nLogonType));
	}
	static CPOPLogonInfo::pointer create(int nLogonType, const char * sAccount, const char * sPassword)
	{
		return CPOPLogonInfo::pointer(new CPOPLogonInfo(nLogonType, sAccount, sPassword));
	}
	CPOPLogonInfo(int nLogonType)
		: m_nLogonType(nLogonType)
		, m_nUserId(0)
		, m_sAppId(0)
		, m_nPasswdAuthMode(PASSWD_AUTH_MODE_ENTBOOST)
	{
	}
	CPOPLogonInfo(int nLogonType, const char * sAccount, const char * sPassword)
		: m_nLogonType(nLogonType)
		, m_nUserId(0)
		, m_sAppId(0)
		, m_sAccount(sAccount)
		, m_sPassword(sPassword)
		, m_nPasswdAuthMode(PASSWD_AUTH_MODE_ENTBOOST)
	{
	}
	virtual cgcObject::pointer copyNew(void) const
	{
		CPOPLogonInfo::pointer result = CPOPLogonInfo::create(m_nLogonType);
		// ???
		return result;
	}
	static tstring GetDomain(const tstring & sAccount)
	{
		std::string::size_type find = sAccount.find("@");
		if (find == std::string::npos)
		{
			return "";
		}
		return sAccount.substr(find+1);
	}
	tstring GetDomain(void) const
	{
		return CPOPLogonInfo::GetDomain(m_sAccount);
	}
	int m_nLogonType;
	mycp::bigint m_nUserId;
	tstring m_sAccount;
	tstring m_sPassword;
	eb::bigint m_sAppId;
	tstring m_sReqCode;
	tstring m_sACMKey;
	tstring m_sOAuthKey;
	EB_USER_LINE_STATE m_nLineState;
	PASSWD_AUTH_MODE m_nPasswdAuthMode;
};

class CDataStreamInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CDataStreamInfo> pointer;
	static CDataStreamInfo::pointer create(unsigned long index, unsigned int size)
	{
		return CDataStreamInfo::pointer(new CDataStreamInfo(index, size));
	}
	
	CDataStreamInfo(unsigned long index, unsigned int size)
		: m_nIndex(index)
		, m_nSize(size)
		, m_nReceived(false)
	{
	}
	virtual cgcObject::pointer copyNew(void) const
	{
		CDataStreamInfo::pointer result = CDataStreamInfo::create(m_nIndex,m_nSize);
		result->m_nReceived = m_nReceived;
		return result;
	}
	unsigned long	m_nIndex;
	unsigned int	m_nSize;
	bool			m_nReceived;	// default false
};

class CChatMsgInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CChatMsgInfo> pointer;
	static CChatMsgInfo::pointer create(eb::bigint nMsgId, const cgcObject::pointer& pChatInfo, const cgcObject::pointer& pFromAccount, EB_MSG_TYPE nMsgType,int nSubType)
	{
		return CChatMsgInfo::pointer(new CChatMsgInfo(nMsgId, pChatInfo, pFromAccount,nMsgType,nSubType));
	}
	static CChatMsgInfo::pointer create(void)
	{
		return CChatMsgInfo::pointer(new CChatMsgInfo());
	}
	static CChatMsgInfo::pointer create(const CChatMsgInfo* p)
	{
		return CChatMsgInfo::pointer(new CChatMsgInfo(p));
	}

	CChatMsgInfo(void)
		: m_nMsgId(0)
		, m_nMsgType(EB_MSG_UNKNOWN),m_nSubType(0)
	{
		m_nAttachZipSize = 0;
		//m_nAcceptEncoding = 0;
		m_nAttachEncoding = 0;
		m_sResourceId = 0;
		m_nResourceFromId = 0;
		m_nResourceFromType = EB_RESOURCE_FROM_TYPE_UNKNOWN;
		m_nResourceType = EB_RESOURCE_UNKNOWN;
		m_nFileId = 0;
		m_bTranOk = false;
		m_nToUserId = 0;
		m_nPrivate = 0;
		m_nOffFile = 0;
		m_nMsgTime = time(0);
		m_pRichBuffer = NULL;
		m_bSave2File = false;
		m_nrichmsgsize = 0;
		m_nprovsize = 0;
		m_pFileHandle = NULL;
		m_FileUnZipSize = 0;
		m_nFileDataSize = 0;
		m_pFileData = NULL;
		m_nFileOffset = 0;
		m_tBeginTranTime = 0;
		m_tRecvTime = 0;
		m_nDSAckCount = 0;
		m_tPercentTime = 0;
		m_nDataStreamSize = 0;
		//m_bOffLineMsg = false;
		m_dwMsgData = 0;
	}
	CChatMsgInfo(const CChatMsgInfo* p)
		: m_nMsgId(0)
		, m_nMsgType(EB_MSG_UNKNOWN),m_nSubType(0)
	{
		m_nAttachZipSize = 0;
		//m_nAcceptEncoding = 0;
		m_nAttachEncoding = 0;
		m_sResourceId = 0;
		m_nResourceFromId = 0;
		m_nResourceFromType = EB_RESOURCE_FROM_TYPE_UNKNOWN;
		m_nResourceType = EB_RESOURCE_UNKNOWN;
		m_nFileId = 0;
		m_bTranOk = false;
		m_nToUserId = 0;
		m_nPrivate = 0;
		m_nOffFile = 0;
		m_nMsgTime = time(0);
		m_pRichBuffer = NULL;
		m_bSave2File = false;
		m_nrichmsgsize = 0;
		m_nprovsize = 0;
		m_pFileHandle = NULL;
		m_FileUnZipSize = 0;
		m_nFileDataSize = 0;
		m_pFileData = NULL;
		m_nFileOffset = 0;
		m_tBeginTranTime = 0;
		m_tRecvTime = 0;
		m_nDSAckCount = 0;
		m_tPercentTime = 0;
		m_nDataStreamSize = 0;
		//m_bOffLineMsg = false;
		m_dwMsgData = 0;
		this->operator = (p);
	}
	CChatMsgInfo(eb::bigint nMsgId, const cgcObject::pointer& pChatInfo, const cgcObject::pointer& pFromAccount, EB_MSG_TYPE nMsgType,int nSubType)
		: m_nMsgId(nMsgId)
		, m_pChatInfo(pChatInfo)
		, m_pFromAccount(pFromAccount)
		, m_nMsgType(nMsgType),m_nSubType(nSubType)
	{
		m_nAttachZipSize = 0;
		//m_nAcceptEncoding = 0;
		m_nAttachEncoding = 0;
		m_sResourceId = 0;
		m_nResourceFromId = 0;
		m_nResourceFromType = EB_RESOURCE_FROM_TYPE_UNKNOWN;
		m_nResourceType = EB_RESOURCE_UNKNOWN;
		m_bTranOk = false;
		m_nToUserId = 0;
		m_nPrivate = 0;
		m_nOffFile = 0;
		m_nMsgTime = time(0);
		m_pRichBuffer = NULL;
		m_bSave2File = false;
		m_nrichmsgsize = 0;
		m_nprovsize = 0;
		m_pFileHandle = NULL;
		m_FileUnZipSize = 0;
		m_nFileDataSize = 0;
		m_pFileData = NULL;
		m_nFileOffset = 0;
		m_tBeginTranTime = 0;
		m_tRecvTime = 0;
		m_nDSAckCount = 0;
		m_tPercentTime = 0;
		m_nDataStreamSize = 0;
		//m_bOffLineMsg = false;
		m_dwMsgData = 0;
	}
	virtual ~CChatMsgInfo(void)
	{
		if (m_pRichBuffer)
		{
			delete[] m_pRichBuffer;
			m_pRichBuffer = NULL;
		}
		if (m_pFileHandle!=NULL)
		{
			fclose(m_pFileHandle);
			m_pFileHandle = NULL;
		}
		m_nFileDataSize = 0;
		if (m_pFileData!=NULL)
		{
			delete[] m_pFileData;
			m_pFileData = NULL;
		}
		m_pWaitStreamList.clear(false);
		m_pRecvAttachList.clear(false);
		m_pRecvUserList.clear(false);
		m_pRecvedList.clear(false);
		m_pRequestedUserList.clear(false);
		m_pAckTime.clear(false);
		m_pOffLineAccount.clear(false);
		m_pRichMsg.reset();
		m_pChatInfo.reset();
		m_pFromAccount.reset();
		//printf("*** ~CChatMsgInfo() m_sRemoveFileName=%s\n",m_sRemoveFileName.c_str());
#ifdef _QT_MAKE_
                if (!m_sRemoveFileName.isEmpty()) {
                    QFile::remove(m_sRemoveFileName);
                }
#else
                if (!m_sRemoveFileName.empty()) {
                    remove(m_sRemoveFileName.c_str());
                }
#endif
	}
	const CChatMsgInfo& operator = (const CChatMsgInfo& p) {return this->operator = (&p);}
	const CChatMsgInfo& operator = (const CChatMsgInfo::pointer& p) {return this->operator = (p.get());}
	const CChatMsgInfo& operator = (const CChatMsgInfo* p)
	{
		if (p!=NULL)
		{
			m_nAttachZipSize = p->m_nAttachZipSize;
			m_nAttachEncoding = p->m_nAttachEncoding;
			m_sResourceId = p->m_sResourceId;
			m_nResourceFromId = p->m_nResourceFromId;
			m_nResourceFromType = p->m_nResourceFromType;
			m_nResourceType = p->m_nResourceType;
			m_nFileId = p->m_nFileId;
			m_sResCmServer = p->m_sResCmServer;
			m_sResCmAppName = p->m_sResCmAppName;
			m_sSavePath = p->m_sSavePath;
			m_bTranOk = p->m_bTranOk;
			m_nToUserId = p->m_nToUserId;
			m_nPrivate = p->m_nPrivate;
			m_nOffFile = p->m_nOffFile;
			m_sSendFromSessionId = p->m_sSendFromSessionId;

			m_pWaitStreamList = p->m_pWaitStreamList;
			//m_pRecvAttachList = p->m_pRecvAttachList;
			{
				BoostReadLock rdlock(const_cast<boost::shared_mutex&>(p->m_pRecvAttachList.mutex()));
				CLockMap<mycp::bigint, cgcAttachment::pointer>::const_iterator pIter = p->m_pRecvAttachList.begin();
				for (; pIter!=p->m_pRecvAttachList.end(); pIter++)
				{
					const cgcAttachment::pointer pOldAttachment = pIter->second;
					m_pRecvAttachList.insert(pOldAttachment->getIndex(), cgcAttachment::create(pOldAttachment.get()),false);
				}
			}
			//m_pRecvUserList = p->m_pRecvUserList;
			{
				BoostReadLock rdlock(const_cast<boost::shared_mutex&>(p->m_pRecvUserList.mutex()));
				CLockMap<mycp::bigint, CEBLogonTypeList::pointer>::const_iterator pIter = p->m_pRecvUserList.begin();
				for (; pIter!=p->m_pRecvUserList.end(); pIter++)
				{
					const CEBLogonTypeList::pointer pOldLogonTypeList = pIter->second;
					m_pRecvUserList.insert(pIter->first, CEBLogonTypeList::create(pOldLogonTypeList.get()),false);
				}
			}

			m_pRecvedList = p->m_pRecvedList;
			m_pRequestedUserList = p->m_pRequestedUserList;
			m_pAckTime = p->m_pAckTime;
			m_pOffLineAccount = p->m_pOffLineAccount;

			m_sFileName = p->m_sFileName;
			m_sExt = p->m_sExt;
			m_sPushStringTitle = p->m_sPushStringTitle;
			m_sPushStringDesc = p->m_sPushStringDesc;
			m_pRichBuffer = p->m_pRichBuffer;
			m_nrichmsgsize = p->m_nrichmsgsize;
			m_nprovsize = p->m_nprovsize;
			m_bSave2File = p->m_bSave2File;
#ifdef _QT_MAKE_
                        if (!m_sFileName.isEmpty() && p->m_pFileHandle!=0) {
                            m_pFileHandle = fopen(m_sFileName.toLocal8Bit().constData(), "rb");	// ? "w+b"
                        }
#else
                        if (!m_sFileName.empty() && p->m_pFileHandle!=0) {
                            m_pFileHandle = fopen(m_sFileName.c_str(), "rb");	// ? "w+b"
                        }
#endif
			m_FileUnZipSize = p->m_FileUnZipSize;
#ifdef _QT_MAKE_
                        if (!p->m_sRemoveFileName.isEmpty()) {
                            m_sRemoveFileName = p->m_sRemoveFileName;
                            const_cast<CChatMsgInfo*>(p)->m_sRemoveFileName.clear();
                        }
#else
			if (!p->m_sRemoveFileName.empty())
			{
				m_sRemoveFileName = p->m_sRemoveFileName;
				const_cast<CChatMsgInfo*>(p)->m_sRemoveFileName.clear();
			}
#endif
			if (p->m_nFileDataSize>0 && p->m_pFileData!=NULL)
			{
				m_nFileDataSize = p->m_nFileDataSize;
				m_pFileData = new unsigned char[m_nFileDataSize+1];
				memcpy(m_pFileData,p->m_pFileData,m_nFileDataSize);
			}
			m_nFileOffset = p->m_nFileOffset;
			m_tBeginTranTime = p->m_tBeginTranTime;
			m_tRecvTime = p->m_tRecvTime;
			m_nDSAckCount = p->m_nDSAckCount;
			m_tPercentTime = p->m_tPercentTime;
			m_nDataStreamSize = p->m_nDataStreamSize;
			m_sMsgTime = p->m_sMsgTime;
			m_sMD5String = p->m_sMD5String;
			m_dwMsgData = p->m_dwMsgData;
			//m_pRichMsg = p->m_pRichMsg;						// *服务端不需要

			m_nMsgId = p->m_nMsgId;
			//m_pChatInfo = p->m_pChatInfo;					// * 通过 SetChatInfo 设置
			//m_pFromAccount = p->m_pFromAccount;		// * 通过 SetFromAccount 设置
			m_nMsgType = p->m_nMsgType;
			m_nSubType = p->m_nSubType;
			m_nMsgTime = p->m_nMsgTime;
			m_sContent = p->m_sContent;
			m_nSize = p->m_nSize;
		}
		return *this;
	}
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}
	eb::bigint GetMsgId(void) const {return m_nMsgId;}
	void SetChatInfo(const cgcObject::pointer& pChatInfo) {m_pChatInfo = pChatInfo;}
	cgcObject::pointer GetChatInfo(void) const {return m_pChatInfo;}
	void SetFromAccount(const cgcObject::pointer& pFromAccount) {m_pFromAccount = pFromAccount;}
	cgcObject::pointer GetFromAccount(void) const {return m_pFromAccount;}
	EB_MSG_TYPE GetMsgType(void) const {return m_nMsgType;}
	int GetSubType(void) const {return m_nSubType;}
	void SetContent(const tstring & value) {m_sContent = value;}
	const tstring & GetContent(void) const {return m_sContent;}
	time_t GetMsgTime(void) const {return m_nMsgTime;}
	void SetSize(mycp::bigint value) {m_nSize = value;}
	mycp::bigint GetSize(void) const {return m_nSize;}
	mycp::bigint GetRealSize(void) const {return m_FileUnZipSize>0?m_FileUnZipSize:m_nSize;}

	mycp::bigint m_nAttachZipSize;
	//int m_nAcceptEncoding;	// 1=gzip,2=deflate(default),3=both
	int m_nAttachEncoding;	// 2=deflate(default)
	mycp::bigint m_sResourceId;
	mycp::bigint m_nResourceFromId;
	EB_RESOURCE_FROM_TYPE m_nResourceFromType;
	EB_RESOURCE_TYPE m_nResourceType;
	mycp::bigint m_nFileId;
	//tstring m_sResourceExt;
	tstring m_sResCmServer;
	tstring m_sResCmAppName;
	tstring m_sSavePath;
	bool m_bTranOk;
	mycp::bigint m_nToUserId;
	int m_nPrivate;
	int m_nOffFile;	// 保存离线文件
	tstring m_sSendFromSessionId;

	CLockMap<int, bool>		m_pWaitStreamList;			// index->
	CLockMap<mycp::bigint, cgcAttachment::pointer>		m_pRecvAttachList;	// index->
	CLockMap<mycp::bigint, CEBLogonTypeList::pointer>		m_pRecvUserList;		// uid->多少个终端接收
	CLockMap<tstring, bool>			m_pRecvedList;		// uid->多少个终端接收
	CLockMap<mycp::bigint, int>				m_pRequestedUserList;	// uid->logontype
	CLockMap<tstring,time_t>				m_pAckTime;				// sid->
	//CLockMap<mycp::bigint,time_t>			m_pAckTime;				// uid->
	CLockMap<mycp::bigint,bool>				m_pOffLineAccount;		// uid->

	boost::mutex	m_mutexMsg;
	boost::mutex	m_mutexFile;
        EBFileString		m_sFileName;
	tstring			m_sExt;
	tstring			m_sPushStringTitle;
	tstring			m_sPushStringDesc;
	char *			m_pRichBuffer;
	unsigned int	m_nrichmsgsize;
	unsigned int	m_nprovsize;
	bool			m_bSave2File;
	FILE *			m_pFileHandle;			// for bitmap/file
	mycp::bigint		m_FileUnZipSize;
        EBFileString		m_sRemoveFileName;
	int				m_nFileDataSize;
	unsigned char* m_pFileData;
	int				m_nFileOffset;		// for USES_NEW_RESOURCE_INFO
	time_t			m_tBeginTranTime;
	time_t			m_tRecvTime;
	int				m_nDSAckCount;
	time_t			m_tPercentTime;		
	unsigned int	m_nDataStreamSize;
	//bool			m_bOffLineMsg;
	tstring			m_sMsgTime;		// null is current time
	tstring			m_sMD5String;
	unsigned int	m_dwMsgData;	// 自定义数据
	cgcObject::pointer	m_pRichMsg;
private:
	eb::bigint			m_nMsgId;
	cgcObject::pointer	m_pChatInfo;
	cgcObject::pointer	m_pFromAccount;
	EB_MSG_TYPE	m_nMsgType;
	int					m_nSubType;
	time_t				m_nMsgTime;
	tstring				m_sContent;
	mycp::bigint			m_nSize;
};
const CChatMsgInfo::pointer NullChatMsgInfo;


class COffChatMsgList
	: public cgcObject
{
public:
	typedef boost::shared_ptr<COffChatMsgList> pointer;
	static COffChatMsgList::pointer create(void)
	{
		return COffChatMsgList::pointer(new COffChatMsgList());
	}
	CLockList<CChatMsgInfo::pointer> m_pOffChatMsgList;
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}
};


//class COffCallList
//	: public cgcObject
//{
//public:
//	typedef boost::shared_ptr<COffCallList> pointer;
//	static COffCallList::pointer create(const tstring & sAccount)
//	{
//		return COffCallList::pointer(new COffCallList(sAccount));
//	}
//
//	COffCallList(const tstring & sAccount)
//		: m_sAccount(sAccount)
//	{
//	}
//	tstring m_sAccount;
//	CLockMap<tstring, bool> m_pCallList;	// callid
//};
#ifdef USES_VIDEOROOM
typedef struct tag_VideoStreamData
{
	void* pApp;
	void* pCallback;
	mycp::bigint m_nCallId;
	//DWORD dwData;
}VideoStreamData;
#endif

#define EB_CALL_DATA_P2P_OK			0x1

class CEBCallInfo
	: public EB_CallInfo
	, public cgcObject
{
public:
	typedef boost::shared_ptr<CEBCallInfo> pointer;
	static CEBCallInfo::pointer create(mycp::bigint sCallId,mycp::bigint sGroupCode,const CEBAccountInfo::pointer& pFrom)
	{
		return CEBCallInfo::pointer(new CEBCallInfo(sCallId,sGroupCode,pFrom));
	}
	static CEBCallInfo::pointer create(const CEBCallInfo::pointer& p)
	{
		return CEBCallInfo::pointer(new CEBCallInfo(p.get()));
	}
	static CEBCallInfo::pointer create(void)
	{
		return CEBCallInfo::pointer(new CEBCallInfo());
	}

	CEBCallInfo(void)
		: EB_CallInfo()
		, m_sInGroupCode(0)
		, m_sCall2GroupFromUserId(0), m_tCall2GroupTime(0)
		, m_bOffLineCall(false)
		, m_nCallState(EB_CALL_STATE_UNKNOWN), m_tCallConnection(0), /*m_nUmServerState(EB_SS_INIT),*/m_nCmServerState(EB_SS_INIT)
#ifdef USES_VIDEOROOM
		, m_nLocalOnVideo(false)
#endif
	{
		m_nRemoteDesktopParam = 0;
		m_nRemoteDesktopSID = 0;
		m_tRDServerTime = 0;
		m_nRDType = EB_RD_UNKNOWN;
		m_sVideoSID = 0;
		m_nVideoParam = 0;
		m_tVSLastTime = 0;
		m_nVideoType = EB_VIDEO_UNKNOWN;
		m_sChatId = 0;
		//m_sOldCallId = 0;
		m_nCC = 0;
		m_bSendCallRecord = false;
		m_nCallTime = 0;
		m_bAutoCallAccept = true;
		m_tLastTime = 0;
		m_nCallData = 0;
#ifdef USES_VIDEOROOM
		m_tP2POkTime = 0;
		memset(&m_pAudioStreamData,0,sizeof(m_pAudioStreamData));
#endif
		m_tCmTime = 0;
	}
	CEBCallInfo(const CEBCallInfo* p)
		: EB_CallInfo()
		, m_sInGroupCode(0)
		, m_sCall2GroupFromUserId(0), m_tCall2GroupTime(0)
		, m_bOffLineCall(false)
		, m_nCallState(EB_CALL_STATE_UNKNOWN), m_tCallConnection(0), /*m_nUmServerState(EB_SS_INIT),*/m_nCmServerState(EB_SS_INIT)
#ifdef USES_VIDEOROOM
		, m_nLocalOnVideo(false)
#endif
	{
		m_nRemoteDesktopParam = 0;
		m_nRemoteDesktopSID = 0;
		m_tRDServerTime = 0;
		m_nRDType = EB_RD_UNKNOWN;
		m_sVideoSID = 0;
		m_nVideoParam = 0;
		m_tVSLastTime = 0;
		m_nVideoType = EB_VIDEO_UNKNOWN;
		m_sChatId = 0;
		//m_sOldCallId = 0;
		m_nCC = 0;
		m_bSendCallRecord = false;
		m_nCallTime = 0;
		m_bAutoCallAccept = true;
		m_tLastTime = 0;
		m_nCallData = 0;
#ifdef USES_VIDEOROOM
		m_tP2POkTime = 0;
		memset(&m_pAudioStreamData,0,sizeof(m_pAudioStreamData));
#endif
		m_tCmTime = 0;
		this->operator =(p);
	}
	CEBCallInfo(mycp::bigint sCallId,mycp::bigint sGroupCode,const CEBAccountInfo::pointer& pFrom)
		: EB_CallInfo(pFrom->GetUserId(),pFrom->GetAccount(),sCallId,sGroupCode)
		, m_sInGroupCode(0),m_pFromAccount(pFrom)
		, m_sCall2GroupFromUserId(0), m_tCall2GroupTime(0)
		, m_bOffLineCall(false)
		, m_nCallState(EB_CALL_STATE_UNKNOWN), m_tCallConnection(0), /*m_nUmServerState(EB_SS_INIT),*/m_nCmServerState(EB_SS_INIT)
#ifdef USES_VIDEOROOM
		, m_nLocalOnVideo(false)
#endif
	{
		m_nRemoteDesktopParam = 0;
		m_nRemoteDesktopSID = 0;
		m_tRDServerTime = 0;
		m_nRDType = EB_RD_UNKNOWN;
		m_sVideoSID = 0;
		m_nVideoParam = 0;
		m_tVSLastTime = 0;
		m_nVideoType = EB_VIDEO_UNKNOWN;
		m_sChatId = 0;
		//m_sOldCallId = 0;
		m_nCC = 0;
		m_bSendCallRecord = false;
		m_nCallTime = 0;
		m_bAutoCallAccept = true;
		m_tLastTime = 0;
		m_nCallData = 0;
#ifdef USES_VIDEOROOM
		m_tP2POkTime = 0;
		memset(&m_pAudioStreamData,0,sizeof(m_pAudioStreamData));
#endif
		m_tCmTime = 0;
	}
	virtual ~CEBCallInfo(void)
	{
		//printf("**** virtual ~CEBCallInfo(void)...\n");

#ifdef USES_REMOTE_DESKTOP
#ifdef USES_RD_COM
		m_pEBRemoteDesktop.Release();
#else
		m_pEBRemoteDesktop.reset();
#endif
#endif
#ifdef USES_VIDEOROOM
		reset_video(false);
#endif
#ifdef USES_CHATROOM
		m_pChatRoom.reset();
#endif
		m_pFromAccount.reset();
		m_pCallUserList.clear(false);
		m_pEnterUserList.clear(false);
		m_pCall2GroupUserIdList.clear(false);
		m_pRDUserIdList.clear(false);
		m_pOnRDUserList.clear(false);
		m_pVideoUserIdList.clear(false);
		m_pResCmInfo.clear(false);
		m_pOfflineFile.clear(false);
		m_pOnVideoUserList.clear(false);
		//printf("**** virtual ~CEBCallInfo(void)ok\n");
	}
	const CEBCallInfo& operator =(const CEBCallInfo& p) {return this->operator = (&p);}
	const CEBCallInfo& operator =(const CEBCallInfo::pointer& p) {return this->operator = (p.get());}
	const CEBCallInfo& operator =(const CEBCallInfo* p)
	{
		if (p!=NULL)
		{
			EB_CallInfo::operator =(p);
			if (m_pFromAccount.get()==NULL)
			{
				CEBAccountInfo::pointer pOldFromAccount = p->GetFromAccountInfo();
				if (pOldFromAccount.get()!=NULL)
				{
					if (m_pFromAccount.get()==NULL)
						m_pFromAccount = CEBAccountInfo::pointer(new CEBAccountInfo());
					m_pFromAccount->operator = (pOldFromAccount.get());
				}else// if (m_pFromAccount.get()==NULL)
				{
					m_pFromAccount = CEBAccountInfo::create(m_nFromUserId,m_sFromAccount);
				}
			}
			//m_sOldCallId = p->m_sOldCallId;
			//m_nFromUserId = p->m_nFromUserId;
			//m_sFromAccount = p->m_sFromAccount;

			//m_pCallUserList = p->m_pCallUserList;	// ***
			m_pEnterUserList = p->m_pEnterUserList;
			m_sInGroupCode = p->m_sInGroupCode;
			m_sCall2GroupFromUserId = p->m_sCall2GroupFromUserId;
			m_tCall2GroupTime = p->m_tCall2GroupTime;
			m_pCall2GroupUserIdList = p->m_pCall2GroupUserIdList;

			m_nRemoteDesktopParam = p->m_nRemoteDesktopParam;
			m_nRemoteDesktopSID = p->m_nRemoteDesktopSID;
			m_tRDServerTime = p->m_tRDServerTime;
			m_sRDServer = p->m_sRDServer;
			m_nRDType = p->m_nRDType;
			m_pRDUserIdList = p->m_pRDUserIdList;
			m_pOnRDUserList = p->m_pOnRDUserList;

			m_sVideoSID = p->m_sVideoSID;
			m_nVideoParam = p->m_nVideoParam;
			m_tVSLastTime = p->m_tVSLastTime;
			m_sVideoServer = p->m_sVideoServer;
			m_nVideoType = p->m_nVideoType;
			m_pVideoUserIdList = p->m_pVideoUserIdList;
			m_pResCmInfo = p->m_pResCmInfo;
			m_pOfflineFile = p->m_pOfflineFile;
			m_pOnVideoUserList = p->m_pOnVideoUserList;
			m_sUmKey = p->m_sUmKey;
			m_sChatId = p->m_sChatId;
			m_sChatKey = p->m_sChatKey;
			m_sCmServer = p->m_sCmServer;
			m_tCmTime = p->m_tCmTime;
			m_sCmHttpServer = p->m_sCmHttpServer;
			m_sCmAppName = p->m_sCmAppName;
			m_sUmServer = p->m_sUmServer;
			m_sUmAppName = p->m_sUmAppName;
			m_bOffLineCall = p->m_bOffLineCall;
			m_nCallState = p->m_nCallState;
			m_tCallConnection = p->m_tCallConnection;
			m_nCmServerState = p->m_nCmServerState;
			m_nCC = p->m_nCC;
			m_bSendCallRecord = p->m_bSendCallRecord;
			m_nCallTime = p->m_nCallTime;
			m_bAutoCallAccept = p->m_bAutoCallAccept;
			m_tLastTime = p->m_tLastTime;
			m_nCallData = p->m_nCallData;
		}
		return *this;
	}
	//virtual cgcObject::pointer copyNew(void) const
	//{
	//	CEBAccountInfo::pointer pNewFromAccount = CEBAccountInfo::create(1,"");
	//	//CEBAccountInfo::pointer pNewFromAccount = CEBAccountInfo::create(m_nFromUserId,m_sFromAccount);
	//	//CEBAccountInfo::pointer pNewFromAccount = m_pFromAccount.get()==NULL?CEBAccountInfo::create(m_nFromUserId,m_sFromAccount):CGC_OBJECT_CAST<CEBAccountInfo>(m_pFromAccount->copyNew());
	//	CEBCallInfo::pointer result = CEBCallInfo::create(2,3,pNewFromAccount);
	//	//CEBCallInfo::pointer result = CEBCallInfo::create(m_sCallId,m_sGroupCode,pNewFromAccount);
	//	//result->m_sOldCallId = m_sOldCallId;
	//	//result->m_nFromUserId = m_nFromUserId;
	//	//result->m_sFromAccount = m_sFromAccount;

	//	//// 
	//	//result->m_pCallUserList = m_pCallUserList;
	//	//result->m_pEnterUserList = m_pEnterUserList;
	//	//result->m_sInGroupCode = m_sInGroupCode;
	//	//result->m_sCall2GroupFromUserId = m_sCall2GroupFromUserId;
	//	//result->m_tCall2GroupTime = m_tCall2GroupTime;
	//	//result->m_pCall2GroupUserIdList = m_pCall2GroupUserIdList;

	//	//result->m_nRemoteDesktopParam = m_nRemoteDesktopParam;
	//	//result->m_nRemoteDesktopSID = m_nRemoteDesktopSID;
	//	//result->m_tRDServerTime = m_tRDServerTime;
	//	//result->m_sRDServer = m_sRDServer;
	//	//result->m_nRDType = m_nRDType;
	//	//result->m_pRDUserIdList = m_pRDUserIdList;
	//	//result->m_pOnRDUserList = m_pOnRDUserList;

	//	//result->m_sVideoSID = m_sVideoSID;
	//	//result->m_nVideoParam = m_nVideoParam;
	//	//result->m_tVSLastTime = m_tVSLastTime;
	//	//result->m_sVideoServer = m_sVideoServer;
	//	//result->m_nVideoType = m_nVideoType;
	//	//result->m_pVideoUserIdList = m_pVideoUserIdList;
	//	//result->m_pResCmInfo = m_pResCmInfo;
	//	//result->m_pOfflineFile = m_pOfflineFile;
	//	//result->m_pOnVideoUserList = m_pOnVideoUserList;
	//	//result->m_sUmKey = m_sUmKey;
	//	//result->m_sChatId = m_sChatId;
	//	//result->m_sChatKey = m_sChatKey;
	//	//result->m_sCmServer = m_sCmServer;
	//	//result->m_tCmTime = m_tCmTime;
	//	//result->m_sCmHttpServer = m_sCmHttpServer;
	//	//result->m_sCmAppName = m_sCmAppName;
	//	//result->m_sUmServer = m_sUmServer;
	//	//result->m_sUmAppName = m_sUmAppName;
	//	//result->m_bOffLineCall = m_bOffLineCall;
	//	//result->m_nCallState = m_nCallState;
	//	//result->m_tCallConnection = m_tCallConnection;
	//	//result->m_nCmServerState = m_nCmServerState;
	//	//result->m_nCC = m_nCC;
	//	//result->m_bSendCallRecord = m_bSendCallRecord;
	//	//result->m_nCallTime = m_nCallTime;
	//	//result->m_bAutoCallAccept = m_bAutoCallAccept;
	//	//result->m_tLastTime = m_tLastTime;
	//	//result->m_nCallData = m_nCallData;
	//	return result;
	//}

	void SetFromAccountInfo(const CEBAccountInfo::pointer& pFromAccount)
	{
		m_pFromAccount = pFromAccount;
		if (m_pFromAccount.get()!=NULL)
		{
			this->SetFromUserId(m_pFromAccount->GetUserId());
			this->SetFromAccount(m_pFromAccount->GetAccount());
		}
	}
	CEBAccountInfo::pointer GetFromAccountInfo(void) const {return m_pFromAccount;}
	CLockMap<mycp::bigint, CEBAccountInfo::pointer>	m_pCallUserList;	// account->
	CLockMap<mycp::bigint, bool> m_pEnterUserList;						// uid->
	eb::bigint m_sInGroupCode;
	eb::bigint m_sCall2GroupFromUserId;
	time_t m_tCall2GroupTime;
	CLockList<mycp::bigint> m_pCall2GroupUserIdList;

	mycp::bigint m_nRemoteDesktopParam;
	int m_nRemoteDesktopSID;
	time_t m_tRDServerTime;
	tstring m_sRDServer;	// 远程桌面
	EB_REMOTE_DESKTOP_TYPE m_nRDType;
	CLockMap<mycp::bigint,bool> m_pRDUserIdList;				// uid->
	CLockMap<mycp::bigint,bool> m_pOnRDUserList;				// account->

	int m_sVideoSID;	// 视频通讯
	mycp::bigint m_nVideoParam;
	time_t m_tVSLastTime;
	tstring m_sVideoServer;	// 视频通讯
	// 1:audio 2:audio&video
	EB_VIDEO_TYPE m_nVideoType;
	CLockMap<mycp::bigint,bool> m_pVideoUserIdList;				// uid->
	CLockMap<mycp::bigint,tstring> m_pResCmInfo;				// msgid->resid,cm_server,cm_app_name
	CLockMap<mycp::bigint,bool> m_pOfflineFile;					// msgid->
	CLockMap<mycp::bigint,bool> m_pOnVideoUserList;				// account->
	tstring m_sUmKey;
	mycp::bigint m_sChatId;
	tstring m_sChatKey;
	tstring m_sCmServer;
	time_t m_tCmTime;
	tstring m_sCmHttpServer;
	tstring m_sCmAppName;
	tstring m_sUmServer;
	tstring m_sUmAppName;
	//mycp::bigint m_sOldCallId;	// 用于合并会话
	bool m_bOffLineCall;
	EB_CALL_STATE m_nCallState;
	time_t		m_tCallConnection;
	//EB_SERVER_STATE m_nUmServerState;
	EB_SERVER_STATE	m_nCmServerState;
	int m_nCC;					// 会话转发次数
	bool m_bSendCallRecord;		// 是否已经发送呼叫记录
	time_t m_nCallTime;
	bool m_bAutoCallAccept;		// for group_id==0
#ifdef USES_REMOTE_DESKTOP
#ifdef USES_RD_COM
	CComPtr<IEBRemoteDesktop>	m_pEBRemoteDesktop;
#else
	Cebrd::pointer				m_pEBRemoteDesktop;
#endif
#endif

	time_t m_tLastTime;
	unsigned int m_nCallData;
#ifdef USES_CHATROOM
	Cchatroom::pointer m_pChatRoom;
	time_t m_tP2POkTime;
#endif
#ifdef USES_VIDEOROOM
	void reset_video(bool lock = true)
	{
		if (lock)
		{
			BoostWriteLock wtLock(m_mutexVideoRoom);
			if (m_pVideoRoom.get()!=NULL)
			{
				m_pVideoRoom->Stop();
				m_pVideoRoom.reset();
			}
		}else if (m_pVideoRoom.get()!=NULL)
		{
			m_pVideoRoom->Stop();
			m_pVideoRoom.reset();
		}
		m_pDestUidVideo.clear();
		m_nLocalOnVideo = false;
		m_pVideoUserParam.clear();
		memset(&m_pAudioStreamData,0,sizeof(m_pAudioStreamData));
	}
	boost::shared_mutex m_mutexVideoRoom; 
	Cebmm::pointer m_pVideoRoom;
	bool m_nLocalOnVideo;
#ifdef _QT_MAKE_
        CLockMap<eb::bigint, QWidget*>	m_pDestUidVideo;		// uid->
#else
	CLockMap<eb::bigint, HWND>	m_pDestUidVideo;		// uid->
#endif
	tag_VideoStreamData m_pAudioStreamData;
	CLockMap<eb::bigint, void*>	m_pVideoUserParam;		// uid->
#endif
private:
	CEBAccountInfo::pointer	m_pFromAccount;
};
const CEBCallInfo::pointer NullCallInfo;

typedef enum EB_CHAT_INFO_EXT_DATA
{
	EB_CHAT_INFO_EXT_DATA_FORBID_SPEECH				= 0x01	// * 禁言
}EB_CHAT_INFO_EXT_DATA;

class CEBChatInfo
	: public cgcObject
	, public boost::enable_shared_from_this<CEBChatInfo>
{
public:
	typedef boost::shared_ptr<CEBChatInfo> pointer;
	static CEBChatInfo::pointer create(void)
	{
		return CEBChatInfo::pointer(new CEBChatInfo());
	}
	static CEBChatInfo::pointer create(const CEBChatInfo* p)
	{
		return CEBChatInfo::pointer(new CEBChatInfo(p));
	}
	static CEBChatInfo::pointer create(mycp::bigint sChatId, const tstring & sChatKey)
	{
		return CEBChatInfo::pointer(new CEBChatInfo(sChatId, sChatKey));
	}

	CEBChatInfo(void)
		: m_tSaveChatUserInfoTime(0)
		, m_bAccountChat(false),m_bAppPushChat(false)
		, m_sGroupCode(0)
		, m_bSaveChatInfo(false),m_bAutoReplyTest(false)
		, m_sCallId(0), m_sChatId(0)
		, m_tSendUmCallTime(0)
		, m_nMsgIndex(0)
		, m_nGroupVer(0)
		, m_nRemoteDesktopParam(0)
		, m_nExtData(0)
	{
	}
	CEBChatInfo(const CEBChatInfo* p)
		: m_tSaveChatUserInfoTime(0)
		, m_bAccountChat(false),m_bAppPushChat(false)
		, m_sGroupCode(0)
		, m_bSaveChatInfo(false),m_bAutoReplyTest(false)
		, m_sCallId(0), m_sChatId(0)
		, m_tSendUmCallTime(0)
		, m_nMsgIndex(0)
		, m_nGroupVer(0)
		, m_nRemoteDesktopParam(0)
		, m_nExtData(0)
	{
		this->operator = (p);
	}
	CEBChatInfo(mycp::bigint sChatId, const tstring & sChatKey)
		: m_tSaveChatUserInfoTime(0)
		, m_bAccountChat(false),m_bAppPushChat(false)
		, m_sGroupCode(0)
		, m_bSaveChatInfo(false),m_bAutoReplyTest(false)
		, m_sChatId(sChatId)
		, m_sChatKey(sChatKey)
		, m_tSendUmCallTime(0)
		, m_nMsgIndex(0)
		, m_nGroupVer(0)
		, m_nRemoteDesktopParam(0)
		, m_nExtData(0)
	{
	}
	virtual ~CEBChatInfo(void)
	{
		//printf("*** ~CEBChatInfo()\n");
		m_pChatUserList.clear(false);
		m_pChatMsgList.clear(false);
	}
	void UpdateOwnerChatInfo(void)
	{
		CEBChatInfo::pointer pOwner = shared_from_this();
		BoostReadLock rdlock(const_cast<boost::shared_mutex&>(m_pChatMsgList.mutex()));
		CLockMap<mycp::bigint, CChatMsgInfo::pointer>::const_iterator pIter = m_pChatMsgList.begin();
		for (; pIter!=m_pChatMsgList.end(); pIter++)
		{
			const CChatMsgInfo::pointer pOldChatMsgInfo = pIter->second;
			pOldChatMsgInfo->SetChatInfo(pOwner);
		}
	}
	const CEBChatInfo& operator = (const CEBChatInfo& p) {return this->operator = (&p);}
	const CEBChatInfo& operator = (const CEBChatInfo::pointer& p) {return this->operator = (p.get());}
	const CEBChatInfo& operator = (const CEBChatInfo* p)
	{
		if (p!=NULL)
		{
			{
				//printf("**** m_pChatUserList start size=%d...\n",(int)p->m_pChatUserList.size());
				BoostReadLock rdlock(const_cast<boost::shared_mutex&>(p->m_pChatUserList.mutex()));
				CLockMap<mycp::bigint, CEBAccountInfo::pointer>::const_iterator pIter = p->m_pChatUserList.begin();
				for (; pIter!=p->m_pChatUserList.end(); pIter++)
				{
					const CEBAccountInfo::pointer pOldAccountInfo = pIter->second;
					//printf("**** pOldAccountInfo=%x\n",(int)pOldAccountInfo.get());
					const CEBAccountInfo::pointer pNewAccountInfo = CEBAccountInfo::create(pOldAccountInfo);
					//printf("**** pNewAccountInfo=%x\n",(int)pNewAccountInfo.get());
					m_pChatUserList.insert(pIter->first, pNewAccountInfo,false);
				}
				//printf("**** m_pChatUserList end ok, size=%d\n",(int)m_pChatUserList.size());
			}
			{
				//printf("**** m_pChatMsgList start size=%d...\n",(int)p->m_pChatMsgList.size());
				BoostReadLock rdlock(const_cast<boost::shared_mutex&>(p->m_pChatMsgList.mutex()));
				CLockMap<mycp::bigint, CChatMsgInfo::pointer>::const_iterator pIter = p->m_pChatMsgList.begin();
				for (; pIter!=p->m_pChatMsgList.end(); pIter++)
				{
					const CChatMsgInfo::pointer pOldChatMsgInfo = pIter->second;
					//printf("**** pOldChatMsgInfo=%x\n",(int)pOldChatMsgInfo.get());
					CChatMsgInfo::pointer pNewChatMsgInfo = CChatMsgInfo::create(pOldChatMsgInfo.get());
					//printf("**** pNewChatMsgInfo=%x\n",(int)pNewChatMsgInfo.get());
					//pNewChatMsgInfo->SetChatInfo(shared_from_this());	// ***这里死锁
					//printf("**** SetChatInfo ok\n");
					CEBAccountInfo::pointer pOldFromAccount = CGC_OBJECT_CAST<CEBAccountInfo>(pOldChatMsgInfo->GetFromAccount());
					CEBAccountInfo::pointer pNewFromAccount;
					if (!m_pChatUserList.find(pOldFromAccount->GetUserId(),pNewFromAccount))
						pNewFromAccount = CEBAccountInfo::create(pOldFromAccount);
					//printf("**** pNewFromAccount=%x\n",(int)pNewFromAccount.get());
					pNewChatMsgInfo->SetFromAccount(pNewFromAccount);
					m_pChatMsgList.insert(pIter->first, pNewChatMsgInfo,false);
				}
				//printf("**** m_pChatMsgList end ok, size=%d\n",(int)m_pChatMsgList.size());
			}

			m_pSaveChatUserInfoTime = p->m_pSaveChatUserInfoTime;
			m_tSaveChatUserInfoTime = p->m_tSaveChatUserInfoTime;

			m_bAccountChat = p->m_bAccountChat;
			m_bAppPushChat = p->m_bAppPushChat;
			m_sGroupCode = p->m_sGroupCode;
			m_bSaveChatInfo = p->m_bSaveChatInfo;
			m_bAutoReplyTest = p->m_bAutoReplyTest;

			m_sUmServer = p->m_sUmServer;
			m_sUmAppName = p->m_sUmAppName;
			m_tSendUmCallTime = p->m_tSendUmCallTime;
			m_nMsgIndex = p->m_nMsgIndex;
			m_nGroupVer = p->m_nGroupVer;
			m_nRemoteDesktopParam = p->m_nRemoteDesktopParam;
			m_nExtData = p->m_nExtData;
			m_sCallId = p->m_sCallId;
			m_sChatId = p->m_sChatId;
			m_sChatKey = p->m_sChatKey;

		}
		return *this;
	}
	virtual cgcObject::pointer copyNew(void) const
	{
		CEBChatInfo::pointer result = CEBChatInfo::create();
		// ???
		return result;
	}

	void SetCallId(mycp::bigint value) {m_sCallId = value;}
	mycp::bigint GetCallId(void) const {return m_sCallId;}

	void SetChatId(mycp::bigint value) {m_sChatId = value;}
	mycp::bigint GetChatId(void) const {return m_sChatId;}
	void SetChatKey(const tstring& value) {m_sChatKey = value;}
	const tstring& GetChatKey(void) const {return m_sChatKey;}
	void reset(void)
	{
		m_bAccountChat = false;
		m_bAppPushChat = false;
		m_sGroupCode = 0;
		m_nRemoteDesktopParam = 0;
		m_nExtData = 0;
		m_sCallId = 0;
		m_sChatId = 0;
		m_sChatKey.clear();
		m_pChatUserList.clear();
		m_pChatMsgList.clear();
		m_tSendUmCallTime = 0;
	}
	CLockMap<eb::bigint, CEBAccountInfo::pointer>	m_pChatUserList;	// uid->
	CLockMap<eb::bigint, CChatMsgInfo::pointer>	m_pChatMsgList;		// msg-id->
	//CLockMap<eb::bigint, bool>	m_pChatMsgOkList;				// msg-id->
	CLockMap<eb::bigint, time_t>	m_pSaveChatUserInfoTime;	// uid->
	time_t m_tSaveChatUserInfoTime;

	bool m_bAccountChat;		// 用户私人资源会话
	bool m_bAppPushChat;		// 用户私人资源会话
	mycp::bigint m_sGroupCode;		// 不为空，表示部门群组聊天
	//tstring m_sVideoSID;
	//tstring m_sVideoServer;
	bool m_bSaveChatInfo;	// 是否已经保存聊天信息
	bool m_bAutoReplyTest;	// 自动回复测试；

	tstring m_sUmServer;
	tstring m_sUmAppName;
	time_t	m_tSendUmCallTime;
	unsigned int m_nMsgIndex;
	mycp::bigint m_nGroupVer;
	mycp::bigint m_nRemoteDesktopParam;
	int m_nExtData;
private:
	mycp::bigint				m_sCallId;
	mycp::bigint				m_sChatId;
	tstring				m_sChatKey;
};
const CEBChatInfo::pointer NullChatInfo;

class CEBPushTaskInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CEBPushTaskInfo> pointer;
	static CEBPushTaskInfo::pointer create(void)
	{
		return CEBPushTaskInfo::pointer(new CEBPushTaskInfo());
	}
	static CEBPushTaskInfo::pointer create(mycp::bigint sPushTaskId, mycp::bigint nAcmKey, const tstring & sTaskKey,mycp::bigint nFromIp)
	{
		return CEBPushTaskInfo::pointer(new CEBPushTaskInfo(sPushTaskId,nAcmKey,sTaskKey,nFromIp));
	}

	CEBPushTaskInfo(void)
		: m_sPushTaskId(0),m_sAcmKey(0),m_nType(0),m_sFromIp(0)
		, m_nTaskAmount(0),m_nPushAmount(0),m_nPushCount(0),m_tLastTime(0)
	{
	}
	CEBPushTaskInfo(mycp::bigint sPushTaskId, mycp::bigint nAcmKey, const tstring & sTaskKey,mycp::bigint nFromIp)
		: m_sPushTaskId(sPushTaskId),m_sAcmKey(nAcmKey),m_sTaskKey(sTaskKey),m_nType(0),m_sFromIp(nFromIp)
		, m_nTaskAmount(0),m_nPushAmount(0),m_nPushCount(0),m_tLastTime(0)
	{
	}
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}

	mycp::bigint GetPushTaskId(void) const {return m_sPushTaskId;}
	void SetAcmKey(mycp::bigint v) {m_sAcmKey = v;}
	mycp::bigint GetAcmKey(void) const {return m_sAcmKey;}
	void SetTaskKey(const tstring& value) {m_sTaskKey = value;}
	const tstring& GetTaskKey(void) const {return m_sTaskKey;}
	void SetType(int nType) {m_nType = nType;}
	int GetType(void) const {return m_nType;}
	void SetFromIp(mycp::bigint v) {m_sFromIp = v;}
	mycp::bigint GetFromIp(void) const {return m_sFromIp;}
	void SetTaskAmount(mycp::bigint v) {m_nTaskAmount = v;}
	mycp::bigint GetTaskAmount(void) const {return m_nTaskAmount;}
	void SetPushAmount(mycp::bigint v) {m_nPushAmount = v;}
	mycp::bigint GetPushAmount(void) const {return m_nPushAmount;}
	unsigned int GetPushCount(void) {return m_nPushCount;}
	unsigned int IncreasePushCount(void) {return (++m_nPushCount);}
	void ResetPushCount(void) {m_nPushCount = 0;}
	void SetLastTime(time_t v) {m_tLastTime = v;}
	time_t GetLastTime(void) const {return m_tLastTime;}

private:
	mycp::bigint		m_sPushTaskId;
	mycp::bigint		m_sAcmKey;
	tstring		m_sTaskKey;
	mycp::bigint		m_sFromIp;
	int				m_nType;
	mycp::bigint		m_nTaskAmount;	// 任务多少
	mycp::bigint		m_nPushAmount;	// 全部已经推多少个
	unsigned int	m_nPushCount;	// 当前推送多少个
	time_t			m_tLastTime;
};
const CEBPushTaskInfo::pointer NullPushTaskInfo;

class CSendOkMsgInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CSendOkMsgInfo> pointer;
	static CSendOkMsgInfo::pointer create(const CEBChatInfo::pointer& pChatInfo,const CChatMsgInfo::pointer& pChatMsgInfo)
	{
		return CSendOkMsgInfo::pointer(new CSendOkMsgInfo(pChatInfo, pChatMsgInfo));
	}

	CSendOkMsgInfo(const CEBChatInfo::pointer& pChatInfo,const CChatMsgInfo::pointer& pChatMsgInfo)
		: m_pChatInfo(pChatInfo)
		, m_pChatMsgInfo(pChatMsgInfo)
	{
		m_tSentOkTime = time(0);
	}
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}

	const CEBChatInfo::pointer& GetChatInfo(void) const {return m_pChatInfo;}
	const CChatMsgInfo::pointer& GetChatMsgInfo(void) const {return m_pChatMsgInfo;}
	void SetSendOkTime(time_t tNew) {m_tSentOkTime = tNew;}
	time_t GetSendOkTime(void) const {return m_tSentOkTime;}
	
private:
	CEBChatInfo::pointer m_pChatInfo;
	CChatMsgInfo::pointer m_pChatMsgInfo;
	time_t m_tSentOkTime;
};

class CAppMsgInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CAppMsgInfo> pointer;
	static CAppMsgInfo::pointer create(eb::bigint sAppId,eb::bigint sMsgId,mycp::bigint nMsgType)
	{
		return CAppMsgInfo::pointer(new CAppMsgInfo(sAppId,sMsgId,nMsgType));
	}

	const eb::bigint GetAppId(void) const {return m_sAppId;}
	void SetFromServiceId(const tstring& v) {m_sFromSid=v;}
	const tstring& GetFromServiceId(void) const {return m_sFromSid;}
	void SetAccount(const tstring& v) {m_sAccount=v;}
	const tstring& GetAccount(void) const {return m_sAccount;}
	void SetMsgId(eb::bigint nMsgId) {m_sMsgId = nMsgId;}
	eb::bigint GetMsgId(void) const {return m_sMsgId;}
	eb::bigint GetMsgType(void) const {return m_nMsgType;}
	void SetMsgName(const tstring& v) {m_sMsgName=v;}
	const tstring& GetMsgName(void) const {return m_sMsgName;}
	void SetMsgContent(const tstring& v) {m_sMsgContent=v;}
	const tstring& GetMsgContent(void) const {return m_sMsgContent;}
	void SetExtData(eb::bigint v) {m_nExtData = v;}
	eb::bigint GetExtData(void) const {return m_nExtData;}

	void SetTime(time_t v) {m_tTime=v;}
	time_t GetTime(void) const {return m_tTime;}
	void SetResend(int v) {m_nResend=v;}
	int GetResend(void) const {return m_nResend;}

	CAppMsgInfo(void)
		: m_sAppId(0),m_sMsgId(0)
		, m_nMsgType(0), m_nExtData(0)
		, m_tTime(0)
		, m_nResend(0)
	{
	}
	CAppMsgInfo(eb::bigint sAppId,eb::bigint sMsgId,eb::bigint nMsgType)
		: m_sAppId(sAppId),m_sMsgId(sMsgId),m_nMsgType(nMsgType), m_nExtData(0)
		, m_nResend(0)
	{
		m_tTime = time(0);
	}
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}

private:
	eb::bigint m_sAppId;
	tstring m_sFromSid;
	tstring m_sAccount;
	eb::bigint m_sMsgId;
	eb::bigint m_nMsgType;
	tstring m_sMsgName;
	tstring m_sMsgContent;
	eb::bigint m_nExtData;
	time_t m_tTime;
	int m_nResend;
};

class CEBSysMsgInfo
	: public CEBAppMsgInfo
	//, public cgcObject
{
public:
	typedef boost::shared_ptr<CEBSysMsgInfo> pointer;
	static CEBSysMsgInfo::pointer create(void)
	{
		return CEBSysMsgInfo::pointer(new CEBSysMsgInfo());
	}
	static CEBSysMsgInfo::pointer create(EB_MSG_TYPE nMsgType, const tstring& sMsgContent="")
	{
		return CEBSysMsgInfo::pointer(new CEBSysMsgInfo(nMsgType,sMsgContent));
	}

	CEBSysMsgInfo(EB_MSG_TYPE nMsgType, const tstring& sMsgContent)
		: CEBAppMsgInfo(0,nMsgType,sMsgContent)
		, m_nEntId(0), m_bSendFromUserId(false)
		, m_bIncreaseUserGroupInfoVer(false), m_bIncludeFromUserIdIncreaseUserGroupInfoVer(false)
	{
	}
	CEBSysMsgInfo(void)
		: CEBAppMsgInfo()
		, m_nEntId(0), m_bSendFromUserId(false)
		, m_bIncreaseUserGroupInfoVer(false), m_bIncludeFromUserIdIncreaseUserGroupInfoVer(false)
	{
	}
	//CEBAccountInfo::pointer m_pFromAccountInfo;
	mycp::bigint m_nEntId;
	bool m_bSendFromUserId;
	bool m_bIncreaseUserGroupInfoVer;
	bool m_bIncludeFromUserIdIncreaseUserGroupInfoVer;
	CLockMap<mycp::bigint,bool> m_pExceptUserIdList;
};

class CApplicationInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CApplicationInfo> pointer;
	static CApplicationInfo::pointer create(eb::bigint sDevId,eb::bigint sAppId,const tstring & sOnlineKey)
	{
		return CApplicationInfo::pointer(new CApplicationInfo(sDevId,sAppId,sOnlineKey));
	}
	static CApplicationInfo::pointer create(const CApplicationInfo::pointer& p)
	{
		return CApplicationInfo::pointer(new CApplicationInfo(p));
	}
	static CApplicationInfo::pointer create(void)
	{
		return CApplicationInfo::pointer(new CApplicationInfo());
	}

	eb::bigint GetDevId(void) const {return m_sDevId;}
	const eb::bigint GetAppId(void) const {return m_sAppId;}
	void SetOnlineKey(const tstring & value) {m_sOnlineKey = value;}
	const tstring & GetOnlineKey(void) const {return m_sOnlineKey;}
	void SetOnlineTime(time_t value) {m_tOnlineTime = value;}
	time_t GetOnlineTime(void) const {return m_tOnlineTime;}

	void SetSotpSessionId(const tstring& v) {m_sSotpSessionId=v;}
	const tstring& GetSotpSessionId(void) const {return m_sSotpSessionId;}

public:
	CApplicationInfo(void)
		: m_sDevId(0), m_sAppId(0)
		, m_tOnlineTime(0)
	{
	}
	CApplicationInfo(const CApplicationInfo* p)
		: m_sDevId(0), m_sAppId(0)
		, m_tOnlineTime(0)
	{
		this->operator = (p);
	}
	CApplicationInfo(const CApplicationInfo& p)
		: m_sDevId(0), m_sAppId(0)
		, m_tOnlineTime(0)
	{
		this->operator = (p);
	}
	CApplicationInfo(const CApplicationInfo::pointer& p)
		: m_sDevId(0), m_sAppId(0)
		, m_tOnlineTime(0)
	{
		this->operator = (p);
	}
	CApplicationInfo(eb::bigint sDevId,eb::bigint sAppId,const tstring & sOnlineKey)
		: m_sDevId(sDevId)
		, m_sAppId(sAppId)
		, m_sOnlineKey(sOnlineKey)
	{
		m_tOnlineTime = time(0);
	}
	virtual ~CApplicationInfo(void)
	{
	}
	const CApplicationInfo& operator = (const CApplicationInfo& p) {return this->operator =(&p);}
	const CApplicationInfo& operator = (const CApplicationInfo::pointer& p) {return this->operator =(p.get());}
	const CApplicationInfo& operator = (const CApplicationInfo* p)
	{
		if (p!=NULL)
		{
			m_sDevId = p->m_sDevId;
			m_sAppId = p->m_sAppId;
			m_sOnlineKey = p->m_sOnlineKey;
			m_tOnlineTime = p->m_tOnlineTime;
			m_sSotpSessionId = p->m_sSotpSessionId;
		}
		return *this;
	}
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}

private:
	eb::bigint		m_sDevId;
	eb::bigint		m_sAppId;
	tstring		m_sOnlineKey;
	time_t			m_tOnlineTime;
	tstring		m_sSotpSessionId;
};

class CServiceInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CServiceInfo> pointer;
	static CServiceInfo::pointer create(int sServiceId,const tstring & sName, const tstring & sAppName, const tstring & sOnlineKey, EB_SERVER_TYPE nServiceType)
	{
		return CServiceInfo::pointer(new CServiceInfo(sServiceId,sName, sAppName, sOnlineKey, nServiceType));
	}

	int GetServiceId(void) const {return m_sServiceId;}
	void SetName(const tstring & value) {m_sName = value;}
	const tstring & GetName(void) const {return m_sName;}
	void SetAppName(const tstring & value) {m_sAppName = value;}
	const tstring & GetAppName(void) const {return m_sAppName;}
	void SetOnlineKey(const tstring & value) {m_sOnlineKey = value;}
	const tstring & GetOnlineKey(void) const {return m_sOnlineKey;}
	void SetOnlineTime(time_t value) {m_tOnlineTime = value;}
	time_t GetOnlineTime(void) const {return m_tOnlineTime;}
	void SetServiceType(EB_SERVER_TYPE value) {m_nServiceType = value;}
	EB_SERVER_TYPE GetServiceType(void) const {return m_nServiceType;}
	void SetAddress(const tstring & value) {
		m_sAddress = value;
		const std::string::size_type find = m_sAddress.find(":");
		if (find!=std::string::npos)
		{
			m_sIp = m_sAddress.substr(0,find);
			m_nPort = atoi(m_sAddress.substr(find+1).c_str());
		}else
		{
			m_sIp = m_sAddress;
			m_nPort = 0;
		}
	}
	void SetSubType(int value) {m_nSubType = value;}
	int GetSubType(void) const {return m_nSubType;}
	const tstring & GetAddress(void) const {return m_sAddress;}
	const tstring & GetIp(void) const {return m_sIp;}
	int GetPort(void) const {return m_nPort;}
	void SetHttpAddress(const tstring & value) {
		m_sHttpAddress = value;
		const std::string::size_type find = m_sHttpAddress.find(":");
		if (find!=std::string::npos)
		{
			m_sHttpIp = m_sHttpAddress.substr(0,find);
			m_nHttpPort = atoi(m_sHttpAddress.substr(find+1).c_str());
		}else
		{
			m_sHttpIp = m_sHttpAddress;
			m_nHttpPort = 0;
		}
	}
	const tstring & GetHttpAddress(void) const {return m_sHttpAddress;}
	const tstring & GetHttpIp(void) const {return m_sHttpIp;}
	int GetHttpPort(void) const {return m_nHttpPort;}
	void SetKey(const tstring & value) {m_sKey = value;}
	const tstring & GetKey(void) const {return m_sKey;}
	void SetMaxCapacity(unsigned int value) {m_nMaxCapacity = value;}
	unsigned int GetMaxCapacity(void) const {return m_nMaxCapacity;}
	void SetCurrentCapacity(unsigned int value) {m_nCurrentCapacity = value;}
	unsigned int GetCurrentCapacity(void) const {return m_nCurrentCapacity;}
	void CurrentCapacityDec(void) {if (m_nCurrentCapacity > 0) m_nCurrentCapacity--;}
	void CurrentCapacityInc(void) {m_nCurrentCapacity++;}

	void SetSotpSessionId(const tstring& v) {m_sSotpSessionId=v;}
	const tstring& GetSotpSessionId(void) const {return m_sSotpSessionId;}
	void SetLocalIp(bool value) {m_bLocalIp = value;}
	bool GetLocalIp(void) const {return m_bLocalIp;}
	//void SetUpdateSysInfo(time_t value) {m_tUpdateSysInfo = value;}
	//time_t GetUpdateSysInfo(void) const {return m_tUpdateSysInfo;}
	CLockMap<mycp::bigint,time_t> theEntGroupVersionChangeList;
	CLockMap<mycp::bigint,int> theEntGroupVersionChangeFinished;

	void SetPublicKey(const tstring& v) {m_sPublicKey = v;}
	const tstring& GetPublicKey(void) const {return m_sPublicKey;}
	CLockMap<int,time_t> m_pTimeList;	// type->

public:
	CServiceInfo(void)
		: m_sServiceId(0)
		, m_tOnlineTime(0)
		, m_nServiceType(EB_SERVER_TYPE_UNKNOWN),m_nSubType(0)
		, m_nPort(0),m_nHttpPort(0)
		, m_nMaxCapacity(0),m_nCurrentCapacity(0)
		, m_bLocalIp(false)
		//, m_tUpdateSysInfo(0)
	{
	}
	CServiceInfo(int sServiceId,const tstring & sName, const tstring & sAppName, const tstring & sOnlineKey, EB_SERVER_TYPE nServiceType)
		: m_sServiceId(sServiceId),m_sName(sName)
		, m_sAppName(sAppName)
		, m_sOnlineKey(sOnlineKey)
		, m_nServiceType(nServiceType), m_nSubType(0)
		, m_nPort(0),m_nHttpPort(0)
		, m_nMaxCapacity(1000), m_nCurrentCapacity(0)
		, m_bLocalIp(false)
		//, m_tUpdateSysInfo(0)
	{
		m_tOnlineTime = time(0);
	}
	virtual ~CServiceInfo(void)
	{
	}
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}

private:
	int		m_sServiceId;
	tstring		m_sName;
	tstring		m_sAppName;
	tstring		m_sOnlineKey;
	time_t			m_tOnlineTime;
	EB_SERVER_TYPE	m_nServiceType;
	int				m_nSubType;
	tstring		m_sAddress;			// ip:port
	tstring		m_sIp;					// ip
	int				m_nPort;
	tstring		m_sHttpAddress;		// ip:port
	tstring		m_sHttpIp;				// ip
	int				m_nHttpPort;
	tstring		m_sKey;
	unsigned int	m_nMaxCapacity;
	unsigned int	m_nCurrentCapacity;
	tstring		m_sSotpSessionId;
	bool			m_bLocalIp;
	//time_t			m_tUpdateSysInfo;
	tstring		m_sPublicKey;
};

class CServiceInfoList
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CServiceInfoList> pointer;
	static CServiceInfoList::pointer create(void)
	{
		return CServiceInfoList::pointer(new CServiceInfoList());
	}
	virtual cgcObject::pointer copyNew(void) const {return cgcNullObject;}

	CLockMap<int, CServiceInfo::pointer>	m_pServiceInfoList;	// serviceid->
};

#define CALLBACK_ENC_3DES		"3des"
#define CALLBACK_ENC_AES_ECB	"aes_ecb"
#define CALLBACK_ENC_AES_CBC	"aes_cbc"

class CEBMailSettingInfo
{
public:
	typedef boost::shared_ptr<CEBMailSettingInfo> pointer;
	static CEBMailSettingInfo::pointer create(const tstring& sDomain, const tstring& sPop3Address, int nPop3Port, const tstring& sSmtpAddress, int nSmtpPort)
	{
		return CEBMailSettingInfo::pointer(new CEBMailSettingInfo(sDomain, sPop3Address, nPop3Port, sSmtpAddress, nSmtpPort));
	}
	static CEBMailSettingInfo::pointer create(void)
	{
		return CEBMailSettingInfo::pointer(new CEBMailSettingInfo());
	}
	tstring m_sDomain;				// entboost.com
	tstring m_sPop3Address;		// pop3.entboost.com,pop.entboost.com
	int m_nPop3Port;					// 110,995(ssl)
	tstring m_sSmtpAddress;		// smtp.entboost.com
	int m_nSmtpPort;					// 25,465(ssl)
	CEBMailSettingInfo(const tstring& sDomain, const tstring& sPop3Address, int nPop3Port, const tstring& sSmtpAddress, int nSmtpPort)
		: m_sDomain(sDomain), m_sPop3Address(sPop3Address), m_nPop3Port(nPop3Port), m_sSmtpAddress(sSmtpAddress), m_nSmtpPort(nSmtpPort)
	{
	}
	CEBMailSettingInfo(void)
		: m_nPop3Port(0), m_nSmtpPort(0)
	{
	}
};

class CEBSysInfo
{
public:
	static int ParseString(const char * lpszString, const char * lpszInterval, std::vector<tstring> & pOut)
	{
		const tstring sIn(lpszString);
		const size_t nInLen = sIn.size();
		const size_t nIntervalLen = strlen(lpszInterval);
		pOut.clear();
		std::string::size_type nFindBegin = 0;
		while (nFindBegin<nInLen)
		{
			std::string::size_type find = sIn.find(lpszInterval,nFindBegin);
			if (find == std::string::npos)
			{
				pOut.push_back(sIn.substr(nFindBegin));
				break;
			}
			if (find==nFindBegin)
			{
				pOut.push_back("");	// 空
			}else
			{
				pOut.push_back(sIn.substr(nFindBegin, (find-nFindBegin)));
			}
			nFindBegin = find+nIntervalLen;
		}
		return (int)pOut.size();
	}
	typedef enum OPEN_REGISTER_TYPE
	{
		OPEN_REGISTER_NONE
		, OPEN_REGISTER_USER	= 0x1
		, OPEN_REGISTER_ENT		= 0x2
	}OPEN_REGISTER_TYPE;
	typedef enum DEPLOY_TYPE
	{
		DEPLOY_TYPE_PRIVATE		// 企业私有云
		, DEPLOY_TYPE_PUBLIC	// 恩布公有云
	}DEPLOY_TYPE;
	typedef enum SAVE_CONVERSATIONS_TYPE
	{
		SAVE_CONVERSATIONS_NONE
		, SAVE_CONVERSATIONS_LOCAL	= 0x1	// 保存本机
		, SAVE_CONVERSATIONS_SERVER	= 0x2	// 保存服务
	}SAVE_CONVERSATIONS_TYPE;
	typedef enum CALLBACK_PARAM_TYPE
	{
		CALLBACK_PARAM_TYPE_REG_DATA			= 0x1
		, CALLBACK_PARAM_TYPE_CHANGE_PASSWD		= 0x2
		, CALLBACK_PARAM_TYPE_ON_OFF_LINE		= 0x4
	}CALLBACK_PARAM_TYPE;

	// 动态
	tstring m_sPrivatePwd;
	tstring m_sPrivateKey;
	tstring m_sPublicKey;

	tstring m_sDefaultGroupName0;
	tstring m_sDefaultGroupName2;
	tstring m_sDefaultGroupName9;
	mycp::bigint m_nSendCallRecordAppId;	// 发送呼叫记录；０不发送
	mycp::bigint m_nSendMailAppId;		// 为0，不需要发送验证邮件；
	tstring m_sDefaultPassword;			// 空使用随机密码，非空默认密码；
	tstring m_sFuncUrl;
	tstring m_sAuthMsgUrl;				// 验证信息URL地址
	tstring m_sForgetPwdUrl;			// 忘记密码URL地址
	tstring m_sEntLogoUrl;				// 企业LOGO
	int m_nOpenRegister;				// 开放注册
	bool m_bOpenVisitor;				// 开放游客
	int m_nAppTestAccountCount;			// 自动创建应用测试帐号数量；0-10
	tstring m_sAccountPrefix;			// 帐号前缀，如@
	//bool m_bUsesUserId;
	int m_nSupportVersion;				// 最低支持版本数；
	int m_nDBType;						// 数据库类型；0=pg；1=mysql；2=sqlite
	tstring m_sUserPwdCallbackUrl;		// 用户密码回调URL
	mycp::bigint m_nCallbackAppId;		// 回调APPID
	int m_nCallParam;
	tstring m_sCallbackEncMode;			// 回调数据加密方式，目前支持：3des,aes_ecb,aes_cbc
	int m_nMaxRetryCallbackData;		// 最大重试多少次，默认5次
	//mycp::bigint m_tLicenseStartTime;	// 开始授权时间
	DEPLOY_TYPE m_nDeployType;					// 部署类型：0私有云；1公有云
	mycp::bigint m_nDeployId;			// 部署ID（服务器ID）
	tstring m_sDeployKey;
	tstring m_sProductName;
	PASSWD_AUTH_MODE m_nPasswdAuthMode;		// 密码验证方式 0=默认恩布; 1=md5(password)禅道合作; 2=PBKDF2
	tstring m_sEntManagerUrl;			// 企业管理后台URL
	int m_nSaveConversations;			// 保存聊天记录 1：保存本机，2：保存在线 3:BOTH(default)
	//bool m_bInviteAdd2GroupAuth;
	//bool m_bReqAdd2GroupAuth;
	bool m_bGroupMemberChangeEvent;
	tstring m_sConversationsUrl;
	mycp::bigint m_nGroupMsgSubId;
	mycp::bigint m_nFindAppSubId;
	//mycp::bigint m_nMyCollectionSubId;
	bool m_bDefaultAllowOutChat;
	bool m_bDefaultAcceptOutChat;
	int m_nOffFileSpaceSize;
	int m_nOnlineMsgSaveDays;
	int m_nOfflineMsgSaveDays;
	int m_nOfflineFileSaveDays;
	int m_nUserCloudDriveSize;
	int m_nUserGroupCloudDriveSize;
	int m_nEntGroupCloudDriveSize;
	int m_nPushTime;
	typedef enum PUSH_CONTENT_TYPE
	{
		PUSH_CONTENT_TYPE_CHAT_CONTENT		// 聊天内容
		, PUSH_CONTENT_TYPE_NEWMSG_ONLY		// 只显示新消息
	}PUSH_CONTENT_TYPE;
	PUSH_CONTENT_TYPE m_nPushContentType;	// 推送消息类型 0:聊天内容 1:只显示新消息
	tstring m_sTextString;
	CLockMap<int,tstring> m_pTextList;	// TEXT_RESOURCE_TYPE->x
	//std::vector<tstring> m_pTextList;
	short m_nMaxRetryCount;
	int m_nTempLockMinute;
	typedef enum TEXT_RESOURCE_TYPE
	{
		TEXT_RESOURCE_EMOTION	= 0
		, TEXT_RESOURCE_PICTURE
		, TEXT_RESOURCE_VOICE
		, TEXT_RESOURCE_FILE
		, TEXT_RESOURCE_OFFFILE
		, TEXT_RESOURCE_CARDINFO
		, TEXT_RESOURCE_USER_ECARD
		, TEXT_RESOURCE_PHONE
		, TEXT_RESOURCE_TEL
		, TEXT_RESOURCE_FAX
		, TEXT_RESOURCE_URL
		, TEXT_RESOURCE_EMAIL
		, TEXT_RESOURCE_ENTERPRISE
		, TEXT_RESOURCE_ADDRESS
		, TEXT_RESOURCE_NEW_MSG
	}TEXT_RESOURCE_TYPE;
	tstring GetTextResourceString(TEXT_RESOURCE_TYPE nResourceType) const
	{
		tstring result;
		if (m_pTextList.find((int)nResourceType,result))
			return result;
		//if (m_pTextList.size()>(size_t)nResourceType)
		//	return m_pTextList[(size_t)nResourceType];
		switch(nResourceType)
		{
		case TEXT_RESOURCE_EMOTION:
			return "emotion";
		case TEXT_RESOURCE_PICTURE:
			return "picture";
		case TEXT_RESOURCE_VOICE:
			return "voice";
		case TEXT_RESOURCE_FILE:
			return "file";
		case TEXT_RESOURCE_OFFFILE:
			return "offline file";
		case TEXT_RESOURCE_CARDINFO:
			return "card info";
		case TEXT_RESOURCE_USER_ECARD:
			return "user ecard";
		case TEXT_RESOURCE_PHONE:
			return "phone";
		case TEXT_RESOURCE_TEL:
			return "tel";
		case TEXT_RESOURCE_FAX:
			return "fax";
		case TEXT_RESOURCE_URL:
			return "url";
		case TEXT_RESOURCE_EMAIL:
			return "email";
		case TEXT_RESOURCE_ENTERPRISE:
			return "enterprise";
		case TEXT_RESOURCE_ADDRESS:
			return "address";
		case TEXT_RESOURCE_NEW_MSG:
			return "new message";
		default:
			break;
		}
		return "";
	}
	typedef enum DELETE_ENT_MEMBER_OP
	{
		DELETE_ENT_MEMBER_OP_NULL	= 0
		, DELETE_ENT_MEMBER_OP_FREEZE
		, DELETE_ENT_MEMBER_OP_DELETE
	}DELETE_ENT_MEMBER_OP;
	DELETE_ENT_MEMBER_OP m_nDeleteEntMemberOp;

	int m_nSystemTempValue;
	typedef enum SYSTEM_TEMP_VALUE
	{
		SYSTEM_TEMP_VALUE_SET_GROUP_USERHEAD_DATA				= 0x1	// 已经同步头像数据
	}SYSTEM_TEMP_VALUE;
	int m_nSystemSetting;
	typedef enum SYSTEM_SETTING_VALUE
	{
		SYSTEM_SETTING_VALUE_AUTH_CONTACT					= 0x00001
		, SYSTEM_SETTING_VALUE_AUTH_INVITEADD2GROUP			= 0x00002
		, SYSTEM_SETTING_VALUE_SEND_REG_MAIL				= 0x00004
		, SYSTEM_SETTING_VALUE_AUTOHIDE_MAINFRAME_PC		= 0x00008
		, SYSTEM_SETTING_VALUE_HIDE_MAINFRAME_PC			= 0x00010
		, SYSTEM_SETTING_VALUE_DISABLE_USER_CLOUD_DRIVE		= 0x00100
		, SYSTEM_SETTING_VALUE_DISABLE_GROUP_SHARED_CLOUD	= 0x00200
		, SYSTEM_SETTING_VALUE_DISABLE_MODIFY_PASSWD		= 0x00400
		, SYSTEM_SETTING_VALUE_DISABLE_VIDEO				= 0x00800
		, SYSTEM_SETTING_VALUE_DISABLE_REMOTE_DESKTOP		= 0x01000
		, SYSTEM_SETTING_VALUE_AUTH_REQADD2GROUP			= 0x10000
		, SYSTEM_SETTING_VALUE_DISABLE_EMAIL_LOGIN			= 0x20000
		, SYSTEM_SETTING_VALUE_DISABLE_ACCOUNT_EDIT			= 0x40000
		, SYSTEM_SETTING_VALUE_DEFAULT_BROWSER_IE			= 0x80000
		, SYSTEM_SETTING_VALUE_DISABLE_ENTBOOST_TW		= 0x100000
		, SYSTEM_SETTING_VALUE_DISABLE_IE_ACCEPTENCODING		= 0x200000
		, SYSTEM_SETTING_VALUE_DISABLE_USER_GROUP			= 0x400000
		, SYSTEM_SETTING_VALUE_DISABLE_MSG_RECEIPT		= 0x800000
		, SYSTEM_SETTING_VALUE_ENABLE_GROUP_USERHEAD				= 0x1000000	// 支持多部门自定义头像
		, SYSTEM_SETTING_VALUE_ENABLE_SAVE_MAIL_CONTACT			= 0x2000000	// 接收邮件自动添加联系人
		, SYSTEM_SETTING_VALUE_DISABLE_STATSUB_GROUP_MEMBER	= 0x4000000	// 禁止统计子部门人数（只显示当前部门人数）
		, SYSTEM_SETTING_VALUE_DISABLE_SEARCH_NAME					= 0x8000000	// 禁止模糊搜索，群组名称，用户名称
	}SYSTEM_SETTING_VALUE;
	int m_nMaxUGCount;
	int m_nMaxContactCount;
	bool m_bUsesUidList;
	bool m_bEmptyUidListAuto;
	tstring m_sRegisterUrl;			// 注册URL地址
	mycp::bigint m_nAutoOpenSubId;
	tstring m_sEncryptionSalt;
	int m_nPBKDF2Count;
	int m_nPBKDF2Length;	// 默认32，＝64个字节密码；
	bool m_bOpenEntMgr;
	//bool m_bOpenUIDRegister;
	tstring m_sAccountAuthUrl;
	tstring m_sAccountActiveUrl;
	mycp::bigint m_nAutoAddGroupId;
	typedef enum CLUSTER_TYPE
	{
		CLUSTER_TYPE_0	= 0
		, CLUSTER_TYPE_1		// 自动用户动态分配
	}CLUSTER_TYPE;
	CLUSTER_TYPE m_nClusterType;
	//tstring m_sPop3ServerList;
	tstring m_sMyMailSetting;
	CLockMap<tstring,CEBMailSettingInfo::pointer> m_sMailSettingList;
	void LoadMailSetting(const tstring& sStringValue)
	{
		m_sMyMailSetting = sStringValue;
		// 格式：tom.com,pop.tom.com,110,smtp.tom.com,25;xxx
		std::vector<tstring> pList1;
		CEBSysInfo::ParseString(sStringValue.c_str(),";",pList1);
		for (size_t i=0; i<pList1.size(); i++)
		{
			std::vector<tstring> pList2;
			if (CEBSysInfo::ParseString(pList1[i].c_str(),",",pList2)>=5)
			{
				const tstring sDomain = pList2[0];
				CEBMailSettingInfo::pointer pMailSettingInfo;
				if (!m_sMailSettingList.find(sDomain,pMailSettingInfo))
				{
					pMailSettingInfo  =  CEBMailSettingInfo::create(sDomain,pList2[1],atoi(pList2[2].c_str()),pList2[3],atoi(pList2[4].c_str()));
					m_sMailSettingList.insert(pMailSettingInfo->m_sDomain,pMailSettingInfo);
				}else
				{
					pMailSettingInfo->m_sPop3Address = pList2[1];
					pMailSettingInfo->m_nPop3Port = atoi(pList2[2].c_str());
					pMailSettingInfo->m_sSmtpAddress = pList2[3];
					pMailSettingInfo->m_nSmtpPort = atoi(pList2[4].c_str());
				}
			}
		}
	}
	mycp::bigint m_nEmailSubId;
	bool m_bAutoTransformAccount;
	int m_nRequestReceiveMailMinTime;	// 默认2分钟
	int m_nRequestReceiveMailMaxTime;	// 默认5分钟
	tstring m_sViewOfficeUrl;
	tstring m_sViewOfficeExt;
	tstring m_sViewImageExt;
	int m_nMaxOffFileSize;						// default 20MB
	int m_nMaxResFileSize;						// default 20MB
	int m_nMaxReceiveOffMsgCount;
	int m_nMaxOAuthTimeDays;					// 默认90天
	typedef enum ECARD_INFO_FLAG
	{
		ECARD_INFO_FLAG_DEFAULT_OPEN	= 0							// 默认开放所有
		, ECARD_INFO_FLAG_DISABLE_TEL_PHONE_NUMBER		// 禁止开放联系电话和手机号码
		, ECARD_INFO_FLAG_OPEN_NAME_HEAD_ONLY					// 只开放名称和头像资料
	}ECARD_INFO_FLAG;
	ECARD_INFO_FLAG m_nECardInfoFlag;
	int m_nMaxBCMsgSaveDays;					// 默认30天
	int m_nEBServerVersion;					// 服务端版本号 如 544=1.25.0.544
	tstring m_sDefaultUrl;				// 默认打开网址URL 
	int m_systemBandwidth;				// 系统带宽，默认10MB

	// *** 以下属于业务配置数据
	tstring m_sAttendStartDate;			// 考勤开始时期，格式:YYYY-mm-dd 默认空未开始
	tstring m_sAttendDialyJobDate;	// 考勤每天作业时期，格式:YYYY-mm-dd 默认空未开始

	CEBSysInfo(void)
		: m_nSendCallRecordAppId(0)
		, m_nSendMailAppId(0)
		, m_nOpenRegister(OPEN_REGISTER_NONE),m_bOpenVisitor(false)
		, m_nAppTestAccountCount(0)
		//, m_bUsesUserId(false)
		, m_nSupportVersion(0)
		, m_nDBType(0)
		, m_nCallbackAppId(0),m_nCallParam(0),m_nMaxRetryCallbackData(5)
		//, m_tLicenseStartTime(0)
		, m_nDeployType(DEPLOY_TYPE_PRIVATE),m_nDeployId(0)
		, m_nPasswdAuthMode(PASSWD_AUTH_MODE_ENTBOOST)
		, m_nSaveConversations(3)
		//, m_bInviteAdd2GroupAuth(false), m_bReqAdd2GroupAuth(true)
		, m_bGroupMemberChangeEvent(true)	// false
		, m_nGroupMsgSubId(0),m_nFindAppSubId(0)//,m_nMyCollectionSubId(0)
		, m_bDefaultAllowOutChat(true)
		, m_bDefaultAcceptOutChat(true)
		, m_nOffFileSpaceSize(50)
		, m_nOnlineMsgSaveDays(30)
		, m_nOfflineMsgSaveDays(90), m_nOfflineFileSaveDays(7)
		, m_nUserCloudDriveSize(50), m_nUserGroupCloudDriveSize(50), m_nEntGroupCloudDriveSize(50)
		, m_nPushTime(30), m_nPushContentType(PUSH_CONTENT_TYPE_CHAT_CONTENT)
		, m_nMaxRetryCount(10),m_nTempLockMinute(10)
		//, m_nDeleteEntMemberOp(DELETE_ENT_MEMBER_OP_DELETE)
		, m_nSystemTempValue(0)	// ??? 1.24以后版本，修改为 0x1
		, m_nSystemSetting(CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_CONTACT)
		, m_nMaxUGCount(15),m_nMaxContactCount(500)
		, m_bUsesUidList(false),m_bEmptyUidListAuto(true)
		, m_nAutoOpenSubId(0)
		, m_nPBKDF2Count(1),m_nPBKDF2Length(32)
		, m_bOpenEntMgr(false)
		//, m_bOpenUIDRegister(false)
		, m_nAutoAddGroupId(0)
		, m_nClusterType(CLUSTER_TYPE_1)
		, m_nEmailSubId(1002300104)
		, m_bAutoTransformAccount(false)
		, m_nRequestReceiveMailMinTime(2), m_nRequestReceiveMailMaxTime(5)
		, m_sViewOfficeExt(".doc.docx.docm.dotx.dotm.xls.xlsx.xlsm.xltx.xltm.xlsb.xlam.ppt.pptx.pptm.potx.potm.ppam.ppsx.ppsm.sldx.sldm.thmx")	// .pdf
		//, m_sViewOfficeUrl("https://view.officeapps.live.com/op/view.aspx?src="), m_sViewOfficeExt(".doc.docx.docm.dotx.dotm.xls.xlsx.xlsm.xltx.xltm.xlsb.xlam.ppt.pptx.pptm.potx.potm.ppam.ppsx.ppsm.sldx.sldm.thmx.pdf")
		, m_sViewImageExt(".png.jpg.jpeg.bmp.ico")
		, m_nMaxOffFileSize(20), m_nMaxResFileSize(20)
		, m_nMaxReceiveOffMsgCount(200)
		, m_nMaxOAuthTimeDays(90)
		, m_nECardInfoFlag(ECARD_INFO_FLAG_DEFAULT_OPEN)
		, m_nMaxBCMsgSaveDays(30)
		, m_nEBServerVersion(544), m_sDefaultUrl("http://www.entboost.com")
		, m_systemBandwidth(10)

	{
		// ** default
		m_nSystemSetting |= CEBSysInfo::SYSTEM_SETTING_VALUE_AUTH_REQADD2GROUP;
		CEBMailSettingInfo::pointer pMailSettingInfo =  CEBMailSettingInfo::create("tom.com","pop.tom.com",110,"smtp.tom.com",25);
		m_sMailSettingList.insert(pMailSettingInfo->m_sDomain,pMailSettingInfo);
		//m_sPop3ServerList = "pop.tom.com";
		////m_sPop3ServerList = "vip.sina.com,sohu.com";

		// ***不能添加，系统只在启动加载一次；
		//m_pTextList.push_back("emotion");
		//m_pTextList.push_back("picture");
		//m_pTextList.push_back("video");
		//m_pTextList.push_back("file");
	}
};

class CEBMailTemplateInfo
{
public:
	typedef boost::shared_ptr<CEBMailTemplateInfo> pointer;
	static CEBMailTemplateInfo::pointer create(mycp::bigint nType)
	{
		return CEBMailTemplateInfo::pointer(new CEBMailTemplateInfo(nType));
	}

	mycp::bigint m_nType;
	tstring m_sSmtpServer;
	int m_nSmtpPort;
	bool m_bIsSsl;
	tstring m_sFromName;
	tstring m_sFromAccount;
	tstring m_sFromPasswd;
	//tstring m_sName;
	tstring m_sSubject;
	tstring m_sBody;
	time_t m_tUpdateTime;
	bool m_bAuthOk;

	void SetMailAddress(const CEBMailTemplateInfo::pointer& pNewValue)
	{
		m_sSmtpServer = pNewValue->m_sSmtpServer;
		m_nSmtpPort = pNewValue->m_nSmtpPort;
		m_bIsSsl = pNewValue->m_bIsSsl;
		m_sFromName = pNewValue->m_sFromName;
		m_sFromAccount = pNewValue->m_sFromAccount;
		m_sFromPasswd = pNewValue->m_sFromPasswd;
		//m_bAuthOk = pNewValue->m_bAuthOk;
		m_tUpdateTime = pNewValue->m_tUpdateTime;
	}

	CEBMailTemplateInfo(mycp::bigint nType)
		: m_nType(nType)
		, m_nSmtpPort(25), m_bIsSsl(false)
		, m_tUpdateTime(0), m_bAuthOk(false)
	{
	}
	CEBMailTemplateInfo(void)
		: m_nType(0)
		, m_nSmtpPort(25), m_bIsSsl(false)
		, m_tUpdateTime(0), m_bAuthOk(false)
	{
	}
};
const CEBMailTemplateInfo::pointer NullEBMailTemplateInfo;

class CEBServerEventInfo
	: public cgcObject
{
public:
	typedef boost::shared_ptr<CEBServerEventInfo> pointer;
	typedef enum SERVER_EVENT_TYPE
	{
		SERVER_EVENT_TYPE_UNKNOWN = 0
		,SERVER_EVENT_TYPE_CHECK_UID_BC_MSG
		,SERVER_EVENT_TYPE_REMOVE_ONLY
	}SERVER_EVENT_TYPE;

	static CEBServerEventInfo::pointer create(void)
	{
		return CEBServerEventInfo::pointer(new CEBServerEventInfo());
	}
	static CEBServerEventInfo::pointer create(SERVER_EVENT_TYPE nEventType)
	{
		return CEBServerEventInfo::pointer(new CEBServerEventInfo(nEventType));
	}

	CEBServerEventInfo(void)
		: m_nEventType(SERVER_EVENT_TYPE_UNKNOWN)
		, m_nEventData1(0), m_nEventData2(0)
		, m_nCount(0)
		, m_pFile(NULL), m_lpszData(NULL)
	{
		m_tLastTime = time(0);
	}
	CEBServerEventInfo(SERVER_EVENT_TYPE nEventType)
		: m_nEventType(nEventType)
		, m_nEventData1(0), m_nEventData2(0)
		, m_nCount(0)
		, m_pFile(NULL), m_lpszData(NULL)
	{
		m_tLastTime = time(0);
	}
	virtual ~CEBServerEventInfo(void)
	{
		if (m_pFile!=NULL)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}
		if (m_lpszData!=NULL)
		{
			delete[] m_lpszData;
			m_lpszData = NULL;
		}
	}
public:
	SERVER_EVENT_TYPE m_nEventType;
	mycp::bigint m_nEventData1;
	mycp::bigint m_nEventData2;
	tstring m_sString1;
	tstring m_sString2;
	time_t m_tLastTime;
	int m_nCount;
	FILE* m_pFile;
	char* m_lpszData;
};

} // namespace entboost

#endif // __eb_objects_h__
