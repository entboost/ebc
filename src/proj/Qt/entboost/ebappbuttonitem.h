#ifndef EBAPPBUTTONITEM_H
#define EBAPPBUTTONITEM_H

#include <ebclientapp.h>
#include <QPushButton>
#include <QLabel>
#include <boost/shared_ptr.hpp>

class EbAppButtonItem : public QObject
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbAppButtonItem> pointer;
    EbAppButtonItem(const QRect &rect, const EB_SubscribeFuncInfo &subscribeFuncInfo, QWidget * parent);
    static EbAppButtonItem::pointer create(const QRect &rect,const EB_SubscribeFuncInfo &subscribeFuncInfo, QWidget * parent);

    qint64 subscribeId(void) const {return m_subscribeFuncInfo.m_nSubscribeId;}
    void move(int x, int y);
    void move(const QPoint &point);
    bool setMsgCount(qint64 subscribeId, int msgCount);
    bool addMsgCount(qint64 subscribeId);
    void updateIcon(const QString &fileName);
    const EB_SubscribeFuncInfo & subscribeFuncInfo(void) const {return m_subscribeFuncInfo;}

    static bool isSysSubscribeId(qint64 subscribeId);
public slots:
    void onClickedAppItem(void);
private:
    QPushButton * m_buttonApp;
    QString m_iconFileName;
    QLabel * m_labelMsg;
    int m_unreadMsgCount;
    EB_SubscribeFuncInfo m_subscribeFuncInfo;
};
const EbAppButtonItem::pointer EbAppButtonItemNull;

#endif // EBAPPBUTTONITEM_H
