#include "mainwindow.h"

#include <QGraphicsTextItem>

//******************************************************************************
// LauncherWindow()
//******************************************************************************
LauncherWindow::LauncherWindow(QWidget *parent) : QMainWindow(parent, Qt::CustomizeWindowHint ) {
    appSettings = new Settings();
    appConstants = new Constants();    
    alarms = new Alarms();

    //****************************************************************************
    // Create application folder if not exists
    //****************************************************************************
    appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    if (!QDir(appDir).exists()) {
        QDir().mkdir(appDir);
    }

    //****************************************************************************
    // Create launcher subfolder if not exists
    //****************************************************************************
    launcherDir = QDir(appDir).filePath(appConstants->getQString("LAUNCHER_FOLDER"));
    if (!QDir(launcherDir).exists()) {
        QDir().mkdir(launcherDir);
    }
    currentLauncherDir = launcherDir;

    setObjectName("borderlessMainWindow");
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::Tool);
    // setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    // to fix taskbar minimize feature
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_QuitOnClose, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setStyleSheet("#borderlessMainWindow{border:1px solid palette(highlight);}");

    mMainWindow = new MainWindow(this);
    setWindowTitle(mMainWindow->windowTitle());
    rpn = new RPN(mMainWindow);

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->setMargin(1);

    QHBoxLayout *windowBarLayout = new QHBoxLayout();
    windowBarLayout->setSpacing(0);
    windowBarLayout->setMargin(0);

    mTitlebarWidget = new QWidget(this);
    mTitlebarWidget->setObjectName("titlebarWidget");
    mTitlebarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mTitlebarWidget->setLayout(windowBarLayout);

    /*
    btnMinimize = new QPushButton(mTitlebarWidget);
    btnMinimize->setObjectName("minimizeButton");
    connect(btnMinimize, SIGNAL(clicked()), this, SLOT(slot_minimized()));

    btnRestore = new QPushButton(mTitlebarWidget);
    btnRestore->setObjectName("restoreButton");
    btnRestore->setVisible(false);
    connect(btnRestore, SIGNAL(clicked()), this, SLOT(slot_restored()));

    btnMaximize = new QPushButton(mTitlebarWidget);
    btnMaximize->setObjectName("maximizeButton");
    connect(btnMaximize, SIGNAL(clicked()), this, SLOT(slot_maximized()));

    btnClose = new QPushButton(mTitlebarWidget);
    btnClose->setObjectName("closeButton");
    btnClose->setIcon(QIcon(":/png/16x16/Close.png"));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(slot_closed()));
    */

    btnClock = new QPushButton(mTitlebarWidget);
    btnClock->setObjectName("clockButton");
    btnClock->setFlat(true);
    btnClock->setIcon(QIcon(":/png/16x16/Clock.png"));
    connect(btnClock, SIGNAL(clicked()), this, SLOT(slotClock()));

    btnPack = new QPushButton(mTitlebarWidget);
    btnPack->setObjectName("packButton");
    btnPack->setFlat(true);
    btnPack->setIcon(QIcon(":/png/16x16/Attach.png"));
    connect(btnPack, SIGNAL(clicked()), this, SLOT(slotPack()));

    /*
    btnSettings = new QPushButton(mTitlebarWidget);
    btnSettings->setObjectName("settingsButton");
    btnSettings->setFlat(true);
    btnSettings->setIcon(QIcon(":/png/16x16/Gear.png"));
    connect(btnSettings, SIGNAL(clicked()), this, SLOT(slotSettings()));
    */

    btnLock = new QPushButton(mTitlebarWidget);
    btnLock->setObjectName("lockButton");
    btnLock->setFlat(true);
    btnLock->setIcon(QIcon(":/png/16x16/Lock.png"));
    connect(btnLock, SIGNAL(clicked()), this, SLOT(slotLock()));

    btnOff = new QPushButton(mTitlebarWidget);
    btnOff->setObjectName("offButton");
    btnOff->setFlat(true);
    btnOff->setIcon(QIcon(":/png/16x16/Standby.png"));
    connect(btnOff, SIGNAL(clicked()), this, SLOT(slotOff()));

    lblTitle = new QLabel(mTitlebarWidget);
    lblTitle->setObjectName("windowTitle");
    lblTitle->setText("Hello");

    lblPrompt = new QLabel(mTitlebarWidget);

    btnPin = new QPushButton(mTitlebarWidget);
    btnPin->setObjectName("pinButton");
    btnPin->setFlat(true);
    btnPin->setIcon(QIcon(":/png/16x16/Player Play.png"));
    connect(btnPin, SIGNAL(clicked()), this, SLOT(slotPin()));

    txtCommand = new QLineEdit(mTitlebarWidget);
    txtCommand->setObjectName("commandLine");
    txtCommand->setStyleSheet("font: 10pt 'Consolas';");
    txtCommand->setText("");
    connect(txtCommand, SIGNAL(returnPressed()), this, SLOT(slotEnter()));
    txtCommand->installEventFilter(this);

    // windowBarLayout->addWidget(lblPrompt);
    windowBarLayout->addWidget(btnPin);
    windowBarLayout->addWidget(lblPrompt);
    windowBarLayout->addWidget(txtCommand);
    windowBarLayout->addWidget(lblTitle);

    // windowBarLayout->addStretch(1);
    /*
    windowBarLayout->addWidget(btnMinimize);
    windowBarLayout->addWidget(btnRestore);
    windowBarLayout->addWidget(btnMaximize);
    windowBarLayout->addWidget(btnClose);
    */
    windowBarLayout->addWidget(btnClock);
    windowBarLayout->addWidget(btnPack);
    // windowBarLayout->addWidget(btnSettings);
    windowBarLayout->addWidget(btnLock);
    windowBarLayout->addWidget(btnOff);

    if (appSettings->get("TITLE_BAR_POSITION_TOP").toBool()) {
        verticalLayout->addWidget(mTitlebarWidget);
        verticalLayout->addWidget(mMainWindow);
    }
    else {
        verticalLayout->addWidget(mMainWindow);
        verticalLayout->addWidget(mTitlebarWidget);
    }

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralWidget->setLayout(verticalLayout);

    connect(mMainWindow->ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
    connect(mMainWindow->ui->btnClearConsole, SIGNAL(clicked()), this, SLOT(slotClearConsole()));
    connect(mMainWindow->ui->btnOpenFile, SIGNAL(clicked()), this, SLOT(slotOpenFile()));
    connect(mMainWindow->ui->btnCloseFile, SIGNAL(clicked()), this, SLOT(slotCloseFile()));
    connect(mMainWindow->ui->btnKillProcess, SIGNAL(clicked()), this, SLOT(slotKillProcess()));

    mMainWindow->ui->btnCloseFile->setEnabled(false);
    mMainWindow->ui->btnKillProcess->setEnabled(false);

    lblDepthStack = new QLabel("0");
    lblDepthStack->setStyleSheet("border: 1px solid black;");
    mMainWindow->ui->statusbar->addPermanentWidget(lblDepthStack);

    lblAngularMode = new QLabel((rpn->angularMode==RPN::DEGREES) ? "DEG" : "RAD");
    lblAngularMode->setStyleSheet("border: 1px solid black;");
    mMainWindow->ui->statusbar->addPermanentWidget(lblAngularMode);

    //****************************************************************************
    // Fill the settings panel
    //****************************************************************************
    appSettings->form(mMainWindow->ui->widSettings);

    //****************************************************************************
    // Set Console style
    //****************************************************************************
    setConsoleStyle();

    //****************************************************************************
    // Display clock timer
    //****************************************************************************
    QTimer *tTime = new QTimer(this);
    tTime->setInterval(1000);
    connect(tTime, &QTimer::timeout, [&]() {
        QString time1 = QDateTime::currentDateTime().toString(appSettings->get("TITLE_BAR_TIME_FORMAT").toString());
        lblTitle->setText(time1);
    } );
    tTime->start();

    //****************************************************************************
    // Alarms timer
    //****************************************************************************
    QTimer *tAlarm=new QTimer(this);
    tAlarm->setInterval(15000);
    connect(tAlarm, SIGNAL(timeout()), this, SLOT(alarmCheck()));
    tAlarm->start();

    setCentralWidget(centralWidget);
    readSettings();
    displayStack(rpn->stack);
    displayVars(rpn->vars);

    initLauncher();
    showMessage("Welcome");
    txtCommand->setFocus();

    //****************************************************************************
    // Set application visible on all desktops for Linux
    //****************************************************************************
#ifdef Q_OS_LINUX
    unsigned long data = 0xFFFFFFFF;
    XChangeProperty (QX11Info::display(),
                  winId(),
                  XInternAtom(QX11Info::display(), "_NET_WM_DESKTOP", false),
                  XA_CARDINAL,
                  32,
                  PropModeReplace,
                  reinterpret_cast<unsigned char *>(&data),
                  1);
#endif

    //****************************************************************************
    // Debug Screens
    //****************************************************************************
    qDebug() << "*** Qt screens ***";
    const auto screens = qApp->screens();
    for (int ii = 0; ii < screens.count(); ++ii) {
        qDebug() << ii+1 << screens[ii]->geometry();
    }
}

