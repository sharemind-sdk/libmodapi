/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "modapi.h"

#include <stdlib.h>
#include "lasterror.h"


SHAREMIND_ENUM_CUSTOM_DEFINE_TOSTRING(SharemindModuleApiError,
                                      SHAREMIND_MODULE_API_ERROR_ENUM)

SHAREMIND_SET_DEFINE(SharemindModulesSet,
                     SharemindModule *,
                     ((uintptr_t) key),
                     SHAREMIND_SET_KEY_EQUALS_voidptr,
                     SHAREMIND_SET_KEY_LESS_THAN_voidptr,
                     SHAREMIND_SET_KEYCOPY_REGULAR,
                     SHAREMIND_SET_KEYFREE_REGULAR,
                     malloc,
                     free,)

SharemindModuleApi * SharemindModuleApi_new(SharemindModuleApiError * error,
                                            const char ** errorStr)
{
    SharemindModuleApi * const modapi =
            (SharemindModuleApi *) malloc(sizeof(SharemindModuleApi));
    if (unlikely(!modapi)) {
        if (error)
            (*error) = SHAREMIND_MODULE_API_OUT_OF_MEMORY;
        if (errorStr)
            (*errorStr) = "Out of memory!";
        return NULL;
    }

    if (!SHAREMIND_LOCK_INIT(modapi)) {
        free(modapi);
        if (error)
            (*error) = SHAREMIND_MODULE_API_MUTEX_ERROR;
        if (errorStr)
            (*errorStr) = "Mutex initialization error!";
        return NULL;
    }

    modapi->lastError = SHAREMIND_MODULE_API_OK;
    modapi->lastErrorStaticString = NULL;

    SharemindModulesSet_init(&modapi->modules);
    SharemindFacilityMap_init(&modapi->moduleFacilityMap, NULL);
    SharemindFacilityMap_init(&modapi->pdFacilityMap, NULL);
    SharemindFacilityMap_init(&modapi->pdpiFacilityMap, NULL);

    return modapi;
}

void SharemindModuleApi_free(SharemindModuleApi * modapi) {
    assert(modapi);
    SHAREMIND_LOCK_DEINIT(modapi);

    assert(modapi->modules.size == 0u);

    SharemindFacilityMap_destroy(&modapi->moduleFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdpiFacilityMap);
    SharemindModulesSet_destroy(&modapi->modules);

    free(modapi);
}

SHAREMIND_LOCK_FUNCTIONS_DEFINE(SharemindModuleApi)

SHAREMIND_LASTERROR_FUNCTIONS_DEFINE(SharemindModuleApi)

SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,module,Module)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pdpi,Pdpi)
