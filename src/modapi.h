/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_MODAPI_H
#define SHAREMIND_LIBMODAPI_MODAPI_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/mutex.h>
#include <sharemind/refs.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct SharemindModuleApi_ {

    SharemindMutex mutex;

    SharemindModuleApiError lastError;
    const char * lastErrorStaticString;
    char * lastErrorDynamicString;

    /** Module facility name to pointer mapping: */
    SharemindFacilityMap moduleFacilityMap;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    SHAREMIND_REFS_DECLARE_FIELDS
};

void SharemindModuleApi_set_error_with_static_string(
        SharemindModuleApi * modapi,
        SharemindModuleApiError error,
        const char * errorString) __attribute__ ((nonnull(1)));

#define OOM(modapi) \
    if (1) { \
        SharemindModuleApi_set_error_with_static_string( \
                (modapi), \
                SHAREMIND_MODULE_API_OUT_OF_MEMORY, \
                "Out of memory!"); \
    } else (void) 0

#define OOR(modapi) \
    if (1) { \
        SharemindModuleApi_set_error_with_static_string( \
                (modapi), \
                SHAREMIND_MODULE_API_REFERENCE_OVERFLOW, \
                "Too many references!"); \
    } else (void) 0

#define MIE(modapi) \
    if (1) { \
        SharemindModuleApi_set_error_with_static_string( \
                (modapi), \
                SHAREMIND_MODULE_API_MUTEX_ERROR, \
                "Mutex initialization error!"); \
    } else (void) 0

bool SharemindModuleApi_set_error_with_dynamic_string(
        SharemindModuleApi * modapi,
        SharemindModuleApiError error,
        const char * errorString) __attribute__ ((nonnull(1)));

SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindModuleApi)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_MODAPI_H */

