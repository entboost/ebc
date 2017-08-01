#include "ebdialogemotionselect.h"
#include "ui_ebdialogemotionselect.h"

//const int const_dialog_emotion_select_width = 400;
//const int const_dialog_emotion_select_height = 400;

const QSize const_emotion_size(32,32);
const QSize const_size_hint(32,32);
const int const_emotion_top_inteval = 0;	// 上边间隔
const int const_emotion_h_inteval = 0;		// 左右二边间隔
const int const_emotion_h_count = 12;		// 水平数量
const int const_emotion_v_count = 6;		// 垂直数量
const int const_emotion_inteval = 1;		// 格线间隔

EbDialogEmotionSelect::EbDialogEmotionSelect(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogEmotionSelect)
  , m_timerIdLoadEmotion(0)
  , m_hasEmptyFile(true)
{
    ui->setupUi(this);

    /// 设置初始大小
    const int titleHeight = theLocales.titleBackgroundHeight();
    const int const_dialog_width = const_emotion_h_inteval*2+const_emotion_h_count*(const_size_hint.width()+const_emotion_inteval)+12;
    const int const_dialog_height = titleHeight+const_emotion_top_inteval+const_emotion_v_count*(const_size_hint.height()+const_emotion_inteval)+3;
//    const int const_dialog_width = theLocales.getLocalInt("emotion-select-dialog.window-size.width", 288);
//    const int const_dialog_height = theLocales.getLocalInt("emotion-select-dialog.window-size.height", 588);
    const QSize const_dialog_size(const_dialog_width,const_dialog_height);
    this->resize(const_dialog_size);
    /// 去掉标题栏 & 和设置标题高度
    this->setWindowFlags( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );
    this->showTitleBackground( titleHeight );

    updateLocaleInfo();

    /// 实现捕捉失去焦点事件，能够判断是否隐藏当前窗口
    ui->listWidgetEmotion->setObjectName("ListIcon");
    ui->listWidgetEmotion->setMovement(QListView::Static);
    ui->listWidgetEmotion->setViewMode( QListView::IconMode );
    ui->listWidgetEmotion->setIconSize(const_emotion_size);
    ui->listWidgetEmotion->setSpacing(const_emotion_inteval);
    ui->listWidgetEmotion->setMouseTracking(true);
    ui->listWidgetEmotion->installEventFilter(this);
    connect( ui->listWidgetEmotion, SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onItemClicked(QListWidgetItem*)));
    connect( ui->listWidgetEmotion, SIGNAL(itemEntered(QListWidgetItem*)),this,SLOT(onItemEntered(QListWidgetItem*)));
    m_timerIdLoadEmotion = this->startTimer(3000);

//    /// for test data
//    for (int i=0; i<100; i++) {
//        const QIcon icon(QPixmap::fromImage(QImage(":/res/entlogo")).scaled(const_emotion_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//        QListWidgetItem * pItem0 = new QListWidgetItem(icon,"", ui->listWidgetEmotion);
//        pItem0->setToolTip("abc");
//        pItem0->setData( Qt::UserRole, QVariant("cm-info") );
//        pItem0->setSizeHint(const_emotion_size);
//        ui->listWidgetEmotion->addItem(pItem0 );
//    }

}

EbDialogEmotionSelect::~EbDialogEmotionSelect()
{
    if (m_timerIdLoadEmotion!=0) {
        this->killTimer(m_timerIdLoadEmotion);
        m_timerIdLoadEmotion = 0;
    }
    delete ui;
}

void EbDialogEmotionSelect::updateLocaleInfo()
{
    const QString sDialogTitle = theLocales.getLocalText("emotion-select-dialog.title","默认表情");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size );

}

