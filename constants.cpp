#include "constants.h"
#include <QString>

//******************************************************************************
// Settings()
//******************************************************************************
Constants::Constants()
{
    constants["AUTHOR"]    = QVariant("J.-P. Liguori");
    constants["COPYRIGHT"] = QVariant("Copyright 2022, J.-P. Liguori");
    constants["LICENSE"]   = QVariant("GPL");
    constants["VERSION"]   = QVariant("0.9.a");
    constants["EMAIL"]     = QVariant("jpl@ozf.fr");
    constants["NVERSION"]  = QVariant(90);

    constants["ORGANIZATION_NAME"]      = QVariant("Open Zero Factory");
    constants["ORGANIZATION_DOMAIN"]    = QVariant("www.ozf.fr/#crush");
    constants["APPLICATION_NAME"]       = QVariant("Crush");
    constants["APP_FOLDER"]             = QVariant(".crush");
    constants["SETTINGS_FILE"]          = QVariant("settings.cfg");
    constants["COMMANDS_FILE"]          = QVariant("crush.xml");
    constants["WEB_REPOSITORY"]         = QVariant("https://www.ozf.fr/crush/");
    constants["XML_SUBFOLDER"]          = QVariant("xml/");

    constants["NOT_AVAILABLE_LABEL"]    = QVariant("NOT_AVAILABLE_LABEL");

    theme["LIGHT"] = QStringList({"#efefef", "#000000", "#ffffff", "#f7f7f7", "#ffffdc", "#000000", "#000000", "#efefef", "#000000", "#ffffff", "#0000ff", "#308cc6", "#ffffff"});
    theme["DARK"] = QStringList({"#353535", "#ffffff", "#191919", "#353535", "#ffffff", "#ffffff", "#ffffff", "#353535", "#ffffff", "#ff0000", "#2a82da", "#2a82da", "#000000"});

    this->aboutText = setAbout();
}

//******************************************************************************
// getInt()
//******************************************************************************
int Constants::getInt(QString param) {
    return (this->constants[param].toInt());
}

//******************************************************************************
// getBool()
//******************************************************************************
bool Constants::getBool(QString param) {
    return (this->constants[param].toBool());
}

//******************************************************************************
// getQString()
//******************************************************************************
QString Constants::getQString(QString param) {
    return (this->constants[param].value<QString>());
}

//******************************************************************************
// setAbout()
//******************************************************************************
QString Constants::setAbout() {
    QString str;
    str = QString(
        "<h2 id=\"crush\">%1 v%2</h2>"
        "<em>branch: %3, version: %4, built_at: %5</em>"
        "<hr>"
        "<blockquote>"
        "<table>"
        "<tbody>").arg(this->getQString("APPLICATION_NAME"), this->getQString("VERSION")).arg(GIT_BRANCH).arg(GIT_HASH).arg(BUILD_TIMESTAMP);

    QMap<QString, QString> props = Utils::systemProperties();
    for (auto iter = props.constBegin(); iter != props.constEnd(); ++iter) {
        // cout << iter.key() << ": " << iter.value() << endl;
        str += QString("<tr><td align=\"left\"><strong>%1</strong></td>"
               "<td align=\"right\">%2</td>"
               "</tr>").arg(iter.key(), iter.value());
    }

    str += QString("</tbody></table>"
           "</blockquote>"
           "<br>"
           "<hr>"
           "<p><center><em>%1</em></center></p>").arg(this->getQString("COPYRIGHT"));

    return str;
}


