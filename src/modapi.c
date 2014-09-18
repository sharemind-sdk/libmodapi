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

    if (unlikely(SharemindMutex_init(&modapi->mutex) != SHAREMIND_MUTEX_OK)) {
        free(modapi);
        if (error)
            (*error) = SHAREMIND_MODULE_API_MUTEX_ERROR;
        if (errorStr)
            (*errorStr) = "Mutex initialization error!";
        return NULL;
    }

    modapi->lastError = SHAREMIND_MODULE_API_OK;
    modapi->lastErrorStaticString = NULL;

    SharemindFacilityMap_init(&modapi->moduleFacilityMap, NULL);
    SharemindFacilityMap_init(&modapi->pdFacilityMap, NULL);
    SharemindFacilityMap_init(&modapi->pdpiFacilityMap, NULL);

    SHAREMIND_REFS_INIT(modapi);
    return modapi;
}

void SharemindModuleApi_free(SharemindModuleApi * modapi) {
    assert(modapi);
    if (unlikely(SharemindMutex_destroy(&modapi->mutex) != SHAREMIND_MUTEX_OK))
        abort();

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(modapi);

    SharemindFacilityMap_destroy(&modapi->moduleFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdpiFacilityMap);

    free(modapi);
}

#define DOLOCK(modapi,lock) \
    if (unlikely(SharemindMutex_ ## lock(&(modapi)->mutex) != SHAREMIND_MUTEX_OK)) { \
        abort(); \
    } else (void) 0
#define LOCK(modapi) DOLOCK((modapi),lock)
#define UNLOCK(modapi) DOLOCK((modapi),unlock)
#define LOCK_CONST(modapi) DOLOCK((modapi),lock_const)
#define UNLOCK_CONST(modapi) DOLOCK((modapi),unlock_const)

SHAREMIND_LASTERROR_DEFINE_FUNCTIONS(ModuleApi)

SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(ModuleApi,module,Module)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(ModuleApi,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(ModuleApi,pdpi,Pdpi)

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_MUTEX(SharemindModuleApi)
