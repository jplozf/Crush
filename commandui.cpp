#include "commandui.h"
#include "ui_mainwindow.h"

//******************************************************************************
// CommandUI()
//******************************************************************************
CommandUI::CommandUI(const QString fName, const QString cmd, Ui::MainWindow* ui, App* app) {
    this->fName = fName;
    this->cmd = cmd;
    this->ui = ui;
    this->widget = ui->pnlArguments;
    this->app = app;
    // par = new(QMap <QString, QString>);

    QVBoxLayout* layout{dynamic_cast<QVBoxLayout*>(widget->layout())};
    if (layout != NULL) {
        clearLayout(layout);
        delete layout;
    }

    QVBoxLayout* newLayout(new QVBoxLayout);
    widget->setLayout(newLayout);

    QFont font;
    font.setBold(true);

    QLabel *label = new QLabel(widget);
    label->setText(cmd);
    label->setFont(font);
    newLayout->addWidget(label);

    this->xeq = parse(fName, cmd, newLayout);


    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    newLayout->addItem(spacer);
}

//******************************************************************************
// clearLayout()
//******************************************************************************
void CommandUI::clearLayout(QLayout *layout) {
    if (layout == NULL)
        return;
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
           delete item->widget();
        }
        delete item;
    }
}

//******************************************************************************
// parse()
//******************************************************************************
QString CommandUI::parse(const QString fName, const QString cmd, QVBoxLayout *layout) {
    QFile f(fName);
    QDomDocument xmlCommands;
    if (f.open(QIODevice::ReadOnly )) {
        xmlCommands.setContent(&f);
        f.close();
    }
    QDomElement root = xmlCommands.documentElement();
    QDomElement Command = root.firstChild().toElement();
    // QString xeq;
    while (!Command.isNull()) {
        if (Command.tagName() == "command") {
            QString Title = Command.attribute("title", "No Title");
            if (Title == cmd) {
                QList<QDomElement> elmXeq;
                findElementsWithTagName(Command, QString("value"), elmXeq);
                xeq = elmXeq.at(0).text();

                int x(0);

                QGroupBox* groupBox = new QGroupBox();
                QVBoxLayout* groupBoxLayout = new QVBoxLayout();
                groupBox->setLayout(groupBoxLayout);

                QList<QDomElement> elmGroups;
                findElementsWithTagName(Command, QString("group"), elmGroups);
                Q_FOREACH(const QDomElement& elmGroup, elmGroups) {
                    QList<QDomElement> options;
                    findElementsWithTagName(elmGroup, QString("option"), options);
                    Q_FOREACH(const QDomElement& option, options) {
                        x++;
                        if (option.attribute("format","NONE") == "SYSNAME") {
                            qDebug() << "SYSNAME";
                            addOptionSysname(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "USERNAME") {
                            qDebug() << "USERNAME";
                            addOptionUsername(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "ESCSTRING") {
                            qDebug() << "ESCSTRING";
                            addOptionEscString(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "PASSWORD") {
                            qDebug() << "PASSWORD";
                            addOptionPassword(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "BUTTON") {
                            qDebug() << "BUTTON";
                            addOptionLink(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "LINK") {
                            qDebug() << "LINK";
                            addOptionLink(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "LIST") {
                            qDebug() << "LIST";
                            addOptionList(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "FILE") {
                            qDebug() << "FILE";
                            addOptionFile(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "NEWFILE") {
                            qDebug() << "NEWFILE";
                            addOptionNewFile(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "DIR") {
                            qDebug() << "DIR";
                            addOptionDir(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "NEWDIR") {
                            qDebug() << "NEWDIR";
                            addOptionNewDir(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "CHECKBOX") {
                            qDebug() << "CHECKBOX";
                            addOptionCheckbox(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "TRAILING") {
                            qDebug() << "TRAILING";
                            addOptionTrailing(groupBoxLayout, x, option);
                        } else if (option.attribute("format","NONE") == "LABEL") {
                            qDebug() << "LABEL";
                            // addOptionUsername(groupBoxLayout, x, option);
                        }
                    }
                    addSpacer(groupBoxLayout, x);
                    layout->addWidget(groupBox);
                }
                /*
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
                */
            }
        }
        Command = Command.nextSibling().toElement();
    }
    return(xeq);
}

//******************************************************************************
// addOptionTrailing()
//******************************************************************************
void CommandUI::addOptionTrailing(QVBoxLayout* layout, int row, const QDomElement& option) {
    QList<QDomElement> elm;
    findElementsWithTagName(option, QString("value"), elm);
    this->trailing.append(spaceOrNot(option) + elm.at(0).text() + spaceOrNot(option));
}

//******************************************************************************
// addOptionCheckbox()
//******************************************************************************
void CommandUI::addOptionCheckbox(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QCheckBox* myCheckBox = new QCheckBox();
    QDomElement opt = option;
    QObject::connect(myCheckBox, &QCheckBox::stateChanged,
        [=](const int state) {
            qDebug() << "connecting";
            uiCheckboxEvent(myCheckBox, state, opt);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myCheckBox);
}

//******************************************************************************
// addOptionDir()
//******************************************************************************
void CommandUI::addOptionDir(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myFileEdit = new QLineEdit();
    QDomElement opt = option;
    QObject::connect(myFileEdit, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiDirEvent(myFileEdit, v, opt);
        }
    );
    QPushButton* myButtonFile = new QPushButton();
    myButtonFile->setIcon(QIcon("16x16/Document.png"));
    QObject::connect(myButtonFile, &QPushButton::clicked,
        [=]() {
            uiButtonDirEvent(myFileEdit);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myFileEdit);
    lineLayout->addWidget(myButtonFile);
}

//******************************************************************************
// addOptionNewDir()
//******************************************************************************
void CommandUI::addOptionNewDir(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myFileEdit = new QLineEdit();
    QDomElement opt = option;
    QObject::connect(myFileEdit, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiNewDirEvent(myFileEdit, v, opt);
        }
    );
    QPushButton* myButtonFile = new QPushButton();
    myButtonFile->setIcon(QIcon("16x16/Document.png"));
    QObject::connect(myButtonFile, &QPushButton::clicked,
        [=]() {
            uiButtonNewDirEvent(myFileEdit);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myFileEdit);
    lineLayout->addWidget(myButtonFile);
}

//******************************************************************************
// addOptionList()
//******************************************************************************
void CommandUI::addOptionList(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QComboBox* myListBox = new QComboBox();
    QDomElement opt = option;
    QObject::connect(myListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](int index) {
            qDebug() << "connecting";
            uiListboxEvent(myListBox, index, opt);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myListBox);
}

//******************************************************************************
// addOptionLink()
//******************************************************************************
void CommandUI::addOptionLink(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QPushButton* myButton = new QPushButton();
    QDomElement opt = option;
    QObject::connect(myButton, &QPushButton::clicked,
        [=]() {
            qDebug() << "connecting";
            uiButtonEvent(opt);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myButton);
}

//******************************************************************************
// addOptionUsername()
//******************************************************************************
void CommandUI::addOptionUsername(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myLineEdit = new QLineEdit();
    QDomElement opt = option;
    QObject::connect(myLineEdit, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiUsernameEvent(myLineEdit, v, opt);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myLineEdit);
}

//******************************************************************************
// addOptionEscString()
//******************************************************************************
void CommandUI::addOptionEscString(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myLineEdit = new QLineEdit();
    QDomElement opt = option;
    QObject::connect(myLineEdit, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiEscStringEvent(myLineEdit, v, opt);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myLineEdit);
}

//******************************************************************************
// addOptionPassword()
//******************************************************************************
void CommandUI::addOptionPassword(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myPassword = new QLineEdit();
    myPassword->setEchoMode(QLineEdit::Password);
    QDomElement opt = option;
    QObject::connect(myPassword, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiPasswordEvent(myPassword, v, opt);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myPassword);
}

//******************************************************************************
// addOptionFile()
//******************************************************************************
void CommandUI::addOptionFile(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myFileEdit = new QLineEdit();
    QDomElement opt = option;
    QObject::connect(myFileEdit, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiFileEvent(myFileEdit, v, opt);
        }
    );
    QPushButton* myButtonFile = new QPushButton();
    myButtonFile->setIcon(QIcon("16x16/Document.png"));
    QObject::connect(myButtonFile, &QPushButton::clicked,
        [=]() {
            uiButtonFileEvent(myFileEdit, getExtensionFile(option));
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myFileEdit);
    lineLayout->addWidget(myButtonFile);
}

//******************************************************************************
// addOptionNewFile()
//******************************************************************************
void CommandUI::addOptionNewFile(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myFileEdit = new QLineEdit();
    QDomElement opt = option;
    QObject::connect(myFileEdit, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiNewFileEvent(myFileEdit, v, opt);
        }
    );
    QPushButton* myButtonFile = new QPushButton();
    myButtonFile->setIcon(QIcon("16x16/Document.png"));
    QObject::connect(myButtonFile, &QPushButton::clicked,
        [=]() {
            uiButtonNewFileEvent(myFileEdit, getExtensionFile(option));
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myFileEdit);
    lineLayout->addWidget(myButtonFile);
}

//******************************************************************************
// addOptionSysname()
//******************************************************************************
void CommandUI::addOptionSysname(QVBoxLayout* layout, int row, const QDomElement& option) {
    QHBoxLayout* lineLayout = new QHBoxLayout();
    layout->addLayout(lineLayout, row);
    QLineEdit* myLineEdit = new QLineEdit();
    QDomElement opt = option;
    QObject::connect(myLineEdit, &QLineEdit::textChanged,
        [=](const QString v) {
            qDebug() << "connecting";
            uiSysnameEvent(myLineEdit, v, opt);
        }
    );
    lineLayout->addWidget(new QLabel(getLabel(option)));
    lineLayout->addWidget(myLineEdit);
    /*
    lineLayout = QHBoxLayout()
    layout.addLayout(lineLayout, row)
    myLineEdit = QLineEdit()
    myLineEdit.textChanged.connect(lambda value, x=myLineEdit, y=option: self.uiSysnameEvent(x, value, option))

    connect(ui->canvas, &MyCanvas::mouseLoc,
            [this](){this->ui->statusBar->showMessage(this->ui->canvas->mouseLocString, 0);});

                    OR

    connect(myLineEdit, &QLineEdit::textChanged, [=](QString obj) { customSlot(obj); });

    lineLayout.addWidget(QLabel(self.getLabel(option)))
    lineLayout.addWidget(myLineEdit)
    */
}
//******************************************************************************
// getLabel()
//******************************************************************************
QString CommandUI::getLabel(const QDomElement& option) {
    QString label = "*NONE";
    QList<QDomElement> elmLabels;
    findElementsWithTagName(option, QString("label"), elmLabels);
    if (!elmLabels.isEmpty()) {
        label = elmLabels[0].text();
    }
    return label;
    /*
    def getLabel(self, option):
        sLabel = const.NOT_AVAILABLE_LABEL
        label = option.find("label")
        if label is not None:
            sLabel = label.text
        return sLabel
    */
}

//******************************************************************************
// getExtensionFile()
//******************************************************************************
QString CommandUI::getExtensionFile(const QDomElement& option) {
    QString ext = "*NONE";
    QList<QDomElement> elmExts;
    findElementsWithTagName(option, QString("ext"), elmExts);
    if (!elmExts.isEmpty()) {
        ext = elmExts[0].text();
    }
    return ext;
}

//******************************************************************************
// addSpacer()
//******************************************************************************
void CommandUI::addSpacer(QVBoxLayout* layout, int row) {
    QSpacerItem* vSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addSpacerItem(vSpacer);
    /*
    def addSpacer(self, layout, row):
        vSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)
        layout.addItem(vSpacer)
    */
}

//******************************************************************************
// uiPasswordEvent()
//******************************************************************************
void CommandUI::uiPasswordEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {
    qDebug() << value;
    qDebug() << option.text();
    if (value == "") {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        removeParameter(elm.at(0).text());
    } else {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        qDebug() << elm.at(0).text();
        addParameter(elm.at(0).text(), spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiEscStringEvent()
//******************************************************************************
void CommandUI::uiEscStringEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {
    qDebug() << value;
    qDebug() << option.text();
    if (value == "") {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        removeParameter(elm.at(0).text());
    } else {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        qDebug() << elm.at(0).text();
        addParameter(elm.at(0).text(), spaceOrNot(option) + "\"" + value + "\"" + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiButtonEvent()
//******************************************************************************
void CommandUI::uiButtonEvent(const QDomElement& option) {
    QList<QDomElement> elm;
    findElementsWithTagName(option, QString("value"), elm);
    qDebug() << elm.at(0).text();
    addParameter(elm.at(0).text(), "");
    updateCommandLine();
}

//******************************************************************************
// uiListboxEvent()
//******************************************************************************
void CommandUI::uiListboxEvent(QComboBox* listBox, int index, const QDomElement& option) {
    QString value = listBox->itemText(index);
    if (value == "") {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        removeParameter(elm.at(0).text());
    } else {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        qDebug() << elm.at(0).text();
        addParameter(elm.at(0).text(), spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiCheckboxEvent()
//******************************************************************************
void CommandUI::uiCheckboxEvent(QCheckBox* listBox, int state, const QDomElement& option) {
    if (state == Qt::Checked) {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        addParameter(elm.at(0).text(), "");
    } else {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        removeParameter(elm.at(0).text());
    }
    updateCommandLine();
}

//******************************************************************************
// uiFileEvent()
//******************************************************************************
void CommandUI::uiFileEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {   
    QString name;
    QList<QDomElement> elm;
    findElementsWithTagName(option, QString("value"), elm);
    if (elm.isEmpty()) {
        name.sprintf("%s_%05d", app->appConstants->getQString("NOT_AVAILABLE_LABEL").toStdString().c_str(), this->anonymous);
        this->anonymous++;
    } else {
        name.sprintf("%s", elm.at(0).text().toStdString().c_str());
    }
    if (value == "") {
        removeParameter(name);
    } else {
        addParameter(name, spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiNewFileEvent()
//******************************************************************************
void CommandUI::uiNewFileEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {
    QString name;
    QList<QDomElement> elm;
    findElementsWithTagName(option, QString("value"), elm);
    if (elm.isEmpty()) {
        name.sprintf("%s_%05d", app->appConstants->getQString("NOT_AVAILABLE_LABEL").toStdString().c_str(), this->anonymous);
        this->anonymous++;
    } else {
        name.sprintf("%s", elm.at(0).text().toStdString().c_str());
    }
    if (value == "") {
        removeParameter(name);
    } else {
        addParameter(name, spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiUsernameEvent()
//******************************************************************************
void CommandUI::uiUsernameEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {
    qDebug() << value;
    qDebug() << option.text();
    if (value == "") {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        removeParameter(elm.at(0).text());
    } else {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        qDebug() << elm.at(0).text();
        addParameter(elm.at(0).text(), spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiSysnameEvent()
//******************************************************************************
void CommandUI::uiSysnameEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {
    qDebug() << value;
    qDebug() << option.text();
    if (value == "") {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        removeParameter(elm.at(0).text());
    } else {
        QList<QDomElement> elm;
        findElementsWithTagName(option, QString("value"), elm);
        qDebug() << elm.at(0).text();
        addParameter(elm.at(0).text(), spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
    /*
    if value == "":
        self.removeParameter(option.find("value").text)
    else:
        self.addParameter(option.find("value").text, self.spaceOrNot(option) + value + self.spaceOrNot(option))
    self.updateCommandLine()
    */
}

//******************************************************************************
// uiButtonFileEvent()
//******************************************************************************
void CommandUI::uiButtonFileEvent(QLineEdit* fileEdit, QString ext) {
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"), QDir::currentPath(), ext);
    fileEdit->setText(fileName);
    /*
    filename = str(QFileDialog.getOpenFileName(None, 'Open file', '', ext))
    fileEdit.setText(os.path.normpath(filename))
    */
}

//******************************************************************************
// uiButtonNewFileEvent()
//******************************************************************************
void CommandUI::uiButtonNewFileEvent(QLineEdit* fileEdit, QString ext) {
    QString fileName = QFileDialog::getSaveFileName(NULL, tr("New File"), QDir::currentPath(), ext);
    fileEdit->setText(fileName);
    /*
    filename = str(QFileDialog.getOpenFileName(None, 'Open file', '', ext))
    fileEdit.setText(os.path.normpath(filename))
    */
}

//******************************************************************************
// uiDirEvent()
//******************************************************************************
void CommandUI::uiDirEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {
    QString name;
    QList<QDomElement> elm;
    findElementsWithTagName(option, QString("value"), elm);
    if (elm.isEmpty()) {
        name.sprintf("%s_%05d", app->appConstants->getQString("NOT_AVAILABLE_LABEL").toStdString().c_str(), this->anonymous);
        this->anonymous++;
    } else {
        name.sprintf("%s", elm.at(0).text().toStdString().c_str());
    }
    if (value == "") {
        removeParameter(name);
    } else {
        addParameter(name, spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiButtonDirEvent()
//******************************************************************************
void CommandUI::uiButtonDirEvent(QLineEdit* fileEdit) {
    QString fileName = QFileDialog::getExistingDirectory(NULL, tr("Open directory"), QDir::currentPath());
    fileEdit->setText(fileName);
}

//******************************************************************************
// uiNewDirEvent()
//******************************************************************************
void CommandUI::uiNewDirEvent(QLineEdit* lineEdit, QString value, const QDomElement& option) {
    QString name;
    QList<QDomElement> elm;
    findElementsWithTagName(option, QString("value"), elm);
    if (elm.isEmpty()) {
        name.sprintf("%s_%05d", app->appConstants->getQString("NOT_AVAILABLE_LABEL").toStdString().c_str(), this->anonymous);
        this->anonymous++;
    } else {
        name.sprintf("%s", elm.at(0).text().toStdString().c_str());
    }
    if (value == "") {
        removeParameter(name);
    } else {
        addParameter(name, spaceOrNot(option) + value + spaceOrNot(option));
    }
    updateCommandLine();
}

//******************************************************************************
// uiButtonNewDirEvent()
//******************************************************************************
void CommandUI::uiButtonNewDirEvent(QLineEdit* fileEdit) {
    QString fileName = QFileDialog::getExistingDirectory(NULL, tr("New directory source"), QDir::currentPath());
    fileEdit->setText(fileName);
}

//******************************************************************************
// addParameter()
//******************************************************************************
void CommandUI::addParameter(QString param, QString value, bool spaces) {
    qDebug() << param;
    qDebug() << value;

      if (!par.isEmpty()) {
        if (par.contains(param)) {
            par[param] = value;
        } else {
            par.insert(param, value);
        }
    } else {
        par.insert(param, value);
    }
    /*
    if not param in self.par:
        self.par[param] = value
    else:
        # self.par.pop(param)
        # self.par[param] = value
        self.replaceParameter(param, value)
    */
}

//******************************************************************************
// replaceParameter()
//******************************************************************************
void CommandUI::replaceParameter(QString param, QString value, bool spaces) {
    par[param] = value;
    /*
    for key in self.par.keys():
        if key == param:
            self.par[key] = value
    */
}

//******************************************************************************
// removeParameter()
//******************************************************************************
void CommandUI::removeParameter(QString param) {
    par.remove(param);
    /*
    if param in self.par:
        self.par.pop(param)
    */
}

//******************************************************************************
// spaceOrNot()
//******************************************************************************
QString CommandUI::spaceOrNot(const QDomElement& option) {
    QString rc(" ");
    QList<QDomElement> elm;
    findElementsWithAttribute(option, QString("nospace"), elm);
    if (!elm.empty())
        rc = "";
    return rc;
    /*
    if 'nospace' in option.attrib:
        return("")
    else:
        return(" ")
    */
}

//******************************************************************************
// updateCommandLine()
//******************************************************************************
void CommandUI::updateCommandLine() {
    QString pvalue;
    QString sXeq(this->xeq + " ");
    QMapIterator<QString, QString> i(this->par);
    while (i.hasNext()) {
        i.next();
        qDebug() << i.key() << ": " << i.value();
        QString param = i.value();
        if (param[0]==' ' && param[param.length()-1]==' ') {
            if (param.trimmed().contains(" ")) {
                pvalue = " '" + param.trimmed() + "' ";
            } else {
                pvalue = " " + param.trimmed() + " ";
            }
        } else {
            if (param.contains(" ")) {
                pvalue = "'" + param.trimmed() + "'";
            } else {
                pvalue = param.trimmed();
            }
        }
        if (i.key().startsWith("*N/A")) {
            sXeq = sXeq + pvalue;
        } else {
            sXeq = sXeq + i.key() + pvalue;
        }
    }
    foreach(QString p, trailing) {
        sXeq = sXeq + p;
    }
    ui->txtCommand->setText(sXeq);
}
    /*
    xeq =  self.xeq + " " 1629.56
    for p in self.par:
        param = self.par[p]
        if param[0]==' ' and param[len(param)-1]==' ':
            if " " in param.strip():
                pvalue = " '" + param.strip() + "' "
            else:
                pvalue = " " + param.strip() + " "
        else:
            if " " in param:
                pvalue = "'" + param.strip() + "'"
            else:
                pvalue = param.strip()
        # NOT_AVAILABLE_LABEL = "*N/A"
        if p[0:len(const.NOT_AVAILABLE_LABEL)] == const.NOT_AVAILABLE_LABEL:
            xeq = xeq + pvalue
        else:
            xeq = xeq + p + pvalue
    for p in self.trailing:
        xeq = xeq + p
    self.widget.txtCommand.setText(xeq)
    */


//******************************************************************************
// findElementsWithAttribute()
//******************************************************************************
void CommandUI::findElementsWithAttribute(const QDomElement& elem, const QString& attr, QList<QDomElement>& foundElements) {
    if (elem.attributes().contains(attr))
        foundElements.append(elem);

    QDomElement child = elem.firstChildElement();
    while (!child.isNull()) {
        findElementsWithAttribute(child, attr, foundElements);
        child = child.nextSiblingElement();
    }
}

//******************************************************************************
// findElementsWithTagName()
//******************************************************************************
void CommandUI::findElementsWithTagName(const QDomElement& elem, const QString& tag, QList<QDomElement>& foundElements) {
    // qDebug() << elem.tagName();
    if (elem.tagName() == tag) {
        foundElements.append(elem);
        // qDebug() << "Found tagName";
    }

    QDomElement child = elem.firstChildElement();
    while (!child.isNull()) {
        findElementsWithTagName(child, tag, foundElements);
        child = child.nextSiblingElement();
    }
}
/*
    tree = ET.parse(file) M!X
    root = tree.getroot()
    for commands in root.iter("commands"):
        for command in commands.iter("command"):
            if cmd == command.attrib['title']:
                xeq = command.find("value").text
                for alt in command.iter("group"):
                    x = 0
                    groupBox = QGroupBox()
                    groupBoxLayout=QVBoxLayout()
                    groupBox.setLayout(groupBoxLayout)
                     options = alt.find("options")
                    for option in options.iter("option"):
                        x = x + 1
                        if option.attrib['format'] == "SYSNAME":
                            self.addOptionSysname(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "USERNAME":
                            self.addOptionUsername(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "ESCSTRING":
                            self.addOptionEscString(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "PASSWORD":
                            self.addOptionPassword(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "BUTTON":
                            self.addOptionLink(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "LINK":
                            self.addOptionLink(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "LIST":
                            self.addOptionList(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "FILE":
                            self.addOptionFile(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "NEWFILE":
                            self.addOptionNewFile(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "DIR":
                            self.addOptionDir(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "NEWDIR":
                            self.addOptionNewDir(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "CHECKBOX":
                            self.addOptionCheckbox(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "TRAILING":
                            self.addOptionTrailing(groupBoxLayout, x, option)
                        elif option.attrib['format'] == "LABEL":
                            print("LABEL")
                    self.addSpacer(groupBoxLayout, x)
                    layout.addWidget(groupBox)
    return xeq
*/



/*
#!/usr/bin/env python
# -*- coding: utf-8 -*-
#-------------------------------------------------------------------------------
#  _____ _____ _____ _____
# |   __|     |   __|  |  |
# |  |  |  |  |__   |     |
# |_____|_____|_____|__|__|
#
#       (C) JPL 2019
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------------
from PyQt5 import uic
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
import xml.etree.ElementTree as ET
import const, os

#-------------------------------------------------------------------------------
# Class CommandsUI
#-------------------------------------------------------------------------------
class CommandsUI():
    xeq = ""
    par = {}
    trailing = []
    anonymous = 0

#-------------------------------------------------------------------------------
# __init__()
#-------------------------------------------------------------------------------
    def __init__ (self, file, cmd, widget):
        self.widget = widget
        self.file = file
        self.cmd = cmd
        layout = widget.pnlArguments.layout()
        self.clearLayout(layout)

        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout,0,0)
        myFont = QFont()
        myFont.setBold(True)
        label = QLabel(cmd)
        label.setFont(myFont)
        lineLayout.addWidget(label)

        self.xeq = self.parseCommand(file, cmd, layout)
        self.updateCommandLine()

        layout.addItem(QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding))

#-------------------------------------------------------------------------------
# reset()
#-------------------------------------------------------------------------------
    def reset(self):
        self.xeq = ""
        self.par.clear()
        self.trailing.clear()
        self.anonymous = 0
        self.__init__(self.file, self.cmd, self.widget)

#-------------------------------------------------------------------------------
# updateCommandLine()
#-------------------------------------------------------------------------------
    def updateCommandLine(self):
        xeq =  self.xeq + " "
        for p in self.par:
            param = self.par[p]
            if param[0]==' ' and param[len(param)-1]==' ':
                if " " in param.strip():
                    pvalue = " '" + param.strip() + "' "
                else:
                    pvalue = " " + param.strip() + " "
            else:
                if " " in param:
                    pvalue = "'" + param.strip() + "'"
                else:
                    pvalue = param.strip()
            if p[0:len(const.NOT_AVAILABLE_LABEL)] == const.NOT_AVAILABLE_LABEL:
                xeq = xeq + pvalue
            else:
                xeq = xeq + p + pvalue
        for p in self.trailing:
            xeq = xeq + p
        self.widget.txtCommand.setText(xeq)

#-------------------------------------------------------------------------------
# addOptionSysname()
#-------------------------------------------------------------------------------
    def addOptionSysname(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        myLineEdit = QLineEdit()
        myLineEdit.textChanged.connect(lambda value, x=myLineEdit, y=option: self.uiSysnameEvent(x, value, option))
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        lineLayout.addWidget(myLineEdit)

#-------------------------------------------------------------------------------
# addOptionUsername()
#-------------------------------------------------------------------------------
    def addOptionUsername(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        myLineEdit = QLineEdit()
        myLineEdit.textChanged.connect(lambda value, x=myLineEdit, y=option: self.uiUsernameEvent(x, value, option))
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        lineLayout.addWidget(myLineEdit)

#-------------------------------------------------------------------------------
# addOptionEscString()
#-------------------------------------------------------------------------------
    def addOptionEscString(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        myLineEdit = QLineEdit()
        myLineEdit.textChanged.connect(lambda value, x=myLineEdit, y=option: self.uiEscStringEvent(x, value, option))
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        lineLayout.addWidget(myLineEdit)

#-------------------------------------------------------------------------------
# addOptionPassword()
#-------------------------------------------------------------------------------
    def addOptionPassword(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        myPassword = QLineEdit()
        myPassword.setEchoMode(QLineEdit.Password)
        myPassword.textChanged.connect(lambda value, x=myPassword, y=option: self.uiPasswordEvent(x, value, option))
        lineLayout.addWidget(myPassword)

#-------------------------------------------------------------------------------
# addOptionFile()
#-------------------------------------------------------------------------------
    def addOptionFile(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        myFileEdit = QLineEdit()
        myFileEdit.textChanged.connect(lambda value, x=myFileEdit, y=option: self.uiFileEvent(x, value, option))
        lineLayout.addWidget(myFileEdit)
        myButtonFile = QPushButton()
        myButtonFile.setIcon(QIcon('16x16/Document.png'))
        myButtonFile.clicked.connect(lambda state, x=myFileEdit, y=self.getExtensionFile(option): self.uiButtonFileEvent(x, y))
        lineLayout.addWidget(myButtonFile)

#-------------------------------------------------------------------------------
# addOptionNewFile()
#-------------------------------------------------------------------------------
    def addOptionNewFile(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        myFileEdit = QLineEdit()
        myFileEdit.textChanged.connect(lambda value, x=myFileEdit, y=option: self.uiNewFileEvent(x, value, option))
        lineLayout.addWidget(myFileEdit)
        myButtonFile = QPushButton()
        myButtonFile.setIcon(QIcon('16x16/Document.png'))
        myButtonFile.clicked.connect(lambda state, x=myFileEdit, y=self.getExtensionFile(option): self.uiButtonNewFileEvent(x, y))
        lineLayout.addWidget(myButtonFile)

#-------------------------------------------------------------------------------
# addOptionDir()
#-------------------------------------------------------------------------------
    def addOptionDir(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        myFileEdit = QLineEdit()
        myFileEdit.textChanged.connect(lambda value, x=myFileEdit, y=option: self.uiDirEvent(x, value, option))
        lineLayout.addWidget(myFileEdit)
        myButtonFile = QPushButton()
        myButtonFile.setIcon(QIcon('16x16/Folder.png'))
        myButtonFile.clicked.connect(lambda state, x=myFileEdit: self.uiButtonDirEvent(x))
        lineLayout.addWidget(myButtonFile)

#-------------------------------------------------------------------------------
# addOptionNewDir()
#-------------------------------------------------------------------------------
    def addOptionNewDir(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        myFileEdit = QLineEdit()
        myFileEdit.textChanged.connect(lambda value, x=myFileEdit, y=option: self.uiNewDirEvent(x, value, option))
        lineLayout.addWidget(myFileEdit)
        myButtonFile = QPushButton()
        myButtonFile.setIcon(QIcon('16x16/Folder.png'))
        myButtonFile.clicked.connect(lambda state, x=myFileEdit: self.uiButtonNewDirEvent(x))
        lineLayout.addWidget(myButtonFile)

#-------------------------------------------------------------------------------
# addOptionCheckbox()
#-------------------------------------------------------------------------------
    def addOptionCheckbox(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        myCheckBox = QCheckBox()
        myCheckBox.stateChanged.connect(lambda state, x=myCheckBox, y=option: self.uiCheckboxEvent(x, state, option))
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        lineLayout.addWidget(myCheckBox)

#-------------------------------------------------------------------------------
# addOptionLink()
#-------------------------------------------------------------------------------
    def addOptionLink(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        myButton = QPushButton(self.getLabel(option))
        myButton.clicked.connect(lambda state, x=option: self.uiButtonEvent(x))
        lineLayout.addWidget(myButton)

#-------------------------------------------------------------------------------
# addOptionTrailing()
#-------------------------------------------------------------------------------
    def addOptionTrailing(self, layout, row, option):
        param = option.find("value").text
        self.trailing.append(self.spaceOrNot(option) + param + self.spaceOrNot(option))

#-------------------------------------------------------------------------------
# addSpacer()
#-------------------------------------------------------------------------------
    def addSpacer(self, layout, row):
        vSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)
        layout.addItem(vSpacer)

#-------------------------------------------------------------------------------
# addOptionList()
#-------------------------------------------------------------------------------
    def addOptionList(self, layout, row, option):
        lineLayout = QHBoxLayout()
        layout.addLayout(lineLayout, row)
        myListBox = QComboBox()
        for item in self.getListItems(option):
            myListBox.addItem(item)
        lineLayout.addWidget(QLabel(self.getLabel(option)))
        myListBox.currentIndexChanged.connect(lambda index, x=myListBox, y=option: self.uiListboxEvent(x, index, option))
        lineLayout.addWidget(myListBox)

#-------------------------------------------------------------------------------
# clearLayout()
#-------------------------------------------------------------------------------
    def clearLayout(self, layout):
        if layout is not None:
            while layout.count():
                item = layout.takeAt(0)
                widget = item.widget()
                if widget is not None:
                    widget.deleteLater()
                else:
                    self.clearLayout(item.layout())

#-------------------------------------------------------------------------------
# parseCommand()
#-------------------------------------------------------------------------------
    def parseCommand(self, file, cmd, layout):
        tree = ET.parse(file)
        root = tree.getroot()
        for commands in root.iter("commands"):
            for command in commands.iter("command"):
                if cmd == command.attrib['title']:
                    xeq = command.find("value").text
                    for alt in command.iter("group"):
                        x = 0
                        groupBox = QGroupBox()
                        groupBoxLayout=QVBoxLayout()
                        groupBox.setLayout(groupBoxLayout)
                        options = alt.find("options")
                        for option in options.iter("option"):
                            x = x + 1
                            if option.attrib['format'] == "SYSNAME":
                                self.addOptionSysname(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "USERNAME":
                                self.addOptionUsername(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "ESCSTRING":
                                self.addOptionEscString(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "PASSWORD":
                                self.addOptionPassword(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "BUTTON":
                                self.addOptionLink(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "LINK":
                                self.addOptionLink(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "LIST":
                                self.addOptionList(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "FILE":
                                self.addOptionFile(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "NEWFILE":
                                self.addOptionNewFile(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "DIR":
                                self.addOptionDir(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "NEWDIR":
                                self.addOptionNewDir(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "CHECKBOX":
                                self.addOptionCheckbox(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "TRAILING":
                                self.addOptionTrailing(groupBoxLayout, x, option)
                            elif option.attrib['format'] == "LABEL":
                                print("LABEL")
                        self.addSpacer(groupBoxLayout, x)
                        layout.addWidget(groupBox)
        return xeq

#-------------------------------------------------------------------------------
# uiSysnameEvent()
#-------------------------------------------------------------------------------
    def uiSysnameEvent(self, lineEdit, value, option):
        if value == "":
            self.removeParameter(option.find("value").text)
        else:
            self.addParameter(option.find("value").text, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiUsernameEvent()
#-------------------------------------------------------------------------------
    def uiUsernameEvent(self, lineEdit, value, option):
        if value == "":
            self.removeParameter(option.find("value").text)
        else:
            self.addParameter(option.find("value").text, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiEscStringEvent()
#-------------------------------------------------------------------------------
    def uiEscStringEvent(self, lineEdit, value, option):
        if value == "":
            self.removeParameter(option.find("value").text)
        else:
            self.addParameter(option.find("value").text, self.spaceOrNot(option) + "\"" + value + "\"" + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiPasswordEvent()
#-------------------------------------------------------------------------------
    def uiPasswordEvent(self, lineEdit, value, option):
        if value == "":
            self.removeParameter(option.find("value").text)
        else:
            self.addParameter(option.find("value").text, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiButtonEvent()
#-------------------------------------------------------------------------------
    def uiButtonEvent(self, option):
        self.addParameter(option.find("value").text, "")
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiListboxEvent()
#-------------------------------------------------------------------------------
    def uiListboxEvent(self, listBox, index, option):
        value = listBox.itemText(index)
        if value == const.NOT_AVAILABLE_LABEL:
            self.removeParameter(option.find("value").text)
        else:
            self.addParameter(option.find("value").text, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiCheckboxEvent()
#-------------------------------------------------------------------------------
    def uiCheckboxEvent(self, checkBox, state, option):
        if state == Qt.Checked:
            self.addParameter(option.find("value").text, "")
        else:
            self.removeParameter(option.find("value").text)
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiButtonFileEvent()
#-------------------------------------------------------------------------------
    def uiButtonFileEvent(self, fileEdit, ext):
        filename = str(QFileDialog.getOpenFileName(None, 'Open file', '', ext))
        fileEdit.setText(os.path.normpath(filename))

#-------------------------------------------------------------------------------
# uiButtonNewFileEvent()
#-------------------------------------------------------------------------------
    def uiButtonNewFileEvent(self, fileEdit, ext):
        filename = str(QFileDialog.getSaveFileName(None, 'New file', '', ext))
        fileEdit.setText(os.path.normpath(filename))

#-------------------------------------------------------------------------------
# uiFileEvent()
#-------------------------------------------------------------------------------
    def uiFileEvent(self, lineEdit, value, option):
        if option.find("value") is None:
            name = (const.NOT_AVAILABLE_LABEL + "%05d" % self.anonymous)
            self.anonymous = self.anonymous + 1
        else:
            name = option.find("value").text
        if value == "":
            self.removeParameter(name)
        else:
            self.addParameter(name, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiNewFileEvent()
#-------------------------------------------------------------------------------
    def uiNewFileEvent(self, lineEdit, value, option):
        if option.find("value") is None:
            name = (const.NOT_AVAILABLE_LABEL + "%05d" % self.anonymous)
            self.anonymous = self.anonymous + 1
        else:
            name = option.find("value").text
        if value == "":
            self.removeParameter(name)
        else:
            self.addParameter(name, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiButtonDirEvent()
#-------------------------------------------------------------------------------
    def uiButtonDirEvent(self, fileEdit):
        filename = str(QFileDialog.getExistingDirectory(None, 'Open directory', ''))
        fileEdit.setText(os.path.normpath(filename))

#-------------------------------------------------------------------------------
# uiButtonNewDirEvent()
#-------------------------------------------------------------------------------
    def uiButtonNewDirEvent(self, fileEdit):
        filename = str(QFileDialog.getExistingDirectory(None, 'New directory source', ''))
        fileEdit.setText(os.path.normpath(filename))

#-------------------------------------------------------------------------------
# uiDirEvent()
#-------------------------------------------------------------------------------
    def uiDirEvent(self, lineEdit, value, option):
        if option.find("value") is None:
            name = (const.NOT_AVAILABLE_LABEL + "%05d" % self.anonymous)
            self.anonymous = self.anonymous + 1
        else:
            name = option.find("value").text
        if value == "":
            self.removeParameter(name)
        else:
            self.addParameter(name, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# uiNewDirEvent()
#-------------------------------------------------------------------------------
    def uiNewDirEvent(self, lineEdit, value, option):
        if option.find("value") is None:
            name = (const.NOT_AVAILABLE_LABEL + "%05d" % self.anonymous)
            self.anonymous = self.anonymous + 1
        else:
            name = option.find("value").text
        if value == "":
            self.removeParameter(name)
        else:
            self.addParameter(name, self.spaceOrNot(option) + value + self.spaceOrNot(option))
        self.updateCommandLine()

#-------------------------------------------------------------------------------
# getLabel()
#-------------------------------------------------------------------------------
    def getLabel(self, option):
        sLabel = const.NOT_AVAILABLE_LABEL
        label = option.find("label")
        if label is not None:
            sLabel = label.text
        return sLabel

#-------------------------------------------------------------------------------
# getExtensionFile()
#-------------------------------------------------------------------------------
    def getExtensionFile(self, option):
        sExt = ""
        ext = option.find("ext")
        if ext is not None:
            sExt = ext.text
        return sExt

#-------------------------------------------------------------------------------
# getListItems()
#-------------------------------------------------------------------------------
    def getListItems(self, list):
        lItems = [const.NOT_AVAILABLE_LABEL]
        for item in list.iter("item"):
            lItems.append(item.text)
        return lItems

#-------------------------------------------------------------------------------
# addParameter()
#-------------------------------------------------------------------------------
    def addParameter(self, param, value, spaces=True):
        if not param in self.par:
            self.par[param] = value
        else:
            # self.par.pop(param)
            # self.par[param] = value
            self.replaceParameter(param, value)

#-------------------------------------------------------------------------------
# replaceParameter()
#-------------------------------------------------------------------------------
    def replaceParameter(self, param, value, spaces=True):
        for key in self.par.keys():
            if key == param:
                self.par[key] = value

#-------------------------------------------------------------------------------
# removeParameter()
#-------------------------------------------------------------------------------
    def removeParameter(self, param):
        if param in self.par:
            self.par.pop(param)

#-------------------------------------------------------------------------------
# spaceOrNot()
#-------------------------------------------------------------------------------
    def spaceOrNot(self, option):
        if 'nospace' in option.attrib:
            return("")
        else:
            return(" ")

"""
TODO :

* addOptionNewFile
* addOptionFiles
* addOptionDirectory
* addOptionNewDirectory
* addGroupBox
* addOptionOptions

"""


 */
