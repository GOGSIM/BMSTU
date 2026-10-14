# Требуем CMake версии 3.16 или новее.
cmake_minimum_required(VERSION 3.16)

# Задаём имя проекта.
project(sem2_hw1)

# Собираем программу по стандарту C++17.
set(CMAKE_CXX_STANDARD 17)
# Запрещаем CMake незаметно перейти на более старый стандарт.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создаём один исполняемый файл из трёх файлов с реализацией.
add_executable(sem2_hw1
    src/main.cpp
    src/SequentialPart.cpp
    src/Warehouse.cpp
)

# PRIVATE означает, что каталог include нужен только этой программе.
target_include_directories(sem2_hw1 PRIVATE include)


