cmake_minimum_required(VERSION 3.16)

project(HWFrontQML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/HWConnector/CMakeLists.txt")
    add_subdirectory(HWConnector)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../HWConnector/CMakeLists.txt")
    add_subdirectory(../HWConnector HWConnector-build)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/DeviceBuilder/CMakeLists.txt")
    add_subdirectory(DeviceBuilder)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../DeviceBuilder/CMakeLists.txt")
    add_subdirectory(../DeviceBuilder DeviceBuilder-build)
endif()

qt_add_executable(HWFrontQML
    src/app/main.cpp

    src/core/core.cpp
    src/core/core.h
    src/core/metricdescriptor.h
    src/core/metricsservice.cpp
    src/core/metricsservice.h
    src/core/session.h
    src/core/sessionmanager.cpp
    src/core/sessionmanager.h
    src/core/sessionstate.h

    src/models/dashboardmetricsmodel.cpp
    src/models/dashboardmetricsmodel.h
    src/models/sessionlistmodel.cpp
    src/models/sessionlistmodel.h

    qml.qrc
    app.rc
)

target_include_directories(HWFrontQML
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

target_link_libraries(HWFrontQML
    PRIVATE
        HWConnector
        DeviceBuilder
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
        Qt6::Core
)

if(ANDROID)
    set_property(TARGET HWFrontQML PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()
