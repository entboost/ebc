#include "ebdialogchangehead.h"
#include "ui_ebdialogchangehead.h"
#include "ebclientapp.h"
#include <QFileDialog>
#include <ebmessagebox.h>
#include "ebiconhelper.h"

const QSize const_emotion_size(80,80);
const QSize const_hint_size(80,80);

EbDialogChangeHead::EbDialogChangeHead(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbDialogChangeHead)
  , m_labelDefaultIcon(0)
  , m_labelCustomIcon(0)
{
    ui->setupUi(this);
    /// 设置初始大小
    this->resize(432,325);  /// 335
    /// 去掉标题栏
    this->setWindowFlags( Qt::FramelessWindowHint );

    const int const_title_height = 80;  /// 90;
    this->showTitleBackground( const_title_height );
    updateLocaleInfo();

    const QRect rectIcon(7,13,16,16);
    /// 默认头像按钮
    m_labelDefaultIcon = new QLabel(ui->pushButtonDefault);
    m_labelDefaultIcon->setObjectName("IconLabel");
    EbIconHelper::Instance()->SetIcon(m_labelDefaultIcon,QChar(0xf2be),10);
    m_labelDefaultIcon->setVisible(true);
    m_labelDefaultIcon->setGeometry( rectIcon );
    const QSize const_check_button_size(86,39+3);   /// 49+3 (3主要用于下面圆角不显示)
    ui->pushButtonDefault->setCheckable(true);
    ui->pushButtonDefault->setObjectName("TitleCheckButton");
    int x = 14;
    int y = 41;
    ui->pushButtonDefault->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
    connect( ui->pushButtonDefault,SIGNAL(clicked()),this,SLOT(onClickedPushButtonDefault()) );
    /// 自定义头像
    m_labelCustomIcon = new QLabel(ui->pushButtonCustom);
    m_labelCustomIcon->setObjectName("IconLabel");
    EbIconHelper::Instance()->SetIcon(m_labelCustomIcon,QChar(0xf1c5),10);
    m_labelCustomIcon->setVisible(true);
    m_labelCustomIcon->setGeometry( rectIcon );
    ui->pushButtonCustom->setCheckable(true);
    ui->pushButtonCustom->setObjectName("TitleCheckButton");
    x += const_check_button_size.width();
    ui->pushButtonCustom->setGeometry( x,y,const_check_button_size.width(),const_check_button_size.height() );
    connect( ui->pushButtonCustom,SIGNAL(clicked()),this,SLOT(onClickedPushButtonCustom()) );

    /// 默认头像列表
    ui->listWidgetDefaultHeads->setObjectName("ListIcon");
    ui->listWidgetDefaultHeads->setMovement(QListView::Static);
    ui->listWidgetDefaultHeads->setViewMode( QListView::IconMode );
    ui->listWidgetDefaultHeads->setIconSize(const_emotion_size);
    ui->listWidgetDefaultHeads->setSpacing(2);
    ui->listWidgetDefaultHeads->setGeometry( 6,const_title_height+5,width()-7,height()-const_title_height-44 );
    connect( ui->listWidgetDefaultHeads,SIGNAL(clicked(QModelIndex)),this,
             SLOT(onClickedListWidgetDefaultHeads(QModelIndex)) );
    loadDefaultHeadImage();
    /// 本地图片按钮
    ui->pushButtonOpenFile->setObjectName("CancelButton");
    ui->pushButtonOpenFile->setGeometry( 28,90,120,39 );
    connect( ui->pushButtonOpenFile,SIGNAL(clicked()),this,SLOT(onClickedPushButtonOpenFile()) );
    ui->labelImageFile->setGeometry( 28,138,120,120 );
    ui->labelImageFile->setText("");
    ui->labelImageFileTip->setStyleSheet("QLabel{color:rgb(237,28,36);}");
    ui->labelImageFileTip->setGeometry( ui->labelImageFile->geometry().left()+5,
                                        ui->labelImageFile->geometry().bottom()+2,80,22 );
    ui->labelImageResult->setGeometry( 28+20+120,138,120,120 );
    ui->labelImageResult->setText("");
    ui->labelImageResultTip->setStyleSheet("QLabel{color:rgb(237,28,36);}");
    ui->labelImageResultTip->setGeometry( ui->labelImageResult->geometry().left()+5,
                                          ui->labelImageResult->geometry().bottom()+2,80,22 );
    /// 默认显示
    onClickedPushButtonDefault();
}

EbDialogChangeHead::~EbDialogChangeHead()
{
    if ( !m_sNewHeadFile.isEmpty() ) {
        QFile::remove(m_sNewHeadFile);
    }
    delete ui;
}

