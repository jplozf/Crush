 #include "mainwindow.h"

//******************************************************************************
// MainWindow()
//******************************************************************************
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    app = new App();

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(slotDoExit()));
    connect(ui->cbxCommands,SIGNAL(currentIndexChanged(const QString)),this,SLOT(slotSelectCommand(const QString)));

    readSettings();
    if (openXMLFile()) {
        populateCommandsList();
        showMessage("Welcome to " + app->appConstants->getQString("APPLICATION_NAME"));
    } else {
        showMessage("No XML file found", 0);
    }
    QString appTitle = QString("%1 %2").arg(app->appConstants->getQString("APPLICATION_NAME"), app->appConstants->getQString("VERSION"));
    setWindowTitle(appTitle);
}

//******************************************************************************
// ~MainWindow()
//******************************************************************************
MainWindow::~MainWindow() {
    delete ui;
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
    if (fName == "*DEFAULT") {
        fName = Utils::pathAppend(app->appDir, QString::fromUtf8(app->appConstants->getString("COMMANDS_FILE").c_str()));
    }
    this->fName = fName;
    QFile f(fName);
    if (f.open(QIODevice::ReadOnly )) {
        xmlCommands.setContent(&f);
        f.close();
        rc = true;
    }
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
