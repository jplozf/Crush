#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <unordered_map>
#include <string>
#include <any>
#include <iostream>
#include <vector>
#include <QString>

class Constants
{
public:
    Constants();
    std::unordered_map<std::string, std::any> constants;
    int getInt(std::string param);
    bool getBool(std::string param);
    QString getQString(std::string param);
    std::string getString(std::string param);
    QString helpText;
    QString aboutText;

private:
    QString setHelp();
    QString setAbout();
};

#endif // CONSTANTS_H
