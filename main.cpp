#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/crush_icon.png"));
    MainWindow w;
    w.show();
    return a.exec();
}