void EbDialogChangeHead::updateLocaleInfo()
{
    const QString sDialogTitle = theLocales.getLocalText("change-head-dialog.title","Change Head");
    this->setWindowTitle( sDialogTitle );
    this->showTitleLogoText( sDialogTitle,const_common_title_font_size,QChar(0xf007),const_common_title_image_font_size );
    /// 显示右上角关闭按钮
    this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
    /// 显示 “保存/关闭” 按钮
    this->showPushButtonStdOkCancel( theLocales.getLocalText("change-head-dialog.button-ok.text","Ok"), "OkButton",
                                     theLocales.getLocalText("base-dialog.close-button.text","Close"), "CancelButton" );
    /// 默认头像按钮
    ui->pushButtonDefault->setText(  theLocales.getLocalText("change-head-dialog.button-default.text","Default") );
    ui->pushButtonDefault->setToolTip( theLocales.getLocalText("change-head-dialog.button-default.tooltip","") );
    /// 自定义头像按钮
    ui->pushButtonCustom->setText(  theLocales.getLocalText("change-head-dialog.button-custom.text","Default") );
    ui->pushButtonCustom->setToolTip( theLocales.getLocalText("change-head-dialog.button-custom.tooltip","") );
    /// 本地图片按钮
    ui->pushButtonOpenFile->setText(  theLocales.getLocalText("change-head-dialog.button-open-file.text","Default") );
    ui->pushButtonOpenFile->setToolTip( theLocales.getLocalText("change-head-dialog.button-open-file.tooltip","") );

    ui->labelImageFileTip->setText( theLocales.getLocalText("change-head-dialog.label-image-file-tip.text","Image File") );
    ui->labelImageResultTip->setText( theLocales.getLocalText("change-head-dialog.label-image-result-tip.text","Image Result") );

}

