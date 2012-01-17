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
#include "module.h"
#include "pd.h"
#include "pdk.h"


SMVM_PDPI * SMVM_PDPI_new(SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);
    assert(pd->isStarted);

    if (!SMVM_PD_ref(pd))
        return NULL;

    SMVM_PDPI * const pdpi = (SMVM_PDPI *) malloc(sizeof(SMVM_PDPI));
    if (unlikely(!pdpi)) {
        OOM(pd->pdk->module->modapi);
        SMVM_PD_unref(pd);
        return NULL;
    }

    pdpi->pdProcessHandle = NULL; /* Just in case */
    pdpi->pd = pd;
    pdpi->isStarted = false;
    pdpi->facilityContext = NULL; /* Just in case */
    SMVM_FacilityMap_init(&pdpi->pdpiFacilityMap, &pd->pdpiFacilityMap);
    SMVM_REFS_INIT(pdpi);
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
        (*(pdpi->pd->pdk->module->api->pdpi_stop))(pdpi);

    SMVM_PD_unref(pdpi->pd);
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

    bool r = (*(pdpi->pd->pdk->module->api->pdpi_start))(pdpi);
    pdpi->isStarted = r;
    return r;
}

void SMVM_PDPI_stop(SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);

    (*(pdpi->pd->pdk->module->api->pdpi_stop))(pdpi);

    pdpi->isStarted = false;
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

void SMVM_PDPI_set_facility_context(SMVM_PDPI * pdpi, void * facilityContext) {
    assert(pdpi);
    pdpi->facilityContext = facilityContext;
}

void * SMVM_PDPI_get_facility_context(const SMVM_PDPI * pdpi) {
    assert(pdpi);
    return pdpi->facilityContext;
}

int SMVM_PDPI_set_facility(SMVM_PDPI * pdpi, const char * name, void * facility) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&pdpi->pdpiFacilityMap, name, facility);
}

void * SMVM_PDPI_get_facility(const SMVM_PDPI * pdpi, const char * name) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&pdpi->pdpiFacilityMap, name);
}
