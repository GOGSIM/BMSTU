cmake_minimum_required(VERSION 3.16)

project(sem2_lab2_calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(sem2_lab2_calculator
    include/CalculatorWindow.h
    src/CalculatorWindow.cpp
    src/main.cpp
)

target_include_directories(sem2_lab2_calculator PRIVATE include)

target_link_libraries(sem2_lab2_calculator
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)

if(MSVC)
    target_compile_options(sem2_lab2_calculator PRIVATE /utf-8)
endif()

