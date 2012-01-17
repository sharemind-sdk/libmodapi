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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../likely.h"
#include "api_0x1.h"
#include "modapi.h"
#include "pd.h"
#include "pdk.h"


static void * SMVM_PDPI_get_facility_wrapper(SMVM_MODAPI_0x1_PDPI_Wrapper * w, const char * name) {
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SMVM_PDPI_get_facility((SMVM_PDPI *) w->internal, name);
}

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

    pdpi->pd = pd;
    pdpi->facilityContext = NULL; /* Just in case */
    SMVM_FacilityMap_init(&pdpi->pdpiFacilityMap, &pd->pdpiFacilityMap);

    const SMVM_PDK * const pdk = pd->pdk;
    SMVM_MODAPI_0x1_PDPI_Wrapper pdpiWrapper = {
        .pdHandle = pd->pdHandle,
        .getPdpiFacility = &SMVM_PDPI_get_facility_wrapper,
        .internal = pdpi
    };

    const int r = (*((SMVM_MODAPI_0x1_PDPI_Startup) pdk->pdpi_startup_impl_or_wrapper))(&pdpiWrapper);
    if (likely(r == 0)) {
        pdpi->pdProcessHandle = pdpiWrapper.pdProcessHandle;
        SMVM_REFS_INIT(pdpi);
        return pdpi;
    }

    SMVM_FacilityMap_destroy(&pdpi->pdpiFacilityMap);
    free(pdpi);

    const char * const errorFormatString = "PDPI startup failed with code %d from the module!";
    const size_t len = strlen(errorFormatString) + sizeof(int) * 3; /* -"%d" + '\0' + '-' + 3 for each byte of int */
    char * const errorString = (char *) malloc(len);
    if (likely(errorString))
        snprintf(errorString, len, errorFormatString, r);
    SMVM_MODAPI_setErrorWithDynamicString(pdk->module->modapi, SMVM_MODAPI_PDPI_STARTUP_FAILED, errorString);

    SMVM_PD_unref(pd);
    return NULL;
}

void SMVM_PDPI_free(SMVM_PDPI * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    SMVM_REFS_ASSERT_IF_REFERENCED(pdpi);

    const SMVM_PDK * const pdk = pdpi->pd->pdk;
    SMVM_MODAPI_0x1_PDPI_Wrapper pdpiWrapper = {
        .pdProcessHandle = pdpi->pdProcessHandle,
        .pdHandle = pdpi->pd->pdHandle,
        .getPdpiFacility = &SMVM_PDPI_get_facility_wrapper,
        .internal = pdpi
    };
    (*((SMVM_MODAPI_0x1_PDPI_Shutdown) pdk->pdpi_shutdown_impl_or_wrapper))(&pdpiWrapper);
    SMVM_PD_unref(pdpi->pd);
    SMVM_FacilityMap_destroy(&pdpi->pdpiFacilityMap);
    free(pdpi);
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
