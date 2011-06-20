#! [0]
QT      +=  webkit network
TEMPLATE        = lib
CONFIG         += e_pjsip
INCLUDEPATH    += ../../elsipo /usr/local/include
HEADERS         = e_pjsip.h \
    pjsipcb.h
SOURCES         = e_pjsip.cpp \
    pjsipcb.cpp
TARGET          = $$qtLibraryTarget(e_pjsip)
DESTDIR         = ../../bin/plugins/e_pjsip
macx-g++:LIBS += -lg7221codec-i386-apple-darwin10.7.0 \
    -lgsmcodec-i386-apple-darwin10.7.0 \
    -lilbccodec-i386-apple-darwin10.7.0 \
    -lmilenage-i386-apple-darwin10.7.0 \
    -lpj-i386-apple-darwin10.7.0 \
    -lpjlib-util-i386-apple-darwin10.7.0 \
    -lpjmedia-audiodev-i386-apple-darwin10.7.0 \
    -lpjmedia-codec-i386-apple-darwin10.7.0 \
    -lpjmedia-i386-apple-darwin10.7.0 \
    -lpjnath-i386-apple-darwin10.7.0 \
    -lpjsip-i386-apple-darwin10.7.0 \
    -lpjsip-simple-i386-apple-darwin10.7.0 \
    -lpjsip-ua-i386-apple-darwin10.7.0 \
    -lpjsua-i386-apple-darwin10.7.0 \
    -lportaudio-i386-apple-darwin10.7.0 \
    -lresample-i386-apple-darwin10.7.0 \
    -lspeex-i386-apple-darwin10.7.0 \
    -lsrtp-i386-apple-darwin10.7.0 \
    -lm \
    -lpthread \
    -lssl \
    -lcrypto \
    -framework CoreAudio \
    -framework AudioToolbox \
    -framework AudioUnit \
    -framework CoreFoundation \
    -framework CoreServices

#! [0]

# install
target.path = ../../bin/plugins/e_pjsip
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS e_pjsip.pro
sources.path = ../../plugins/e_pjsip
INSTALLS += target sources

symbian {
    include($$PWD/../../../symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}

maemo5: include($$PWD/../../../maemo5pkgrules.pri)
symbian: warning(This example might not fully work on Symbian platform)
maemo5: warning(This example might not fully work on Maemo platform)
simulator: warning(This example might not fully work on Simulator platform)
