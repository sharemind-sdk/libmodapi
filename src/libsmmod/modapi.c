/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "libsmmod.h"

#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "../likely.h"
#include "api_0x1.h"
#include "modapi_0x1.h"


/*******************************************************************************
  List of APIs
*******************************************************************************/

const struct {
    SMVM_MODAPI_Error (* const module_load)(SMVM_Module * m);
    void (* const module_unload)(SMVM_Module * m);

    SMVM_MODAPI_Error (* const module_init)(SMVM_Module * m);
    void (* const module_deinit)(SMVM_Module * m);

    size_t (* const module_get_num_syscalls)(const SMVM_Module * m);
    SMVM_Syscall * (* const module_get_syscall)(const SMVM_Module * m, size_t index);
    SMVM_Syscall * (* const module_find_syscall)(const SMVM_Module * m, const char * signature);

    size_t (* const module_get_num_pdks)(const SMVM_Module * m);
    SMVM_PDK * (* const module_get_pdk)(const SMVM_Module * m, size_t index);
    SMVM_PDK * (* const module_find_pdk)(const SMVM_Module * m, const char * name);
} apis[] = {
    { .module_load = &loadModule_0x1, .module_unload = &unloadModule_0x1,
      .module_init = &initModule_0x1, .module_deinit = &deinitModule_0x1,
      .module_get_num_syscalls = &getNumSyscalls_0x1,
      .module_get_syscall = &getSyscall_0x1,
      .module_find_syscall = &findSyscall_0x1,
      .module_get_num_pdks = &getNumPdks_0x1,
      .module_get_pdk = &getPdk_0x1,
      .module_find_pdk = &findPdk_0x1
    }
};


/*******************************************************************************
  SMVM_MODAPI
*******************************************************************************/

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
        SMVM_FacilityMap_init(&modapi->syscallFacilityMap, NULL);
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
    SMVM_FacilityMap_destroy(&modapi->syscallFacilityMap);
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

int SMVM_MODAPI_set_syscall_facility(SMVM_MODAPI * modapi, const char * name, void * facility) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&modapi->syscallFacilityMap, name, facility);
}

