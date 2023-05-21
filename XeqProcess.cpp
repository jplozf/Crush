#include "XeqProcess.h"
#include "qwidget.h"
#include "ui_mainwindow.h"

//******************************************************************************
// XeqProcess()
//******************************************************************************
XeqProcess::XeqProcess(QString program, QStringList arguments, App *app, Ui::MainWindow *ui) {
    if (!app->processRunning) {
        this->t.start();
        this->app = app;
        this->ui = ui;
        this->app->processRunning = true;

        QLabel *lblRC = this->ui->statusBar->findChild<QLabel*>("lblRC");
        lblRC->setText(QString("RC=R"));

        QLabel *lblLED = this->ui->statusBar->findChild<QLabel*>("lblLED");
        lblLED->setPixmap(QPixmap(":/led_red.png"));

        QLabel *lblTimeElpased = this->ui->statusBar->findChild<QLabel*>("lblTimeElpased");
        lblTimeElpased->setText("------ ms");

        outWidget = this->ui->txtConsoleOut;
        errWidget = this->ui->txtConsoleOut;
        outWidget->setTextBackgroundColor(this->app->appSettings->get("CONSOLE_BACKGROUND").value<QColor>());
        outWidget->setTextColor(this->app->appSettings->get("CONSOLE_PROMPT_COLOR").value<QColor>());
        outWidget->append("");
        outWidget->append(this->app->appSettings->get("CONSOLE_PROMPT").toString() + program + " " +arguments.join(" "));

        connect(&mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
        connect(&mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
        connect(&mProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
        connect(&mProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(errorOccurred(QProcess::ProcessError)));

        mProcess.start(program, arguments);
        this->PID = mProcess.processId();
        QLabel *lblPID = this->ui->statusBar->findChild<QLabel*>("lblPID");
        lblPID->setText(QString("%1").arg(this->PID, 20));
    }
}

//******************************************************************************
// errorOccured()
//******************************************************************************
void XeqProcess::errorOccurred(QProcess::ProcessError error) {
    qDebug() << "ERROR_OCCURED";
    QString txtError;
    if (errWidget!=nullptr) {
        switch (error) {
            case QProcess::FailedToStart:
                txtError = "QProcess::FailedToStart";
            break;
            case QProcess::Crashed:
                txtError = "QProcess::Crashed";
            break;
            case QProcess::Timedout:
                txtError = "QProcess::Timedout";
            break;
            case QProcess::WriteError:
                txtError = "QProcess::WriteError";
            break;
            case QProcess::ReadError:
                txtError = "QProcess::ReadError";
            break;
            case QProcess::UnknownError:
            default:
                txtError = "QProcess::UnknownError";
            break;
        }
        errWidget->setTextColor(this->app->appSettings->get("CONSOLE_ERR_COLOR").value<QColor>());
        errWidget->append(txtError);
    }

    int elapsed = this->t.elapsed();
    this->app->processRunning = false;
    QLabel *lblRC = this->ui->statusBar->findChild<QLabel*>("lblRC");
    lblRC->setText(QString("RC=X"));

    QLabel *lblLED = this->ui->statusBar->findChild<QLabel*>("lblLED");
    lblLED->setPixmap(QPixmap(":/led_green.png"));

    QLabel *lblTimeElpased = this->ui->statusBar->findChild<QLabel*>("lblTimeElpased");
    QString eTime;
    eTime.sprintf("%06d ms", elapsed);
    lblTimeElpased->setText(eTime);

    QLabel *lblPID = this->ui->statusBar->findChild<QLabel*>("lblPID");
    lblPID->setText("                    ");

    if (this->ui->chkNotification->isChecked()) {
#ifdef Q_OS_LINUX
        system("notify-send \"Crush\" \"Command completed in error.\"");
        qDebug() << "NOTIFICATION";
#else
        QMessageBox *mbox = new QMessageBox;
        mbox->setWindowTitle(tr("Crush"));
        mbox->setText("Command completed in error.");
        mbox->show();
        QTimer::singleShot(2000, mbox, SLOT(hide()));
#endif
    }
}

//******************************************************************************
// finished()
//******************************************************************************
void XeqProcess::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "FINISHED";
    int elapsed = this->t.elapsed();
    this->app->processRunning = false;
    QLabel *lblRC = this->ui->statusBar->findChild<QLabel*>("lblRC");
    lblRC->setText(QString("RC=%1").arg(exitCode));

    QLabel *lblLED = this->ui->statusBar->findChild<QLabel*>("lblLED");
    lblLED->setPixmap(QPixmap(":/led_green.png"));

    QLabel *lblTimeElpased = this->ui->statusBar->findChild<QLabel*>("lblTimeElpased");
    QString eTime;
    eTime.sprintf("%06d ms", elapsed);
    lblTimeElpased->setText(eTime);

    QLabel *lblPID = this->ui->statusBar->findChild<QLabel*>("lblPID");
    lblPID->setText("                    ");

    if (this->ui->chkNotification->isChecked()) {
#ifdef Q_OS_LINUX
        system("notify-send \"Crush\" \"Command completed successfully.\"");
        qDebug() << "NOTIFICATION";
#else
        QMessageBox *mbox = new QMessageBox;
        mbox->setWindowTitle(tr("Crush"));
        mbox->setText("Command completed successfully.");
        mbox->show();
        QTimer::singleShot(2000, mbox, SLOT(hide()));
#endif
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
        QTextCursor cursor = outWidget->textCursor();
        cursor.movePosition(QTextCursor::End);
        outWidget->setTextCursor(cursor);
    }
    // qDebug() << out;
}

//******************************************************************************
// readyReadStandardError()
//******************************************************************************
void XeqProcess::readyReadStandardError() {
    QString err = mProcess.readAllStandardError();
    if (errWidget!=nullptr) {
        errWidget->setTextColor(this->app->appSettings->get("CONSOLE_ERR_COLOR").value<QColor>());
        errWidget->append(err.trimmed());
        QTextCursor cursor = errWidget->textCursor();
        cursor.movePosition(QTextCursor::End);
        errWidget->setTextCursor(cursor);
    }
    // qDebug() << err;
}

//******************************************************************************
// killMe()
//******************************************************************************
void XeqProcess::killMe() {
    if (mProcess.state() != QProcess::NotRunning)
        mProcess.kill();
}