//******************************************************************************
// setConsoleStyle()
//******************************************************************************
void LauncherWindow::setConsoleStyle() {
    QString css;
    css.sprintf("QTextEdit {color: '%s'; background-color: '%s'; font-size: %spx;}",
                appSettings->get("CONSOLE_TEXT_COLOR").toString().toStdString().c_str(),
                appSettings->get("CONSOLE_BACKGROUND_COLOR").toString().toStdString().c_str(),
                appSettings->get("CONSOLE_FONT_SIZE").toString().toStdString().c_str()
                );
    qDebug() << css;
    mMainWindow->ui->txtConsole->setStyleSheet(css);
}

//******************************************************************************
// setPrompt()
//******************************************************************************
void LauncherWindow::setPrompt(QString s) {
    lblPrompt->setText(s);
}

//******************************************************************************
// clearPrompt()
//******************************************************************************
void LauncherWindow::clearPrompt() {
    lblPrompt->setText("");
}

//******************************************************************************
// eventFilter()
//******************************************************************************
bool LauncherWindow::eventFilter(QObject* obj, QEvent *event) {
    if (obj == txtCommand) {
        if(event->type() == 6 ) { // QEvent::KeyPress
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Up) {
                // qDebug() << "lineEdit -> Qt::Key_Up";
                txtCommand->setText(rpn->getPrevious());
                return true;
            } else if(keyEvent->key() == Qt::Key_Down) {
                // qDebug() << "lineEdit -> Qt::Key_Down";
                txtCommand->setText(rpn->getNext());
                return true;
            }
        }
        return false;
    }
    return QMainWindow::eventFilter(obj, event);
}

//******************************************************************************
// tabChanged()
//******************************************************************************
void LauncherWindow::onTabChanged(int iTab) {
    if (currentTabIndex == 3 && mMainWindow->movieWidget->isVideoAvailable == true) {
        mMainWindow->movieWidget->pauseForce();
    }
    if (iTab == 3) {
        if (mMainWindow->movieWidget->isPlaying == true && mMainWindow->movieWidget->isVideoAvailable == true) {
            mMainWindow->movieWidget->playForce();
        }
    }
    currentTabIndex = iTab;
}

//******************************************************************************
// closeEvent()
//******************************************************************************
void LauncherWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
    QMainWindow::closeEvent(event);
}

