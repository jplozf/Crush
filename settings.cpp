#include "settings.h"

//******************************************************************************
// Settings()
//******************************************************************************
Settings::Settings()
{
    // Set  the defaults values...
#ifdef Q_OS_LINUX
#else
#endif
    defaults["CONSOLE_PROMPT"] = QVariant("Gosh >>> ");
    defaults["CONSOLE_BACKGROUND"] = QVariant("white");
    defaults["CONSOLE_OUT_COLOR"] = QVariant("black");
    defaults["CONSOLE_ERR_COLOR"] = QVariant("red");
    defaults["CONSOLE_PROMPT_COLOR"] = QVariant("blue");
    defaults["CONSOLE_FONT_FAMILY"] = QVariant("Courier");
    defaults["CONSOLE_FONT_SIZE"] = QVariant("8");
    defaults["CONSOLE_FONT_STYLE"] = QVariant("normal");
    defaults["CONSOLE_FONT_WEIGHT"] = QVariant("normal");
    defaults["CONSOLE_CODEPAGE"] = QVariant("cp850");
    // defaults["COMMAND_FILE"] = QVariant("gosh.xml");
    defaults["STATUSBAR_TIMEOUT"] = QVariant(3000);
    defaults["XML_KEYWORD_COLOR"] = QVariant("#8B008B");
    defaults["XML_KEYWORD_WEIGHT"] = QVariant(QFont::Bold);
    defaults["XML_KEYWORD_ITALIC"] = QVariant(false);
    defaults["XML_ELEMENT_COLOR"] = QVariant("#117700");
    defaults["XML_ELEMENT_WEIGHT"] = QVariant(QFont::Bold);
    defaults["XML_ELEMENT_ITALIC"] = QVariant(false);
    defaults["XML_ATTRIBUTE_COLOR"] = QVariant("#2020D2");
    defaults["XML_ATTRIBUTE_WEIGHT"] = QVariant(QFont::Bold);
    defaults["XML_ATTRIBUTE_ITALIC"] = QVariant(true);
    defaults["XML_VALUE_COLOR"] = QVariant("#FF0000");
    defaults["XML_VALUE_WEIGHT"] = QVariant(QFont::Normal);
    defaults["XML_VALUE_ITALIC"] = QVariant(false);
    defaults["XML_COMMENT_COLOR"] = QVariant("#808080");
    defaults["XML_COMMENT_WEIGHT"] = QVariant(QFont::Normal);
    defaults["XML_COMMENT_ITALIC"] = QVariant(false);
    defaults["XML_TAB_STOP"] = QVariant(4);

    // Read the settings from user's settings
    read();

    // Check if all defaults settings are set and set them if any
    foreach (const auto& defaultKey, defaults.keys()) {
        if (!settings.contains(defaultKey)) {
            settings[defaultKey] = defaults[defaultKey];
        }
    }

    // Delete all keys that are no more in the defaults settings values
    foreach (const auto& settingKey, settings.keys()) {
        if (!defaults.contains(settingKey)) {
            settings.erase(settings.find(settingKey));
        }
    }

    // Save the settings to user's settings
    write();
}

//******************************************************************************
// get()
//******************************************************************************
QVariant Settings::get(QString param) {
    return this->settings[param];
}

//******************************************************************************
// write()
//******************************************************************************
void Settings::write() {
    // TODO : Apply new settings in real time
    Constants *appConstants = new Constants();
    QDir appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    QFile fSettings(QDir(appDir).filePath(appConstants->getQString("SETTINGS_FILE")));
    if (fSettings.open(QIODevice::WriteOnly)) {
        QDataStream out(&fSettings);
        out.setVersion(QDataStream::Qt_5_3);
        out << settings;
    }
}

//******************************************************************************
// read()
//******************************************************************************
void Settings::read() {
    Constants *appConstants = new Constants();
    QDir appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    QFile fSettings(QDir(appDir).filePath(appConstants->getQString("SETTINGS_FILE")));
    if (fSettings.open(QIODevice::ReadOnly)) {
        QDataStream in(&fSettings);
        in.setVersion(QDataStream::Qt_5_3);
        in >> settings;
    }
}

//******************************************************************************
// form()
//******************************************************************************
void Settings::form(QWidget *w) {
    QFormLayout *form = new QFormLayout(w);
    form->setLabelAlignment(Qt::AlignRight);

    for(auto e : settings.keys())
    {
        QLabel *lblSetting = new QLabel(e);
        QLineEdit *txtSetting = new QLineEdit(settings.value(e).toString());
        connect(txtSetting, &QLineEdit::textChanged, [=]{handleTextChanged(lblSetting, txtSetting);});
        form->addRow(lblSetting, txtSetting);
    }
    w->setLayout(form);
}

//******************************************************************************
// handleTextChanged()
//******************************************************************************
void Settings::handleTextChanged(QLabel *lbl, QLineEdit *txt) {
    // qDebug(lbl->text().toLatin1());
    settings[lbl->text().toLatin1()] = QVariant(txt->text().toLatin1());
    write();
}
