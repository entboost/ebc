#ifndef EBWIDGETMODIFYPASSWORD_H
#define EBWIDGETMODIFYPASSWORD_H

#include "ebclientapp.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>

class EbWidgetModifyPassword : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetModifyPassword(QWidget *parent = 0);

    void updateLocaleInfo(void);
    void setFocus(void);
    void save(void);
signals:

public slots:

private:
    QLabel * m_labelCurrentPassword;
    QLineEdit * m_lineEditCurrentPassword;
    QLabel * m_labelNewPassword;
    QLineEdit * m_lineEditNewPassword;
    QLabel * m_labelConfirmPassword;
    QLineEdit * m_lineEditConfirmPassword;
};

#endif // EBWIDGETMODIFYPASSWORD_H
