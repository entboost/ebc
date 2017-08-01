#ifndef EBWEBENGINEPROFILE_H
#define EBWEBENGINEPROFILE_H

#include <QWebEngineProfile>

class EbWebEngineProfile : public QWebEngineProfile
{
public:
    EbWebEngineProfile(QObject *parent=0);

protected:
};

#endif // EBWEBENGINEPROFILE_H
