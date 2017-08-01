#include "ebtextblockuserdata.h"

EbTextBlockUserData::EbTextBlockUserData(qint64 msgId)
    : m_msgId(msgId)
    , m_flags(0)
    , m_userId(0)
//    , m_widget(0)
{

}

EbTextBlockUserData::~EbTextBlockUserData(void)
{

}

//void EbTextBlockUserData::createWidget(QWidget *parent)
//{
//    if (m_widget==0) {
//        m_widget = new QLabel(parent);
//        if (isReceiveMessage()) {
//            m_widget->setStyleSheet("QLabel{background-color: rgb(255,0,0);border:1px solid rgb(224,224,224);border-radius: 2px;}");
////            m_widget->setObjectName("ReceiveMessage");
//        }
//        else {
//            m_widget->setStyleSheet("QLabel{background-color: rgb(255,255,0);border:1px solid rgb(224,224,224);border-radius: 2px;}");
////            m_widget->setObjectName("SendMessage");
//        }
//        m_widget->setText("asdfwqelirqiewjrqewra;sdfja;sdjf");
//    }
//    m_widget->raise();
//}

//void EbTextBlockUserData::setGeometry(const QRect &rect)
//{
//    if (m_widget!=0) {
//        m_widget->setGeometry(rect);
//    }
//}
