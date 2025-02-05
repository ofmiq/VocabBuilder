cmake_minimum_required(VERSION 3.16)

project(VocabBuilder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Concurrent)

set(CMAKE_AUTOUIC_SEARCH_PATHS resources)

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/FileParser.cpp
    src/FileProcessingService.cpp
    src/WordDefinitionFetcher.cpp
    include/MainWindow.h
    include/FileParser.h
    include/IFileParser.h
    include/FileProcessingService.h
    include/WordDefinitionFetcher.h
    resources/MainWindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VocabBuilder
        WIN32
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(VocabBuilder
        WIN32
        ${PROJECT_SOURCES}
    )
endif()

target_include_directories(VocabBuilder PRIVATE include)

target_link_libraries(VocabBuilder PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(VocabBuilder)
endif()
