######################################################################
# Automatically generated by qmake (2.01a) Sun Dec 21 21:50:42 2008
######################################################################
include(qtablet.pri)

TEMPLATE = subdirs
CONFIG  += ordered
SUBDIRS = core \
          gui \
          wall



images.files        = images
images.path         = $$INSTALLDIR
configuration.files = config
configuration.path  = $$INSTALLDIR
INSTALLS += images configuration
