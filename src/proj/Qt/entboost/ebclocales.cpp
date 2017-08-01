#include "ebclocales.h"
//#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

EbGroupTypeName::EbGroupTypeName(int value, const std::string& name, const std::string& shortName)
    : m_value(value), m_name(name), m_shortName(shortName)
{
}
EbGroupTypeName::pointer EbGroupTypeName::create(int value, const std::string& name, const std::string& shortName)
{
    return EbGroupTypeName::pointer(new EbGroupTypeName(value, name, shortName));
}

EbDayOfWeekInfo::EbDayOfWeekInfo(int value, const std::string& name, const std::string& shortName)
    : m_value(value), m_name(name), m_shortName(shortName)
{
}
EbDayOfWeekInfo::pointer EbDayOfWeekInfo::create(int value, const std::string& name, const std::string& shortName)
{
    return EbDayOfWeekInfo::pointer(new EbDayOfWeekInfo(value, name, shortName));
}

EbColorInfo::EbColorInfo(const QString& name, const QColor& color)
    : m_name(name), m_color(color)
{
}
EbColorInfo::pointer EbColorInfo::create(const QString& name, const QColor& color)
{
    return EbColorInfo::pointer(new EbColorInfo(name, color));
}

EbcLocales::EbcLocales()
    : m_titleBackgroundHeight(42)
{
    m_dayOfWeekTemp = EbDayOfWeekInfo::create(0,"","");
    m_groupTypeNameTemp = EbGroupTypeName::create(0,"","");
}

bool EbcLocales::loadLocaleFile(const std::string& fileName)
{
    try {
        m_pt.clear();
        m_colors.clear();
        m_dayOfWeeks.clear();
        m_groupTypeNames.clear();
        read_json(fileName, m_pt);
//        read_xml(fileName, m_pt);

        m_titleBackgroundHeight = m_pt.get("base-dialog.title.background-height", 42);
        char lpszBuffer[128];
        /// 加载 day-of-week
        for (int i=0; i<=6; i++) {
            sprintf(lpszBuffer,"day-of-week.%d.name",i);
            const std::string name = m_pt.get(lpszBuffer, "");
            sprintf(lpszBuffer,"day-of-week.%d.short",i);
            const std::string shortName = m_pt.get(lpszBuffer, "");
            m_dayOfWeeks.push_back(EbDayOfWeekInfo::create(i,name,shortName));
        }
        /// 加载群组类型名称
        for (int i=0; i<=3; i++) {
            if (i==3) {
                i = 9;  /// 临时讨论组
            }
            sprintf(lpszBuffer,"group-name.%d.name",i);
            const std::string name = m_pt.get(lpszBuffer, "");
            if (name.empty()) continue;
            sprintf(lpszBuffer,"group-name.%d.short",i);
            const std::string shortName = m_pt.get(lpszBuffer, "");
            if (shortName.empty()) continue;
            EbGroupTypeName::pointer groupTypeName = EbGroupTypeName::create(i,name,shortName);
            sprintf(lpszBuffer,"group-name.%d.member",i);
            groupTypeName->setMember( m_pt.get(lpszBuffer, "member") );
            sprintf(lpszBuffer,"group-name.%d.manager",i);
            groupTypeName->setManager( m_pt.get(lpszBuffer, "manager") );
            m_groupTypeNames.push_back(groupTypeName);
        }
        /// ** 加载颜色列表数据
        ptree image_array = m_pt.get_child("color-skin.colors");
        ptree::iterator iter = image_array.begin();
        for (; iter!=image_array.end(); iter++) {
            const boost::property_tree::ptree::value_type& v = *iter;
            const std::string name = v.second.get("name","");
            if (name.empty()) continue;
            const int r = v.second.get("r", -1);
            if (r==-1) continue;
            const int g = v.second.get("g", -1);
            if (g==-1) continue;
            const int b = v.second.get("b", -1);
            if (b==-1) continue;
            EbColorInfo::pointer colorInfo = EbColorInfo::create(QString::fromStdString(name),QColor(r,g,b));
            m_colors.push_back(colorInfo);
        }
        return true;
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return false;
}

int EbcLocales::getLocalInt(const char* textPath, int defaultInt)
{
    try {
        return m_pt.get(textPath, defaultInt);
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return defaultInt;
}

std::string EbcLocales::getLocalStdString(const char* textPath, const char* defaultText)
{
    try {
        return m_pt.get(textPath, defaultText);
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return defaultText;
}
#ifdef _QT_MAKE_
QString EbcLocales::getLocalText(const char* textPath, const char* defaultText)
#else
std::string EbcLocales::getLocalText(const char* textPath, const char* defaultText)
#endif
{
    try {
#ifdef _QT_MAKE_
        QString result = QString::fromStdString(m_pt.get(textPath, defaultText));
        return result;
#else
        return m_pt.get(textPath, defaultString);
#endif
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return defaultText;
}

const EbDayOfWeekInfo::pointer& EbcLocales::getDayOfWeekInfo(int dayOfWeek) const
{
    if (dayOfWeek>=0 && (dayOfWeek+1)<=(int)m_dayOfWeeks.size()) {
        return m_dayOfWeeks[dayOfWeek];
    }
    m_dayOfWeekTemp->setValue(dayOfWeek);
    return m_dayOfWeekTemp;
}

const EbGroupTypeName::pointer & EbcLocales::getGroupTypeName(int groupType) const
{
    if (groupType==9) {
        groupType = 3;  /// 临时讨论组
    }
    if (groupType>=0 && (groupType+1)<=(int)m_groupTypeNames.size()) {
        return m_groupTypeNames[groupType];
    }
    m_groupTypeNameTemp->setValue(groupType);
    return m_groupTypeNameTemp;
}
