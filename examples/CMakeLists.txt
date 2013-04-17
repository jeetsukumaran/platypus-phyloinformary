# Required CMake
cmake_minimum_required(VERSION 2.8)

# Project name: sets ${PROJECT_NAME}
project("platypus-phyloinformary-examples" CXX)

# Versioning
set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 1)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION
  ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION})

# disallow in-source build
if ( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
  message(FATAL_ERROR
"In-source builds are not allowed."
"Please create a directory and run cmake from there, passing the path to this"
"source directory as the last argument."
"This process created the file `CMakeCache.txt' and the directory `CMakeFiles'."
"Please delete them.")
endif()

# CXX="/usr/platform/bin/g++" cmake -DENABLE_INSTALL_PREFIX=true -DCMAKE_INSTALL_PREFIX="/custom/install/prefix"
option(ENABLE_INSTALL_PREFIX "Install build targets to system (path given by '-DCMAKE_INSTALL_PREFIX' or '${CMAKE_INSTALL_PREFIX}' if not specified)." OFF)
if ( ENABLE_INSTALL_PREFIX )
    set (CMAKE_INSTALL_PREFIX installed CACHE PATH "Installation root")
else()
    set (CMAKE_INSTALL_PREFIX installed CACHE PATH "Installation root" FORCE)
endif()

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
    set(DEF_INSTALL_CMAKE_DIR lib/CMake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# # Make relative paths absolute (needed later on)
# foreach(p LIB BIN INCLUDE CMAKE)
#   set(var INSTALL_${p}_DIR)
#   if(NOT IS_ABSOLUTE "${${var}}")
#     set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
#   endif()
# endforeach()

# set up include-directories
include_directories(
  "${PROJECT_SOURCE_DIR}"   # to find foo/foo.h
  "${PROJECT_BINARY_DIR}")  # to find foo/config.h

# standard flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
if (CMAKE_BUILD_TYPE MATCHES debug)
    add_definitions(-O0 -Wall -g)
else()
    add_definitions(-O3 -Wall)
endif()

# add main library to includes
include_directories(../src)

# Add sub-directories
add_subdirectory(sim-coalescent-trees)
