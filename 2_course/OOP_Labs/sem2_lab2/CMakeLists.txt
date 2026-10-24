# Минимальная версия CMake, которая нужна для сборки проекта.
cmake_minimum_required(VERSION 3.16)

# Создаём C++-проект с именем sem2_lab2_calculator.
project(sem2_lab2_calculator LANGUAGES CXX)

# Используем стандарт C++17 без нестандартных расширений компилятора.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# AUTOMOC запускает Meta-Object Compiler для классов с макросом Q_OBJECT.
set(CMAKE_AUTOMOC ON)

# Сначала ищем доступную версию Qt, затем подключаем её модуль Widgets.
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Из перечисленных исходных файлов будет собрана исполняемая программа.
add_executable(sem2_lab2_calculator
    include/CalculatorWindow.h
    src/CalculatorWindow.cpp
    src/main.cpp
)

# PRIVATE означает, что каталог include нужен только этой цели сборки.
target_include_directories(sem2_lab2_calculator PRIVATE include)

# Подключаем библиотеку Qt Widgets к исполняемому файлу.
target_link_libraries(sem2_lab2_calculator
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)

# Для компилятора Microsoft явно включаем чтение исходников в UTF-8.
if(MSVC)
    target_compile_options(sem2_lab2_calculator PRIVATE /utf-8)
endif()
