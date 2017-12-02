#include "CodeCompletion.h"

#include <ApplicationServices/ApplicationServices.h>

#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QTextDocument>

#include <iostream>


#include <unistd.h>
#include <cstdlib>
#include <cmath>

#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStateDLL.h"

#include <vector>

#define CodeCompletion_DUMMY_FILENAME "dummyfile.cpp"

GyroDATA EEGcontrol;
bool unlockkey = false;

QString input[MAXN];
int inputtot;

char wordlist[MAXL][MAXL];
int wordlisttot;
int wordlistlength[MAXL];

void getWordList()
{
    if(char *env = getenv("WORDLIST"))
    {
        for(int i = 0;i < sizeof(env); i ++)
        {
            if(env[i] == ',') wordlisttot ++;
            else wordlist[wordlisttot][wordlistlength[wordlisttot] ++] = env[i];
        }
    }
    else return;
}





void keyboardSimulate(int keyexample)
{
    CGEventRef mkey = CGEventCreateKeyboardEvent(NULL, keyexample, true);
    CGEventPost(kCGAnnotatedSessionEventTap, mkey);
    CFRelease(mkey);
}
CThread::CThread()
{

}
const IEE_MotionDataChannel_t targetChannelList[] = {
    IMD_COUNTER,
    IMD_GYROX,
    IMD_GYROY,
    IMD_GYROZ,
    IMD_ACCX,
    IMD_ACCY,
    IMD_ACCZ,
    IMD_MAGX,
    IMD_MAGY,
    IMD_MAGZ,
    IMD_TIMESTAMP
};
std::unique_ptr<void, void(*)(DataHandle)> hMotionData(IEE_MotionDataCreate(), &IEE_MotionDataFree);

bool  outOfBound = false;
float currX      = 0,
      currY      = 0;
float xmax       = 0,
      ymax       = 0,
      x          = 0;
float preX       = 0,
      preY       = 0;
int   incOrDec   = 10;
int   count      = 0;
float oldXVal    = 0,
      oldYVal    = 0;
double maxRadius = 10000;

void showGyro(bool consoler)
{
    IEE_MotionDataUpdateHandle(0, hMotionData.get());
    unsigned int nSamplesTaken = 0;
    IEE_MotionDataGetNumberOfSample(hMotionData.get(), &nSamplesTaken);

    if (nSamplesTaken != 0) {
        std::vector<double> data(nSamplesTaken);
        for (int sampleIdx = 0; sampleIdx<(int)nSamplesTaken; ++sampleIdx) {
            for (int i = 0;
                i<sizeof(targetChannelList) / sizeof(IEE_MotionDataChannel_t);
                i++) {

                IEE_MotionDataGet(hMotionData.get(), targetChannelList[i],
                    data.data(), data.size());
                if (i == 1 && consoler)
                    std::cout << "GyroX =" << data[sampleIdx];
                if (i == 2 && consoler)
                {
                    std::cout << "GyroY =" << data[sampleIdx];
                    std::cout << std::endl;
                }
            }
        }
    }
}
void changeXY(int x) // x = 0 : idle
{
    if( currX >0 )
    {
        float temp = currY/currX;
        currX -= incOrDec;
        currY = temp*currX;
    }
    else if( currX < 0)
    {
        float temp = currY/currX;
        currX += incOrDec;
        currY = temp*currX;
    }
    else
    {
        if( currY > 0 ) currY -= incOrDec;
        else if( currY <0 ) currY += incOrDec;
    }
    if( x == 0)
    {
        if( (std::abs(currX) <= incOrDec) && (std::abs(currY) <= incOrDec))
        {
            xmax = 0;
            ymax = 0;
        }
        else
        {
            xmax = currX;
            ymax = currY;
        }
    }
    else
    {
        if( (std::abs(currX) <= incOrDec) && (std::abs(currY) <= incOrDec))
        {
            xmax = 0;
            ymax = 0;
        }
    }
}


