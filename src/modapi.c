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
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindSyscall *,
        SharemindModulesSet,
        findSyscall,
        const char * signature)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindSyscall *,
        SharemindModulesSet,
        findSyscall,
        const char * signature,
        NULL,
        assert(item->key);
        SharemindSyscall * const result =
                SharemindModule_findSyscall(item->key, signature);
        if (result)
            return result)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindPdk *,
        SharemindModulesSet,
        findPdk,
        const char *)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindPdk *,
        SharemindModulesSet,
        findPdk,
        const char * signature,
        NULL,
        assert(item->key);
        SharemindPdk * const result =
                SharemindModule_findPdk(item->key, signature);
        if (result)
            return result)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindPd *,
        SharemindModulesSet,
        findPd,
        const char *)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindPd *,
        SharemindModulesSet,
        findPd,
        const char * signature,
        NULL,
        assert(item->key);
        SharemindPd * const result =
                SharemindModule_findPd(item->key, signature);
        if (result)
            return result)
SHAREMIND_SET_DECLARE_DESTROY_WITH_INLINE(SharemindModulesSet,
                                          moduleFree,,
                                          static inline)
SHAREMIND_SET_DEFINE_DESTROY_WITH_INLINE(SharemindModulesSet,
                                         moduleFree,
                                         SharemindModule *,,
                                         free,
                                         static inline,
                                         SharemindModule_free(*item))

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

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(modapi)) {
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

    SharemindModulesSet_destroy_with_moduleFree(&modapi->modules);

    SHAREMIND_RECURSIVE_LOCK_DEINIT(modapi);

    SharemindFacilityMap_destroy(&modapi->moduleFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdpiFacilityMap);

    free(modapi);
}

SharemindSyscall * SharemindModuleApi_findSyscall(const SharemindModuleApi * m,
                                                  const char * signature)
{
    assert(m);
    return SharemindModulesSet_foreach_with_findSyscall(&m->modules, signature);
}

SharemindPdk * SharemindModuleApi_findPdk(const SharemindModuleApi * m,
                                          const char * name)
{
    assert(m);
    return SharemindModulesSet_foreach_with_findPdk(&m->modules, name);
}

SharemindPd * SharemindModuleApi_findPd(const SharemindModuleApi * m,
                                        const char * name)
{
    assert(m);
    return SharemindModulesSet_foreach_with_findPd(&m->modules, name);
}

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DEFINE(SharemindModuleApi)

SHAREMIND_LASTERROR_FUNCTIONS_DEFINE(SharemindModuleApi)

SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,module,Module)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pdpi,Pdpi)
