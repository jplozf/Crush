#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDir>
#include <QMap>
#include <QDateTime>
#include <QCryptographicHash>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QSaveFile>
#include <QDebug>
#include <QDirIterator>


class Utils
{
public:
    static qint64 dirSize(QString dirPath);
    static QString formatSize(qint64 size);
    static QMap<QString, QString> fileProperties(QString f);
    static QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);
    static QString capitalize(const QString &str);
    static QString pathAppend(const QString& path1, const QString& path2);
    static void copyDirectoryNested(QString from,QString to);
    static void downloadFile(const QString& url, const QString& target);
    static QString getExtension(QString file);
    static QString tsToString(QString ts, QString fmt="dddd d MMMM yyyy @ hh:mm:ss");
    static QString secondsToString(qint64 seconds);
};

#endif // UTILS_H
