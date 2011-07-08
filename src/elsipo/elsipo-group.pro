#! [0]
TEMPLATE    = subdirs
SUBDIRS	    = elsipo \
              plugins/e_echo \
              plugins/e_pjsip
CONFIG -= DEBUG
CONFIG -= debug
#! [0]

# install
target.path = bin/elsipo
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS elsipo.pro
sources.path = elsipo
INSTALLS += target sources

