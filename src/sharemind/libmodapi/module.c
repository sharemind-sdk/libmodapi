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


SHAREMIND_Module * SHAREMIND_Module_new(SHAREMIND_MODAPI * modapi, const char * filename) {
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    if (!SHAREMIND_MODAPI_refs_ref(modapi)) {
        OOR(modapi);
        return NULL;
    }

    const uint32_t (* modApiVersions)[];
    const char ** modName;
    const uint32_t * modVersion;
    size_t i;

    SHAREMIND_Module * m = (SHAREMIND_Module *) malloc(sizeof(SHAREMIND_Module));
    if (unlikely(!m)) {
        OOM(modapi);
        SHAREMIND_MODAPI_refs_unref(modapi);
        return NULL;
    }
    m->apiData = NULL;
    m->moduleHandle = NULL;
    m->isInitialized = false;
    m->modapi = modapi;

    m->filename = strdup(filename);
    if (unlikely(!m->filename)) {
        OOM(modapi);
        goto SHAREMIND_Module_new_fail_0;
    }

    /* Load module: */
    (void) dlerror();
    m->handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
    if (unlikely(!m->handle)) {
        SHAREMIND_MODAPI_setErrorWithDynamicString(modapi, SHAREMIND_MODAPI_UNABLE_TO_OPEN_MODULE, dlerror());
        goto SHAREMIND_Module_new_fail_1;
    }

    /* Determine API version to use: */
    modApiVersions = (const uint32_t (*)[]) dlsym(m->handle, "SHAREMIND_MOD_api_versions");
    if (unlikely(!modApiVersions || (*modApiVersions)[0] == 0u)) {
        SHAREMIND_MODAPI_setErrorWithDynamicString(modapi, SHAREMIND_MODAPI_INVALID_MODULE, dlerror());
        goto SHAREMIND_Module_new_fail_2;
    }
    i = 0u;
    m->apiVersion = 0u;
    do {
        if ((*modApiVersions)[i] > m->apiVersion
            && (*modApiVersions)[i] >= SHAREMIND_MODAPI_API_MIN_VERSION
            && (*modApiVersions)[i] <= SHAREMIND_MODAPI_API_VERSION)
        {
            m->apiVersion = (*modApiVersions)[i];
        }
    } while ((*modApiVersions)[++i] == 0u);
    if (unlikely(m->apiVersion <= 0u)) {
        SHAREMIND_MODAPI_setErrorWithStaticString(modapi, SHAREMIND_MODAPI_API_NOT_SUPPORTED, "API not supported!");
        goto SHAREMIND_Module_new_fail_2;
    }
    m->api = &SHAREMIND_APIs[m->apiVersion - 1u];

    /* Determine module name: */
    modName = (const char **) dlsym(m->handle, "SHAREMIND_MOD_name");
    if (unlikely(!modName)) {
        SHAREMIND_MODAPI_setErrorWithDynamicString(modapi, SHAREMIND_MODAPI_INVALID_MODULE, dlerror());
        goto SHAREMIND_Module_new_fail_2;
    }
    if (unlikely(!*modName)) {
        SHAREMIND_MODAPI_setErrorWithStaticString(modapi, SHAREMIND_MODAPI_INVALID_MODULE, NULL);
        goto SHAREMIND_Module_new_fail_2;
    }
    m->name = strdup(*modName);
    if (unlikely(!m->name)) {
        OOM(modapi);
        goto SHAREMIND_Module_new_fail_2;
    }

    /* Determine module version: */
    modVersion = (const uint32_t *) dlsym(m->handle, "SHAREMIND_MOD_version");
    if (unlikely(!modVersion)) {
        SHAREMIND_MODAPI_setErrorWithDynamicString(modapi, SHAREMIND_MODAPI_INVALID_MODULE, dlerror());
        goto SHAREMIND_Module_new_fail_3;
    }
    m->version = *modVersion;

    /* Do API specific loading: */
    {
        SHAREMIND_REFS_INIT(m);
        SHAREMIND_MODAPI_Error status = (*(m->api->module_load))(m);
        if (likely(status == SHAREMIND_MODAPI_OK)) {
            SHAREMIND_FacilityMap_init(&m->moduleFacilityMap, &modapi->moduleFacilityMap);
            SHAREMIND_FacilityMap_init(&m->pdFacilityMap, &modapi->pdFacilityMap);
            SHAREMIND_FacilityMap_init(&m->pdpiFacilityMap, &modapi->pdpiFacilityMap);
            return m;
        }

        SHAREMIND_MODAPI_setErrorWithStaticString(modapi, status, NULL);
    }

SHAREMIND_Module_new_fail_3:

    free(m->name);

SHAREMIND_Module_new_fail_2:

    dlclose(m->handle);

SHAREMIND_Module_new_fail_1:

    free(m->filename);

SHAREMIND_Module_new_fail_0:

    SHAREMIND_MODAPI_refs_unref(modapi);
    free(m);
    return NULL;
}

