#ifndef CodeCompletion_H
#define CodeCompletion_H

#include <QTextEdit>
#include <QThread>
#include <clang-c/Index.h>

#define MAXL 1000
#define MAXN 10000
struct GyroDATA{
    int x;
    int y;
};



void EEGCLOSE();
GyroDATA GetGyroDATA();
void EEGINIT();

class QCompleter;

class CThread : public QThread
{
    Q_OBJECT
    public:
        CThread();
        void run();
};

class CodeCompletion : public QTextEdit
{
    Q_OBJECT
public:
    CodeCompletion(QWidget *parent = 0);
    virtual ~CodeCompletion() { }
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
private:
    QCompleter *m_CompleterPopup;
    CXIndex m_ClangIndex;
    void populateCompleterPopupViaClangCodeComplete(const QString &token);
private slots:
    void insertCompletion(const QString &completion);
};



#endif // CodeCompletion_H
