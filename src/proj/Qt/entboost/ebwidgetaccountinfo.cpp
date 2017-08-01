#include "ebwidgetaccountinfo.h"
#include "ebmessagebox.h"

EbWidgetAccountInfo::EbWidgetAccountInfo(QWidget *parent) : QWidget(parent)
{
    const int const_y	= 18;
    const int const_x1	= 45-28;	// 28是父窗口左边间隔
    //const int const_x2	= 133-28;
    const int const_combo_width		= 165;
    const int const_static_width	= 80;
    const int const_edit_width		= const_combo_width*2+const_static_width+10;
    const int const_edit_height		= 21;
    const int const_height_interval	= 33;
    /// 帐号
    int nX = const_x1;
    int nY = const_y;
    m_labelAccount = new QLabel(this);
    m_labelAccount->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditAccount = new QLineEdit(this);
    m_lineEditAccount->setReadOnly(true);
    m_lineEditAccount->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 名称
    nX += (const_combo_width+10);
    m_labelName = new QLabel(this);
    m_labelName->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditName = new QLineEdit(this);
    m_lineEditName->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 性别
    nX = const_x1;
    nY += const_height_interval;
    m_labelGender = new QLabel(this);
    m_labelGender->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_comboBoxGender = new QComboBox(this);
    m_comboBoxGender->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 生日
    nX += (const_combo_width+10);
    //nX = const_x1;
    //nY += const_height_interval;
    m_labelBirthday = new QLabel(this);
    m_labelBirthday->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_dateEditBirthday = new QDateEdit(this);
    m_dateEditBirthday->setCalendarPopup(true);
    m_dateEditBirthday->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 手机
    nX = const_x1;
    nY += const_height_interval;
    m_labelPhone = new QLabel(this);
    m_labelPhone->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditPhone = new QLineEdit(this);
    m_lineEditPhone->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 电话
    nX += (const_combo_width+10);
    m_labelTel = new QLabel(this);
    m_labelTel->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditTel = new QLineEdit(this);
    m_lineEditTel->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 邮箱
    nX = const_x1;
    nY += const_height_interval;
    m_labelEmail = new QLabel(this);
    m_labelEmail->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditEmail = new QLineEdit(this);
    m_lineEditEmail->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 主页
    nX += (const_combo_width+10);
    m_labelUrl = new QLabel(this);
    m_labelUrl->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditUrl = new QLineEdit(this);
    m_lineEditUrl->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 地区
    nX = const_x1;
    nY += const_height_interval;
    m_labelArea = new QLabel(this);
    m_labelArea->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_comboBoxArea1 = new QComboBox(this);
    m_comboBoxArea1->setGeometry(nX,nY,82,const_edit_height);	// 83+83-1=165=const_combo_width
    connect( m_comboBoxArea1,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChangedArea1(int)) );
    nX += 82;
    m_comboBoxArea2 = new QComboBox(this);
    m_comboBoxArea2->setGeometry(nX,nY,82,const_edit_height);
    connect( m_comboBoxArea2,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChangedArea2(int)) );
    nX += 82;
    m_comboBoxArea3 = new QComboBox(this);
    m_comboBoxArea3->setGeometry(nX,nY,90,const_edit_height);
    connect( m_comboBoxArea3,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChangedArea3(int)) );
    nX += 90;
    m_comboBoxArea4 = new QComboBox(this);
    m_comboBoxArea4->setGeometry(nX,nY,80,const_edit_height);
    /// 地址
    nX = const_x1;
    nY += const_height_interval;
    m_labelAddress = new QLabel(this);
    m_labelAddress->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditAddress = new QLineEdit(this);
    m_lineEditAddress->setGeometry(nX,nY,const_combo_width,const_edit_height);
    /// 邮编
    nX += (const_combo_width+10);
    m_labelZipCode = new QLabel(this);
    m_labelZipCode->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_lineEditZipCode = new QLineEdit(this);
    m_lineEditZipCode->setGeometry(nX,nY,const_combo_width,const_edit_height);

    /// 备注
    nX = const_x1;
    nY += const_height_interval;
    m_labelDescription = new QLabel(this);
    m_labelDescription->setGeometry(nX,nY,const_static_width,const_edit_height);
    nX += const_static_width;
    m_plainTextEditDescription = new QPlainTextEdit(this);
    m_plainTextEditDescription->setGeometry(nX,nY,const_edit_width,60);

    if (theApp->isDisableAccountEdit() || !theApp->m_ebum.EB_CanEditMyBaseAccountInfo()) {
        //m_comboBirYear.ModifyStyle(CBS_DROPDOWN,0);
        //m_comboBirYear.ModifyStyle(0,CBS_DROPDOWNLIST);
        m_lineEditName->setReadOnly(true);
        m_comboBoxGender->setEnabled(false);
        m_dateEditBirthday->setReadOnly(true);
        m_lineEditPhone->setReadOnly(true);
        m_lineEditTel->setReadOnly(true);
        m_lineEditEmail->setReadOnly(true);
        m_lineEditUrl->setReadOnly(true);
        m_comboBoxArea1->setEnabled(false);
        m_comboBoxArea2->setEnabled(false);
        m_comboBoxArea3->setEnabled(false);
        m_comboBoxArea4->setEnabled(false);
        m_lineEditAddress->setReadOnly(true);
        m_lineEditZipCode->setReadOnly(true);
    }

    updateLocaleInfo();
    load();
    checkData();
}

