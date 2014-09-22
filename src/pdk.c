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
#include "pd.h"


SHAREMIND_STRINGMAP_DEFINE(SharemindPdMap,SharemindPd,malloc,free,strdup,)

bool SharemindPdk_init(SharemindPdk * pdk,
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
        SharemindModuleApi_setErrorOor(module->modapi);
        goto SharemindPdk_init_error_0;
    }
    #endif

    if (!SHAREMIND_LOCK_INIT(pdk)) {
        SharemindModuleApi_setErrorMie(module->modapi);
        goto SharemindPdk_init_error_1;
    }

    pdk->name = strdup(name);
    if (unlikely(!pdk->name)) {
        SharemindModuleApi_setErrorOom(module->modapi);
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
    SharemindFacilityMap_init(&pdk->pdFacilityMap, &module->pdFacilityMap);
    SharemindFacilityMap_init(&pdk->pdpiFacilityMap, &module->pdpiFacilityMap);
    SharemindPdMap_init(&pdk->pds);
    return true;

SharemindPdk_init_error_2:

    SHAREMIND_LOCK_DEINIT(pdk);

SharemindPdk_init_error_1:

    #ifndef NDEBUG
    SharemindModule_refs_unref(module);
    #endif

SharemindPdk_init_error_0:

    return false;
}

void SharemindPdk_destroy(SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->name);
    assert(pdk->pd_startup_impl_or_wrapper);
    assert(pdk->pd_shutdown_impl_or_wrapper);
    assert(pdk->pdpi_startup_impl_or_wrapper);
    assert(pdk->pdpi_shutdown_impl_or_wrapper);
    assert(pdk->module);

    assert(!pdk->pds.size);

    free(pdk->name);
    #ifndef NDEBUG
    SharemindModule_refs_unref(pdk->module);
    #endif

    SharemindPdMap_destroy(&pdk->pds);
    SharemindFacilityMap_destroy(&pdk->pdFacilityMap);
    SharemindFacilityMap_destroy(&pdk->pdpiFacilityMap);
    SHAREMIND_LOCK_DEINIT(pdk);
}

SHAREMIND_LOCK_FUNCTIONS_DEFINE(SharemindPdk)
SHAREMIND_LASTERROR_FUNCTIONS_DEFINE(SharemindPdk)

const char * SharemindPdk_name(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->name);
    return pdk->name; // No locking: const after SharemindPdk_init
}

SharemindModule * SharemindPdk_module(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->module);
    return pdk->module; // No locking: const after SharemindPdk_init
}

SharemindModuleApi * SharemindPdk_modapi(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    return pdk->module->modapi; // No locking: const after SharemindPdk_init
}

size_t SharemindPdk_index(const SharemindPdk * pdk) {
    assert(pdk);
    return pdk->pdk_index; // No locking: const after SharemindPdk_init
}

SharemindPd * SharemindPdk_findPd(const SharemindPdk * pdk, const char * name) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    SharemindModuleApi_lock(pdk->module->modapi);
    SharemindPd * const r = SharemindPdMap_get(&pdk->pds, name);
    SharemindModuleApi_unlock(pdk->module->modapi);
    return r;
}

SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindPdk,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindPdk,pdpi,Pdpi)
