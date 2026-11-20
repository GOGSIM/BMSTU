cmake_minimum_required(VERSION 3.16)
project(sem2_lab34_text_editor LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Xml)
add_library(editor_core
    include/DocumentEditor.h src/DocumentEditor.cpp
    include/SyntaxSettings.h src/SyntaxSettings.cpp
    include/SyntaxHighlighter.h src/SyntaxHighlighter.cpp
    include/MainWindow.h src/MainWindow.cpp
    include/UnsavedDocumentsDialog.h src/UnsavedDocumentsDialog.cpp)
target_include_directories(editor_core PUBLIC include)
target_link_libraries(editor_core PUBLIC Qt6::Widgets Qt6::Xml)
if(MSVC)
    target_compile_options(editor_core PUBLIC /utf-8)
else()
    target_compile_options(editor_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(sem2_lab34_text_editor WIN32 src/main.cpp resources/editor.qrc)
target_link_libraries(sem2_lab34_text_editor PRIVATE editor_core)
configure_file(settings/syntaxes.xml settings/syntaxes.xml COPYONLY)
include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Test)
    add_executable(editor_tests tests/EditorTests.cpp resources/editor.qrc)
    target_link_libraries(editor_tests PRIVATE editor_core Qt6::Test)
    target_compile_definitions(editor_tests PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    if(WIN32 AND TARGET Qt6::QOffscreenIntegrationPlugin)
        # windeployqt размещает qwindows для приложения; тестам также нужен
        # offscreen-плагин рядом с локальными DLL Qt.
        add_custom_command(TARGET editor_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:editor_tests>/platforms
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt6::QOffscreenIntegrationPlugin> $<TARGET_FILE_DIR:editor_tests>/platforms)
    endif()
    add_test(NAME editor_tests COMMAND editor_tests -platform offscreen -o -,txt)
    set_tests_properties(editor_tests PROPERTIES TIMEOUT 45)
endif()
