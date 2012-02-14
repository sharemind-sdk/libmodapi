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


SharemindModule * SharemindModule_new(SharemindModuleApi * modapi, const char * filename) {
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    if (!SharemindModuleApi_refs_ref(modapi)) {
        OOR(modapi);
        return NULL;
    }

    const uint32_t (* modApiVersions)[];
    const char ** modName;
    const uint32_t * modVersion;
    size_t i;

    SharemindModule * m = (SharemindModule *) malloc(sizeof(SharemindModule));
    if (unlikely(!m)) {
        OOM(modapi);
        SharemindModuleApi_refs_unref(modapi);
        return NULL;
    }
    m->apiData = NULL;
    m->moduleHandle = NULL;
    m->isInitialized = false;
    m->modapi = modapi;

    m->filename = strdup(filename);
    if (unlikely(!m->filename)) {
        OOM(modapi);
        goto SharemindModule_new_fail_0;
    }

    /* Load module: */
    (void) dlerror();
    m->libHandle = dlopen(filename, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
    if (unlikely(!m->libHandle)) {
        SharemindModuleApi_set_error_with_dynamic_string(modapi, SHAREMIND_MODULE_API_UNABLE_TO_OPEN_MODULE, dlerror());
        goto SharemindModule_new_fail_1;
    }

    /* Determine API version to use: */
    modApiVersions = (const uint32_t (*)[]) dlsym(m->libHandle, "sharemindModuleApiSupportedVersions");
    if (unlikely(!modApiVersions || (*modApiVersions)[0] == 0u)) {
        SharemindModuleApi_set_error_with_dynamic_string(modapi, SHAREMIND_MODULE_API_INVALID_MODULE, dlerror());
        goto SharemindModule_new_fail_2;
    }
    i = 0u;
    m->apiVersion = 0u;
    do {
        if ((*modApiVersions)[i] > m->apiVersion
            && (*modApiVersions)[i] >= SHAREMIND_MODULE_API_API_MIN_VERSION
            && (*modApiVersions)[i] <= SHAREMIND_MODULE_API_API_VERSION)
        {
            m->apiVersion = (*modApiVersions)[i];
        }
    } while ((*modApiVersions)[++i] == 0u);
    if (unlikely(m->apiVersion <= 0u)) {
        SharemindModuleApi_set_error_with_static_string(modapi, SHAREMIND_MODULE_API_API_NOT_SUPPORTED, "API not supported!");
        goto SharemindModule_new_fail_2;
    }
    m->api = &SHAREMIND_APIs[m->apiVersion - 1u];

    /* Determine module name: */
    modName = (const char **) dlsym(m->libHandle, "sharemindModuleApiModuleName");
    if (unlikely(!modName)) {
        SharemindModuleApi_set_error_with_dynamic_string(modapi, SHAREMIND_MODULE_API_INVALID_MODULE, dlerror());
        goto SharemindModule_new_fail_2;
    }
    if (unlikely(!*modName)) {
        SharemindModuleApi_set_error_with_static_string(modapi, SHAREMIND_MODULE_API_INVALID_MODULE, NULL);
        goto SharemindModule_new_fail_2;
    }
    m->name = strdup(*modName);
    if (unlikely(!m->name)) {
        OOM(modapi);
        goto SharemindModule_new_fail_2;
    }

    /* Determine module version: */
    modVersion = (const uint32_t *) dlsym(m->libHandle, "sharemindModuleApiModuleVersion");
    if (unlikely(!modVersion)) {
        SharemindModuleApi_set_error_with_dynamic_string(modapi, SHAREMIND_MODULE_API_INVALID_MODULE, dlerror());
        goto SharemindModule_new_fail_3;
    }
    m->version = *modVersion;

    /* Do API specific loading: */
    {
        SHAREMIND_REFS_INIT(m);
        SharemindModuleApiError status = (*(m->api->module_load))(m);
        if (likely(status == SHAREMIND_MODULE_API_OK)) {
            SharemindFacilityMap_init(&m->moduleFacilityMap, &modapi->moduleFacilityMap);
            SharemindFacilityMap_init(&m->pdFacilityMap, &modapi->pdFacilityMap);
            SharemindFacilityMap_init(&m->pdpiFacilityMap, &modapi->pdpiFacilityMap);
            return m;
        }

        SharemindModuleApi_set_error_with_static_string(modapi, status, NULL);
    }

SharemindModule_new_fail_3:

    free(m->name);

SharemindModule_new_fail_2:

    dlclose(m->libHandle);

SharemindModule_new_fail_1:

    free(m->filename);

SharemindModule_new_fail_0:

    SharemindModuleApi_refs_unref(modapi);
    free(m);
    return NULL;
}

void SharemindModule_free(SharemindModule * m) {
    assert(m);
    if (likely(m->isInitialized))
        SharemindModule_mod_deinit(m);

    (*(m->api->module_unload))(m);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    free(m->name);
    dlclose(m->libHandle);
    free(m->filename);
    SharemindModuleApi_refs_unref(m->modapi);

    SharemindFacilityMap_destroy(&m->moduleFacilityMap);
    SharemindFacilityMap_destroy(&m->pdFacilityMap);
    SharemindFacilityMap_destroy(&m->pdpiFacilityMap);
    free(m);
}

SharemindModuleApiError SharemindModule_mod_init(SharemindModule * m) {
    assert(m);
    assert(!m->isInitialized);
    SharemindModuleApiError e = (*(m->api->module_init))(m);
    if (likely(e == SHAREMIND_MODULE_API_OK)) {
        assert(m->moduleHandle);
        m->isInitialized = true;
    }

    return e;
}

void SharemindModule_mod_deinit(SharemindModule * m) {
    assert(m);
    (*(m->api->module_deinit))(m);
    m->isInitialized = false;
}

bool SharemindModule_mod_is_initialized(const SharemindModule * m) {
    assert(m);
    return m->isInitialized;
}

void * SharemindModule_get_handle(const SharemindModule * m) {
    assert(m);
    return m->moduleHandle;
}

SharemindModuleApi * SharemindModule_get_modapi(const SharemindModule * m) {
    assert(m);
    return m->modapi;
}

const char * SharemindModule_get_filename(const SharemindModule * m) {
    assert(m);
    return m->filename;
}

const char * SharemindModule_get_name(const SharemindModule * m) {
    assert(m);
    return m->name;
}

uint32_t SharemindModule_get_api_version_in_use(const SharemindModule * m) {
    assert(m);
    return m->apiVersion;
}

size_t SharemindModule_get_num_syscalls(const SharemindModule * m) {
    assert(m);
    return (*(m->api->module_get_num_syscalls))(m);
}

SharemindSyscall * SharemindModule_get_syscall(const SharemindModule * m, size_t index) {
    assert(m);
    return (*(m->api->module_get_syscall))(m, index);
}

SharemindSyscall * SharemindModule_find_syscall(const SharemindModule * m, const char * signature) {
    assert(m);
    return (*(m->api->module_find_syscall))(m, signature);
}

size_t SharemindModule_get_num_pdks(const SharemindModule * m) {
    assert(m);
    return (*(m->api->module_get_num_pdks))(m);
}

SharemindPdk * SharemindModule_get_pdk(const SharemindModule * m, size_t index) {
    assert(m);
    return (*(m->api->module_get_pdk))(m, index);
}

SharemindPdk * SharemindModule_find_pdk(const SharemindModule * m, const char * name) {
    assert(m);
    return (*(m->api->module_find_pdk))(m, name);
}

int SharemindModule_set_facility(SharemindModule * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&m->moduleFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindModule_get_facility(const SharemindModule * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&m->moduleFacilityMap, name);
}

int SharemindModule_set_pd_facility(SharemindModule * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&m->pdFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindModule_get_pd_facility(const SharemindModule * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&m->pdFacilityMap, name);
}

int SharemindModule_set_pdpi_facility(SharemindModule * m, const char * name, void * facility, void * context) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&m->pdpiFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindModule_get_pdpi_facility(const SharemindModule * m, const char * name) {
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&m->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SharemindModule)
