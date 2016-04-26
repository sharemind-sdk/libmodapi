/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#define SHAREMIND_INTERNAL_

#include "module.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "modapi.h"
#include "module_0x1.h"
#include "pd_0x1.h"
#include "pdpi_0x1.h"


static SharemindApi const SharemindApis[] = {
    { .moduleLoad = &SharemindModule_load_0x1,
      .moduleUnload = &SharemindModule_unload_0x1,
      .moduleInit = &SharemindModule_init_0x1,
      .moduleDeinit = &SharemindModule_deinit_0x1,
      .numSyscalls = &SharemindModule_numSyscalls_0x1,
      .syscall = &SharemindModule_syscall_0x1,
      .findSyscall = &SharemindModule_findSyscall_0x1,
      .numPdks = &SharemindModule_numPdks_0x1,
      .pdk = &SharemindModule_pdk_0x1,
      .findPdk = &SharemindModule_findPdk_0x1,
      .numPds = &SharemindModule_numPds_0x1,
      .pd = &SharemindModule_pd_0x1,
      .findPd = &SharemindModule_findPd_0x1,
      .startPd = &SharemindPd_start_0x1,
      .stopPd = &SharemindPd_stop_0x1,
      .startPdpi = &SharemindPdpi_start_0x1,
      .stopPdpi = &SharemindPdpi_stop_0x1
    }
};

SharemindModule * SharemindModuleApi_newModule(SharemindModuleApi * modapi,
                                               const char * filename,
                                               const char * conf)
{
    assert(modapi);
    assert(filename);
    assert(filename[0]);

    const SharemindModuleInfo * moduleInfo;
    size_t i;

    SharemindModule * const m =
            (SharemindModule *) malloc(sizeof(SharemindModule));
    if (unlikely(!m)) {
        SharemindModuleApi_setErrorOom(modapi);
        goto SharemindModule_new_fail_0;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(m)) {
        SharemindModuleApi_setErrorMie(modapi);
        goto SharemindModule_new_fail_1;
    }

    SHAREMIND_LIBMODAPI_LASTERROR_INIT(m);
    SHAREMIND_TAG_INIT(m);

    m->apiData = NULL;
    m->moduleHandle = NULL;
    m->isInitialized = false;
    m->modapi = modapi;

    m->filename = strdup(filename);
    if (unlikely(!m->filename)) {
        SharemindModuleApi_setErrorOom(modapi);
        goto SharemindModule_new_fail_2;
    }

    if (likely(conf && conf[0])) {
        m->conf = strdup(conf);
        if (!m->conf) {
            SharemindModuleApi_setErrorOom(modapi);
            goto SharemindModule_new_fail_3;
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
                    dlerror());
        goto SharemindModule_new_fail_4;
    }

    /* Read module info: */
    moduleInfo = (const SharemindModuleInfo *) dlsym(m->libHandle,
                                                     "sharemindModuleInfo");
    if (unlikely(!moduleInfo)) {
        SharemindModuleApi_setError(
                    modapi,
                    SHAREMIND_MODULE_API_INVALID_MODULE,
                    "The \"sharemindModuleInfo\" symbol was not found!");
        goto SharemindModule_new_fail_5;
    }

    /* Verify module name: */
    if (unlikely(!moduleInfo->moduleName[0])) {
        SharemindModuleApi_setError(
                    modapi,
                    SHAREMIND_MODULE_API_API_NOT_SUPPORTED,
                    "Invalid module name!");
        goto SharemindModule_new_fail_5;
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
        goto SharemindModule_new_fail_5;
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
        goto SharemindModule_new_fail_5;
    }
    m->api = &SharemindApis[m->apiVersion - 1u];

    /* Read module name: */
    m->name = strndup(moduleInfo->moduleName, sizeof(moduleInfo->moduleName));
    if (unlikely(!m->name)) {
        SharemindModuleApi_setErrorOom(modapi);
        goto SharemindModule_new_fail_5;
    }

    /* Read module version: */
    m->version = moduleInfo->moduleVersion;

    SharemindFacilityMap_init(&m->facilityMap,
                              &modapi->moduleFacilityMap);
    SharemindFacilityMap_init(&m->pdFacilityMap,
                              &modapi->pdFacilityMap);
    SharemindFacilityMap_init(&m->pdpiFacilityMap,
                              &modapi->pdpiFacilityMap);
    #ifndef NDEBUG
    SHAREMIND_REFS_INIT(m);
    #endif

    /* Do API specific loading: */
    SharemindModuleApi_lock(modapi);
    if (unlikely(!(*(m->api->moduleLoad))(m)))
        goto SharemindModule_new_fail_6;

    /* Register with SharemindModuleApi: */
    if (unlikely(!SharemindModulesSet_insertNew(&modapi->modules, m))) {
        SharemindModuleApi_setErrorOom(modapi);
        goto SharemindModule_new_fail_7;
    }
    SharemindModuleApi_unlock(modapi);
    return m;

SharemindModule_new_fail_7:

    (*(m->api->moduleUnload))(m);

SharemindModule_new_fail_6:

    SharemindModuleApi_unlock(modapi);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    SharemindFacilityMap_destroy(&m->pdpiFacilityMap);
    SharemindFacilityMap_destroy(&m->pdFacilityMap);
    SharemindFacilityMap_destroy(&m->facilityMap);
    free(m->name);

SharemindModule_new_fail_5:

    dlclose(m->libHandle);

SharemindModule_new_fail_4:

    free(m->conf);

SharemindModule_new_fail_3:

    free(m->filename);

SharemindModule_new_fail_2:

    SHAREMIND_RECURSIVE_LOCK_DEINIT(m);

SharemindModule_new_fail_1:

    free(m);

SharemindModule_new_fail_0:

    return NULL;
}