void EbWidgetAccountInfo::updateLocaleInfo()
{
    m_labelAccount->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-account.text","Account:") );
    m_labelName->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-name.text","Name:") );
    m_labelGender->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-gender.text","Gender:") );
    m_comboBoxGender->clear();
    m_comboBoxGender->addItem("");
    m_comboBoxGender->addItem( theLocales.getLocalText("gender.1.name","Male"),QVariant((int)EB_GENDER_MALE) );
    m_comboBoxGender->addItem( theLocales.getLocalText("gender.2.name","Female"),QVariant((int)EB_GENDER_FEMALE) );
    m_labelBirthday->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-birthday.text","Birthday:") );
    m_dateEditBirthday->setDisplayFormat( theLocales.getLocalText("my-center-dialog.account-info-widget.date-birthday.format","yyyy-MM-dd") );
    m_labelPhone->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-phone.text","Phone:") );
    m_labelTel->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-tel.text","Tel:") );
    m_labelEmail->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-email.text","Email:") );
    m_labelUrl->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-url.text","Url:") );
    m_labelArea->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-area.text","Area:") );
    m_labelAddress->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-address.text","Address:") );
    m_labelZipCode->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-zipcode.text","Zip Code:") );
    m_labelDescription->setText( theLocales.getLocalText("my-center-dialog.account-info-widget.label-description.text","Description:") );

}

