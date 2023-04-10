#include "settings.h"

//******************************************************************************
// Settings()
//******************************************************************************
Settings::Settings()
{
    // Set  the defaults values...
#ifdef Q_OS_LINUX
    defaults["DEFAULT_EDITOR"] = QVariant("/usr/bin/geany");
    defaults["DEFAULT_TERMINAL"] = QVariant("/usr/bin/tabby");
    defaults["DEFAULT_BROWSER"] = QVariant("/usr/bin/google-chrome-stable");
    defaults["GIT_BINARY_PATH"] = QVariant("/usr/bin/git");
    defaults["DEFAULT_REPOSITORY"] = QVariant("~/Projets/");
#else
    defaults["DEFAULT_EDITOR"] = QVariant("C:/Program Files/PSPad editor/PSPad.exe");
    defaults["DEFAULT_TERMINAL"] = QVariant("C:/Program Files (x86)/Mobatek/MobaXterm/MobaXterm.exe");
    defaults["DEFAULT_BROWSER"] = QVariant("C:/Program Files/Google/Chrome/Application/chrome.exe");
    defaults["GIT_BINARY_PATH"] = QVariant("C:/Program Files (x86)/git/git.exe");
    defaults["DEFAULT_REPOSITORY"] = QVariant("C:/Projets/");
#endif
    defaults["MRU_PROJECTS"] = QVariant(5);
    defaults["PROJECT_USER_NAME"] = QVariant("");
    defaults["PROJECT_USER_COMPANY"] = QVariant("");
    defaults["PROJECT_USER_MAIL"] = QVariant("");
    defaults["PROJECT_USER_WEB"] = QVariant("");
    defaults["DATETIME_FORMAT"] = QVariant("dddd, d MMMM yyyy @ hh:mm:ss");
    defaults["STATUSBAR_TIMEOUT"] = QVariant(3000);
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
    QLabel *lblSetting = new QLabel("Templates Customizing");
    btnTemplatesCutomizing = new QPushButton("...");
    form->addRow(lblSetting, btnTemplatesCutomizing);
    // TODO : Add "Reset Templates Customizing" option

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
