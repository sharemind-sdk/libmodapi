/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_MODULE_H
#define SHAREMIND_LIBMODAPI_MODULE_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/recursivemutex.h>
#include <sharemind/refs.h>
#include <stdint.h>
#include "apis.h"
#include "facilitymap.h"
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct SharemindModule_ {

    SharemindRecursiveMutex mutex;

    void * libHandle;
    char * filename;
    char * name;
    char * conf;
    uint32_t apiVersion;
    uint32_t version;
    const SharemindApi * api;

    void * apiData;
    void * moduleHandle;

    bool isInitialized;
    SharemindModuleApi * modapi;

    /** Module facility name to pointer mapping: */
    SharemindFacilityMap moduleFacilityMap;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    #ifndef NDEBUG
    SHAREMIND_REFS_DECLARE_FIELDS
    #endif

};

#ifndef NDEBUG
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindModule)
#endif

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_MODULE_H */

