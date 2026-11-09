cmake_minimum_required(VERSION 3.16)
project(DeviceLink VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network SerialPort)

qt_add_executable(DeviceLink
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/TcpSession.h
    src/TcpSession.cpp
    src/FrameParser.h
    src/FrameParser.cpp
    src/LocalEchoServer.h
    src/LocalEchoServer.cpp
)

target_include_directories(DeviceLink PRIVATE src)
target_link_libraries(DeviceLink PRIVATE Qt6::Widgets Qt6::Network Qt6::SerialPort)

if (WIN32)
    set_target_properties(DeviceLink PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