//******************************************************************************
// saveSettings()
//******************************************************************************
void LauncherWindow::saveSettings() {
    //**************************************************************************
    // Application state saving
    //**************************************************************************
    QSettings registry(appConstants->getQString("ORGANIZATION_NAME"), appConstants->getQString("APPLICATION_NAME"));
    registry.setValue("geometry", saveGeometry());
    registry.setValue("windowState", saveState());
    registry.setValue("splitFocus", mMainWindow->ui->tabVideoSplitter->saveState());
    registry.setValue("splitConsole", mMainWindow->ui->tabConsoleSplitter->saveState());
    registry.setValue("currentTab", mMainWindow->ui->tabWidget->currentIndex());
    registry.setValue("currentMedia", mMainWindow->movieWidget->currentMedia);
    registry.setValue("currentSession", mMainWindow->movieWidget->file_1_url_2);
    registry.setValue("currentPosition", mMainWindow->movieWidget->mediaPlayer->position());
    registry.setValue("angularMode", lblAngularMode->text());
    // TODO : Add Pin/Windowed state saving

    //**************************************************************************
    // RPN::Entries saving
    //**************************************************************************
    QVectorIterator<QString> iEntries(rpn->entries);
    int jEntries(0);
    int cEntries(0);
    int mEntries(appSettings->get("CONSOLE_HISTORY").toInt());
    int eEntries(rpn->entries.length());
    qDebug() << eEntries;
    registry.beginWriteArray("Entries");
    while (iEntries.hasNext()) {
        QString entry = iEntries.next();
        if ((eEntries - cEntries) <= mEntries) {
            registry.setArrayIndex(jEntries++);
            registry.setValue("Entry", entry);
        }
        cEntries++;
    }
    registry.endArray();

    //**************************************************************************
    // RPN::Vars saving
    //**************************************************************************
    registry.setValue("Variables", rpn->vars);

    //**************************************************************************
    // RPN::Stack saving
    //**************************************************************************
    QVectorIterator<QVariant> iStack(rpn->stack);
    int jStack(0);
    registry.beginWriteArray("Stack");
    while (iStack.hasNext()) {
        registry.setArrayIndex(jStack++);
        QVariant v = iStack.next();
        qDebug() << v.typeName();
        registry.setValue("StackValue", v);
        registry.setValue("StackType", v.typeName());
    }
    registry.endArray();

    //**************************************************************************
    // Notepad saving
    //**************************************************************************
    QFile fNotepad(QDir(appDir).filePath(appConstants->getQString("NOTEPAD_FILE")));
    fNotepad.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    fNotepad.write(mMainWindow->ui->txtNotepad->toPlainText().toUtf8());
    fNotepad.close();

    //**************************************************************************
    // Focus saving
    //**************************************************************************
    QFile fFocus(QDir(appDir).filePath(appConstants->getQString("FOCUS_FILE")));
    fFocus.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    fFocus.write(mMainWindow->ui->txtFocus->toPlainText().toUtf8());
    fFocus.close();

    //**************************************************************************
    // Settings saving
    //**************************************************************************
    Settings mySettings;
    mySettings.write();
}

//******************************************************************************
// readSettings()
//******************************************************************************
void LauncherWindow::readSettings() {
    QSettings registry(appConstants->getQString("ORGANIZATION_NAME"), appConstants->getQString("APPLICATION_NAME"));

    const QByteArray geometry = registry.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }

    const QByteArray windowState = registry.value("windowState", QByteArray()).toByteArray();
    if (!windowState.isEmpty()) {
        restoreState(registry.value("windowState").toByteArray());
    }

    const QByteArray splitFocus = registry.value("splitFocus", QByteArray()).toByteArray();
    if (!splitFocus.isEmpty()) {
        mMainWindow->ui->tabVideoSplitter->restoreState(registry.value("splitFocus").toByteArray());
    }

    const QByteArray splitConsole = registry.value("splitConsole", QByteArray()).toByteArray();
    if (!splitConsole.isEmpty()) {
        mMainWindow->ui->tabConsoleSplitter->restoreState(registry.value("splitConsole").toByteArray());
    }

    const int currentTab = registry.value("currentTab").toInt();
    if (currentTab >= 0) {
        mMainWindow->ui->tabWidget->setCurrentIndex(currentTab);
        currentTabIndex = currentTab;
    }

    const int currentSession = registry.value("currentSession").toInt();
    const QString currentMedia = registry.value("currentMedia").toString();
    const qint64 currentPosition = registry.value("currentPosition").toUInt();
    lblAngularMode->setText((registry.value("angularMode").toString().isEmpty()) ? "DEG" : registry.value("angularMode").toString());
    rpn->angularMode = (lblAngularMode->text() == "DEG") ? RPN::DEGREES : RPN::RADIANS;
    bool currentRun = false;
    if (currentMedia != "" && currentSession != 0) {
        if (currentTab == 3) {
            currentRun = true;
        }
        mMainWindow->movieWidget->restoreSession(currentMedia, currentSession, currentPosition, currentRun);
    }
    // TODO : Add Pin/Windowed state restoring

    //**************************************************************************
    // RPN::Entries restoring
    //**************************************************************************
    int sizeEntries = registry.beginReadArray("Entries");
    for (int i = 0; i < sizeEntries; ++i) {
        registry.setArrayIndex(i);
        rpn->entries.append(registry.value("Entry").toString());
    }
    registry.endArray();

    //**************************************************************************
    // RPN::Vars restoring
    //**************************************************************************
    rpn->vars = registry.value("Variables").toMap();

    //**************************************************************************
    // RPN::Stack restoring
    //**************************************************************************
    int sizeStack = registry.beginReadArray("Stack");
    for (int i = 0; i < sizeStack; ++i) {
        registry.setArrayIndex(i);
        auto value = registry.value("StackValue");
        const auto typeName = registry.value("StackType").toString();
        const bool wasNull = value.isNull();
        const auto t = QMetaType::type(typeName.toUtf8());
        if (value.userType() != t && !value.convert(t) && !wasNull) {
            value = registry.value("StackVlue");
        }
        qDebug() << value.typeName();
        rpn->stack.append(value);
    }
    registry.endArray();

    //**************************************************************************
    // Notepad restoring
    //**************************************************************************
    QFile fNotepad(QDir(appDir).filePath(appConstants->getQString("NOTEPAD_FILE")));
    fNotepad.open(QFile::ReadOnly | QFile::Text);
    mMainWindow->ui->txtNotepad->setText(fNotepad.readAll());
    fNotepad.close();

    //**************************************************************************
    // Focus restoring
    //**************************************************************************
    QFile fFocus(QDir(appDir).filePath(appConstants->getQString("FOCUS_FILE")));
    fFocus.open(QFile::ReadOnly | QFile::Text);
    mMainWindow->ui->txtFocus->setText(fFocus.readAll());
    fFocus.close();
}