void EbWidgetAccountInfo::load()
{
    theApp->m_ebum.EB_GetMyAccountInfo(&m_myAccountInfo);

    m_lineEditAccount->setText( m_myAccountInfo.GetAccount().c_str() );
    m_lineEditName->setText( m_myAccountInfo.GetUserName().c_str() );
    m_comboBoxGender->setCurrentIndex( (int)m_myAccountInfo.GetGender() );

    const int m_nBirthday = m_myAccountInfo.GetBirthday();
    const int year = m_nBirthday/10000;
    const int month = (m_nBirthday%10000)/100;
    const int day = m_nBirthday%100;
    m_dateEditBirthday->setDate( QDate(year,month,day) );
    m_lineEditPhone->setText( m_myAccountInfo.GetMobile().c_str() );
    m_lineEditTel->setText( m_myAccountInfo.GetTel().c_str() );
    m_lineEditEmail->setText( m_myAccountInfo.GetEmail().c_str() );
    if (!m_myAccountInfo.GetArea1String().empty()) {
        int nIndex = m_comboBoxArea1->findText(m_myAccountInfo.GetArea1String().c_str());
        if (nIndex<0) {
            nIndex = m_comboBoxArea1->count();
            m_comboBoxArea1->addItem( m_myAccountInfo.GetArea1String().c_str(),QVariant((int)m_myAccountInfo.GetArea1()) );
        }
        m_comboBoxArea1->setCurrentIndex(nIndex);
        theApp->m_ebum.EB_GetAreaInfo(m_myAccountInfo.GetArea1(),2);
    }
    if (!m_myAccountInfo.GetArea2String().empty()) {
        int nIndex = m_comboBoxArea2->findText(m_myAccountInfo.GetArea2String().c_str());
        if (nIndex<0) {
            nIndex = m_comboBoxArea2->count();
            m_comboBoxArea2->addItem( m_myAccountInfo.GetArea2String().c_str(),QVariant((int)m_myAccountInfo.GetArea2()) );
        }
        m_comboBoxArea2->setCurrentIndex(nIndex);
        theApp->m_ebum.EB_GetAreaInfo(m_myAccountInfo.GetArea2(),3);
    }
    if (!m_myAccountInfo.GetArea3String().empty()) {
        int nIndex = m_comboBoxArea3->findText(m_myAccountInfo.GetArea3String().c_str());
        if (nIndex<0) {
            nIndex = m_comboBoxArea3->count();
            m_comboBoxArea3->addItem( m_myAccountInfo.GetArea3String().c_str(),QVariant((int)m_myAccountInfo.GetArea3()) );
        }
        m_comboBoxArea3->setCurrentIndex(nIndex);
        theApp->m_ebum.EB_GetAreaInfo(m_myAccountInfo.GetArea3(),4);
    }
    if (!m_myAccountInfo.GetArea4String().empty()) {
        int nIndex = m_comboBoxArea4->findText(m_myAccountInfo.GetArea4String().c_str());
        if (nIndex<0) {
            nIndex = m_comboBoxArea4->count();
            m_comboBoxArea4->addItem( m_myAccountInfo.GetArea4String().c_str(),QVariant((int)m_myAccountInfo.GetArea4()) );
        }
        m_comboBoxArea4->setCurrentIndex(nIndex);
    }
    m_lineEditUrl->setText( m_myAccountInfo.GetUrl().c_str() );
    m_lineEditAddress->setText( m_myAccountInfo.GetAddress().c_str() );
    m_lineEditZipCode->setText( m_myAccountInfo.GetZipCode().c_str() );
    m_plainTextEditDescription->setPlainText( m_myAccountInfo.GetDescription().c_str() );
}

