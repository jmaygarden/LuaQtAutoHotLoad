cmake_minimum_required(VERSION 3.1.0)

project(LuaQtAutoHotLoad)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Populate a CMake variable with the sources
set(LuaQtAutoHotLoad_SRCS
    lauxlib.h
    lua.h
    lua.hpp
    lua5.1.lib
    luaconf.h
    lualib.h
    MainWindow.h
    MainWindow.cpp
    main.cpp
    SettingsWindow.h
    SettingsWindow.cpp
    LuaReplWidget.h
    LuaReplWidget.cpp
    sol.hpp
)

# Tell CMake to create the helloworld executable
add_executable(LuaQtAutoHotLoad WIN32 ${LuaQtAutoHotLoad_SRCS})

add_library(Lua STATIC IMPORTED)
set_target_properties(Lua
  PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lua5.1.lib
)

link_directories(${PROJECT_SOURCE_DIR})
target_link_libraries(LuaQtAutoHotLoad Qt5::Core Qt5::Gui Qt5::Widgets Lua)
