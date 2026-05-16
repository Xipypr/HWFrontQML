cmake_minimum_required(VERSION 3.16)

project(HWFrontQML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Git REQUIRED)

function(initialize_required_submodule submodule_dir)
    set(submodule_path "${CMAKE_CURRENT_SOURCE_DIR}/${submodule_dir}")

    if(NOT EXISTS "${submodule_path}/CMakeLists.txt")
        message(STATUS "Initializing required submodule: ${submodule_dir}")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${submodule_dir}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE submodule_update_result
        )

        if(NOT submodule_update_result EQUAL 0)
            message(FATAL_ERROR "Failed to initialize required submodule '${submodule_dir}'")
        endif()
    endif()

    if(NOT EXISTS "${submodule_path}/CMakeLists.txt")
        message(FATAL_ERROR "Required submodule '${submodule_dir}' was not found or does not contain CMakeLists.txt")
    endif()
endfunction()

initialize_required_submodule(HWConnector)
initialize_required_submodule(DeviceBuilder)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)

add_subdirectory(HWConnector)
add_subdirectory(DeviceBuilder)

if(NOT TARGET HWConnector)
    message(FATAL_ERROR "Required target 'HWConnector' was not created by submodule 'HWConnector'")
endif()

if(NOT TARGET DeviceBuilder)
    message(FATAL_ERROR "Required target 'DeviceBuilder' was not created by submodule 'DeviceBuilder'")
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
