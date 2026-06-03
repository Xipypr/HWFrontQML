cmake_minimum_required(VERSION 3.16)

project(HardwareMonitor
    VERSION 1.0.0
    LANGUAGES CXX
)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(HardwareMonitor
    hardwaresnapshot.cpp
    hardwaresnapshot.h
    hardwaredevice.h
    hardwarekind.h
    measurement.h
    metrickind.h
    lhmsnapshotparser.cpp
    lhmsnapshotparser.h
    parsedsensorvalue.cpp
    parsedsensorvalue.h
    unit.h
)

set_target_properties(HardwareMonitor
    PROPERTIES
        AUTOMOC ON
        CXX_EXTENSIONS OFF
)

target_compile_features(HardwareMonitor
    PUBLIC
        cxx_std_17
)

target_include_directories(HardwareMonitor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(HardwareMonitor
    PUBLIC
        Qt6::Core
)
