/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "pdk.h"

#include <assert.h>
#include <sharemind/likely.h>
#include <stdlib.h>
#include <string.h>
#include "modapi.h"
#include "module.h"


int SharemindPdk_init(SharemindPdk * pdk,
                      size_t pdk_index,
                      const char * name,
                      void (* pd_startup_impl)(void),
                      void (* pd_startup_wrapper)(void),
                      void (* pd_shutdown_impl)(void),
                      void (* pd_shutdown_wrapper)(void),
                      void (* pd_process_startup_impl)(void),
                      void (* pd_process_startup_wrapper)(void),
                      void (* pd_process_shutdown_impl)(void),
                      void (* pd_process_shutdown_wrapper)(void),
                      SharemindModule * module)
{
    assert(pdk);
    assert(name);
    assert(pd_startup_impl);
    assert(pd_shutdown_impl);
    assert(pd_process_startup_impl);
    assert(pd_process_shutdown_impl);
    assert(module);
    assert(module->modapi);

    #ifndef NDEBUG
    if (unlikely(!SharemindModule_refs_ref(module))) {
        OOR(module->modapi);
        goto SharemindPdk_init_error_0;
    }
    #endif

    if (unlikely(SharemindMutex_init(&pdk->mutex) != SHAREMIND_MUTEX_OK)) {
        MIE(module->modapi);
        goto SharemindPdk_init_error_1;
    }

    pdk->name = strdup(name);
    if (unlikely(!pdk->name)) {
        OOM(module->modapi);
        goto SharemindPdk_init_error_2;
    }

    pdk->pdk_index = pdk_index;

    if (pd_startup_wrapper) {
        pdk->pd_startup_impl_or_wrapper = pd_startup_wrapper;
        pdk->pd_startup_null_or_impl = pd_startup_impl;
    } else {
        pdk->pd_startup_impl_or_wrapper = pd_startup_impl;
        pdk->pd_startup_null_or_impl = NULL;
    }

    if (pd_shutdown_wrapper) {
        pdk->pd_shutdown_impl_or_wrapper = pd_shutdown_wrapper;
        pdk->pd_shutdown_null_or_impl = pd_shutdown_impl;
    } else {
        pdk->pd_shutdown_impl_or_wrapper = pd_shutdown_impl;
        pdk->pd_shutdown_null_or_impl = NULL;
    }

    if (pd_process_startup_wrapper) {
        pdk->pdpi_startup_impl_or_wrapper = pd_process_startup_wrapper;
        pdk->pdpi_startup_null_or_impl = pd_process_startup_impl;
    } else {
        pdk->pdpi_startup_impl_or_wrapper = pd_process_startup_impl;
        pdk->pdpi_startup_null_or_impl = NULL;
    }

    if (pd_process_shutdown_wrapper) {
        pdk->pdpi_shutdown_impl_or_wrapper = pd_process_shutdown_wrapper;
        pdk->pdpi_shutdown_null_or_impl = pd_process_shutdown_impl;
    } else {
        pdk->pdpi_shutdown_impl_or_wrapper = pd_process_shutdown_impl;
        pdk->pdpi_shutdown_null_or_impl = NULL;
    }

    pdk->module = module;
    SHAREMIND_REFS_INIT(pdk);
    SharemindFacilityMap_init(&pdk->pdFacilityMap, &module->pdFacilityMap);
    SharemindFacilityMap_init(&pdk->pdpiFacilityMap, &module->pdpiFacilityMap);
    return 1;

SharemindPdk_init_error_2:

    if (unlikely(SharemindMutex_destroy(&pdk->mutex) != SHAREMIND_MUTEX_OK))
        abort();

SharemindPdk_init_error_1:

    #ifndef NDEBUG
    SharemindModule_refs_unref(module);
    #endif

SharemindPdk_init_error_0:

    return 0;
}

void SharemindPdk_destroy(SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->name);
    assert(pdk->pd_startup_impl_or_wrapper);
    assert(pdk->pd_shutdown_impl_or_wrapper);
    assert(pdk->pdpi_startup_impl_or_wrapper);
    assert(pdk->pdpi_shutdown_impl_or_wrapper);
    assert(pdk->module);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pdk);

    free(pdk->name);
    #ifndef NDEBUG
    SharemindModule_refs_unref(pdk->module);
    #endif

    SharemindFacilityMap_destroy(&pdk->pdFacilityMap);
    SharemindFacilityMap_destroy(&pdk->pdpiFacilityMap);
    if (unlikely(SharemindMutex_destroy(&pdk->mutex) != SHAREMIND_MUTEX_OK))
        abort();
}

#define DOLOCK(pdk,lock) \
    if (unlikely(SharemindMutex_ ## lock(&(pdk)->mutex) != SHAREMIND_MUTEX_OK)) { \
        abort(); \
    } else (void) 0
#define LOCK(pdk) DOLOCK((pdk),lock)
#define UNLOCK(pdk) DOLOCK(pdk,unlock)
#define LOCK_CONST(pdk) DOLOCK((pdk),lock_const)
#define UNLOCK_CONST(pdk) DOLOCK((pdk),unlock_const)

const char * SharemindPdk_get_name(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->name);
    return pdk->name; // No locking: const after SharemindPdk_init
}

SharemindModule * SharemindPdk_get_module(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->module);
    return pdk->module; // No locking: const after SharemindPdk_init
}

SharemindModuleApi * SharemindPdk_get_modapi(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    return pdk->module->modapi; // No locking: const after SharemindPdk_init
}

size_t SharemindPdk_get_index(const SharemindPdk * pdk) {
    assert(pdk);
    return pdk->pdk_index; // No locking: const after SharemindPdk_init
}

bool SharemindPdk_set_pd_facility(SharemindPdk * pdk,
                                  const char * name,
                                  void * facility,
                                  void * context)
{
    assert(pdk);
    assert(name);
    assert(name[0]);
    LOCK(pdk);
    const bool r = SharemindFacilityMap_set(&pdk->pdFacilityMap,
                                            name,
                                            facility,
                                            context);
    UNLOCK(pdk);
    return r;
}

const SharemindFacility * SharemindPdk_get_pd_facility(const SharemindPdk * pdk,
                                                       const char * name)
{
    assert(pdk);
    assert(name);
    assert(name[0]);
    LOCK_CONST(pdk);
    const SharemindFacility * const r =
            SharemindFacilityMap_get(&pdk->pdFacilityMap, name);
    UNLOCK_CONST(pdk);
    return r;
}

bool SharemindPdk_set_pdpi_facility(SharemindPdk * pdk,
                                    const char * name,
                                    void * facility,
                                    void * context)
{
    assert(pdk);
    assert(name);
    assert(name[0]);
    LOCK(pdk);
    const bool r = SharemindFacilityMap_set(&pdk->pdpiFacilityMap,
                                            name,
                                            facility,
                                            context);
    UNLOCK(pdk);
    return r;
}

const SharemindFacility * SharemindPdk_get_pdpi_facility(
        const SharemindPdk * pdk,
        const char * name)
{
    assert(pdk);
    assert(name);
    assert(name[0]);
    LOCK_CONST(pdk);
    const SharemindFacility * const r =
            SharemindFacilityMap_get(&pdk->pdpiFacilityMap, name);
    UNLOCK_CONST(pdk);
    return r;
}

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_MUTEX(SharemindPdk)
