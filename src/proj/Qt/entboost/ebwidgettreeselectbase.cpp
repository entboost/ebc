#include "ebwidgettreeselectbase.h"
#include <ebiconhelper.h>

EbWidgetTreeSelectBase::EbWidgetTreeSelectBase(EB_VIEW_MODE viewMode, QWidget *parent) : QWidget(parent)
  , m_viewMode(viewMode)
  , m_selectedUserCallback(0)
{
    //    m_treeWidget->setRootIsDecorated( false ); ///去掉虚线边框（这是去掉根ITEM前面图标）
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setFrameStyle(QFrame::NoFrame); ///去掉边框
    if (m_viewMode==EB_VIEW_SELECT_USER) {
        m_treeWidget->setObjectName("HasBorder");
    }
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setIconSize(const_tree_icon_size);
    m_treeWidget->setExpandsOnDoubleClick(false);
    m_treeWidget->setMouseTracking(false);
    m_treeWidget->setFrameShape(QFrame::NoFrame);
    m_treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeWidget->setAllColumnsShowFocus(false);
    m_treeWidget->setWordWrap(true);
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setMouseTracking(true);
    //    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
//    connect( m_treeWidget,SIGNAL(itemPressed(QTreeWidgetItem*,int)),this,SLOT(onItemClicked(QTreeWidgetItem*,int)) );

    /// “添加”按钮
    m_pushButtonSelect = new QPushButton(this);
    m_pushButtonSelect->setParent( m_treeWidget );
    m_pushButtonSelect->setVisible(false);
    m_pushButtonSelect->setObjectName("CallButton");
//    connect( m_pushButtonSelect, SIGNAL(clicked()),this,SLOT(onClickedPushButtonSelect()) );
    EbIconHelper::Instance()->SetIcon(m_pushButtonSelect,QChar(0xf067),12 );
    /// “打开会话”按钮
    m_pushButtonCall = new QPushButton(this);
    m_pushButtonCall->setParent( m_treeWidget );
    m_pushButtonCall->setVisible(false);
    m_pushButtonCall->setObjectName("CallButton");
//    connect( m_pushButtonCall, SIGNAL(clicked()),this,SLOT(onClickedPushButtonCall()) );
    EbIconHelper::Instance()->SetIcon(m_pushButtonCall,QChar(0xf27a),12 );
    /// “修改我的名片”
    m_pushButtonEdit = new QPushButton(this);
    m_pushButtonEdit->setParent( m_treeWidget );
    m_pushButtonEdit->setVisible(false);
    m_pushButtonEdit->setObjectName("CallButton");
//    connect( m_pushButtonEdit, SIGNAL(clicked()),this,SLOT(onClickedPushButtonEdit()) );
    EbIconHelper::Instance()->SetIcon(m_pushButtonEdit,QChar(0xf2c3),12 );

}

void EbWidgetTreeSelectBase::updateLocaleInfo()
{
    m_pushButtonSelect->setToolTip( theLocales.getLocalText("main-frame.button-select.tooltip","Select") );
    m_pushButtonCall->setToolTip( theLocales.getLocalText("main-frame.button-call.tooltip","open chat") );
    m_pushButtonEdit->setToolTip( theLocales.getLocalText("main-frame.button-edit.tooltip","edit member info") );

}

void EbWidgetTreeSelectBase::timerCheckState()
{
    if ( m_pushButtonCall->isVisible() || m_pushButtonEdit->isVisible() || m_pushButtonSelect->isVisible() ) {
        /// 实现定期自动判断当前鼠标位置，并刷新 call 按钮
        const QRect& rect = m_treeWidget->geometry();
        const QPoint pointMouseToDialog = this->mapFromGlobal(this->cursor().pos());
        if (!rect.contains(pointMouseToDialog)) {
            m_pushButtonCall->setVisible(false);
            m_pushButtonEdit->setVisible(false);
            m_pushButtonSelect->setVisible(false);
        }
    }
}
