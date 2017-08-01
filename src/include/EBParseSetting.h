// EBParseSetting.h file here
#ifndef __EBParseSetting_h__
#define __EBParseSetting_h__
#ifdef _MSC_VER
#pragma warning(disable:4819)
#endif
#include <string>
#ifdef _QT_MAKE_
#include <QRgb>
#endif

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
#ifdef _QT_MAKE_
	static std::string str2utf8(const char * strSource, const char* sourceCodename);	// *sourceCodename=GBK
	static std::string utf82str(const char * strSource, const char* targetCodename);	// *targetCodename=GBK
#else
	static std::string str_convert(const char * strSource, int sourceCodepage, int targetCodepage);
#endif
public:
	bool load(const char* filename);

	//const std::string & GetBaseKey(void) const {return m_sBaseKey;}
	const std::string & GetCallKey(void) const {return m_sCallKey;}
	const std::string & GetServerAddress(void) const {return m_sServerAddress;}
	//unsigned int GetLocalPort(void) const {return m_nLocalPort;}

#ifdef _QT_MAKE_
    QRgb GetDefaultColor(void) const {return m_nDefaultColor;}
#else
    COLORREF GetDefaultColor(void) const {return m_nDefaultColor;}
#endif

//#ifdef WIN32
//	COLORREF GetDefaultColor(void) const {return m_nDefaultColor;}
//#else
//	unsigned long GetDefaultColor(void) const {return m_nDefaultColor;}
//#endif
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
#ifdef _QT_MAKE_
    QRgb            m_nDefaultColor;
#else
    COLORREF        m_nDefaultColor;
#endif
//#ifdef WIN32
//	COLORREF		m_nDefaultColor;
//#else
//	unsigned long	m_nDefaultColor;
//#endif
	std::string		m_sVersion;
    int			m_nSDKVersion;
	// login
	std::string		m_sEnterprise;
	std::string		m_sLoginText;
    int             m_nDefaultUIStyleType;	// 0:offiace; 1:chat(default)
};
//extern CEBParseSetting theSetting;

} // namespace entboost

#endif // __EBParseSetting_h__
