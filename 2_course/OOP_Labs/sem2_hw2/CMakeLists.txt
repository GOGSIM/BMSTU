cmake_minimum_required(VERSION 3.16)
project(sem2_hw2_xml_viewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# MOC создаёт служебный код для классов с макросом Q_OBJECT.
set(CMAKE_AUTOMOC ON)
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Xml)

# Одна библиотека позволяет приложению и тестам использовать те же классы.
add_library(xml_viewer_core
    include/XmlTreeModel.h src/XmlTreeModel.cpp
    include/MainWindow.h src/MainWindow.cpp
)
target_include_directories(xml_viewer_core PUBLIC include)
target_link_libraries(xml_viewer_core PUBLIC Qt6::Widgets Qt6::Xml)
if(MSVC)
    target_compile_options(xml_viewer_core PUBLIC /utf-8)
else()
    target_compile_options(xml_viewer_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(sem2_hw2_xml_viewer src/main.cpp)
target_link_libraries(sem2_hw2_xml_viewer PRIVATE xml_viewer_core)
# Данные копируются к exe, поэтому диалог открывается в нужном каталоге
# даже при запуске программы из другого рабочего каталога.
add_custom_command(TARGET sem2_hw2_xml_viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:sem2_hw2_xml_viewer>/data
)

include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Test)
    add_executable(xml_viewer_tests tests/XmlViewerTests.cpp)
    target_link_libraries(xml_viewer_tests PRIVATE xml_viewer_core Qt6::Test)
    target_compile_definitions(xml_viewer_tests PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
    add_test(NAME xml_viewer_tests COMMAND xml_viewer_tests -platform offscreen)
endif()