//******************************************************************************
// mousePressEvent()
//******************************************************************************
void LauncherWindow::mousePressEvent(QMouseEvent* event) {
    if (!mTitlebarWidget->underMouse() && !lblTitle->underMouse())
        return;

    if(event->button() == Qt::LeftButton) {
        mMoving = true;
        mLastMousePosition = event->pos();
    }
}

//******************************************************************************
// mouseMoveEvent()
//******************************************************************************
void LauncherWindow::mouseMoveEvent(QMouseEvent* event) {
    if (!mTitlebarWidget->underMouse() && !lblTitle->underMouse())
        return;

    void closeEvent(QCloseEvent *);
    if( event->buttons().testFlag(Qt::LeftButton) && mMoving) {
        this->move(this->pos() + (event->pos() - mLastMousePosition));
    }
}

//******************************************************************************
// mouseReleaseEvent()
//******************************************************************************
void LauncherWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (!mTitlebarWidget->underMouse() && !lblTitle->underMouse())
        return;

    if(event->button() == Qt::LeftButton) {
        mMoving = false;
    }
}

//******************************************************************************
// mouseDoubleClickEvent()
//******************************************************************************
void LauncherWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    if (!mTitlebarWidget->underMouse() && !lblTitle->underMouse())
        return;
    if (mMainWindow->isVisible()==true) {
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
        mMainWindow->setVisible(false);
    } else {
        this->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
        mMainWindow->setVisible(true);
    }
/*
    mMaximized = !mMaximized;
    if (mMaximized) {
        slotMaximized();
    } else {
        slotRestored();
    }
*/
}

//******************************************************************************
// slotMinimized()
//******************************************************************************
void LauncherWindow::slotMinimized() {
    setWindowState(Qt::WindowMinimized);
}

//******************************************************************************
// slotRestored()
//******************************************************************************
void LauncherWindow::slotRestored() {
    btnRestore->setVisible(false);
    btnMaximize->setVisible(true);
    setWindowState(Qt::WindowNoState);
    setStyleSheet("#borderlessMainWindow{border:1px solid palette(highlight);}");
}

//******************************************************************************
// slotMaximized()
//******************************************************************************
void LauncherWindow::slotMaximized() {
    btnRestore->setVisible(true);
    btnMaximize->setVisible(false);
    setWindowState(Qt::WindowMaximized);
    setStyleSheet("#borderlessMainWindow{border:1px solid palette(base);}");
}

//******************************************************************************
// slotEnter()
//******************************************************************************
void LauncherWindow::slotEnter() {
    QString cmd = txtCommand->text().trimmed();
    if (!cmd.isEmpty()) {
        rpn->setEntry(cmd);
        lblPrompt->setText("");
        int rc = rpn->xeq();
        if (rc == RPN::RC_OK) {
            displayOutput(cmd, rpn->out);
        } else if (rc == RPN::RC_CLS) {
            mMainWindow->ui->txtConsole->setText("");
            showMessage("Console cleared");
        } else if (rc == RPN::RC_MODE) {
            lblAngularMode->setText((rpn->angularMode==RPN::DEGREES) ? "DEG" : "RAD");
            displayOutput(cmd, rpn->out);
            // TODO : Display stack
        } else if (rc == RPN::RC_LOCK) {
            slotLock();
        } else if (rc == RPN::RC_EXIT) {
            slotClosed();
        } else if (rc == RPN::RC_SHUTDOWN) {
            slotOff();
        } else if (rc == RPN::RC_ALPHA) {
            displayOutput(cmd, rpn->alpha);
        } else if (rc == RPN::RC_ALIAS) {
            // TODO : Display ALIAS result
        } else if (rc == RPN::RC_COMMAND) {
            runCommand("./", rpn->out.mid(1), mMainWindow->ui->txtConsole);
        } else if (rc == RPN::RC_PROMPT) {
            lblPrompt->setText(rpn->out + " : ");
            displayStack(rpn->stack);
            displayVars(rpn->vars);
            lblDepthStack->setText(QString::number(rpn->stack.length()));
        } else {
            displayOutput(cmd, rpn->error);
        }
        QTimer::singleShot(0, txtCommand, SLOT(selectAll()));
    }
}

//******************************************************************************
// displayOutput()
//******************************************************************************
void LauncherWindow::displayOutput(QString cmd, QString out) {
    mMainWindow->ui->txtConsole->append(appSettings->get("PROMPT_WINDOW_MODE").toString() + cmd);
    mMainWindow->ui->txtConsole->append(appSettings->get("PROMPT_WINDOW_MODE").toString() + out);
    mMainWindow->ui->txtConsole->append("");
    displayStack(rpn->stack);
    displayVars(rpn->vars);
    lblDepthStack->setText(QString::number(rpn->stack.length()));
    if (mMainWindow->isVisible() == true) {
        showMessage(appSettings->get("PROMPT_WINDOW_MODE").toString() + out, false);
    } else {
        displayOSD(appSettings->get("PROMPT_BAR_MODE").toString() + out);
    }
}

//******************************************************************************
// displayStack()
//******************************************************************************
void LauncherWindow::displayStack(QStack<QVariant> stack) {
    mMainWindow->ui->lstStack->clear();
    for (QVariant i : stack) {
        QListWidgetItem *lwi = new QListWidgetItem();
        lwi->setTextAlignment(Qt::AlignRight);
        if (i.userType() == QMetaType::QStringList) {
            lwi->setText("{ " + i.toStringList().join(" ") + " }");
        } else {
            lwi->setText(i.toString());
        }
        mMainWindow->ui->lstStack->addItem(lwi);
    }
    mMainWindow->ui->lstStack->scrollToBottom();
    lblDepthStack->setText(QString::number(stack.length()));
}

//******************************************************************************v
// displayVars()
//******************************************************************************
void LauncherWindow::displayVars(QMap<QString, QVariant> vars) {
    mMainWindow->ui->lstVars->clear();
    QMapIterator<QString, QVariant> i(vars);
    while (i.hasNext()) {
        i.next();
        QListWidgetItem *lwi = new QListWidgetItem();
        if (i.value().userType() == QMetaType::QStringList) {
            lwi->setText(i.key() + " : " + "{ " + i.value().toStringList().join(" ") + " }");
        } else {
            lwi->setText(i.key() + " : " + i.value().toString());
        }
        mMainWindow->ui->lstVars->addItem(lwi);
    }
}