void updateDisplay(void)
{
    showGyro(false);

   int gyroX = 0, gyroY = 0;
   IEE_HeadsetGetGyroDelta(0, &gyroX, &gyroY);
   xmax += gyroX;
   ymax += gyroY;

   if( outOfBound )
   {
       if( preX != gyroX && preY != gyroY )
       {
           xmax = currX;
           ymax = currY;
       }
   }

   double val = sqrt((float)(xmax*xmax + ymax*ymax));

   std::cout <<"xmax : " << xmax <<" ; ymax : " << ymax << std::endl;
    EEGcontrol.x = xmax;
    EEGcontrol.y = ymax;

   if( val >= maxRadius )
   {
       changeXY(1);
       outOfBound = true;
       preX = gyroX;
       preY = gyroY;
   }
   else
   {
       outOfBound = false;
        if(oldXVal == gyroX && oldYVal == gyroY)
        {
            ++count;
            if( count > 10 )
            {
                changeXY(0);
            }
        }
        else
        {
            count = 0;
            currX = xmax;
            currY = ymax;
            oldXVal = gyroX;
            oldYVal = gyroY;
        }
   }

      usleep(10000);
}

void CThread::run()
{
    EmoEngineEventHandle hEvent = IEE_EmoEngineEventCreate();
    EmoStateHandle eState = IEE_EmoStateCreate();

    unsigned int userID = -1;
    int state           = 0;
    bool ready          = false;

    if (IEE_EngineConnect() != EDK_OK) {
        std::cout << "Emotiv Driver start up failed.";
        return;
    }

    std::cout << "Please make sure your headset is on and don't move your head.";
    std::cout << std::endl;

    IEE_MotionDataSetBufferSizeInSec(1);

    while(true)
    {

        state = IEE_EngineGetNextEvent(hEvent);

        if (state == EDK_OK)
        {
            IEE_Event_t eventType = IEE_EmoEngineEventGetType(hEvent);
            IEE_EmoEngineEventGetUserId(hEvent, &userID);

            if (eventType == IEE_UserAdded) {
                 std::cout << "User added" << std::endl << std::endl;
                 userID = 0;
                 ready = true;
            }
        }

        if(!ready) continue;

        int gyroX = 0, gyroY = 0;
        int err = IEE_HeadsetGetGyroDelta(userID, &gyroX, &gyroY);

        if (err == EDK_OK){
            std::cout << std::endl;
            std::cout << "You can move your head now." << std::endl;
            usleep(10000);
            break;
        }else if (err == EDK_GYRO_NOT_CALIBRATED){
            std::cout << "Gyro is not calibrated. Please stay still." << std::endl;
            showGyro(true);
        }else if (err == EDK_CANNOT_ACQUIRE_DATA){
            std::cout << "Cannot acquire data" << std::endl;
            showGyro(true);
        }else{
            std::cout << "No headset is connected" << std::endl;
        }
        usleep(10000);
    }

    //
    int currenty = 0;
    int currentx = 0;
    while(1){
        updateDisplay();

        if(unlockkey)
        {
            if(EEGcontrol.y - currenty > 800)
            {
                keyboardSimulate(0x7E);
                currenty = EEGcontrol.y - 100;
            }
            if(EEGcontrol.y - currenty < -800)
            {
                keyboardSimulate(0x7D);
                currenty = EEGcontrol.y + 100;
            }
            if(EEGcontrol.x - currentx > 1000)
            {
                keyboardSimulate(0x30);
                keyboardSimulate(0x31);
                currentx = EEGcontrol.x - 100;
            }
            if(EEGcontrol.x< -2000)
            {
                keyboardSimulate(0x7B);
            }
        }
        usleep(10000);
    }

    //
}

