#ifndef EBWIDGETAPPBAR_H
#define EBWIDGETAPPBAR_H

#include <ebclientapp.h>
#include <QWidget>
#include "ebappbuttonitem.h"
#include <vector>

class EbWidgetAppBar : public QWidget
{
    Q_OBJECT
public:
    enum Orientation {
        Horizontal,
        Vertical
    };
    explicit EbWidgetAppBar(EB_FUNC_LOCATION funcLocation, QWidget *parent = nullptr);
    virtual ~EbWidgetAppBar(void);

    EB_FUNC_LOCATION funcLocation(void) const {return m_funcLocation;}
    void setOrientation(Orientation v) {m_orientation=v;}
    Orientation orientation(void) const {return m_orientation;}

    bool existSubscribeId(qint64 id) const;
    EbAppButtonItem::pointer subscribeButtonItem(qint64 id) const;
    void setSubscribeMsgCount(qint64 subscribeId, int msgCount);
    void addSubscribeMsg(qint64 subscribeId);
signals:
    void clickedSubApp(const EB_SubscribeFuncInfo &funcInfo);

public slots:
    void onClickedAppItem(void);

protected:
    void onMove(void);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void timerEvent(QTimerEvent *e);

    bool loadFuncInfo(void);
    QRect getButtonIndexRect(int index) const;
    void insertIndex(int index, const EbAppButtonItem::pointer &buttonItem);
private:
    std::vector<EbAppButtonItem::pointer> m_lists;
    Orientation m_orientation;
    EB_FUNC_LOCATION m_funcLocation;
    int m_timerIdLoadFuncInfo;
};

#endif // EBWIDGETAPPBAR_H
