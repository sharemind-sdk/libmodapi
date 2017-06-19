#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8.12")
PROJECT("SHAREMIND_LIBMODAPI" "C")

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(LIBMODAPI_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}.0")
SET(LIBMODAPI_COMPAT_VERSION "${LIBMODAPI_VERSION}")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCHeaders REQUIRED)
INCLUDE_DIRECTORIES(${SharemindCHeaders_INCLUDE_DIRS})

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
CHECK_ADD_COMPILER_FLAG("-Wno-padded")

SET(LIBMODAPI_INSTALL_DEFINITIONS "-D__STDC_LIMIT_MACROS"
                                  "-D_XOPEN_SOURCE=700")
ADD_DEFINITIONS(${SharemindCHeaders_DEFINITIONS}
                ${LIBMODAPI_INSTALL_DEFINITIONS})



################################################################################
# libmodapi
################################################################################

SET(SHAREMIND_LIBMODAPI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/libmodapi.h")
SET(SHAREMIND_LIBMODAPI_API_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/api.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/api_0x1.h"
)

FILE(GLOB_RECURSE SHAREMIND_LIBMODAPI_HEADERS_P
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
LIST(REMOVE_ITEM SHAREMIND_LIBMODAPI_HEADERS_P
    ${SHAREMIND_LIBMODAPI_HEADERS}
    ${SHAREMIND_LIBMODAPI_API_HEADERS})

IF(NOT SHAREMIND_LIBMODAPI_LINKING_MODE)
    SET(SHAREMIND_LIBMODAPI_LINKING_MODE SHARED)
ENDIF()

FILE(GLOB_RECURSE SHAREMIND_LIBMODAPI_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
ADD_LIBRARY("libmodapi" ${SHAREMIND_LIBMODAPI_LINKING_MODE}
            ${SHAREMIND_LIBMODAPI_SOURCES}
            ${SHAREMIND_LIBMODAPI_HEADERS}
            ${SHAREMIND_LIBMODAPI_API_HEADERS}
            ${SHAREMIND_LIBMODAPI_HEADERS_P}
)
STRING(REGEX REPLACE "(([^\\.]+\\.[^\\.]+)\\.).*" "\\2" LIBMODAPI_SOVERSION
                     "${LIBMODAPI_VERSION}")
SET_TARGET_PROPERTIES("libmodapi" PROPERTIES
                      OUTPUT_NAME "sharemind_modapi"
                      VERSION "${LIBMODAPI_VERSION}"
                      SOVERSION "${LIBMODAPI_SOVERSION}")
TARGET_LINK_LIBRARIES("libmodapi" ${SharemindCHeaders_LIBRARIES} "-ldl")

INSTALL(TARGETS "libmodapi"
        COMPONENT lib
        LIBRARY DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBMODAPI_HEADERS}
        DESTINATION "include/sharemind/libmodapi"
        COMPONENT dev)

INSTALL(FILES ${SHAREMIND_LIBMODAPI_API_HEADERS}
        DESTINATION "include/sharemind/libmodapi"
        COMPONENT api-dev)

# Install cmake files:
FILE(GLOB_RECURSE sharemind_libmodapi_in_files
     "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_sharemind_libmodapi_in_files_in_IDE"
                  SOURCES ${sharemind_libmodapi_in_files})
SET(LIBMODAPI_INSTALL_LINK_LIBRARIES ${SharemindCHeaders_LIBRARIES}
                                     "-L${CMAKE_INSTALL_PREFIX}/lib"
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
        DESTINATION "lib/SharemindLibmodapi"
        COMPONENT common-dev)


# Packaging:
SET(CPACK_PACKAGE_VENDOR "Cybernetica AS")
SET(CPACK_PACKAGE_CONTACT "Sharemind packaging <sharemind-packaging@cyber.ee>")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_NAME "libsharemind-modapi")
SET(CPACK_DEBIAN_PACKAGE_RELEASE "cyber1")
SET(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
SET(CPACK_DEB_COMPONENT_INSTALL "ON")
SET(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

SET(CPACK_DEBIAN_LIB_PACKAGE_NAME "libsharemind-modapi")
SET(CPACK_COMPONENT_COMMON-DEV_DESCRIPTION
    "Sharemind Module API library common development files")
SET(CPACK_COMPONENT_API-DEV_DESCRIPTION
    "Sharemind Module API library API headers")
SET(CPACK_COMPONENT_DEV_DESCRIPTION
    "Sharemind Module API library development headers")
SET(CPACK_COMPONENT_LIB_DESCRIPTION "Sharemind Module API library")
SET(CPACK_DEBIAN_API-DEV_PACKAGE_DEPENDS
    "libsharemind-modapi-common-dev (= ${LIBMODAPI_VERSION}), libsharemind-cheaders-dev (>= 0.1.0), libc6-dev (>= 2.19), libgcc-4.8-dev | libgcc-4.9-dev | libgcc-5-dev | libgcc-6-dev | libgcc-7-dev")
SET(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS
    "libsharemind-modapi (= ${LIBMODAPI_VERSION}), ${CPACK_DEBIAN_API-DEV_PACKAGE_DEPENDS}, libsharemind-modapi-api-dev (= ${LIBMODAPI_VERSION})")
SET(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libc6 (>= 2.19)")
SET(CPACK_DEBIAN_COMMON-DEV_PACKAGE_SECTION "libdevel")
SET(CPACK_DEBIAN_API-DEV_PACKAGE_SECTION "libdevel")
SET(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
SET(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")
SET(CPACK_DEBIAN_COMMON-DEV_PACKAGE_ARCHITECTURE "any")
SET(CPACK_DEBIAN_API-DEV_PACKAGE_ARCHITECTURE "any")
SET(CPACK_DEBIAN_DEV_PACKAGE_ARCHITECTURE "any")
INCLUDE(CPack)
