#include "ebdialogselectuser.h"
#include "ui_ebdialogselectuser.h"
#include <ebiconhelper.h>
#include <eblistwidgetitem.h>
#include <ebwidgetmygroup.h>
#include <ebwidgetmycontact.h>
#include <ebwidgetmyenterprise.h>

EbDialogSelectUser::EbDialogSelectUser(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogSelectUser)
  , m_enterpriseInfo(0)
  , m_singleSelect(false)
  , m_selectedGroupId(0)
{
    /// 设置过滤 ESC/Enter 按键
    this->setFilterEscapeKey(true);
    this->setFilterEnterKey(true);

    m_widgetMyGroup = new EbWidgetMyGroup(EB_VIEW_SELECT_USER,this);
    m_widgetMyGroup->setSelectedUserCallback(this);
    connect( m_widgetMyGroup,SIGNAL(selectedItemInfo(EbWidgetItemInfo::pointer)),this,SLOT(onSelectedItemInfo(EbWidgetItemInfo::pointer)) );
    m_widgetMyContact = new EbWidgetMyContact(EB_VIEW_SELECT_USER,this);
    m_widgetMyContact->setSelectedUserCallback(this);
    connect( m_widgetMyContact,SIGNAL(selectedItemInfo(EbWidgetItemInfo::pointer)),this,SLOT(onSelectedItemInfo(EbWidgetItemInfo::pointer)) );
    m_widgetMyEnterprise = new EbWidgetMyEnterprise(EB_VIEW_SELECT_USER,this);
    m_widgetMyEnterprise->setSelectedUserCallback(this);
    connect( m_widgetMyEnterprise,SIGNAL(selectedItemInfo(EbWidgetItemInfo::pointer)),this,SLOT(onSelectedItemInfo(EbWidgetItemInfo::pointer)) );

    m_lineEditSearch = new EbLineEdit(this);
    m_lineEditSearch->setMouseFocusInSelectAll(true);
//    m_lineEditSearch->installEventFilter(this);
    connect( m_lineEditSearch,SIGNAL(keyPressEnter(QString)),this,SLOT(onSearchEditKeyPressEnter(QString)) );
    connect( m_lineEditSearch,SIGNAL(keyPressEsc()),this,SLOT(onSearchEditKeyPressEsc()) );
    connect( m_lineEditSearch,SIGNAL(textChanged(QString)),this,SLOT(onSearchEditTextChange(QString)) );
    connect( m_lineEditSearch,SIGNAL(keyPressDown()),this,SLOT(onSearchEditKeyPressDown()) );
    /// 主界面搜索结果列表
    m_widgetSearchResult = new EbWidgetSearchResult(EB_SEARCH_FROM_SELECT_USER,this);
    m_widgetSearchResult->setSelectedUserCallback(this);
    m_widgetSearchResult->setVisible(false);
//    connect( m_widgetSearchResult,SIGNAL(searchFirst(QString)),this,SLOT(onSearchFirst(QString)) );
    connect( m_widgetSearchResult,SIGNAL(clickedSearchResultUrl(QString)),this,SLOT(onClickedSearchResultUrl(QString)) );
    connect( m_widgetSearchResult->listResults(),SIGNAL(keyPressFirstItemUp()),this,SLOT(onListResultsKeyPressFirstItemUp()) );
    connect( m_widgetSearchResult->listResults(),SIGNAL(keyPressEsc()),this,SLOT(onListResultsKeyPressEsc()) );
    connect( m_widgetSearchResult,SIGNAL(selectedItemInfo(EbWidgetItemInfo::pointer)),this,SLOT(onSelectedItemInfo(EbWidgetItemInfo::pointer)) );

    ui->setupUi(this);
    /// 设置初始大小
    this->resize(548,452);
#ifdef __MACH__
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::CustomizeWindowHint);
#else
    /// 去掉标题栏
    this->setWindowFlags(Qt::FramelessWindowHint );
