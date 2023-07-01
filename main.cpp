#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");
    /*
    palette.setColor(QPalette.Window, QColor(theme[0]));
    palette.setColor(QPalette.WindowText, QColor(theme[1]));
    palette.setColor(QPalette.Base, QColor(theme[2]));
    palette.setColor(QPalette.AlternateBase, QColor(theme[3]));
    palette.setColor(QPalette.ToolTipBase, QColor(theme[4]));
    palette.setColor(QPalette.ToolTipText, QColor(theme[5]));
    palette.setColor(QPalette.Text, QColor(theme[6]));
    palette.setColor(QPalette.Button, QColor(theme[7]));
    palette.setColor(QPalette.ButtonText, QColor(theme[8]));
    palette.setColor(QPalette.BrightText, QColor(theme[9]));
    palette.setColor(QPalette.Link, QColor(theme[10]));
    palette.setColor(QPalette.Highlight, QColor(theme[11]));
    palette.setColor(QPalette.HighlightedText, QColor(theme[12]));
    */
    a.setWindowIcon(QIcon(":/crush_icon.png"));
    MainWindow w;
    w.show();
    return a.exec();
}
