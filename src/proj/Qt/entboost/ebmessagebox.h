#ifndef EBMESSAGEBOX_H
#define EBMESSAGEBOX_H

#include <QDialog>
#include <QMessageBox>
#include <ebdialogbase.h>

namespace Ui {
class EbMessageBox;
}

const QSize const_image_size(42,41);

class EbMessageBox : public EbDialogBase
{
    Q_OBJECT
public:
    enum IMAGE_TYPE {
        IMAGE_NULL
        , IMAGE_INFORMATION
        , IMAGE_QUESTION
        , IMAGE_WARNING
        , IMAGE_ERROR
    };
    explicit EbMessageBox(QWidget *parent = 0);
    ~EbMessageBox(void);

    /// icon mayby QChar::Null
    static int doExec(QWidget *parent, const QString &title, QChar icon, const QString &text, IMAGE_TYPE imageType, int autoClose=0, int nButtonType=QMessageBox::Ok|QMessageBox::Cancel);
    static void doShow(QWidget *parent, const QString &title, QChar icon, const QString &text, IMAGE_TYPE imageType, int autoClose=0, int nButtonType=QMessageBox::NoButton);

    QString m_title;
    QChar m_icon;
    QString m_text;
    IMAGE_TYPE m_imageType;
    int m_buttonType;
    int m_autoClose;

    void init(void);    /// *** for ->show()
protected:
    virtual int exec(void);
//    void showEvent(QShowEvent *e);
    virtual void timerEvent(QTimerEvent *e);
private:
    Ui::EbMessageBox *ui;
//    QLabel* labelImage;
//    QLabel* labelText;
    int m_closeTimerId;
    int m_closeIndex;
};

#endif // EBMESSAGEBOX_H
