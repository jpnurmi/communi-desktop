######################################################################
# Communi: quassel.pri
######################################################################

COMMUNI_QUASSELDIR = $$PWD/quassel/src/common

exists($$COMMUNI_QUASSELDIR) {
    DEFINES += HAVE_QUASSEL
    DEPENDPATH += $$COMMUNI_QUASSELDIR
    INCLUDEPATH += $$COMMUNI_QUASSELDIR
    QMAKE_CLEAN += $$COMMUNI_QUASSELDIR/*~

    HEADERS += $$COMMUNI_QUASSELDIR/backlogmanager.h
    HEADERS += $$COMMUNI_QUASSELDIR/bufferinfo.h
    HEADERS += $$COMMUNI_QUASSELDIR/identity.h
    HEADERS += $$COMMUNI_QUASSELDIR/ircchannel.h
    HEADERS += $$COMMUNI_QUASSELDIR/ircuser.h
    HEADERS += $$COMMUNI_QUASSELDIR/message.h
    HEADERS += $$COMMUNI_QUASSELDIR/network.h
    HEADERS += $$COMMUNI_QUASSELDIR/quassel.h
    HEADERS += $$COMMUNI_QUASSELDIR/signalproxy.h
    HEADERS += $$COMMUNI_QUASSELDIR/syncableobject.h
    HEADERS += $$COMMUNI_QUASSELDIR/types.h
    HEADERS += $$COMMUNI_QUASSELDIR/util.h

    SOURCES += $$COMMUNI_QUASSELDIR/backlogmanager.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/bufferinfo.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/identity.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/ircchannel.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/ircuser.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/message.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/network.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/quassel.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/signalproxy.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/syncableobject.cpp
    SOURCES += $$COMMUNI_QUASSELDIR/util.cpp

    win32 {
        SOURCES += $$COMMUNI_QUASSELDIR/logbacktrace_win.cpp
    }
    unix {
        SOURCES += $$COMMUNI_QUASSELDIR/logbacktrace_unix.cpp
    }
    mac {
        HEADERS += $$COMMUNI_QUASSELDIR/mac_utils.h
        SOURCES += $$COMMUNI_QUASSELDIR/mac_utils.cpp
    }
}
