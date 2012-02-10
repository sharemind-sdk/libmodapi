/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "pdpi.h"

#include <assert.h>
#include <stdlib.h>
#include "modapi.h"
#include "module.h"
#include "pd.h"
#include "pdk.h"


SHAREMIND_PDPI * SHAREMIND_PDPI_new(SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);

    if (!SHAREMIND_PD_refs_ref(pd)) {
        OOR(pd->pdk->module->modapi);
        return NULL;
    }

    SHAREMIND_PDPI * const pdpi = (SHAREMIND_PDPI *) malloc(sizeof(SHAREMIND_PDPI));
    if (unlikely(!pdpi)) {
        OOM(pd->pdk->module->modapi);
        SHAREMIND_PD_refs_unref(pd);
        return NULL;
    }

    pdpi->pdProcessHandle = NULL; /* Just in case */
    pdpi->pd = pd;
    pdpi->isStarted = false;
    pdpi->facilityContext = NULL; /* Just in case */
    SHAREMIND_FacilityMap_init(&pdpi->pdpiFacilityMap, &pd->pdpiFacilityMap);
    SHAREMIND_REFS_INIT(pdpi);
    SHAREMIND_NAMED_REFS_INIT(pdpi,startedRefs);
    return pdpi;
}

void SHAREMIND_PDPI_free(SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pdpi);

    if (pdpi->isStarted)
        SHAREMIND_PDPI_stop(pdpi);

    SHAREMIND_PD_refs_unref(pdpi->pd);
    SHAREMIND_FacilityMap_destroy(&pdpi->pdpiFacilityMap);
    free(pdpi);
}

bool SHAREMIND_PDPI_is_started(const SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    return pdpi->isStarted;
}

bool SHAREMIND_PDPI_start(SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);
    assert(pdpi->pd->pdk->module->modapi);

    if (pdpi->isStarted)
        return true;

    SHAREMIND_PD * const pd = pdpi->pd;
    const SHAREMIND_Module * const module = pd->pdk->module;
    if (!SHAREMIND_PD_startedRefs_ref(pd)) {
        OOR(module->modapi);
        return false;
    }

    bool r = (*(module->api->pdpi_start))(pdpi);
    pdpi->isStarted = r;
    return r;
}

void SHAREMIND_PDPI_stop(SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);

    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pdpi,startedRefs);

    (*(pdpi->pd->pdk->module->api->pdpi_stop))(pdpi);

    pdpi->isStarted = false;
    SHAREMIND_PD_startedRefs_unref(pdpi->pd);
}

void * SHAREMIND_PDPI_get_handle(const SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    return pdpi->pdProcessHandle;
}

SHAREMIND_PD * SHAREMIND_PDPI_get_pd(const SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    return pdpi->pd;
}

SHAREMIND_PDK * SHAREMIND_PDPI_get_pdk(const SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    return pdpi->pd->pdk;
}

SHAREMIND_Module * SHAREMIND_PDPI_get_module(const SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    return pdpi->pd->pdk->module;
}

SHAREMIND_MODAPI * SHAREMIND_PDPI_get_modapi(const SHAREMIND_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);
    return pdpi->pd->pdk->module->modapi;
}

int SHAREMIND_PDPI_set_facility(SHAREMIND_PDPI * pdpi, const char * name, void * facility, void * context) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&pdpi->pdpiFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_PDPI_get_facility(const SHAREMIND_PDPI * pdpi, const char * name) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&pdpi->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SHAREMIND_PDPI)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS(SHAREMIND_PDPI,startedRefs)
