QT += quick

TEMPLATE = app
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        core.cpp \
        dashboardmetricsmodel.cpp \
        main.cpp \
        metricsservice.cpp \
        sessionlistmodel.cpp \
        sessionmanager.cpp

RESOURCES += qml.qrc

RC_FILE = app.rc

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-HWConnector-Desktop_Qt_6_2_3_MinGW_64_bit-Release/release/ -lHWConnector
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-HWConnector-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/ -lHWConnector

#android:CONFIG(release, debug|release): LIBS += -L../build-HWConnector-Android_Qt_6_2_3_Clang_armeabi_v7a-Release/ -lHWConnector
#else:android:CONFIG(debug, debug|release): LIBS += -L../build-HWConnector-Android_Qt_6_2_3_Clang_armeabi_v7a_1e13e7-Debug/ -lHWConnector

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build/lib/ -lHWConnector
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build/lib/ -lHWConnector

android:CONFIG(release, debug|release): LIBS += "-L$$PWD/../build/lib/" -lHWConnector
else:android:CONFIG(debug, debug|release): LIBS += "-L$$PWD/../build/lib/" -lHWConnector

INCLUDEPATH += $$PWD/../HWConnector
DEPENDPATH += $$PWD/../HWConnector

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-DeviceBuilder-Desktop_Qt_6_2_3_MinGW_64_bit-Release/release/ -lDeviceBuilder
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-DeviceBuilder-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/debug/ -lDeviceBuilder

#android:CONFIG(release, debug|release): LIBS += -L../build-DeviceBuilder-Android_Qt_6_2_3_Clang_armeabi_v7a-Release/ -lDeviceBuilder
#else:android:CONFIG(debug, debug|release): LIBS += -L../build-DeviceBuilder-Android_Qt_6_2_3_Clang_armeabi_v7a_1e13e7-Debug/ -lDeviceBuilder

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../build/lib// -lDeviceBuilder
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../build/lib/ -lDeviceBuilder

android:CONFIG(release, debug|release): LIBS += "-L$$PWD/../build/lib/" -lDeviceBuilder
else:android:CONFIG(debug, debug|release): LIBS += "-L$$PWD/../build/lib/" -lDeviceBuilder

INCLUDEPATH += $$PWD/../DeviceBuilder/src
DEPENDPATH += $$PWD/../DeviceBuilder/src

HEADERS += \
    core.h \
    dashboardmetricsmodel.h \
    metricdescriptor.h \
    metricsservice.h \
    session.h \
    sessionlistmodel.h \
    sessionmanager.h \
    sessionstate.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/xml/qtprovider_paths.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
