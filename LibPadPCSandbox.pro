TEMPLATE = app
CONFIG += console c++17

include($$PWD/Dependencies/Dependencies.pri)

SOURCES += \
    main.cpp \
    padPhy.cpp \
    test_main.c

HEADERS += \
    LFrameworkConfig.h

LIBS += -lUser32 -lAdvapi32 -lOle32 -lWinusb -lSetupAPI


#pragma comment(lib, )
#pragma comment(lib,
#pragma comment(lib, )
#pragma comment(lib, )
#pragma comment(lib, )
