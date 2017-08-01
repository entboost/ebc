#include "ebdialogbase.h"
#include <QPainter>
#include <QTimer>
#include "ebclientapp.h"
#include "ebiconhelper.h"
#include "../../../include/ebc_public.h"
#include "../../../include/colorconver.h"

EbDialogBase::EbDialogBase(QWidget *parent)
    : EbDialogType(parent)
    , m_bMousePress(false)
    , m_dialogFlags(EB_DIALOG_FLAG_DRAW_BORDER)
    , m_labelTitleLogo(NULL), m_labelTitleText(NULL)
    , m_widgetTitleBackground(NULL)
    , m_pushButtonSysClose(NULL), m_pushButtonSysMin(NULL), m_pushButtonSysMax(0), m_enableDoubleClickMax(false)
    , m_sizeButton(const_okcancel_button_size)
    , m_rightInterval(18), m_bottomInterval(10)
    , m_pushButtonOk(NULL), m_pushButtonCancel(NULL)
{
    m_windowRect = this->rect();
    /// 设置窗口图标
    setWindowIcon(QIcon(QStringLiteral(":/res/mainframe.ico")));

    /// 鼠标可拖动
    this->setMouseEasyMove(true);
    /// 设置背景色
//    this->setBackgroundColor(theDefaultFlatBgColor);

//    this->setAttribute(Qt::WA_TranslucentBackground, true);//设置透明2-窗体标题栏不透明,背景透明
    /// 去掉标题栏
//    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
//    this->setWindowFlags( Qt::FramelessWindowHint );
//    this->setWindowFlags (Qt::CustomizeWindowHint );  // 鼠标可以缩放窗口（***不行，会有边框）
//    this->setWindowFlags( this->windowFlags() | Qt::FramelessWindowHint);

}
EbDialogBase::~EbDialogBase(void)
{

}

void EbDialogBase::setOkCancelButtonSize(const QSize &size)
{
    m_sizeButton = size;
}

//void EbDialogBase::setBackgroundColor(QRgb rgb)
//{
//    QPalette pPaletteBg(this->palette());
//    //设置蓝色
//    // theDefaultFlatBgColor
//    pPaletteBg.setColor(QPalette::Background, QColor(rgb));
////    pPaletteBg.setColor(QPalette::Background, QColor(250,162,232));
//    this->setAutoFillBackground(true);
//    this->setPalette(pPaletteBg);
//}

const int const_dialog_title_x = 7;
const int const_dialog_title_y = 7;
const int const_dialog_title_icon_size = 18;
void EbDialogBase::showTitleLogoText(const QString& textString, int /*textSize*/, QChar logoChar, int logoSize, const QString& titleTextObjectName)
{
    if (m_labelTitleLogo==NULL && logoChar!=QChar::Null) {
        m_labelTitleLogo = new QLabel(this);
        EbIconHelper::Instance()->SetIcon(m_labelTitleLogo,logoChar,logoSize);
        m_labelTitleLogo->setGeometry( const_dialog_title_x,const_dialog_title_y,const_dialog_title_icon_size,const_dialog_title_icon_size );
        m_labelTitleLogo->setObjectName("DialogTitleLogo");
    }

    if (m_labelTitleText==NULL) {
        m_labelTitleText = new QLabel(textString, this);
        const int x = m_labelTitleLogo==NULL?const_dialog_title_x:(const_dialog_title_x+const_dialog_title_icon_size+8);
        m_labelTitleText->setGeometry( x,const_dialog_title_y,200,const_dialog_title_icon_size );   // 18
        if (!titleTextObjectName.isEmpty()) {
            m_labelTitleText->setObjectName(titleTextObjectName);
        }
//        m_labelTitleText->setObjectName("DialogTitleText");
    }
    else {
        m_labelTitleText->setText(textString);
    }
}

void EbDialogBase::showTitleBackground(int height,const QString& objectName)
{
    if (m_widgetTitleBackground==NULL) {
        m_widgetTitleBackground = new QWidget(this);
        m_widgetTitleBackground->setObjectName(objectName);
    }
    m_widgetTitleBackground->lower();
    /// 显示高度
    m_widgetTitleBackground->setGeometry( 0, 0, rect().width(), height );
//    /// 显示背景颜色
//    QPalette pal(m_widgetTitleBackground->palette());
//    pal.setColor(QPalette::Background, color);
//    m_widgetTitleBackground->setAutoFillBackground(true);
    //    m_widgetTitleBackground->setPalette(pal);
}

