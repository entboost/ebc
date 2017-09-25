#include "ebclocales.h"
#include <QFile>
//#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

EbGroupTypeName::EbGroupTypeName(int value, const std::string& name, const std::string& shortName)
    : m_value(value), m_name(name), m_shortName(shortName)
{
}


#ifdef _QT_QML_
EbGroupTypeName::EbGroupTypeName(QQuickItem *parent)
    : QQuickItem(parent)
    , m_value(0)
{

}
EbGroupTypeName::EbGroupTypeName(const EbGroupTypeName *other, QQuickItem *parent)
    : QQuickItem(parent)
    , m_value(other->value()), m_name(other->name()), m_shortName(other->shortName())
    , m_member(other->member()), m_manager(other->manager())
{

}
#endif

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

EbcLocales::EbcLocales(QObject *parent)
    : QObject(parent)
    , m_titleBackgroundHeight(42)
{
    m_dayOfWeekTemp = EbDayOfWeekInfo::create(0,"","");
    m_groupTypeNameTemp = EbGroupTypeName::create(0,"","");
}

EbcLocales::~EbcLocales(void)
{
    m_pt.clear();
}

void EbcLocales::loadLocaleList()
{
    for (int i=0; i<m_localeInfoList.size(); i++) {
        const EbLocaleInfo::pointer &info = m_localeInfoList[i];
        emit localeInfo(info->language(), info->name(), info->file());
    }
}

bool EbcLocales::loadLocaleListByQrc(const QString &qrcName)
{
    QFile file(qrcName);
    if (file.open(QFile::ReadOnly)) {
        const QByteArray byteArray = file.readAll();
        std::istringstream jsonStream;
        jsonStream.str(byteArray.constData());
        return loadLocaleList(jsonStream);
    }
    return false;
}

bool EbcLocales::loadLocaleList(std::istringstream &jsonStream)
{
    try {
        m_localeInfoList.clear();
        ptree pt;
        read_json(jsonStream, pt);
        loadLocaleList(pt);
        pt.clear();
        return true;
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return false;
}

bool EbcLocales::loadLocaleList(const std::string &fileName)
{
    try {
        m_localeInfoList.clear();
        ptree pt;
        read_json(fileName, pt);
        loadLocaleList(pt);
        pt.clear();
        return true;
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return false;
}

void EbcLocales::loadLocaleList(const ptree &pt)
{
    /// ** 加载列表数据
    ptree localeList = pt.get_child("locales");
    ptree::iterator iter = localeList.begin();
    for (; iter!=localeList.end(); iter++) {
        const boost::property_tree::ptree::value_type& v = *iter;
        const std::string language = v.second.get("language","");
        if (language.empty()) continue;
        const std::string name = v.second.get("name","");
        if (name.empty()) continue;
        const std::string file = v.second.get("file","");
        if (file.empty()) continue;
        EbLocaleInfo::pointer localeInfo = EbLocaleInfo::create(
                    QString::fromStdString(language),
                    QString::fromStdString(name),
                    QString::fromStdString(file));
        m_localeInfoList.push_back(localeInfo);
    }
}

bool EbcLocales::loadLocaleFileByQrc(const QString &qrcName)
{
    QFile file(qrcName);
    if (file.open(QFile::ReadOnly)) {
        const QByteArray byteArray = file.readAll();
        std::istringstream jsonStream;
        jsonStream.str(byteArray.constData());
        return loadLocaleFile(jsonStream);
    }
    return false;
}

bool EbcLocales::loadLocaleFile(std::istringstream &jsonStream)
{
    try {
        m_pt.clear();
        m_colors.clear();
        m_dayOfWeeks.clear();
        m_groupTypeNames.clear();
        read_json(jsonStream, m_pt);
        loadLocaleFile();
        return true;
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return false;
}

bool EbcLocales::loadLocaleFile(const std::string& fileName)
{
    try {
        m_pt.clear();
        m_colors.clear();
        m_dayOfWeeks.clear();
        m_groupTypeNames.clear();
        read_json(fileName, m_pt);
        loadLocaleFile();
        return true;
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return false;
}

void EbcLocales::loadLocaleFile(void)
{
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

std::string EbcLocales::getLocalStdString(const char* textPath, const char* defaultText) const
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
QString EbcLocales::getLocalText(const QString &textPath, const QString &defaultText) const
{
    try {
        return QString::fromStdString(m_pt.get(textPath.toStdString(), defaultText.toStdString()));
    }
    catch(ptree_error &) {
    }
    catch(...) {
    }
    return defaultText;
}
#endif

#ifdef _QT_MAKE_
QString EbcLocales::getLocalText(const char* textPath, const char* defaultText) const
#else
std::string EbcLocales::getLocalText(const char* textPath, const char* defaultText) const
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

#ifdef _QT_QML_
EbGroupTypeName *EbcLocales::getGroupTypeInfo(int groupType) const
{
    if (groupType==9) {
        groupType = 3;  /// 临时讨论组
    }
    EbGroupTypeName::pointer result;
    if (groupType>=0 && (groupType+1)<=(int)m_groupTypeNames.size()) {
        result  = m_groupTypeNames[groupType];
    }
    else {
        m_groupTypeNameTemp->setValue(groupType);
        result = m_groupTypeNameTemp;
    }
    return new EbGroupTypeName(result.get());
}
#endif

EbLocaleInfo::EbLocaleInfo(const QString &language, const QString &name, const QString &file)
    : m_language(language)
    , m_name(name)
    , m_file(file)
{

}

EbLocaleInfo::pointer EbLocaleInfo::create(const QString &language, const QString &name, const QString &file)
{
    return EbLocaleInfo::pointer(new EbLocaleInfo(language, name, file));
}
