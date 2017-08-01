// boost_ini.h file here
#ifndef __boost_ini_h__
#define __boost_ini_h__

//#ifndef WIN32
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp> 
#include <boost/foreach.hpp>
using namespace boost::property_tree;
//#endif

#ifdef WIN32
#define snprintf     _snprintf
#endif

//#ifndef WIN32
inline unsigned long
GetPrivateProfileStringABoost(
        const char* lpAppName,
        const char* lpKeyName,
        const char* lpDefault,
        char* lpReturnedString,
        unsigned long nSize,
        const char* lpFileName
        )
{
    boost::property_tree::ptree pt,pt_child;
    const char* lpDefaulTemp = (lpDefault==NULL)?"":lpDefault;
    try {
        boost::property_tree::ini_parser::read_ini(lpFileName, pt);
        if (strstr(lpKeyName,".")==NULL) {
            std::string sGetPath(lpAppName);
            sGetPath.append(".");
            sGetPath.append(lpKeyName);
            const std::string stemp(pt.get(sGetPath, lpDefaulTemp));
            return (unsigned long)snprintf(lpReturnedString,nSize,"%s",stemp.c_str());
        }
        else {
            std::string sGetPath(lpAppName);
            sGetPath.append("/");
            sGetPath.append(lpKeyName);
            const boost::property_tree::ptree::path_type path(sGetPath,'/');
            const std::string stemp(pt.get(path, lpDefaulTemp));
            return (unsigned long)snprintf(lpReturnedString,nSize,"%s",stemp.c_str());
            //pt_child = pt.get_child(lpAppName);
            //BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt_child) {
            //	const std::string value = v.second.get_value("");
            //	if (v.first.compare(lpKeyName)==0) {
            //		return (unsigned long)snprintf(lpReturnedString,nSize,"%s",value.c_str());
            //	}
            //}
        }
    }
    catch(ptree_error &) {
        //return 0;
    }
    return lpDefault==NULL?0:(unsigned long)snprintf(lpReturnedString,nSize,"%s",lpDefault);
}

inline unsigned int
GetPrivateProfileIntABoost(
        const char* lpAppName,
        const char* lpKeyName,
        int nDefault,
        const char* lpFileName
        )
{
    boost::property_tree::ptree pt,pt_child;
    try {
        boost::property_tree::ini_parser::read_ini(lpFileName, pt);
        if (strstr(lpKeyName,".")==NULL) {
            std::string sGetPath(lpAppName);
            sGetPath.append(".");
            sGetPath.append(lpKeyName);
            return (unsigned int)pt.get(sGetPath, nDefault);
        }
        else {
            std::string sGetPath(lpAppName);
            sGetPath.append("/");
            sGetPath.append(lpKeyName);
            const boost::property_tree::ptree::path_type path(sGetPath,'/');
            return (unsigned int)pt.get(path, nDefault);
        }
    }
    catch(ptree_error &) {
        //return 0;
    }
    return (unsigned int)nDefault;
}

inline int
WritePrivateProfileStringABoost(
        const char* lpAppName,
        const char* lpKeyName,
        const char* lpString,
        const char* lpFileName
        )
{
    boost::property_tree::ptree pt,pt_child;
    const char* lpStringTemp = (lpString==NULL)?"":lpString;
    try {
        FILE * f = fopen(lpFileName,"r");
        if (f==NULL) {
            f = fopen(lpFileName,"w");
            if (f==NULL) {
                return 0;
            }
        }
        fclose(f);
        boost::property_tree::ini_parser::read_ini(lpFileName, pt);
        if (strstr(lpKeyName,".")==NULL) {
            std::string sGetPath(lpAppName);
            sGetPath.append(".");
            sGetPath.append(lpKeyName);
            pt.put(sGetPath,lpStringTemp);
        }
        else {
            std::string sGetPath(lpAppName);
            sGetPath.append("/");
            sGetPath.append(lpKeyName);
            const boost::property_tree::ptree::path_type path(sGetPath,'/');
            pt.put(path,lpStringTemp);
        }
        boost::property_tree::ini_parser::write_ini(lpFileName, pt);
        return 1;
    }
    catch(ptree_error &) {
        //return 0;
    }
    return 0;
}

inline int
WritePrivateProfileIntABoost(
        const char* lpAppName,
        const char* lpKeyName,
        int nValue,
        const char* lpFileName
        )
{
    boost::property_tree::ptree pt,pt_child;
    try {
        FILE * f = fopen(lpFileName,"r");
        if (f==NULL) {
            f = fopen(lpFileName,"w");
            if (f==NULL) {
                return 0;
            }
        }
        fclose(f);

        boost::property_tree::ini_parser::read_ini(lpFileName, pt);
        if (strstr(lpKeyName,".")==NULL) {
            std::string sGetPath(lpAppName);
            sGetPath.append(".");
            sGetPath.append(lpKeyName);
            pt.put(sGetPath,nValue);
        }
        else {
            std::string sGetPath(lpAppName);
            sGetPath.append("/");
            sGetPath.append(lpKeyName);
            const boost::property_tree::ptree::path_type path(sGetPath,'/');
            pt.put(path,nValue);
        }
        boost::property_tree::ini_parser::write_ini(lpFileName, pt);
        return 1;
    }
    catch(ptree_error &) {
        //return 0;
    }
    return 0;
}

//#endif


#endif // __boost_ini_h__
