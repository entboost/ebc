#ifndef EBKEYFILTER_H
#define EBKEYFILTER_H

#include <QObject>

class EbKeyFilter : public QObject
{
    Q_OBJECT
public:
    explicit EbKeyFilter(QObject *parent = nullptr);
    static EbKeyFilter* instance(void);

    ///设置过滤对象
//    Q_INVOKABLE QObject *setRootFilter(QObject *object);
//    Q_INVOKABLE void removeRootFilter(QObject *object);
    Q_INVOKABLE void setFilter(QObject *object, int deep=1);
    Q_INVOKABLE void removeFilter(QObject *object, int deep=1);
signals:
    void keyBackPressed(QObject *object);
public slots:

protected:
    //事件过滤
    virtual bool eventFilter(QObject *watched, QEvent *e);
private:
};

#endif // EBKEYFILTER_H
