#ifndef EBTEXTEDIT_H
#define EBTEXTEDIT_H

#include <ebclientapp.h>
#include <QTextEdit>
const QString theMimeDataResourceName("mime-resource:");

class EbTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit EbTextEdit(QWidget *parent = Q_NULLPTR);
    virtual ~EbTextEdit(void);

    bool formatInputMsg(EB_ChatRoomRichMsg *pOutMsgFormat);
signals:
    void dropSendFile(const QString &filePath);
    void requestSendText(void);
protected:
//    virtual void timerEvent(QTimerEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
//    virtual QMimeData *createMimeDataFromSelection(void) const;
    virtual bool canInsertFromMimeData(const QMimeData *source) const;
    virtual void insertFromMimeData(const QMimeData *source);

//    void check(void);
private:
//    int m_timerCheck;
    bool m_bMousePress;
};

#endif // EBTEXTEDIT_H
