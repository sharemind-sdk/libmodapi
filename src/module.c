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
#include <stdlib.h>
#include <string.h>
#include "modapi.h"


// Include API list:
#include "apis.c"


SharemindModule * SharemindModuleApi_newModule(SharemindModuleApi * modapi,
                                               const char * filename,
                                               const char * conf)
{
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    SharemindModule * m;
    const SharemindModuleInfo * moduleInfo;
    size_t i;

    if (!SharemindModuleApi_refs_ref(modapi)) {
        SharemindModuleApi_setErrorOor(modapi);
        goto SharemindModule_new_fail_0;
    }

    m = (SharemindModule *) malloc(sizeof(SharemindModule));
    if (unlikely(!m)) {
        SharemindModuleApi_setErrorOom(modapi);
        goto SharemindModule_new_fail_1;
    }
    if (unlikely(SharemindRecursiveMutex_init(&m->mutex) != SHAREMIND_MUTEX_OK)) {
        SharemindModuleApi_setErrorMie(modapi);
        goto SharemindModule_new_fail_2;
    }

    m->apiData = NULL;
    m->moduleHandle = NULL;
    m->isInitialized = false;
    m->modapi = modapi;

    m->filename = strdup(filename);
    if (unlikely(!m->filename)) {
        SharemindModuleApi_setErrorOom(modapi);
        goto SharemindModule_new_fail_3;
    }

    if (likely(conf && conf[0])) {
        m->conf = strdup(conf);
        if (!m->conf) {
            SharemindModuleApi_setErrorOom(modapi);
            goto SharemindModule_new_fail_4;
        }
    } else {
        m->conf = NULL;
    }

    /* Load module: */
    m->libHandle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    if (unlikely(!m->libHandle)) {
        SharemindModuleApi_setError(
                    modapi,
                    SHAREMIND_MODULE_API_UNABLE_TO_OPEN_MODULE,
                    "dlopen() failed!");
        goto SharemindModule_new_fail_5;
    }

    /* Read module info: */
    moduleInfo = (const SharemindModuleInfo *) dlsym(m->libHandle,
                                                     "sharemindModuleInfo");
    if (unlikely(!moduleInfo)) {
        SharemindModuleApi_setError(
                    modapi,
                    SHAREMIND_MODULE_API_INVALID_MODULE,
                    "The \"sharemindModuleInfo\" symbol was not found!");
        goto SharemindModule_new_fail_6;
    }

    /* Verify module name: */
    if (unlikely(!moduleInfo->moduleName[0])) {
        SharemindModuleApi_setError(
                    modapi,
                    SHAREMIND_MODULE_API_API_NOT_SUPPORTED,
                    "Invalid module name!");
        goto SharemindModule_new_fail_6;
    }

    /*
      Verify module supported versions, determine API version compatibility and
      select API version to use:
    */
    if (unlikely((moduleInfo->supportedVersions[0u] == 0u)
                 || (moduleInfo->supportedVersions[15u] != 0u)))
    {
        SharemindModuleApi_setError(
                    modapi,
                    SHAREMIND_MODULE_API_API_NOT_SUPPORTED,
                    "Invalid supported API list!");
        goto SharemindModule_new_fail_6;
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
    } while (moduleInfo->supportedVersions[++i] != 0u);
    if (unlikely(m->apiVersion <= 0u)) {
        SharemindModuleApi_setError(
                    modapi,
                    SHAREMIND_MODULE_API_API_NOT_SUPPORTED,
                    "API not supported!");
        goto SharemindModule_new_fail_6;
    }
    m->api = &SharemindApis[m->apiVersion - 1u];

    /* Read module name: */
    m->name = strndup(moduleInfo->moduleName, sizeof(moduleInfo->moduleName));
    if (unlikely(!m->name)) {
        SharemindModuleApi_setErrorOom(modapi);
        goto SharemindModule_new_fail_6;
    }

    /* Read module version: */
    m->version = moduleInfo->moduleVersion;

    /* Do API specific loading: */
    {
        #ifndef NDEBUG
        SHAREMIND_REFS_INIT(m);
        #endif
        if (unlikely(!(*(m->api->moduleLoad))(m)))
            goto SharemindModule_new_fail_7;

        SharemindFacilityMap_init(&m->facilityMap,
                                  &modapi->moduleFacilityMap);
        SharemindFacilityMap_init(&m->pdFacilityMap,
                                  &modapi->pdFacilityMap);
        SharemindFacilityMap_init(&m->pdpiFacilityMap,
                                  &modapi->pdpiFacilityMap);
        return m;
    }

SharemindModule_new_fail_7:

    free(m->name);

SharemindModule_new_fail_6:

    dlclose(m->libHandle);

SharemindModule_new_fail_5:

    if (likely(m->conf))
        free(m->conf);

SharemindModule_new_fail_4:

    free(m->filename);

SharemindModule_new_fail_3:

    if (unlikely(SharemindRecursiveMutex_destroy(&m->mutex) != SHAREMIND_MUTEX_OK))
        abort();

SharemindModule_new_fail_2:

    free(m);

SharemindModule_new_fail_1:

    SharemindModuleApi_refs_unref(modapi);

SharemindModule_new_fail_0:

    return NULL;
}

