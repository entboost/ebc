#include "ebdialogpoptip.h"
#include "ui_ebdialogpoptip.h"
#include <ebdialogmainframe.h>

EbDialogPopTip::EbDialogPopTip(TipType type, QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogPopTip)
  , m_type(type)
  , m_msgId(0)
{
    ui->setupUi(this);
    m_workView = EbWidgetWorkView::create("", this);
    m_workView->setOpenLinkInNewTabAddUrlEnable(true);
    this->resize(250, 180);

    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setDoubleClickMaxEnable(true);
    this->showTitleBackground(32);
    this->setOkCancelButtonSize(QSize(55, 19));
    this->setRightInterval(8);
    this->setBottomInterval(6);

    /// 显示右上角关闭按钮
    showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );

    QString title;
    switch (m_type) {
    case AuthMessage: {
        title = theLocales.getLocalText("pop-tip-dialog.0.title","My Message");
        this->showPushButtonStdOkCancel(theLocales.getLocalText("pop-tip-dialog.0.button.text","My Message"), "RequestAddContact");
        m_pushButtonOk->setToolTip(theLocales.getLocalText("pop-tip-dialog.0.button.tooltip",""));
        const QRect &rectScreen = theApp->screenRect();
        this->move(rectScreen.width()-width()-5, (rectScreen.height()-height())/2); /// 显示在右边中间位置
        break;
    }
    case NoticeMessage: {
        title = theLocales.getLocalText("pop-tip-dialog.1.title","My Message");
        this->showPushButtonStdOkCancel(theLocales.getLocalText("pop-tip-dialog.1.button.text","My Message"), "RequestAddContact");
        m_pushButtonOk->setToolTip(theLocales.getLocalText("pop-tip-dialog.1.button.tooltip",""));
        const QRect &rectScreen = theApp->screenRect();
        this->move(rectScreen.width()-width()-5, (rectScreen.height()-height()*2)/2);   /// 显示在右边 中间靠上一点位置
        break;
    }
    case BroadcastMessage: {
        title = theLocales.getLocalText("pop-tip-dialog.2.title","Broadcast Message");
        this->showPushButtonStdOkCancel(theLocales.getLocalText("pop-tip-dialog.2.button.text","My Message"), "RequestAddContact");
        m_pushButtonOk->setToolTip(theLocales.getLocalText("pop-tip-dialog.2.button.tooltip",""));
        const QRect &rectScreen = theApp->screenRect();
#ifdef __MATH__
        this->move(rectScreen.width()-width()-5, rectScreen.height()-height()-85);  /// 显示在右下角
#else
        this->move(rectScreen.width()-width()-5, rectScreen.height()-height()-40);  /// 显示在右下角
#endif
        break;
    }
    default: {
        title = "title";
        this->showPushButtonStdOkCancel(theLocales.getLocalText("base-dialog.ok-button.text","Ok"));
        break;
    }
    }
    this->setWindowTitle(title);
    this->showTitleLogoText(title);
}

EbDialogPopTip::~EbDialogPopTip()
{
    delete ui;
}

EbDialogPopTip *EbDialogPopTip::create(EbDialogPopTip::TipType type, bool deleteOnClose, QWidget *parent)
{
    EbDialogPopTip * dlg = new EbDialogPopTip(type, parent);
    if (deleteOnClose) {
        dlg->setAttribute(Qt::WA_DeleteOnClose);
    }
    dlg->setModal(false);
    dlg->setWindowModality(Qt::WindowModal);
    return dlg;
}

void EbDialogPopTip::setPopTipMessage(const QString &message, eb::bigint msgId, const QVariant &variant1, const QVariant &variant2)
{
    m_workView->setHtml(message);
    m_msgId = msgId;
    m_variant1 = variant1;
    m_variant2 = variant2;

    switch (m_type) {
    case BroadcastMessage: {
        const QString title = m_variant2.toString();
        if (!title.isEmpty()) {
            this->setWindowTitle(title);
            this->showTitleLogoText(title);
        }
    }
    default: {
        break;
    }
    }
}

void EbDialogPopTip::accept()
{
    if (m_msgId>0) {
        theApp->m_ebum.EB_AckMsg(m_msgId,1);
        m_msgId = 0;
    }

    switch (m_type) {
    case AuthMessage:
    case NoticeMessage:
    case BroadcastMessage: {
        const eb::bigint groupMsgSubId = theApp->groupMsgSubId();
        if (groupMsgSubId==0) {
            break;
        }
        if (!theApp->mainWnd()->openSubscribeFuncWindow(groupMsgSubId, "", m_variant1.toString().toStdString())) {
            return;
        }
        break;
    }
    default: {
        break;
    }
    }
    EbDialogBase::accept();
}

void EbDialogPopTip::resizeEvent(QResizeEvent *e)
{
    const int y = titleBackgroundHeight();
    const int x = 1;
    m_workView->setGeometry(x, y, width()-x*2, height()-y-25);
    EbDialogBase::resizeEvent(e);
}
