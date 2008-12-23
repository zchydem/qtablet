INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(animation.pri)

SOURCES +=  $$PWD/qeasingcurve.cpp \
            $$PWD/easing.cpp
HEADERS +=  $$PWD/qeasingcurve.h \
            $$PWD/common.h

DEFINES     += QT_EXPERIMENTAL_SOLUTION
DEFINES     += QT_BUILD_ANIMATION_LIB

