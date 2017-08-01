#include "ebwidgetchatrecord.h"
#include <ebiconhelper.h>
#include <ebmessagebox.h>

const int const_one_page_count = 50;	/// 一页显示多少条

EbWidgetChatRecord::EbWidgetChatRecord(const EbcCallInfo::pointer &callInfo,QWidget *parent) : QWidget(parent)
  , m_callInfo(callInfo)
  , m_nRecordPages(0)
  , m_nCurrentPage(-1)
  , m_timerIdLoadChatRecord(0)
  , m_bViewSearchResult(false)
  , m_bSearchByDate(false)
{
    assert ( m_callInfo.get()!=0 );
    m_buttonViewAll = new QPushButton(this);
    m_buttonViewAll->setCheckable(true);
    m_buttonViewAll->setObjectName("MsgCheckButton");
    connect( m_buttonViewAll,SIGNAL(clicked()),this,SLOT(onClickedButtonViewMsg()) );
    m_buttonViewImage = new QPushButton(this);
    m_buttonViewImage->setCheckable(true);
    m_buttonViewImage->setObjectName("MsgCheckButton");
    connect( m_buttonViewImage,SIGNAL(clicked()),this,SLOT(onClickedButtonViewMsg()) );
    m_buttonViewFile = new QPushButton(this);
    m_buttonViewFile->setCheckable(true);
    m_buttonViewFile->setObjectName("MsgCheckButton");
    connect( m_buttonViewFile,SIGNAL(clicked()),this,SLOT(onClickedButtonViewMsg()) );
    m_buttonViewAudio = new QPushButton(this);
    m_buttonViewAudio->setCheckable(true);
    m_buttonViewAudio->setObjectName("MsgCheckButton");
    connect( m_buttonViewAudio,SIGNAL(clicked()),this,SLOT(onClickedButtonViewMsg()) );
    m_buttonViewText = new QPushButton(this);
    m_buttonViewText->setCheckable(true);
    m_buttonViewText->setObjectName("MsgCheckButton");
    connect( m_buttonViewText,SIGNAL(clicked()),this,SLOT(onClickedButtonViewMsg()) );
    m_lineEditSearch = new EbLineEdit(this);
    m_lineEditSearch->setMaxLength(64);
    connect( m_lineEditSearch,SIGNAL(textChanged(QString)),this,SLOT(onTextChangeLineEditSearch(QString)) );
    m_buttonClearAll = new QPushButton(this);
    m_buttonClearAll->setObjectName("BlackMainColorButton");
    connect( m_buttonClearAll,SIGNAL(clicked()),this,SLOT(onClickedButtonClearAll()) );

    m_textBrowser = EbTextBrowser::create(m_callInfo,this);
    m_checkBoxOnlineMsg = new QCheckBox(this);
    connect( m_checkBoxOnlineMsg,SIGNAL(clicked()),this,SLOT(onClickedCheckBoxOnlineMsg()) );
    m_dateEdit = new QDateEdit(this);
    m_dateEdit->setFrame(false);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDate(QDate::currentDate());
    connect( m_dateEdit,SIGNAL(userDateChanged(QDate)),this,SLOT(onUserDateChanged(QDate)) );

    m_buttonMoveFirst = new QPushButton(this);
    connect( m_buttonMoveFirst,SIGNAL(clicked(bool)),this,SLOT(onClickedButtonMoveFirst()) );
    m_buttonMoveFirst->setObjectName("BlackMainColorButton");
    EbIconHelper::Instance()->SetIcon(m_buttonMoveFirst,QChar(0xf100),10);
    m_buttonMovePrev = new QPushButton(this);
    connect( m_buttonMovePrev,SIGNAL(clicked(bool)),this,SLOT(onClickedButtonMovePrev()) );
    m_buttonMovePrev->setObjectName("BlackMainColorButton");
    EbIconHelper::Instance()->SetIcon(m_buttonMovePrev,QChar(0xf104),10);
    m_buttonMoveNext = new QPushButton(this);
    connect( m_buttonMoveNext,SIGNAL(clicked(bool)),this,SLOT(onClickedButtonMoveNext()) );
    m_buttonMoveNext->setObjectName("BlackMainColorButton");
    EbIconHelper::Instance()->SetIcon(m_buttonMoveNext,QChar(0xf105),10);
    m_buttonMoveLast = new QPushButton(this);
    connect( m_buttonMoveLast,SIGNAL(clicked(bool)),this,SLOT(onClickedButtonMoveLast()) );
    m_buttonMoveLast->setObjectName("BlackMainColorButton");
    EbIconHelper::Instance()->SetIcon(m_buttonMoveLast,QChar(0xf101),10);

    updateLocaleInfo();
    onClickedButtonViewMsg();
}

