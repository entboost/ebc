#include "ebframefindtext.h"
#include "ebiconhelper.h"
#include "eblineedit.h"
#include "ebclientapp.h"

const QSize const_button_size(22,22);
EbFrameFindText::EbFrameFindText(QWidget *parent) : QFrame(parent)
  , m_timerIdTextChangeFind(0)
{
    this->setObjectName("FindTextFrame");

    m_lineEditFindText = new EbLineEdit(this);
    connect( m_lineEditFindText,SIGNAL(keyPressEsc()),this,SLOT(onKeyPressEscFindText()) );
    connect( m_lineEditFindText,SIGNAL(keyPressEnter(QString)),this,SLOT(onKeyPressEnter(QString)) );
    connect( m_lineEditFindText,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged(QString)) );

    m_labelStatus = new QLabel(this);

    m_buttonFindUp = new QPushButton(this);
    m_buttonFindUp->resize(const_button_size);
    m_buttonFindUp->setObjectName("SearchBarButton");
    EbIconHelper::Instance()->SetIcon(m_buttonFindUp,QChar(0xf077),9);
    connect( m_buttonFindUp,SIGNAL(clicked()),this,SLOT(onClickedButtonFindUp()) );
    m_buttonFindDown = new QPushButton(this);
    m_buttonFindDown->resize(const_button_size);
    m_buttonFindDown->setObjectName("SearchBarButton");
    EbIconHelper::Instance()->SetIcon(m_buttonFindDown,QChar(0xf078),9);
    connect( m_buttonFindDown,SIGNAL(clicked()),this,SLOT(onClickedButtonFindDown()) );
    m_buttonClose = new QPushButton(this);
    m_buttonClose->resize(const_button_size);
    m_buttonClose->setObjectName("SearchBarCloseButton");
    EbIconHelper::Instance()->SetIcon(m_buttonClose,QChar(0xf00d),9);
    connect( m_buttonClose,SIGNAL(clicked()),this,SLOT(onClickedButtonClose()) );

    updateLocaleInfo();
}

void EbFrameFindText::showFocusInput(const QString &selectedText)
{
    m_lineEditFindText->setText(selectedText);
    if ( !selectedText.isEmpty() ) {
        m_lineEditFindText->selectAll();
    }
    m_lineEditFindText->setFocus();
    if ( !this->isVisible() ) {
        this->show();
    }
}

void EbFrameFindText::updateLocaleInfo()
{
    m_lineEditFindText->setPlaceholderText( theLocales.getLocalText("find-text-frame.edit-search.bg-text","") );
    m_buttonFindUp->setToolTip( theLocales.getLocalText("find-text-frame.button-up.toolip","") );
    m_buttonFindDown->setToolTip( theLocales.getLocalText("find-text-frame.button-down.toolip","") );
    m_buttonClose->setToolTip( theLocales.getLocalText("find-text-frame.button-close.toolip","") );
}

void EbFrameFindText::onKeyPressEscFindText()
{
    onClickedButtonClose();
}

void EbFrameFindText::onKeyPressEnter(const QString &text)
{
    emit findText(false,text);
}

void EbFrameFindText::onTextChanged(const QString &text)
{
    m_buttonFindUp->setEnabled( !text.isEmpty() );
    m_buttonFindDown->setEnabled( !text.isEmpty() );
    if (m_timerIdTextChangeFind!=0) {
        this->killTimer(m_timerIdTextChangeFind);
        m_timerIdTextChangeFind = 0;
    }
    if ( !text.isEmpty() ) {
        m_timerIdTextChangeFind = this->startTimer(500);
    }

}

void EbFrameFindText::onClickedButtonFindUp()
{
    emit findText( true,this->m_lineEditFindText->text() );
    this->m_lineEditFindText->setFocus();
}

void EbFrameFindText::onClickedButtonFindDown()
{
    emit findText( false,this->m_lineEditFindText->text() );
    this->m_lineEditFindText->setFocus();
}

void EbFrameFindText::onClickedButtonClose()
{
    emit exitFindText();
    this->hide();
}

void EbFrameFindText::resizeEvent(QResizeEvent *event)
{
    int x = 6;
    int y = 8;
    int w = width()-const_button_size.width()*3-x;
    m_lineEditFindText->setGeometry( x,y,w,22 );
    x = m_lineEditFindText->geometry().right();
    y -= 1;
    m_buttonFindUp->move( x,y );
    x += const_button_size.width();
    m_buttonFindDown->move( x,y );
    x += const_button_size.width();
    m_buttonClose->move( x,y );

    QFrame::resizeEvent(event);
}

void EbFrameFindText::timerEvent(QTimerEvent *e)
{
    if ( m_timerIdTextChangeFind!=0 && m_timerIdTextChangeFind==e->timerId() ) {
        this->killTimer(m_timerIdTextChangeFind);
        m_timerIdTextChangeFind = 0;
        emit findText( false,m_lineEditFindText->text() );
    }

    QFrame::timerEvent(e);
}
