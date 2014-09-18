/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "module_0x1.h"

#include <assert.h>
#include <dlfcn.h>
#include <sharemind/stringmap.h>
#include "modapi.h"
#include "module.h"
#include "pdk.h"
#include "syscall.h"


SHAREMIND_STRINGMAP_DECLARE(SharemindPdkMap,SharemindPdk,)
SHAREMIND_STRINGMAP_DEFINE(SharemindPdkMap,SharemindPdk,malloc,free,strdup,)

SHAREMIND_MAP_DECLARE(SharemindSyscallMap,
                      char *,
                      const char * const,
                      SharemindSyscall,)
SHAREMIND_STRINGMAP_DEFINE(SharemindSyscallMap,
                           SharemindSyscall,
                           malloc,
                           free,
                           strdup,)


typedef SharemindModuleApi0x1ModuleContext ModuleContext;
typedef SharemindModuleApi0x1ModuleInitializer ModuleInitializer;
typedef SharemindModuleApi0x1ModuleDeinitializer ModuleDeinitializer;
typedef SharemindModuleApi0x1SyscallDefinitions SyscallDefinitions;
typedef SharemindModuleApi0x1PdkDefinitions PdkDefinitions;


static const SharemindFacility * SharemindModule_facilityWrapper(
        ModuleContext * w,
        const char * name)
{
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SharemindModule_facility((SharemindModule *) w->internal, name);
}

static void SharemindSyscallMap_destroyer(const char * const * key,
                                          SharemindSyscall * value)
{
    (void) key;
    SharemindSyscall_destroy(value);
}

static void SharemindPdkMap_destroyer(const char * const * key,
                                      SharemindPdk * value)
{
    (void) key;
    SharemindPdk_destroy(value);
}


/** API 0x1 data */
typedef struct {
    ModuleInitializer initializer;
    ModuleDeinitializer deinitializer;
    SharemindSyscallMap syscalls;
    SharemindPdkMap pdks;
} ApiData;

