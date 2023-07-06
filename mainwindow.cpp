 #include "mainwindow.h"

//******************************************************************************
// MainWindow()
//******************************************************************************
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    app = new App();
    dirtyFlag = false;
    iCommands = 0;

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(slotDoExit()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(slotDoOpenFile()));
    connect(ui->cbxCommands,SIGNAL(currentIndexChanged(const QString)),this,SLOT(slotSelectCommand(const QString)));
    connect(ui->btnEditXML, SIGNAL(clicked()), this, SLOT(slotDoEditXML()));
    connect(ui->txtEditXML, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPosition()));
    connect(ui->btnSaveXML, SIGNAL(clicked()), this, SLOT(slotDoSaveXML()));
    connect(ui->btnImportXML, SIGNAL(clicked()), this, SLOT(slotDoImportXML()));
    connect(ui->btnExportXML, SIGNAL(clicked()), this, SLOT(slotDoExportXML()));
    connect(ui->btnBreak, SIGNAL(clicked()), this, SLOT(slotBreakCommand()));
    connect(ui->btnClearConsole, SIGNAL(clicked()), this, SLOT(slotClearConsole()));
    connect(ui->btnCopyConsole, SIGNAL(clicked()), this, SLOT(slotCopyConsole()));
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F3), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(slotRunCommand()));
    connect(ui->btnEnter, SIGNAL(clicked()), this, SLOT(slotRunCommand()));
    ui->txtCommand->installEventFilter(this);

    readSettings();
    if (openXMLFile()) {
        populateCommandsList();
        showMessage(QString("Welcome to %1").arg(app->appConstants->getQString("APPLICATION_NAME")));
    } else {
        showMessage("No XML file found", 0);
    }
    initUI();
    QString appTitle = QString("%1 %2").arg(app->appConstants->getQString("APPLICATION_NAME"), app->appConstants->getQString("VERSION"));
    setWindowTitle(appTitle);

    QTimer::singleShot(500, this, &MainWindow::setDelayed);
    getEnvironment();
 }

//******************************************************************************
// ~MainWindow()
//******************************************************************************
MainWindow::~MainWindow() {
    delete ui;
    // delete xp;
    delete cUI;
}

