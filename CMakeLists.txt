cmake_minimum_required(VERSION 3.14)

set(CMAKE_PREFIX_PATH "C:/Qt/5.15.0/msvc2019_64/lib/cmake")
set(Qt5Widgets_DIR "C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5Widgets")
set(Qt5_DIR "C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5")

project(War LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_USE_QTXML TRUE)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(${Qt5Widgets_INCLUDES})

file(GLOB CPPS "*.cpp")

add_definitions(${Qt5Widgets_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")


add_executable(War ${CPPS} pictures.qrc)
target_link_libraries(War Qt5::Widgets)
target_link_libraries(War Qt5::Core)
target_link_libraries(War Qt5::Gui)