#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8.12")
PROJECT("SHAREMIND_LIBMODAPI" "C" "CXX")

FOREACH(dir IN LISTS CMAKE_PREFIX_PATH)
  INCLUDE_DIRECTORIES(BEFORE SYSTEM "${dir}/include")
ENDFOREACH()
INCLUDE_DIRECTORIES(BEFORE SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/../")

ADD_COMPILE_OPTIONS(
  "-O2"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)

# Find boost - seems to be required since the variables aren't populated otherwise
FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=1 -fwrapv -Wabi -Wbad-function-cast -Wc++-compat -Wformat -Wswitch-default -Wunused -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wconversion -Wsign-conversion -Wunused-macros -D_POSIX_C_SOURCE=200809L")

INCLUDE(CheckCCompilerFlag)
MACRO(FLAGS_MAGIC flags)
    CHECK_C_COMPILER_FLAG("-${flags}" SHAREMIND_HAS_CFLAGS__${flags})
    IF(SHAREMIND_HAS_CFLAGS__${flags})
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -${flags}")
    ENDIF(SHAREMIND_HAS_CFLAGS__${flags})
ENDMACRO(FLAGS_MAGIC)
FLAGS_MAGIC("Weverything")
FLAGS_MAGIC("Wlogical-op")

SET(LIBMODAPI_INSTALL_DEFINITIONS "-D__STDC_LIMIT_MACROS")
ADD_DEFINITIONS("${LIBMODAPI_INSTALL_DEFINITIONS}")



################################################################################
# libmodapi
################################################################################

SET(SHAREMIND_LIBMODAPI_HEADERS
    "libmodapi/libmodapi.h"
    "libmodapi/api.h"
    "libmodapi/api_0x1.h"
)

SET(SHAREMIND_LIBMODAPI_HEADERS_P
    "libmodapi/apis.h"
    "libmodapi/facilitymap.h"
    "libmodapi/modapi.h"
    "libmodapi/module.h"
    "libmodapi/module_0x1.h"
    "libmodapi/pd.h"
    "libmodapi/pd_0x1.h"
    "libmodapi/pdk.h"
    "libmodapi/pdpi.h"
    "libmodapi/pdpi_0x1.h"
    "libmodapi/syscall.h")

IF(NOT SHAREMIND_LIBMODAPI_LINKING_MODE)
    SET(SHAREMIND_LIBMODAPI_LINKING_MODE SHARED)
ENDIF()

ADD_LIBRARY("libmodapi" ${SHAREMIND_LIBMODAPI_LINKING_MODE}
            "libmodapi/apis.c"
            "libmodapi/facilitymap.c"
            "libmodapi/modapi.c"
            "libmodapi/module.c"
            "libmodapi/module_0x1.c"
            "libmodapi/pd.c"
            "libmodapi/pd_0x1.c"
            "libmodapi/pdk.c"
            "libmodapi/pdpi.c"
            "libmodapi/pdpi_0x1.c"
            "libmodapi/syscall.c"
            ${SHAREMIND_LIBMODAPI_HEADERS}
            ${SHAREMIND_LIBMODAPI_HEADERS_P}
)

SET(LIBMODAPI_VERSION "0.1.0.0")
SET_TARGET_PROPERTIES("libmodapi" PROPERTIES
                      OUTPUT_NAME "sharemind_modapi"
                      VERSION "${LIBMODAPI_VERSION}"
                      SOVERSION "0.1")

INSTALL(TARGETS "libmodapi"
        LIBRARY DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBMODAPI_HEADERS}
        DESTINATION "include/sharemind/libmodapi")

# Install cmake files:
FILE(GLOB_RECURSE sharemind_libmodapi_in_files "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_sharemind_libmodapi_in_files_in_IDE" SOURCES ${sharemind_libmodapi_in_files})
SET(LIBMODAPI_INSTALL_LINK_LIBRARIES "-L${CMAKE_INSTALL_PREFIX}/lib" "-lsharemind_modapi")
SET(LIBMODAPI_INSTALL_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibmodapiConfig.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfig.cmake" @ONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibmodapiConfigVersion.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfigVersion.cmake" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiConfigVersion.cmake"
        DESTINATION "lib/SharemindLibmodapi")