//******************************************************************************
// setDelayed()
//******************************************************************************
void MainWindow::setDelayed() {
    connect(ui->txtEditXML, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

//******************************************************************************
// initUI()
//******************************************************************************
void MainWindow::initUI() {
    //**************************************************************************
    // Commands Editor's settings
    //**************************************************************************
    QFont font;
    font.setFamily("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);

    struct TextFormat textFormats[5] = {
        {Qt::blue, QFont::Bold, false},
        {Qt::darkMagenta, QFont::Bold, false},
        {Qt::darkBlue, QFont::Bold, true},
        {Qt::darkRed, QFont::Normal, false},
        {Qt::gray, QFont::Normal, false}
    };

    textFormats[0].foreground = app->appSettings->get("XML_KEYWORD_COLOR").value<QColor>();
    textFormats[0].weight = app->appSettings->get("XML_KEYWORD_WEIGHT").toInt();
    textFormats[0].italic = app->appSettings->get("XML_KEYWORD_ITALIC").toBool();
    textFormats[1].foreground = app->appSettings->get("XML_ELEMENT_COLOR").value<QColor>();
    textFormats[1].weight = app->appSettings->get("XML_ELEMENT_WEIGHT").toInt();
    textFormats[1].italic = app->appSettings->get("XML_ELEMENT_ITALIC").toBool();
    textFormats[2].foreground = app->appSettings->get("XML_ATTRIBUTE_COLOR").value<QColor>();
    textFormats[2].weight = app->appSettings->get("XML_ATTRIBUTE_WEIGHT").toInt();
    textFormats[2].italic = app->appSettings->get("XML_ATTRIBUTE_ITALIC").toBool();
    textFormats[3].foreground = app->appSettings->get("XML_VALUE_COLOR").value<QColor>();
    textFormats[3].weight = app->appSettings->get("XML_VALUE_WEIGHT").toInt();
    textFormats[3].italic = app->appSettings->get("XML_VALUE_ITALIC").toBool();
    textFormats[4].foreground = app->appSettings->get("XML_COMMENT_COLOR").value<QColor>();
    textFormats[4].weight = app->appSettings->get("XML_KEYWORD_WEIGHT").toInt();
    textFormats[4].italic = app->appSettings->get("XML_COMMENT_WEIGHT").toBool();

    ui->txtEditXML->setFont(font);
    ui->txtConsoleOut->setStyleSheet("background-color: " + this->app->appSettings->get("CONSOLE_BACKGROUND").value<QColor>().name() + ";");

    const int tabStop = app->appSettings->get("XML_TAB_STOP").toInt();

    QFontMetrics metrics(font);
    ui->txtEditXML->setTabStopWidth(tabStop * metrics.width(' '));
    highlighter = new BasicXMLSyntaxHighlighter(ui->txtEditXML->document());
    highlighter->setFormats(textFormats);
    Q_UNUSED(highlighter);
    ui->txtEditXML->setReadOnly(true);
    ui->txtEditXML->show();

    ui->lblDirtyFlag->setText("locked");
    ui->btnSaveXML->setEnabled(false);

    //**************************************************************************
    // Settings Form
    //**************************************************************************
    app->appSettings->form(ui->boxSettings);

    //**************************************************************************
    // StatusBar
    //**************************************************************************
    lblRC = new QLabel("RC=0", this);
    lblRC->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lblRC->setObjectName("lblRC");
    lblRC->setFont(QFont("Courier"));
    lblRC->setToolTip("Return code for previous process");
    ui->statusBar->addPermanentWidget(lblRC, 0);

    lblPID = new QLabel("                    ", this);
    lblPID->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lblPID->setObjectName("lblPID");
    lblPID->setFont(QFont("Courier"));
    lblPID->setToolTip("PID for running process");
    ui->statusBar->addPermanentWidget(lblPID, 0);

    lblTimeElpased = new QLabel("------ ms", this);
    lblTimeElpased->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lblTimeElpased->setObjectName("lblTimeElpased");
    lblTimeElpased->setFont(QFont("Courier"));
    lblTimeElpased->setToolTip("Elapsed time in milliseconds");
    ui->statusBar->addPermanentWidget(lblTimeElpased, 0);

    lblLED = new QLabel(this);
    lblLED->setPixmap(QPixmap(":/led_green.png"));
    lblLED->setObjectName("lblLED");
    ui->statusBar->addPermanentWidget(lblLED, 0);

    //**************************************************************************
    // About tab
    //**************************************************************************
    QFile file(":/help.html");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    QString html = app->appConstants->aboutText;
    html += stream.readAll();
    ui->txtAbout->setHtml(html);
}


//******************************************************************************
// selectCommand()
//******************************************************************************
void MainWindow::slotSelectCommand(const QString cmd) {
    // buildCommandScreen(cmd);
    // cUI.CommandsUI(os.path.join(self.appDir, const.COMMANDS_FILE), self.cbxCommands.currentText(), self)
    cUI = new CommandUI(fName, cmd, ui, app);
}

//******************************************************************************
// showMessage()
//******************************************************************************
void MainWindow::showMessage(const QString &message, int timeout) {
    if (timeout == -1) {
        timeout = app->appSettings->get("APPLICATION_STATUSBAR_TIMEOUT").toInt();
    }
    ui->statusBar->showMessage(message, timeout);
}

//******************************************************************************
// slotDoExit()
//******************************************************************************
void MainWindow::slotDoExit() {
    if (xp)
        xp->killMe();
    this->close();
}

//******************************************************************************
// slotDoEditXML()
//******************************************************************************
void MainWindow::slotDoEditXML() {
    if (ui->txtEditXML->isReadOnly()) {
        ui->lblDirtyFlag->setText("editing");
        ui->txtEditXML->setReadOnly(false);
        ui->txtEditXML->show();
        showMessage("Editing XML");
    } else {
        ui->lblDirtyFlag->setText("locked");
        ui->txtEditXML->setReadOnly(true);
        ui->txtEditXML->show();
        showMessage("Locking XML");
    }
}

//******************************************************************************
// slotCursorPosition()
//******************************************************************************
void MainWindow::slotCursorPosition() {
    int line(ui->txtEditXML->textCursor().blockNumber() + 1);
    int col(ui->txtEditXML->textCursor().columnNumber() + 1);
    ui->lblColumnCursor->setText(QString::number(col));
    ui->lblLineCursor->setText(QString::number(line));
}

//******************************************************************************
// slotTextChanged()
//******************************************************************************
void MainWindow::slotTextChanged() {
    dirtyFlag = true;
    ui->lblDirtyFlag->setText("*modified*");
    ui->btnSaveXML->setEnabled(true);
}

//******************************************************************************
// slotDoSaveXML()
//******************************************************************************
void MainWindow::slotDoSaveXML() {
    QFile f(this->fName);
    if (f.open(QFile::Text | QFile::WriteOnly | QFile::Truncate)) {
        QTextStream stream(&f);
        qDebug() << ui->txtEditXML->document()->toPlainText();
        stream << ui->txtEditXML->document()->toPlainText() << Qt::endl;
        f.close();
        xmlCommands.setContent(&f);
        populateCommandsList();
        dirtyFlag = false;
        ui->lblDirtyFlag->setText("*saved*");
        ui->btnSaveXML->setEnabled(false);
        showMessage("XML file saved");
    } else {
        showMessage("Can't save XML file");
    }
}

//******************************************************************************
// mergeXML()
//******************************************************************************
void MainWindow::mergeXML(QString f1, QString f2) {
    QDomDocument doc1, doc2;
    QFile file1(f1);
    QFile file2(f2);

    if (file1.open(QIODevice::ReadOnly) && file2.open(QIODevice::ReadOnly)) {
        if (doc1.setContent(&file1)) {
            if (doc2.setContent(&file2)) {
                // XML data loaded successfully
                QDomElement root1 = doc1.documentElement();
                QDomElement root2 = doc2.documentElement();
                QDomNodeList nodes = root2.childNodes();

                for (int i = 0; i < nodes.count(); i++) {
                    QDomNode node = nodes.at(i);
                    QDomNode importedNode = doc1.importNode(node, true);
                    root1.appendChild(importedNode);
                }

                QDateTime now = QDateTime::currentDateTime();
                QString timestamp = now.toString(QLatin1String("yyyyMMdd-hhmmss"));
                QString copyfile = QString("%1.%2").arg(this->fName).arg(timestamp);
                QFile::copy(this->fName, copyfile);

                QFile outputFile(this->fName);

                if (outputFile.open(QFile::Text | QFile::WriteOnly | QFile::Truncate)) {
                    QTextStream stream(&outputFile);
                    doc1.save(stream, 4); // Use indentation of 4 spaces
                    outputFile.close();
                    // xmlCommands.setContent(&outputFile);
                    openXMLFile();
                    populateCommandsList();
                    dirtyFlag = false;
                    ui->lblDirtyFlag->setText("*saved*");
                    ui->btnSaveXML->setEnabled(false);
                } else {
                    // Error opening output file
                    qDebug() << "Error opening output file";
                    showMessage(QString("Error opening output file"));
                }
            } else {
                qDebug() << "Error loading XML #2 content";
                showMessage(QString("Error loading XML #2 content from %1").arg(f2));
            }
        } else {
            // Error loading XML content
            qDebug() << "Error loading XML #1 content";
            showMessage(QString("Error loading XML #1 content from %1").arg(f1));
        }
        file1.close();
        file2.close();
    } else {
        // Error opening files
        qDebug() << "Error opening files";
        showMessage(QString("Error opening files %1 and/or %2").arg(f1).arg(f2));
    }
}

//******************************************************************************
// slotDoImportXML()
//******************************************************************************
void MainWindow::slotDoImportXML() {
    QString source = QFileDialog::getOpenFileName(this, tr("Import File"), QDir::homePath(), tr("XML File (*.xml)"));
    if (!source.isEmpty()) {
        mergeXML(source, this->fName);
        showMessage(QString("XML file %1 imported into current XML file").arg(source));
    } else {
        showMessage("XML file not imported");
    }
}

//******************************************************************************
// slotDoExportXML()
//******************************************************************************
void MainWindow::slotDoExportXML() {
    QString target = QFileDialog::getSaveFileName(this, tr("Export File"),QDir::homePath(),tr("XML File (*.xml)"));
    if (!target.isEmpty()) {
        QFile::copy(this->fName, target);
        showMessage(QString("XML file exported to %1").arg(target));
    } else {
        showMessage("XML file not exported");
    }
}

//******************************************************************************
// slotRunCommand()
//******************************************************************************
void MainWindow::slotRunCommand() {
    qDebug() << "RUNME";
    QString cmd = this->ui->txtCommand->text();
    if (cmd.startsWith("!")) {
        if (cmd.toUpper() == "!BREAK") {
            slotBreakCommand();
        }
        if (cmd.toUpper() == "!QUIT") {
            this->close();
        }
        if (cmd.toUpper() == "!CLS") {
            slotClearConsole();
        }
    } else {
        if (!app->processRunning) {
            qDebug() << "NEW PROCESS";
            if (this->ui->chkClearConsole->isChecked()) {
                this->ui->txtConsoleOut->setText("");
            }
            this->ui->lblTitle->setText(cmd);
            QStringList args = cmd.split(" ");
            QString pgm = args.first();
            args.removeFirst();
            xp = new XeqProcess(pgm, args, app, ui);
            showMessage(QString("Process %1 launched").arg(xp->PID));
        } else {
            qDebug() << "SEND TEXT";
            xp->mProcess.write(cmd.toLatin1());
        }
            // TODO : Send text to process if process is already running => xp->mProcess.write(...);
    }
    this->ui->txtCommand->selectAll();
    this->aCommands.append(cmd);
    this->iCommands = this->aCommands.count() - 1;
    this->ui->tabWidget->setCurrentIndex(0);
    this->ui->txtCommand->setFocus();
}

//******************************************************************************
// slotBreakCommand()
//******************************************************************************
void MainWindow::slotBreakCommand() {
    if (xp) {
        if (xp->mProcess.state() != QProcess::NotRunning) {
            showMessage(QString("Process %1 killed").arg(xp->PID));
            xp->killMe();
        } else {
            showMessage("Nothing is running");
        }
    } else {
        showMessage("Nothing is running");
    }
}

//******************************************************************************
// slotClearConsole()
//******************************************************************************
void MainWindow::slotClearConsole() {
    this->ui->txtConsoleOut->setText("");
    if (this->app->processRunning == false) {
        this->ui->lblTitle->setText("");

        QLabel *lblRC = this->ui->statusBar->findChild<QLabel*>("lblRC");
        lblRC->setText("RC=0");

        QLabel *lblLED = this->ui->statusBar->findChild<QLabel*>("lblLED");
        lblLED->setPixmap(QPixmap(":/led_green.png"));

        QLabel *lblTimeElpased = this->ui->statusBar->findChild<QLabel*>("lblTimeElpased");
        lblTimeElpased->setText("000000 ms");
    }
    showMessage("Console's output cleared");
}

//******************************************************************************
// slotCopyConsole()
//******************************************************************************
void MainWindow::slotCopyConsole() {
    QClipboard *cp = QGuiApplication::clipboard();
    cp->setText(this->ui->txtConsoleOut->toPlainText());
    showMessage("Console's output copied into clipboard");
}

//******************************************************************************
// eventFilter()
//******************************************************************************
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->txtCommand) {
        if (event->type() == QKeyEvent::KeyPress) {
            QKeyEvent * ke = static_cast<QKeyEvent*>(event);
            if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
                slotRunCommand();
                return true; // do not process this event further
            }
            if (ke->key() == Qt::Key_Down) {
                if (!aCommands.isEmpty()) {
                    if (this->iCommands > 0) {
                        this->iCommands--;
                        this->ui->txtCommand->setText(this->aCommands.at(this->iCommands));
                    } else {
                        this->iCommands = this->aCommands.count() - 1;
                        this->ui->txtCommand->setText(this->aCommands.at(this->iCommands));
                    }
                }
                return true; // do not process this event further
            }
            if (ke->key() == Qt::Key_Up) {
                if (!aCommands.isEmpty()) {
                    if (this->iCommands < this->aCommands.count() - 1) {
                        this->iCommands++;
                        this->ui->txtCommand->setText(this->aCommands.at(this->iCommands));
                    } else {
                        this->iCommands = 0;
                        this->ui->txtCommand->setText(this->aCommands.at(this->iCommands));
                    }
                }
                return true; // do not process this event further
            }
        }
        return false; // process this event further
    } else {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(watched, event);
    }
}