#endif

    const int const_title_height = 80;
    this->showTitleBackground( const_title_height );
    updateLocaleInfo();

    const QRect rectIcon(7,13,16,16);
    /// 部门
    m_labelMyGroupIcon = new QLabel(ui->pushButtonMyGroup);
    m_labelMyGroupIcon->setObjectName("IconLabel");
    EbIconHelper::Instance()->SetIcon(m_labelMyGroupIcon,QChar(0xf0c0),10);
    m_labelMyGroupIcon->setVisible(true);
    m_labelMyGroupIcon->setGeometry( rectIcon );
    const QSize const_check_button_size(86,39+3);   /// 49+3 (3主要用于下面圆角不显示)
    ui->pushButtonMyGroup->setCheckable(true);
    ui->pushButtonMyGroup->setObjectName("TitleCheckButton");
    int x = 14;
    int y = 41;
    ui->pushButtonMyGroup->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
    connect( ui->pushButtonMyGroup,SIGNAL(clicked()),this,SLOT(onClickedButtonMyGroup()) );
    /// 联系人
    m_labelMyContactIcon = new QLabel(ui->pushButtonMyContact);
    m_labelMyContactIcon->setObjectName("IconLabel");
    EbIconHelper::Instance()->SetIcon(m_labelMyContactIcon,QChar(0xf007),10);
    m_labelMyContactIcon->setVisible(true);
    m_labelMyContactIcon->setGeometry( rectIcon );
    ui->pushButtonMyContact->setCheckable(true);
    ui->pushButtonMyContact->setObjectName("TitleCheckButton");
    x += const_check_button_size.width();
    ui->pushButtonMyContact->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
    connect( ui->pushButtonMyContact,SIGNAL(clicked()),this,SLOT(onClickedButtonMyContact()) );
    /// 公司
    m_labelMyEnterpriseIcon = new QLabel(ui->pushButtonMyEnterprise);
    m_labelMyEnterpriseIcon->setObjectName("IconLabel");
    EbIconHelper::Instance()->SetIcon(m_labelMyEnterpriseIcon,QChar(0xf19c),10);
    m_labelMyEnterpriseIcon->setVisible(true);
    m_labelMyEnterpriseIcon->setGeometry( rectIcon );
    ui->pushButtonMyEnterprise->setCheckable(true);
    ui->pushButtonMyEnterprise->setObjectName("TitleCheckButton");
    x += const_check_button_size.width();
    ui->pushButtonMyEnterprise->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
    connect( ui->pushButtonMyEnterprise,SIGNAL(clicked()),this,SLOT(onClickedButtonMyEnterprise()) );

    /// 已选择用户：
    ui->listWidgetSelected->setMouseTracking(true);
    ui->listWidgetSelected->setObjectName("SelectedUser");
    ui->listWidgetSelected->setIconSize(const_tree_icon_size);
    connect( ui->listWidgetSelected,SIGNAL(itemEntered(QListWidgetItem*)),this,SLOT(onItemEnteredSelected(QListWidgetItem*)) );
    /// 删除
    ui->pushButtonDelete->setParent(ui->listWidgetSelected);
    EbIconHelper::Instance()->SetIcon(ui->pushButtonDelete,QChar(0xf014),12 );
    ui->pushButtonDelete->setObjectName("CallDelButton");
    ui->pushButtonDelete->setVisible(false);
    connect( ui->pushButtonDelete,SIGNAL(clicked()),this,SLOT(onClickedButtonDelete()) );


    onClickedButtonMyGroup();
    QTimer::singleShot( 100,this,SLOT(onLoadEnterpriseInfo()) );
}

EbDialogSelectUser::~EbDialogSelectUser()
{
    delete ui;
}

void EbDialogSelectUser::updateLocaleInfo()
{
    const QString sDialogTitle = theLocales.getLocalText("select-user-dialog.title","Select User");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf007),const_common_title_image_font_size );
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示 “保存/关闭” 按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("base-dialog.ok-button.text","Ok"), "OkButton",
                                     theLocales.getLocalText("base-dialog.close-button.text","Close"), "CancelButton" );

    /// 部门
    ui->pushButtonMyGroup->setText(  theLocales.getLocalText("select-user-dialog.button-group.text","Group") );
    ui->pushButtonMyGroup->setToolTip( theLocales.getLocalText("select-user-dialog.button-group.tooltip","") );
    /// 联系人
    ui->pushButtonMyContact->setText(  theLocales.getLocalText("select-user-dialog.button-contact.text","Contact") );
    ui->pushButtonMyContact->setToolTip( theLocales.getLocalText("select-user-dialog.button-contact.tooltip","") );
    /// 公司
    ui->pushButtonMyEnterprise->setText(  theLocales.getLocalText("select-user-dialog.button-enterprise.text","Enterprise") );
    ui->pushButtonMyEnterprise->setToolTip( theLocales.getLocalText("select-user-dialog.button-enterprise.tooltip","") );
    m_lineEditSearch->setPlaceholderText( theLocales.getLocalText("select-user-dialog.edit-search.bg-text","Search") );
    m_lineEditSearch->setToolTip( theLocales.getLocalText("select-user-dialog.edit-search.tooltip","") );
    updateSelectedCount();

    ui->pushButtonDelete->setToolTip( theLocales.getLocalText("select-user-dialog.button-delete.tooltip","Delete") );

}

