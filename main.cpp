#include "mainwindow.h"
#include <QApplication>

QApplication *a;

//******************************************************************************
// main()
//******************************************************************************
int main(int argc, char *argv[])
{
    a = new QApplication(argc, argv);
    a->setWindowIcon(QIcon(":/crush_icon.png"));
    MainWindow w(a);
    w.show();
    return a->exec();
}