//******************************************************************************
// closeEvent()
//******************************************************************************
void MainWindow::closeEvent(QCloseEvent *event) {
    if (app->appSettings->get("APPLICATION_CONFIRM_EXIT").toBool() == true) {
        QMessageBox::StandardButton rc;
        rc = QMessageBox::question(this, app->appConstants->getQString("APPLICATION_NAME"), QString("Really quit ?\n"), QMessageBox::Yes|QMessageBox::No);
        if (rc == QMessageBox::Yes) {
            saveSettings();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        saveSettings();
        event->accept();
    }
}

//******************************************************************************
// saveSettings()
//******************************************************************************
void MainWindow::saveSettings() {
    //**************************************************************************
    // Application state saving
    //**************************************************************************
    QSettings registry(app->appConstants->getQString("ORGANIZATION_NAME"), app->appConstants->getQString("APPLICATION_NAME"));
    registry.setValue("geometry", saveGeometry());
    registry.setValue("windowState", saveState());
    registry.setValue("splitter", ui->splitter->saveState());
    registry.setValue("tab", ui->tabWidget->currentIndex());
    registry.setValue("clearConsole", ui->chkClearConsole->isChecked());
    registry.setValue("sendNotification", ui->chkNotification->isChecked());

    //**************************************************************************
    // History saving
    //**************************************************************************
    QListIterator<QString> itCommands(aCommands);
    int jCommands(0);
    registry.beginWriteArray("History");
    while (itCommands.hasNext()) {
      QString project = itCommands.next();
      registry.setArrayIndex(jCommands++);
      registry.setValue("History", project);
    }
    registry.endArray();

    //**************************************************************************
    // Settings saving
    //**************************************************************************
    Settings mySettings;
    mySettings.write();
}

//******************************************************************************
// readSettings()
//******************************************************************************
void MainWindow::readSettings() {
    QSettings registry(app->appConstants->getQString("ORGANIZATION_NAME"), app->appConstants->getQString("APPLICATION_NAME"));

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

    const QByteArray splitter = registry.value("splitter", QByteArray()).toByteArray();
    if (!splitter.isEmpty()) {
        ui->splitter->restoreState(registry.value("splitter").toByteArray());
    }

    const int tabIndex = registry.value("tab", 0).toInt();
    ui->tabWidget->setCurrentIndex(tabIndex);

    const int clearConsole = registry.value("clearConsole", false).toBool();
    ui->chkClearConsole->setChecked(clearConsole);

    const int sendNotification = registry.value("sendNotification", false).toBool();
    ui->chkNotification->setChecked(sendNotification);

    //**************************************************************************
    // History restoring
    //**************************************************************************
    int sizeHistory = registry.beginReadArray("History");
    for (int i = 0; i < sizeHistory; ++i) {
        registry.setArrayIndex(i);
        aCommands.append(registry.value("History").toString());
    }
    registry.endArray();
}

//******************************************************************************
// populateCommandsList()
//******************************************************************************
void MainWindow::populateCommandsList() {
    ui->cbxCommands->clear();
    QDomElement root = xmlCommands.documentElement();
    QDomElement Command = root.firstChild().toElement();

    while (!Command.isNull()) {
        if (Command.tagName() == "command") {
            QString Title = Command.attribute("title", "No Title");
            ui->cbxCommands->addItem(Title);
        }
        Command = Command.nextSibling().toElement();
    }
}

//******************************************************************************
// buildCommandScreen()
//******************************************************************************
void MainWindow::buildCommandScreen(QString cmd) {
    qDebug() << cmd;
    bool found(false);
    QDomElement root = xmlCommands.documentElement();
    QDomElement Command = root.firstChild().toElement();

    while (!Command.isNull()) {
        if (Command.tagName() == "command") {
            QString Title = Command.attribute("title", "No Title");
            if (Title == cmd) {
                found = true;
                QDomElement Child=Command.firstChild().toElement();
                QString Name;
                double Value;

                // Read each child of the component node
                while (!Child.isNull()) {
                    // Read Name and value
                    if (Child.tagName()=="NAME") Name=Child.firstChild().toText().data();
                    if (Child.tagName()=="VALUE") Value=Child.firstChild().toText().data().toDouble();

                    // Next child
                    Child = Child.nextSibling().toElement();
                }

                // Display component data
                // std::cout << "Component " << ID.toStdString().c_str() << std::endl;
                std::cout << "   Name  = " << Name.toStdString().c_str() << std::endl;
                std::cout << "   Value = " << Value << std::endl;
                std::cout << std::endl;
            }
        }
        Command = Command.nextSibling().toElement();
    }

    if (!found) {
        showMessage("Command not found");
    }
}

//******************************************************************************
// slotDoOpenFile()
//******************************************************************************
void MainWindow::slotDoOpenFile() {
    QString source = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("XML File (*.xml)"));
    if (!source.isEmpty()) {
        openXMLFile(source);
        showMessage(QString("Opening XML file %1").arg(source));
    } else {
        showMessage("No XML file specified");
    }
}

