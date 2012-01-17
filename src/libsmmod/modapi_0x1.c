/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "modapi_0x1.h"
#include "modapi_0x1_internal.h"

#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "../stringmap.h"
#include "libsmmod.h"
#include "pd.h"
#include "pdk.h"
#include "syscall.h"


SM_STRINGMAP_DECLARE(SMVM_PDKMap,SMVM_PDK,)
SM_STRINGMAP_DEFINE(SMVM_PDKMap,SMVM_PDK,malloc,free,strdup,)

SM_MAP_DECLARE(SMVM_SyscallMap,char *,const char * const,SMVM_Syscall,)
SM_STRINGMAP_DEFINE(SMVM_SyscallMap,SMVM_Syscall,malloc,free,strdup,)

static void * SMVM_Module_get_facility_wrapper(SMVM_MODAPI_0x1_Module_Context * w, const char * name) {
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SMVM_Module_get_facility((SMVM_Module *) w->internal, name);
}

static void SMVM_SyscallMap_destroyer(const char * const * key, SMVM_Syscall * value) {
    (void) key;
    SMVM_Syscall_destroy(value);
}

static void SMVM_PDKMap_destroyer(const char * const * key, SMVM_PDK * value) {
    (void) key;
    SMVM_PDK_destroy(value);
}

/* API 0x1 data */

typedef struct {
    SMVM_MODAPI_0x1_Module_Initializer initializer;
    SMVM_MODAPI_0x1_Module_Deinitializer deinitializer;
    SMVM_SyscallMap syscalls;
    SMVM_PDKMap pdks;
} ApiData;


