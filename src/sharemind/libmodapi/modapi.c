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


SHAREMIND_ENUM_CUSTOM_DEFINE_TOSTRING(SharemindModuleApiError,SHAREMIND_MODULE_API_ERROR_ENUM)

#define SHAREMIND_MODULE_API_DEFINE_ERRORSTRING(unused,unused2,e) \
    [(int) SHAREMIND_T(2,0,e)] = "Out of memory while generating error message for error with code " SHAREMIND_2S(SHAREMIND_T(2,0,e)) "!",

const char * const SHAREMIND_MODAPI_OomErrorStrings[SHAREMIND_MODAPI_ERROR_COUNT + 1] = {
    BOOST_PP_SEQ_FOR_EACH(SHAREMIND_MODULE_API_DEFINE_ERRORSTRING,_,SHAREMIND_MODULE_API_ERROR_ENUM)
};


SharemindModuleApi * SharemindModuleApi_new() {
    SharemindModuleApi * const modapi = (SharemindModuleApi *) malloc(sizeof(SharemindModuleApi));
    if (likely(modapi)) {
        modapi->lastError = SHAREMIND_MODAPI_OK;
        modapi->lastErrorDynamicString = NULL;
        modapi->lastErrorStaticString = NULL;

        SharemindFacilityMap_init(&modapi->moduleFacilityMap, NULL);
        SharemindFacilityMap_init(&modapi->pdFacilityMap, NULL);
        SharemindFacilityMap_init(&modapi->pdpiFacilityMap, NULL);

        SHAREMIND_REFS_INIT(modapi);
    }
    return modapi;
}

void SharemindModuleApi_free(SharemindModuleApi * modapi) {
    assert(modapi);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(modapi);
    if (modapi->lastErrorDynamicString)
        free(modapi->lastErrorDynamicString);

    SharemindFacilityMap_destroy(&modapi->moduleFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdpiFacilityMap);

    free(modapi);
}

SharemindModuleApiError SharemindModuleApi_get_last_error(const SharemindModuleApi * modapi) {
    assert(modapi);
    return modapi->lastError;
}

const char * SharemindModuleApi_get_last_error_string(const SharemindModuleApi * modapi) {
    assert(modapi);
    if (unlikely(modapi->lastError == SHAREMIND_MODAPI_OK)) {
        return NULL;
    } else if (modapi->lastErrorStaticString) {
        return modapi->lastErrorStaticString;
    } else {
        return modapi->lastErrorDynamicString;
    }
}

void SharemindModuleApi_clear_error(SharemindModuleApi * modapi) {
    assert(modapi);
    modapi->lastError = SHAREMIND_MODAPI_OK;
}

void SharemindModuleApi_set_error_with_static_string(SharemindModuleApi * modapi,
                                          SharemindModuleApiError error,
                                          const char * errorString)
{
    assert(modapi);
    assert(error != SHAREMIND_MODAPI_OK);

    if (unlikely(!errorString && !errorString[0]))
        errorString = SharemindModuleApiError_toString(error);

    assert(errorString);
    assert(errorString[0]);

    modapi->lastErrorStaticString = errorString;
    modapi->lastError = error;
}

bool SharemindModuleApi_set_error_with_dynamic_string(SharemindModuleApi * modapi,
                                           SharemindModuleApiError error,
                                           const char * errorString)
{
    assert(modapi);
    assert(error != SHAREMIND_MODAPI_OK);

    const bool hasErrorString = errorString && errorString[0];
    if (likely(hasErrorString)) {
        const size_t errorStringLength = strlen(errorString);
        assert(errorStringLength > 0);

        modapi->lastError = error;
        char * const newErrorString = (char *) realloc(modapi->lastErrorDynamicString, errorStringLength + 1);
        if (likely(newErrorString)) {
            strcpy(newErrorString, errorString);
            modapi->lastErrorDynamicString = newErrorString;
            modapi->lastErrorStaticString = NULL;
            return true;
        }
    }
    modapi->lastErrorStaticString = SHAREMIND_MODAPI_OomErrorStrings[(int) error];
    return !hasErrorString;
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SharemindModuleApi)

int SharemindModuleApi_set_module_facility(SharemindModuleApi * modapi, const char * name, void * facility, void * context) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&modapi->moduleFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindModuleApi_get_module_facility(const SharemindModuleApi * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&modapi->moduleFacilityMap, name);
}

int SharemindModuleApi_set_pd_facility(SharemindModuleApi * modapi, const char * name, void * facility, void * context) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&modapi->pdFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindModuleApi_get_pd_facility(const SharemindModuleApi * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&modapi->pdFacilityMap, name);
}

int SharemindModuleApi_set_pdpi_facility(SharemindModuleApi * modapi, const char * name, void * facility, void *context) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&modapi->pdpiFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindModuleApi_get_pdpi_facility(const SharemindModuleApi * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&modapi->pdpiFacilityMap, name);
}