//******************************************************************************
// displayOSD()
//******************************************************************************
void LauncherWindow::displayOSD(QString out) {
    OSDDialog *osd = new OSDDialog(this, &out);
    osd->setModal(false);
    osd->move(mapToGlobal(this->rect().bottomLeft()));
    txtCommand->setFocus();
    osd->show();
    txtCommand->setFocus();
}

//******************************************************************************
// slotClosed()
//******************************************************************************
void LauncherWindow::slotClosed() {
    close();
}

//******************************************************************************
// slotClock()
//******************************************************************************
void LauncherWindow::slotClock() {
    // TODO : Alarms
    AlarmsDialog dlg = new AlarmsDialog(this);
    int rc = dlg.exec();
    if (rc == QDialog::Accepted) {
        Alarms::alarm a = dlg.a;
        alarms->add(a);
    }
}

//******************************************************************************
// slotPin()
//******************************************************************************
void LauncherWindow::slotPin() {
    if (mMainWindow->isVisible()==true) {
        saveSettings();

        // QScreen *myScreen = QGuiApplication::primaryScreen();
        // QRect screenGeometry = myScreen->availableGeometry();

        // QScreen *pScreen = QGuiApplication::screenAt(this->mapToGlobal({this->width()/2,0}));
        QScreen *pScreen = getActiveScreen(this);
        QRect availableScreenSize = pScreen->availableGeometry();

        // QDesktopWidget widget;
        // QRect screenGeometry = widget.screenGeometry();//.availableGeometry(widget.primaryScreen());
        int height = this->pos().y() + btnPin->pos().y();
        bool topBar = appSettings->get("TITLE_BAR_POSITION_TOP").toBool();
        if (topBar == false) {
            height +=  mMainWindow->height();
        }

        qDebug("Screen Width = %d", availableScreenSize.width());
        qDebug("Screen Left  = %d", availableScreenSize.left());
        int width = availableScreenSize.width() + availableScreenSize.left() - btnPin->width() - 2;
        qDebug("POS          = %d", width);
        // shrink the window as the titlebar only
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
        // pause the media if any
        if (currentTabIndex == 3 && mMainWindow->movieWidget->isVideoAvailable == true) {
            mMainWindow->movieWidget->pauseForce();
        }

        // hide unnecessary controls in the titlebar
        lblPrompt->setVisible(false);
        btnClock->setVisible(false);
        btnOff->setVisible(false);
        btnLock->setVisible(false);
        btnPack->setVisible(false);
        // btnSettings->setVisible(false);
        txtCommand->setVisible(false);
        lblTitle->setVisible(false);
        mMainWindow->setVisible(false);

        // move the remaining button against the edge of the screen
        move(width, height);

    } else {
        this->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
        mMainWindow->setVisible(true);
        btnClock->setVisible(true);
        btnOff->setVisible(true);
        btnLock->setVisible(true);
        btnPack->setVisible(true);
        // btnSettings->setVisible(true);
        txtCommand->setVisible(true);
        lblTitle->setVisible(true);
        lblPrompt->setVisible(true);
        readSettings();
        if (currentTabIndex == 3 && mMainWindow->movieWidget->isVideoAvailable == true) {
            if (mMainWindow->movieWidget->isPlaying == true) {
                mMainWindow->movieWidget->playForce();
            }
        }
    }
}

//******************************************************************************
// slotPack()
//******************************************************************************
void LauncherWindow::slotPack() {
    if (mMainWindow->isVisible()==true) {
        saveSettings();
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
        if (currentTabIndex == 3 && mMainWindow->movieWidget->isVideoAvailable == true) {
            mMainWindow->movieWidget->pauseForce();
        }
        mMainWindow->setVisible(false);
    } else {
        this->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);        
        mMainWindow->setVisible(true);
        readSettings();
        if (currentTabIndex == 3 && mMainWindow->movieWidget->isVideoAvailable == true) {
            if (mMainWindow->movieWidget->isPlaying == true) {
                mMainWindow->movieWidget->playForce();
            }
        }
    }
}

//******************************************************************************
// slotLock()
//******************************************************************************
void LauncherWindow::slotLock() {
    // pause the media if any
    if (currentTabIndex == 3 && mMainWindow->movieWidget->isVideoAvailable == true) {
        mMainWindow->movieWidget->pauseForce();
    }

    QProcess *process = new QProcess(this);
#ifdef Q_OS_LINUX
    QString program = "dbus-send --type=method_call --dest=org.mate.ScreenSaver /org/mate/ScreenSaver org.mate.ScreenSaver.Lock";
    process->start(program);
#endif
#ifdef Q_OS_WIN32
    QString program = "rundll32.exe user32.dll,LockWorkStation";
    process->start(program);
#endif
}

//******************************************************************************
// slotOff()
//******************************************************************************
void LauncherWindow::slotOff() {
    PowerDialog dlg = new PowerDialog(this);
    int rc = dlg.exec();
    if (rc == PowerDialog::LOCK) {
        slotLock();
    } else if (rc == PowerDialog::EXIT) {
        close();
    } else if (rc == PowerDialog::SHUTDOWN) {
        shutdownComputer(dlg.getTimeout(), false);
    } else if (rc == PowerDialog::REBOOT) {
        shutdownComputer(dlg.getTimeout(), true);
    }
}

//******************************************************************************
// slotSettings()
//******************************************************************************
void LauncherWindow::slotSettings() {
    mMainWindow->ui->tabWidget->setCurrentIndex(4);
}

//******************************************************************************
// slotClearConsole()
//******************************************************************************
void LauncherWindow::slotClearConsole() {
    mMainWindow->ui->txtConsole->setText("");
    showMessage("Console cleared");
}

