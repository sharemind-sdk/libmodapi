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


SharemindPdpi * SharemindPdpi_new(SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);

    if (!SharemindPd_refs_ref(pd)) {
        OOR(pd->pdk->module->modapi);
        return NULL;
    }

    SharemindPdpi * const pdpi = (SharemindPdpi *) malloc(sizeof(SharemindPdpi));
    if (unlikely(!pdpi)) {
        OOM(pd->pdk->module->modapi);
        SharemindPd_refs_unref(pd);
        return NULL;
    }

    pdpi->pdProcessHandle = NULL; /* Just in case */
    pdpi->pd = pd;
    pdpi->isStarted = false;
    pdpi->facilityContext = NULL; /* Just in case */
    SharemindFacilityMap_init(&pdpi->pdpiFacilityMap, &pd->pdpiFacilityMap);
    SHAREMIND_REFS_INIT(pdpi);
    SHAREMIND_NAMED_REFS_INIT(pdpi,startedRefs);
    return pdpi;
}

void SharemindPdpi_free(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pdpi);

    if (pdpi->isStarted)
        SharemindPdpi_stop(pdpi);

    SharemindPd_refs_unref(pdpi->pd);
    SharemindFacilityMap_destroy(&pdpi->pdpiFacilityMap);
    free(pdpi);
}

bool SharemindPdpi_is_started(const SharemindPdpi * pdpi) {
    assert(pdpi);
    return pdpi->isStarted;
}

bool SharemindPdpi_start(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);
    assert(pdpi->pd->pdk->module->modapi);

    if (pdpi->isStarted)
        return true;

    SharemindPd * const pd = pdpi->pd;
    const SharemindModule * const module = pd->pdk->module;
    if (!SharemindPd_startedRefs_ref(pd)) {
        OOR(module->modapi);
        return false;
    }

    bool r = (*(module->api->pdpi_start))(pdpi);
    pdpi->isStarted = r;
    return r;
}

void SharemindPdpi_stop(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);

    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pdpi,startedRefs);

    (*(pdpi->pd->pdk->module->api->pdpi_stop))(pdpi);

    pdpi->isStarted = false;
    SharemindPd_startedRefs_unref(pdpi->pd);
}

void * SharemindPdpi_get_handle(const SharemindPdpi * pdpi) {
    assert(pdpi);
    return pdpi->pdProcessHandle;
}

SharemindPd * SharemindPdpi_get_pd(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    return pdpi->pd;
}

SharemindPdk * SharemindPdpi_get_pdk(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    return pdpi->pd->pdk;
}

SharemindModule * SharemindPdpi_get_module(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    return pdpi->pd->pdk->module;
}

SharemindModuleApi * SharemindPdpi_get_modapi(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);
    return pdpi->pd->pdk->module->modapi;
}

int SharemindPdpi_set_facility(SharemindPdpi * pdpi, const char * name, void * facility, void * context) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&pdpi->pdpiFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindPdpi_get_facility(const SharemindPdpi * pdpi, const char * name) {
    assert(pdpi);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&pdpi->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SharemindPdpi)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS(SharemindPdpi,startedRefs)