void EbWidgetAccountInfo::save()
{
    const QString sUserName = m_lineEditName->text();
    if (sUserName.isEmpty()) {
        /// 个人名称不能为空：<br>请输入！
        m_lineEditName->setFocus();
        EbMessageBox::doShow( NULL, "", QChar::Null,
                              theLocales.getLocalText( "message-show.user-name-empty-error","User Name Empty" ),
                              EbMessageBox::IMAGE_WARNING,default_warning_auto_close );
        return;
    }

    const QString sDescription = m_plainTextEditDescription->toPlainText();
    theApp->m_ebum.EB_GetMyAccountInfo(&m_myAccountInfo);
    if (theApp->isDisableAccountEdit() || !theApp->m_ebum.EB_CanEditMyBaseAccountInfo()) {
        /// 没有普通资料修改权限
    }
    else {
        /// 计算生日整数
        const QDate date = m_dateEditBirthday->date();
        const int m_nBirthday = date.year()*10000+date.month()*100+date.day();
        const QString sMobile = m_lineEditPhone->text();
        const QString sTel = m_lineEditTel->text();
        const QString sEmail = m_lineEditEmail->text();
        const int nArea1 = m_comboBoxArea1->currentIndex()<0?0:m_comboBoxArea1->currentData().toInt();
        const int nArea2 = m_comboBoxArea2->currentIndex()<0?0:m_comboBoxArea2->currentData().toInt();
        const int nArea3 = m_comboBoxArea3->currentIndex()<0?0:m_comboBoxArea3->currentData().toInt();
        const int nArea4 = m_comboBoxArea4->currentIndex()<0?0:m_comboBoxArea4->currentData().toInt();
        const QString sAddress = m_lineEditAddress->text();
        const QString sUrl = m_lineEditUrl->text();
        const QString sZipCode = m_lineEditZipCode->text();
        m_myAccountInfo.SetUserName(sUserName.toStdString());
        m_myAccountInfo.SetGender((EB_GENDER_TYPE)m_comboBoxGender->currentIndex());
        m_myAccountInfo.SetBirthday(m_nBirthday);
        m_myAccountInfo.SetMobile(sMobile.toStdString());
        m_myAccountInfo.SetTel(sTel.toStdString());
        m_myAccountInfo.SetEmail(sEmail.toStdString());
        m_myAccountInfo.SetArea1(nArea1);
        m_myAccountInfo.SetArea2(nArea2);
        m_myAccountInfo.SetArea3(nArea3);
        m_myAccountInfo.SetArea4(nArea4);
        m_myAccountInfo.SetUrl(sUrl.toStdString());
        m_myAccountInfo.SetAddress(sAddress.toStdString());
        m_myAccountInfo.SetZipCode(sZipCode.toStdString());
    }
    m_myAccountInfo.SetDescription(sDescription.toStdString());
    theApp->m_ebum.EB_SetMyAccountInfo(&m_myAccountInfo);
}

void EbWidgetAccountInfo::checkData()
{
    if (m_comboBoxArea1->count()<=1) {
        theApp->m_ebum.EB_GetAreaInfo(0,1);
    }
    /// 更新用户帐号
    const tstring sUserName = theApp->m_ebum.EB_GetUserName();;
    m_myAccountInfo.SetUserName(sUserName);
    m_lineEditName->setText( m_myAccountInfo.GetUserName().c_str() );
}

void EbWidgetAccountInfo::onAreaInfo(const EB_AreaInfo *pAreaInfo, int nParameter)
{
    QComboBox * pComboArea = 0;
    switch(nParameter)
    {
    case 1:
        /// 国家或地区
        pComboArea = m_comboBoxArea1;
        break;
    case 2:
        /// 省或地区
        pComboArea = m_comboBoxArea2;
        break;
    case 3:
        /// 市或地区
        pComboArea = m_comboBoxArea3;
        break;
    case 4:
        /// 地区
        pComboArea = m_comboBoxArea4;
        break;
    default:
        return;
    }
    const int nFind = pComboArea->findText(pAreaInfo->m_sAreaName.c_str());
    if (nFind<0) {
        pComboArea->addItem( pAreaInfo->m_sAreaName.c_str(), QVariant((int)pAreaInfo->m_nAreaCode) );
    }
}

void EbWidgetAccountInfo::onCurrentIndexChangedArea1(int index)
{
    const int nAreaCode = m_comboBoxArea1->itemData(index).toInt();
    m_comboBoxArea2->clear();
    m_comboBoxArea3->clear();
    m_comboBoxArea4->clear();
    theApp->m_ebum.EB_GetAreaInfo(nAreaCode,2);
}

void EbWidgetAccountInfo::onCurrentIndexChangedArea2(int index)
{
    const int nAreaCode = m_comboBoxArea2->itemData(index).toInt();
    m_comboBoxArea3->clear();
    m_comboBoxArea4->clear();
    theApp->m_ebum.EB_GetAreaInfo(nAreaCode,3);
}

void EbWidgetAccountInfo::onCurrentIndexChangedArea3(int index)
{
    const int nAreaCode = m_comboBoxArea3->itemData(index).toInt();
    m_comboBoxArea4->clear();
    theApp->m_ebum.EB_GetAreaInfo(nAreaCode,4);
}
