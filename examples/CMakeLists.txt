##############################################################################
## Required CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

##############################################################################
## Project name: sets ${PROJECT_NAME}
project("platypus-phyloinformary-examples" CXX)

##############################################################################
## Disallow in-source build
if ( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
  message(FATAL_ERROR
"In-source builds are not allowed."
"Please create a directory and run cmake from there, passing the path to this"
"source directory as the last argument."
"This process created the file `CMakeCache.txt' and the directory `CMakeFiles'."
"Please delete them.")
endif()

##############################################################################
## Compiler Setup
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
IF (CMAKE_BUILD_TYPE MATCHES debug)
    ADD_DEFINITIONS(-O0 -Wall -g)
ELSE()
    ADD_DEFINITIONS(-O3 -Wall)
ENDIF()

##############################################################################
## Installation path
## Install prefix is sub-directory `install` of top build directory unless
## explicity specified as otherwise by using `-DCMAKE_INSTALL_PREFIX=/foo/bar`
## option on cmake.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install path" FORCE )
endif()

# set up include-directories
# include_directories(
#   "${PROJECT_SOURCE_DIR}"   # to find foo/foo.h
#   "${PROJECT_BINARY_DIR}")  # to find foo/config.h

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
if (CMAKE_BUILD_TYPE MATCHES debug)
    add_definitions(-O0 -Wall -g)
else()
    add_definitions(-O3 -Wall)
endif()

##############################################################################
## Include directories
INCLUDE_DIRECTORIES(
  "${PROJECT_SOURCE_DIR}/../include" # for <platypus/platypus.hpp>
  "${PROJECT_BINARY_DIR}"            # to find foo/config.h
    )

##############################################################################
## Sources
add_subdirectory(src/sim-coalescent-trees)