void EbDialogChangeHead::setHeadResorceFile(const QString &headResourceFile)
{
    m_sHeadResourceFile = headResourceFile;
    tstring headResourceMd5;
    mycp::bigint nFileSize = 0;
    libEbc::GetFileMd5(m_sHeadResourceFile,nFileSize,headResourceMd5);
    m_sHeadResourceMd5 = headResourceMd5.c_str();

    if ( !m_sHeadResourceFile.isEmpty() && QFileInfo::exists(m_sHeadResourceFile) ) {
        ui->labelImageFile->setPixmap( QPixmap(m_sHeadResourceFile).scaled(120,120,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
        ui->labelImageResult->setPixmap( QPixmap(m_sHeadResourceFile).scaled(120,120,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    }
}

void EbDialogChangeHead::onClickedPushButtonDefault()
{
    updateClickedPushButton(sender());
}

void EbDialogChangeHead::onClickedPushButtonCustom()
{
    updateClickedPushButton(sender());
}

void EbDialogChangeHead::updateClickedPushButton(const QObject *sender)
{
    const bool isClickedDefault = (sender==0 || sender==ui->pushButtonDefault)?true:false;
    ui->pushButtonDefault->setChecked( isClickedDefault );
    ui->listWidgetDefaultHeads->setVisible( isClickedDefault );

    ui->pushButtonCustom->setChecked( !isClickedDefault );
    ui->pushButtonOpenFile->setVisible( !isClickedDefault );
    ui->labelImageFile->setVisible( !isClickedDefault );
    ui->labelImageFileTip->setVisible( !isClickedDefault );
    ui->labelImageResult->setVisible( !isClickedDefault );
    ui->labelImageResultTip->setVisible( !isClickedDefault );
    this->setButtonOkVisible( !isClickedDefault );
}

//static QString theHeadFilter = "Image Files (*.png *.jpg *.jpeg *.bmp *.ico)";
void EbDialogChangeHead::onClickedPushButtonOpenFile()
{
    const QString caption = theLocales.getLocalText("change-head-dialog.open-file-dialog.caption","Select Image File");
    const QString filterImageFile = theLocales.getLocalText("change-head-dialog.open-file-dialog.filter","Image Files");
    const QString selectedFilter = filterImageFile + " (*.png *.jpg *.jpeg *.bmp *.ico)";
    const QString path = QFileDialog::getOpenFileName(this, caption, ".", selectedFilter);
    if ( !path.isEmpty() ) {
        const QString jpgPhotoFile = QString("%1/%2%3.jpg").arg(theApp->m_ebum.EB_GetAppDataTempPath())
                .arg(time(0)%100000000).arg(rand()%1000000);

        QImage image1(path);
        if (image1.isNull()) {
            /// 打开图片文件失败：<br>请重试！
            EbMessageBox::doShow( NULL, "", QChar::Null,
                                  theLocales.getLocalText( "message-show.open-image-error","Open Image Error" ),
                                  EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            return;
        }

        ui->labelImageFile->setPixmap( QPixmap(path).scaled(120,120,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );

        int nNewImageWidth = 0;
        {
            const int nImageWidth = image1.width();
            const int nImageHeight = image1.height();
            QImage image2;
            if (nImageWidth>nImageHeight) {
                const int x = (nImageWidth-nImageHeight)/2;
                image2 = image1.copy(x,0,nImageHeight,nImageHeight);
                nNewImageWidth = nImageHeight;
            }
            else if (nImageWidth<nImageHeight) {
                image2 = image1.copy(0,0,nImageWidth,nImageWidth);
                nNewImageWidth = nImageWidth;
            }
            else {
                image2 = image1;
                nNewImageWidth = nImageWidth;
            }
            /// 保存到 JPG 文件
            if (nNewImageWidth>2048)
                image2.save(jpgPhotoFile,0,60);
            else if (nNewImageWidth>1024)
                image2.save(jpgPhotoFile,0,80);
            else if (nNewImageWidth>512)
                image2.save(jpgPhotoFile,0,90);
            else
                image2.save(jpgPhotoFile,0,100);
        }

        if ( !m_sNewHeadFile.isEmpty() ) {
            QFile::remove(m_sNewHeadFile);
        }
        m_sNewHeadFile = jpgPhotoFile;

        if ( !QFileInfo::exists(m_sNewHeadFile) ) {
            /// 图片转换失败：\r\n请重试！
            EbMessageBox::doShow( NULL, "", QChar::Null,
                                  theLocales.getLocalText( "message-show.image-save-error","Image Save Error" ),
                                  EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
            return;
        }
        ui->labelImageResult->setPixmap( QPixmap(m_sNewHeadFile).scaled(120,120,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
    }
}

void EbDialogChangeHead::onClickedListWidgetDefaultHeads(const QModelIndex &index)
{
    QListWidgetItem * item = ui->listWidgetDefaultHeads->item(index.row());
    if (item==0) {
        return;
    }
    const qint64 resourceId = item->data(Qt::UserRole).toLongLong();
    const QString resourceFile = item->data(Qt::UserRole+1).toString();
    emit onSelectedImage( resourceId,resourceFile );
}

void EbDialogChangeHead::resizeEvent(QResizeEvent * e)
{
//    const int listHeight = height()-theLocales.titleBackgroundHeight()-30-44;
//    ui->listWidgetDefaultHeads->setGeometry( 1,90,width()-2,listHeight );

    EbDialogBase::resizeEvent(e);
}

void EbDialogChangeHead::accept()
{
    if ( m_sNewHeadFile.isEmpty() ) return;
    tstring sNewFileMd5;
    mycp::bigint nNewFileSize = 0;
    if (!libEbc::GetFileMd5(m_sNewHeadFile,nNewFileSize,sNewFileMd5)) return;
    if (m_sNewHeadFile==m_sHeadResourceFile && m_sHeadResourceMd5==sNewFileMd5.c_str()) {
        return;
    }

    if (nNewFileSize>500*1024) {	// 500KB
        /// 头像文件超过 500KB：\r\n请选择小于 500KB 图片文件后重试！
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.head-image-too-large","Head Image Too Large Error" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }

    emit onSelectedImage( 0,m_sNewHeadFile );
//    SetHeadResorceFile(m_sNewHeadFile);
//    theEBAppClient.EB_SetMyGroupHeadFile(m_sGroupCode,m_sNewHeadFile.c_str());
//    m_sNewHeadFile.clear();
//    this->GetParent()->PostMessage(EB_COMMAND_CHANGE_HEAD);

//    EbDialogBase::accept();
}

void EbDialogChangeHead::loadDefaultHeadImage()
{
//    /// for test
//    for (int i=0; i<10; i++) {
//        const QIcon icon(QPixmap::fromImage(QImage(":/res/entlogo")).scaled(const_emotion_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//        QListWidgetItem * pItem0 = new QListWidgetItem(icon,"", ui->listWidgetDefaultHeads);
//        pItem0->setSizeHint(const_hint_size);
//        ui->listWidgetDefaultHeads->addItem(pItem0 );
//    }
//    return;
    std::vector<EB_EmotionInfo> pEnterpriseImageList;
    theApp->m_ebum.EB_GetDefaultHeadList(pEnterpriseImageList);
    for (size_t i=0; i<pEnterpriseImageList.size(); i++) {
        const EB_EmotionInfo& pEmotionInfo = pEnterpriseImageList[i];
        if ( pEmotionInfo.m_sResFile.isEmpty() || !QFileInfo::exists(pEmotionInfo.m_sResFile) ) {
            continue;
        }

        const QIcon icon(QPixmap::fromImage(QImage(pEmotionInfo.m_sResFile)).scaled(const_emotion_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        QListWidgetItem * pItem0 = new QListWidgetItem(icon,"", ui->listWidgetDefaultHeads);
        pItem0->setToolTip(pEmotionInfo.m_sDescription.c_str());
        pItem0->setData( Qt::UserRole, QVariant(pEmotionInfo.m_sResId) );
        pItem0->setData( Qt::UserRole+1, QVariant(pEmotionInfo.m_sResFile) );
        pItem0->setSizeHint(const_hint_size);
        ui->listWidgetDefaultHeads->addItem(pItem0 );
    }
}

//void EbDialogChangeHead::timerEvent(QTimerEvent * e)
//{
//    EbDialogBase::timerEvent(e);
//}
