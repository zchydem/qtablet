# #####################################################################
# Automatically generated by qmake (2.01a) Sun Dec 14 00:50:36 2008
# #####################################################################
include(../qtablet.pri)
TEMPLATE = lib
TARGET = QTabletGui
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += dll
OBJECTS_DIR = $$BUILDDIR/gui/obj
MOC_DIR = $$BUILDDIR/gui/moc
DESTDIR = $$BUILDDIR/gui/lib

# Input
HEADERS += pannableview.hh \
    imageitem.hh \
    abstractitem.hh \
    globals.hh \
    pannableviewitem.hh \
    labelitem.hh
SOURCES += pannableview.cc \
    imageitem.cc \
    abstractitem.cc \
    pannableviewitem.cc \
    labelitem.cc

target.path = $$INSTALLDIR/lib
headers.files = $$HEADERS
headers.path = $$INSTALLDIR/include
INSTALLS += target \
    headers
