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
#define SHAREMIND_COMMA ,
SHAREMIND_SET_DECLARE_FOREACH_WITH(
        SharemindModulesSet,
        SharemindModule *,
        findSyscall,
        const char * SHAREMIND_COMMA SharemindSyscall **,
        static inline)
SHAREMIND_SET_DEFINE_FOREACH_WITH(
        SharemindModulesSet,
        SharemindModule *,
        findSyscall,
        const char * SHAREMIND_COMMA SharemindSyscall ** ,
        const char * signature SHAREMIND_COMMA SharemindSyscall ** result,
        signature SHAREMIND_COMMA result,
        static inline)
SHAREMIND_SET_DECLARE_FOREACH_WITH(
        SharemindModulesSet,
        SharemindModule *,
        findPdk,
        const char * SHAREMIND_COMMA SharemindPdk **,
        static inline)
SHAREMIND_SET_DEFINE_FOREACH_WITH(
        SharemindModulesSet,
        SharemindModule *,
        findPdk,
        const char * SHAREMIND_COMMA SharemindPdk ** ,
        const char * name SHAREMIND_COMMA SharemindPdk ** result,
        name SHAREMIND_COMMA result,
        static inline)
#undef SHAREMIND_COMMA

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

static inline bool SharemindModuleApi_findSyscallHelper(
        SharemindModule * const * module,
        const char * signature,
        SharemindSyscall ** result)
{
    assert(module);
    assert(signature);
    assert(result);
    SharemindSyscall * const sc =
            SharemindModule_findSyscall(*module, signature);
    if (!sc)
        return true;
    *result = sc;
    return false;
}

SharemindSyscall * SharemindModuleApi_findSyscall(const SharemindModuleApi * m,
                                                  const char * signature)
{
    assert(m);
    SharemindSyscall * result;
    if (SharemindModulesSet_foreach_with_findSyscall(
                &m->modules,
                &SharemindModuleApi_findSyscallHelper,
                signature,
                &result))
        return NULL;
    assert(result);
    return result;
}

static inline bool SharemindModuleApi_findPdkHelper(
        SharemindModule * const * module,
        const char * signature,
        SharemindPdk ** result)
{
    assert(module);
    assert(signature);
    assert(result);
    SharemindPdk * const pdk =
            SharemindModule_findPdk(*module, signature);
    if (!pdk)
        return true;
    *result = pdk;
    return false;
}

SharemindPdk * SharemindModuleApi_findPdk(const SharemindModuleApi * m,
                                          const char * name)
{
    assert(m);
    SharemindPdk * result;
    if (SharemindModulesSet_foreach_with_findPdk(
                &m->modules,
                &SharemindModuleApi_findPdkHelper,
                name,
                &result))
        return NULL;
    assert(result);
    return result;
}

SHAREMIND_LOCK_FUNCTIONS_DEFINE(SharemindModuleApi)

SHAREMIND_LASTERROR_FUNCTIONS_DEFINE(SharemindModuleApi)

SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,module,Module)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pdpi,Pdpi)
