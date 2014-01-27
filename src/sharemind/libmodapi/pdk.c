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

    if (!SharemindModule_refs_ref(module)) {
        OOR(module->modapi);
        return 0;
    }

    pdk->pdk_index = pdk_index;

    pdk->name = strdup(name);
    if (unlikely(!pdk->name)) {
        SharemindModule_refs_unref(module);
        return 0;
    }

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
    SharemindModule_refs_unref(pdk->module);

    SharemindFacilityMap_destroy(&pdk->pdFacilityMap);
    SharemindFacilityMap_destroy(&pdk->pdpiFacilityMap);
}

const char * SharemindPdk_get_name(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->name);
    return pdk->name;
}

SharemindModule * SharemindPdk_get_module(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->module);
    return pdk->module;
}

SharemindModuleApi * SharemindPdk_get_modapi(const SharemindPdk * pdk) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    return pdk->module->modapi;
}

size_t SharemindPdk_get_index(const SharemindPdk * pdk) {
    assert(pdk);
    return pdk->pdk_index;
}

bool SharemindPdk_set_pd_facility(SharemindPdk * pdk, const char * name, void * facility, void * context) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&pdk->pdFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindPdk_get_pd_facility(const SharemindPdk * pdk, const char * name) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&pdk->pdFacilityMap, name);
}

bool SharemindPdk_set_pdpi_facility(SharemindPdk * pdk, const char * name, void * facility, void * context) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&pdk->pdpiFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindPdk_get_pdpi_facility(const SharemindPdk * pdk, const char * name) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&pdk->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SharemindPdk)
