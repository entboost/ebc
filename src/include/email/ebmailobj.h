// ebmailobj.h file here
#ifndef __ebmailobj_h__
#define __ebmailobj_h__

#include <stl/lockmap.h>
#include <CGCBase/cgcparameter.h>
using namespace mycp;

class CEBMailInfo;
typedef boost::shared_ptr<CEBMailInfo> CEBMailInfoPointer;

class CEBMailNameInfo
{
public:
	typedef boost::shared_ptr<CEBMailNameInfo> pointer;
	static CEBMailNameInfo::pointer create(const std::string& sCharset, const std::string& sName)
	{
		return CEBMailNameInfo::pointer(new CEBMailNameInfo(sCharset, sName));
	}
	static CEBMailNameInfo::pointer create(void)
	{
		return CEBMailNameInfo::pointer(new CEBMailNameInfo());
	}
	void SetCharset(const std::string& sCharset) {m_sCharset = sCharset;}
	const std::string& GetCharset(void) const {return m_sCharset;}
	void SetName(const std::string& sName) {m_sName = sName;}
	const std::string& GetName(void) const {return m_sName;}
	void SetEmail(const std::string& sEmail) {m_sEmail = sEmail;}
	const std::string& GetEmail(void) const {return m_sEmail;}
	void SetFullName(bool v) {m_bFullName = v;}
	bool GetFullName(void) const {return m_bFullName;}

	CEBMailNameInfo(const std::string& sCharset, const std::string& sName)
		: m_sCharset(sCharset), m_sName(sName)
		, m_bFullName(false)
	{}
	CEBMailNameInfo(void)
	{}
	virtual ~CEBMailNameInfo(void)
	{}
private:
	std::string m_sCharset;
	std::string m_sName;
	std::string m_sEmail;
	bool m_bFullName;
};

class CEBMailAttachmentInfo : public CEBMailNameInfo
{
public:
	typedef boost::shared_ptr<CEBMailAttachmentInfo> pointer;
	static CEBMailAttachmentInfo::pointer create(const std::string& sCharset, const std::string& sName)
	{
		return CEBMailAttachmentInfo::pointer(new CEBMailAttachmentInfo(sCharset, sName));
	}
	static CEBMailAttachmentInfo::pointer create(void)
	{
		return CEBMailAttachmentInfo::pointer(new CEBMailAttachmentInfo());
	}
	//CEBMailNameInfo m_pFileInfo;

	void SetAttachmentData1(const char* sContent, size_t nSize) {
		if (m_pAttachmentData!=0)
			delete[] m_pAttachmentData;
		m_nAttachmentSize = nSize;
		m_pAttachmentData = new char[m_nAttachmentSize+1];
		memcpy(m_pAttachmentData,sContent,m_nAttachmentSize);
		m_pAttachmentData[m_nAttachmentSize] = '\0';
	}
	void SetAttachmentData2(char* sContent, size_t nSize) {
		if (m_pAttachmentData!=0)
			delete[] m_pAttachmentData;
		m_nAttachmentSize = nSize;
		m_pAttachmentData = sContent;
		m_pAttachmentData[m_nAttachmentSize] = '\0';
	}
	bool IsHasAttachmentData(void) {return (m_pAttachmentData!=0 && m_nAttachmentSize>0)?true:false;}
	size_t GetAttachmentSize(void) {return m_nAttachmentSize;}
	const char* GetAttachmentData1(void) {return m_pAttachmentData;}
	char* GetAttachmentData2(size_t& pOutSize) {
		char * result = m_pAttachmentData;
		m_pAttachmentData = 0;
		pOutSize = m_nAttachmentSize;
		m_nAttachmentSize = 0;
		return result;
	}
	void SetContentId(const std::string& sContentId) {m_sContentId = sContentId;}
	const std::string& GetContentId(void) const {return m_sContentId;}
	bool IsContentIdData(void) const {return m_sContentId.empty()?false:true;}

	CEBMailAttachmentInfo(const std::string& sCharset, const std::string& sName)
		: CEBMailNameInfo(sCharset,sName)
		, m_pAttachmentData(0), m_nAttachmentSize(0)
	{}
	CEBMailAttachmentInfo(void)
		: m_pAttachmentData(0), m_nAttachmentSize(0)
	{}
	virtual ~CEBMailAttachmentInfo(void)
	{
		if (m_pAttachmentData!=0)
		{
			delete[] m_pAttachmentData;
			m_pAttachmentData = 0;
		}
	}
private:
	char* m_pAttachmentData;
	size_t m_nAttachmentSize;
	std::string m_sContentId;
};

class CEBMailBaseInfo
{
public:
	typedef boost::shared_ptr<CEBMailBaseInfo> pointer;
	static CEBMailBaseInfo::pointer create(void)
	{
		return CEBMailBaseInfo::pointer(new CEBMailBaseInfo());
	}

