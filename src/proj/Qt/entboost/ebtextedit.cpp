#include "ebtextedit.h"
#include <QTextBlock>
#include <QMimeData>

EbTextEdit::EbTextEdit(QWidget *parent)
    : QTextEdit(parent)
//    , m_timerCheck(0)
    , m_bMousePress(false)
{
    this->setMouseTracking(true);

    /// 移到指定行号
//    QTextEdit* p = new QTextEdit;
//    QTextBlock block = p->document()->findBlockByNumber(nLineNum);
//    p->setTextCursor(QTextCursor(block));

//    m_timerCheck = this->startTimer(500);
}

EbTextEdit::~EbTextEdit()
{
//    if (m_timerCheck!=0) {
//        this->killTimer(m_timerCheck);
//        m_timerCheck = 0;
//    }

}

inline int getImageQuality(int size) {
    if (size>=(1280*720))
        return 75;
    else if (size>=(1024*720))
        return 80;
    else if (size>=(800*600))
        return 90;
    else
        return -1;
}

bool EbTextEdit::formatInputMsg(EB_ChatRoomRichMsg *pOutMsgFormat)
{
    QTextDocument *document = this->document();
    if (document->isEmpty()) {
        this->setFocus();
        return false;
    }

    QTextBlock currentBlock = document->begin();
    QTextBlock::iterator it;
//    QTextCursor cursor = ui.textEditInput->textCursor();

    std::string sFindText;
    while( true) {
        /// 在修改chatformat时会改变当前Block的fragment
        for (it = currentBlock.begin(); !(it.atEnd()); ) {
            QTextFragment currentFragment = it.fragment();
            QTextImageFormat newImageFormat = currentFragment.charFormat().toImageFormat();

            if (newImageFormat.isValid()) {
                /// 判断出这个fragment为image
                if ( !sFindText.empty() ) {
                    pOutMsgFormat->AddText( sFindText.c_str() );
                    sFindText.clear();
                }
                const QString resourceName = newImageFormat.name();
                QFile fileImage(resourceName);
                if (fileImage.open(QFile::ReadOnly)) {
                    /// 是本地图片文件
                    QByteArray data = fileImage.readAll();
                    fileImage.close();
                    const int dataSize = data.size();
                    if (dataSize>0) {
                        char * imageData = new char[dataSize];
                        memcpy(imageData, data.constData(), dataSize);
                        EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)pOutMsgFormat->AddObject( imageData,(unsigned long)dataSize );
                        /// 用于本端显示
                        pMsgItem->SetText(resourceName.toStdString().c_str());
                    }
                }
                else if (resourceName.indexOf(theMimeDataResourceName)==0) {
                    /// 截图
                    const QVariant imageResource = document->resource( QTextDocument::ImageResource, QUrl(resourceName) );
                    if ( imageResource.isValid() ) {

                        char lpszBuffer[260];
                        sprintf(lpszBuffer,"%s/%08d%06d.jpg",theApp->userImagePath().toStdString().c_str(),
                                (int)time(0)%100000000,rand()%1000000);
                        const QString jpgPhotoFile(lpszBuffer);

                        const QVariant::Type imageResourceType = imageResource.type();
                        switch (imageResourceType) {
                        case QVariant::Pixmap: {
                            const QPixmap pixmap = imageResource.value<QPixmap>();
                            const int size = pixmap.width()*pixmap.height();
                            pixmap.save(jpgPhotoFile,0,getImageQuality(size));
                            break;
                        }
                        case QVariant::Image: {
                            const QImage image = imageResource.value<QImage>();
                            const int size = image.width()*image.height();
                            image.save(jpgPhotoFile,0,getImageQuality(size));
                            break;
                        }
//                        case QVariant::Icon: {
//                            const QIcon image = imageResource.value<QIcon>();
//                            rect.setWidth( image.width() );
//                            rect.setHeight( image.height() );
//                            break;
//                        }
                        case QVariant::Bitmap: {
                            const QBitmap bitmap = imageResource.value<QBitmap>();
                            const int size = bitmap.width()*bitmap.height();
                            bitmap.save(jpgPhotoFile,0,getImageQuality(size));
                            break;
                        }
                        default:
                            break;
                        }

                        QFile fileJpg(jpgPhotoFile);
                        if (fileJpg.open(QFile::ReadOnly)) {
                            QByteArray data = fileJpg.readAll();
                            fileJpg.close();
                            const int dataSize = data.size();
                            if (dataSize>0) {
                                char * imageData = new char[dataSize];
                                memcpy(imageData, data.constData(), dataSize);
                                EB_ChatRoomMsgItem* pMsgItem = (EB_ChatRoomMsgItem*)pOutMsgFormat->AddObject( imageData,(unsigned long)dataSize );
                                /// 用于本端显示
                                pMsgItem->SetText(jpgPhotoFile.toStdString().c_str());
                            }

                        }
                    }
                }
                else {
                    pOutMsgFormat->AddResource( resourceName.toStdString().c_str());
                }
                ++it;
                continue;
            }

            if (currentFragment.isValid()) {
                const QString strText = currentFragment.text();
                if ( !sFindText.empty() ) {
                    sFindText.append("\n");
                }
                sFindText.append( strText.toStdString() );
                ++it;
            }
            else {
                // ???
                break;
            }
        }

        currentBlock = currentBlock.next();
        if(!currentBlock.isValid())
            break;
    }
    if ( !sFindText.empty() ) {
        pOutMsgFormat->AddText( sFindText.c_str() );
    }
    this->clear();
    this->setFocus();
    return true;
}

