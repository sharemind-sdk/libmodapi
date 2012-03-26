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
#include "module.h"
#include "pdk.h"
#include "syscall.h"


SHAREMIND_STRINGMAP_DECLARE(SharemindPdkMap,SharemindPdk,)
SHAREMIND_STRINGMAP_DEFINE(SharemindPdkMap,SharemindPdk,malloc,free,strdup,)

SHAREMIND_MAP_DECLARE(SharemindSyscallMap,char *,const char * const,SharemindSyscall,)
SHAREMIND_STRINGMAP_DEFINE(SharemindSyscallMap,SharemindSyscall,malloc,free,strdup,)

static const SharemindFacility * SHAREMIND_Module_get_facility_wrapper(SharemindModuleApi0x1ModuleContext * w, const char * name) {
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SharemindModule_get_facility((SharemindModule *) w->internal, name);
}

static void SharemindSyscallMap_destroyer(const char * const * key, SharemindSyscall * value) {
    (void) key;
    SharemindSyscall_destroy(value);
}

static void SharemindPdkMap_destroyer(const char * const * key, SharemindPdk * value) {
    (void) key;
    SharemindPdk_destroy(value);
}

/* API 0x1 data */

typedef struct {
    SharemindModuleApi0x1ModuleInitializer initializer;
    SharemindModuleApi0x1ModuleDeinitializer deinitializer;
    SharemindSyscallMap syscalls;
    SharemindPdkMap pdks;
} SharemindModuleApiData0x1;


