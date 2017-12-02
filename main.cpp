#include "CodeCompletion.h"
#include <QApplication>

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
    CThread *EEG = new CThread();
    EEG->start();
    CodeCompletion w;

    w.show();

    return a.exec();
}
