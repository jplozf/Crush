#include "constants.h"
#include <QString>

//******************************************************************************
// Settings()
//******************************************************************************
Constants::Constants()
{
    constants["AUTHOR"]    = std::string("J.-P. Liguori");
    constants["COPYRIGHT"] = std::string("Copyright 2022, J.-P. Liguori");
    constants["LICENSE"]   = std::string("GPL");
    constants["VERSION"]   = std::string("0.9.a");
    constants["EMAIL"]     = std::string("jpl@ozf.fr");
    constants["NVERSION"]  = 90;

    constants["ORGANIZATION_NAME"]      = std::string("Open Zero Factory");
    constants["ORGANIZATION_DOMAIN"]    = std::string("www.ozf.fr/#crush");
    constants["APPLICATION_NAME"]       = std::string("Crush");
    constants["APP_FOLDER"]             = std::string(".crush");
    constants["SETTINGS_FILE"]          = std::string("settings.cfg");
    constants["COMMANDS_FILE"]          = std::string("crush.xml");
    constants["WEB_REPOSITORY"]         = std::string("https://www.ozf.fr/crush/");
    constants["XML_SUBFOLDER"]          = std::string("xml/");

    constants["NOT_AVAILABLE_LABEL"]    = std::string("NOT_AVAILABLE_LABEL");

    this->helpText = setHelp();
    this->aboutText = setAbout();
}

//******************************************************************************
// getInt()
//******************************************************************************
int Constants::getInt(std::string param) {
    return (std::any_cast<int>(this->constants[param]));
}

//******************************************************************************
// getBool()
//******************************************************************************
bool Constants::getBool(std::string param) {
    return (std::any_cast<bool>(this->constants[param]));
}

//******************************************************************************
// getString()
//******************************************************************************
std::string Constants::getString(std::string param) {
    return (std::any_cast<std::string>(this->constants[param]));
}

//******************************************************************************
// getQString()
//******************************************************************************
QString Constants::getQString(std::string param) {
    std::string t = std::any_cast<std::string>(this->constants[param]);
    return (QString::fromUtf8(t.c_str()));
}

//******************************************************************************
// setHelp()
//******************************************************************************
QString Constants::setHelp() {
    QString str(
        "<!DOCTYPE html"
        "<html>"
        "<head>"
        "<title>Help</title>"
        "</head>"
        "<body>"
        "<h2 id=\"commands-available\">Commands available</h2>"
        "<p>Here are some built-in commands available for general purpose. These commands are all introducing by ! character.</p>"
        "<ul>"
        "<li><strong>!BREAK</strong></li>"
        "</ul>"
        "<blockquote>"
        "<p>Stops the current running command.</p>"
        "</blockquote>"
        "<ul>"
        "<li><strong>!CLS</strong></li>"
        "</ul>"
        "<blockquote>"
        "<p>Clear the console.</p>"
        "</blockquote>"
        "<ul>"
        "<li><strong>!QUIT</strong></li>"
        "</ul>"
        "<blockquote>"
        "<p>Exit the Crush program. The same action as F10 key.</p>"
        "</blockquote>"
        "</body>"
        "</html>"
    );
    return str;
}

//******************************************************************************
// setAbout()
//******************************************************************************
QString Constants::setAbout() {
    QString str;
    str = QString(
        "<!DOCTYPE html"
        "<html>"
        "<head>"
        "<title>About</title>"
        "</head>"
        "<body>"
        "<h2 id=\"crush\">%1 v%2</h2>"
        "<hr>"
        "<blockquote>"
        "<table>"
        "<tbody>").arg(this->getQString("APPLICATION_NAME"), this->getQString("VERSION"));

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
           "<p><center><em>%1</em></center></p>"
           "</body>"
           "</html>").arg(this->getQString("COPYRIGHT"));

    return str;
}