void EbTextEdit::keyPressEvent(QKeyEvent *e)
{
    if ( !this->document()->isEmpty() && (e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return) ) {
        if ( theApp->sendKeyType()==EB_SEND_KEY_ENTER ) {
            /// Enter 回车发送消息
            if ( (e->modifiers()&Qt::ControlModifier)!=0 ) {
                /// 需要回车
                this->insertPlainText("\n");
                return;
            }
            else {
                emit requestSendText();
                return;
            }
        }
        else {
            /// Ctrl+Enter 发送消息
            if ( (e->modifiers()&Qt::ControlModifier)!=0 ) {
                emit requestSendText();
                return;
            }
            else {
                /// 增加换行默认跑后面即可
            }
        }
    }

    QTextEdit::keyPressEvent(e);
}

//void EbTextEdit::timerEvent(QTimerEvent *event)
//{
//    if ( m_timerCheck==event->timerId() ) {
//        check();
//    }

//    QTextEdit::timerEvent(event);
//}


void EbTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;
    }
    QTextEdit::mousePressEvent(event);
}

void EbTextEdit::mouseReleaseEvent(QMouseEvent * e)
{
    m_bMousePress = false;
    QTextEdit::mousePressEvent(e);
}
void EbTextEdit::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint pos1 = event->pos();
    if (!m_bMousePress && false) {
//        this->h
//        this->hasFocus()

        QTextCursor cursor = this->textCursor();
        if (cursor.hasSelection()) {
            const int nSelectionStart = cursor.selectionStart();
            const int nSelectionEnd = cursor.selectionEnd();
            const QString text = cursor.selectedText();

            QTextCursor cur = this->textCursor();//返回当前光标对象
            cur.setPosition(nSelectionStart, QTextCursor::KeepAnchor);//移到起始位置
            const QPoint topleft = this->cursorRect(cur).topLeft();
            cur.setPosition(nSelectionEnd, QTextCursor::KeepAnchor);//移到起始位置
//            cur.movePosition(QTextCursor::NoMove, QTextCursor::KeepAnchor, nSelectionEnd-nSelectionStart+2);
//            cur.select(QTextCursor::WordUnderCursor);   // 选择
//            this->setTextCursor(cur);
            const QPoint bottomright = this->cursorRect(cur).bottomRight();
            const QRect rectSelection(topleft,bottomright);
            if (rectSelection.contains(pos1)) {
                // 鼠标移到选择单行内容
                QString test2(text);
            }
        }
    }
//    QPoint pos2 = event->globalPos();

//    QTextCursor cursor = cursorForPosition(pos1);
//    const bool isNull = cursor.isNull();
//    const int position = cursor.position();
//    const bool hasSelection = cursor.hasSelection();
//    const QString selectionText = cursor.selectedText();
//    if (!selectionText.isEmpty()) {
//        int i=0;
//    }
//    QTextBlock textBlock = cursor.block();
//    const bool isValid = textBlock.isValid();
//    const QString text = textBlock.text();
//    if (!text.isEmpty()) {
//        int i=0;
//    }

    QTextEdit::mouseMoveEvent(event);
}

//QMimeData *EbTextEdit::createMimeDataFromSelection() const
//{
//    return QTextEdit::createMimeDataFromSelection();
//}

bool EbTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void EbTextEdit::insertFromMimeData(const QMimeData *source)
{
//    const QString text = source->text();
//    const QString html = source->html();

    if ( source!=0 && source->hasUrls() ) {
        /// 发送文件
        const QList<QUrl> urls = source->urls();
        for ( int i=0; i<urls.size(); i++ ) {
            const QUrl & url = urls.at(i);
            const QString fileName = url.toLocalFile();
            emit dropSendFile(fileName);
        }
        return;
    }
    else if ( source!=0 && source->hasImage() ) {
        const QVariant imageData = source->imageData();
        static int theIndex = 0;
        const QString resourceInfo = QString("%1-%2").arg(theMimeDataResourceName).arg( theIndex++ );
        this->document()->addResource(QTextDocument::ImageResource, QUrl(resourceInfo), imageData);
        this->textCursor().insertImage(resourceInfo);
    }
    QTextEdit::insertFromMimeData(source);
}

//void EbTextEdit::check()
//{
////    QTextCursor cursor2;

////    const QPoint pos1(3,3);  // 这个可以取到内容的
//    const QPoint cursorPoint = this->cursor().pos();
//    const QPoint posthis2 = this->pos();
//    const QPoint posThis = this->mapToGlobal(posthis2);  // 223,587
////    const QPoint posThis = this->mapToGlobal(QPoint(0,0));  // 223,587
//    const QPoint posThis1 = this->mapFromGlobal(posThis);
//    int x = posThis1.x();
//    const QPoint posThis2 = this->mapFromGlobal(cursorPoint);
//    x = posThis2.x();
////    const QPoint posThis = ((QWidget*)this->parent())->mapToParent(posThis2); // 这个取出来是 0，5xx
////    const QPoint posThis = this->mapToGlobal(QPoint(0,0));
//    const QPoint pos1(cursorPoint.x()-posThis.x(),cursorPoint.y()-posThis.y());
////    const QPoint pos1 = this->mapToParent(cursorPoint); //
////    const QPoint pos2 = this->mapTo(this, cursorPoint);
////    const QPoint pos1 = ((QWidget*)this->parent())->mapFromGlobal(cursorPoint);
////    const QPoint pos1 = this->mapFromGlobal(cursorPoint);
//    int x2 = pos1.x();
//    int y2 = pos1.y();
//    QTextCursor cursor = cursorForPosition(pos1);
//    const bool isNull = cursor.isNull();
//    const int position = cursor.position();
//    const bool hasSelection = cursor.hasSelection();
//    const QString selectionText = cursor.selectedText();
//    QTextBlock textBlock = cursor.block();
//    const bool isValid = textBlock.isValid();
//    const QString text = textBlock.text();
//    int i=0;

//}
