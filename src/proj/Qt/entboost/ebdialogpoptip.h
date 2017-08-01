#ifndef EBDIALOGPOPTIP_H
#define EBDIALOGPOPTIP_H

#include <ebclientapp.h>
#include <ebdialogbase.h>
#include <ebwidgetworkview.h>

namespace Ui {
class EbDialogPopTip;
}

class EbDialogPopTip : public EbDialogBase
{
    Q_OBJECT
public:
    enum TipType {
        AuthMessage,  /// 验证消息
        NoticeMessage,  /// 通知消息
        BroadcastMessage  /// 广播消息
    };
    explicit EbDialogPopTip(TipType type, QWidget *parent = 0);
    ~EbDialogPopTip();
    static EbDialogPopTip * create(TipType type, bool deleteOnClose=true, QWidget *parent = 0);

    void setPopTipMessage(const QString &message, eb::bigint msgId=0, const QVariant &variant1=QVariant(), const QVariant &variant2=QVariant());
public slots:
protected:
    virtual void accept(void);
    virtual void resizeEvent(QResizeEvent *e);
private:
    Ui::EbDialogPopTip *ui;
    EbWidgetWorkView::pointer m_workView;
    TipType m_type;
    eb::bigint m_msgId;
    QVariant m_variant1;
    QVariant m_variant2;
};

#endif // EBDIALOGPOPTIP_H
