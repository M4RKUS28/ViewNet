QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    viewnet.cpp

HEADERS += \
    viewnet.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Release/release/ -lGenNet
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Debug/debug/ -lGenNet
else:unix: LIBS += -L$$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Release/ -lGenNet

INCLUDEPATH += $$PWD/../GenNet
DEPENDPATH += $$PWD/../GenNet

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Release/release/libGenNet.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Debug/debug/libGenNet.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Release/release/GenNet.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Debug/debug/GenNet.lib
else:unix: PRE_TARGETDEPS += $$PWD/../build-GenNet-Desktop_Qt_6_5_0_MinGW_64_bit-Release/libGenNet.a

