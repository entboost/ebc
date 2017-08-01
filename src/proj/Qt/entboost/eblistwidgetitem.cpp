#include <QtGui>
#include "eblistwidgetitem.h"

EbListWidgetItem::EbListWidgetItem(QListWidget *view, int type)
    : QListWidgetItem( view,type )
{
}

EbListWidgetItem::EbListWidgetItem(const QString &text, QListWidget *view, int type)
    : QListWidgetItem( text,view,type )
{
}

EbListWidgetItem::EbListWidgetItem(const QIcon &icon, const QString &text, QListWidget *view, int type)
    : QListWidgetItem( icon,text,view,type )
{

}

EbListWidgetItem::~EbListWidgetItem()
{
    m_itemInfo.reset();
}

bool EbListWidgetItem::operator<(const QListWidgetItem &other) const
{
    if (m_itemInfo.get()==0) {
        return false;
    }
    const EbWidgetItemInfo* pItemInfo = ((EbListWidgetItem*)&other)->m_itemInfo.get();
    return m_itemInfo->operator <(pItemInfo);
}
