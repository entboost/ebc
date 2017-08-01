#ifndef EBDIALOGEMOTIONSELECT_H
#define EBDIALOGEMOTIONSELECT_H

#include "ebclientapp.h"
#include <ebdialogbase.h>
#include <QListWidgetItem>

namespace Ui {
class EbDialogEmotionSelect;
}

class EbDialogEmotionSelect : public EbDialogBase
{
    Q_OBJECT
public:
    explicit EbDialogEmotionSelect(QWidget *parent = 0);
    ~EbDialogEmotionSelect(void);

    void updateLocaleInfo(void);
signals:
    void selectedResource(const QIcon &icon,mycp::bigint resourceId, const QString &resourceCmInfo,const QString &resourceFilePath);
public slots:
    void onItemClicked(QListWidgetItem *item);
    void onItemEntered(QListWidgetItem *item);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual bool eventFilter(QObject *obj, QEvent *e);
    virtual void timerEvent(QTimerEvent *e);
    void checkFocusOut(void);
    void loadEmotion(void);
private:
    Ui::EbDialogEmotionSelect *ui;
    int m_timerIdLoadEmotion;
    bool m_hasEmptyFile;
    CLockMap<mycp::bigint,bool> m_loadedResourceIdList;
};

#endif // EBDIALOGEMOTIONSELECT_H
