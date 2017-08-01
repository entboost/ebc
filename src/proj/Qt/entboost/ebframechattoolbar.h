#ifndef EBFRAMECHATTOOLBAR_H
#define EBFRAMECHATTOOLBAR_H

#include "ebclientapp.h"
#include <QFrame>
#include <QPushButton>
#include <QVariant>
#include <vector>

class EbFrameChatToolBar : public QFrame
{
    Q_OBJECT
public:
    enum Type {
        Unknown,
        SelectedText,
        PointImage,
        PointFile,
        PointUserHead
    };
    explicit EbFrameChatToolBar(const EbcCallInfo::pointer &callInfo, QWidget *parent = 0);
    virtual ~EbFrameChatToolBar(void);

    void setCallInfo(const EbcCallInfo::pointer &callInfo);
    void updateMsgType( const QPoint &pos, qint64 msgId, Type type, const QVariant &parameter1=QVariant(), const QVariant &parameter2=QVariant() );
    void updateLocaleInfo(void);
signals:
    void deleteMessage(qint64 msgId);
public slots:
    void onClickedButton0(bool checked);
    void onClickedButton1(bool checked);
    void deleteMessage(void);
    void withdrawMessage(void);
    bool canWithdrawMessage(void) const;
    void collectMessage(void);
    bool canCollectMessage(void) const;
    void onClickedButton2(bool checked);
    void onClickedButton3(bool checked);
    void onClickedButton4(bool checked);
    void onClickedButton5(bool checked);
    void onClickedButton6(bool checked);
protected:
    void moveSize(void);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void timerEvent(QTimerEvent *e);
    void hideAllButton(void);
    void showHideButton(void);
private:
    EbcCallInfo::pointer m_callInfo;
    std::vector<QPushButton*> m_buttonList;
    int m_showButtonCount;
    qint64 m_msgId;
    qint64 m_msgFromUserId;
    int m_msgReadFlag;
    int m_msgType;
    Type m_type;
    QVariant m_parameter1;
    QVariant m_parameter2;
    int m_timerIdCheckMousePos;
};

#endif // EBFRAMECHATTOOLBAR_H
