/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "module.h"

#include <dlfcn.h>
#include "modapi.h"


SMVM_Module * SMVM_Module_new(SMVM_MODAPI * modapi, const char * filename) {
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    if (!SMVM_MODAPI_refs_ref(modapi)) {
        OOR(modapi);
        return NULL;
    }

    const uint32_t (* modApiVersions)[];
    const char ** modName;
    const uint32_t * modVersion;
    size_t i;

    SMVM_Module * m = (SMVM_Module *) malloc(sizeof(SMVM_Module));
    if (unlikely(!m)) {
        OOM(modapi);
        SMVM_MODAPI_refs_unref(modapi);
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
    m->api = &SMVM_APIs[m->apiVersion - 1u];

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
        SHAREMIND_REFS_INIT(m);
        SMVM_MODAPI_Error status = (*(m->api->module_load))(m);
        if (likely(status == SMVM_MODAPI_OK)) {
            SMVM_FacilityMap_init(&m->moduleFacilityMap, &modapi->moduleFacilityMap);
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

    SMVM_MODAPI_refs_unref(modapi);
    free(m);
    return NULL;
}

void SMVM_Module_free(SMVM_Module * m) {
    assert(m);
    if (likely(m->isInitialized))
        SMVM_Module_mod_deinit(m);

    (*(m->api->module_unload))(m);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    free(m->name);
    dlclose(m->handle);
    free(m->filename);
    SMVM_MODAPI_refs_unref(m->modapi);

    SMVM_FacilityMap_destroy(&m->moduleFacilityMap);
    SMVM_FacilityMap_destroy(&m->pdFacilityMap);
    SMVM_FacilityMap_destroy(&m->pdpiFacilityMap);
    free(m);
}

SMVM_MODAPI_Error SMVM_Module_mod_init(SMVM_Module * m) {
    assert(m);
    assert(!m->isInitialized);
    SMVM_MODAPI_Error e = (*(m->api->module_init))(m);
    if (likely(e == SMVM_MODAPI_OK)) {
        assert(m->moduleHandle);
        m->isInitialized = true;
    }

    return e;
}

void SMVM_Module_mod_deinit(SMVM_Module * m) {
    assert(m);
    (*(m->api->module_deinit))(m);
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
    return (*(m->api->module_get_num_syscalls))(m);
}

SMVM_Syscall * SMVM_Module_get_syscall(const SMVM_Module * m, size_t index) {
    assert(m);
    return (*(m->api->module_get_syscall))(m, index);
}

SMVM_Syscall * SMVM_Module_find_syscall(const SMVM_Module * m, const char * signature) {
    assert(m);
    return (*(m->api->module_find_syscall))(m, signature);
}

size_t SMVM_Module_get_num_pdks(const SMVM_Module * m) {
    assert(m);
    return (*(m->api->module_get_num_pdks))(m);
}

SMVM_PDK * SMVM_Module_get_pdk(const SMVM_Module * m, size_t index) {
    assert(m);
    return (*(m->api->module_get_pdk))(m, index);
}

SMVM_PDK * SMVM_Module_find_pdk(const SMVM_Module * m, const char * name) {
    assert(m);
    return (*(m->api->module_find_pdk))(m, name);
}

int SMVM_Module_set_facility(SMVM_Module * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&m->moduleFacilityMap, name, facility, context);
}

const SMVM_Facility * SMVM_Module_get_facility(const SMVM_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&m->moduleFacilityMap, name);
}

int SMVM_Module_set_pd_facility(SMVM_Module * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&m->pdFacilityMap, name, facility, context);
}

const SMVM_Facility * SMVM_Module_get_pd_facility(const SMVM_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&m->pdFacilityMap, name);
}

int SMVM_Module_set_pdpi_facility(SMVM_Module * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&m->pdpiFacilityMap, name, facility, context);
}

const SMVM_Facility * SMVM_Module_get_pdpi_facility(const SMVM_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&m->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SMVM_Module)
