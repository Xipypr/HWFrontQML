QT += quick

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        core.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-HWConnector-Desktop_Qt_6_2_3_MinGW_64_bit-Release/release/ -lHWConnector
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-HWConnector-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/ -lHWConnector

INCLUDEPATH += $$PWD/../HWConnector
DEPENDPATH += $$PWD/../HWConnector

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-DeviceBuilder-Desktop_Qt_6_2_3_MinGW_64_bit-Release/release/ -lDeviceBuilder
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-DeviceBuilder-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/ -lDeviceBuilder

INCLUDEPATH += $$PWD/../DeviceBuilder/src
DEPENDPATH += $$PWD/../DeviceBuilder/src

HEADERS += \
    core.h
