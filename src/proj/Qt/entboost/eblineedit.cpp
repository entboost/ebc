#include "eblineedit.h"
#include <QTimer>

EbLineEdit::EbLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_mouseFocusInSelectAll(false)
{

}

void EbLineEdit::mouseFocusInSelectAll()
{
    this->selectAll();
}

void EbLineEdit::focusInEvent(QFocusEvent *e)
{
    if (m_mouseFocusInSelectAll) {
        const Qt::FocusReason focusReason = e->reason();
        if (focusReason==Qt::MouseFocusReason) {
            QTimer::singleShot( 10, this, SLOT(mouseFocusInSelectAll()) );
        }
    }

    QLineEdit::focusInEvent(e);
}

void EbLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return) {
        const QString text = this->text();
        if ( !text.isEmpty() ) {
            emit keyPressEnter( text );
        }
    }
    else if (e->key()==Qt::Key_Escape) {
        emit keyPressEsc();
    }
    else if (e->key()==Qt::Key_Down) {
        emit keyPressDown();
    }
    QLineEdit::keyPressEvent(e);
}
