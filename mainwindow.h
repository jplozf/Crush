#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtXml>
#include <QFile>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QTimer>
#include <QShortcut>
#include <QHash>
#include <QClipboard>
#include <QGuiApplication>

#include "qobjectdefs.h"
#include "ui_mainwindow.h"
#include "app.h"
#include "utils.h"
#include "commandui.h"
#include "BasicXMLSyntaxHighlighter.h"
#include "XeqProcess.h"

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
    void initUI();
    bool dirtyFlag;

private:
    Ui::MainWindow *ui;
    void populateCommandsList();
    void buildCommandScreen(QString cmd);
    bool openXMLFile(QString fName = "*DEFAULT");
    QDomDocument xmlCommands;
    void showMessage(const QString &message, int timeout = -1);
    QString fName;
    BasicXMLSyntaxHighlighter *highlighter ;
    void setDelayed();
    XeqProcess *xp;
    CommandUI *cUI;
    bool eventFilter(QObject *watched, QEvent *event);
    QList<QString> aCommands;
    int iCommands;
    QLabel *lblRC;
    QLabel *lblTimeElpased;
    QLabel *lblLED;
    QLabel *lblPID;
    void mergeXML(QString f1, QString f2);
    void getEnvironment();

private slots:
    void slotSelectCommand(const QString);
    void slotDoExit();
    void slotDoOpenFile();
    void slotDoEditXML();
    void slotCursorPosition();
    void slotTextChanged();
    void slotDoSaveXML();
    void slotDoImportXML();
    void slotDoExportXML();
    void slotRunCommand();
    void slotBreakCommand();
    void slotClearConsole();
    void slotCopyConsole();

};
#endif // MAINWINDOW_H