CodeCompletion::CodeCompletion(QWidget *parent)
    : QTextEdit(parent)
    , m_CompleterPopup(new QCompleter(this))
{
    m_CompleterPopup->setCaseSensitivity(Qt::CaseSensitive);
    m_CompleterPopup->setWidget(this);
    m_CompleterPopup->setCompletionMode(QCompleter::PopupCompletion);
    connect(m_CompleterPopup, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

    resize(720,480);
    setWindowTitle(tr("Code Completion"));
}
void CodeCompletion::keyPressEvent(QKeyEvent *e)
{
    if(m_CompleterPopup->popup()->isVisible())
    {
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
    if(!isShortcut)
        QTextEdit::keyPressEvent(e);

    static QString endOfWordCharacters("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
    bool hasModifier = (e->modifiers() != Qt::NoModifier);

    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    QString completionPrefix = tc.selectedText();
    unlockkey = false;
    if(!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 1 || endOfWordCharacters.contains(e->text().right(1))))
    {
        m_CompleterPopup->popup()->hide();
        return;
    }

    populateCompleterPopupViaClangCodeComplete(completionPrefix);

    if(completionPrefix != m_CompleterPopup->completionPrefix())
    {
        m_CompleterPopup->setCompletionPrefix(completionPrefix);
        m_CompleterPopup->popup()->setCurrentIndex(m_CompleterPopup->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(m_CompleterPopup->popup()->sizeHintForColumn(0) + m_CompleterPopup->popup()->verticalScrollBar()->sizeHint().width());
    m_CompleterPopup->complete(cr);
    unlockkey = true;

}
void CodeCompletion::focusInEvent(QFocusEvent *e)
{
    m_CompleterPopup->setWidget(this);
    QTextEdit::focusInEvent(e);
}
void CodeCompletion::populateCompleterPopupViaClangCodeComplete(const QString &token)
{
/*
    CXUnsavedFile unsavedFiles[1];
    unsavedFiles[0].Filename = CodeCompletion_DUMMY_FILENAME;
    QString docString = document()->toPlainText();
    std::string docStdString = docString.toStdString();
    unsavedFiles[0].Contents = docStdString.c_str();
    unsavedFiles[0].Length = docStdString.length();

    CXTranslationUnit translationUnit = clang_parseTranslationUnit(m_ClangIndex, CodeCompletion_DUMMY_FILENAME, 0, 0, unsavedFiles, 1, CXTranslationUnit_PrecompiledPreamble);
    if(!translationUnit)
    {
        QMessageBox::critical(this, tr("Error"), tr("parseTranslationUnit failed"));
        m_CompleterPopup->setModel(new QStringListModel(m_CompleterPopup));
        return;
    }

    //clang_reparseTranslationUnit(u, 0, 0, 0);

    int line = textCursor().blockNumber()+1;
    qDebug() << "Line: " << line;
    int column = (textCursor().positionInBlock()+1)-token.length(); //columNumber instead?
    qDebug() << "Column: " << column;

    CXCodeCompleteResults* codeCompleteResults = clang_codeCompleteAt(translationUnit, CodeCompletion_DUMMY_FILENAME, line, column, unsavedFiles, 1, 0);
    if(!codeCompleteResults)
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not complete"));
        m_CompleterPopup->setModel(new QStringListModel(m_CompleterPopup));
        return;
    }
    */

    QStringList codeCompletionEntries;
    /*
    for(unsigned i = 0; i < codeCompleteResults->NumResults; ++i)
    {
        const CXCompletionString& str = codeCompleteResults->Results[i].CompletionString;
        for(unsigned j = 0; j < clang_getNumCompletionChunks(str); ++j)
        {
            if(clang_getCompletionChunkKind(str, j) != CXCompletionChunk_TypedText)
                continue;

            const CXString& out = clang_getCompletionChunkText(str, j);
            const char* codeCompletionEntry = clang_getCString(out);
            QString codeCompletionEntryQString = QString::fromLatin1(codeCompletionEntry, strlen(codeCompletionEntry));
            if(!codeCompletionEntryQString.trimmed().isEmpty())
                codeCompletionEntries.append(codeCompletionEntry);
        }
    }

    m_CompleterPopup->setModel(new QStringListModel(codeCompletionEntries, m_CompleterPopup));
    clang_disposeCodeCompleteResults(codeCompleteResults);
*/
    char test1[] = "include";
    char test2[] = "inline";
    char test3[] = "int";
    char test4[] = "iostream";
    char test5[] = "stdlib";
    char test6[] = "stdio";
    char test7[] = "system";
    char test8[] = "fprintf";
    char test9[] = "fscanf";
    char test10[] = "main";
    char test11[] = "maintain";
    codeCompletionEntries.append(test1);
    codeCompletionEntries.append(test2);
    codeCompletionEntries.append(test3);
    codeCompletionEntries.append(test4);
    codeCompletionEntries.append(test5);
    codeCompletionEntries.append(test6);
    codeCompletionEntries.append(test7);
    codeCompletionEntries.append(test8);
    codeCompletionEntries.append(test9);

    m_CompleterPopup->setModel(new QStringListModel(codeCompletionEntries, m_CompleterPopup));
/*
    bool testtest = m_CompleterPopup->setCurrentRow(1);
    qDebug() << testtest << endl;
*/

}
void CodeCompletion::insertCompletion(const QString &completion)
{
    if(m_CompleterPopup->widget() != this)
        return;
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    tc.removeSelectedText(); //capitalization may have been wrong, since we're doing case insensitive completing
    tc.insertText(completion);
    setTextCursor(tc);
}