void EbDialogSelectUser::timerCheckState()
{
    if (ui->pushButtonDelete->isVisible()) {
        const QRect& rect = ui->listWidgetSelected->geometry();
        const QPoint pointMouseToDialog = this->mapFromGlobal(this->cursor().pos());
        if (!rect.contains(pointMouseToDialog)) {
            ui->pushButtonDelete->setVisible(false);
        }
    }
    if (m_widgetMyGroup!=0) {
        m_widgetMyGroup->timerCheckState();
    }
    if (m_widgetMyContact!=0) {
        m_widgetMyContact->timerCheckState();
    }
    if (m_widgetMyEnterprise!=0) {
        m_widgetMyEnterprise->timerCheckState();
    }

}

void EbDialogSelectUser::clearSelected()
{
    m_existedUser.clear();
    m_selectedList.clear();
    ui->listWidgetSelected->clear();
    updateSelectedCount();
}

void EbDialogSelectUser::onGroupInfo(const EB_GroupInfo *groupInfo)
{
    if (groupInfo->m_sEnterpriseCode>0 && m_widgetMyEnterprise!=0) {
        m_widgetMyEnterprise->onGroupInfo(groupInfo);
    }
    if (groupInfo->m_nMyEmpId>0 && this->m_widgetMyGroup!=0) {
        this->m_widgetMyGroup->onGroupInfo(groupInfo);
    }
}

void EbDialogSelectUser::onMemberInfo(const EB_MemberInfo *memberInfo, bool bChangeLineState)
{
//    const bool bIsMyDefaultMember = (memberInfo->GetEventParameter()==1)?true:false;
    const bool bIsMyGroupMember = (memberInfo->GetEventBigParameter()==1)?true:false;
    if ( m_widgetMyEnterprise!=0 ) {
        m_widgetMyEnterprise->onMemberInfo(memberInfo,bChangeLineState);
    }
    if ( m_widgetMyGroup!=0 && bIsMyGroupMember ) {
        m_widgetMyGroup->onMemberInfo(memberInfo,bChangeLineState);
    }
}

void EbDialogSelectUser::onRemoveGroup(const EB_GroupInfo *groupInfo)
{
    if (groupInfo->m_sEnterpriseCode>0 && m_widgetMyEnterprise!=0) {
        m_widgetMyEnterprise->onRemoveGroup(groupInfo);
    }
    if (groupInfo->m_nMyEmpId>0 && this->m_widgetMyGroup!=0) {
        this->m_widgetMyGroup->onRemoveGroup(groupInfo);
    }
}

void EbDialogSelectUser::deleteMemberInfo(const EB_GroupInfo *groupInfo, eb::bigint nMemberCode, bool fromDeleteGroup)
{
    if (groupInfo->m_sEnterpriseCode>0 && m_widgetMyEnterprise!=0) {
        m_widgetMyEnterprise->deleteMemberInfo(groupInfo,nMemberCode,fromDeleteGroup);
    }
    if (groupInfo->m_nMyEmpId>0 && this->m_widgetMyGroup!=0) {
        this->m_widgetMyGroup->deleteMemberInfo(groupInfo,nMemberCode,fromDeleteGroup);
    }
}

void EbDialogSelectUser::onSelectedItemInfo(const EbWidgetItemInfo::pointer & itemInfo)
{
    if ( itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_CONTACT ||
         itemInfo->m_nItemType==EbWidgetItemInfo::ITEM_TYPE_MEMBER ) {
        if (m_selectedList.exist(itemInfo->m_sAccount)) {
            return;
        }
        m_selectedList.insert(itemInfo->m_sAccount,itemInfo);
        const QIcon icon = itemInfo->m_hItem!=0?itemInfo->m_hItem->icon(0):itemInfo->m_listItem->icon();
        const QString name = itemInfo->m_hItem!=0?itemInfo->m_hItem->text(0):itemInfo->m_listItem->text();
        EbListWidgetItem * ebitem = new EbListWidgetItem( icon,name,ui->listWidgetSelected );
        ebitem->m_itemInfo = itemInfo;
        ui->listWidgetSelected->addItem(ebitem);
        updateSelectedCount();
    }

}