SharemindModuleApiError SHAREMIND_Module_load_0x1(SharemindModule * m) {
    assert(m);

    SharemindModuleApiError status;
    SharemindModuleApi0x1SyscallDefinitions * scs;
    SharemindModuleApi0x1PdkDefinitions * pdks;

    SharemindModuleApiData0x1 * apiData;

    apiData = (SharemindModuleApiData0x1 *) malloc(sizeof(SharemindModuleApiData0x1));
    if (unlikely(!apiData)) {
        status = SHAREMIND_MODULE_API_OUT_OF_MEMORY;
        goto loadModule_0x1_fail_0;
    }

    /* Handle loader function: */
    apiData->initializer = (SharemindModuleApi0x1ModuleInitializer) dlsym(m->libHandle, "sharemind_module_api_0x1_module_init");
    if (unlikely(!apiData->initializer)) {
        status = SHAREMIND_MODULE_API_API_ERROR;
        goto loadModule_0x1_fail_1;
    }

    /* Handle unloader function: */
    apiData->deinitializer = (SharemindModuleApi0x1ModuleDeinitializer) dlsym(m->libHandle, "sharemind_module_api_0x1_module_deinit");
    if (unlikely(!apiData->deinitializer)) {
        status = SHAREMIND_MODULE_API_API_ERROR;
        goto loadModule_0x1_fail_1;
    }

    /* Handle system call definitions: */
    SharemindSyscallMap_init(&apiData->syscalls);
    scs = (SharemindModuleApi0x1SyscallDefinitions *) dlsym(m->libHandle, "sharemindModuleApi0x1SyscallDefinitions");
    if (likely(scs)) {
        size_t i = 0u;
        while ((*scs)[i].signature) {
            SharemindSyscall * sc;
            size_t oldSize;

            if (unlikely(!(*scs)[i].fptr)) {
                status = SHAREMIND_MODULE_API_API_ERROR;
                goto loadModule_0x1_fail_2;
            }

            oldSize = apiData->syscalls.size;
            sc = SharemindSyscallMap_get_or_insert(&apiData->syscalls, (*scs)[i].signature);
            assert(oldSize <= apiData->syscalls.size);
            if (unlikely(!sc)) {
                status = SHAREMIND_MODULE_API_OUT_OF_MEMORY;
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(oldSize == apiData->syscalls.size)) {
                status = SHAREMIND_MODULE_API_DUPLICATE_SYSCALL;
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(!SharemindSyscall_init(sc, (*scs)[i].signature, (*scs)[i].fptr, NULL, m))) {
                status = SHAREMIND_MODULE_API_OUT_OF_MEMORY;
                int r = SharemindSyscallMap_remove(&apiData->syscalls, (*scs)[i].signature);
                assert(r == 1); (void) r;
                goto loadModule_0x1_fail_2;
            }

            i++;
        }
        if (unlikely((*scs)[i].fptr)) {
            status = SHAREMIND_MODULE_API_API_ERROR;
            goto loadModule_0x1_fail_2;
        }
    }

    /* Handle protection domain kind definitions: */
    SharemindPdkMap_init(&apiData->pdks);
    pdks = (SharemindModuleApi0x1PdkDefinitions *) dlsym(m->libHandle, "sharemindModuleApi0x1PdkDefinitions");
    if (pdks) {
        size_t i = 0u;
        while ((*pdks)[i].name) {
            SharemindPdk * pdk;
            size_t oldSize;

            if (unlikely(!(*pdks)[i].pd_startup_f
                         || !(*pdks)[i].pd_shutdown_f
                         || !(*pdks)[i].pdpi_startup_f
                         || !(*pdks)[i].pdpi_shutdown_f))
            {
                status = SHAREMIND_MODULE_API_API_ERROR;
                goto loadModule_0x1_fail_3;
            }

            oldSize = apiData->pdks.size;
            pdk = SharemindPdkMap_get_or_insert(&apiData->pdks, (*pdks)[i].name);
            assert(oldSize <= apiData->syscalls.size);
            if (unlikely(!pdk)) {
                status = SHAREMIND_MODULE_API_OUT_OF_MEMORY;
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(oldSize == apiData->pdks.size)) {
                status = SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN;
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(!SharemindPdk_init(pdk,
                                        i,
                                        (*pdks)[i].name,
                                        (*pdks)[i].pd_startup_f, NULL,
                                        (*pdks)[i].pd_shutdown_f, NULL,
                                        (*pdks)[i].pdpi_startup_f, NULL,
                                        (*pdks)[i].pdpi_shutdown_f, NULL,
                                        m)))
            {
                status = SHAREMIND_MODULE_API_OUT_OF_MEMORY;
                int r = SharemindPdkMap_remove(&apiData->pdks, (*pdks)[i].name);
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
            status = SHAREMIND_MODULE_API_API_ERROR;
            goto loadModule_0x1_fail_3;
        }
    }

    /** \todo Handle protection domain definitions: */
    m->apiData = (void *) apiData;

    return SHAREMIND_MODULE_API_OK;

loadModule_0x1_fail_3:

    SharemindPdkMap_destroy_with(&apiData->pdks, &SharemindPdkMap_destroyer);

loadModule_0x1_fail_2:

    SharemindSyscallMap_destroy_with(&apiData->syscalls, &SharemindSyscallMap_destroyer);

loadModule_0x1_fail_1:

    free(apiData);

loadModule_0x1_fail_0:

    return status;
}

void SHAREMIND_Module_unload_0x1(SharemindModule * const m) {
    assert(m);
    assert(m->apiData);

    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    SharemindPdkMap_destroy_with(&apiData->pdks, &SharemindPdkMap_destroyer);
    SharemindSyscallMap_destroy_with(&apiData->syscalls, &SharemindSyscallMap_destroyer);
    free(apiData);
}

SharemindModuleApiError SHAREMIND_Module_init_0x1(SharemindModule * const m) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    SharemindModuleApi0x1ModuleContext context = {
        .moduleHandle = NULL, /* Just in case */
        .getModuleFacility = &SHAREMIND_Module_get_facility_wrapper,
        .internal = m
    };
    SharemindModuleApi0x1InitializerCode r = apiData->initializer(&context);
    switch (r) {
        case SHAREMIND_MODULE_API_0x1_IC_OK:
            if (!context.moduleHandle) {
                apiData->deinitializer(&context);
                return SHAREMIND_MODULE_API_API_ERROR;
            }
            m->moduleHandle = context.moduleHandle;
            return SHAREMIND_MODULE_API_OK;
        case SHAREMIND_MODULE_API_0x1_IC_OUT_OF_MEMORY:
            return SHAREMIND_MODULE_API_OUT_OF_MEMORY;
        case SHAREMIND_MODULE_API_0x1_IC_ERROR:
            return SHAREMIND_MODULE_API_MODULE_ERROR;
        default:
            return SHAREMIND_MODULE_API_API_ERROR; /* False positive with -Wunreachable-code */
    }
}

void SHAREMIND_Module_deinit_0x1(SharemindModule * const m) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    SharemindModuleApi0x1ModuleContext context = {
        .moduleHandle = m->moduleHandle,
        .getModuleFacility = &SHAREMIND_Module_get_facility_wrapper,
        .internal = m
    };
    apiData->deinitializer(&context);
}

size_t SHAREMIND_Module_get_num_syscalls_0x1(const SharemindModule * m) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    return apiData->syscalls.size;
}

SharemindSyscall * SHAREMIND_Module_get_syscall_0x1(const SharemindModule * m, size_t index) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    return SharemindSyscallMap_value_at(&apiData->syscalls, index);
}


SharemindSyscall * SHAREMIND_Module_find_syscall_0x1(const SharemindModule * m, const char * signature) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    return SharemindSyscallMap_get(&apiData->syscalls, signature);
}

size_t SHAREMIND_Module_get_num_pdks_0x1(const SharemindModule * m) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    return apiData->pdks.size;
}

SharemindPdk * SHAREMIND_Module_get_pdk_0x1(const SharemindModule * m, size_t index) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    return SharemindPdkMap_value_at(&apiData->pdks, index);
}


SharemindPdk * SHAREMIND_Module_find_pdk_0x1(const SharemindModule * m, const char * name) {
    SharemindModuleApiData0x1 * const apiData = (SharemindModuleApiData0x1 *) m->apiData;

    return SharemindPdkMap_get(&apiData->pdks, name);
}
