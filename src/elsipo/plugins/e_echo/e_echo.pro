#! [0]
QT      +=  webkit network
TEMPLATE        = lib
CONFIG         += e_echo
INCLUDEPATH    += ../../elsipo
HEADERS         = e_echo.h
SOURCES         = e_echo.cpp
TARGET          = $$qtLibraryTarget(e_echo)
DESTDIR         = ../../bin/plugins/e_echo
#! [0]

# install
target.path = ../../bin/plugins/e_echo
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS echo.pro
sources.path = ../../plugins/e_echo
INSTALLS += target sources

symbian {
    include($$PWD/../../../symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}

maemo5: include($$PWD/../../../maemo5pkgrules.pri)
symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
