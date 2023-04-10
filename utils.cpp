#include "utils.h"

//******************************************************************************
// dirSize()
//******************************************************************************
qint64 Utils::dirSize(QString dirPath) {
    qint64 size = 0;
    QDir dir(dirPath);
    //calculate total size of current directories' files
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }
    //add size of child directories recursively
    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for(QString childDirPath : dir.entryList(dirFilters))
        size+= dirSize(dirPath + QDir::separator() + childDirPath);
    return size;
}

//******************************************************************************
// formatSize()
//******************************************************************************
QString Utils::formatSize(qint64 size) {
    QStringList units = {"Bytes", "KB", "MB", "GB", "TB", "PB"};
    int i;
    double outputSize = size;
    for(i=0; i<units.size()-1; i++) {
        if(outputSize<1024) break;
        outputSize= outputSize/1024;
    }
    return QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]);
}

//******************************************************************************
// fileProperties()
//******************************************************************************
QMap<QString, QString> Utils::fileProperties(QString f) {
    QMap<QString, QString> props;
    QFileInfo fi(f);

    props.insert("File Name", fi.fileName());
    props.insert("Size Bytes", QString::number(fi.size()));
    props.insert("Size Human",formatSize(fi.size()));
    props.insert("Date Created", fi.birthTime().toString("yyyy/MM/dd hh:mm:ss"));
    props.insert("Date Modified", fi.lastModified().toString("yyyy/MM/dd hh:mm:ss"));
    props.insert("Owner User", fi.owner());
    props.insert("Owner Group", fi.group());
    props.insert("Hash MD4", fileChecksum(f, QCryptographicHash::Md4).toHex());
    props.insert("Hash MD5", fileChecksum(f, QCryptographicHash::Md5).toHex());
    props.insert("Hash SHA1", fileChecksum(f, QCryptographicHash::Sha1).toHex());
    props.insert("Hash SHA224", fileChecksum(f, QCryptographicHash::Sha224).toHex());
    props.insert("Hash SHA256", fileChecksum(f, QCryptographicHash::Sha256).toHex());
    props.insert("Hash SHA384", fileChecksum(f, QCryptographicHash::Sha384).toHex());
    props.insert("Hash SHA512", fileChecksum(f, QCryptographicHash::Sha512).toHex());
    props.insert("Hash SHA3_224", fileChecksum(f, QCryptographicHash::Sha3_224).toHex());
    props.insert("Hash SHA3_256", fileChecksum(f, QCryptographicHash::Sha3_256).toHex());
    props.insert("Hash SHA3_384", fileChecksum(f, QCryptographicHash::Sha3_384).toHex());
    props.insert("Hash SHA3_512", fileChecksum(f, QCryptographicHash::Sha3_512).toHex());

    return props;
}

//******************************************************************************
// fileChecksum()
//******************************************************************************
QByteArray Utils::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm) {
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    // Returns empty QByteArray() on failure.
    return QByteArray();
}

//******************************************************************************
// capitalize()
//******************************************************************************
QString Utils::capitalize(const QString &str) {
  QString tmp = str;
  // if you want to ensure all other letters are lowercase:
  tmp = tmp.toLower();
  tmp[0] = str[0].toUpper();
  return tmp;
}

//******************************************************************************
// pathAppend()
//******************************************************************************
QString Utils::pathAppend(const QString& path1, const QString& path2) {
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

//******************************************************************************
// downloadFile()
//******************************************************************************
void Utils::downloadFile(const QString& url, const QString& target) {
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop event;
    QAbstractSocket::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    QByteArray content = response->readAll();
    QSaveFile file(target);
    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.commit();
}

//******************************************************************************
// copyDirectoryNested()
//******************************************************************************
void Utils::copyDirectoryNested(QString from,QString to)
{
    QDirIterator it(from, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString file_in = it.next();
        QFileInfo file_info = QFileInfo(file_in);
        QString file_out = file_in;
        file_out.replace(from,to);
        if(file_info.isFile())
        {
            //is file copy
            qDebug() << QFile::copy(file_in, file_out);
            qDebug() << file_in << "<----" << file_out;
        }

        if(file_info.isDir())
        {
            //dir mkdir
            QDir dir(file_out);
            if (!dir.exists())
                qDebug() << "mkpath"<< dir.mkpath(".");
        }
    }
}

//******************************************************************************
// getExtension()
//******************************************************************************
QString Utils::getExtension(QString file) {
    QFileInfo fi(file);
    return ("." + fi.suffix());
}

//******************************************************************************
// tsToString()
// Format a timestamp string to human readable string
//******************************************************************************
QString Utils::tsToString(QString ts, QString fmt) {
    QDateTime qdt = QDateTime::fromString(ts, "yyyyMMdd-hhmmss");
    QString txt = qdt.toString(fmt);
    return txt;
}

//******************************************************************************
// secondsToString()
//******************************************************************************
QString Utils::secondsToString(qint64 seconds) {
    const qint64 DAY = 86400;
    qint64 days = seconds / DAY;
    QTime t = QTime(0,0).addSecs(seconds % DAY);
    return QString("%1 days, %2 hours, %3 minutes, %4 seconds").arg(days).arg(t.hour()).arg(t.minute()).arg(t.second());
}
