 #include "mainwindow.h"

//******************************************************************************
// MainWindow()
//******************************************************************************
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    app = new App();
    dirtyFlag = false;

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(slotDoExit()));
    connect(ui->cbxCommands,SIGNAL(currentIndexChanged(const QString)),this,SLOT(slotSelectCommand(const QString)));
    connect(ui->btnEditXML, SIGNAL(clicked()), this, SLOT(slotDoEditXML()));
    connect(ui->txtEditXML, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPosition()));
    connect(ui->btnSaveXML, SIGNAL(clicked()), this, SLOT(slotDoSaveXML()));

    readSettings();
    if (openXMLFile()) {
        populateCommandsList();
        showMessage("Welcome to " + app->appConstants->getQString("APPLICATION_NAME"));
    } else {
        showMessage("No XML file found", 0);
    }
    initUI();
    QString appTitle = QString("%1 %2").arg(app->appConstants->getQString("APPLICATION_NAME"), app->appConstants->getQString("VERSION"));
    setWindowTitle(appTitle);

    QTimer::singleShot(500, this, &MainWindow::setDelayed);
}

//******************************************************************************
// ~MainWindow()
//******************************************************************************
MainWindow::~MainWindow() {
    delete ui;
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

    // QTextEdit* editor = new QTextEdit();
    ui->txtEditXML->setFont(font);

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

    app->appSettings->form(ui->boxSettings);
}


//******************************************************************************
// selectCommand()
//******************************************************************************
void MainWindow::slotSelectCommand(const QString cmd) {
    // buildCommandScreen(cmd);
    // cUI.CommandsUI(os.path.join(self.appDir, const.COMMANDS_FILE), self.cbxCommands.currentText(), self)
    CommandUI *cUI = new CommandUI(fName, cmd, ui, app);
}

//******************************************************************************
// showMessage()
//******************************************************************************
void MainWindow::showMessage(const QString &message, int timeout) {
    if (timeout == -1) {
        timeout = app->appSettings->get("STATUSBAR_TIMEOUT").toInt();
    }
    ui->statusBar->showMessage(message, timeout);
}

//******************************************************************************
// slotDoExit()
//******************************************************************************
void MainWindow::slotDoExit() {
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
    if (f.open(QIODevice::Text | QIODevice::ReadWrite )) {
        QTextStream stream(&f);
        stream << ui->txtEditXML->document()->toPlainText() << endl;
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
// closeEvent()
//******************************************************************************
void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton rc;
    rc = QMessageBox::question(this, app->appConstants->getQString("APPLICATION_NAME"), QString("Really quit ?\n"), QMessageBox::Yes|QMessageBox::No);
    if (rc == QMessageBox::Yes) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
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
// openXMLFile()
//******************************************************************************
bool MainWindow::openXMLFile(QString fName) {
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
    }
    ui->txtEditXML->blockSignals(false);
    return rc;
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
