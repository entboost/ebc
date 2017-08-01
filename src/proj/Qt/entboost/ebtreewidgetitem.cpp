#include "ebclientapp.h"
#include "ebtreewidgetitem.h"

EbTreeWidgetItem::EbTreeWidgetItem(EbTreeWidgetItem::QTreeWidgetItem *parent, int type)
    : QTreeWidgetItem(parent, type)
{

}

EbTreeWidgetItem::EbTreeWidgetItem(EbTreeWidgetItem::QTreeWidgetItem *parent, const QStringList &strings, int type)
    : QTreeWidgetItem(parent, strings, type)
{

}

EbTreeWidgetItem::EbTreeWidgetItem(QTreeWidget *view, const QStringList &strings, int type)
    : QTreeWidgetItem(view, strings, type)
{

}

EbTreeWidgetItem::~EbTreeWidgetItem()
{
    m_itemInfo.reset();
}

//void EbTreeWidgetItem::deleteChildItem(QTreeWidgetItem *childItem)
//{
//    QTreeWidgetItem * item = takeChild( indexOfChild(childItem) );
//    if (item!=0) {
//        delete item;
//    }
//}

bool EbTreeWidgetItem::operator<(const QTreeWidgetItem &other) const
{
    if (m_itemInfo.get()==0) {
        return false;
    }
    const EbWidgetItemInfo* pItemInfo = ((EbTreeWidgetItem*)&other)->m_itemInfo.get();
    return m_itemInfo->operator <(pItemInfo);
//    const CTreeItemInfo * pItemInfo1 = m_itemInfo.get();
//    const CTreeItemInfo * pItemInfo2 = ((EbTreeWidgetItem*)&other)->m_itemInfo.get();
//    if (pItemInfo1==NULL || pItemInfo2==NULL) return false;

//    if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) {
////            const bool bAppFrameShowed = theApp.GetWorkFrameShowed();
////#ifdef USES_SUPPORT_UI_STYLE
////            if (bAppFrameShowed || (pItemInfo1->m_nSubType==1 && pItemInfo2->m_nSubType==1))
////#else
////            if (bAppFrameShowed)
////#endif
////                return pItemInfo1->m_dwItemData<pItemInfo2->m_dwItemData?1:-1;
////            else
////                return pItemInfo1->m_dwItemData>pItemInfo2->m_dwItemData?1:-1;
//    }
//    else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP) {
//        // 群组（部门）排序
//        if (pItemInfo1->m_nSubType!=pItemInfo2->m_nSubType)						// a 先按类型排序
//            return pItemInfo1->m_nSubType<pItemInfo2->m_nSubType?true:false;
//        else if (pItemInfo1->m_nIndex!=pItemInfo2->m_nIndex)					// b 再按排序比较
//            return pItemInfo1->m_nIndex<pItemInfo2->m_nIndex?false:true;        // 排序值高显示在前面（小于对调位置）
//        else																// c 最后按名称gbk排序
//            return pItemInfo1->m_sName<pItemInfo2->m_sName;
////                return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
//    }
//    else if ((pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER) ||
//        (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_CONTACT)) {
//        if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_MEMBER) {
//            /// 员工按照权限类型比较排序
//            if (pItemInfo1->m_nIndex!=pItemInfo2->m_nIndex) {					/// a 先按排序比较
//                return pItemInfo1->m_nIndex<pItemInfo2->m_nIndex?false:true;    /// 排序值高显示在前面（小于对调位置）
//            }
//            else if (pItemInfo1->m_nSubType!=pItemInfo2->m_nSubType) {			/// b 再按权限类型排序
//                return pItemInfo1->m_nSubType<pItemInfo2->m_nSubType?false:true;    /// 权限小排后面，对调位置
//            }
//        }
//        else {
//            /// 联系人，按照有没有UID排序；
//            if (pItemInfo2->m_nUserId>0 && pItemInfo1->m_nUserId==0) return false; 		/// 第2个有UID，对换位置
//            else if (pItemInfo1->m_nUserId>0 && pItemInfo2->m_nUserId==0) return true;	/// 第2个0，不用对换位置
//        }

//        // 员工比较，先比较在线状态
//        if ((pItemInfo1->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && pItemInfo2->m_dwItemData<EB_LINE_STATE_ONLINE_NEW) ||
//            (pItemInfo1->m_dwItemData>=EB_LINE_STATE_BUSY && pItemInfo2->m_dwItemData<EB_LINE_STATE_BUSY)) {
//            // a 第一个在线，第二个其他 不用换位置
//            // b 第一个离开/忙，第二个离线 不用换位置
//            return true;
//        }
//        else if ((pItemInfo2->m_dwItemData==EB_LINE_STATE_ONLINE_NEW && pItemInfo1->m_dwItemData<EB_LINE_STATE_ONLINE_NEW) ||
//            (pItemInfo2->m_dwItemData>=EB_LINE_STATE_BUSY && pItemInfo1->m_dwItemData<EB_LINE_STATE_BUSY)) {
//            // a 第2个在线，第1个其他 对调位置
//            // b 第2个离开/忙，第1个离线 对调位置
//            return false;
//        }
//        return pItemInfo1->m_sName<pItemInfo2->m_sName;
////            return strcmp(pItemInfo1->m_sName.c_str(), pItemInfo2->m_sName.c_str());
//    }
//    else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) {
//        // 第一个是部门（目录），排前面（不用调位置）
//        return true;
//    }
//    else if (pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_GROUP || pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_DIR) {
//        // 第二个是部门（目录），对调位置
//        return false;
//    }
//    else if (pItemInfo1->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE && pItemInfo2->m_nItemType==CTreeItemInfo::ITEM_TYPE_FILE) {
//        return pItemInfo1->m_nBigId<pItemInfo2->m_nBigId?false:true;		// 时间大，排前面（小于对调位置）
//    }
//    return true;
}
