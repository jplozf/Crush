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