void SharemindModule_free(SharemindModule * m) {
    assert(m);

    if (likely(m->isInitialized))
        SharemindModule_deinit(m);

    SharemindModuleApi_lock(m->modapi);
    SharemindModulesSet_remove(&m->modapi->modules, m);
    SharemindModuleApi_unlock(m->modapi);
    (*(m->api->moduleUnload))(m);

    SHAREMIND_TAG_DESTROY(m);
    #ifndef NDEBUG
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(m);
    #endif
    free(m->name);
    dlclose(m->libHandle);
    if (likely(m->conf))
        free(m->conf);
    free(m->filename);

    SharemindFacilityMap_destroy(&m->facilityMap);
    SharemindFacilityMap_destroy(&m->pdFacilityMap);
    SharemindFacilityMap_destroy(&m->pdpiFacilityMap);
    SHAREMIND_RECURSIVE_LOCK_DEINIT(m);
    free(m);
}

SharemindModuleApiError SharemindModule_init(SharemindModule * m) {
    assert(m);
    SharemindModule_lock(m);
    if (m->isInitialized) {
        SharemindModule_unlock(m);
        return SHAREMIND_MODULE_API_OK;
    }
    const SharemindModuleApiError e = (*(m->api->moduleInit))(m);
    if (likely(e == SHAREMIND_MODULE_API_OK)) {
        assert(m->moduleHandle);
        m->isInitialized = true;
    }
    SharemindModule_unlock(m);
    return e;
}

void SharemindModule_deinit(SharemindModule * m) {
    assert(m);
    SharemindModule_lock(m);
    (*(m->api->moduleDeinit))(m);
    m->isInitialized = false;
    SharemindModule_unlock(m);
}

bool SharemindModule_isInitialized(const SharemindModule * m) {
    assert(m);
    SharemindModule_lockConst(m);
    const bool r = m->isInitialized;
    SharemindModule_unlockConst(m);
    return r;
}

void * SharemindModule_handle(const SharemindModule * m) {
    assert(m);
    SharemindModule_lockConst(m);
    void * const r = m->moduleHandle;
    SharemindModule_unlockConst(m);
    return r;
}

