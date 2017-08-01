#include "eblistwidget.h"

EbListWidget::EbListWidget(QWidget *parent)
    : QListWidget(parent)
{

}

void EbListWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return) {
        const QListWidgetItem * item = this->currentItem();
        if ( item!=0 ) {
            emit keyPressEnter( item );
        }
    }
    else if (e->key()==Qt::Key_Up) {
        if (this->currentRow()==0) {
            emit keyPressFirstItemUp();
        }
    }
    else if (e->key()==Qt::Key_Escape) {
        emit keyPressEsc();
    }

    QListWidget::keyPressEvent(e);
}
