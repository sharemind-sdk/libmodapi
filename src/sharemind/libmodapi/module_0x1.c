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


SHAREMIND_STRINGMAP_DECLARE(SHAREMIND_PDKMap,SHAREMIND_PDK,)
SHAREMIND_STRINGMAP_DEFINE(SHAREMIND_PDKMap,SHAREMIND_PDK,malloc,free,strdup,)

SHAREMIND_MAP_DECLARE(SHAREMIND_SyscallMap,char *,const char * const,SHAREMIND_Syscall,)
SHAREMIND_STRINGMAP_DEFINE(SHAREMIND_SyscallMap,SHAREMIND_Syscall,malloc,free,strdup,)

static const SHAREMIND_Facility * SHAREMIND_Module_get_facility_wrapper(SHAREMIND_MODAPI_0x1_Module_Context * w, const char * name) {
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SHAREMIND_Module_get_facility((SHAREMIND_Module *) w->internal, name);
}

static void SHAREMIND_SyscallMap_destroyer(const char * const * key, SHAREMIND_Syscall * value) {
    (void) key;
    SHAREMIND_Syscall_destroy(value);
}

static void SHAREMIND_PDKMap_destroyer(const char * const * key, SHAREMIND_PDK * value) {
    (void) key;
    SHAREMIND_PDK_destroy(value);
}

/* API 0x1 data */

typedef struct {
    SHAREMIND_MODAPI_0x1_Module_Initializer initializer;
    SHAREMIND_MODAPI_0x1_Module_Deinitializer deinitializer;
    SHAREMIND_SyscallMap syscalls;
    SHAREMIND_PDKMap pdks;
} SHAREMIND_Module_ApiData_0x1;


