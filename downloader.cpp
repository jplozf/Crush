#include "downloader.h"

//******************************************************************************
// downloadFile()
//******************************************************************************
void Downloader::downloadFile(const QString& url, const QString& target) {
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop event;
    QEventLoop::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    QByteArray content = response->readAll();
    QSaveFile file(target);
    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.commit();
}

//******************************************************************************
// getMD5FromLocalFile()
//******************************************************************************
QString Downloader::getMD5FromLocalFile(const QString& file) {
    QFile f(file);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray().toHex();
}

//******************************************************************************
// getFilesFromIndex()
//******************************************************************************
QList<QStringList> Downloader::getFilesFromIndex(const QString& url) {
    QList<QStringList> rc;
    QUuid uuid = QUuid::createUuid();
    QString tmpFileName = QDir::toNativeSeparators(QDir::tempPath() + QDir::separator() +  "OZF-" + uuid.toString(QUuid::WithoutBraces));
    downloadFile(url, tmpFileName);
    QFile tmpFile(tmpFileName);
    if (tmpFile.open(QIODevice::ReadOnly)) {
       QTextStream in(&tmpFile);
       while (!in.atEnd()) {
          QString line = in.readLine();
          QString fileName = line.mid(0, line.indexOf(","));
          QString MD5File = (line.mid(line.indexOf(",") + 1)).trimmed();
          rc.append({fileName, MD5File});
       }
       tmpFile.close();
    }
    // tmpFile.remove();
    return rc;
}