void EbDialogEmotionSelect::onItemClicked(QListWidgetItem *item)
{
    const QIcon icon = item->icon();
    const mycp::bigint resourceId = item->data(Qt::UserRole).toLongLong();
    const QString resourceCmInfo = item->data(Qt::UserRole+1).toString();
    const QString resourceFilePath = item->data(Qt::UserRole+2).toString();
    emit selectedResource( icon,resourceId,resourceCmInfo,resourceFilePath );
//    this->setFocus();
    this->setVisible(false);
}

void EbDialogEmotionSelect::onItemEntered(QListWidgetItem *item)
{
//    const int index = item->data(Qt::UserRole).toInt();

}

void EbDialogEmotionSelect::resizeEvent(QResizeEvent * e)
{
    const int titleHeight = theLocales.titleBackgroundHeight();
    ui->listWidgetEmotion->setGeometry( 1,titleHeight,width()-2,height()-titleHeight-1 );
    EbDialogBase::resizeEvent(e);
}

void EbDialogEmotionSelect::checkFocusOut()
{
    if (!this->hasFocus() && !ui->listWidgetEmotion->hasFocus()) {
        if (!this->geometry().contains( cursor().pos() )) {
            this->setVisible( false );
        }
    }
}

void EbDialogEmotionSelect::focusOutEvent(QFocusEvent *event)
{
    checkFocusOut();
    EbDialogBase::focusOutEvent(event);

}

bool EbDialogEmotionSelect::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->listWidgetEmotion) {
        if (e->type() == QEvent::FocusOut) {
            checkFocusOut();
            return true;
        }
        else {
            return false;
        }
    }
    else {
        // pass the event on to the parent class
        return EbDialogBase::eventFilter(obj, e);
    }
}

void EbDialogEmotionSelect::loadEmotion()
{
    if (ui->listWidgetEmotion->count()==0 || m_hasEmptyFile) {	// 前面有空，需要判断多一次
        std::vector<EB_EmotionInfo> pEnterpriseImageList;
        theApp->m_ebum.EB_GetMyEmotionList(pEnterpriseImageList);
        m_hasEmptyFile = false;
        for (size_t i=0; i<pEnterpriseImageList.size(); i++) {
            const EB_EmotionInfo& pEmotionInfo = pEnterpriseImageList[i];
            if (pEmotionInfo.m_sResFile.isEmpty()) {
                m_hasEmptyFile = true;
                break;
            }
            if ( !QFileInfo::exists(pEmotionInfo.m_sResFile) ) {
                m_hasEmptyFile = true;
                break;
            }
            if (!m_loadedResourceIdList.insert(pEmotionInfo.m_sResId,true,false)) {
                /// 前面已经添加，取下一个
                continue;
            }
            const QIcon icon(QPixmap::fromImage(QImage(pEmotionInfo.m_sResFile)).scaled(const_emotion_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            QListWidgetItem * pItem0 = new QListWidgetItem(icon,"", ui->listWidgetEmotion);
            pItem0->setToolTip(pEmotionInfo.m_sDescription.c_str());
            tstring sResourceCmInfo;
            theApp->m_ebum.EB_GetResourceCmInfo(pEmotionInfo.m_sResId,sResourceCmInfo);
            pItem0->setData( Qt::UserRole, QVariant(pEmotionInfo.m_sResId) );
            pItem0->setData( Qt::UserRole+1, QVariant(sResourceCmInfo.c_str()) );
            pItem0->setData( Qt::UserRole+2, QVariant(pEmotionInfo.m_sResFile) );
            pItem0->setSizeHint(const_size_hint);
            ui->listWidgetEmotion->addItem(pItem0 );
        }
    }
}

void EbDialogEmotionSelect::timerEvent(QTimerEvent *event)
{
    if (m_timerIdLoadEmotion!=0 && m_timerIdLoadEmotion==event->timerId()) {
        loadEmotion();
        if (ui->listWidgetEmotion->count()>0 && !m_hasEmptyFile) {
            this->killTimer(m_timerIdLoadEmotion);
            m_timerIdLoadEmotion = 0;
        }
    }

    EbDialogBase::timerEvent(event);
}
