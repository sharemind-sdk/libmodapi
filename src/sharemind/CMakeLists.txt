#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8")
PROJECT("SHAREMIND_LIBMODAPI" "C" "CXX")

# Find boost - seems to be required since the variables aren't populated otherwise
FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -O2 -fstrict-aliasing -Wstrict-aliasing=1 -fwrapv -Wabi -Wbad-function-cast -Wc++-compat -Wformat -Wswitch-default -Wunused -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wconversion -Wsign-conversion -Wunused-macros -D_POSIX_C_SOURCE=200809L")

INCLUDE(CheckCCompilerFlag)
MACRO(FLAGS_MAGIC flags)
    CHECK_C_COMPILER_FLAG("-${flags}" SHAREMIND_HAS_CFLAGS__${flags})
    IF(SHAREMIND_HAS_CFLAGS__${flags})
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -${flags}")
    ENDIF(SHAREMIND_HAS_CFLAGS__${flags})
ENDMACRO(FLAGS_MAGIC)
FLAGS_MAGIC("Weverything")
FLAGS_MAGIC("Wlogical-op")

ADD_DEFINITIONS("-D_XOPEN_SOURCE=500" "-D__STDC_LIMIT_MACROS")



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

SET_TARGET_PROPERTIES("libmodapi" PROPERTIES
                      OUTPUT_NAME "sharemind_modapi"
                      VERSION "0.1.0.0"
                      SOVERSION "0.1")

INSTALL(TARGETS "libmodapi"
        LIBRARY DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBMODAPI_HEADERS}
        DESTINATION "include/sharemind/libmodapi")
