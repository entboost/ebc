#ifndef EBWIDGETSELECTBASE_H
#define EBWIDGETSELECTBASE_H

#include <QWidget>
#include <QPushButton>
#include <ebcallback.h>
#include <ebdefines.h>

class EbWidgetTreeSelectBase : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetTreeSelectBase(EB_VIEW_MODE viewMode,QWidget *parent = nullptr);

    void setSelectedUserCallback(EbSelectedUserCallback *cb) {m_selectedUserCallback=cb;}
    void updateLocaleInfo(void);
    void timerCheckState(void);
signals:
    void selectedItemInfo(const EbWidgetItemInfo::pointer &itemInfo);
public slots:

protected:
    EB_VIEW_MODE m_viewMode;
    EbSelectedUserCallback * m_selectedUserCallback;
    QTreeWidget * m_treeWidget;
    QPushButton * m_pushButtonSelect;  /// for EB_VIEW_SELECT_USER
    QPushButton * m_pushButtonCall;
    QPushButton * m_pushButtonEdit;
};

#endif // EBWIDGETSELECTBASE_H
