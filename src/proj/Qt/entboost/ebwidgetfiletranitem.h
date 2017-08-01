#ifndef EBWIDGETFILETRANITEM_H
#define EBWIDGETFILETRANITEM_H

#include <ebclientapp.h>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <boost/shared_ptr.hpp>

class EbWidgetFileTranItem : public QWidget
{
    Q_OBJECT
public:
    typedef boost::shared_ptr<EbWidgetFileTranItem> pointer;
    explicit EbWidgetFileTranItem(bool isSending, const CCrFileInfo *fileInfo, QWidget *parent = 0);
    static EbWidgetFileTranItem::pointer create(bool isSending, const CCrFileInfo *fileInfo, QWidget *parent = 0);

    void updateFileInfo(const CCrFileInfo *fileInfo);
    void setFilePercent(const CChatRoomFilePercent *filePercent);
    void updateLocaleInfo(void);
signals:

public slots:
    void onButtonSave2Cloud(void);
    void onButtonSaveas(void);
    void onButtonSave(void);
    void onButtonReject(void);
    void onButtonOffSend(void);
    void onButtonCancel(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
private:
    bool m_isSending;					/// default false;
    CCrFileInfo m_fileInfo;
    QLabel * m_labelFileName;
    QProgressBar * m_progressBar;
    QPushButton * m_buttonSave2Cloud;
    QPushButton * m_buttonSaveas;
    QPushButton * m_buttonSave;
    QPushButton * m_buttonReject;
    QPushButton * m_buttonOffSend;
    QPushButton * m_buttonCancel;
};

#endif // EBWIDGETFILETRANITEM_H
