cmake_minimum_required(VERSION 2.6)

project(ThemeParkReader)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules/modules/")
set(Boost_DEBUG on)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED filesystem program_options) 
find_package(FreeImage REQUIRED)

add_subdirectory("ThemeParkReader")





