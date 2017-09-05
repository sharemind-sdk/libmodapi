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

CMAKE_MINIMUM_REQUIRED(VERSION "3.0")
PROJECT(SharemindLibModapi VERSION "0.3.0" LANGUAGES "C")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers REQUIRED)

SharemindSetProjectVersion()


FIND_PACKAGE(SharemindCHeaders REQUIRED)
FIND_PACKAGE(SharemindModuleApis REQUIRED)


################################################################################
# libmodapi
################################################################################

# Headers:
SET(SHAREMIND_LIBMODAPI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/libmodapi.h")
INSTALL(FILES ${SHAREMIND_LIBMODAPI_HEADERS}
        DESTINATION "include/sharemind/libmodapi"
        COMPONENT "dev")

# Private headers:
FILE(GLOB_RECURSE SHAREMIND_LIBMODAPI_HEADERS_P
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
LIST(REMOVE_ITEM SHAREMIND_LIBMODAPI_HEADERS_P
    ${SHAREMIND_LIBMODAPI_HEADERS}
    ${SHAREMIND_LIBMODAPI_API_HEADERS})

# The library:
SharemindSetC99CompileOptions(COMPILE_FLAGS "-fwrapv")
FILE(GLOB_RECURSE SHAREMIND_LIBMODAPI_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
SharemindAddSharedLibrary("libmodapi"
    SOURCES ${SHAREMIND_LIBMODAPI_SOURCES}
            ${SHAREMIND_LIBMODAPI_HEADERS}
            ${SHAREMIND_LIBMODAPI_API_HEADERS}
            ${SHAREMIND_LIBMODAPI_HEADERS_P}
    OUTPUT_NAME "sharemind_modapi"
    LINK_LIBRARIES
        ${SharemindCHeaders_LIBRARIES}
        "-ldl"
    LEGACY_DEFINITIONS
        ${SharemindCHeaders_DEFINITIONS}
    COMPILE_DEFINITIONS
        "_XOPEN_SOURCE=700"
    INCLUDE_DIRECTORIES
        ${SharemindCHeaders_INCLUDE_DIRS}
        ${SharemindModuleApis_INCLUDE_DIRS}
)


# Install cmake files:
SharemindCreateCMakeFindFiles(
    INCLUDE_DIRS
        "${CMAKE_INSTALL_PREFIX}/include"
    DEFINITIONS
        "-D__STDC_LIMIT_MACROS"
        "-D_XOPEN_SOURCE=700"
    LIBRARIES
        ${SharemindCHeaders_LIBRARIES}
        "-L${CMAKE_INSTALL_PREFIX}/lib"
        "-lsharemind_modapi"
)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-modapi"
    DESCRIPTION "Sharemind Module API library"
    DEB_SECTION "libs"
    DEB_DEPENDS "libc6 (>= 2.19)"
)
SharemindAddComponentPackage("dev"
    NAME "libsharemind-modapi-dev"
    DESCRIPTION "Sharemind Module API library development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-modapi (= ${SharemindLibModapi_VERSION})"
        "libsharemind-cheaders-dev (>= 0.1.0)"
        "sharemind-module-apis-dev (>= 1.0.0)"
        "libc6-dev (>= 2.19)"
        "libgcc-4.8-dev"
        "| libgcc-4.9-dev"
        "| libgcc-5-dev"
        "| libgcc-6-dev"
        "| libgcc-7-dev"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-modapi-dbg"
    DESCRIPTION "Sharemind Module API library debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS "libsharemind-modapi (= ${SharemindLibModapi_VERSION})"
)
SharemindPackagingFinalize()
