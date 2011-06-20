QT      +=  webkit network
HEADERS    = elsipo.h \
             elsipointerface.h \
    elsipointerface.h
SOURCES    = elsipo.cpp \
             main.cpp

TARGET     = elsipo
win32 {
    CONFIG(debug, release|debug):DESTDIR = ../bin/debug/
    CONFIG(release, release|debug):DESTDIR = ../bin/release/
} else {
    DESTDIR    = ../bin/
}

# install
target.path = ../bin
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS elsipowindow.pro
sources.path = ../elsipo
INSTALLS += target sources

symbian: include($$PWD/../../../symbianpkgrules.pri)
maemo5: include($$PWD/../../../maemo5pkgrules.pri)

symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