int EbDialogBase::titleBackgroundHeight() const
{
    return (m_widgetTitleBackground==0)?0:m_widgetTitleBackground->height();
}
void EbDialogBase::showPushButtonSysMin(const QString& sTooltip, const QString& objectName)
{
#ifdef __MACH__
#else
    if (m_pushButtonSysMin==NULL) {
        m_pushButtonSysMin = new QPushButton(this);
//        m_pushButtonSysMin->setVisible(true);
        m_pushButtonSysMin->raise();
        EbIconHelper::Instance()->SetIcon( m_pushButtonSysMin,QChar(0xf2d1),9-2 );
        int x = this->rect().width()-const_sys_button_size.width()-const_sys_button_right_interval;
        if (m_pushButtonSysClose!=NULL)
            x -= const_sys_button_size.width();
        if (m_pushButtonSysMax!=NULL)
            x -= const_sys_button_size.width();
        m_pushButtonSysMin->setGeometry( x,0,const_sys_button_size.width(),const_sys_button_size.height() );
        connect( m_pushButtonSysMin,SIGNAL(clicked()),this,SLOT(onClickedPushButtonSysMin()) );
    }
//    else {
//        const int x = this->rect().width()-m_pushButtonSysMin->width();
//        m_pushButtonSysMin->move(x, 0);
//    }

    m_pushButtonSysMin->setObjectName(objectName);
//    m_pushButtonSysMin->setStyleSheet(sStyleSheet);
    m_pushButtonSysMin->setToolTip(sTooltip);
#endif
}
void EbDialogBase::showPushButtonSysMax(const QString& sTooltip, const QString& objectName)
{
#ifdef __MACH__
#else
    if (m_pushButtonSysMax==NULL) {
        m_pushButtonSysMax = new QPushButton(this);
//        m_pushButtonSysMax->setVisible(true);
        m_pushButtonSysMax->raise();
        EbIconHelper::Instance()->SetIcon( m_pushButtonSysMax,QChar(0xf2d0),9-2 );
        int x = this->rect().width()-const_sys_button_size.width()-const_sys_button_right_interval;
        if (m_pushButtonSysClose!=NULL)
            x -= const_sys_button_size.width();
        m_pushButtonSysMax->setGeometry( x,0,const_sys_button_size.width(),const_sys_button_size.height() );
        connect( m_pushButtonSysMax,SIGNAL(clicked()),this,SLOT(onClickedPushButtonSysMax()) );
    }
//    else {
//        const int x = this->rect().width()-m_pushButtonSysMax->width();
//        m_pushButtonSysMax->move(x, 0);
//    }
    m_pushButtonSysMax->setObjectName(objectName);
//    m_pushButtonSysMax->setStyleSheet(sStyleSheet);
    m_pushButtonSysMax->setToolTip(sTooltip);
#endif
}

void EbDialogBase::showPushButtonSysClose(const QString& sTooltip,const QString& objectName)
{
#ifdef __MACH__
#else
    if (m_pushButtonSysClose==NULL) {
        m_pushButtonSysClose = new QPushButton(this);
//        m_pushButtonSysClose->setVisible(true);
        m_pushButtonSysClose->setObjectName(objectName);
        m_pushButtonSysClose->setToolTip(sTooltip);
        m_pushButtonSysClose->raise();
        EbIconHelper::Instance()->SetIcon( m_pushButtonSysClose,QChar(0xf00d),9 );
        const int x = this->rect().width()-const_sys_button_size.width()-const_sys_button_right_interval;
        m_pushButtonSysClose->setGeometry( x,0,const_sys_button_size.width(),const_sys_button_size.height() );
        connect( m_pushButtonSysClose,SIGNAL(clicked()),this,SLOT(reject()) );
    }
//    else {
//        const int x = this->rect().width()-m_pushButtonSysClose->width();
//        m_pushButtonSysClose->move(x, 0);
//    }
#endif
}

