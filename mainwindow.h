#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml>
#include <QFile>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>

#include "qobjectdefs.h"
#include "ui_mainwindow.h"
#include "app.h"
#include "utils.h"
#include "commandui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    App *app;
    void closeEvent(QCloseEvent *);
    void saveSettings();
    void readSettings();

private:
    Ui::MainWindow *ui;
    void populateCommandsList();
    void buildCommandScreen(QString cmd);
    bool openXMLFile(QString fName = "*DEFAULT");
    QDomDocument xmlCommands;
    void showMessage(const QString &message, int timeout = -1);
    QString fName;

private slots:
    void slotSelectCommand(const QString);
    void slotDoExit();

};
#endif // MAINWINDOW_H
