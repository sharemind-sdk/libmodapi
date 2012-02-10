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


int SHAREMIND_PDK_init(SHAREMIND_PDK * pdk,
                  size_t pdk_index,
                  const char * name,
                  void * pd_startup_impl,
                  void * pd_startup_wrapper,
                  void * pd_shutdown_impl,
                  void * pd_shutdown_wrapper,
                  void * pd_process_startup_impl,
                  void * pd_process_startup_wrapper,
                  void * pd_process_shutdown_impl,
                  void * pd_process_shutdown_wrapper,
                  SHAREMIND_Module * module)
{
    assert(pdk);
    assert(name);
    assert(pd_startup_impl);
    assert(pd_shutdown_impl);
    assert(pd_process_startup_impl);
    assert(pd_process_shutdown_impl);
    assert(module);
    assert(module->modapi);

    if (!SHAREMIND_Module_refs_ref(module)) {
        OOR(module->modapi);
        return 0;
    }

    pdk->pdk_index = pdk_index;

    pdk->name = strdup(name);
    if (unlikely(!pdk->name)) {
        SHAREMIND_Module_refs_unref(module);
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
    SHAREMIND_FacilityMap_init(&pdk->pdFacilityMap, &module->pdFacilityMap);
    SHAREMIND_FacilityMap_init(&pdk->pdpiFacilityMap, &module->pdpiFacilityMap);
    return 1;
}

void SHAREMIND_PDK_destroy(SHAREMIND_PDK * pdk) {
    assert(pdk);
    assert(pdk->name);
    assert(pdk->pd_startup_impl_or_wrapper);
    assert(pdk->pd_shutdown_impl_or_wrapper);
    assert(pdk->pdpi_startup_impl_or_wrapper);
    assert(pdk->pdpi_shutdown_impl_or_wrapper);
    assert(pdk->module);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pdk);

    free(pdk->name);
    SHAREMIND_Module_refs_unref(pdk->module);

    SHAREMIND_FacilityMap_destroy(&pdk->pdFacilityMap);
    SHAREMIND_FacilityMap_destroy(&pdk->pdpiFacilityMap);
}

const char * SHAREMIND_PDK_get_name(const SHAREMIND_PDK * pdk) {
    assert(pdk);
    assert(pdk->name);
    return pdk->name;
}

SHAREMIND_Module * SHAREMIND_PDK_get_module(const SHAREMIND_PDK * pdk) {
    assert(pdk);
    assert(pdk->module);
    return pdk->module;
}

SHAREMIND_MODAPI * SHAREMIND_PDK_get_modapi(const SHAREMIND_PDK * pdk) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    return pdk->module->modapi;
}

size_t SHAREMIND_PDK_get_index(const SHAREMIND_PDK * pdk) {
    assert(pdk);
    return pdk->pdk_index;
}

int SHAREMIND_PDK_set_pd_facility(SHAREMIND_PDK * pdk, const char * name, void * facility, void * context) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&pdk->pdFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_PDK_get_pd_facility(const SHAREMIND_PDK * pdk, const char * name) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&pdk->pdFacilityMap, name);
}

int SHAREMIND_PDK_set_pdpi_facility(SHAREMIND_PDK * pdk, const char * name, void * facility, void * context) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&pdk->pdpiFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_PDK_get_pdpi_facility(const SHAREMIND_PDK * pdk, const char * name) {
    assert(pdk);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&pdk->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SHAREMIND_PDK)
