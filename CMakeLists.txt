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
PROJECT(SharemindLibModapi VERSION "0.3.4" LANGUAGES "C")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.6 REQUIRED)

FIND_PACKAGE(SharemindCHeaders 1.3.0 REQUIRED)
FIND_PACKAGE(SharemindModuleApis 1.1.0 REQUIRED)


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
SharemindAddSharedLibrary(LibModapi
    SOURCES ${SHAREMIND_LIBMODAPI_SOURCES}
            ${SHAREMIND_LIBMODAPI_HEADERS}
            ${SHAREMIND_LIBMODAPI_API_HEADERS}
            ${SHAREMIND_LIBMODAPI_HEADERS_P}
    OUTPUT_NAME "sharemind_modapi"
)
TARGET_INCLUDE_DIRECTORIES(LibModapi
    INTERFACE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> # TODO
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    )
TARGET_COMPILE_DEFINITIONS(LibModapi
    PUBLIC
        "_XOPEN_SOURCE=700"
        "__STDC_LIMIT_MACROS"
    )
TARGET_LINK_LIBRARIES(LibModapi
    PRIVATE
        "dl"
    PUBLIC
        "Sharemind::CHeaders"
        "Sharemind::ModuleApis"
    )
SharemindCreateCMakeFindFilesForTarget(LibModapi
    DEPENDENCIES
        "SharemindCHeaders 1.3.0"
        "SharemindModuleApis 1.1.0"
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
        "libsharemind-modapi (= ${SharemindLibModapi_DEB_lib_PACKAGE_VERSION})"
        "libsharemind-cheaders-dev (>= 1.3.0)"
        "sharemind-module-apis-dev (>= 1.1.0)"
        "libc6-dev (>= 2.19)"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-modapi-dbg"
    DESCRIPTION "Sharemind Module API library debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS "libsharemind-modapi (= ${SharemindLibModapi_DEB_lib_PACKAGE_VERSION})"
)
SharemindPackagingFinalize()
