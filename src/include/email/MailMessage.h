// MailMessage.h: interface for the CMailMessage class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAILMESSAGE_H__55DE48CC_BEA4_11D1_870E_444553540000__INCLUDED_)
#define AFX_MAILMESSAGE_H__55DE48CC_BEA4_11D1_870E_444553540000__INCLUDED_

#include <CGCBase/cgcSmartString.h>
#include <vector>
using namespace mycp;

// trim from start
static inline tstring &ltrim(tstring &s) {
	s.erase(0,s.find_first_not_of(" "));
	//s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}
// trim from end
static inline tstring &rtrim(tstring &s) {
	s.erase(s.find_last_not_of(" ") + 1);
	//s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}
static inline tstring &trim(tstring &s) {return ltrim(rtrim(s));} 

static inline void TrimMailName(tstring& sString)
{
	trim(sString);
	if (sString.empty()) return;
	if (sString.c_str()[0]=='\'' || sString.c_str()[0]=='"')
		sString = sString.substr(1);
	if (sString.empty()) return;
	const size_t nSize = sString.size();
	if (sString.c_str()[nSize-1]=='\'' || sString.c_str()[nSize-1]=='"')
	{
		if (nSize==1)
			sString = "";
		else
			sString = sString.substr(0,nSize-1);
	}
}

class CMailMessage  
{
public:
	CMailMessage(const char* sCharset = "gbk");
	virtual ~CMailMessage(void);

	enum RECIPIENTS_TYPE { TO, CC, BCC }; // <JFO>

	static bool GetMainAddressName(const tstring & sFullAddress, tstring& pOutName, tstring& pOutMail);

	void FormatMailMessage(void);
	int GetNumRecipients(RECIPIENTS_TYPE type = TO /* <JFO> */) const;
	bool GetRecipient( tstring& sEmailAddress, tstring& sFriendName, int nIndex = 0, RECIPIENTS_TYPE type = TO /* <JFO> */ );
	bool AddRecipient( const char* szEmailAddress, const char* szFriendlyName = "", RECIPIENTS_TYPE type = TO /* <JFO> */ );
	bool AddMultipleRecipients( const char* szRecipients = NULL, RECIPIENTS_TYPE type = TO /* <JFO> */ );
	size_t GetCharsPerLine(void) const {return m_nCharsPerLine;}
	void SetCharsPerLine( size_t nCharsPerLine ) {m_nCharsPerLine = nCharsPerLine;}

	tstring m_sCharset;	// utf-8,gbk,...
	tstring m_sFrom;
	tstring m_sSubject;
	tstring m_sEnvelope;
	tstring m_sHeader;
	tstring m_sBody;

private:
	size_t m_nCharsPerLine;
	class CRecipient
	{
		public:
			tstring m_sEmailAddress;
			tstring m_sFriendName;
	};
	std::vector<CRecipient> m_Recipients;
	std::vector<CRecipient> m_CCRecipients;
	std::vector<CRecipient> m_BCCRecipients;

protected:
	virtual void prepare_header();
	virtual void prepare_body();
	virtual void end_header();
	virtual void start_header();
	virtual void add_header_line( const char* szHeaderLine );
};

#endif // !defined(AFX_MAILMESSAGE_H__55DE48CC_BEA4_11D1_870E_444553540000__INCLUDED_)
