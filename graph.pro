QT       +=\
    core gui\
    printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


INCLUDEPATH += C:/Qt/Qcustompilot
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    C:/Qt/Qcustompilot/qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    C:/Qt/Qcustompilot/qcustomplot.h

FORMS += \
    mainwindow.ui


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