int EbDialogBase::getSysButtonWidth(void) const
{
    int ret = const_sys_button_right_interval;
    if (m_pushButtonSysMin!=NULL) {
        ret += m_pushButtonSysMin->width();
    }
    if (m_pushButtonSysMax!=NULL) {
        ret += m_pushButtonSysMax->width();
    }
    if (m_pushButtonSysClose!=NULL) {
        ret += m_pushButtonSysClose->width();
    }
    return ret;
}
void EbDialogBase::showPushButtonStdOkCancel(const QString& sOkText, const QString& okObjectName,
                                             const QString& sCancelText, const QString& cancelObjectName)
{
    if (m_pushButtonCancel==NULL && !sCancelText.isEmpty()) {
        m_pushButtonCancel = new QPushButton(sCancelText, this);
        m_pushButtonCancel->resize(m_sizeButton);
        connect( m_pushButtonCancel,SIGNAL(clicked()),this,SLOT(reject()) );
    }
    int x = this->rect().width()-m_sizeButton.width()-m_rightInterval;   /// *最右边间隔
    const int y = this->rect().height()-m_sizeButton.height()-m_bottomInterval; /// *最下面间隔
    if (m_pushButtonCancel!=NULL) {
        m_pushButtonCancel->setObjectName(cancelObjectName);
//        m_pushButtonCancel->setStyleSheet(sStyleSheetCancel);
        m_pushButtonCancel->move(x,y);

        x -= (m_sizeButton.width()+10);
    }

    if (m_pushButtonOk==NULL && !sOkText.isEmpty()) {
        m_pushButtonOk = new QPushButton(sOkText, this);
        m_pushButtonOk->resize(m_sizeButton);
        /// 设置默认回车响应按键
//        m_pushButtonOk->setShortcut(Qt::Key_Enter); //设置快捷键为enter键
//        m_pushButtonOk->setShortcut(Qt::Key_Return); //设置快捷键为小键盘上的enter键
        m_pushButtonOk->setDefault(true);
        connect( m_pushButtonOk,SIGNAL(clicked()),this,SLOT(accept()) );
    }
    if (m_pushButtonOk!=NULL) {
        m_pushButtonOk->setObjectName(okObjectName);
//        m_pushButtonOk->setStyleSheet(sStyleSheetOk);
        m_pushButtonOk->move(x,y);
    }
}

void EbDialogBase::setButtonStdOkTooltip(const QString &tooltip)
{
    if (m_pushButtonOk!=0) {
        m_pushButtonOk->setToolTip(tooltip);
    }
}

void EbDialogBase::setMouseEasyMove(bool bEnable)
{
    if (bEnable) {
        m_dialogFlags |= EB_DIALOG_FLAG_EASY_MOVE;
    }
    else {
        m_dialogFlags &= ~EB_DIALOG_FLAG_EASY_MOVE;
    }
}
void EbDialogBase::setFilterEscapeKey(bool b)
{
    if (b) {
        m_dialogFlags |= EB_DIALOG_FLAG_FILTER_ESC_KEY;
    }
    else {
        m_dialogFlags &= ~EB_DIALOG_FLAG_FILTER_ESC_KEY;
    }
}
void EbDialogBase::setFilterEnterKey(bool b)
{
    if (b) {
        m_dialogFlags |= EB_DIALOG_FLAG_FILTER_ENTER_KEY;
    }
    else {
        m_dialogFlags &= ~EB_DIALOG_FLAG_FILTER_ENTER_KEY;
    }
}

void EbDialogBase::setDrawBorder(bool b)
{
    if (b) {
        m_dialogFlags |= EB_DIALOG_FLAG_DRAW_BORDER;
    }
    else {
        m_dialogFlags &= ~EB_DIALOG_FLAG_DRAW_BORDER;
    }
}


void EbDialogBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;
    }

    if (isMouseEasyMove()) {
        /// 窗口移动距离
        move_point = event->globalPos() - this->pos();
    }
    EbDialogType::mousePressEvent(event);
}

void EbDialogBase::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (m_enableDoubleClickMax && e->button()==Qt::LeftButton) {
        onClickedPushButtonSysMax();
        return;
    }
    EbDialogType::mouseDoubleClickEvent(e);
}

void EbDialogBase::mouseReleaseEvent(QMouseEvent * e)
{
    m_bMousePress = false;
    EbDialogType::mouseReleaseEvent(e);
}

