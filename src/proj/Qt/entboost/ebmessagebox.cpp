#include "ebmessagebox.h"
#include "ui_ebmessagebox.h"
#include "ebclientapp.h"

EbMessageBox::EbMessageBox(QWidget *parent) :
    EbDialogBase(parent),
    ui(new Ui::EbMessageBox)

  , m_icon(QChar::Null)
  , m_imageType(IMAGE_INFORMATION)
  , m_buttonType(QMessageBox::NoButton)
  , m_autoClose(0)

//  , labelImage(NULL)
//  , labelText(NULL)

  , m_closeTimerId(0)
  , m_closeIndex(0)

{
    ui->setupUi(this);
    /// 去掉标题栏
#ifdef __MACH__
    this->setWindowFlags( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );
#else
    this->setWindowFlags( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );
#endif
//    this->setWindowFlags( Qt::Dialog|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint );
//    // 设置位置，显示在上面
//    this->showTitleBackground(theDefaultBgColorTitleHeight);

//    QFont font = ui->labelText->font();
//    font.setPointSize(10);
//    ui->labelText->setFont(font);
    ui->labelText->setWordWrap(true);
//    背景透明
    ui->labelText->setStyleSheet("QLabel{background-color:rgb(0,0,0,0); color: rgb(64,64,64); border-radius: 0px;}");
}

