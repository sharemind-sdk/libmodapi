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


SHAREMIND_ENUM_CUSTOM_DEFINE_TOSTRING(SharemindModuleApiError,
                                      SHAREMIND_MODULE_API_ERROR_ENUM)

#define SHAREMIND_MODULE_API_DEFINE_ERRORSTRING(unused,unused2,e) \
    [(int) SHAREMIND_T(2,0,e)] = "Out of memory while generating error " \
                                 "message for error with code " \
                                 SHAREMIND_2S(SHAREMIND_T(2,0,e)) "!",

static const char * const sharemindModuleApiOomErrorStrings[
        SHAREMIND_MODULE_API_ERROR_COUNT + 1] =
{
    BOOST_PP_SEQ_FOR_EACH(SHAREMIND_MODULE_API_DEFINE_ERRORSTRING,
                          _,
                          SHAREMIND_MODULE_API_ERROR_ENUM)
};


SharemindModuleApi * SharemindModuleApi_new() {
    SharemindModuleApi * const modapi =
            (SharemindModuleApi *) malloc(sizeof(SharemindModuleApi));
    if (unlikely(!modapi))
        goto SharemindModuleApi_new_error;

    if (unlikely(SharemindMutex_init(&modapi->mutex) != SHAREMIND_MUTEX_OK))
        goto SharemindModuleApi_new_error2;

    modapi->lastError = SHAREMIND_MODULE_API_OK;
    modapi->lastErrorDynamicString = NULL;
    modapi->lastErrorStaticString = NULL;

    SharemindFacilityMap_init(&modapi->moduleFacilityMap, NULL);
    SharemindFacilityMap_init(&modapi->pdFacilityMap, NULL);
    SharemindFacilityMap_init(&modapi->pdpiFacilityMap, NULL);

    SHAREMIND_REFS_INIT(modapi);
    return modapi;

SharemindModuleApi_new_error2:

    free(modapi);

SharemindModuleApi_new_error:

    return NULL;

}

void SharemindModuleApi_free(SharemindModuleApi * modapi) {
    assert(modapi);
    if (unlikely(SharemindMutex_destroy(&modapi->mutex) != SHAREMIND_MUTEX_OK))
        abort();

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(modapi);
    if (modapi->lastErrorDynamicString)
        free(modapi->lastErrorDynamicString);

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

SharemindModuleApiError SharemindModuleApi_get_last_error(
        const SharemindModuleApi * modapi)
{
    assert(modapi);
    LOCK_CONST(modapi);
    const SharemindModuleApiError r = modapi->lastError;
    UNLOCK_CONST(modapi);
    return r;
}

const char * SharemindModuleApi_get_last_error_string(
        const SharemindModuleApi * modapi)
{
    assert(modapi);
    const char * r;
    LOCK_CONST(modapi);
    if (unlikely(modapi->lastError == SHAREMIND_MODULE_API_OK)) {
        r = NULL;
    } else if (modapi->lastErrorStaticString) {
        r = modapi->lastErrorStaticString;
    } else {
        r = modapi->lastErrorDynamicString;
    }
    UNLOCK_CONST(modapi);
    return r;
}

void SharemindModuleApi_clear_error(SharemindModuleApi * modapi) {
    assert(modapi);
    LOCK(modapi);
    modapi->lastError = SHAREMIND_MODULE_API_OK;
    UNLOCK(modapi);
}

void SharemindModuleApi_set_error_with_static_string(
        SharemindModuleApi * modapi,
        SharemindModuleApiError error,
        const char * errorString)
{
    assert(modapi);
    assert(error != SHAREMIND_MODULE_API_OK);

    if (unlikely(!errorString || !errorString[0]))
        errorString = SharemindModuleApiError_toString(error);

    assert(errorString);
    assert(errorString[0]);

    LOCK(modapi);
    modapi->lastErrorStaticString = errorString;
    modapi->lastError = error;
    UNLOCK(modapi);
}

bool SharemindModuleApi_set_error_with_dynamic_string(
        SharemindModuleApi * modapi,
        SharemindModuleApiError error,
        const char * errorString)
{
    assert(modapi);
    assert(error != SHAREMIND_MODULE_API_OK);

    const bool hasErrorString = errorString && errorString[0];
    if (likely(hasErrorString)) {
        const size_t errorStringLength = strlen(errorString);
        assert(errorStringLength > 0);

        LOCK(modapi);
        modapi->lastError = error;
        char * const newErrorString =
                (char *) realloc(modapi->lastErrorDynamicString,
                                 errorStringLength + 1);
        if (likely(newErrorString)) {
            strcpy(newErrorString, errorString);
            modapi->lastErrorDynamicString = newErrorString;
            modapi->lastErrorStaticString = NULL;
            UNLOCK(modapi);
            return true;
        }
    } else {
        LOCK(modapi);
    }
    modapi->lastErrorStaticString =
            sharemindModuleApiOomErrorStrings[(int) error];
    UNLOCK(modapi);
    return !hasErrorString;
}

bool SharemindModuleApi_set_module_facility(SharemindModuleApi * modapi,
                                            const char * name,
                                            void * facility,
                                            void * context)
{
    assert(modapi);
    assert(name);
    assert(name[0]);
    LOCK(modapi);
    const bool r = SharemindFacilityMap_set(&modapi->moduleFacilityMap,
                                            name,
                                            facility,
                                            context);
    UNLOCK(modapi);
    return r;
}

const SharemindFacility * SharemindModuleApi_get_module_facility(
        const SharemindModuleApi * modapi,
        const char * name)
{
    assert(modapi);
    assert(name);
    assert(name[0]);
    LOCK_CONST(modapi);
    const SharemindFacility * const r =
            SharemindFacilityMap_get(&modapi->moduleFacilityMap, name);
    UNLOCK_CONST(modapi);
    return r;
}

bool SharemindModuleApi_set_pd_facility(SharemindModuleApi * modapi,
                                        const char * name,
                                        void * facility,
                                        void * context)
{
    assert(modapi);
    assert(name);
    assert(name[0]);
    LOCK(modapi);
    const bool r = SharemindFacilityMap_set(&modapi->pdFacilityMap,
                                            name,
                                            facility,
                                            context);
    UNLOCK(modapi);
    return r;
}

const SharemindFacility * SharemindModuleApi_get_pd_facility(
        const SharemindModuleApi * modapi,
        const char * name)
{
    assert(modapi);
    assert(name);
    assert(name[0]);
    LOCK_CONST(modapi);
    const SharemindFacility * const r =
            SharemindFacilityMap_get(&modapi->pdFacilityMap, name);
    UNLOCK_CONST(modapi);
    return r;
}

bool SharemindModuleApi_set_pdpi_facility(SharemindModuleApi * modapi,
                                          const char * name,
                                          void * facility,
                                          void * context)
{
    assert(modapi);
    assert(name);
    assert(name[0]);
    LOCK(modapi);
    const bool r = SharemindFacilityMap_set(&modapi->pdpiFacilityMap,
                                            name,
                                            facility,
                                            context);
    UNLOCK(modapi);
    return r;
}

const SharemindFacility * SharemindModuleApi_get_pdpi_facility(
        const SharemindModuleApi * modapi,
        const char * name)
{
    assert(modapi);
    assert(name);
    assert(name[0]);
    LOCK_CONST(modapi);
    const SharemindFacility * const r =
            SharemindFacilityMap_get(&modapi->pdpiFacilityMap, name);
    UNLOCK_CONST(modapi);
    return r;
}

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_MUTEX(SharemindModuleApi)