void EbDialogBase::mouseMoveEvent(QMouseEvent *event)
{
    if (isMouseEasyMove() && m_bMousePress) {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
    EbDialogType::mouseMoveEvent(event);
}

void EbDialogBase::paintEvent(QPaintEvent *event)
{
    if (isDrawBorder()) {
        QPainter painter(this);
        /// 绘制边框
        painter.setPen(QColor(GetRValue(theDefaultFlatLine2Color), GetGValue(theDefaultFlatLine2Color), GetBValue(theDefaultFlatLine2Color)));
#ifndef __MACH__    /// MAC 不画上边框线
        painter.drawLine(0, 0, this->width() - 1, 0);
#endif
        painter.drawLine(0, 0, 0, this->height() - 1);
        painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height() - 1);
        painter.drawLine(0, this->height() - 1, this->width() - 1, this->height() - 1);

        //    QPainter painter(this);
        //    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
        ////    painter.setBrush(QBrush(Qt::red));
        //    painter.setPen(Qt::transparent);
        //    QRect rect = this->rect();
        //    rect.setWidth(rect.width() - 1);
        //    rect.setHeight(rect.height() - 1);
        //    painter.drawRoundedRect(rect, 15, 15);
        ////    //也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
        ////    {
        ////        QPainterPath painterPath;
        ////        painterPath.addRoundedRect(rect, 15, 15);
        ////        painter.drawPath(painterPath);
        ////    }
        ///
    }
    EbDialogType::paintEvent(event);
}

void EbDialogBase::keyPressEvent(QKeyEvent * e)
{
    if ( isFilterEscapeKey() && e->key()==Qt::Key_Escape) {
        return;
    }
    else if ( isFilterEnterKey() && (e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return)) {
        return;
    }
    EbDialogType::keyPressEvent(e);
}

void EbDialogBase::resizeEvent(QResizeEvent *e)
{
    if (m_widgetTitleBackground!=NULL) {
        const QSize size = m_widgetTitleBackground->size();
        m_widgetTitleBackground->setGeometry( 0, 0,e->size().width(),size.height() );
    }

    int x = e->size().width()-const_sys_button_right_interval;
    if (m_pushButtonSysClose!=NULL) {
        x -= m_pushButtonSysClose->width();
        m_pushButtonSysClose->move(x, 0);
    }
    if (m_pushButtonSysMax!=NULL) {
        x -= m_pushButtonSysMax->width();
        m_pushButtonSysMax->move(x, 0);
    }
    if (m_pushButtonSysMin!=NULL) {
        x -= m_pushButtonSysMin->width();
        m_pushButtonSysMin->move(x, 0);
    }

    EbDialogType::resizeEvent(e);
}

void EbDialogBase::setButtonOkEnabled(bool bEnable)
{
    if (m_pushButtonOk!=0) {
        m_pushButtonOk->setEnabled(bEnable);
    }
}

void EbDialogBase::setButtonOkVisible(bool visible)
{
    if (m_pushButtonOk!=0) {
        m_pushButtonOk->setVisible(visible);
    }
}

void EbDialogBase::setButtonCancelVisible(bool visible)
{
    if (m_pushButtonCancel!=0) {
        m_pushButtonCancel->setVisible(visible);
    }
}

void EbDialogBase::onClickedPushButtonSysMin(void)
{
    if (onBeforeClickedPushButtonSysMin()) {
        this->showMinimized();
    }
}

bool EbDialogBase::isMaximizedEb(void) const
{
    const QRect& rect = this->rect();
    const QRect& deskRect = theApp->deskRect();
#ifdef __MACH__
    return (rect.width()==deskRect.width() && rect.height()+30>deskRect.height())?true:false; /// ?
#else
    return (rect.width()==deskRect.width() && rect.height()==deskRect.height())?true:false;
#endif
}

void EbDialogBase::setMaxRestoreIcon(bool max)
{
    if (m_pushButtonSysMax==NULL) {
        return;
    }
    if (max) {
        /// 还原按钮
        EbIconHelper::Instance()->SetIcon( m_pushButtonSysMax,QChar(0xf2d0),9-2 );
    }
    else {
        /// 最大化按钮
        EbIconHelper::Instance()->SetIcon( m_pushButtonSysMax,QChar(0xf2d2),9-2 );
    }
}
void EbDialogBase::onClickedPushButtonSysMax(void)
{
    if (!onBeforeClickedPushButtonSysMax()) {
        return;
    }
    if (this->isMaximizedEb()) {
        this->setGeometry(m_windowRect);
        /// 还原窗口后，显示最大化按钮
        setMaxRestoreIcon(true);
    }
    else {
        const QRect& rect = this->rect();
        const QPoint point = this->mapToGlobal(rect.topLeft());
        m_windowRect = QRect(point,rect.size());
        this->setGeometry(theApp->deskRect());
//        this->showMaximized();
        /// 最大化窗口后，显示还原按钮
        setMaxRestoreIcon(false);
    }
}

