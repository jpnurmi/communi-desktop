######################################################################
# Communi
######################################################################

QUASSELDIR = $$PWD/3rdparty/src/common

exists($$QUASSELDIR) {
    TEMPLATE = lib
    COMMUNI += core
    CONFIG += communi_plugin

    HEADERS += $$PWD/quasselplugin.h
    SOURCES += $$PWD/quasselplugin.cpp

    include(protocol/quasselprotocol.pri)
}
