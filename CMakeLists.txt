##############################################################################
## Required CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

##############################################################################
## Project name: sets ${PROJECT_NAME}
project("platypus-phyloinformary" CXX)

##############################################################################
## Versioning
SET(PROJECT_MAJOR_VERSION 0)
SET(PROJECT_MINOR_VERSION 1)
SET(PROJECT_PATCH_VERSION 0)
SET(PROJECT_VERSION
  ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION})

##############################################################################
## Disallow in-source build
IF( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
  MESSAGE(FATAL_ERROR
"In-source builds are not allowed."
"Please create a directory and run cmake from there, passing the path to this"
"source directory as the last argument."
"This process created the file `CMakeCache.txt' and the directory `CMakeFiles'."
"Please delete them.")
ENDIF()

##############################################################################
## Install paths
set(INSTALL_LIB_DIR lib CACHE PATH "Installation sub-directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation sub-directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
IF (WIN32 AND NOT CYGWIN)
  SET(DEF_INSTALL_CMAKE_DIR CMake)
ELSE()
  SET(DEF_INSTALL_CMAKE_DIR lib/CMake/${PROJECT_NAME})
ENDIF()
SET(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

##############################################################################
## Compiler Setup
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
IF (CMAKE_BUILD_TYPE MATCHES debug)
    ADD_DEFINITIONS(-O0 -Wall -g)
ELSE()
    ADD_DEFINITIONS(-O3 -Wall)
ENDIF()

##############################################################################
## Include directories
INCLUDE_DIRECTORIES(
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_BINARY_DIR}"   # to find foo/config.h
    )

##############################################################################
## Testing
ENABLE_TESTING ()
ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