//******************************************************************************
// slotOpenFile()
//******************************************************************************
void LauncherWindow::slotOpenFile() {
    QFileDialog dlg;
    QString homeDir=QDir::homePath () ;
    QString fn = dlg.getOpenFileName(0, tr("Open Log File..."), homeDir, tr("Log files (*)"));
    if (!fn.isEmpty())
    {
        showMessage("Opening " + fn);
        mMainWindow->ui->lblOpenFile->setText(fn);
        mMainWindow->ui->btnCloseFile->setEnabled(true);
        logFile.setFileName(fn);
        logFile.open(QIODevice::ReadOnly|QIODevice::Text);
        logStream.setDevice(&logFile);
        connect(&timerLog, SIGNAL(timeout()), this, SLOT(readLog()));
        logMe = true;
        timerLog.start(100);
    }
}

//******************************************************************************
// readLog()
//******************************************************************************
void LauncherWindow::readLog() {
    while(!logStream.atEnd() && logMe) {
        QString line = logStream.readLine();
        mMainWindow->ui->txtConsole->append(line);
    }
}

//******************************************************************************
// slotCloseFile()
//******************************************************************************
void LauncherWindow::slotCloseFile() {
    mMainWindow->ui->lblOpenFile->setText("");
    showMessage("Closing file");
    mMainWindow->ui->btnCloseFile->setEnabled(false);
    logMe = false;
    logFile.close();
}

//******************************************************************************
// slotKillProcess()
//******************************************************************************
void LauncherWindow::slotKillProcess() {
    mMainWindow->ui->lblPID->setText("");
    showMessage("Stopping process");
    mMainWindow->ui->btnKillProcess->setEnabled(false);
    this->pCmd->kill();
}

//******************************************************************************
// showMessage()
//******************************************************************************
void LauncherWindow::showMessage(QString msg, bool timed) {
    if (timed) {
        mMainWindow->ui->statusbar->showMessage(msg, appSettings->get("MESSAGE_TIMEOUT").toInt());
    } else {
        mMainWindow->ui->statusbar->showMessage(msg, 0);
    }
}

//******************************************************************************
// initLauncher()
//******************************************************************************
void LauncherWindow::initLauncher() {
    //**************************************************************************
    // Set FileSystem Mode for Launcher
    //**************************************************************************
    listModel = new MyFileSystemModel(this, appSettings);
    listModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    mMainWindow->ui->lstLauncher->setModel(listModel);
    mMainWindow->ui->lstLauncher->setRootIndex(listModel->setRootPath(launcherDir));
    mMainWindow->ui->lstLauncher->setDragEnabled(true);
    mMainWindow->ui->lstLauncher->setAcceptDrops(true);
    mMainWindow->ui->lstLauncher->setDragDropMode(QAbstractItemView::DragDrop);

    mMainWindow->ui->lstLauncher->setSelectionMode(QAbstractItemView::SingleSelection);
    mMainWindow->ui->lstLauncher->viewport()->setAcceptDrops(true);
    mMainWindow->ui->lstLauncher->setDropIndicatorShown(true);

    int viewMode = appSettings->get("LAUNCHER_VIEW").toInt();
    if (viewMode == Settings::LAUNCHER_VIEW_ICON) {
        mMainWindow->ui->lstLauncher->setViewMode(QListView::IconMode);
    } else {
        mMainWindow->ui->lstLauncher->setViewMode(QListView::ListMode);
    }

    //**************************************************************************
    // Bind Context Menus for Launcher
    //**************************************************************************
    mMainWindow->ui->lstLauncher->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mMainWindow->ui->lstLauncher, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onContextMenuLauncher(QPoint)));
    connect(mMainWindow->ui->lstLauncher, SIGNAL(clicked(QModelIndex)), SLOT(onLaunchItemClicked(QModelIndex)));

    //**************************************************************************
    // Set Context Menu for Launcher
    //**************************************************************************
    mnuLauncher = new QMenu(mMainWindow->ui->lstLauncher);

    mnuNew = new QMenu(tr("New..."), mMainWindow->ui->lstLauncher);
    // mnuNew->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogOpenButton)));
    mnaNewFolder = new QAction(tr("Folder"), mMainWindow->ui->lstLauncher);
    mnaNewShortcut = new QAction(tr("Shortcut"), mMainWindow->ui->lstLauncher);
    mnuNew->addAction(mnaNewFolder);
    mnuNew->addAction(mnaNewShortcut);

    mnuDisplay = new QMenu(tr("Display..."), mMainWindow->ui->lstLauncher);
    // mnuDisplay->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogOpenButton)));
    mnaDisplayIcon = new QAction(tr("Icon"), mMainWindow->ui->lstLauncher);
    mnaDisplayList = new QAction(tr("List"), mMainWindow->ui->lstLauncher);
    mnuDisplay->addAction(mnaDisplayIcon);
    mnuDisplay->addAction(mnaDisplayList);

    mnuLauncher->addMenu(mnuDisplay);
    mnuLauncher->addMenu(mnuNew);

    //**************************************************************************
    // Set Context Menu for List Items
    //**************************************************************************
    mnuList = new QMenu(mMainWindow->ui->lstLauncher);
    mnaListRename = new QAction(tr("Rename..."), mMainWindow->ui->lstLauncher);
    mnaListDelete = new QAction(tr("Delete..."), mMainWindow->ui->lstLauncher);
    mnaListEdit = new QAction(tr("Edit..."), mMainWindow->ui->lstLauncher);

    mnuList->addAction(mnaListRename);
    mnuList->addAction(mnaListDelete);
    mnuList->addAction(mnaListEdit);
}

