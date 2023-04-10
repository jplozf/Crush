#ifndef APP_H
#define APP_H

#include <QString>
#include "constants.h"
#include "settings.h"
#include "utils.h"
#include "downloader.h"

class App
{
public:
    App();
    QString appDir;
    QString docDir;
    Settings *appSettings;
    Constants *appConstants;
    QVector<QString> mruProjects;
};

#endif // APP_H