void SharemindModule_free(SharemindModule * m) {
    assert(m);
    if (likely(m->isInitialized))
        SharemindModule_deinit(m);

    (*(m->api->moduleUnload))(m);
    #ifndef NDEBUG
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    #endif
    free(m->name);
    dlclose(m->libHandle);
    if (likely(m->conf))
        free(m->conf);
    free(m->filename);
    SharemindModuleApi_refs_unref(m->modapi);

    SharemindFacilityMap_destroy(&m->facilityMap);
    SharemindFacilityMap_destroy(&m->pdFacilityMap);
    SharemindFacilityMap_destroy(&m->pdpiFacilityMap);
    if (unlikely(SharemindRecursiveMutex_destroy(&m->mutex) != SHAREMIND_MUTEX_OK))
        abort();
    free(m);
}

#define DOLOCK(module,lock) \
    if (unlikely(SharemindRecursiveMutex_ ## lock(&(module)->mutex) != SHAREMIND_MUTEX_OK)) { \
        abort(); \
    } else (void) 0
#define LOCK(module) DOLOCK((module),lock)
#define UNLOCK(module) DOLOCK((module),unlock)
#define LOCK_CONST(module) DOLOCK((module),lock_const)
#define UNLOCK_CONST(module) DOLOCK((module),unlock_const)

SHAREMIND_LASTERROR_DEFINE_FUNCTIONS(Module)

SharemindModuleApiError SharemindModule_init(SharemindModule * m) {
    assert(m);
    LOCK(m);
    assert(!m->isInitialized);
    SharemindModuleApiError e;
    if (likely((*(m->api->moduleInit))(m))) {
        assert(m->moduleHandle);
        m->isInitialized = true;
        e = SHAREMIND_MODULE_API_OK;
    } else {
        e = m->lastError;
    }
    UNLOCK(m);
    return e;
}

void SharemindModule_deinit(SharemindModule * m) {
    assert(m);
    LOCK(m);
    (*(m->api->moduleDeinit))(m);
    m->isInitialized = false;
    UNLOCK(m);
}

bool SharemindModule_isInitialized(const SharemindModule * m) {
    assert(m);
    LOCK_CONST(m);
    const bool r = m->isInitialized;
    UNLOCK_CONST(m);
    return r;
}

void * SharemindModule_handle(const SharemindModule * m) {
    assert(m);
    LOCK_CONST(m);
    void * const r = m->moduleHandle;
    UNLOCK_CONST(m);
    return r;
}

SharemindModuleApi * SharemindModule_modapi(const SharemindModule * m) {
    assert(m);
    return m->modapi; // No locking: const after SharemindModule_new.
}

const char * SharemindModule_filename(const SharemindModule * m) {
    assert(m);
    return m->filename; // No locking: const after SharemindModule_new.
}

const char * SharemindModule_name(const SharemindModule * m) {
    assert(m);
    return m->name; // No locking: const after SharemindModule_new.
}

const char * SharemindModule_conf(const SharemindModule * m) {
    assert(m);
    assert(m->conf);
    assert(m->conf[0]);
    return m->conf; // No locking: const after SharemindModule_new.
}

uint32_t SharemindModule_apiVersionInUse(const SharemindModule * m) {
    assert(m);
    return m->apiVersion; // No locking: const after SharemindModule_new.
}

size_t SharemindModule_numSyscalls(const SharemindModule * m) {
    assert(m);
    // No locking: all const after SharemindModule_new.
    return (*(m->api->numSyscalls))(m);
}

SharemindSyscall * SharemindModule_syscall(const SharemindModule * m,
                                           size_t index)
{
    assert(m);
    // No locking: all const after SharemindModule_new.
    return (*(m->api->syscall))(m, index);
}

SharemindSyscall * SharemindModule_findSyscall(const SharemindModule * m,
                                               const char * signature)
{
    assert(m);
    // No locking: all const after SharemindModule_new.
    return (*(m->api->findSyscall))(m, signature);
}

size_t SharemindModule_numPdks(const SharemindModule * m) {
    assert(m);
    // No locking: all const after SharemindModule_new.
    return (*(m->api->numPdks))(m);
}

SharemindPdk * SharemindModule_pdk(const SharemindModule * m, size_t index) {
    assert(m);
    // No locking: all const after SharemindModule_new.
    return (*(m->api->pdk))(m, index);
}

SharemindPdk * SharemindModule_findPdk(const SharemindModule * m,
                                       const char * name)
{
    assert(m);
    // No locking: all const after SharemindModule_new.
    return (*(m->api->findPdk))(m, name);
}

SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(Module)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(Module,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(Module,pdpi,Pdpi)

#ifndef NDEBUG
SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindModule)
#endif
