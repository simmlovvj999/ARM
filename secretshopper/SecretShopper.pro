#############################################################
# SecretShopper - qmake project file.
# Open this file in Qt Creator: File -> Open File or Project.
#############################################################

QT += core gui widgets network

TARGET   = SecretShopper
TEMPLATE = app

CONFIG += c++17 warn_on
INCLUDEPATH += $$PWD

DEFINES += QT_DEPRECATED_WARNINGS

msvc {
    QMAKE_CXXFLAGS += /utf-8
}

mingw {
    QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8 -Wno-deprecated-copy
}

# Sources
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mockpriceserver.cpp \
    loyaltysystem.cpp \
    priceservice.cpp \
    routeoptimizer.cpp \
    productlistwidget.cpp \
    storelistwidget.cpp \
    reportwidget.cpp \
    settingswidget.cpp

# Headers
HEADERS += \
    models.h \
    mainwindow.h \
    mockpriceserver.h \
    loyaltysystem.h \
    priceservice.h \
    routeoptimizer.h \
    productlistwidget.h \
    storelistwidget.h \
    reportwidget.h \
    settingswidget.h

# Platform settings
win32 {
    CONFIG(release, debug|release): CONFIG += windows
    CONFIG(debug,   debug|release): CONFIG += console
}

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0
}

# Build output directories. Paths are relative to this .pro location,
# so the project can be moved and still opened directly via SecretShopper.pro.
CONFIG(debug, debug|release) {
    DESTDIR     = $$PWD/bin/debug
    OBJECTS_DIR = $$PWD/build/debug/obj
    MOC_DIR     = $$PWD/build/debug/moc
    RCC_DIR     = $$PWD/build/debug/rcc
    UI_DIR      = $$PWD/build/debug/ui
} else {
    DESTDIR     = $$PWD/bin/release
    OBJECTS_DIR = $$PWD/build/release/obj
    MOC_DIR     = $$PWD/build/release/moc
    RCC_DIR     = $$PWD/build/release/rcc
    UI_DIR      = $$PWD/build/release/ui
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
