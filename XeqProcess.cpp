#include "XeqProcess.h"
#include "qwidget.h"
#include "ui_mainwindow.h"

//******************************************************************************
// XeqProcess()
//******************************************************************************
XeqProcess::XeqProcess(QString program, QStringList arguments, App *app, Ui::MainWindow *ui) {
    this->app = app;
    this->ui = ui;
    outWidget = this->ui->txtConsoleOut;
    errWidget = this->ui->txtConsoleOut;
    if (mProcess.state() == QProcess::NotRunning) {
        if (outWidget!=nullptr) {
            outWidget->setTextBackgroundColor(this->app->appSettings->get("CONSOLE_BACKGROUND").value<QColor>());
            outWidget->setTextColor(this->app->appSettings->get("CONSOLE_PROMPT_COLOR").value<QColor>());
            outWidget->append(this->app->appSettings->get("CONSOLE_PROMPT").toString() + program + " " +arguments.join(" "));
        }
        mProcess.start(program, arguments);
        connect(&mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
        connect(&mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    }
}

//******************************************************************************
// readyReadStandardOutput()
//******************************************************************************
void XeqProcess::readyReadStandardOutput() {
    QString out = mProcess.readAllStandardOutput();
    if (outWidget!=nullptr) {
        outWidget->setTextColor(this->app->appSettings->get("CONSOLE_OUT_COLOR").value<QColor>());
        outWidget->append(out.trimmed());
    }
    qDebug() << out;
}

//******************************************************************************
// readyReadStandardError()
//******************************************************************************
void XeqProcess::readyReadStandardError() {
    QString err = mProcess.readAllStandardError();
    if (errWidget!=nullptr) {
        errWidget->setTextColor(this->app->appSettings->get("CONSOLE_ERR_COLOR").value<QColor>());
        errWidget->append(err.trimmed());
    }
    qDebug() << err;
}

//******************************************************************************
// killMe()
//******************************************************************************
void XeqProcess::killMe() {
    if (mProcess.state() != QProcess::NotRunning)
        mProcess.kill();
}
