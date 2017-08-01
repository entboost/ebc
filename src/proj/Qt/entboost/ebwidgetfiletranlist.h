#ifndef EBWIDGETFILETRANLIST_H
#define EBWIDGETFILETRANLIST_H

#include <QWidget>
#include <ebwidgetfiletranitem.h>

class EbWidgetFileTranList : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetFileTranList(QWidget *parent = nullptr);

    void onExitChat(bool bHangup);
    void onSendingFile(const CCrFileInfo * fileInfo);
    void onReceivingFile(const CCrFileInfo * fileInfo);
    void onFilePercent(const CChatRoomFilePercent * filePercent);
    void deleteTranFile(eb::bigint msgId);
    bool isEmpty(void) const;
signals:

public slots:
    void onAddFileTranItem(void);
protected:
    void moveSize(void);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void closeEvent(QCloseEvent *e);
private:
//    EbcCallInfo::pointer m_callInfo;
    CLockList<CCrFileInfo> m_pAddTranFile;
//    CLockList<EbWidgetFileTranItem::pointer> m_pDelTranFile;
    CLockMap<eb::bigint, EbWidgetFileTranItem::pointer> m_tranFileList;	/// msgid->
};

#endif // EBWIDGETFILETRANLIST_H
