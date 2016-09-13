// EBParseSetting.h file here
#ifndef __EBParseSetting_h__
#define __EBParseSetting_h__
#ifdef WIN32
#pragma warning(disable:4819)
#endif
#include <string>

#ifdef EB_APPCLIENT_EXPORTS
#define EB_EBPARSESETTING_API __declspec(dllexport)
#else // EB_APPCLIENT_EXPORTS
#define EB_EBPARSESETTING_API __declspec(dllimport)
#endif // EB_APPCLIENT_EXPORTS

namespace entboost {

class EB_EBPARSESETTING_API CEBParseSetting
{
public:
	CEBParseSetting(void);
	virtual ~CEBParseSetting(void);
	static std::string str_convert(const char * strSource, int sourceCodepage, int targetCodepage);
public:
	bool load(const char* filename);

	//const std::string & GetBaseKey(void) const {return m_sBaseKey;}
	const std::string & GetCallKey(void) const {return m_sCallKey;}
	const std::string & GetServerAddress(void) const {return m_sServerAddress;}
	//unsigned int GetLocalPort(void) const {return m_nLocalPort;}
	COLORREF GetDefaultColor(void) const {return m_nDefaultColor;}
	const std::string& GetVersion(void) const {return m_sVersion;}
	int GetSDKVersion(void) const {return m_nSDKVersion;}
	int GetDefaultUIStyleType(void) const {return m_nDefaultUIStyleType;}	// 0:office; 1:chat(default)

	const std::string& GetEnterprise(void) const {return m_sEnterprise;}
	const std::string& GetLoginText(void) const {return m_sLoginText;}

private:
	//void InsertTitle(const boost::property_tree::ptree::value_type & v);

private:
	//std::string		m_sBaseKey;
	std::string		m_sCallKey;
	std::string		m_sServerAddress;
	//unsigned int	m_nLocalPort;
	COLORREF		m_nDefaultColor;
	std::string		m_sVersion;
	int				m_nSDKVersion;
	// login
	std::string		m_sEnterprise;
	std::string		m_sLoginText;
	int						m_nDefaultUIStyleType;	// 0:offiace; 1:chat(default)
};
//extern CEBParseSetting theSetting;

} // namespace entboost

#endif // __EBParseSetting_h__