//******************************************************************************
// openXMLFile()
//******************************************************************************
bool MainWindow::openXMLFile(QString fName) {
    // TODO : Manage "open file" option from main menu
    bool rc(false);
    ui->txtEditXML->blockSignals(true);
    if (fName == "*DEFAULT") {
        fName = Utils::pathAppend(app->appDir, QString::fromUtf8(app->appConstants->getString("COMMANDS_FILE").c_str()));
    }
    this->fName = fName;
    QFile f(fName);
    if (f.open(QIODevice::Text | QIODevice::ReadOnly )) {
        xmlCommands.setContent(&f);

        QString content;
        f.seek(0);
        while(!f.atEnd())
            content.append(f.readLine());
        ui->txtEditXML->setPlainText(content);

        f.close();
        rc = true;
    } else {
        // Manage the case where there is no crush.xml file, copy the default one from the resource file
        QMessageBox::StandardButton msg;
        msg = QMessageBox::question(this, app->appConstants->getQString("APPLICATION_NAME"), QString("XML file not found.\nOpening the default one ?\n"), QMessageBox::Yes|QMessageBox::No);
        if (msg == QMessageBox::Yes) {
            QString fName = Utils::pathAppend(app->appDir, QString::fromUtf8(app->appConstants->getString("COMMANDS_FILE").c_str()));
#ifdef Q_OS_LINUX
            QFile::copy(":/crush_lnx.xml", fName);
#else
            QFile::copy(":/crush_win.xml", fName);
#endif
            this->fName = fName;
            QFile f(fName);
            if (f.open(QIODevice::Text | QIODevice::ReadOnly )) {
                xmlCommands.setContent(&f);

                QString content;
                f.seek(0);
                while(!f.atEnd())
                    content.append(f.readLine());
                ui->txtEditXML->setPlainText(content);

                f.close();
                rc = true;
            }
        }
    }
    ui->txtEditXML->blockSignals(false);
    return rc;
}

//******************************************************************************
// getEnvironment()
//******************************************************************************
void MainWindow::getEnvironment() {
    QProcessEnvironment qpe  = QProcessEnvironment::systemEnvironment();
    QFormLayout *form = new QFormLayout(ui->boxEnvironment);

    QStringList keys = qpe.keys();
    foreach (const QString& varName, keys) {
        QLabel* label = new QLabel(varName);
        QLineEdit* lineEdit = new QLineEdit(qpe.value(varName));
        lineEdit->setReadOnly(true);
        form->addRow(label, lineEdit);
    }
    ui->boxEnvironment->setLayout(form);
}

/*
#-------------------------------------------------------------------------------
# commandsClicked()
#-------------------------------------------------------------------------------
    def commandsClicked(self, txt):
        self.resetCommand()
        self.statusBar.showMessage(self.cbxCommands.currentText(), settings.dbSettings['TIMER_STATUS'])
        self.myCommandsUI = cUI.CommandsUI(os.path.join(self.appDir, const.COMMANDS_FILE), self.cbxCommands.currentText(), self)
*/