	void SetContentTypeCharset(const std::string& sCharset) {m_sContentTypeCharset = sCharset;}
	const std::string& GetContentTypeCharset(void) const {return m_sContentTypeCharset;}
	void SetContentTransferEncoding(const std::string& sEncoding) {m_sContentTransferEncoding = sEncoding;}
	const std::string& GetContentTransferEncoding(void) {return m_sContentTransferEncoding;}
	void SetContentData1(const std::string& sContent) {
		if (m_pContentData!=0)
			delete[] m_pContentData;
		m_nContentSize = sContent.size();
		m_pContentData = new char[m_nContentSize+1];
		memcpy(m_pContentData,sContent.c_str(),m_nContentSize);
		m_pContentData[m_nContentSize] = '\0';
	}
	void SetContentData2(const char* sContent, size_t nSize) {
		if (m_pContentData!=0)
			delete[] m_pContentData;
		m_nContentSize = nSize;
		m_pContentData = new char[m_nContentSize+1];
		memcpy(m_pContentData,sContent,m_nContentSize);
		m_pContentData[m_nContentSize] = '\0';
	}
	void SetContentData3(char* sContent, size_t nSize) {
		if (m_pContentData!=0)
			delete[] m_pContentData;
		m_nContentSize = nSize;
		m_pContentData = sContent;
		m_pContentData[m_nContentSize] = '\0';
	}
	const char* GetContentData(void) {return m_pContentData;}
	size_t GetContentSize(void) {return m_nContentSize;}
	bool IsHasContentData(void) const {return (m_nContentSize==0 || m_pContentData==NULL)?false:true;}
	void SetContentId(const std::string& sContentId) {m_sContentId = sContentId;}
	const std::string& GetContentId(void) const {return m_sContentId;}

	CLockMap<std::string,cgcParameter::pointer> m_pHeaders;
	CEBMailInfoPointer m_pMessageInfo;
	CEBMailAttachmentInfo::pointer m_pAttachment;

	CEBMailBaseInfo(void)
		: m_pContentData(0), m_nContentSize(0)
	{}
	virtual ~CEBMailBaseInfo(void)
	{
		if (m_pContentData!=0)
		{
			delete[] m_pContentData;
			m_pContentData = 0;
		}
		m_pHeaders.clear();
		m_pMessageInfo.reset();
		m_pAttachment.reset();
	}
private:
	std::string m_sContentTypeCharset;
	std::string m_sContentTransferEncoding;
	char* m_pContentData;
	size_t m_nContentSize;
	std::string m_sContentId;
};

class CEBMailBoundaryInfo
{
public:
	typedef boost::shared_ptr<CEBMailBoundaryInfo> pointer;
	static CEBMailBoundaryInfo::pointer create(const std::string& sBoundary)
	{
		return CEBMailBoundaryInfo::pointer(new CEBMailBoundaryInfo(sBoundary));
	}

	const std::string& GetBoundary(void) const {return m_sBoundary;}
	std::vector<CEBMailBaseInfo::pointer> m_pBoundaryList;

	CEBMailBoundaryInfo(const std::string& sBoundary)
		: m_sBoundary(sBoundary)
	{}
	virtual ~CEBMailBoundaryInfo(void)
	{
		m_pBoundaryList.clear();
	}
private:
	std::string m_sBoundary;
};

class CEBMailInfo : public CEBMailBaseInfo
{
public:
	typedef boost::shared_ptr<CEBMailInfo> pointer;
	static CEBMailInfo::pointer create(void)
	{
		return CEBMailInfo::pointer(new CEBMailInfo());
	}
	CEBMailNameInfo m_pSubject;
	CEBMailNameInfo m_pFrom;
	std::vector<CEBMailNameInfo::pointer> m_pTo;
	std::vector<CEBMailNameInfo::pointer> m_pCc;
	std::vector<CEBMailBoundaryInfo::pointer> m_pBoundaryList;
	void SetDate(const std::string& sDate) {m_sDate = sDate;}
	const std::string& GetDate(void) const {return m_sDate;}
	void SetDateSecond(time_t tDate) {m_tDate = tDate;}
	const time_t GetDateSecond(void) const {return m_tDate;}
	void SetHead(const std::string& sDate) {m_sHead = sDate;}
	const std::string& GetHead(void) const {return m_sHead;}
	void SetMailData(const std::string& sMaileData) {m_sMailData = sMaileData;}
	const std::string& GetMailData(void) const {return m_sMailData;}
	void SetMailSize(int nMailSize) {m_nMailSize = nMailSize;}
	int GetMailSize(void) const {return m_nMailSize;}
	CLockMap<std::string,CEBMailBaseInfo::pointer> m_pContentData;
	std::vector<CEBMailAttachmentInfo::pointer> m_pAttachmentList;

	CEBMailInfo(void)
		: m_tDate(0),m_nMailSize(0)
	{}
	virtual ~CEBMailInfo(void)
	{
		m_pBoundaryList.clear();
		m_pContentData.clear();
		m_pAttachmentList.clear();
	}
private:
	std::string m_sDate;
	time_t m_tDate;
	std::string m_sHead;
	std::string m_sMailData;
	int m_nMailSize;
};
const CEBMailInfo::pointer NullEBMailInfo;

#endif // __ebmailobj_h__