EbWidgetChatRecord::~EbWidgetChatRecord()
{
    m_widgetWorkView.reset();
}

void EbWidgetChatRecord::updateLocaleInfo()
{
    m_buttonViewAll->setText( theLocales.getLocalText("chat-record.button-view-all.text","All") );
    m_buttonViewAll->setToolTip( theLocales.getLocalText("chat-record.button-view-all.tooltip","") );
    m_buttonViewImage->setText( theLocales.getLocalText("chat-record.button-view-image.text","Image") );
    m_buttonViewImage->setToolTip( theLocales.getLocalText("chat-record.button-view-image.tooltip","") );
    m_buttonViewFile->setText( theLocales.getLocalText("chat-record.button-view-file.text","File") );
    m_buttonViewFile->setToolTip( theLocales.getLocalText("chat-record.button-view-file.tooltip","") );
    m_buttonViewAudio->setText( theLocales.getLocalText("chat-record.button-view-audio.text","Audio") );
    m_buttonViewAudio->setToolTip( theLocales.getLocalText("chat-record.button-view-audio.tooltip","") );
    m_buttonViewText->setText( theLocales.getLocalText("chat-record.button-view-text.text","Text") );
    m_buttonViewText->setToolTip( theLocales.getLocalText("chat-record.button-view-text.tooltip","") );
    m_lineEditSearch->setPlaceholderText( theLocales.getLocalText("chat-record.edit-search.bg-text","Search Local Chat Message") );
    m_lineEditSearch->setToolTip( theLocales.getLocalText("chat-record.edit-search.tooltip","") );
    m_buttonClearAll->setText( theLocales.getLocalText("chat-record.button-clear-all.text","Clear All") );
    m_buttonClearAll->setToolTip( theLocales.getLocalText("chat-record.button-clear-all.tooltip","") );
    m_checkBoxOnlineMsg->setText( theLocales.getLocalText("chat-record.checkbox-online-msg.text","View Online Msg") );
    m_checkBoxOnlineMsg->setToolTip( theLocales.getLocalText("chat-record.checkbox-online-msg.tooltip","") );
    m_dateEdit->setDisplayFormat( theLocales.getLocalText("chat-record.date-search.format","") );
    m_buttonMoveFirst->setToolTip( theLocales.getLocalText("chat-record.button-move-first.tooltip","") );
    m_buttonMovePrev->setToolTip( theLocales.getLocalText("chat-record.button-move-prev.tooltip","") );
    m_buttonMoveNext->setToolTip( theLocales.getLocalText("chat-record.button-move-next.tooltip","") );
    m_buttonMoveLast->setToolTip( theLocales.getLocalText("chat-record.button-move-last.tooltip","") );

}

void EbWidgetChatRecord::updateViewButtonCheckState()
{
    const QPushButton * obj = (QPushButton*)sender();
    m_buttonViewAll->setChecked( (obj==0 || m_buttonViewAll==obj)?true:false );
    m_buttonViewImage->setChecked( m_buttonViewImage==obj?true:false );
    m_buttonViewFile->setChecked( m_buttonViewFile==obj?true:false );
    m_buttonViewAudio->setChecked( m_buttonViewAudio==obj?true:false );
    m_buttonViewText->setChecked( m_buttonViewText==obj?true:false );

}

void EbWidgetChatRecord::onClickedButtonViewMsg()
{
    updateViewButtonCheckState();
    if (m_timerIdLoadChatRecord!=0) {
        this->killTimer(m_timerIdLoadChatRecord);
    }
    m_timerIdLoadChatRecord = this->startTimer(100);
}

