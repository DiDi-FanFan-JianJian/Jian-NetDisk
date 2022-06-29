QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lws2_32

# gbk±àÂë
QMAKE_CFLAGS += -fexec-charset=GBK -finput-charset=GBK
QMAKE_CXXFLAGS += -fexec-charset=GBK -finput-charset=GBK

INCLUDEPATH += "C:\Program Files\OpenSSL-Win64\include"

LIBS    += "C:\Program Files\OpenSSL-Win64\lib\libssl.lib" \
           "C:\Program Files\OpenSSL-Win64\lib\libcrypto.lib"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    file_function.cpp \
    main.cpp \
    mysocket.cpp \
    netdisk.cpp \
    signinwidget.cpp \
    signupdialog.cpp \
    transferlistdialog.cpp \
    md5.cpp

HEADERS += \
    file_function.h \
    global_msg.h \
    message.h \
    mysocket.h \
    netdisk.h \
    signinwidget.h \
    signupdialog.h \
    transferlistdialog.h \
    md5.h

FORMS += \
    netdisk.ui \
    signinwidget.ui \
    signupdialog.ui \
    transferlistdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    icon.qrc
