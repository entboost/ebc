#ifndef EBFILEINFO_H
#define EBFILEINFO_H

#include <QtGui>
#include <boost/shared_ptr.hpp>
#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace mycp;
#include "../../../include/eb_object.h"
#include "../../../include/chatroomhandle.h"
using namespace entboost;
//#include <QtQuick/QQuickItem>

class EbFileInfo : public CCrFileInfo
{
public:
    typedef boost::shared_ptr<EbFileInfo> pointer;
    EbFileInfo(const CCrFileInfo &other);
    static EbFileInfo::pointer create(const CCrFileInfo &other);

    void setProgressValue(int v) {m_progressValue = v;}
    int progressValue(void) const {return m_progressValue;}
    void setReceive(bool v) {m_isReceive = v;}
    bool isReceive(void) const {return m_isReceive;}

private:
    int m_progressValue;    /// 0-100
    bool m_isReceive;
};

#endif // EBFILEINFO_H