//******************************************************************************
// openContextMenuLauncher()
//******************************************************************************
void LauncherWindow::onContextMenuLauncher(QPoint position) {
    if(!((mMainWindow->ui->lstLauncher->selectionModel()->selectedIndexes()).empty()))
    {
        //**********************************************************************
        // Context Menu for Items
        //**********************************************************************
        QAction *action = mnuList->exec(mMainWindow->ui->lstLauncher->viewport()->mapToGlobal(position));
        if (action == mnaListRename) {
            // Action : RENAME
            bool ok;
            QModelIndex index = mMainWindow->ui->lstLauncher->currentIndex();
            QString thisFileName = listModel->fileInfo(index).baseName();
            QString newName = QInputDialog::getText(this, tr("Rename"),
                                                 tr("New name:"), QLineEdit::Normal,
                                                 thisFileName, &ok);
            if (ok && !newName.isEmpty()) {
                QFileInfo fi(thisFileName);
                if (fi.suffix() == appConstants->getQString("LAUNCHER_SUFFIX")) {
                    showMessage("Renaming " + thisFileName);
                    Shortcut *sh = new Shortcut(thisFileName);
                }
                showMessage(newName);
            }
        } else {
            // Action : DELETE
            if (action == mnaListDelete) {
                QModelIndex index = mMainWindow->ui->lstLauncher->currentIndex();
                QString thisFileName = listModel->fileInfo(index).absoluteFilePath();
                QFileInfo fi(thisFileName);
                if (fi.isDir()) {
                    QDir dir(thisFileName);
                    dir.removeRecursively();
                } else {
                    QFile file(thisFileName);
                    file.remove();
                }
                showMessage("Removing " + thisFileName);
            } else {
                // Action : EDIT
                if (action == mnaListEdit) {
                    QModelIndex index = mMainWindow->ui->lstLauncher->currentIndex();
                    QString thisFileName = listModel->fileInfo(index).absoluteFilePath();
                    QFileInfo fi(thisFileName);
                    if (fi.suffix() == appConstants->getQString("LAUNCHER_SUFFIX")) {
                        showMessage("Editing " + thisFileName);
                        Shortcut *sh = new Shortcut(thisFileName);
                        editShortcut(sh);
                    } else {
                        showMessage("Can't edit " + thisFileName);
                    }
                }
            }
        }
        // Clear the selection
        mMainWindow->ui->lstLauncher->selectionModel()->clear();
    } else {
        //**********************************************************************
        // Context Menu for List View
        //**********************************************************************
        QAction *action = mnuLauncher->exec(mMainWindow->ui->lstLauncher->viewport()->mapToGlobal(position));
        if (action == mnaNewShortcut) {
            newShortcut();
        } else {
            if (action == mnaNewFolder) {
                newFolder();
            } else {
                if (action == mnaDisplayIcon) {
                    mMainWindow->ui->lstLauncher->setViewMode(QListView::IconMode);
                    // displayLauncher();
                } else {
                    if (action == mnaDisplayList) {
                        mMainWindow->ui->lstLauncher->setViewMode(QListView::ListMode);
                        // displayLauncher();
                    }
                }
            }
        }
    }
}

//******************************************************************************
// editShortcut()
//******************************************************************************
void LauncherWindow::editShortcut(Shortcut *sh) {
    EditShortcutDialog dlg = EditShortcutDialog(this, sh);
    int rc = dlg.exec();
    if (rc == QDialog::Accepted) {
        updateShortcut(&dlg);
    }
}

//******************************************************************************
// updateShortcut()
//******************************************************************************
void LauncherWindow::updateShortcut(EditShortcutDialog *dlg) {
    // TODO : Double-check & fix shortcut edit
    // TODO : Fix the folder location creation
    QString shortcutName = dlg->getShortcutName();
    QString shortcutCommand = dlg->getShortcutCommand();
    QString shortcutIcon = dlg->getShortcutIcon();
    QString shortcutComment = dlg->getShortcutComment();
    QString shortcutType = dlg->getShortcutType();

    Shortcut *sh = new Shortcut(shortcutName, shortcutIcon, shortcutCommand, shortcutComment, shortcutType);
    sh->write(currentLauncherDir);
}

//******************************************************************************
// newShortcut()
//******************************************************************************
void LauncherWindow::newShortcut() {
    NewShortcutDialog dlg = new NewShortcutDialog(this);
    int rc = dlg.exec();
    if (rc == QDialog::Accepted) {
        createShortcut(&dlg);
    }
}

//******************************************************************************
// createShortcut()
//******************************************************************************
void LauncherWindow::createShortcut(NewShortcutDialog *dlg) {
    QString shortcutName = dlg->getShortcutName();
    QString shortcutCommand = dlg->getShortcutCommand();
    QString shortcutIcon = dlg->getShortcutIcon();
    QString shortcutComment = dlg->getShortcutComment();
    QString shortcutType = dlg->getShortcutType();

    Shortcut *sh = new Shortcut(shortcutName, shortcutIcon, shortcutCommand, shortcutComment, shortcutType);
    sh->write(currentLauncherDir);
}

//******************************************************************************
// newFolder()
//******************************************************************************
void LauncherWindow::newFolder() {
    NewFolderDialog dlg = new NewFolderDialog(this);
    int rc = dlg.exec();
    if (rc == QDialog::Accepted) {
        QString folderName = dlg.getFolderName();
        QString folderDir = QDir(currentLauncherDir).filePath(folderName);
        if (!QDir(folderDir).exists()) {
            QDir().mkdir(folderDir);
        }
    }
}

//******************************************************************************
// shutdownComputer()
//******************************************************************************
void LauncherWindow::shutdownComputer(int timeout=1, bool reboot=false) {
#ifdef Q_OS_LINUX
    // In order to perform shutdown options without running them as sudo,
    // please key in these two following lines :
    // sudo visudo
    // jpl ALL=(ALL) NOPASSWD: /sbin/poweroff, /sbin/reboot, /sbin/shutdown
    if (reboot == true) {
        if (timeout == 0) {
            showMessage("Rebooting the computer now");
            QStringList args = {"-r", "now"};
            QProcess::startDetached("shutdown", args);
        } else {
            showMessage(QString("Rebooting the computer in %1 minute(s)").arg(timeout));
            QStringList args = {"-r", QString::number(timeout)};
            QProcess::startDetached("shutdown", args);
        }
    } else {
        if (timeout == 0) {
            showMessage("Shutting down the computer now");
            QStringList args = {"now"};
            QProcess::startDetached("shutdown", args);
        } else {
            showMessage(QString("Shutting down the computer in %1 minute(s)").arg(timeout));
            QStringList args = {QString::number(timeout)};
            QProcess::startDetached("shutdown", args);
        }
    }
#endif
    // TODO : Test Windows reboot & shutdown
#ifdef Q_OS_WIN32
    if (reboot == true) {
        if (timeout == 0) {
            showMessage("Rebooting the computer now");
            QStringList args = {"/r", "/t", "0"};
            QProcess::startDetached("shutdown", args);
        } else {
            showMessage(QString("Rebooting the computer in %1 minute(s)").arg(timeout));
            QStringList args = {"/r", "/t", QString::number(60 * timeout)};
            QProcess::startDetached("shutdown", args);
        }
    } else {
        if (timeout == 0) {
            showMessage("Shutting down the computer now");
            QStringList args = {"/s", "/t", "0"};
            QProcess::startDetached("shutdown", args);
        } else {
            showMessage(QString("Shutting down the computer in %1 minute(s)").arg(timeout));
            QStringList args = {"/s", "/t", QString::number(60 * timeout)};
            QProcess::startDetached("shutdown", args);
        }
    }
    // QProcess::startDetached("shutdown -s -f -t 00");
#endif
}

