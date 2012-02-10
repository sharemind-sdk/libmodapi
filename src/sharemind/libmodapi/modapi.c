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

/*#include "libmodapi.h"

#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "../likely.h"
#include "apis.h"*/


SHAREMIND_ENUM_CUSTOM_DEFINE_TOSTRING(SHAREMIND_MODAPI_Error,SHAREMIND_ENUM_MODAPI_Error)

#define SHAREMIND_MODAPI_DEFINE_ERRORSTRING(unused,unused2,e) \
    [(int) SHAREMIND_T(2,0,e)] = "Out of memory while generating error message for error with code " SHAREMIND_2S(SHAREMIND_T(2,0,e)) "!",

const char * const SHAREMIND_MODAPI_OomErrorStrings[SHAREMIND_MODAPI_ERROR_COUNT + 1] = {
    BOOST_PP_SEQ_FOR_EACH(SHAREMIND_MODAPI_DEFINE_ERRORSTRING,_,SHAREMIND_ENUM_MODAPI_Error)
};


SHAREMIND_MODAPI * SHAREMIND_MODAPI_new() {
    SHAREMIND_MODAPI * const modapi = (SHAREMIND_MODAPI *) malloc(sizeof(SHAREMIND_MODAPI));
    if (likely(modapi)) {
        modapi->lastError = SHAREMIND_MODAPI_OK;
        modapi->lastErrorDynamicString = NULL;
        modapi->lastErrorStaticString = NULL;

        SHAREMIND_FacilityMap_init(&modapi->moduleFacilityMap, NULL);
        SHAREMIND_FacilityMap_init(&modapi->pdFacilityMap, NULL);
        SHAREMIND_FacilityMap_init(&modapi->pdpiFacilityMap, NULL);

        SHAREMIND_REFS_INIT(modapi);
    }
    return modapi;
}

void SHAREMIND_MODAPI_free(SHAREMIND_MODAPI * modapi) {
    assert(modapi);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(modapi);
    if (modapi->lastErrorDynamicString)
        free(modapi->lastErrorDynamicString);

    SHAREMIND_FacilityMap_destroy(&modapi->moduleFacilityMap);
    SHAREMIND_FacilityMap_destroy(&modapi->pdFacilityMap);
    SHAREMIND_FacilityMap_destroy(&modapi->pdpiFacilityMap);

    free(modapi);
}

SHAREMIND_MODAPI_Error SHAREMIND_MODAPI_lastError(const SHAREMIND_MODAPI * modapi) {
    assert(modapi);
    return modapi->lastError;
}

const char * SHAREMIND_MODAPI_lastErrorString(const SHAREMIND_MODAPI * modapi) {
    assert(modapi);
    if (unlikely(modapi->lastError == SHAREMIND_MODAPI_OK)) {
        return NULL;
    } else if (modapi->lastErrorStaticString) {
        return modapi->lastErrorStaticString;
    } else {
        return modapi->lastErrorDynamicString;
    }
}

void SHAREMIND_MODAPI_clearError(SHAREMIND_MODAPI * modapi) {
    assert(modapi);
    modapi->lastError = SHAREMIND_MODAPI_OK;
}

void SHAREMIND_MODAPI_setErrorWithStaticString(SHAREMIND_MODAPI * modapi,
                                          SHAREMIND_MODAPI_Error error,
                                          const char * errorString)
{
    assert(modapi);
    assert(error != SHAREMIND_MODAPI_OK);

    if (unlikely(!errorString && !errorString[0]))
        errorString = SHAREMIND_MODAPI_Error_toString(error);

    assert(errorString);
    assert(errorString[0]);

    modapi->lastErrorStaticString = errorString;
    modapi->lastError = error;
}

bool SHAREMIND_MODAPI_setErrorWithDynamicString(SHAREMIND_MODAPI * modapi,
                                           SHAREMIND_MODAPI_Error error,
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

SHAREMIND_REFS_DEFINE_FUNCTIONS(SHAREMIND_MODAPI)

int SHAREMIND_MODAPI_set_module_facility(SHAREMIND_MODAPI * modapi, const char * name, void * facility, void * context) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&modapi->moduleFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_MODAPI_get_module_facility(const SHAREMIND_MODAPI * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&modapi->moduleFacilityMap, name);
}

int SHAREMIND_MODAPI_set_pd_facility(SHAREMIND_MODAPI * modapi, const char * name, void * facility, void * context) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&modapi->pdFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_MODAPI_get_pd_facility(const SHAREMIND_MODAPI * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&modapi->pdFacilityMap, name);
}

int SHAREMIND_MODAPI_set_pdpi_facility(SHAREMIND_MODAPI * modapi, const char * name, void * facility, void *context) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&modapi->pdpiFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_MODAPI_get_pdpi_facility(const SHAREMIND_MODAPI * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&modapi->pdpiFacilityMap, name);
}
