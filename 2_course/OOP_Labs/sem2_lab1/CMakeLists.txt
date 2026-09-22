cmake_minimum_required(VERSION 3.16)

project(sem2_lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sem2_lab1
    src/main.cpp
    src/Mesh.cpp
    src/AneuMeshLoader.cpp
)

target_include_directories(sem2_lab1 PRIVATE include)