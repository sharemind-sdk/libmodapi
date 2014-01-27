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
#include <string.h>
#include "modapi.h"


// Include API list:
#include "apis.c"


SharemindModule * SharemindModule_new(SharemindModuleApi * modapi,
                                      const char * filename,
                                      const char * conf)
{
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    if (!SharemindModuleApi_refs_ref(modapi)) {
        OOR(modapi);
        return NULL;
    }

    const SharemindModuleInfo * moduleInfo;
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

    if (likely(conf && conf[0])) {
        m->conf = strdup(conf);
        if (!m->conf)
            goto SharemindModule_new_fail_1;
    } else {
        m->conf = NULL;
    }

    /* Load module: */
    (void) dlerror();
    m->libHandle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    if (unlikely(!m->libHandle)) {
        SharemindModuleApi_set_error_with_dynamic_string(
                    modapi,
                    SHAREMIND_MODULE_API_UNABLE_TO_OPEN_MODULE,
                    dlerror());
        goto SharemindModule_new_fail_2;
    }

    /* Read module info: */
    moduleInfo = (const SharemindModuleInfo *) dlsym(m->libHandle,
                                                     "sharemindModuleInfo");
    if (unlikely(!moduleInfo)) {
        SharemindModuleApi_set_error_with_dynamic_string(
                    modapi,
                    SHAREMIND_MODULE_API_INVALID_MODULE,
                    dlerror());
        goto SharemindModule_new_fail_3;
    }

    /* Verify module name: */
    if (unlikely(!moduleInfo->moduleName[0])) {
        SharemindModuleApi_set_error_with_static_string(
                    modapi,
                    SHAREMIND_MODULE_API_API_NOT_SUPPORTED,
                    "Invalid module name!");
        goto SharemindModule_new_fail_3;
    }

    /*
      Verify module supported versions, determine API version compatibility and
      select API version to use:
    */
    if (unlikely(moduleInfo->supportedVersions[0] == 0u)) {
        SharemindModuleApi_set_error_with_static_string(
                    modapi,
                    SHAREMIND_MODULE_API_API_NOT_SUPPORTED,
                    "Invalid supported API list!");
        goto SharemindModule_new_fail_3;
    }
    i = 0u;
    m->apiVersion = 0u;
    do {
        if (moduleInfo->supportedVersions[i] > m->apiVersion
            && moduleInfo->supportedVersions[i]
               >= SHAREMIND_MODULE_API_API_MIN_VERSION
            && moduleInfo->supportedVersions[i]
               <= SHAREMIND_MODULE_API_API_VERSION)
        {
            m->apiVersion = moduleInfo->supportedVersions[i];
        }
    } while (moduleInfo->supportedVersions[++i] == 0u);
    if (unlikely(m->apiVersion <= 0u)) {
        SharemindModuleApi_set_error_with_static_string(
                    modapi,
                    SHAREMIND_MODULE_API_API_NOT_SUPPORTED,
                    "API not supported!");
        goto SharemindModule_new_fail_3;
    }
    m->api = &SharemindApis[m->apiVersion - 1u];

    /* Read module name: */
    m->name = strndup(moduleInfo->moduleName, sizeof(moduleInfo->moduleName));
    if (unlikely(!m->name)) {
        OOM(modapi);
        goto SharemindModule_new_fail_3;
    }

    /* Read module version: */
    m->version = moduleInfo->moduleVersion;

    /* Do API specific loading: */
    {
        SHAREMIND_REFS_INIT(m);
        SharemindModuleApiError status = (*(m->api->module_load))(m);
        if (unlikely(status != SHAREMIND_MODULE_API_OK)) {
            SharemindModuleApi_set_error_with_static_string(modapi,
                                                            status,
                                                            NULL);
            goto SharemindModule_new_fail_4;
        }

        SharemindFacilityMap_init(&m->moduleFacilityMap,
                                  &modapi->moduleFacilityMap);
        SharemindFacilityMap_init(&m->pdFacilityMap,
                                  &modapi->pdFacilityMap);
        SharemindFacilityMap_init(&m->pdpiFacilityMap,
                                  &modapi->pdpiFacilityMap);
        return m;
    }

SharemindModule_new_fail_4:

    free(m->name);

SharemindModule_new_fail_3:

    dlclose(m->libHandle);

SharemindModule_new_fail_2:

    if (likely(m->conf))
        free(m->conf);

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
    if (likely(m->conf))
        free(m->conf);
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

const char * SharemindModule_get_conf(const SharemindModule * m) {
    assert(m);
    assert(m->conf);
    assert(m->conf[0]);
    return m->conf;
}

uint32_t SharemindModule_get_api_version_in_use(const SharemindModule * m) {
    assert(m);
    return m->apiVersion;
}

size_t SharemindModule_get_num_syscalls(const SharemindModule * m) {
    assert(m);
    return (*(m->api->module_get_num_syscalls))(m);
}

SharemindSyscall * SharemindModule_get_syscall(const SharemindModule * m,
                                               size_t index)
{
    assert(m);
    return (*(m->api->module_get_syscall))(m, index);
}

SharemindSyscall * SharemindModule_find_syscall(const SharemindModule * m,
                                                const char * signature)
{
    assert(m);
    return (*(m->api->module_find_syscall))(m, signature);
}

size_t SharemindModule_get_num_pdks(const SharemindModule * m) {
    assert(m);
    return (*(m->api->module_get_num_pdks))(m);
}

SharemindPdk * SharemindModule_get_pdk(const SharemindModule * m,
                                       size_t index)
{
    assert(m);
    return (*(m->api->module_get_pdk))(m, index);
}

SharemindPdk * SharemindModule_find_pdk(const SharemindModule * m,
                                        const char * name)
{
    assert(m);
    return (*(m->api->module_find_pdk))(m, name);
}

bool SharemindModule_set_facility(SharemindModule * m,
                                  const char * name,
                                  void * facility,
                                  void * context)
{
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&m->moduleFacilityMap,
                                    name,
                                    facility,
                                    context);
}

const SharemindFacility * SharemindModule_get_facility(
        const SharemindModule * m,
        const char * name)
{
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&m->moduleFacilityMap, name);
}

bool SharemindModule_set_pd_facility(SharemindModule * m,
                                     const char * name,
                                     void * facility,
                                     void * context)
{
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&m->pdFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindModule_get_pd_facility(
        const SharemindModule * m,
        const char * name)
{
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&m->pdFacilityMap, name);
}

bool SharemindModule_set_pdpi_facility(SharemindModule * m,
                                       const char * name,
                                       void * facility,
                                       void * context)
{
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&m->pdpiFacilityMap,
                                    name,
                                    facility,
                                    context);
}

const SharemindFacility * SharemindModule_get_pdpi_facility(
        const SharemindModule * m,
        const char * name)
{
    assert(m);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&m->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SharemindModule)
