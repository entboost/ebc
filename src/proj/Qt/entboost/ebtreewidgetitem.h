#ifndef EBTREEWIDGETITEM_H
#define EBTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <ebwidgetiteminfo.h>
//#include "../../../include/ebc_public.h"

class EbTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit EbTreeWidgetItem(QTreeWidgetItem *parent = 0, int type=QTreeWidgetItem::Type);
    explicit EbTreeWidgetItem(QTreeWidgetItem *parent, const QStringList &strings, int type=QTreeWidgetItem::Type);
    explicit EbTreeWidgetItem(QTreeWidget *view, const QStringList &strings, int type=QTreeWidgetItem::Type);
    virtual ~EbTreeWidgetItem(void);

//    void deleteChildItem(QTreeWidgetItem *childItem);
    EbWidgetItemInfo::pointer m_itemInfo;
//    CTreeItemInfo::pointer m_itemInfo;
protected:
    virtual bool operator<(const QTreeWidgetItem &other) const;
};

#endif // EBTREEWIDGETITEM_H