SharemindModuleApi * SharemindModule_moduleApi(const SharemindModule * m) {
    assert(m);
    return m->modapi; // No locking: const after SharemindModuleApi_newModule.
}

const char * SharemindModule_filename(const SharemindModule * m) {
    assert(m);
    return m->filename; // No locking: const after SharemindModuleApi_newModule.
}

const char * SharemindModule_name(const SharemindModule * m) {
    assert(m);
    return m->name; // No locking: const after SharemindModuleApi_newModule.
}

const char * SharemindModule_conf(const SharemindModule * m) {
    assert(m);
    assert(m->conf);
    assert(m->conf[0]);
    return m->conf; // No locking: const after SharemindModuleApi_newModule.
}

uint32_t SharemindModule_apiVersionInUse(const SharemindModule * m) {
    assert(m);
    // No locking: const after SharemindModuleApi_newModule:
    return m->apiVersion;
}

size_t SharemindModule_numSyscalls(const SharemindModule * m) {
    assert(m);
    // No locking: all const after SharemindModuleApi_newModule.
    return (*(m->api->numSyscalls))(m);
}

SharemindSyscall * SharemindModule_syscall(const SharemindModule * m,
                                           size_t index)
{
    assert(m);
    // No locking: all const after SharemindModuleApi_newModule.
    return (*(m->api->syscall))(m, index);
}

SharemindSyscall * SharemindModule_findSyscall(const SharemindModule * m,
                                               const char * signature)
{
    assert(m);
    // No locking: all const after SharemindModuleApi_newModule.
    return (*(m->api->findSyscall))(m, signature);
}

SharemindSyscallWrapper SharemindModule_syscallWrapper(
        const SharemindModule * m,
        const char * signature)
{
    assert(m);
    assert(signature);
    SharemindModule_lockConst(m);
    SharemindSyscall * const sc = (*(m->api->findSyscall))(m, signature);
    if (sc) {
        const SharemindSyscallWrapper result = {
            SharemindSyscall_wrapper(sc).callable,
            m->moduleHandle
        };
        SharemindModule_unlockConst(m);
        return result;
    }
    SharemindModule_unlockConst(m);
    static const SharemindSyscallWrapper nullWrapper = { NULL, NULL };
    return nullWrapper;
}

size_t SharemindModule_numPdks(const SharemindModule * m) {
    assert(m);
    // No locking: all const after SharemindModuleApi_newModule.
    return (*(m->api->numPdks))(m);
}

SharemindPdk * SharemindModule_pdk(const SharemindModule * m, size_t index) {
    assert(m);
    // No locking: all const after SharemindModuleApi_newModule.
    return (*(m->api->pdk))(m, index);
}

SharemindPdk * SharemindModule_findPdk(const SharemindModule * m,
                                       const char * name)
{
    assert(m);
    // No locking: all const after SharemindModuleApi_newModule.
    return (*(m->api->findPdk))(m, name);
}

size_t SharemindModule_numPds(const SharemindModule * m) {
    assert(m);
    SharemindModule_lockConst(m);
    size_t const r = (*(m->api->numPds))(m);
    SharemindModule_unlockConst(m);
    return r;
}

SharemindPd * SharemindModule_pd(const SharemindModule * m, size_t index) {
    assert(m);
    SharemindModule_lockConst(m);
    SharemindPd * const r = (*(m->api->pd))(m, index);
    SharemindModule_unlockConst(m);
    return r;
}

SharemindPd * SharemindModule_findPd(const SharemindModule * m,
                                     const char * name)
{
    assert(m);
    SharemindModule_lockConst(m);
    SharemindPd * const r = (*(m->api->findPd))(m, name);
    SharemindModule_unlockConst(m);
    return r;
}

SHAREMIND_LIBMODAPI_LASTERROR_FUNCTIONS_DEFINE(SharemindModule)

SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(SharemindModule)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModule,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModule,pdpi,Pdpi)

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindModule,)

#ifndef NDEBUG
SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindModule)
#endif
