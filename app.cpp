#include "app.h"

//******************************************************************************
// Class App()
//******************************************************************************
App::App() {
    appSettings = new Settings();
    appConstants = new Constants();

    //**************************************************************************
    // Create application folder if not exists, and...
    // Dump the resources content into this new folder.
    //**************************************************************************
    appDir = QDir(QDir::homePath()).filePath(appConstants->getQString("APP_FOLDER"));
    if (!QDir(appDir).exists()) {
        QDir().mkdir(appDir);
        Utils:: copyDirectoryNested(":/", appDir + QDir::separator());
    }

    //**************************************************************************
    // Download the documentation files
    //**************************************************************************
    // docDir = appDir + QDir::separator() + appConstants->getQString("DOC_SUBFOLDER") + QDir::separator();
    docDir = appDir + "/" + appConstants->getQString("XML_SUBFOLDER");
    if (!QDir(docDir).exists()) {
        QDir().mkdir(docDir);
    }
    QList<QStringList> f = Downloader::getFilesFromIndex(appConstants->getQString("WEB_REPOSITORY") + "index.txt");
    foreach(QStringList item, f) {
        Downloader::downloadFile(appConstants->getQString("WEB_REPOSITORY") + item[0], docDir + item[0]);
    }
}
