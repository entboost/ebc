#ifndef EBWIDGETACCOUNTINFO_H
#define EBWIDGETACCOUNTINFO_H

#include "ebclientapp.h"
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPlainTextEdit>

class EbWidgetAccountInfo : public QWidget
{
    Q_OBJECT
public:
    explicit EbWidgetAccountInfo(QWidget *parent = 0);

    void updateLocaleInfo(void);
    void load(void);
    void save(void);
    void checkData(void);
    void onAreaInfo(const EB_AreaInfo *areaInfo, int parameter);
signals:

public slots:
    void onCurrentIndexChangedArea1(int index);
    void onCurrentIndexChangedArea2(int index);
    void onCurrentIndexChangedArea3(int index);
private:
    QLabel * m_labelAccount;
    QLineEdit * m_lineEditAccount;
    QLabel * m_labelName;
    QLineEdit * m_lineEditName;
    QLabel * m_labelGender;
    QComboBox * m_comboBoxGender;
    QLabel * m_labelBirthday;
    QDateEdit * m_dateEditBirthday;
    QLabel * m_labelPhone;
    QLineEdit * m_lineEditPhone;
    QLabel * m_labelTel;
    QLineEdit * m_lineEditTel;
    QLabel * m_labelEmail;
    QLineEdit * m_lineEditEmail;
    QLabel * m_labelUrl;
    QLineEdit * m_lineEditUrl;
    QLabel * m_labelArea;
    QComboBox * m_comboBoxArea1;
    QComboBox * m_comboBoxArea2;
    QComboBox * m_comboBoxArea3;
    QComboBox * m_comboBoxArea4;
    QLabel * m_labelAddress;
    QLineEdit * m_lineEditAddress;
    QLabel * m_labelZipCode;
    QLineEdit * m_lineEditZipCode;
    QLabel * m_labelDescription;
    QPlainTextEdit * m_plainTextEditDescription;
    EB_AccountInfo m_myAccountInfo;
};

#endif // EBWIDGETACCOUNTINFO_H