SHAREMIND_MODAPI_Error SHAREMIND_Module_load_0x1(SHAREMIND_Module * m) {
    assert(m);

    SHAREMIND_MODAPI_Error status;
    SHAREMIND_MODAPI_0x1_Syscall_Definitions * scs;
    SHAREMIND_MODAPI_0x1_PDK_Definitions * pdks;

    SHAREMIND_Module_ApiData_0x1 * apiData;

    apiData = (SHAREMIND_Module_ApiData_0x1 *) malloc(sizeof(SHAREMIND_Module_ApiData_0x1));
    if (unlikely(!apiData)) {
        status = SHAREMIND_MODAPI_OUT_OF_MEMORY;
        goto loadModule_0x1_fail_0;
    }

    /* Handle loader function: */
    apiData->initializer = (SHAREMIND_MODAPI_0x1_Module_Initializer) dlsym(m->handle, "SHAREMIND_MOD_0x1_init");
    if (unlikely(!apiData->initializer)) {
        status = SHAREMIND_MODAPI_API_ERROR;
        goto loadModule_0x1_fail_1;
    }

    /* Handle unloader function: */
    apiData->deinitializer = (SHAREMIND_MODAPI_0x1_Module_Deinitializer) dlsym(m->handle, "SHAREMIND_MOD_0x1_deinit");
    if (unlikely(!apiData->deinitializer)) {
        status = SHAREMIND_MODAPI_API_ERROR;
        goto loadModule_0x1_fail_1;
    }

    /* Handle system call definitions: */
    SHAREMIND_SyscallMap_init(&apiData->syscalls);
    scs = (SHAREMIND_MODAPI_0x1_Syscall_Definitions *) dlsym(m->handle, "SHAREMIND_MOD_0x1_syscalls");
    if (likely(scs)) {
        size_t i = 0u;
        while ((*scs)[i].name) {
            SHAREMIND_Syscall * sc;
            size_t oldSize;

            if (unlikely(!(*scs)[i].syscall_f)) {
                status = SHAREMIND_MODAPI_API_ERROR;
                goto loadModule_0x1_fail_2;
            }

            oldSize = apiData->syscalls.size;
            sc = SHAREMIND_SyscallMap_get_or_insert(&apiData->syscalls, (*scs)[i].name);
            assert(oldSize <= apiData->syscalls.size);
            if (unlikely(!sc)) {
                status = SHAREMIND_MODAPI_OUT_OF_MEMORY;
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(oldSize == apiData->syscalls.size)) {
                status = SHAREMIND_MODAPI_DUPLICATE_SYSCALL;
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(!SHAREMIND_Syscall_init(sc, (*scs)[i].name, (*scs)[i].syscall_f, NULL, m))) {
                status = SHAREMIND_MODAPI_OUT_OF_MEMORY;
                int r = SHAREMIND_SyscallMap_remove(&apiData->syscalls, (*scs)[i].name);
                assert(r == 1); (void) r;
                goto loadModule_0x1_fail_2;
            }

            i++;
        }
        if (unlikely((*scs)[i].syscall_f)) {
            status = SHAREMIND_MODAPI_API_ERROR;
            goto loadModule_0x1_fail_2;
        }
    }

    /* Handle protection domain kind definitions: */
    SHAREMIND_PDKMap_init(&apiData->pdks);
    pdks = (SHAREMIND_MODAPI_0x1_PDK_Definitions *) dlsym(m->handle, "SHAREMIND_MOD_0x1_pdks");
    if (pdks) {
        size_t i = 0u;
        while ((*pdks)[i].name) {
            SHAREMIND_PDK * pdk;
            size_t oldSize;

            if (unlikely(!(*pdks)[i].pd_startup_f
                         || !(*pdks)[i].pd_shutdown_f
                         || !(*pdks)[i].pdpi_startup_f
                         || !(*pdks)[i].pdpi_shutdown_f))
            {
                status = SHAREMIND_MODAPI_API_ERROR;
                goto loadModule_0x1_fail_3;
            }

            oldSize = apiData->pdks.size;
            pdk = SHAREMIND_PDKMap_get_or_insert(&apiData->pdks, (*pdks)[i].name);
            assert(oldSize <= apiData->syscalls.size);
            if (unlikely(!pdk)) {
                status = SHAREMIND_MODAPI_OUT_OF_MEMORY;
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(oldSize == apiData->pdks.size)) {
                status = SHAREMIND_MODAPI_DUPLICATE_PROTECTION_DOMAIN;
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(!SHAREMIND_PDK_init(pdk,
                                        i,
                                        (*pdks)[i].name,
                                        (*pdks)[i].pd_startup_f, NULL,
                                        (*pdks)[i].pd_shutdown_f, NULL,
                                        (*pdks)[i].pdpi_startup_f, NULL,
                                        (*pdks)[i].pdpi_shutdown_f, NULL,
                                        m)))
            {
                status = SHAREMIND_MODAPI_OUT_OF_MEMORY;
                int r = SHAREMIND_PDKMap_remove(&apiData->pdks, (*scs)[i].name);
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
            status = SHAREMIND_MODAPI_API_ERROR;
            goto loadModule_0x1_fail_3;
        }
    }

    /** \todo Handle protection domain definitions: */
    m->apiData = (void *) apiData;

    return SHAREMIND_MODAPI_OK;

loadModule_0x1_fail_3:

    SHAREMIND_PDKMap_destroy_with(&apiData->pdks, &SHAREMIND_PDKMap_destroyer);

loadModule_0x1_fail_2:

    SHAREMIND_SyscallMap_destroy_with(&apiData->syscalls, &SHAREMIND_SyscallMap_destroyer);

loadModule_0x1_fail_1:

    free(apiData);

loadModule_0x1_fail_0:

    return status;
}

void SHAREMIND_Module_unload_0x1(SHAREMIND_Module * const m) {
    assert(m);
    assert(m->apiData);

    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    SHAREMIND_PDKMap_destroy_with(&apiData->pdks, &SHAREMIND_PDKMap_destroyer);
    SHAREMIND_SyscallMap_destroy_with(&apiData->syscalls, &SHAREMIND_SyscallMap_destroyer);
    free(apiData);
}

SHAREMIND_MODAPI_Error SHAREMIND_Module_init_0x1(SHAREMIND_Module * const m) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    SHAREMIND_MODAPI_0x1_Module_Context context = {
        .moduleHandle = NULL, /* Just in case */
        .getModuleFacility = &SHAREMIND_Module_get_facility_wrapper,
        .internal = m
    };
    SHAREMIND_MODAPI_0x1_Initializer_Code r = apiData->initializer(&context);
    switch (r) {
        case SHAREMIND_MODAPI_0x1_IC_OK:
            if (!context.moduleHandle) {
                apiData->deinitializer(&context);
                return SHAREMIND_MODAPI_API_ERROR;
            }
            m->moduleHandle = context.moduleHandle;
            return SHAREMIND_MODAPI_OK;
        case SHAREMIND_MODAPI_0x1_IC_OUT_OF_MEMORY:
            return SHAREMIND_MODAPI_OUT_OF_MEMORY;
        case SHAREMIND_MODAPI_0x1_IC_ERROR:
            return SHAREMIND_MODAPI_MODULE_ERROR;
        default:
            return SHAREMIND_MODAPI_API_ERROR;
    }
}

void SHAREMIND_Module_deinit_0x1(SHAREMIND_Module * const m) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    SHAREMIND_MODAPI_0x1_Module_Context context = {
        .moduleHandle = m->moduleHandle,
        .getModuleFacility = &SHAREMIND_Module_get_facility_wrapper,
        .internal = m
    };
    apiData->deinitializer(&context);
}

size_t SHAREMIND_Module_get_num_syscalls_0x1(const SHAREMIND_Module * m) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    return apiData->syscalls.size;
}

SHAREMIND_Syscall * SHAREMIND_Module_get_syscall_0x1(const SHAREMIND_Module * m, size_t index) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    return SHAREMIND_SyscallMap_value_at(&apiData->syscalls, index);
}


SHAREMIND_Syscall * SHAREMIND_Module_find_syscall_0x1(const SHAREMIND_Module * m, const char * signature) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    return SHAREMIND_SyscallMap_get(&apiData->syscalls, signature);
}

size_t SHAREMIND_Module_get_num_pdks_0x1(const SHAREMIND_Module * m) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    return apiData->pdks.size;
}

SHAREMIND_PDK * SHAREMIND_Module_get_pdk_0x1(const SHAREMIND_Module * m, size_t index) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    return SHAREMIND_PDKMap_value_at(&apiData->pdks, index);
}


SHAREMIND_PDK * SHAREMIND_Module_find_pdk_0x1(const SHAREMIND_Module * m, const char * name) {
    SHAREMIND_Module_ApiData_0x1 * const apiData = (SHAREMIND_Module_ApiData_0x1 *) m->apiData;

    return SHAREMIND_PDKMap_get(&apiData->pdks, name);
}
