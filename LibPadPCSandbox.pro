TEMPLATE = app
CONFIG += console c++17

include($$PWD/Dependencies/Dependencies.pri)

SOURCES += \
    main.c \
    padPhy.cpp

HEADERS += \
    LFrameworkConfig.h

LIBS += -lUser32 -lAdvapi32 -lOle32 -lWinusb -lSetupAPI