EbMessageBox::~EbMessageBox()
{
    if ( m_closeTimerId!=0 ) {
        killTimer(m_closeTimerId);
        m_closeTimerId = 0;
    }
    delete ui;
}

 int EbMessageBox::doExec(QWidget *parent, const QString& title, QChar icon, const QString& text, IMAGE_TYPE imageType,
                          int autoClose, int nButtonType)
 {
     EbMessageBox pEbMessageBox(parent);
     pEbMessageBox.m_title = title;
     pEbMessageBox.m_icon = icon;
     pEbMessageBox.m_text = text;
     pEbMessageBox.m_imageType = imageType;
     pEbMessageBox.m_buttonType = nButtonType;
     pEbMessageBox.m_autoClose = autoClose;
     return pEbMessageBox.exec();
 }
 void EbMessageBox::doShow(QWidget* parent, const QString& title, QChar icon, const QString& text, IMAGE_TYPE imageType,
                           int autoClose, int /*nButtonType*/)
 {
     EbMessageBox * pEbMessageBox = new EbMessageBox(parent);
//     if (parent==NULL) {
         pEbMessageBox->setAttribute(Qt::WA_DeleteOnClose);
//     }
     pEbMessageBox->m_title = title;
     pEbMessageBox->m_icon = icon;
     pEbMessageBox->m_text = text;
     pEbMessageBox->m_imageType = imageType;
     pEbMessageBox->m_buttonType = QMessageBox::NoButton;
     pEbMessageBox->m_autoClose = autoClose<=0?default_warning_auto_close:autoClose;
//     pEbMessageBox->m_buttonType = nButtonType;
//     pEbMessageBox->m_autoClose = autoClose;
     pEbMessageBox->setModal(false);
     pEbMessageBox->setWindowModality(Qt::WindowModal);
//     pEbMessageBox->setWindowFlags(pEbMessageBox->windowFlags()|Qt::WindowStaysOnTopHint);
     pEbMessageBox->setFocus();
     pEbMessageBox->init();
     pEbMessageBox->show();
 }

 void EbMessageBox::init()
 {
     this->setWindowTitle( m_title );

     // 显示右上角关闭按钮
     if (m_buttonType==QMessageBox::NoButton) {
         const QSize const_dialog_size(360,88);
         this->resize(const_dialog_size);
//         this->setAttribute(Qt::WA_TranslucentBackground, true);//设置透明2-窗体标题栏不透明,背景透明

         const int const_image_x = 12;
         const int const_image_y = 24;
         ui->labelImage->setGeometry( const_image_x,const_image_y,const_image_size.width(),const_image_size.height() );
         const int x = const_image_x+const_image_size.width()+8;
         ui->labelText->setGeometry( x,3,const_dialog_size.width()-x-5,const_dialog_size.height()-8);
         ui->labelText->setAlignment( Qt::AlignLeft|Qt::AlignVCenter );
     }
     else {
         const QSize const_dialog_size(380,201);
         this->resize(const_dialog_size);

         const int const_image_x = 38;
         const int const_image_y = 68;
     //     const int const_image_y = m_closeOnly?48:68;
         ui->labelImage->setGeometry( const_image_x,const_image_y,const_image_size.width(),const_image_size.height() );
         const int x = const_image_x+const_image_size.width()+10;
         ui->labelText->setGeometry( x,const_image_y,const_dialog_size.width()-x-10,const_dialog_size.height()-const_image_y-20);

         this->showTitleLogoText( m_title,const_common_title_font_size,m_icon,const_common_title_image_font_size );
         // 设置位置，显示在上面
         this->showTitleBackground( theLocales.titleBackgroundHeight() );
         this->showPushButtonSysClose( theLocales.getLocalText("base-dialog.close-button.tooltip","Close") );
     }
     // 显示 1=Ok/Yes/Retry/Save/Open/Abort” 2=“Cancel/Close/No/Ignore
     QString sOkText;
     QString sCancelText;
     if ((m_buttonType&QMessageBox::Ok)!=0)
         sOkText = theLocales.getLocalText("base-dialog.ok-button.text","Ok");
     else if ((m_buttonType&QMessageBox::Yes)!=0)
         sOkText = theLocales.getLocalText("base-dialog.yes-button.text","Yes");
     else if ((m_buttonType&QMessageBox::Retry)!=0)
         sOkText = theLocales.getLocalText("base-dialog.retry-button.text","Retry");
     else if ((m_buttonType&QMessageBox::Save)!=0)
         sOkText = theLocales.getLocalText("base-dialog.save-button.text","Save");
     else if ((m_buttonType&QMessageBox::Open)!=0)
         sOkText = theLocales.getLocalText("base-dialog.open-button.text","Open");
     else if ((m_buttonType&QMessageBox::Abort)!=0)
         sOkText = theLocales.getLocalText("base-dialog.abort-button.text","Abort");

     if ((m_buttonType&QMessageBox::Cancel)!=0)
         sCancelText = theLocales.getLocalText("base-dialog.cancel-button.text","Cancel");
     else if ((m_buttonType&QMessageBox::Close)!=0)
         sCancelText = theLocales.getLocalText("base-dialog.close-button.text","Close");
     else if ((m_buttonType&QMessageBox::No)!=0)
         sCancelText = theLocales.getLocalText("base-dialog.no-button.text","No");
     else if ((m_buttonType&QMessageBox::Ignore)!=0)
         sCancelText = theLocales.getLocalText("base-dialog.ignore-button.text","Ignore");

     this->showPushButtonStdOkCancel( sOkText, "OkButton", sCancelText, "CancelButton" );

     switch (m_imageType) {
     case IMAGE_INFORMATION:
         ui->labelImage->setPixmap( QPixmap(":/res/imgtip").scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
         break;
     case IMAGE_QUESTION:
         ui->labelImage->setPixmap( QPixmap(":/res/imgquestion").scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
         break;
     case IMAGE_WARNING:
         ui->labelImage->setPixmap( QPixmap(":/res/imgwarning").scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
         break;
     case IMAGE_ERROR:
         ui->labelImage->setPixmap( QPixmap(":/res/imgerror").scaled(const_image_size,Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
         break;
     default:
         break;
     }
     if (m_buttonType==QMessageBox::NoButton && !m_title.isEmpty()) {
         ui->labelText->setText(m_title+"<br>"+m_text);
     }
     else {
         ui->labelText->setText(m_text);
     }
     if ( m_autoClose>0 && m_closeTimerId==0 ) {
         m_closeTimerId = this->startTimer(1000);
     }

 }

 int EbMessageBox::exec()
 {
     init();
     return EbDialogBase::exec();
 }

 void EbMessageBox::timerEvent(QTimerEvent *event)
 {
     if (event->timerId()==m_closeTimerId) {
         if ((++m_closeIndex)>=m_autoClose) {
             this->killTimer(m_closeTimerId);
             m_closeTimerId = 0;
             this->reject();
             return;
         }
     }
     EbDialogBase::timerEvent(event);
 }