void * SMVM_MODAPI_get_syscall_facility(const SMVM_MODAPI * modapi, const char * name) {
    assert(modapi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&modapi->syscallFacilityMap, name);
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


/*******************************************************************************
  SMVM_Module
*******************************************************************************/

SMVM_Module * SMVM_Module_new(SMVM_MODAPI * modapi, const char * filename) {
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    if (!SMVM_MODAPI_ref(modapi))
        return NULL;

    const uint32_t (* modApiVersions)[];
    const char ** modName;
    const uint32_t * modVersion;
    size_t i;

    SMVM_Module * m = (SMVM_Module *) malloc(sizeof(SMVM_Module));
    if (unlikely(!m)) {
        OOM(modapi);
        SMVM_MODAPI_unref(modapi);
        return NULL;
    }
    m->apiData = NULL;
    m->moduleHandle = NULL;
    m->isInitialized = false;
    m->modapi = modapi;

    m->filename = strdup(filename);
    if (unlikely(!m->filename)) {
        OOM(modapi);
        goto SMVM_Module_new_fail_0;
    }

    /* Load module: */
    (void) dlerror();
    m->handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
    if (unlikely(!m->handle)) {
        SMVM_MODAPI_setErrorWithDynamicString(modapi, SMVM_MODAPI_UNABLE_TO_OPEN_MODULE, dlerror());
        goto SMVM_Module_new_fail_1;
    }

    /* Determine API version to use: */
    modApiVersions = (const uint32_t (*)[]) dlsym(m->handle, "SMVM_MOD_api_versions");
    if (unlikely(!modApiVersions || (*modApiVersions)[0] == 0u)) {
        SMVM_MODAPI_setErrorWithDynamicString(modapi, SMVM_MODAPI_INVALID_MODULE, dlerror());
        goto SMVM_Module_new_fail_2;
    }
    i = 0u;
    m->apiVersion = 0u;
    do {
        if ((*modApiVersions)[i] > m->apiVersion
            && (*modApiVersions)[i] >= SMVM_MODAPI_API_MIN_VERSION
            && (*modApiVersions)[i] <= SMVM_MODAPI_API_VERSION)
        {
            m->apiVersion = (*modApiVersions)[i];
        }
    } while ((*modApiVersions)[++i] == 0u);
    if (unlikely(m->apiVersion <= 0u)) {
        SMVM_MODAPI_setErrorWithStaticString(modapi, SMVM_MODAPI_API_NOT_SUPPORTED, "API not supported!");
        goto SMVM_Module_new_fail_2;
    }

    /* Determine module name: */
    modName = (const char **) dlsym(m->handle, "SMVM_MOD_name");
    if (unlikely(!modName)) {
        SMVM_MODAPI_setErrorWithDynamicString(modapi, SMVM_MODAPI_INVALID_MODULE, dlerror());
        goto SMVM_Module_new_fail_2;
    }
    if (unlikely(!*modName)) {
        SMVM_MODAPI_setErrorWithStaticString(modapi, SMVM_MODAPI_INVALID_MODULE, NULL);
        goto SMVM_Module_new_fail_2;
    }
    m->name = strdup(*modName);
    if (unlikely(!m->name)) {
        OOM(modapi);
        goto SMVM_Module_new_fail_2;
    }

    /* Determine module version: */
    modVersion = (const uint32_t *) dlsym(m->handle, "SMVM_MOD_version");
    if (unlikely(!modVersion)) {
        SMVM_MODAPI_setErrorWithDynamicString(modapi, SMVM_MODAPI_INVALID_MODULE, dlerror());
        goto SMVM_Module_new_fail_3;
    }
    m->version = *modVersion;

    /* Do API specific loading: */
    {
        SMVM_REFS_INIT(m);
        SMVM_MODAPI_Error status = (*(apis[m->apiVersion - 1u].module_load))(m);
        if (likely(status == SMVM_MODAPI_OK)) {
            m->facilityContext = NULL;
            SMVM_FacilityMap_init(&m->moduleFacilityMap, &modapi->moduleFacilityMap);
            SMVM_FacilityMap_init(&m->syscallFacilityMap, &modapi->syscallFacilityMap);
            SMVM_FacilityMap_init(&m->pdFacilityMap, &modapi->pdFacilityMap);
            SMVM_FacilityMap_init(&m->pdpiFacilityMap, &modapi->pdpiFacilityMap);
            return m;
        }

        SMVM_MODAPI_setErrorWithStaticString(modapi, status, NULL);
    }

SMVM_Module_new_fail_3:

    free(m->name);

SMVM_Module_new_fail_2:

    dlclose(m->handle);

SMVM_Module_new_fail_1:

    free(m->filename);

SMVM_Module_new_fail_0:

    SMVM_MODAPI_unref(modapi);
    free(m);
    return NULL;
}

void SMVM_Module_free(SMVM_Module * m) {
    assert(m);
    if (likely(m->isInitialized))
        SMVM_Module_mod_deinit(m);

    (*(apis[m->apiVersion - 1u].module_unload))(m);
    SMVM_REFS_ASSERT_IF_REFERENCED(m);
    free(m->name);
    dlclose(m->handle);
    free(m->filename);
    SMVM_MODAPI_unref(m->modapi);

    SMVM_FacilityMap_destroy(&m->moduleFacilityMap);
    SMVM_FacilityMap_destroy(&m->syscallFacilityMap);
    SMVM_FacilityMap_destroy(&m->pdFacilityMap);
    SMVM_FacilityMap_destroy(&m->pdpiFacilityMap);
    free(m);
}

SMVM_MODAPI_Error SMVM_Module_mod_init(SMVM_Module * m) {
    assert(m);
    assert(!m->isInitialized);
    SMVM_MODAPI_Error e = (*(apis[m->apiVersion - 1u].module_init))(m);
    if (likely(e == SMVM_MODAPI_OK)) {
        assert(m->moduleHandle);
        m->isInitialized = true;
    }

    return e;
}

void SMVM_Module_mod_deinit(SMVM_Module * m) {
    assert(m);
    (*(apis[m->apiVersion - 1u].module_deinit))(m);
    m->isInitialized = false;
}

void * SMVM_Module_get_handle(const SMVM_Module * m) {
    assert(m);
    return m->moduleHandle;
}

SMVM_MODAPI * SMVM_Module_get_modapi(const SMVM_Module * m) {
    assert(m);
    return m->modapi;
}

const char * SMVM_Module_get_name(const SMVM_Module * m) {
    assert(m);
    return m->name;
}

uint32_t SMVM_Module_get_api_version_in_use(const SMVM_Module * m) {
    assert(m);
    return m->apiVersion;
}

size_t SMVM_Module_get_num_syscalls(const SMVM_Module * m) {
    assert(m);
    return (*(apis[m->apiVersion - 1u].module_get_num_syscalls))(m);
}

SMVM_Syscall * SMVM_Module_get_syscall(const SMVM_Module * m, size_t index) {
    assert(m);
    return (*(apis[m->apiVersion - 1u].module_get_syscall))(m, index);
}

SMVM_Syscall * SMVM_Module_find_syscall(const SMVM_Module * m, const char * signature) {
    assert(m);
    return (*(apis[m->apiVersion - 1u].module_find_syscall))(m, signature);
}

size_t SMVM_Module_get_num_pdks(const SMVM_Module * m) {
    assert(m);
    return (*(apis[m->apiVersion - 1u].module_get_num_pdks))(m);
}

SMVM_PDK * SMVM_Module_get_pdk(const SMVM_Module * m, size_t index) {
    assert(m);
    return (*(apis[m->apiVersion - 1u].module_get_pdk))(m, index);
}

SMVM_PDK * SMVM_Module_find_pdk(const SMVM_Module * m, const char * name) {
    assert(m);
    return (*(apis[m->apiVersion - 1u].module_find_pdk))(m, name);
}

void SMVM_Module_set_facility_context(SMVM_Module * m, void * facilityContext) {
    assert(m);
    m->facilityContext = facilityContext;
}

void * SMVM_Module_get_facility_context(const SMVM_Module * m) {
    assert(m);
    return m->facilityContext;
}

int SMVM_Module_set_facility(SMVM_Module * m, const char * name, void * facility) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&m->moduleFacilityMap, name, facility);
}

void * SMVM_Module_get_facility(const SMVM_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&m->moduleFacilityMap, name);
}

int SMVM_Module_set_pd_facility(SMVM_Module * m, const char * name, void * facility) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&m->pdFacilityMap, name, facility);
}

void * SMVM_Module_get_pd_facility(const SMVM_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&m->pdFacilityMap, name);
}

int SMVM_Module_set_syscall_facility(SMVM_Module * m, const char * name, void * facility) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&m->syscallFacilityMap, name, facility);
}

void * SMVM_Module_get_syscall_facility(const SMVM_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&m->syscallFacilityMap, name);
}

int SMVM_Module_set_pdpi_facility(SMVM_Module * m, const char * name, void * facility) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&m->pdpiFacilityMap, name, facility);
}

void * SMVM_Module_get_pdpi_facility(const SMVM_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&m->pdpiFacilityMap, name);
}

SMVM_REFS_DEFINE_FUNCTIONS(SMVM_Module)
