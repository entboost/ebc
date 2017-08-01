#ifndef EBLABEL_H
#define EBLABEL_H

#include <QLabel>
#include <QMouseEvent>

class EbLabel : public QLabel
{
    Q_OBJECT
public:
    explicit EbLabel(QWidget *parent = 0, Qt::WindowFlags f=Qt::WindowFlags());
    explicit EbLabel(const QString &text, QWidget *parent = 0, Qt::WindowFlags f=Qt::WindowFlags());
    virtual ~EbLabel(void);

signals:
    void clicked(void);
    void keyPressEnter(void);
public slots:

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
//    virtual void focusInEvent(QFocusEvent *e);
//    virtual void focusOutEvent(QFocusEvent *e);
};

#endif // EBLABEL_H
