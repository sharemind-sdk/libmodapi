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


SMVM_PDPI * SMVM_PDPI_new(SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);

    if (!SMVM_PD_refs_ref(pd)) {
        OOR(pd->pdk->module->modapi);
        return NULL;
    }

    SMVM_PDPI * const pdpi = (SMVM_PDPI *) malloc(sizeof(SMVM_PDPI));
    if (unlikely(!pdpi)) {
        OOM(pd->pdk->module->modapi);
        SMVM_PD_refs_unref(pd);
        return NULL;
    }

    pdpi->pdProcessHandle = NULL; /* Just in case */
    pdpi->pd = pd;
    pdpi->isStarted = false;
    pdpi->facilityContext = NULL; /* Just in case */
    SMVM_FacilityMap_init(&pdpi->pdpiFacilityMap, &pd->pdpiFacilityMap);
    SMVM_REFS_INIT(pdpi);
    SMVM_NAMED_REFS_INIT(pdpi,startedRefs);
    return pdpi;
}

void SMVM_PDPI_free(SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);

    SMVM_REFS_ASSERT_IF_REFERENCED(pdpi);

    if (pdpi->isStarted)
        SMVM_PDPI_stop(pdpi);

    SMVM_PD_refs_unref(pdpi->pd);
    SMVM_FacilityMap_destroy(&pdpi->pdpiFacilityMap);
    free(pdpi);
}

bool SMVM_PDPI_is_started(const SMVM_PDPI * pdpi) {
    assert(pdpi);
    return pdpi->isStarted;
}

bool SMVM_PDPI_start(SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);
    assert(pdpi->pd->pdk->module->modapi);

    if (pdpi->isStarted)
        return true;

    SMVM_PD * const pd = pdpi->pd;
    const SMVM_Module * const module = pd->pdk->module;
    if (!SMVM_PD_startedRefs_ref(pd)) {
        OOR(module->modapi);
        return false;
    }

    bool r = (*(module->api->pdpi_start))(pdpi);
    pdpi->isStarted = r;
    return r;
}

void SMVM_PDPI_stop(SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);

    SMVM_NAMED_REFS_ASSERT_IF_REFERENCED(pdpi,startedRefs);

    (*(pdpi->pd->pdk->module->api->pdpi_stop))(pdpi);

    pdpi->isStarted = false;
    SMVM_PD_startedRefs_unref(pdpi->pd);
}

void * SMVM_PDPI_get_handle(const SMVM_PDPI * pdpi) {
    assert(pdpi);
    return pdpi->pdProcessHandle;
}

SMVM_PD * SMVM_PDPI_get_pd(const SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    return pdpi->pd;
}

SMVM_PDK * SMVM_PDPI_get_pdk(const SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    return pdpi->pd->pdk;
}

SMVM_Module * SMVM_PDPI_get_module(const SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    return pdpi->pd->pdk->module;
}

SMVM_MODAPI * SMVM_PDPI_get_modapi(const SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);
    return pdpi->pd->pdk->module->modapi;
}

int SMVM_PDPI_set_facility(SMVM_PDPI * pdpi, const char * name, void * facility, void * context) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&pdpi->pdpiFacilityMap, name, facility, context);
}

const SMVM_Facility * SMVM_PDPI_get_facility(const SMVM_PDPI * pdpi, const char * name) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&pdpi->pdpiFacilityMap, name);
}

SMVM_REFS_DEFINE_FUNCTIONS(SMVM_PDPI)
SMVM_NAMED_REFS_DEFINE_FUNCTIONS(SMVM_PDPI,startedRefs)
