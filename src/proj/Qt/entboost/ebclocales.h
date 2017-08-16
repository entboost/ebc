#ifndef EBCLOCALES_H
#define EBCLOCALES_H

#include <QObject>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;
#ifdef _QT_MAKE_
#include <QString>
#endif
#include <boost/shared_ptr.hpp>
#include <QColor>
#include <vector>

class EbGroupTypeName
{
public:
    typedef boost::shared_ptr<EbGroupTypeName> pointer;
    static EbGroupTypeName::pointer create(int value, const std::string &name, const std::string &shortName);
    void setValue(int v) {m_value=v;}
    int value(void) const {return m_value;}
    const std::string &name(void) const {return m_name;}
    const std::string &shortName(void) const {return m_shortName;}
    void setMember(const std::string &v) {m_member=v;}
    const std::string &member(void) const {return m_member;}
    void setManager(const std::string &v) {m_manager=v;}
    const std::string &manager(void) const {return m_manager;}
    EbGroupTypeName(int value, const std::string &name, const std::string &shortName);
private:
    int m_value;            /// 0,1,2,9
    std::string m_name;         /// 部门/
    std::string m_shortName;    /// 部门
    std::string m_member;    /// 部门员工/群组成员/...
    std::string m_manager;    /// 部门经理/群主/...
};

class EbDayOfWeekInfo
{
public:
    typedef boost::shared_ptr<EbDayOfWeekInfo> pointer;
    static EbDayOfWeekInfo::pointer create(int value, const std::string &name, const std::string &shortName);
    void setValue(int v) {m_value=v;}
    int value(void) const {return m_value;}
    const std::string &name(void) const {return m_name;}
    const std::string &shortName(void) const {return m_shortName;}
    EbDayOfWeekInfo(int value, const std::string &name, const std::string &shortName);
private:
    int m_value;            /// 0,1,...
    std::string m_name;         /// 星期天，星期一，...
    std::string m_shortName;    /// 周日，周一，...
};

class EbColorInfo
{
public:
    typedef boost::shared_ptr<EbColorInfo> pointer;
    static EbColorInfo::pointer create(const QString &name, const QColor &color);
    const QString &name(void) const {return m_name;}
    const QColor &color(void) const {return m_color;}
    EbColorInfo(const QString &name, const QColor &color);
private:
    QString m_name;
    QColor m_color;
};

class EbLocaleInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString file READ file WRITE setFile)
public:
    typedef boost::shared_ptr<EbLocaleInfo> pointer;
    EbLocaleInfo(const QString &language, const QString &name, const QString &file);
    static EbLocaleInfo::pointer create(const QString &language, const QString &name, const QString &file);
    void setLanguage(const QString &v) {m_language=v;}
    const QString &language(void) const {return m_language;}
    void setName(const QString &v) {m_name=v;}
    const QString &name(void) const {return m_name;}
    void setFile(const QString &v) {m_file=v;}
    const QString &file(void) const {return m_file;}
private:
    QString m_language;
    QString m_name;
    QString m_file;
};

class EbcLocales : public QObject
{
    Q_OBJECT
public:
    EbcLocales(QObject * parent=0);
    virtual ~EbcLocales(void);

signals:
    void localeInfo(const QString &language, const QString &name, const QString &file);
public:
    Q_INVOKABLE void loadLocaleList(void);
    Q_INVOKABLE bool loadLocaleListByQrc(const QString &qrcName);
    bool loadLocaleList(std::istringstream &jsonStream);
    bool loadLocaleList(const std::string &fileName);
    void loadLocaleList(const ptree &pt);
    Q_INVOKABLE bool loadLocaleFileByQrc(const QString &qrcName);
    bool loadLocaleFile(std::istringstream &jsonStream);
    bool loadLocaleFile(const std::string &fileName);
    void loadLocaleFile(void);
    int getLocalInt(const char *textPath, int defaultInt);
    std::string getLocalStdString(const char *textPath, const char *defaultText);
#ifdef _QT_MAKE_
    Q_INVOKABLE QString getLocalText(const QString &textPath, const QString &defaultText);
    QString getLocalText(const char *textPath, const char *defaultText);
#else
    std::string getLocalText(const char *textPath, const char *defaultText);
#endif

    int titleBackgroundHeight(void) const {return m_titleBackgroundHeight;}
    const std::vector<EbColorInfo::pointer> &colors(void) const {return m_colors;}
    const EbDayOfWeekInfo::pointer &getDayOfWeekInfo(int dayOfWeek) const;
    const EbGroupTypeName::pointer &getGroupTypeName(int groupType) const;
    const std::vector<EbLocaleInfo::pointer> &localeInfoList(void) const {return m_localeInfoList;}
private:
    ptree m_pt;
    std::vector<EbColorInfo::pointer> m_colors;
    int m_titleBackgroundHeight;
    std::vector<EbDayOfWeekInfo::pointer> m_dayOfWeeks;
    EbDayOfWeekInfo::pointer m_dayOfWeekTemp;
    std::vector<EbGroupTypeName::pointer> m_groupTypeNames;
    EbGroupTypeName::pointer m_groupTypeNameTemp;
    std::vector<EbLocaleInfo::pointer> m_localeInfoList;
};

#endif // EBCLOCALES_H