void EbDialogSelectUser::onItemEnteredSelected(QListWidgetItem *item)
{
    if (!ui->listWidgetSelected->hasFocus()) {
        ui->listWidgetSelected->setFocus();
    }
    /// 滚动条能正常显示
    const QRect rectItem = ui->listWidgetSelected->visualItemRect(item);
    const int y = rectItem.y()+1;
    /// -2（配合下面的 y+1）实现删除按钮显示时，保留ITEM边框，
    const int buttonSize = rectItem.height()-2;
    const EbListWidgetItem * ebitem = (EbListWidgetItem*)item;
    ui->pushButtonDelete->setGeometry( rectItem.right()-buttonSize,y+1,buttonSize,buttonSize );
    ui->pushButtonDelete->setProperty("track-item",QVariant((quint64)ebitem));
    ui->pushButtonDelete->setVisible(true);
}

void EbDialogSelectUser::updateClickedButton(const QObject *sender)
{
    ui->pushButtonMyGroup->setChecked( (sender==0 || sender==ui->pushButtonMyGroup)?true:false );
    m_widgetMyGroup->setVisible( ui->pushButtonMyGroup->isChecked() );
    ui->pushButtonMyContact->setChecked( (sender==ui->pushButtonMyContact)?true:false );
    m_widgetMyContact->setVisible( ui->pushButtonMyContact->isChecked() );
    ui->pushButtonMyEnterprise->setChecked( (sender==ui->pushButtonMyEnterprise)?true:false );
    m_widgetMyEnterprise->setVisible( ui->pushButtonMyEnterprise->isChecked() );

}

void EbDialogSelectUser::onClickedButtonMyGroup()
{
    updateClickedButton(sender());

}

void EbDialogSelectUser::onClickedButtonMyContact()
{
    updateClickedButton(sender());

}

void EbDialogSelectUser::onClickedButtonMyEnterprise()
{
    updateClickedButton(sender());

}

void EbDialogSelectUser::updateSelectedCount()
{
    const QString text = theLocales.getLocalText("select-user-dialog.label-selected.text","Selected: ");
    const int count = ui->listWidgetSelected->count();
    if ( count==0 ) {
        ui->labelSelected->setText( text );
    }
    else {
        ui->labelSelected->setText( QString("%1 (%2)").arg(text).arg(count) );
    }

}

void EbDialogSelectUser::onClickedButtonDelete()
{
    const EbListWidgetItem* ebitem = (EbListWidgetItem*)ui->pushButtonDelete->property("track-item").toULongLong();
    if (ebitem==0) {
        return;
    }
    ui->pushButtonDelete->hide();
    ui->pushButtonDelete->setProperty("track-item",QVariant((quint64)0));
    m_selectedList.remove(ebitem->m_itemInfo->m_sAccount);
    QListWidgetItem * item = ui->listWidgetSelected->takeItem( ui->listWidgetSelected->row(ebitem) );
    if (item!=0) {
        delete item;
    }
    updateSelectedCount();
}

void EbDialogSelectUser::onLoadEnterpriseInfo()
{
    theApp->m_ebum.EB_FindAllGroupInfo((CEBSearchCallback*)this,0);
    theApp->m_ebum.EB_FindAllContactInfo((CEBSearchCallback*)this);

}

void EbDialogSelectUser::onSearchEditTextChange(const QString &text)
{
//    if (m_canSearch) {
        m_widgetSearchResult->search(text.toStdString().c_str());
//    }
}

void EbDialogSelectUser::onSearchEditKeyPressEsc()
{
    if (m_widgetSearchResult->isVisible()) {
        m_widgetSearchResult->setVisible(false);
    }
}

void EbDialogSelectUser::onSearchEditKeyPressEnter(const QString &text)
{
//    if ( !text.isEmpty() ) {
//        emit searchKeyPressEnter(text);
//    }
    if (m_widgetSearchResult->isVisible()) {
        m_widgetSearchResult->hide();
    }
}

void EbDialogSelectUser::onSearchEditKeyPressDown()
{
    m_widgetSearchResult->setFocusSelectFirst();
}

//void EbDialogSelectUser::onSearchFirst(const QString &url)
//{
//    if ( !m_canUpdateSearchFirst ) {
//        return;
//    }
//    const QString text = m_lineEditSearch->text();
//    if (text.isEmpty()) {
//        return;
//    }
//    const int index = url.indexOf(text);
//    if (index<0) {
//        return;
//    }
//    /// 取后面文本用于显示
//    const QString newText = url.mid(index);
//    m_lineEditSearch->setText(newText);
//    /// 选择后面未输入内容
//    m_lineEditSearch->setSelection(text.length(),newText.length());
//}

