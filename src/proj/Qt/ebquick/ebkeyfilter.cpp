#include "ebkeyfilter.h"
#include <QMutex>
#include <QKeyEvent>

EbKeyFilter::EbKeyFilter(QObject *parent) : QObject(parent)
{

}

EbKeyFilter *EbKeyFilter::instance()
{
    static QMutex mutex;
    static QScopedPointer<EbKeyFilter> instance;
    if (Q_UNLIKELY(!instance)) {
        mutex.lock();
        if(!instance)
        {
            instance.reset(new EbKeyFilter);
        }
        mutex.unlock();
    }
    return instance.data();
}

//QObject *EbKeyFilter::setRootFilter(QObject *object)
//{
//    if (object==0) {
//        return 0;
//    }
//    QObject *root = object;
//    while (root->parent()!=0) {
//        root = root->parent();
//    }
//    root->installEventFilter(this);
//    return root;
//}

//void EbKeyFilter::removeRootFilter(QObject *object)
//{
//    if (object==0) {
//        return;
//    }
//    QObject *root = object;
//    while (root->parent()!=0) {
//        root = root->parent();
//    }
//    root->removeEventFilter(this);
//}

void EbKeyFilter::setFilter(QObject *object, int deep)
{
    if (object==0) {
        return;
    }
    object->installEventFilter(this);
    for (int i=2; i<=deep; i++) {
        if (object->parent()==0) {
            return;
        }
        object = object->parent();
        object->installEventFilter(this);
    }
}

void EbKeyFilter::removeFilter(QObject *object, int deep)
{
    if (object==0) {
        return;
    }
    object->removeEventFilter(this);
    for (int i=2; i<=deep; i++) {
        if (object->parent()==0) {
            return;
        }
        object = object->parent();
        object->removeEventFilter(this);
    }
}

bool EbKeyFilter::eventFilter(QObject *watched, QEvent *e)
{
    /// 获取事件类型
    if (e->type()==QEvent::KeyPress) {
        ///转换成键盘事件
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
        /// 判断是否是back事件
        if (keyEvent->key()==Qt::Key_Back) {
            /// 发送back键按下的信号
            emit keyBackPressed(watched);
            return true;
        }
    }
    return QObject::eventFilter(watched, e);
}
