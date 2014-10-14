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


SHAREMIND_STRINGMAP_DECLARE_BODY(SharemindPdkMap, SharemindPdk)
SHAREMIND_STRINGMAP_DECLARE_init(SharemindPdkMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_init(SharemindPdkMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_destroy(SharemindPdkMap,static inline,,)
SHAREMIND_STRINGMAP_DEFINE_destroy(SharemindPdkMap,
                                   static inline,
                                   SharemindPdk,,
                                   free,
                                   SharemindPdk_destroy(&v->value);)
SHAREMIND_STRINGMAP_DECLARE_get(SharemindPdkMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_get(SharemindPdkMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_at(SharemindPdkMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_at(SharemindPdkMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_insertHint(SharemindPdkMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_insertHint(SharemindPdkMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_emplaceAtHint(SharemindPdkMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_emplaceAtHint(SharemindPdkMap, static inline)
SHAREMIND_STRINGMAP_DECLARE_insertAtHint(SharemindPdkMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_insertAtHint(SharemindPdkMap,
                                        static inline,
                                        strdup,
                                        malloc,
                                        free)
SHAREMIND_STRINGMAP_DECLARE_remove(SharemindPdkMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_remove(SharemindPdkMap,
                                  static inline,
                                  SharemindPdk,
                                  free,
                                  SharemindPdk_destroy(&v->value);)
SHAREMIND_STRINGMAP_DECLARE_foreach_detail(
        static inline size_t SharemindPdkMap_numPds,
        SharemindPdkMap,,)
SHAREMIND_STRINGMAP_DEFINE_foreach_detail(
        static inline size_t SharemindPdkMap_numPds,
        SharemindPdkMap,,
        size_t result = 0u;,
        result,
        assert(v->key);
        const size_t r = SharemindPdk_numPds(&v->value);
        if (SIZE_MAX - r <= result)
            return SIZE_MAX;
        result += r;)
SHAREMIND_STRINGMAP_DECLARE_foreach_detail(
        static inline SharemindPd * SharemindPdkMap_pd,
        SharemindPdkMap,
        SHAREMIND_COMMA size_t index,)
SHAREMIND_STRINGMAP_DEFINE_foreach_detail(
        static inline SharemindPd * SharemindPdkMap_pd,
        SharemindPdkMap,
        SHAREMIND_COMMA size_t index,,
        NULL,
        assert(v->key);
        const size_t n = SharemindPdk_numPds(&v->value);
        if (index < n)
            return SharemindPdk_pd(&v->value, index);
        index -= n;)
SHAREMIND_STRINGMAP_DECLARE_foreach_detail(
        static inline SharemindPd * SharemindPdkMap_findPd,
        SharemindPdkMap,
        SHAREMIND_COMMA const char * signature,)
SHAREMIND_STRINGMAP_DEFINE_foreach_detail(
        static inline SharemindPd * SharemindPdkMap_findPd,
        SharemindPdkMap,
        SHAREMIND_COMMA const char * signature,,
        NULL,
        assert(v->key);
        SharemindPd * const result =
                SharemindPdk_findPd(&v->value, signature);
        if (result)
            return result;)

SHAREMIND_STRINGMAP_DECLARE_BODY(SharemindSyscallMap, SharemindSyscall)
SHAREMIND_STRINGMAP_DECLARE_init(SharemindSyscallMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_init(SharemindSyscallMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_destroy(SharemindSyscallMap,static inline,,)
SHAREMIND_STRINGMAP_DEFINE_destroy(SharemindSyscallMap,
                                   static inline,
                                   SharemindSyscall,,
                                   free,
                                   SharemindSyscall_destroy(&v->value);)
SHAREMIND_STRINGMAP_DECLARE_get(SharemindSyscallMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_get(SharemindSyscallMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_at(SharemindSyscallMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_at(SharemindSyscallMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_insertHint(SharemindSyscallMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_insertHint(SharemindSyscallMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_emplaceAtHint(SharemindSyscallMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_emplaceAtHint(SharemindSyscallMap, static inline)
SHAREMIND_STRINGMAP_DECLARE_insertAtHint(SharemindSyscallMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_insertAtHint(SharemindSyscallMap,
                                        static inline,
                                        strdup,
                                        malloc,
                                        free)
SHAREMIND_STRINGMAP_DECLARE_remove(SharemindSyscallMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_remove(SharemindSyscallMap,
                                  static inline,
                                  SharemindSyscall,
                                  free,)

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
        char syscallSignatureBuffer[
                SHAREMIND_MODULE_API_0x1_SYSCALL_SIGNATURE_BUFFER_SIZE + 1u];
        syscallSignatureBuffer[
                SHAREMIND_MODULE_API_0x1_SYSCALL_SIGNATURE_BUFFER_SIZE] = '\0';
        while ((*scs)[i].signature[0]) {
            if (unlikely(!(*scs)[i].fptr)) {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_API_ERROR,
                            "A system call definition in module has a NULL "
                            "function pointer!");
                goto loadModule_0x1_fail_2;
            }
            memcpy(syscallSignatureBuffer,
                   (*scs)[i].signature,
                   SHAREMIND_MODULE_API_0x1_SYSCALL_SIGNATURE_BUFFER_SIZE);
            if (unlikely(SharemindModuleApi_findSyscall(
                             m->modapi,
                             syscallSignatureBuffer)))
            {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_DUPLICATE_SYSCALL,
                            "System call already provided by another module!");
                goto loadModule_0x1_fail_2;
            }
            void * const hint =
                    SharemindSyscallMap_insertHint(&apiData->syscalls,
                                                   syscallSignatureBuffer);
            if (unlikely(!hint)) {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_API_ERROR,
                            "Duplicate system call definitions in module!");
                goto loadModule_0x1_fail_2;
            }
            SharemindSyscallMap_value * const v =
                    SharemindSyscallMap_insertAtHint(&apiData->syscalls,
                                                     syscallSignatureBuffer,
                                                     hint);
            if (unlikely(!v)) {
                SharemindModuleApi_setErrorOom(m->modapi);
                goto loadModule_0x1_fail_2;
            }
            if (unlikely(!SharemindSyscall_init(&v->value,
                                                syscallSignatureBuffer,
                                                (void (*)(void)) (*scs)[i].fptr,
                                                m)))
            {
                SharemindModuleApi_setErrorOom(m->modapi);
                #ifndef NDEBUG
                const bool r =
                #endif
                        SharemindSyscallMap_remove(&apiData->syscalls,
                                                   syscallSignatureBuffer);
                assert(r);
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
        char pdkNameBuffer[SHAREMIND_MODULE_API_0x1_PDK_NAME_BUFFER_SIZE + 1u];
        pdkNameBuffer[SHAREMIND_MODULE_API_0x1_PDK_NAME_BUFFER_SIZE] = '\0';
        while ((*pdks)[i].name[0]) {
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
            memcpy(pdkNameBuffer,
                   (*pdks)[i].name,
                   SHAREMIND_MODULE_API_0x1_PDK_NAME_BUFFER_SIZE);
            if (unlikely(SharemindModuleApi_findPdk(
                             m->modapi,
                             pdkNameBuffer)))
            {
                SharemindModuleApi_setError(
                        m->modapi,
                        SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN_KIND,
                        "Protection domain kind already provided by another "
                        "module!");
                goto loadModule_0x1_fail_3;
            }
            void * const hint =
                    SharemindPdkMap_insertHint(&apiData->pdks, pdkNameBuffer);
            if (unlikely(!hint)) {
                SharemindModuleApi_setError(
                            m->modapi,
                            SHAREMIND_MODULE_API_API_ERROR,
                            "Duplicate protection domain kind definitions in "
                            "module!");
                goto loadModule_0x1_fail_3;
            }
            SharemindPdkMap_value * const v =
                    SharemindPdkMap_insertAtHint(&apiData->pdks,
                                                 pdkNameBuffer,
                                                 hint);
            if (unlikely(!v)) {
                SharemindModuleApi_setErrorOom(m->modapi);
                goto loadModule_0x1_fail_3;
            }
            if (unlikely(!SharemindPdk_init(
                             &v->value,
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
                #ifndef NDEBUG
                const bool r =
                #endif
                        SharemindPdkMap_remove(&apiData->pdks, pdkNameBuffer);
                assert(r);
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

    SharemindPdkMap_destroy(&apiData->pdks);

loadModule_0x1_fail_2:

    SharemindSyscallMap_destroy(&apiData->syscalls);

loadModule_0x1_fail_1:

    free(apiData);

loadModule_0x1_fail_0:

    return false;
}

void SharemindModule_unload_0x1(SharemindModule * const m) {
    assert(m);
    assert(m->apiData);

    ApiData * const apiData = (ApiData *) m->apiData;

    SharemindPdkMap_destroy(&apiData->pdks);
    SharemindSyscallMap_destroy(&apiData->syscalls);
    free(apiData);
}

SharemindModuleApiError SharemindModule_init_0x1(SharemindModule * const m) {
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
                return SHAREMIND_MODULE_API_API_ERROR;
            }
            m->moduleHandle = context.moduleHandle;
            return SHAREMIND_MODULE_API_OK;
        #define SHAREMIND_EC(theirs,ours) \
            case SHAREMIND_MODULE_API_0x1_ ## theirs: \
                SharemindModule_setError(m, \
                                         SHAREMIND_MODULE_API_ ## ours, \
                                         "Module returned " #theirs "!"); \
                return SHAREMIND_MODULE_API_ ## ours
        #define SHAREMIND_EC2(e) SHAREMIND_EC(e,e)
        SHAREMIND_EC2(OUT_OF_MEMORY);
        SHAREMIND_EC2(IMPLEMENTATION_LIMITS_REACHED);
        SHAREMIND_EC2(SHAREMIND_ERROR);
        SHAREMIND_EC2(MODULE_ERROR);
        SHAREMIND_EC(GENERAL_ERROR, MODULE_ERROR);
        SHAREMIND_EC(MISSING_FACILITY, MODULE_ERROR);
        SHAREMIND_EC(INVALID_MODULE_CONFIGURATION, MODULE_ERROR);
        #undef SHAREMIND_EC2
        #undef SHAREMIND_EC
        default:
            SharemindModule_setError(m,
                                     SHAREMIND_MODULE_API_API_ERROR,
                                     "Module returned an unexpected error!");
            return SHAREMIND_MODULE_API_API_ERROR;
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
    SharemindSyscallMap_value * const v =
            SharemindSyscallMap_at(&apiData->syscalls, index);
    return v ? (&v->value) : NULL;
}


SharemindSyscall * SharemindModule_findSyscall_0x1(const SharemindModule * m,
                                                   const char * signature)
{
    ApiData * const apiData = (ApiData *) m->apiData;
    SharemindSyscallMap_value * const v =
            SharemindSyscallMap_get(&apiData->syscalls, signature);
    return v ? (&v->value) : NULL;
}

size_t SharemindModule_numPdks_0x1(const SharemindModule * m) {
    ApiData * const apiData = (ApiData *) m->apiData;

    return apiData->pdks.size;
}

SharemindPdk * SharemindModule_pdk_0x1(const SharemindModule * m, size_t index)
{
    ApiData * const apiData = (ApiData *) m->apiData;
    SharemindPdkMap_value * const v =
            SharemindPdkMap_at(&apiData->pdks, index);
    return v ? (&v->value) : NULL;
}


SharemindPdk * SharemindModule_findPdk_0x1(const SharemindModule * m,
                                           const char * name)
{
    ApiData * const apiData = (ApiData *) m->apiData;
    SharemindPdkMap_value * const v = SharemindPdkMap_get(&apiData->pdks, name);
    return v ? (&v->value) : NULL;
}


size_t SharemindModule_numPds_0x1(const SharemindModule * m) {
    assert(m);
    ApiData * const apiData = (ApiData *) m->apiData;
    return SharemindPdkMap_numPds(&apiData->pdks);
}

SharemindPd * SharemindModule_pd_0x1(const SharemindModule * m, size_t index) {
    assert(m);
    ApiData * const apiData = (ApiData *) m->apiData;
    return SharemindPdkMap_pd(&apiData->pdks, index);
}

SharemindPd * SharemindModule_findPd_0x1(const SharemindModule * m,
                                         const char * name)
{
    assert(m);
    assert(name);
    ApiData * const apiData = (ApiData *) m->apiData;
    return SharemindPdkMap_findPd(&apiData->pdks, name);
}
