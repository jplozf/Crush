#include "XeqProcess.h"
#include "ui_mainwindow.h"

//******************************************************************************
// XeqProcess()
//******************************************************************************
XeqProcess::XeqProcess(QString program, QStringList arguments, QString cwd, App *app, Ui::MainWindow *ui) {
    if (!app->processRunning) {
        this->t.start();
        this->app = app;
        this->ui = ui;
        this->app->processRunning = true;
        this->mProcess = new QProcess();

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

        connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
        connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
        connect(mProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
        connect(mProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(errorOccurred(QProcess::ProcessError)));

        QString prefix = this->app->appSettings->get("CONSOLE_SHELL_PREFIX").toString();
        if (prefix != "") {
            QStringList prefArgs = prefix.split(" ");
            prefArgs.append(program);
            program = prefArgs[0];
            int i(0);
            for (const auto& a : prefArgs) {
                if (i > 0) {
                    arguments.insert(i-1, a);
                }
                i++;
            }
        }
        qDebug() << program;
        qDebug() << arguments;
        mProcess->setWorkingDirectory(cwd);
        mProcess->start(program, arguments);

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        // timer->start(100);

        this->PID = mProcess->processId();
        QLabel *lblPID = this->ui->statusBar->findChild<QLabel*>("lblPID");
        lblPID->setText(QString("PID:%1").arg(this->PID, 16));
    }
}

//******************************************************************************
// update()
//******************************************************************************
void XeqProcess::update() {
    qDebug() << "FLUSH";
    fflush(stdout);
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
        notify("Command completed in error.");
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
        notify("Command completed successfully.");
    }
}

//******************************************************************************
// readyReadStandardOutput()
//******************************************************************************
void XeqProcess::readyReadStandardOutput() {
    QByteArray out = mProcess->readAllStandardOutput();
    if (outWidget!=nullptr) {
        QTextCodec *codec = QTextCodec::codecForName(this->app->appSettings->get("CONSOLE_CODEPAGE").toString().toStdString().c_str());
        outWidget->setTextColor(this->app->appSettings->get("CONSOLE_OUT_COLOR").value<QColor>());
        outWidget->append(codec->toUnicode(out.trimmed()));
        QTextCursor cursor = outWidget->textCursor();
        cursor.movePosition(QTextCursor::End);
        outWidget->setTextCursor(cursor);
    }
    qDebug() << out;
}

//******************************************************************************
// readyReadStandardError()
//******************************************************************************
void XeqProcess::readyReadStandardError() {
    QByteArray err = mProcess->readAllStandardError();
    if (errWidget!=nullptr) {
        QTextCodec *codec = QTextCodec::codecForName(this->app->appSettings->get("CONSOLE_CODEPAGE").toString().toStdString().c_str());
        errWidget->setTextColor(this->app->appSettings->get("CONSOLE_ERR_COLOR").value<QColor>());
        errWidget->append(codec->toUnicode(err.trimmed()));
        QTextCursor cursor = errWidget->textCursor();
        cursor.movePosition(QTextCursor::End);
        errWidget->setTextCursor(cursor);
    }
    qDebug() << err;
}

//******************************************************************************
// killMe()
//******************************************************************************
void XeqProcess::killMe() {
    if (mProcess->state() != QProcess::NotRunning)
        mProcess->kill();
}

//******************************************************************************
// notify()
//******************************************************************************
void XeqProcess::notify(QString message, QString title) {
    if (title == "*DEFAULT") {
        title = this->app->appConstants->getQString("APPLICATION_NAME");
    }
    qDebug() << "NOTIFICATION";
#ifdef Q_OS_LINUX
    if (this->app->appSettings->get("APPLICATION_NOTIFICATION_SYSTEM").toBool() == true) {
        system(QString("notify-send \"%1\" \"%2\"").arg(title).arg(message).toStdString().c_str());
    } else {
        QMessageBox *mbox = new QMessageBox;
        mbox->setWindowTitle(title);
        mbox->setText(message);
        mbox->show();

        QSize sizePopup = mbox->frameSize();
        QScreen *screen = qApp->screens().at(0);
        int heightScreen = screen->availableGeometry().height();
        int widthScreen = screen->availableGeometry().width();
        mbox->move(widthScreen - sizePopup.width() - 10, heightScreen - sizePopup.height() - 10);

        QTimer::singleShot(this->app->appSettings->get("APPLICATION_NOTIFICATION_TIMEOUT").toInt(), mbox, SLOT(hide()));
    }
#else
    QMessageBox *mbox = new QMessageBox;
    mbox->setWindowTitle(title);
    mbox->setText(message);
    mbox->show();

    QSize sizePopup = mbox->frameSize();
    QScreen *screen = qApp->screens().at(0);
    int heightScreen = screen->availableGeometry().height();
    int widthScreen = screen->availableGeometry().width();
    mbox->move(widthScreen - sizePopup.width() - 10, heightScreen - sizePopup.height() - 10);

    QTimer::singleShot(this->app->appSettings->get("APPLICATION_NOTIFICATION_TIMEOUT").toInt(), mbox, SLOT(hide()));
#endif
}
