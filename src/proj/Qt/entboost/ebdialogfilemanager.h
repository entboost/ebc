#ifndef EBDIALOGFILEMANAGER_H
#define EBDIALOGFILEMANAGER_H

#include <ebclientapp.h>
#include <ebdialogbase.h>
#include <ebwidgetfiletranlist.h>
#include <ebwidgetiteminfo.h>

namespace Ui {
class EbDialogFileManager;
}

class EbDialogFileManager : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogFileManager(QWidget *parent = 0);
    ~EbDialogFileManager(void);

    void updateLocaleInfo(void);
    void timerCheckState(void);
    void onSendingFile(const CCrFileInfo *fileInfo);
    void onReceivingFile(const CCrFileInfo *fileInfo);
    void onReceivedFile(const CCrFileInfo *fileInfo);
    void onFilePercent(const CChatRoomFilePercent *filePercent);
    void deleteTranFile(eb::bigint msgId);
    bool isEmpty(void) const;
public slots:
    void updateWidgetInfo(bool traningFile);
    void onClickedButtonTraningFile(void);
    void onClickedButtonTranedFile(void);
    void onClickedButtonRefresh(void);
    void onRefreshTranedFile(void);
    void onTextChangedSearch(const QString &text);
    void onItemDoubleClicked(QListWidgetItem *item);
    void onItemEntered(QListWidgetItem *item);
    void onClickedButtonOpenFile(void);
    void onClickedButtonOpenDir(void);
    void onClickedButtonDeleteFile(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
private:
    Ui::EbDialogFileManager *ui;
    EbWidgetFileTranList * m_tranFiles;
    CLockMap<eb::bigint,EbWidgetItemInfo::pointer> m_pItemItemInfo;	/// msgid->
};

#endif // EBDIALOGFILEMANAGER_H
