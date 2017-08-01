#ifndef EBLISTWIDGETITEM_H
#define EBLISTWIDGETITEM_H

#include <QListWidgetItem>
#include <ebwidgetiteminfo.h>
//#include "../../../include/ebc_public.h"

class EbListWidgetItem : public QListWidgetItem
{
public:
    explicit EbListWidgetItem(QListWidget *view=Q_NULLPTR, int type=Type);
    explicit EbListWidgetItem(const QString &text, QListWidget *view=Q_NULLPTR, int type=Type);
    explicit EbListWidgetItem(const QIcon &icon, const QString &text, QListWidget *view=Q_NULLPTR, int type=Type);
    virtual ~EbListWidgetItem(void);

    EbWidgetItemInfo::pointer m_itemInfo;
//    CTreeItemInfo::pointer m_itemInfo;
protected:
    virtual bool operator<(const QListWidgetItem &other) const;
};

#endif // EBLISTWIDGETITEM_H
