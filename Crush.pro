QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

GIT_HASH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)\\\""
GIT_BRANCH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --abbrev-ref HEAD)\\\""
linux-g++*: BUILD_TIMESTAMP="\\\"$$system(date -u +\""%Y-%m-%dT%H:%M:%SUTC\"")\\\""
win32:      BUILD_TIMESTAMP="\\\"$$system(echo %date%-%time%)\\\""
DEFINES += GIT_HASH=$$GIT_HASH GIT_BRANCH=$$GIT_BRANCH BUILD_TIMESTAMP=$$BUILD_TIMESTAMP

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BasicXMLSyntaxHighlighter.cpp \
    XeqProcess.cpp \
    app.cpp \
    commandui.cpp \
    constants.cpp \
    downloader.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    utils.cpp

HEADERS += \
    BasicXMLSyntaxHighlighter.h \
    XeqProcess.h \
    app.h \
    commandui.h \
    constants.h \
    downloader.h \
    mainwindow.h \
    settings.h \
    utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Crush.qrc

DISTFILES += \
    LICENSE.md \
    README.md \
    Screenshots/Crush_About.png \
    Screenshots/Crush_Console_Output.png \
    Screenshots/Crush_Settings.png \
    Screenshots/Crush_XML_Commands.png \
    gosh.xml \
    scratch \
    todo
