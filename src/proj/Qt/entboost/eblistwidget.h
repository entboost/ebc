#ifndef EBLISTWIDGET_H
#define EBLISTWIDGET_H

#include <QListWidget>
#include <QKeyEvent>

class EbListWidget : public QListWidget
{
    Q_OBJECT
public:
    EbListWidget(QWidget *parent=0);

signals:
    void keyPressFirstItemUp(void);
    void keyPressEsc(void);
    void keyPressEnter(const QListWidgetItem *item);
protected:
    virtual void keyPressEvent(QKeyEvent *e);
};

#endif // EBLISTWIDGET_H
