cmake_minimum_required(VERSION 3.16)

project(VocabBuilder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Concurrent)

set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    FileParser.h
    FileParser.cpp
    IFileParser.h
    FileProcessingService.h
    FileProcessingService.cpp
    WordDefinitionFetcher.h
    WordDefinitionFetcher.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VocabBuilder
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(VocabBuilder
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(VocabBuilder PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(VocabBuilder)
endif()
