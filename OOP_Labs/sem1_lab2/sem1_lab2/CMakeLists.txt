add_executable(sem1_lab2
    Exceptions.cpp
    Main.cpp
    Matrix.cpp
    Vector.cpp
)

target_include_directories(sem1_lab2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
    target_compile_options(sem1_lab2 PRIVATE /W4 /permissive-)
else()
    target_compile_options(sem1_lab2 PRIVATE -Wall -Wextra -Wpedantic)
endif()