#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QString>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QSaveFile>
#include <QFile>
#include <QCryptographicHash>
#include <QDir>
#include <QUuid>


class Downloader
{
public:
    static void downloadFile(const QString& url, const QString& target);
    static QList<QStringList> getFilesFromIndex(const QString& url);
    static QString getMD5FromLocalFile(const QString& file);
    static bool isFileModified(const QString& file);
};

#endif // DOWNLOADER_H