void EbDialogSelectUser::onClickedSearchResultUrl(const QString &url)
{
    m_lineEditSearch->setText(url);
    m_lineEditSearch->selectAll();
    if (m_widgetSearchResult->isVisible()) {
        m_widgetSearchResult->setVisible(false);
    }
}

void EbDialogSelectUser::onListResultsKeyPressFirstItemUp()
{
    m_lineEditSearch->setFocus();
}

void EbDialogSelectUser::onListResultsKeyPressEsc()
{
    m_lineEditSearch->setFocus();
    m_widgetSearchResult->hide();
}

void EbDialogSelectUser::resizeEvent(QResizeEvent *e)
{
    const int const_left_intever = 15;
    const int const_width = (width()-const_left_intever*3)/2;    /// 去掉3个15，二边宽度各取一半
    const int const_search_height = 22;
    int y = titleBackgroundHeight()+15;
    int x = const_left_intever;
    m_lineEditSearch->setGeometry( x,y,const_width,const_search_height );
    x += (const_width+12);
    ui->labelSelected->setGeometry( x,y,const_width,const_search_height );
    y += (const_search_height+5);
    const int const_list_height = height()-y-60;
    ui->listWidgetSelected->setGeometry( x,y,const_width,const_list_height );
    m_widgetMyGroup->setGeometry( const_left_intever,y,const_width, const_list_height );
    m_widgetMyContact->setGeometry( const_left_intever,y,const_width, const_list_height );
    m_widgetMyEnterprise->setGeometry( const_left_intever,y,const_width, const_list_height );

    if (m_widgetSearchResult!=0) {
        int x = m_lineEditSearch->geometry().left();
        int y = m_lineEditSearch->geometry().bottom()+1;
        const int searchResultHeight = m_widgetSearchResult->height();
        m_widgetSearchResult->setGeometry( x+1,y,m_lineEditSearch->width()-2,searchResultHeight );
    }

    EbDialogBase::resizeEvent(e);
}

void EbDialogSelectUser::accept()
{
    if (m_selectedList.empty()) {
        return;
    }
    if (m_widgetSearchResult->isVisible()) {
        m_widgetSearchResult->setVisible(false);
    }
    EbDialogBase::accept();
}

bool EbDialogSelectUser::isSelectedUser(const EbWidgetItemInfo::pointer &itemInfo)
{
    if (m_singleSelect && !m_selectedList.empty()) {
        /// 单选模式
        return true;
    }
    if ( m_existedUser.exist(itemInfo->m_sAccount) ||
         m_selectedList.exist(itemInfo->m_sAccount) ) {
        return true;
    }
    return false;
}

void EbDialogSelectUser::onGroupInfo(const EB_GroupInfo *pGroupInfo, const EB_EnterpriseInfo *pEnterpriseInfo, unsigned long dwParam)
{
    if (pEnterpriseInfo!=0 && pGroupInfo->m_sEnterpriseCode>0){
        if (m_enterpriseInfo==0 || m_enterpriseInfo!=pEnterpriseInfo) {
            m_enterpriseInfo = pEnterpriseInfo;
            m_widgetMyEnterprise->onEnterpriseInfo(pEnterpriseInfo);
        }
        m_widgetMyEnterprise->onGroupInfo(pGroupInfo);
    }
    const bool bIsMyGroup = pGroupInfo->m_nMyEmpId>0?true:false;
    if (bIsMyGroup) {
        /// 这是我的群组
        m_widgetMyGroup->onGroupInfo(pGroupInfo);
    }
}

void EbDialogSelectUser::onMemberInfo(const EB_GroupInfo *pGroupInfo, const EB_MemberInfo *pMemberInfo, unsigned long dwParam)
{
    if (dwParam==0) {
        if (pGroupInfo->m_sEnterpriseCode>0){
            m_widgetMyEnterprise->onMemberInfo(pMemberInfo,false);
        }
        const bool bIsMyGroup = pGroupInfo->m_nMyEmpId>0?true:false;
        if (bIsMyGroup) {
            /// 这是我的群组
            m_widgetMyGroup->onMemberInfo(pMemberInfo,false);
        }
    }
    else {
        /// 搜索返回
    }
}

void EbDialogSelectUser::onContactInfo(const EB_ContactInfo *pContactInfo, unsigned long dwParam)
{
    if (dwParam==0) {
        m_widgetMyContact->onContactInfo(pContactInfo);
    }
    else {
        /// 搜索返回

    }
}