bool SharemindModule_load_0x1(SharemindModule * m) {
    assert(m);

    const PdkDefinitions * pdks;
    const SyscallDefinitions * scs;

    ApiData * const apiData = (ApiData *) malloc(sizeof(ApiData));
    if (unlikely(!apiData)) {
        SharemindModuleApi_setErrorOom(m->modapi);
        goto loadModule_0x1_fail_0;
    }

    /* Handle loader function: */
    apiData->initializer = (ModuleInitializer) dlsym(m->libHandle,
                                                     "sharemind_module_api_0x1_"
                                                     "module_init");
    if (unlikely(!apiData->initializer)) {
        SharemindModuleApi_setError(m->modapi,
                                    SHAREMIND_MODULE_API_API_ERROR,
                                    "Missing required symbol \"sharemind_"
                                    "module_api_0x1_module_init\"!");
        goto loadModule_0x1_fail_1;
    }

    /* Handle unloader function: */
    apiData->deinitializer = (ModuleDeinitializer) dlsym(m->libHandle,
                                                         "sharemind_module_api_"
                                                         "0x1_module_deinit");
    if (unlikely(!apiData->deinitializer)) {

        SharemindModuleApi_setError(m->modapi,
                                    SHAREMIND_MODULE_API_API_ERROR,
                                    "Missing required symbol \"sharemind_"
                                    "module_api_0x1_module_deinit\"!");
        goto loadModule_0x1_fail_1;
    }

    /* Handle system call definitions: */
    SharemindSyscallMap_init(&apiData->syscalls);
    scs = (SyscallDefinitions *) dlsym(m->libHandle,
                                       "sharemindModuleApi0x1"
                                       "SyscallDefinitions");
    if (likely(scs)) {
        size_t i = 0u;
        static const size_t syscallSignatureBufSize = sizeof((*scs)->signature);
        char syscallSignatureBuffer[syscallSignatureBufSize + 1];
        syscallSignatureBuffer[syscallSignatureBufSize] = '\0';
        while ((*scs)[i].signature[0]) {
            memcpy(syscallSignatureBuffer,
                   (*scs)[i].signature,
                   syscallSignatureBufSize);

            if (unlikely(!(*scs)[i].fptr)) {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_API_ERROR,
                            "A system call definition in module has a NULL "
                            "function pointer!");
                goto loadModule_0x1_fail_2;
            }

            const size_t oldSize = apiData->syscalls.size;
            SharemindSyscall * const sc = SharemindSyscallMap_get_or_insert(
                                                  &apiData->syscalls,
                                                  syscallSignatureBuffer);
            assert(oldSize <= apiData->syscalls.size
                   && apiData->syscalls.size - oldSize <= 1u);
            if (unlikely(!sc)) {
                SharemindModuleApi_setErrorOom(m->modapi);
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(oldSize == apiData->syscalls.size)) {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_DUPLICATE_SYSCALL,
                            "Duplicate system call definitions in module!");
                goto loadModule_0x1_fail_2;
            }
            assert(apiData->syscalls.size - oldSize == 1u);
            if (unlikely(!SharemindSyscall_init(sc,
                                                syscallSignatureBuffer,
                                                (void (*)(void)) (*scs)[i].fptr,
                                                NULL,
                                                m)))
            {
                SharemindModuleApi_setErrorOom(m->modapi);
                int r = SharemindSyscallMap_remove(&apiData->syscalls,
                                                   syscallSignatureBuffer);
                assert(r == 1); (void) r;
                goto loadModule_0x1_fail_2;
            }

            i++;
        }
        if (unlikely((*scs)[i].fptr)) {
            SharemindModuleApi_setError(
                        m->modapi,
                        SHAREMIND_MODULE_API_API_ERROR,
                        "The system call definition list in module did not end "
                        "properly!");
            goto loadModule_0x1_fail_2;
        }
    }

    /* Handle protection domain kind definitions: */
    SharemindPdkMap_init(&apiData->pdks);
    pdks = (PdkDefinitions *) dlsym(m->libHandle,
                                    "sharemindModuleApi0x1PdkDefinitions");
    if (pdks) {
        size_t i = 0u;
        static const size_t pdkNameBufferSize = sizeof((*pdks)->name);
        char pdkNameBuffer[pdkNameBufferSize + 1];
        pdkNameBuffer[pdkNameBufferSize] = '\0';
        while ((*pdks)[i].name[0]) {
            memcpy(pdkNameBuffer, (*pdks)[i].name, pdkNameBufferSize);

            if (unlikely(!(*pdks)[i].pd_startup_f
                         || !(*pdks)[i].pd_shutdown_f
                         || !(*pdks)[i].pdpi_startup_f
                         || !(*pdks)[i].pdpi_shutdown_f))
            {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_API_ERROR,
                            "An invalid item in protection domain kind "
                            "definitions list in module!");
                goto loadModule_0x1_fail_3;
            }

            const size_t oldSize = apiData->pdks.size;
            SharemindPdk * const pdk =
                    SharemindPdkMap_get_or_insert(&apiData->pdks,
                                                  pdkNameBuffer);
            assert(oldSize <= apiData->pdks.size
                   && apiData->pdks.size - oldSize <= 1u);
            if (unlikely(!pdk)) {
                SharemindModuleApi_setErrorOom(m->modapi);
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(oldSize == apiData->pdks.size)) {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN,
                            "Duplicate protection domain kind definitions in "
                            "module!");
                goto loadModule_0x1_fail_3;
            }
            assert(apiData->pdks.size - oldSize == 1u);
            if (unlikely(!SharemindPdk_init(
                             pdk,
                             i,
                             pdkNameBuffer,
                             (void (*)(void)) (*pdks)[i].pd_startup_f,
                             (void (*)(void)) NULL,
                             (void (*)(void)) (*pdks)[i].pd_shutdown_f,
                             (void (*)(void)) NULL,
                             (void (*)(void)) (*pdks)[i].pdpi_startup_f,
                             (void (*)(void)) NULL,
                             (void (*)(void)) (*pdks)[i].pdpi_shutdown_f,
                             (void (*)(void)) NULL,
                             m)))
            {
                /* Note that SharemindPdk_init calls the
                 * SharemindModuleApi_setError*() functions itself. */
                int r = SharemindPdkMap_remove(&apiData->pdks, pdkNameBuffer);
                assert(r == 1); (void) r;
                goto loadModule_0x1_fail_3;
            }

            i++;
        }
        if (unlikely((*pdks)[i].pd_startup_f
                     || (*pdks)[i].pd_shutdown_f
                     || (*pdks)[i].pdpi_startup_f
                     || (*pdks)[i].pdpi_shutdown_f))
        {
            SharemindModuleApi_setError(
                        m->modapi,
                        SHAREMIND_MODULE_API_API_ERROR,
                        "The protection domain kind definition list in module "
                        "did not end properly!");
            goto loadModule_0x1_fail_3;
        }
    }

    m->apiData = (void *) apiData;

    return true;