void SHAREMIND_Module_free(SHAREMIND_Module * m) {
    assert(m);
    if (likely(m->isInitialized))
        SHAREMIND_Module_mod_deinit(m);

    (*(m->api->module_unload))(m);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    free(m->name);
    dlclose(m->handle);
    free(m->filename);
    SHAREMIND_MODAPI_refs_unref(m->modapi);

    SHAREMIND_FacilityMap_destroy(&m->moduleFacilityMap);
    SHAREMIND_FacilityMap_destroy(&m->pdFacilityMap);
    SHAREMIND_FacilityMap_destroy(&m->pdpiFacilityMap);
    free(m);
}

SHAREMIND_MODAPI_Error SHAREMIND_Module_mod_init(SHAREMIND_Module * m) {
    assert(m);
    assert(!m->isInitialized);
    SHAREMIND_MODAPI_Error e = (*(m->api->module_init))(m);
    if (likely(e == SHAREMIND_MODAPI_OK)) {
        assert(m->moduleHandle);
        m->isInitialized = true;
    }

    return e;
}

void SHAREMIND_Module_mod_deinit(SHAREMIND_Module * m) {
    assert(m);
    (*(m->api->module_deinit))(m);
    m->isInitialized = false;
}

void * SHAREMIND_Module_get_handle(const SHAREMIND_Module * m) {
    assert(m);
    return m->moduleHandle;
}

SHAREMIND_MODAPI * SHAREMIND_Module_get_modapi(const SHAREMIND_Module * m) {
    assert(m);
    return m->modapi;
}

const char * SHAREMIND_Module_get_name(const SHAREMIND_Module * m) {
    assert(m);
    return m->name;
}

uint32_t SHAREMIND_Module_get_api_version_in_use(const SHAREMIND_Module * m) {
    assert(m);
    return m->apiVersion;
}

size_t SHAREMIND_Module_get_num_syscalls(const SHAREMIND_Module * m) {
    assert(m);
    return (*(m->api->module_get_num_syscalls))(m);
}

SHAREMIND_Syscall * SHAREMIND_Module_get_syscall(const SHAREMIND_Module * m, size_t index) {
    assert(m);
    return (*(m->api->module_get_syscall))(m, index);
}

SHAREMIND_Syscall * SHAREMIND_Module_find_syscall(const SHAREMIND_Module * m, const char * signature) {
    assert(m);
    return (*(m->api->module_find_syscall))(m, signature);
}

size_t SHAREMIND_Module_get_num_pdks(const SHAREMIND_Module * m) {
    assert(m);
    return (*(m->api->module_get_num_pdks))(m);
}

SHAREMIND_PDK * SHAREMIND_Module_get_pdk(const SHAREMIND_Module * m, size_t index) {
    assert(m);
    return (*(m->api->module_get_pdk))(m, index);
}

SHAREMIND_PDK * SHAREMIND_Module_find_pdk(const SHAREMIND_Module * m, const char * name) {
    assert(m);
    return (*(m->api->module_find_pdk))(m, name);
}

int SHAREMIND_Module_set_facility(SHAREMIND_Module * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&m->moduleFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_Module_get_facility(const SHAREMIND_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&m->moduleFacilityMap, name);
}

int SHAREMIND_Module_set_pd_facility(SHAREMIND_Module * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&m->pdFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_Module_get_pd_facility(const SHAREMIND_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&m->pdFacilityMap, name);
}

int SHAREMIND_Module_set_pdpi_facility(SHAREMIND_Module * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&m->pdpiFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_Module_get_pdpi_facility(const SHAREMIND_Module * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&m->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SHAREMIND_Module)