void EbWidgetChatRecord::onTextChangeLineEditSearch(const QString &text)
{
    static QString theSearchString;
    if (text==theSearchString) {
        return;
    }
    theSearchString = text;
    if (this->isVisible()) {	/// 避免关闭窗口后，重新打开；
        if (m_timerIdLoadChatRecord!=0) {
            this->killTimer(m_timerIdLoadChatRecord);
        }
        m_bViewSearchResult = text.isEmpty()?false:true;
        m_timerIdLoadChatRecord = this->startTimer(100);
    }
}

void EbWidgetChatRecord::onClickedButtonClearAll()
{
    if ( this->m_textBrowser->document()->isEmpty() ) {
        return;
    }
    if (m_fromName.isEmpty()) {
        m_fromName = m_callInfo->fromName().c_str();
    }

    QString title;
    QString text;
    if ( m_callInfo->isGroupCall() ) {
        title = theLocales.getLocalText("message-box.clear-group-chat-record.title","Clear Chat Record");
        text = theLocales.getLocalText("message-box.clear-group-chat-record.text","Confirm Clear:<br>[GROUP_NAME] Chat Record?");
        text.replace("[GROUP_NAME]",m_fromName);
    }
    else {
        title = theLocales.getLocalText("message-box.clear-user-chat-record.title","Clear Chat Record");
        text = theLocales.getLocalText("message-box.clear-user-chat-record.text","Confirm Clear:<br>[USER_NAME](USER_ID) Chat Record?");
        text.replace("[USER_NAME]",m_fromName);
        text.replace("[USER_ID]",QString::number(m_callInfo->fromUserId()));
    }
    if (EbMessageBox::doExec( 0,title, QChar::Null, text, EbMessageBox::IMAGE_QUESTION )==QDialog::Accepted) {
        const bool isAccount = m_callInfo->isGroupCall()?false:true;
        const eb::bigint id = m_callInfo->isGroupCall()?m_callInfo->groupId():m_callInfo->fromUserId();
        theApp->deleteDbRecord( id,isAccount);
        this->m_textBrowser->clear();
    }
}

void EbWidgetChatRecord::onClickedCheckBoxOnlineMsg()
{
    if ( m_checkBoxOnlineMsg->isChecked() ) {
        if (m_widgetWorkView.get()==0) {
            const CEBString funcUrl = theApp->m_ebum.EB_GetConversationsUrl(m_callInfo->fromUserId(),m_callInfo->groupId());
            if ( !funcUrl.empty() ) {
                m_widgetWorkView = EbWidgetWorkView::create( false,QUrl(funcUrl.c_str()),"",this );
            }
            else {
                m_checkBoxOnlineMsg->setEnabled(false);
                m_checkBoxOnlineMsg->setChecked(false);
                return;
            }
        }
        ///
        m_buttonViewAll->setVisible(false);
        m_buttonViewImage->setVisible(false);
        m_buttonViewFile->setVisible(false);
        m_buttonViewAudio->setVisible(false);
        m_buttonViewText->setVisible(false);
        m_lineEditSearch->setVisible(false);
        m_buttonClearAll->setVisible(false);
        m_dateEdit->setVisible(false);
        m_buttonMoveFirst->setVisible(false);
        m_buttonMovePrev->setVisible(false);
        m_buttonMoveNext->setVisible(false);
        m_buttonMoveLast->setVisible(false);

        const int const_interval = 1;
        const bool m_bChildMode = true;
        const int nExpolorerTop = m_bChildMode?0:theDefaultBgColorTitleHeight;
        m_widgetWorkView->setGeometry(const_interval,nExpolorerTop,width()-const_interval*2,height()-nExpolorerTop-28);
        m_widgetWorkView->setVisible(true);
        m_textBrowser->setVisible(false);
    }
    else {
        m_buttonViewAll->setVisible(true);
        m_buttonViewImage->setVisible(true);
        m_buttonViewFile->setVisible(true);
        m_buttonViewAudio->setVisible(true);
        m_buttonViewText->setVisible(true);
        m_lineEditSearch->setVisible(true);
        m_buttonClearAll->setVisible(true);
        m_dateEdit->setVisible(true);
        m_buttonMoveFirst->setVisible(true);
        m_buttonMovePrev->setVisible(true);
        m_buttonMoveNext->setVisible(true);
        m_buttonMoveLast->setVisible(true);
        m_textBrowser->setVisible(true);

        if (m_widgetWorkView.get()) {
            m_widgetWorkView->setVisible(false);
        }
    }

}