loadModule_0x1_fail_3:

    SharemindPdkMap_destroy_with(&apiData->pdks, &SharemindPdkMap_destroyer);

loadModule_0x1_fail_2:

    SharemindSyscallMap_destroy_with(&apiData->syscalls,
                                     &SharemindSyscallMap_destroyer);

loadModule_0x1_fail_1:

    free(apiData);

loadModule_0x1_fail_0:

    return false;
}

void SharemindModule_unload_0x1(SharemindModule * const m) {
    assert(m);
    assert(m->apiData);

    ApiData * const apiData = (ApiData *) m->apiData;

    SharemindPdkMap_destroy_with(&apiData->pdks,
                                 &SharemindPdkMap_destroyer);
    SharemindSyscallMap_destroy_with(&apiData->syscalls,
                                     &SharemindSyscallMap_destroyer);
    free(apiData);
}

bool SharemindModule_init_0x1(SharemindModule * const m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    ModuleContext context = {
        .moduleHandle = NULL, /* Just in case */
        .getModuleFacility = &SharemindModule_facilityWrapper,
        .internal = m,
        .conf = m->conf
    };
    switch (apiData->initializer(&context)) {
        case SHAREMIND_MODULE_API_0x1_OK:
            if (!context.moduleHandle) {
                apiData->deinitializer(&context);
                SharemindModule_setError(m,
                                         SHAREMIND_MODULE_API_API_ERROR,
                                         "Module handle was not initialized!");
                return false;
            }
            m->moduleHandle = context.moduleHandle;
            return true;
        case SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY:
            SharemindModule_setErrorOom(m);
            return false;
        case SHAREMIND_MODULE_API_0x1_SHAREMIND_ERROR:
            SharemindModule_setError(m,
                                     SHAREMIND_MODULE_API_SHAREMIND_ERROR,
                                     "Module notified a Sharemind fault!");
            return false;
        case SHAREMIND_MODULE_API_0x1_MODULE_ERROR:
            SharemindModule_setError(m,
                                     SHAREMIND_MODULE_API_MODULE_ERROR,
                                     "Module notified a module error!");
            return false;
        case SHAREMIND_MODULE_API_0x1_GENERAL_ERROR:
            SharemindModule_setError(m,
                                     SHAREMIND_MODULE_API_MODULE_ERROR,
                                     "Module notified a general error!");
            return false;
        default:
            SharemindModule_setError(m,
                                     SHAREMIND_MODULE_API_API_ERROR,
                                     "Module returned an unexpected error!");
            return false;
    }
}

void SharemindModule_deinit_0x1(SharemindModule * const m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    ModuleContext context = {
        .moduleHandle = m->moduleHandle,
        .getModuleFacility = &SharemindModule_facilityWrapper,
        .internal = m,
        .conf = m->conf
    };
    apiData->deinitializer(&context);
}

size_t SharemindModule_numSyscalls_0x1(const SharemindModule * m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return apiData->syscalls.size;
}

SharemindSyscall * SharemindModule_syscall_0x1(const SharemindModule * m,
                                               size_t index)
{
    ApiData * const apiData = (ApiData *) m->apiData;

    return SharemindSyscallMap_value_at(&apiData->syscalls, index);
}


SharemindSyscall * SharemindModule_findSyscall_0x1(const SharemindModule * m,
                                                   const char * signature)
{
    ApiData * const apiData = (ApiData *) m->apiData;

    return SharemindSyscallMap_get(&apiData->syscalls, signature);
}

size_t SharemindModule_numPdks_0x1(const SharemindModule * m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return apiData->pdks.size;
}

SharemindPdk * SharemindModule_pdk_0x1(const SharemindModule * m, size_t index)
{
    ApiData * const apiData = (ApiData *) m->apiData;

    return SharemindPdkMap_value_at(&apiData->pdks, index);
}


SharemindPdk * SharemindModule_findPdk_0x1(const SharemindModule * m,
                                           const char * name)
{
    ApiData * const apiData = (ApiData *) m->apiData;

    return SharemindPdkMap_get(&apiData->pdks, name);
}
