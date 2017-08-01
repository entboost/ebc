#ifndef EBDIALOGCHANGEHEAD_H
#define EBDIALOGCHANGEHEAD_H

#include <ebdialogbase.h>
//#include <QListWidgetItem>

namespace Ui {
class EbDialogChangeHead;
}

class EbDialogChangeHead : public EbDialogBase
{
    Q_OBJECT

public:
    explicit EbDialogChangeHead(QWidget *parent = 0);
    ~EbDialogChangeHead(void);

    void updateLocaleInfo(void);
    void setHeadResorceFile(const QString &headResourceFile);
signals:
    void onSelectedImage(qint64 resourceId,const QString &resourceFile);
public slots:
    void onClickedPushButtonDefault(void);
    void onClickedPushButtonCustom(void);
    void updateClickedPushButton(const QObject *sender);
    void onClickedPushButtonOpenFile(void);
    void onClickedListWidgetDefaultHeads(const QModelIndex &index);
protected:
    virtual void resizeEvent(QResizeEvent *e);
//    virtual void timerEvent(QTimerEvent *e);
    virtual void accept(void);

    void loadDefaultHeadImage(void);
private:
    Ui::EbDialogChangeHead *ui;
    QLabel * m_labelDefaultIcon;
    QLabel * m_labelCustomIcon;
//    int m_timerIdLoadImage;
    QString m_sHeadResourceFile;
    QString m_sHeadResourceMd5;
    QString m_sNewHeadFile;
};

#endif // EBDIALOGCHANGEHEAD_H
