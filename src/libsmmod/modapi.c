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

/*#include "libsmmod.h"

#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "../likely.h"
#include "apis.h"*/


SM_ENUM_CUSTOM_DEFINE_TOSTRING(SMVM_MODAPI_Error,SMVM_ENUM_MODAPI_Error)

#define SMVM_MODAPI_DEFINE_ERRORSTRING(unused,unused2,e) \
    [(int) SM_T(2,0,e)] = "Out of memory while generating error message for error with code " SM_2S(SM_T(2,0,e)) "!",

const char * const SMVM_MODAPI_OomErrorStrings[SMVM_MODAPI_ERROR_COUNT + 1] = {
    BOOST_PP_SEQ_FOR_EACH(SMVM_MODAPI_DEFINE_ERRORSTRING,_,SMVM_ENUM_MODAPI_Error)
};


SMVM_MODAPI * SMVM_MODAPI_new() {
    SMVM_MODAPI * const modapi = (SMVM_MODAPI *) malloc(sizeof(SMVM_MODAPI));
    if (likely(modapi)) {
        modapi->lastError = SMVM_MODAPI_OK;
        modapi->lastErrorDynamicString = NULL;
        modapi->lastErrorStaticString = NULL;

        modapi->facilityContext = NULL;
        SMVM_FacilityMap_init(&modapi->moduleFacilityMap, NULL);
        SMVM_FacilityMap_init(&modapi->pdFacilityMap, NULL);
        SMVM_FacilityMap_init(&modapi->pdpiFacilityMap, NULL);

        SMVM_REFS_INIT(modapi);
    }
    return modapi;
}

void SMVM_MODAPI_free(SMVM_MODAPI * modapi) {
    assert(modapi);
    SMVM_REFS_ASSERT_IF_REFERENCED(modapi);
    if (modapi->lastErrorDynamicString)
        free(modapi->lastErrorDynamicString);

    SMVM_FacilityMap_destroy(&modapi->moduleFacilityMap);
    SMVM_FacilityMap_destroy(&modapi->pdFacilityMap);
    SMVM_FacilityMap_destroy(&modapi->pdpiFacilityMap);

    free(modapi);
}

SMVM_MODAPI_Error SMVM_MODAPI_lastError(const SMVM_MODAPI * modapi) {
    assert(modapi);
    return modapi->lastError;
}

const char * SMVM_MODAPI_lastErrorString(const SMVM_MODAPI * modapi) {
    assert(modapi);
    if (unlikely(modapi->lastError == SMVM_MODAPI_OK)) {
        return NULL;
    } else if (modapi->lastErrorStaticString) {
        return modapi->lastErrorStaticString;
    } else {
        return modapi->lastErrorDynamicString;
    }
}

void SMVM_MODAPI_clearError(SMVM_MODAPI * modapi) {
    assert(modapi);
    modapi->lastError = SMVM_MODAPI_OK;
}

void SMVM_MODAPI_setErrorWithStaticString(SMVM_MODAPI * modapi,
                                          SMVM_MODAPI_Error error,
                                          const char * errorString)
{
    assert(modapi);
    assert(error != SMVM_MODAPI_OK);

    if (unlikely(!errorString && !errorString[0]))
        errorString = SMVM_MODAPI_Error_toString(error);

    assert(errorString);
    assert(errorString[0]);

    modapi->lastErrorStaticString = errorString;
    modapi->lastError = error;
}

bool SMVM_MODAPI_setErrorWithDynamicString(SMVM_MODAPI * modapi,
                                           SMVM_MODAPI_Error error,
                                           const char * errorString)
{
    assert(modapi);
    assert(error != SMVM_MODAPI_OK);

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
    modapi->lastErrorStaticString = SMVM_MODAPI_OomErrorStrings[(int) error];
    return !hasErrorString;
}

SMVM_REFS_DEFINE_FUNCTIONS(SMVM_MODAPI)

void SMVM_MODAPI_set_facility_context(SMVM_MODAPI * modapi, void * facilityContext) {
    assert(modapi);
    modapi->facilityContext = facilityContext;
}

void * SMVM_MODAPI_get_facility_context(const SMVM_MODAPI * modapi) {
    assert(modapi);
    return modapi->facilityContext;
}

int SMVM_MODAPI_set_module_facility(SMVM_MODAPI * modapi, const char * name, void * facility) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&modapi->moduleFacilityMap, name, facility);
}

void * SMVM_MODAPI_get_module_facility(const SMVM_MODAPI * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&modapi->moduleFacilityMap, name);
}

int SMVM_MODAPI_set_pd_facility(SMVM_MODAPI * modapi, const char * name, void * facility) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&modapi->pdFacilityMap, name, facility);
}

void * SMVM_MODAPI_get_pd_facility(const SMVM_MODAPI * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&modapi->pdFacilityMap, name);
}

int SMVM_MODAPI_set_pdpi_facility(SMVM_MODAPI * modapi, const char * name, void * facility) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&modapi->pdpiFacilityMap, name, facility);
}

void * SMVM_MODAPI_get_pdpi_facility(const SMVM_MODAPI * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&modapi->pdpiFacilityMap, name);
}