void EbWidgetChatRecord::onUserDateChanged(const QDate &/*date*/)
{
    if (m_timerIdLoadChatRecord!=0) {
        this->killTimer(m_timerIdLoadChatRecord);
    }
    m_bSearchByDate = true;
    m_timerIdLoadChatRecord = this->startTimer(100);
}

void EbWidgetChatRecord::onClickedButtonMoveFirst()
{
    if (m_nRecordPages<=1) {
        return;
    }
    if (m_nCurrentPage!=0) {
        m_nCurrentPage = 0;
        if (m_timerIdLoadChatRecord!=0) {
            this->killTimer(m_timerIdLoadChatRecord);
        }
        m_timerIdLoadChatRecord = this->startTimer(100);
    }
}

void EbWidgetChatRecord::onClickedButtonMovePrev()
{
    if (m_nRecordPages<=1) {
        return;
    }
    if (m_nCurrentPage==-1) {
        m_nCurrentPage = m_nRecordPages-2;
    }
    else if (m_nCurrentPage>0) {
        m_nCurrentPage -= 1;
    }
    if (m_timerIdLoadChatRecord!=0) {
        this->killTimer(m_timerIdLoadChatRecord);
    }
    m_timerIdLoadChatRecord = this->startTimer(100);
}

void EbWidgetChatRecord::onClickedButtonMoveNext()
{
    if (m_nRecordPages<=1) {
        return;
    }
    if (m_nCurrentPage==-1) {
        return;
    }
    else if ((m_nCurrentPage+1)<m_nRecordPages) {
        m_nCurrentPage += 1;
        if (m_timerIdLoadChatRecord!=0) {
            this->killTimer(m_timerIdLoadChatRecord);
        }
        m_timerIdLoadChatRecord = this->startTimer(100);
    }
}

void EbWidgetChatRecord::onClickedButtonMoveLast()
{
    if (m_nRecordPages<=1) {
        return;
    }
    if (m_nCurrentPage==-1) {
        return;
    }
    m_nCurrentPage = -1;
    if (m_timerIdLoadChatRecord!=0) {
        this->killTimer(m_timerIdLoadChatRecord);
    }
    m_timerIdLoadChatRecord = this->startTimer(100);
}