//******************************************************************************
// onLaunchItemClicked()
//******************************************************************************
void LauncherWindow::onLaunchItemClicked(QModelIndex index)
{
    currentLauncherDir = listModel->fileInfo(index).absoluteFilePath();
    if (listModel->fileInfo(index).isDir()) {
        showMessage(currentLauncherDir);
        if (currentLauncherDir == launcherDir) {
            listModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        } else {
            listModel->setFilter(QDir::AllEntries | QDir::NoDot);
        }
    } else {        
        QFileInfo fi(currentLauncherDir);
        if (fi.suffix() == appConstants->getQString("LAUNCHER_SUFFIX")) {
            Shortcut *sh = new Shortcut(currentLauncherDir);
            showMessage("Running " + sh->exec);
            sh->launch();
        } else {
#ifdef Q_OS_LINUX
            showMessage("Opening " + currentLauncherDir);
            QProcess::startDetached("xdg-open", QStringList(currentLauncherDir));
#endif
            // TODO : start WIN32
#ifdef Q_OS_WIN32
            // TODO : start
#endif
        }
    }
    mMainWindow->ui->lstLauncher->setRootIndex(listModel->setRootPath(currentLauncherDir));
}

//******************************************************************************
// alarmCheck()
//******************************************************************************
void LauncherWindow::alarmCheck() {
    // TODO : check alarms
    // qDebug() << "Check Alarm !";
    QList<Alarms::alarm> *alarms;
    Alarms ac;
    alarms = ac.getAlarms();
    for(int i = 0; i < alarms->count(); ++i )
    {
        if(ac.isEnabled(alarms->at(i))) {
            // qDebug() << alarms->at(i).message;
            QDateTime RightNow = QDateTime::currentDateTime();
            bool soundAlarm = false;

            if (ac.getHour(alarms->at(i)) == RightNow.time().hour() && ac.getMinute(alarms->at(i)) == RightNow.time().minute()) {
                switch(RightNow.date().dayOfWeek()) {
                    //WeekDay Alarms
                    case 1:
                    if(ac.isMonEnabled(alarms->at(i))) {
                        soundAlarm = true;
                    }
                    break;

                    case 2:
                    if(ac.isTueEnabled(alarms->at(i))) {
                        soundAlarm = true;
                    }
                    break;

                    case 3:
                    if(ac.isWedEnabled(alarms->at(i))) {
                        soundAlarm = true;
                    }
                    break;

                    case 4:
                    if(ac.isThuEnabled(alarms->at(i))) {
                        soundAlarm = true;
                    }
                    break;

                    case 5:
                    if(ac.isFriEnabled(alarms->at(i))) {
                        soundAlarm = true;
                    }
                    break;

                    case 6:
                    if(ac.isSatEnabled(alarms->at(i))) {
                        soundAlarm = true;
                    }
                    break;

                    case 7:
                    if(ac.isSunEnabled(alarms->at(i))) {
                        soundAlarm = true;
                    }
                    break;
                }
            }

            if(soundAlarm) {
                QSound::play(ac.getSound(alarms->at(i)));
                displayOutput("alarm", ac.getMessage(alarms->at(i)));
            }
        }
    }
}

//******************************************************************************
// getActiveScreen()
//******************************************************************************
QScreen *LauncherWindow::getActiveScreen(QWidget* pWidget) const
{
    QScreen* pActive = nullptr;
    while (pWidget) {
        auto w = pWidget->windowHandle();
        if (w != nullptr) {
            pActive = w->screen();
            break;
        }
        else
            pWidget = pWidget->parentWidget();
    }
    return pActive;
}

//******************************************************************************
// runCommand()
//******************************************************************************
void LauncherWindow::runCommand(QString dir, QString cmd, QTextEdit *view) {
    auto process = new QProcess;
    process->setWorkingDirectory(dir);

    // Slot Read STDIN
    QObject::connect(process, &QProcess::readyReadStandardOutput, [process,view]() {
        auto output=process->readAllStandardOutput();
        view->append(output.trimmed());
    });

    // Slot Read STDERR
    QObject::connect(process, &QProcess::readyReadStandardError, [process,view]() {
        auto output=process->readAllStandardError();
        view->append(output.trimmed());
    });

    // Slot FINISHED
    QObject::connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), [this]() {
        this->mMainWindow->ui->txtConsole->append("Process finished");
        this->mMainWindow->ui->lblPID->setText("");
        this->mMainWindow->ui->btnKillProcess->setEnabled(false);
    });

#ifdef Q_OS_LINUX
    process->start(cmd);
#endif
#ifdef Q_OS_WIN32
    cmd = "/c " + cmd;
    QStringList cmdList= cmd.split(" ", QString::SkipEmptyParts);
    process->start("cmd", cmdList);
#endif

    process->setProcessChannelMode(QProcess::MergedChannels);
    process->waitForStarted();
    qDebug() << process->error();
    this->pCmd = process;
    mMainWindow->ui->lblPID->setText("PID:" + QString::number(this->pCmd->processId()));
    mMainWindow->ui->btnKillProcess->setEnabled(true);
    view->show();
}

//******************************************************************************
// MainWindow()
//******************************************************************************
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint)
    , ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_QuitOnClose, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    statusBar()->setSizeGripEnabled(true);
    ui->txtNotepad->setText("Hello");
    movieWidget = new MovieWidget(this);
    ui->tabVideoSplitter->addWidget(movieWidget);
}

//******************************************************************************
// ~MainWindow()
//******************************************************************************
MainWindow::~MainWindow()
{
    delete ui;
}
