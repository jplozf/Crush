#ifndef COMMANDUI_H
#define COMMANDUI_H

#include <QWidget>
#include <QLayout>
#include <QLayoutItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QFont>
#include <QtXml>
#include <QFile>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CommandUI : public QObject
{
    Q_OBJECT

public:
    explicit CommandUI(const QString fName, const QString cmd, Ui::MainWindow* ui);

private:
    Ui::MainWindow* ui;
    QString fName;
    QString cmd;
    QWidget *widget;
    QString xeq;
    mutable QMutex mutex;
    QMap<QString, QString> par;
    QList<QString> trailing;
    void clearLayout(QLayout *layout);
    QString parse(const QString fName, const QString cmd, QVBoxLayout *layout);
    void findElementsWithAttribute(const QDomElement& elem, const QString& attr, QList<QDomElement>& foundElements);
    void findElementsWithTagName(const QDomElement& elem, const QString& tag, QList<QDomElement>& foundElements);
    void addOptionSysname(QVBoxLayout* layout, int row, const QDomElement& option);    
    void addOptionUsername(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionEscString(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionPassword(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionLink(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionList(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionFile(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionNewFile(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionDir(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionNewDir(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionCheckbox(QVBoxLayout* layout, int row, const QDomElement& option);
    void addOptionTrailing(QVBoxLayout* layout, int row, const QDomElement& option);
    void addParameter(QString param, QString value, bool spaces=true);
    void replaceParameter(QString param, QString value, bool spaces=true);
    void removeParameter(QString param);
    QString spaceOrNot(const QDomElement& option);
    QString getLabel(const QDomElement& option);
    void updateCommandLine();
    void addSpacer(QVBoxLayout* layout, int row);

private slots:
    void uiSysnameEvent(QLineEdit* lineEdit, QString value, const QDomElement& option);
    void uiUsernameEvent(QLineEdit* lineEdit, QString value, const QDomElement& option);
    void uiEscStringEvent(QLineEdit* lineEdit, QString value, const QDomElement& option);
    void uiPasswordEvent(QLineEdit* lineEdit, QString value, const QDomElement& option);
    void uiButtonEvent(const QDomElement& option);
    void uiListboxEvent(QComboBox* listBox, int index, const QDomElement& option);

};

#endif // COMMANDUI_H
