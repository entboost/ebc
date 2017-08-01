#include "eblabel.h"

EbLabel::EbLabel(QWidget *parent,Qt::WindowFlags f) : QLabel(parent,f)
{

}

EbLabel::EbLabel(const QString &text, QWidget *parent, Qt::WindowFlags f) : QLabel(text,parent,f)
{

}

EbLabel::~EbLabel()
{

}

void EbLabel::mousePressEvent(QMouseEvent *ev)
{
    if ( this->isEnabled() && ev->button()==Qt::LeftButton ) {
        emit clicked();
        return;
    }
    QLabel::mousePressEvent(ev);
}

void EbLabel::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return) {
        emit keyPressEnter();
    }
    QLabel::keyPressEvent(e);
}

