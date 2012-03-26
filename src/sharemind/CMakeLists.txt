#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8")
CMAKE_POLICY(SET CMP0015 NEW)
PROJECT("SHAREMIND_LIBMODAPI" "C" "CXX")

# Find boost - seems to be required since the variables aren't populated otherwise
FIND_PACKAGE(Boost REQUIRED)

IF(NOT SHAREMIND_LIB_DIR)
    MESSAGE(FATAL_ERROR "SHAREMIND_LIB_DIR not defined!")
ENDIF(NOT SHAREMIND_LIB_DIR)

IF(NOT SHAREMIND_INCLUDE_DESTINATION)
    MESSAGE(FATAL_ERROR "SHAREMIND_INCLUDE_DESTINATION not defined!")
ENDIF(NOT SHAREMIND_INCLUDE_DESTINATION)

IF(NOT SHAREMIND_EXT_VM_GLOBALS_DIR)
    SET(SHAREMIND_EXT_VM_GLOBALS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../ext/vm_globals")
ENDIF(NOT SHAREMIND_EXT_VM_GLOBALS_DIR)
IF(NOT IS_DIRECTORY "${SHAREMIND_EXT_VM_GLOBALS_DIR}/include/sharemind")
    MESSAGE(FATAL_ERROR "SHAREMIND_EXT_VM_GLOBALS_DIR is invalid!")
ENDIF()
INCLUDE_DIRECTORIES(BEFORE SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/../sharemind/.." "${SHAREMIND_EXT_VM_GLOBALS_DIR}/include" ${Boost_INCLUDE_DIRS})


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

SET(OTHER_FILES "")
FUNCTION(PREPARE_OTHER_FILES)
    FOREACH(OTHER_FILE ${OTHER_FILES})
        SET(P ${P} "${OTHER_FILE}")
    ENDFOREACH(OTHER_FILE ${OTHER_FILES})
    SET(PREPARED_OTHER_FILES ${PREPARED_OTHER_FILES} ${P} PARENT_SCOPE)
ENDFUNCTION(PREPARE_OTHER_FILES)


################################################################################
# libmodapi
################################################################################

SET(OTHER_FILES ${OTHER_FILES}
    "libmodapi/api.h"
    "libmodapi/api_0x1.h"
    "libmodapi/apis.h"
    "libmodapi/facilitymap.h"
    "libmodapi/libmodapi.h"
    "libmodapi/modapi.h"
    "libmodapi/module.h"
    "libmodapi/module_0x1.h"
    "libmodapi/pd.h"
    "libmodapi/pd_0x1.h"
    "libmodapi/pdk.h"
    "libmodapi/pdpi.h"
    "libmodapi/pdpi_0x1.h"
    "libmodapi/syscall.h")

PREPARE_OTHER_FILES()
ADD_LIBRARY("libmodapi" SHARED
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
            ${PREPARED_OTHER_FILES}) # <-- hack for Qt Creator to include other files

SET_TARGET_PROPERTIES("libmodapi" PROPERTIES
                      OUTPUT_NAME "sharemind_modapi"
                      LIBRARY_OUTPUT_DIRECTORY "${SHAREMIND_LIB_DIR}"
                      VERSION "0.1.0.0"
                      SOVERSION "0.1")
INSTALL(FILES "libmodapi/libmodapi.h" "libmodapi/api.h" "libmodapi/api_0x1.h"
        DESTINATION "${SHAREMIND_INCLUDE_DESTINATION}/libmodapi")

INSTALL(TARGETS "libmodapi"
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "lib"
        PUBLIC_HEADER DESTINATION "include")