SMVM_MODAPI_Error loadModule_0x1(SMVM_Module * m) {
    assert(m);

    SMVM_MODAPI_Error status;
    SMVM_MODAPI_0x1_Syscall_Definitions * scs;
    SMVM_MODAPI_0x1_PDK_Definitions * pdks;

    ApiData * apiData;

    apiData = (ApiData *) malloc(sizeof(ApiData));
    if (unlikely(!apiData)) {
        status = SMVM_MODAPI_OUT_OF_MEMORY;
        goto loadModule_0x1_fail_0;
    }

    /* Handle loader function: */
    apiData->initializer = (SMVM_MODAPI_0x1_Module_Initializer) dlsym(m->handle, "SMVM_MOD_0x1_init");
    if (unlikely(!apiData->initializer)) {
        status = SMVM_MODAPI_API_ERROR;
        goto loadModule_0x1_fail_1;
    }

    /* Handle unloader function: */
    apiData->deinitializer = (SMVM_MODAPI_0x1_Module_Deinitializer) dlsym(m->handle, "SMVM_MOD_0x1_deinit");
    if (unlikely(!apiData->deinitializer)) {
        status = SMVM_MODAPI_API_ERROR;
        goto loadModule_0x1_fail_1;
    }

    /* Handle system call definitions: */
    SMVM_SyscallMap_init(&apiData->syscalls);
    scs = (SMVM_MODAPI_0x1_Syscall_Definitions *) dlsym(m->handle, "SMVM_MOD_0x1_syscalls");
    if (likely(scs)) {
        size_t i = 0u;
        while ((*scs)[i].name) {
            SMVM_Syscall * sc;
            size_t oldSize;

            if (unlikely(!(*scs)[i].syscall_f)) {
                status = SMVM_MODAPI_API_ERROR;
                goto loadModule_0x1_fail_2;
            }

            oldSize = apiData->syscalls.size;
            sc = SMVM_SyscallMap_get_or_insert(&apiData->syscalls, (*scs)[i].name);
            assert(oldSize <= apiData->syscalls.size);
            if (unlikely(!sc)) {
                status = SMVM_MODAPI_OUT_OF_MEMORY;
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(oldSize == apiData->syscalls.size)) {
                status = SMVM_MODAPI_DUPLICATE_SYSCALL;
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(!SMVM_Syscall_init(sc, (*scs)[i].name, (*scs)[i].syscall_f, NULL, m))) {
                status = SMVM_MODAPI_OUT_OF_MEMORY;
                int r = SMVM_SyscallMap_remove(&apiData->syscalls, (*scs)[i].name);
                assert(r == 1); (void) r;
                goto loadModule_0x1_fail_2;
            }

            i++;
        }
        if (unlikely((*scs)[i].syscall_f)) {
            status = SMVM_MODAPI_API_ERROR;
            goto loadModule_0x1_fail_2;
        }
    }

    /* Handle protection domain kind definitions: */
    SMVM_PDKMap_init(&apiData->pdks);
    pdks = (SMVM_MODAPI_0x1_PDK_Definitions *) dlsym(m->handle, "SMVM_MOD_0x1_pdks");
    if (pdks) {
        size_t i = 0u;
        while ((*pdks)[i].name) {
            SMVM_PDK * pdk;
            size_t oldSize;

            if (unlikely(!(*pdks)[i].pd_startup_f
                         || !(*pdks)[i].pd_shutdown_f
                         || !(*pdks)[i].pdpi_startup_f
                         || !(*pdks)[i].pdpi_shutdown_f))
            {
                status = SMVM_MODAPI_API_ERROR;
                goto loadModule_0x1_fail_3;
            }

            oldSize = apiData->pdks.size;
            pdk = SMVM_PDKMap_get_or_insert(&apiData->pdks, (*pdks)[i].name);
            assert(oldSize <= apiData->syscalls.size);
            if (unlikely(!pdk)) {
                status = SMVM_MODAPI_OUT_OF_MEMORY;
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(oldSize == apiData->pdks.size)) {
                status = SMVM_MODAPI_DUPLICATE_PROTECTION_DOMAIN;
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(!SMVM_PDK_init(pdk,
                                        i,
                                        (*pdks)[i].name,
                                        (*pdks)[i].pd_startup_f, NULL,
                                        (*pdks)[i].pd_shutdown_f, NULL,
                                        (*pdks)[i].pdpi_startup_f, NULL,
                                        (*pdks)[i].pdpi_shutdown_f, NULL,
                                        m)))
            {
                status = SMVM_MODAPI_OUT_OF_MEMORY;
                int r = SMVM_PDKMap_remove(&apiData->pdks, (*scs)[i].name);
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
            status = SMVM_MODAPI_API_ERROR;
            goto loadModule_0x1_fail_3;
        }
    }

    /** \todo Handle protection domain definitions: */
    m->apiData = (void *) apiData;

    return SMVM_MODAPI_OK;

loadModule_0x1_fail_3:

    SMVM_PDKMap_destroy_with(&apiData->pdks, &SMVM_PDKMap_destroyer);

loadModule_0x1_fail_2:

    SMVM_SyscallMap_destroy_with(&apiData->syscalls, &SMVM_SyscallMap_destroyer);

loadModule_0x1_fail_1:

    free(apiData);

loadModule_0x1_fail_0:

    return status;
}

void unloadModule_0x1(SMVM_Module * const m) {
    assert(m);
    assert(m->apiData);

    ApiData * const apiData = (ApiData *) m->apiData;

    SMVM_PDKMap_destroy_with(&apiData->pdks, &SMVM_PDKMap_destroyer);
    SMVM_SyscallMap_destroy_with(&apiData->syscalls, &SMVM_SyscallMap_destroyer);
    free(apiData);
}

SMVM_MODAPI_Error initModule_0x1(SMVM_Module * const m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    SMVM_MODAPI_0x1_Module_Context context = {
        .getModuleFacility = &SMVM_Module_get_facility_wrapper,
        .internal = m
    };
    SMVM_MODAPI_0x1_Initializer_Code r = apiData->initializer(&context);
    switch (r) {
        case SMVM_MODAPI_0x1_IC_OK:
            if (!context.moduleHandle) {
                apiData->deinitializer(&context);
                return SMVM_MODAPI_API_ERROR;
            }
            m->moduleHandle = context.moduleHandle;
            return SMVM_MODAPI_OK;
        case SMVM_MODAPI_0x1_IC_OUT_OF_MEMORY:
            return SMVM_MODAPI_OUT_OF_MEMORY;
        case SMVM_MODAPI_0x1_IC_ERROR:
            return SMVM_MODAPI_MODULE_ERROR;
        default:
            return SMVM_MODAPI_API_ERROR;
    }
}

void deinitModule_0x1(SMVM_Module * const m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    SMVM_MODAPI_0x1_Module_Context context = {
        .moduleHandle = m->moduleHandle,
        .getModuleFacility = &SMVM_Module_get_facility_wrapper,
        .internal = m
    };
    apiData->deinitializer(&context);
}

size_t getNumSyscalls_0x1(const SMVM_Module * m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return apiData->syscalls.size;
}

SMVM_Syscall * getSyscall_0x1(const SMVM_Module * m, size_t index) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return SMVM_SyscallMap_value_at(&apiData->syscalls, index);
}


SMVM_Syscall * findSyscall_0x1(const SMVM_Module * m, const char * signature) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return SMVM_SyscallMap_get(&apiData->syscalls, signature);
}

size_t getNumPdks_0x1(const SMVM_Module * m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return apiData->pdks.size;
}

SMVM_PDK * getPdk_0x1(const SMVM_Module * m, size_t index) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return SMVM_PDKMap_value_at(&apiData->pdks, index);
}


SMVM_PDK * findPdk_0x1(const SMVM_Module * m, const char * name) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return SMVM_PDKMap_get(&apiData->pdks, name);
}
