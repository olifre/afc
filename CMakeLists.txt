cmake_minimum_required(VERSION 3.20)
project(afc VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -Wpedantic)

option(BUILD_WITH_WERROR "Build with -Werror flag" OFF)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(BUILD_WITH_WERROR)
  add_compile_options(-Werror)
  message(STATUS "Compiling with -Werror...")
endif()

find_package(ZLIB REQUIRED)

add_executable(afc afc.cpp)
include_directories(afc ZLIB_INCLUDE_DIRS)
target_link_libraries(afc ZLIB::ZLIB)

install(TARGETS afc DESTINATION bin)
