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


SMVM_PD * SMVM_PD_new(SMVM_PDK * pdk, const char * name, const char * conf) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    assert(pdk->module->moduleHandle);
    assert(name);
    assert(name[0]);

    if (!SMVM_PDK_refs_ref(pdk)) {
        OOR(pdk->module->modapi);
        return NULL;
    }

    SMVM_PD * const pd = (SMVM_PD *) malloc(sizeof(SMVM_PD));
    if (unlikely(!pd))
        goto SMVM_PD_new_fail_0;

    pd->name = strdup(name);
    if (unlikely(!pd->name))
        goto SMVM_PD_new_fail_1;

    if (likely(conf && conf[0])) {
        pd->conf = strdup(conf);
        if (!pd->conf)
            goto SMVM_PD_new_fail_2;
    } else {
        pd->conf = NULL;
    }

    pd->pdHandle = NULL; /* Just in case */
    pd->pdk = pdk;
    pd->isStarted = false;
    pd->facilityContext = NULL; /* Just in case */
    SMVM_FacilityMap_init(&pd->pdFacilityMap, &pdk->pdFacilityMap);
    SMVM_FacilityMap_init(&pd->pdpiFacilityMap, &pdk->pdpiFacilityMap);
    SMVM_REFS_INIT(pd);
    SMVM_NAMED_REFS_INIT(pd,startedRefs);
    return pd;

SMVM_PD_new_fail_2:

    free(pd->name);

SMVM_PD_new_fail_1:

    free(pd);

SMVM_PD_new_fail_0:

    OOM(pdk->module->modapi);
    SMVM_PDK_refs_unref(pdk);
    return NULL;
}

void SMVM_PD_free(SMVM_PD * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->pdk);

    if (pd->isStarted)
        SMVM_PD_stop(pd);

    SMVM_REFS_ASSERT_IF_REFERENCED(pd);
    SMVM_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

    if (likely(pd->conf))
        free(pd->conf);
    free(pd->name);
    SMVM_PDK_refs_unref(pd->pdk);

    SMVM_FacilityMap_destroy(&pd->pdFacilityMap);
    SMVM_FacilityMap_destroy(&pd->pdpiFacilityMap);
    free(pd);
}

bool SMVM_PD_is_started(const SMVM_PD * pd) {
    assert(pd);
    return pd->isStarted;
}

bool SMVM_PD_start(SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->pd_start);

    if (pd->isStarted)
        return true;

    return (*(pd->pdk->module->api->pd_start))(pd);
}

void SMVM_PD_stop(SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->pd_start);

    if (!pd->isStarted)
        return;

    SMVM_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

    (*(pd->pdk->module->api->pd_stop))(pd);
    pd->isStarted = false;
}

SMVM_PDK * SMVM_PD_get_pdk(const SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    return pd->pdk;
}

SMVM_Module * SMVM_PD_get_module(const SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    return pd->pdk->module;
}

SMVM_MODAPI * SMVM_PD_get_modapi(const SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);
    return pd->pdk->module->modapi;
}

const char * SMVM_PD_get_name(const SMVM_PD * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->name[0]);
    return pd->name;
}

const char * SMVM_PD_get_conf(const SMVM_PD * pd) {
    assert(pd);
    assert(pd->conf);
    assert(pd->conf[0]);
    return pd->conf;
}

void * SMVM_PD_get_handle(const SMVM_PD * pd) {
    assert(pd);
    return pd->pdHandle;
}

void SMVM_PD_set_facility_context(SMVM_PD * pd, void * facilityContext) {
    assert(pd);
    pd->facilityContext = facilityContext;
}

void * SMVM_PD_get_facility_context(const SMVM_PD * pd) {
    assert(pd);
    return pd->facilityContext;
}

int SMVM_PD_set_facility(SMVM_PD * pd, const char * name, void * facility, void * context) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&pd->pdFacilityMap, name, facility, context);
}

const SMVM_Facility * SMVM_PD_get_facility(const SMVM_PD * pd, const char * name) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&pd->pdFacilityMap, name);
}

int SMVM_PD_set_pdpi_facility(SMVM_PD * pd, const char * name, void * facility, void * context) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_set(&pd->pdpiFacilityMap, name, facility, context);
}

const SMVM_Facility * SMVM_PD_get_pdpi_facility(const SMVM_PD * pd, const char * name) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SMVM_FacilityMap_get(&pd->pdpiFacilityMap, name);
}

SMVM_REFS_DEFINE_FUNCTIONS(SMVM_PD)
SMVM_NAMED_REFS_DEFINE_FUNCTIONS(SMVM_PD,startedRefs)
