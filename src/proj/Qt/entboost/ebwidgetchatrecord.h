#ifndef EbWidgetChatRecord_H
#define EbWidgetChatRecord_H

#include <ebclientapp.h>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QDateEdit>
#include <eblineedit.h>
#include <ebtextbrowser.h>
#include <ebwidgetworkview.h>

class EbWidgetChatRecord : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetChatRecord(const EbcCallInfo::pointer &callInfo, QWidget *parent = nullptr);
    virtual ~EbWidgetChatRecord(void);

    void updateLocaleInfo(void);
    EbTextBrowser *textBrowser(void) const {return m_textBrowser;}
signals:
public slots:
    void updateViewButtonCheckState(void);
    void onClickedButtonViewMsg(void);
    void onTextChangeLineEditSearch(const QString &text);
    void onClickedButtonClearAll(void);
    void onClickedCheckBoxOnlineMsg(void);
    void onUserDateChanged(const QDate &date);
    void onClickedButtonMoveFirst(void);
    void onClickedButtonMovePrev(void);
    void onClickedButtonMoveNext(void);
    void onClickedButtonMoveLast(void);

    void onLoadChatRecord(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void timerEvent(QTimerEvent *e);
private:
    EbcCallInfo::pointer m_callInfo;
    QString m_fromName;
    QPushButton *m_buttonViewAll;
    QPushButton *m_buttonViewImage;
    QPushButton *m_buttonViewFile;
    QPushButton *m_buttonViewAudio;
    QPushButton *m_buttonViewText;
    EbLineEdit *m_lineEditSearch;
    QPushButton *m_buttonClearAll;
    EbTextBrowser *m_textBrowser;
    EbWidgetWorkView::pointer m_widgetWorkView;
    QCheckBox *m_checkBoxOnlineMsg;
    QDateEdit *m_dateEdit;
    QPushButton *m_buttonMoveFirst;
    QPushButton *m_buttonMovePrev;
    QPushButton *m_buttonMoveNext;
    QPushButton *m_buttonMoveLast;
    int m_nRecordPages;
    int m_nCurrentPage;
    int m_timerIdLoadChatRecord;
    bool m_bViewSearchResult;
    bool m_bSearchByDate;
};

#endif // EbWidgetChatRecord_H
