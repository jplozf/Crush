#ifndef XEQPROCESS_H
#define XEQPROCESS_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QTextEdit>
#include <QTime>
#include "app.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class XeqProcess : public QObject
{
    Q_OBJECT
public:
    explicit XeqProcess(QString program, QStringList arguments, App *app, Ui::MainWindow *ui);
    void killMe();

private:
    QProcess mProcess;
    QTextEdit *outWidget;
    QTextEdit *errWidget;
    App *app;
    Ui::MainWindow *ui;
    QTime t;

private slots:
    void readyReadStandardOutput();
    void readyReadStandardError();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void errorOccurred(QProcess::ProcessError error);
};

#endif // XEQPROCESS_H
