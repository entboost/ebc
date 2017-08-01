#ifndef EBTEXTBLOCKUSERDATA_H
#define EBTEXTBLOCKUSERDATA_H

#include <QTextBlockUserData>
#include <QWidget>
#include <QFrame>
#include <QLabel>

class EbTextBlockUserData : public QTextBlockUserData
{
public:
    enum TypeFlag {
        Unknown   = 0x0,
        TitleMessage = 0x1,
        ChatMessage = 0x2,
        ReceiveMessage = 0x4,
        FileMessage = 0x8
    };
    EbTextBlockUserData(qint64 msgId);
    virtual ~EbTextBlockUserData(void);

    bool isTitleMessage(void) const {return (m_flags&TitleMessage)!=0?true:false;}
    bool isChatMessage(void) const {return (m_flags&ChatMessage)!=0?true:false;}
    bool isReceiveMessage(void) const {return (m_flags&ReceiveMessage)!=0?true:false;}
    bool isFileMessage(void) const {return (m_flags&FileMessage)!=0?true:false;}
    void setFlags(int v) {m_flags=v;}
    int flags(void) const {return m_flags;}
    void setMsgId(qint64 v) {m_msgId = v;}
    qint64 msgId(void) const {return m_msgId;}
    void setUserId(qint64 v) {m_userId = v;}
    qint64 userId(void) const {return m_userId;}
    void setFilePath(const QString &v) {m_filePath=v;}
    const QString &filePath(void) const {return m_filePath;}

//    void createWidget(QWidget *parent);
//    void setGeometry(const QRect & rect);

//    void setParameter1(const QVariant &v) {m_parameter1 = v;}
//    const QVariant &parameter1(void) const {returtn m_parameter1;}
//    void setParameter2(const QVariant &v) {m_parameter2 = v;}
//    const QVariant &parameter2(void) const {returtn m_parameter2;}
private:
    qint64 m_msgId;
    int m_flags;
    qint64 m_userId;
    QString m_filePath;
//    QLabel * m_widget;
//    QVariant m_parameter1;
//    QVariant m_parameter2;
};

#endif // EBTEXTBLOCKUSERDATA_H
