
#ifdef WIN32
#include "Windows.h"
#endif
#ifdef _QT_MAKE_
#include <QCoreApplication>
#ifdef Q_OS_ANDROID
#include <QStandardPaths>
#include <QDir>
#endif /// Q_OS_ANDROID
#include <QFileInfo>
#include <QString>
#include <QColor>
#include <QTextCodec>
#include <QRgb>
//#include <QDebug>
#else
#include "Shlwapi.h"
#endif
#include "EBParseSetting.h"

#ifndef WIN32
#include "../include/boost_ini.h"
#endif

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/xml_parser.hpp>
//#include <boost/foreach.hpp>
//using boost::property_tree::ptree;

namespace entboost {

CEBParseSetting::CEBParseSetting(void)
//: m_nLocalPort(0)
: m_nDefaultColor(0)
, m_nSDKVersion(0)
, m_nDefaultUIStyleType(1)

{
}

CEBParseSetting::~CEBParseSetting(void)
{
	//FreeHandle();
}

#ifdef _QT_MAKE_
std::string CEBParseSetting::str2utf8(const char * strSource, const char* sourceCodename)
{
	QTextCodec* pSourceCodec = QTextCodec::codecForName(sourceCodename);
	if (pSourceCodec==NULL) return "";

	const QString qstr = pSourceCodec->toUnicode(strSource);
	QByteArray arr = qstr.toUtf8();
	const std::string cstr = arr.data();  
	return cstr;
}
std::string CEBParseSetting::utf82str(const char * strSource, const char* targetCodename)
{
	QTextCodec* pTargetCodec = QTextCodec::codecForName(targetCodename);
	if (pTargetCodec==NULL) return "";

    const QString qstr = QString::fromUtf8(strSource);
    QByteArray arr = pTargetCodec->fromUnicode(qstr);
	const std::string cstr = arr.data();  
	return cstr;  
}

#else
std::string CEBParseSetting::str_convert(const char * strSource, int sourceCodepage, int targetCodepage)
{
	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strSource, -1, NULL, 0);
	if (unicodeLen <= 0) return "";

	wchar_t * pUnicode = new wchar_t[unicodeLen];
	memset(pUnicode,0,(unicodeLen)*sizeof(wchar_t));

	MultiByteToWideChar(sourceCodepage, 0, strSource, -1, (wchar_t*)pUnicode, unicodeLen);

	char * pTargetData = 0;
	int targetLen = WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);
	if (targetLen <= 0)
	{
		delete[] pUnicode;
		return "";
	}

	pTargetData = new char[targetLen];
	memset(pTargetData, 0, targetLen);

	WideCharToMultiByte(targetCodepage, 0, (wchar_t*)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);

	std::string result = pTargetData;
	//	tstring result(pTargetData, targetLen);
	delete[] pUnicode;
	delete[] pTargetData;
	return   result;
}
#endif

