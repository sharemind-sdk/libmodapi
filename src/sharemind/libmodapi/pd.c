/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "pd.h"

#include <stdlib.h>
#include "apis.h"
#include "modapi.h"
#include "module.h"
#include "pdk.h"


SHAREMIND_PD * SHAREMIND_PD_new(SHAREMIND_PDK * pdk, const char * name, const char * conf) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    assert(pdk->module->moduleHandle);
    assert(name);
    assert(name[0]);

    if (!SHAREMIND_PDK_refs_ref(pdk)) {
        OOR(pdk->module->modapi);
        return NULL;
    }

    SHAREMIND_PD * const pd = (SHAREMIND_PD *) malloc(sizeof(SHAREMIND_PD));
    if (unlikely(!pd))
        goto SHAREMIND_PD_new_fail_0;

    pd->name = strdup(name);
    if (unlikely(!pd->name))
        goto SHAREMIND_PD_new_fail_1;

    if (likely(conf && conf[0])) {
        pd->conf = strdup(conf);
        if (!pd->conf)
            goto SHAREMIND_PD_new_fail_2;
    } else {
        pd->conf = NULL;
    }

    pd->pdHandle = NULL; /* Just in case */
    pd->pdk = pdk;
    pd->isStarted = false;
    pd->facilityContext = NULL; /* Just in case */
    SHAREMIND_FacilityMap_init(&pd->pdFacilityMap, &pdk->pdFacilityMap);
    SHAREMIND_FacilityMap_init(&pd->pdpiFacilityMap, &pdk->pdpiFacilityMap);
    SHAREMIND_REFS_INIT(pd);
    SHAREMIND_NAMED_REFS_INIT(pd,startedRefs);
    return pd;

SHAREMIND_PD_new_fail_2:

    free(pd->name);

SHAREMIND_PD_new_fail_1:

    free(pd);

SHAREMIND_PD_new_fail_0:

    OOM(pdk->module->modapi);
    SHAREMIND_PDK_refs_unref(pdk);
    return NULL;
}

void SHAREMIND_PD_free(SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->pdk);

    if (pd->isStarted)
        SHAREMIND_PD_stop(pd);

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pd);
    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

    if (likely(pd->conf))
        free(pd->conf);
    free(pd->name);
    SHAREMIND_PDK_refs_unref(pd->pdk);

    SHAREMIND_FacilityMap_destroy(&pd->pdFacilityMap);
    SHAREMIND_FacilityMap_destroy(&pd->pdpiFacilityMap);
    free(pd);
}

bool SHAREMIND_PD_is_started(const SHAREMIND_PD * pd) {
    assert(pd);
    return pd->isStarted;
}

bool SHAREMIND_PD_start(SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->pd_start);

    if (pd->isStarted)
        return true;

    return (*(pd->pdk->module->api->pd_start))(pd);
}

void SHAREMIND_PD_stop(SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->pd_start);

    if (!pd->isStarted)
        return;

    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

    (*(pd->pdk->module->api->pd_stop))(pd);
    pd->isStarted = false;
}

SHAREMIND_PDK * SHAREMIND_PD_get_pdk(const SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    return pd->pdk;
}

SHAREMIND_Module * SHAREMIND_PD_get_module(const SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    return pd->pdk->module;
}

SHAREMIND_MODAPI * SHAREMIND_PD_get_modapi(const SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);
    return pd->pdk->module->modapi;
}

const char * SHAREMIND_PD_get_name(const SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->name[0]);
    return pd->name;
}

const char * SHAREMIND_PD_get_conf(const SHAREMIND_PD * pd) {
    assert(pd);
    assert(pd->conf);
    assert(pd->conf[0]);
    return pd->conf;
}

void * SHAREMIND_PD_get_handle(const SHAREMIND_PD * pd) {
    assert(pd);
    return pd->pdHandle;
}

void SHAREMIND_PD_set_facility_context(SHAREMIND_PD * pd, void * facilityContext) {
    assert(pd);
    pd->facilityContext = facilityContext;
}

void * SHAREMIND_PD_get_facility_context(const SHAREMIND_PD * pd) {
    assert(pd);
    return pd->facilityContext;
}

int SHAREMIND_PD_set_facility(SHAREMIND_PD * pd, const char * name, void * facility, void * context) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&pd->pdFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_PD_get_facility(const SHAREMIND_PD * pd, const char * name) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&pd->pdFacilityMap, name);
}

int SHAREMIND_PD_set_pdpi_facility(SHAREMIND_PD * pd, const char * name, void * facility, void * context) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_set(&pd->pdpiFacilityMap, name, facility, context);
}

const SHAREMIND_Facility * SHAREMIND_PD_get_pdpi_facility(const SHAREMIND_PD * pd, const char * name) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SHAREMIND_FacilityMap_get(&pd->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SHAREMIND_PD)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS(SHAREMIND_PD,startedRefs)
