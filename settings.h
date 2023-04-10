#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QVariant>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QStandardPaths>
#include <QPair>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>

#include "constants.h"
#include "utils.h"

class Settings: public QObject
{
    Q_OBJECT
public:
    Settings();
    QMap<QString, QVariant> defaults;
    QMap<QString, QVariant> settings;
    QVariant get(QString param);
    QPushButton *btnTemplatesCutomizing;
    void write();
    void read();
    void form(QWidget *);
    void handleTextChanged(QLabel *lbl, QLineEdit *txt);
};

#endif // SETTINGS_H