void EbWidgetChatRecord::onLoadChatRecord()
{
    char msgTypeSQLString[24];
    memset(msgTypeSQLString,0,sizeof(msgTypeSQLString));
    if ( m_buttonViewImage->isChecked() ) {
        sprintf( msgTypeSQLString, " AND msg_type=%d", (int)MRT_JPG );
    }
    else if (m_buttonViewFile->isChecked()) {
        sprintf( msgTypeSQLString, " AND msg_type=%d", (int)MRT_FILE );
    }
    else if (m_buttonViewAudio->isChecked())  {
        sprintf( msgTypeSQLString, " AND msg_type=%d", (int)MRT_WAV );
    }
    else if (m_buttonViewText->isChecked())  {
        sprintf( msgTypeSQLString, " AND msg_type=%d", (int)MRT_TEXT );
    }

    char searchSQLString[128];
    memset(searchSQLString,0,sizeof(searchSQLString));
    if (m_bViewSearchResult) {
        const QString sSearchString = this->m_lineEditSearch->text();
        const eb::bigint m_nViewMsgId = 0;
        if (m_nViewMsgId==0 && !sSearchString.isEmpty()) {
            tstring sSearchStringTemp(sSearchString.toStdString());
            CSqliteCdbc::escape_string_in(sSearchStringTemp);
            sprintf( searchSQLString, " AND msg_text LIKE '%%%s%%'",sSearchStringTemp.c_str());
        }
    }
    char dateSQLString[64];
    memset(dateSQLString,0,sizeof(dateSQLString));
    if (m_bSearchByDate) {
        const QDate date = m_dateEdit->date();
        sprintf( dateSQLString, " AND date(msg_time)>=date('%04d-%02d-%02d')",date.year(),date.month(),date.day());
    }

    char sql[1024];
    if (m_callInfo->isGroupCall()) {
        sprintf( sql, "select count(msg_id) FROM msg_record_t WHERE dep_code=%lld %s%s",
                 m_callInfo->groupId(),msgTypeSQLString,searchSQLString);
    }
    else if (m_callInfo->fromUserId()>0) {
        sprintf( sql, "select count(msg_id) FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s%s",
                 m_callInfo->fromUserId(),m_callInfo->fromUserId(),msgTypeSQLString,searchSQLString);
    }
    else {
        return;
    }
    int nRecordCount = 0;
    int nCookie = 0;
    theApp->m_sqliteUser->select(sql, nCookie);
    cgcValueInfo::pointer pRecord = theApp->m_sqliteUser->first(nCookie);
    if (pRecord.get()!=0)
    {
        nRecordCount = pRecord->getVector()[0]->getIntValue();
    }
    theApp->m_sqliteUser->reset(nCookie);
    if (nRecordCount<=0) {
        m_textBrowser->clear();
//        m_nLastMsgId = 0;
//        m_nFromUid = 0;
//        m_nToUid = 0;
        m_nRecordPages = 0;
        m_nCurrentPage = -1;
        m_dateEdit->setEnabled(false);
        m_buttonMoveFirst->setEnabled(false);
        m_buttonMovePrev->setEnabled(false);
        m_buttonMoveNext->setEnabled(false);
        m_buttonMoveLast->setEnabled(false);
    }
    else {
        m_nRecordPages = (nRecordCount+const_one_page_count-1)/const_one_page_count;	/// 总共多少页
        if (m_nCurrentPage>m_nRecordPages)
            m_nCurrentPage = m_nRecordPages-1;
        int nOffsetPage = m_nCurrentPage==-1?(m_nRecordPages-1):m_nCurrentPage;
        int nOffset = nOffsetPage*const_one_page_count;
        if (m_bViewSearchResult) {
            m_nRecordPages = 1;
            nOffsetPage = 0;
            nOffset = 0;
        }
        const eb::bigint m_nViewMsgId = 0;
        if (m_nViewMsgId>0 && !m_bViewSearchResult) {
//            if (!m_bFindAllMsgId)
//            {
//                m_bFindAllMsgId = true;
//                if (m_sAccount>0)
//                {
//                    sSql.Format(_T("select msg_id FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) ORDER BY msg_time"),m_sAccount,m_sAccount);
//                }else if (m_sGroupCode>0)
//                {
//                    sSql.Format(_T("select msg_id FROM msg_record_t WHERE dep_code=%lld ORDER BY msg_time"),m_sGroupCode);
//                }else
//                    return;
//                nCookie = 0;
//                theApp.m_pBoUsers->select(sSql, nCookie);
//                pRecord = theApp.m_pBoUsers->first(nCookie);
//                int nIndex = 0;
//                while (pRecord.get()!=NULL)
//                {
//                    const eb::bigint sMsgId = pRecord->getVector()[0]->getBigIntValue();
//                    m_pMsgIdOffsetList.insert(sMsgId,(nIndex++));
//                    pRecord = theApp.m_pBoUsers->next(nCookie);
//                }
//                theApp.m_pBoUsers->reset(nCookie);
//            }
//            int nMsgIdRecordIndex = 0;
//            m_pMsgIdOffsetList.find(m_nViewMsgId,nMsgIdRecordIndex);
//            nOffsetPage = nMsgIdRecordIndex/const_one_page_count;
//            m_nCurrentPage = nOffsetPage;
//            nOffset = m_nCurrentPage*const_one_page_count;
        }
        else if (m_bSearchByDate) {
            if (m_callInfo->isGroupCall()) {
                sprintf( sql, "select count(msg_id) FROM msg_record_t WHERE dep_code=%lld %s%s%s", m_callInfo->groupId(),
                         msgTypeSQLString,dateSQLString,searchSQLString);
            }
            else if (m_callInfo->fromUserId()>0) {
                sprintf( sql, "select count(msg_id) FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s%s%s",
                        m_callInfo->fromUserId(),m_callInfo->fromUserId(),msgTypeSQLString,dateSQLString,searchSQLString);
            }
            else {
                return;
            }
            int nTimeRecordCount = 0;
            nCookie = 0;
            theApp->m_sqliteUser->select(sql, nCookie);
            pRecord = theApp->m_sqliteUser->first(nCookie);
            if (pRecord.get()!=0) {
                nTimeRecordCount = pRecord->getVector()[0]->getIntValue();
            }
            theApp->m_sqliteUser->reset(nCookie);
            nOffset = MAX(0,(nRecordCount-nTimeRecordCount));
            nOffsetPage = nOffset/const_one_page_count;
            m_nCurrentPage = nOffsetPage;
        }

        m_dateEdit->setEnabled(true);
        if (m_nRecordPages==1 || m_bViewSearchResult) {
            m_buttonMoveFirst->setEnabled(false);
            m_buttonMovePrev->setEnabled(false);
            m_buttonMoveNext->setEnabled(false);
            m_buttonMoveLast->setEnabled(false);
        }
        else {
            m_buttonMoveFirst->setEnabled(nOffsetPage>0?true:false);
            /// 上一页[%d/%d]
            m_buttonMovePrev->setToolTip( QString("%1[%2/%3]").arg(theLocales.getLocalText("chat-record.button-move-prev.tooltip","")).arg(nOffsetPage).arg(m_nRecordPages) );
            m_buttonMovePrev->setEnabled(nOffsetPage>0);
            m_buttonMoveNext->setEnabled((nOffsetPage+1)<m_nRecordPages);
            /// 下一页[%d/%d]
            m_buttonMoveNext->setToolTip( QString("%1[%2/%3]").arg(theLocales.getLocalText("chat-record.button-move-next.tooltip","")).arg(MIN(nOffsetPage+2,m_nRecordPages)).arg(m_nRecordPages) );
            m_buttonMoveLast->setEnabled((nOffsetPage+1)<m_nRecordPages);
        }

        if (m_callInfo->isGroupCall()) {
            if (m_bViewSearchResult) {
                sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag "
                         "FROM msg_record_t WHERE dep_code=%lld %s%s ORDER BY msg_time",m_callInfo->groupId(),msgTypeSQLString,searchSQLString);
            }
            else {
                sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag "
                         "FROM msg_record_t WHERE dep_code=%lld %s ORDER BY msg_time LIMIT %d OFFSET %d",m_callInfo->groupId(),
                         msgTypeSQLString,const_one_page_count,nOffset);
            }
            m_textBrowser->loadMsgRecord(sql,false);
        }
        else if (m_callInfo->fromUserId()>0) {
            if (m_bViewSearchResult) {
                sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag "
                         "FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s%s ORDER BY msg_time",
                         m_callInfo->fromUserId(),m_callInfo->fromUserId(),msgTypeSQLString,searchSQLString);
            }
            else {
                sprintf( sql, "select Datetime(msg_time,'localtime'),msg_id,off_time,from_uid,from_name,to_uid,to_name,private,msg_type,msg_name,msg_text,read_flag "
                         "FROM msg_record_t WHERE dep_code=0 AND (from_uid=%lld OR to_uid=%lld) %s ORDER BY msg_time LIMIT %d OFFSET %d",
                         m_callInfo->fromUserId(),m_callInfo->fromUserId(),msgTypeSQLString,const_one_page_count,nOffset);
            }
            m_textBrowser->loadMsgRecord(sql,false);
        }

