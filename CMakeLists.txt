cmake_minimum_required(VERSION 3.16)

project(HWFrontQML VERSION 1.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BUILD_SHARED_LIBS "Build HWFrontQML dependencies as shared libraries" OFF)

set(HWFRONTQML_DEPLOY_AFTER_BUILD_DEFAULT ON)

option(HWFRONTQML_DEPLOY_AFTER_BUILD "Create a runnable deployment folder after building HWFrontQML release builds" ${HWFRONTQML_DEPLOY_AFTER_BUILD_DEFAULT})
set(HWFRONTQML_DEPLOY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deploy" CACHE PATH "Directory for the runnable HWFrontQML deployment")

if(HWFRONTQML_DEPLOY_AFTER_BUILD AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Disabling HWFRONTQML_DEPLOY_AFTER_BUILD: deployment is available only for Release builds")
    set(HWFRONTQML_DEPLOY_AFTER_BUILD OFF CACHE BOOL "Create a runnable deployment folder after building HWFrontQML release builds" FORCE)
endif()

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
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml LinguistTools)

if(HWFRONTQML_DEPLOY_AFTER_BUILD)
    if(NOT WIN32)
        message(FATAL_ERROR "HWFRONTQML_DEPLOY_AFTER_BUILD currently supports Windows deployment via windeployqt only")
    endif()

    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt
        HINTS "${QT_BIN_DIR}"
        REQUIRED
    )
endif()

add_subdirectory(HWConnector)
add_subdirectory(HardwareMonitorContract)

if(NOT TARGET HWConnector)
    message(FATAL_ERROR "Required target 'HWConnector' was not created by submodule 'HWConnector'")
endif()

if(NOT TARGET HardwareMonitorContract)
    message(FATAL_ERROR "Required target 'HardwareMonitorContract' was not created")
endif()

function(configure_required_dependency_linkage target_name)
    get_target_property(target_type ${target_name} TYPE)

    if(target_type STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${target_name}
            PUBLIC
                ${ARGN}
        )
        message(STATUS "Configured ${target_name} for static linkage")
    elseif(target_type STREQUAL "SHARED_LIBRARY" OR target_type STREQUAL "MODULE_LIBRARY")
        set_property(GLOBAL APPEND PROPERTY HWFRONTQML_DEPLOY_RUNTIME_TARGETS ${target_name})
        message(STATUS "Configured ${target_name} for dynamic linkage")
    else()
        message(FATAL_ERROR "Target '${target_name}' has unsupported type '${target_type}'")
    endif()
endfunction()

configure_required_dependency_linkage(HWConnector
    HWCONNECTOR_STATIC
    HWCONNECTOR_LIBRARY
)
qt_add_executable(HWFrontQML
    src/app/main.cpp
    src/app/languagemanager.cpp
    src/app/languagemanager.h

    src/core/core.cpp
    src/core/core.h
    src/core/dashboardmetricprofile.cpp
    src/core/dashboardmetricprofile.h
    src/core/metricdescriptor.h
    src/core/metricsservice.cpp
    src/core/metricsservice.h
    src/core/session.h
    src/core/sessionmanager.cpp
    src/core/sessionmanager.h
    src/core/sessionstate.h

    src/lhmparser/lhmclassification.cpp
    src/lhmparser/lhmclassification.h
    src/lhmparser/lhmsnapshotparser.cpp
    src/lhmparser/lhmsnapshotparser.h
    src/lhmparser/parsedsensorvalue.cpp
    src/lhmparser/parsedsensorvalue.h

    src/linuxadapter/linuxsnapshotparser.cpp
    src/linuxadapter/linuxsnapshotparser.h

    src/models/dashboardmetricsmodel.cpp
    src/models/dashboardmetricsmodel.h
    src/models/sessionlistmodel.cpp
    src/models/sessionlistmodel.h

    qml.qrc
    app.rc
)

qt_add_translations(HWFrontQML
    TS_FILES
        translations/HWFrontQML_ru.ts
    RESOURCE_PREFIX
        "/i18n"
)

target_include_directories(HWFrontQML
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lhmparser
        ${CMAKE_CURRENT_SOURCE_DIR}/src/linuxadapter
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

target_link_libraries(HWFrontQML
    PRIVATE
        HWConnector
        HardwareMonitorContract
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
        Qt6::Core
)

if(WIN32)
    set_target_properties(HWFrontQML PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(HWFRONTQML_DEPLOY_AFTER_BUILD)
    add_custom_command(TARGET HWFrontQML POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${HWFRONTQML_DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${HWFRONTQML_DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:HWFrontQML>"
            "${HWFRONTQML_DEPLOY_DIR}/$<TARGET_FILE_NAME:HWFrontQML>"
        COMMENT "Preparing HWFrontQML deployment folder at ${HWFRONTQML_DEPLOY_DIR}"
        VERBATIM
    )

    get_property(HWFRONTQML_DEPLOY_RUNTIME_TARGETS GLOBAL PROPERTY HWFRONTQML_DEPLOY_RUNTIME_TARGETS)
    foreach(runtime_target IN LISTS HWFRONTQML_DEPLOY_RUNTIME_TARGETS)
        add_custom_command(TARGET HWFrontQML POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${runtime_target}>"
                "${HWFRONTQML_DEPLOY_DIR}/$<TARGET_FILE_NAME:${runtime_target}>"
            COMMENT "Copying ${runtime_target} runtime to ${HWFRONTQML_DEPLOY_DIR}"
            VERBATIM
        )
    endforeach()

    add_custom_command(TARGET HWFrontQML POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/qml"
            --release
            "${HWFRONTQML_DEPLOY_DIR}/$<TARGET_FILE_NAME:HWFrontQML>"
        COMMENT "Deploying Qt runtime to ${HWFRONTQML_DEPLOY_DIR}"
        VERBATIM
    )
endif()

if(ANDROID)
    set_property(TARGET HWFrontQML PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()