bool CEBParseSetting::load(const char* filename)
{
//#ifdef _QT_MAKE_
//    qDebug() << "CEBParseSetting::load() : " << filename;
//#endif

    /// 用户配置服务器地址
#ifdef _QT_MAKE_
#ifdef Q_OS_ANDROID
    const QString writablePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    const QString pApplicationDirPath = QDir(writablePath).absolutePath();
#else
	const QString pApplicationDirPath = QCoreApplication::applicationDirPath();
#endif
    const QString sEBCSettingTemp = pApplicationDirPath+"/datas/ebc.ini";
    if (QFileInfo::exists(sEBCSettingTemp))
#else
	char str[MAX_PATH];
    GetModuleFileNameA( NULL, str, MAX_PATH);
	std::string sEBCSetting(str);
	sEBCSetting = sEBCSetting.substr(0,sEBCSetting.rfind('\\'));
	sEBCSetting.append("\\datas\\ebc.ini");
    if (::PathFileExistsA(sEBCSetting.c_str()))
#endif
	{
#ifdef _QT_MAKE_
        const std::string sEBCSetting = sEBCSettingTemp.toLocal8Bit().toStdString();
#endif
        char lpszBuffer[1024];
		memset(lpszBuffer,0,sizeof(lpszBuffer));
#ifdef WIN32
        ::GetPrivateProfileStringA("system","server","",lpszBuffer,sizeof(lpszBuffer),sEBCSetting.c_str());
#else
        GetPrivateProfileStringABoost("system","server","",lpszBuffer,sizeof(lpszBuffer),sEBCSetting.c_str());
#endif
		m_sServerAddress = lpszBuffer;
	}

    if (m_nSDKVersion==0) {
#ifdef WIN32
        m_nSDKVersion = GetPrivateProfileIntA("setting","sdk-ver",0,filename);
#else
        m_nSDKVersion = GetPrivateProfileIntABoost("setting","sdk-ver",0,filename);
#endif
    }
//#ifdef WIN32
//    m_nSDKVersion = GetPrivateProfileIntA("setting","sdk-ver",0,filename);
//#else
////    m_nSDKVersion = 514;
//    if (m_nSDKVersion==0) {
//        m_nSDKVersion = GetPrivateProfileIntABoost("setting","sdk-ver",0,filename);
//    }
//#endif
	if (m_nSDKVersion>0)
	{
		// 新版本
        char lpszBuffer[260];
        memset(lpszBuffer,0,sizeof(lpszBuffer));
        if (m_sVersion.empty()) {
#ifdef WIN32
            ::GetPrivateProfileStringA("setting","version","",lpszBuffer,260,filename);
#else
            //        GetPrivateProfileStringABoost("setting","version","",lpszBuffer,260,filename);
            sprintf(lpszBuffer, "2.0.0.%d", m_nSDKVersion);
#endif
            m_sVersion = lpszBuffer;
        }
        //// 用户配置服务器地址
		//char str[MAX_PATH];
        //GetModuleFileNameA( NULL, str, MAX_PATH);
		//std::string sEBCSetting(str);
		//sEBCSetting = sEBCSetting.substr(0,sEBCSetting.rfind('\\'));
		//sEBCSetting.append("\\datas\\ebc.ini");
		//if (::PathFileExists(sEBCSetting.c_str()))
		//{
		//	TCHAR lpszBuffer[1024];
		//	memset(lpszBuffer,0,sizeof(lpszBuffer));
        //	::GetPrivateProfileStringA("system","server","",lpszBuffer,sizeof(lpszBuffer),sEBCSetting.c_str());
		//	m_sServerAddress = lpszBuffer;
		//}
		if (m_sServerAddress.empty())
		{
			memset(lpszBuffer,0,sizeof(lpszBuffer));
#ifdef WIN32
            ::GetPrivateProfileStringA("setting","server","",lpszBuffer,260,filename);
#else
            GetPrivateProfileStringABoost("setting","server","",lpszBuffer,260,filename);
#endif
			m_sServerAddress = lpszBuffer;
		}
		memset(lpszBuffer,0,sizeof(lpszBuffer));
		//MessageBox(NULL,"111",lpszBuffer,MB_OK);

#ifdef WIN32
        ::GetPrivateProfileStringA("setting","enterprise","",lpszBuffer,260,filename);
#else
        GetPrivateProfileStringABoost("setting","enterprise","",lpszBuffer,260,filename);
#endif
#ifdef _QT_MAKE_
        m_sEnterprise = lpszBuffer;
//		m_sEnterprise = utf82str(lpszBuffer, "GBK");
#else
		m_sEnterprise = str_convert(lpszBuffer,CP_UTF8, CP_ACP);
#endif

#ifdef WIN32
        m_nDefaultUIStyleType = GetPrivateProfileIntA("setting","uistyle-type",1,filename);
        const BYTE r = (BYTE)GetPrivateProfileIntA("default-color","r",255,filename);
        const BYTE g = (BYTE)GetPrivateProfileIntA("default-color","g",127,filename);
        const BYTE b = (BYTE)GetPrivateProfileIntA("default-color","b",39,filename);
//		m_nDefaultColor = RGB(r,g,b);
#else
        m_nDefaultUIStyleType = GetPrivateProfileIntABoost("setting","uistyle-type",1,filename);
        const unsigned char r = (unsigned char)GetPrivateProfileIntABoost("default-color","r",255,filename);
        const unsigned char g = (unsigned char)GetPrivateProfileIntABoost("default-color","g",127,filename);
        const unsigned char b = (unsigned char)GetPrivateProfileIntABoost("default-color","b",39,filename);
//        m_nDefaultColor = QColor::fromRgb(r,g,b).rgb();
#endif

#ifdef _QT_MAKE_
        m_nDefaultColor = QColor::fromRgb(r,g,b).rgb();
#else
        m_nDefaultColor = RGB(r,g,b);
#endif
		return true;
	}

	//// ???兼容旧版本，1.15版本更新，1.16以后版本删除；
	//ptree pt;
	////read_xml(filename.c_str(), pt);
	//try
	//{
	//	std::ifstream ifs(filename, std::ios_base::in|std::ios_base::binary);
	//	if (!ifs.is_open())
	//		return false;
	//	std::stringstream stringStream;
	//	char temp;
	//	int index = 0;
	//	while (ifs.peek() != EOF)
	//	{
	//		ifs.get(temp);
	//		stringStream << (char)(temp + (index++));
	//	}
	//	ifs.close();
	//	std::basic_istream<typename boost::property_tree::ptree::key_type::value_type> basicStream(stringStream.rdbuf());
	//	read_xml(basicStream, pt);

	//	m_sCallKey = pt.get("setting.base.call-key", "");
	//	//m_sBaseKey = pt.get("setting.base.key", "");
	//	//MessageBox(NULL,m_sServerAddress.c_str(),"",MB_OK);
	//	if (m_sServerAddress.empty())
	//		m_sServerAddress = pt.get("setting.base.center-server", "");
	//	//m_nLocalPort = pt.get("setting.base.local-port", 0);
	//	BYTE r = (BYTE)pt.get("setting.base.default-color.r", 255);
	//	BYTE g = (BYTE)pt.get("setting.base.default-color.g", 127);
	//	BYTE b = (BYTE)pt.get("setting.base.default-color.b", 39);
	//	m_nDefaultColor = RGB(r,g,b);
	//	m_sVersion = pt.get("setting.base.version", "0.0.0.1");
	//	m_nSDKVersion = pt.get("setting.base.sdk-ver", (int)0);
	//	m_sEnterprise = pt.get("setting.login.ent", "");
	//	if (!m_sEnterprise.empty())
	//		m_sEnterprise = str_convert(m_sEnterprise.c_str(), CP_UTF8, CP_ACP);
	//	m_sLoginText = pt.get("setting.login.text", "");
	//	if (!m_sLoginText.empty())
	//		m_sLoginText = str_convert(m_sLoginText.c_str(), CP_UTF8, CP_ACP);
	//	return true;
	//}catch(std::exception&)
	//{
	//}catch(...)
	//{
	//}
#ifdef _QT_MAKE_
	m_nDefaultColor = QColor::fromRgb(86,190,245).rgb();
#else
	m_nDefaultColor = RGB(86,190,245);
#endif
	//try
	//{
	//	BOOST_FOREACH(const ptree::value_type &v, pt.get_child("setting.lts"))
	//		InsertTitle(v);
	//}catch(...)
	//{}
	return false;
}
//
//void CEBParseSetting::InsertTitle(const boost::property_tree::ptree::value_type & v)
//{
//	//if (v.first.compare("lt") == 0)
//	//{
//	//	int disable = v.second.get("disable", 0);
//	//	if (disable == 1) return;
//
//	//	int level = v.second.get("l", -1);
//	//	std::string title = v.second.get("t", "");
//	//	if (level < 0 || title.empty()) return;
//	//	int roomlevel = v.second.get("rl", DEFAULT_ROOMLEVEL);
//	//	this->m_pLevelTitles.push_back(CLevelTitle(level, roomlevel, title));
//	//}
//}

} // namespace entboost
