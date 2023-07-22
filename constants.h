#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <unordered_map>
#include <string>
#include <any>
#include <iostream>
#include <vector>
#include <QString>
#include <QSysInfo>

#include <utils.h>

class Constants
{
public:
    Constants();
    int getInt(QString param);
    bool getBool(QString param);
    QString getQString(QString param);
    QString aboutText;
    QMap<QString, QVariant> constants;
    QMap<QString, QStringList> theme;


private:
    QString setAbout();
};

#endif // CONSTANTS_H