//        if (m_nViewMsgId>0) {
//            m_pMrFrameInterface->ViewLine(m_nViewMsgId);
//            m_nViewMsgId = 0;
//        }
//        else {
            if (m_bSearchByDate) {
                m_bSearchByDate = false;
                m_textBrowser->moveTextBrowserToStart();
//                if (nSearchByDateMsgId>0)
//                    m_pMrFrameInterface->ViewLine(nSearchByDateMsgId);
//                else
//                    m_pMrFrameInterface->ScrollToPos(-1);
            }
            else if (m_bViewSearchResult) {
                m_textBrowser->moveTextBrowserToStart();
            }
//        }
    }
}

void EbWidgetChatRecord::resizeEvent(QResizeEvent *event)
{
//    const int const_minbtn_width = 24;
//    const int const_minbtn_height = 22;
//    int btnx = 0;
//    btnx = cx-const_minbtn_width-2;
//    if (m_btnClose.GetSafeHwnd()!=NULL)
//        m_btnClose.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
//    btnx -= const_minbtn_width;
//    if (m_btnMax.GetSafeHwnd()!=NULL)
//        m_btnMax.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);
//    btnx -= const_minbtn_width;
//    if (m_btnMin.GetSafeHwnd()!=NULL)
//        m_btnMin.MoveWindow(btnx, 1,const_minbtn_width,const_minbtn_height);

    const bool m_bChildMode = true;
    int y = m_bChildMode?10:52;
    int x = 10;
    const int const_viewbtn_width = 35;
    const int const_viewbtn_height = 20;
    m_buttonViewAll->setGeometry(x, y+1,const_viewbtn_width,const_viewbtn_height);
    x += (const_viewbtn_width+2);
    m_buttonViewImage->setGeometry(x, y+1,const_viewbtn_width,const_viewbtn_height);
    x += (const_viewbtn_width+2);
    m_buttonViewFile->setGeometry(x, y+1,const_viewbtn_width,const_viewbtn_height);
    x += (const_viewbtn_width+2);
    m_buttonViewAudio->setGeometry(x, y+1,const_viewbtn_width,const_viewbtn_height);
    x += (const_viewbtn_width+2);
    m_buttonViewText->setGeometry(x, y+1,const_viewbtn_width,const_viewbtn_height);
    x += (const_viewbtn_width+5);
    const int const_btn_width = 60;
    m_lineEditSearch->setGeometry(x,y,MIN(120,width()-m_buttonViewText->geometry().right()-const_btn_width-10-5),22);	/// 196
    m_buttonClearAll->setGeometry(width()-const_btn_width-10,y+1,const_btn_width,const_viewbtn_height);

    const int const_interval = 1;
    if ( m_checkBoxOnlineMsg->isChecked() && m_widgetWorkView.get()!=0 ) {
        const int nExpolorerTop = m_bChildMode?0:theDefaultBgColorTitleHeight;
        m_widgetWorkView->setGeometry(const_interval,nExpolorerTop,width()-const_interval*2,height()-nExpolorerTop-28);
    }
    y = m_bChildMode?42:84;
    m_textBrowser->setGeometry(const_interval,y,width()-const_interval*2,height()-y-28);
    m_textBrowser->moveTextBrowserToEnd();
    m_checkBoxOnlineMsg->setGeometry(5,height()-22,110,14);

    y = height()-24;
    const int const_arraw_btn_size = 16;
    x = width()-const_arraw_btn_size-4;
    m_buttonMoveLast->setGeometry(x,y,const_arraw_btn_size,const_arraw_btn_size);
    x -= const_arraw_btn_size;
    m_buttonMoveNext->setGeometry(x,y,const_arraw_btn_size,const_arraw_btn_size);
    x -= (const_arraw_btn_size+2);	/// ** 10
    m_buttonMovePrev->setGeometry(x,y,const_arraw_btn_size,const_arraw_btn_size);
    x -= const_arraw_btn_size;
    m_buttonMoveFirst->setGeometry(x,y,const_arraw_btn_size,const_arraw_btn_size);
    x -= 117;
    m_dateEdit->setGeometry(x,y-2,115,22);

//    MovePanelFind(cx);
//    if (m_pPanelStatus!=NULL && m_pPanelStatus->IsWindowVisible())
//    {
//        SetTimer(TIMER_SHOW_STATUS_MESSAGE,100,NULL);
//    }

    QWidget::resizeEvent(event);
}

void EbWidgetChatRecord::timerEvent(QTimerEvent *e)
{
    if (m_timerIdLoadChatRecord!=0 && m_timerIdLoadChatRecord==e->timerId()) {
        this->killTimer(m_timerIdLoadChatRecord);
        m_timerIdLoadChatRecord = 0;
        onLoadChatRecord();
    }
    QWidget::timerEvent(e);
}
