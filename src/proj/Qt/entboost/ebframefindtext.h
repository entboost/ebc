#ifndef EBFRAMEFINDTEXT_H
#define EBFRAMEFINDTEXT_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>

class EbLineEdit;

#define DEFAULT_PANEL_FIND_WIDTH	280
#define DEFAULT_PANEL_FIND_HEIGHT	38
#define DEFAULT_PANEL_FIND_RIGHT	20

class EbFrameFindText : public QFrame
{
    Q_OBJECT
public:
    explicit EbFrameFindText(QWidget* parent=0);

    void showFocusInput(const QString &selectedText);
    void updateLocaleInfo(void);
signals:
    void findText(bool back,const QString &text);
    void exitFindText(void);
public slots:
    void onKeyPressEscFindText(void);
    void onKeyPressEnter(const QString &text);
    void onTextChanged(const QString &text);

    void onClickedButtonFindUp(void);
    void onClickedButtonFindDown(void);
    void onClickedButtonClose(void);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    virtual void timerEvent(QTimerEvent *e);
private:
    EbLineEdit * m_lineEditFindText;
    QLabel * m_labelStatus;
    QPushButton * m_buttonFindUp;
    QPushButton * m_buttonFindDown;
    QPushButton * m_buttonClose;
    int m_timerIdTextChangeFind;
};

#endif // EBFRAMEFINDTEXT_H
