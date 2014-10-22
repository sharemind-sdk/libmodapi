#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8.12")
PROJECT("SHAREMIND_LIBMODAPI" "C")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(SharemindVmGlobals REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}
                    ${SharemindVmGlobals_INCLUDE_DIRS})

ADD_COMPILE_OPTIONS(
    "-std=c99" "-O2" "-fwrapv" "-Wall" "-Wextra" "-Wunused" "-Wabi"
    "-Wstrict-aliasing=1" "-Wbad-function-cast" "-Wformat" "-Wswitch-default"
    "-Wfloat-equal" "-Wshadow" "-Wpointer-arith" "-Wcast-qual"
    "-Wstrict-prototypes" "-Wconversion" "-Wsign-conversion" "-Wunused-macros"
    "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
    "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
    "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
    "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)

INCLUDE(CheckCCompilerFlag)
FUNCTION(CHECK_ADD_COMPILER_FLAG flag)
    STRING(SUBSTRING "${flag}" 1 -1 FlagName)
    CHECK_C_COMPILER_FLAG("${flag}" C_COMPILER_HAS_${FlagName}_FLAG)
    IF(C_COMPILER_HAS_${FlagName}_FLAG)
        ADD_COMPILE_OPTIONS("${flag}")
    ENDIF()
ENDFUNCTION()
CHECK_ADD_COMPILER_FLAG("-Weverything")
CHECK_ADD_COMPILER_FLAG("-Wlogical-op")

SET(LIBMODAPI_INSTALL_DEFINITIONS "-D__STDC_LIMIT_MACROS"
                                  "-D_XOPEN_SOURCE=700")
ADD_DEFINITIONS(${SharemindVmGlobals_DEFINITIONS}
                ${LIBMODAPI_INSTALL_DEFINITIONS})



################################################################################
# libmodapi
################################################################################

SET(SHAREMIND_LIBMODAPI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/libmodapi.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/libmodapicxx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/api.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/api_0x1.h"
)

FILE(GLOB_RECURSE SHAREMIND_LIBMODAPI_HEADERS_P
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
LIST(REMOVE_ITEM SHAREMIND_LIBMODAPI_HEADERS_P ${SHAREMIND_LIBMODAPI_HEADERS})

IF(NOT SHAREMIND_LIBMODAPI_LINKING_MODE)
    SET(SHAREMIND_LIBMODAPI_LINKING_MODE SHARED)
ENDIF()

FILE(GLOB_RECURSE SHAREMIND_LIBMODAPI_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
ADD_LIBRARY("libmodapi" ${SHAREMIND_LIBMODAPI_LINKING_MODE}
            ${SHAREMIND_LIBMODAPI_SOURCES}
            ${SHAREMIND_LIBMODAPI_HEADERS}
            ${SHAREMIND_LIBMODAPI_HEADERS_P}
)
SET(LIBMODAPI_VERSION "0.2.0.0")
SET(LIBMODAPI_COMPAT_VERSION "0.2.0.0")
STRING(REGEX REPLACE "(([^\\.]+\\.[^\\.]+)\\.).*" "\\2" LIBMODAPI_SOVERSION
                     "${LIBMODAPI_VERSION}")
SET_TARGET_PROPERTIES("libmodapi" PROPERTIES
                      OUTPUT_NAME "sharemind_modapi"
                      VERSION "${LIBMODAPI_VERSION}"
                      SOVERSION "${LIBMODAPI_SOVERSION}")
TARGET_LINK_LIBRARIES("libmodapi" "-ldl")

INSTALL(TARGETS "libmodapi"
        LIBRARY DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBMODAPI_HEADERS}
        DESTINATION "include/sharemind/libmodapi")

# Install cmake files:
FILE(GLOB_RECURSE sharemind_libmodapi_in_files
     "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_sharemind_libmodapi_in_files_in_IDE"
                  SOURCES ${sharemind_libmodapi_in_files})
SET(LIBMODAPI_INSTALL_LINK_LIBRARIES "-L${CMAKE_INSTALL_PREFIX}/lib"
                                     "-lsharemind_modapi")
SET(LIBMODAPI_INSTALL_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibmodapiConfig.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfig.cmake"
               @ONLY)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibmodapiConfigVersion.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfigVersion.cmake" @ONLY)
INSTALL(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfigVersion.cmake"
        DESTINATION "lib/SharemindLibmodapi")
