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


SharemindPd * SharemindPd_new(SharemindPdk * pdk, const char * name, const char * conf) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    assert(pdk->module->moduleHandle);
    assert(name);
    assert(name[0]);

    if (!SharemindPdk_refs_ref(pdk)) {
        OOR(pdk->module->modapi);
        return NULL;
    }

    SharemindPd * const pd = (SharemindPd *) malloc(sizeof(SharemindPd));
    if (unlikely(!pd))
        goto SharemindPd_new_fail_0;

    pd->name = strdup(name);
    if (unlikely(!pd->name))
        goto SharemindPd_new_fail_1;

    if (likely(conf && conf[0])) {
        pd->conf = strdup(conf);
        if (!pd->conf)
            goto SharemindPd_new_fail_2;
    } else {
        pd->conf = NULL;
    }

    pd->pdHandle = NULL; /* Just in case */
    pd->pdk = pdk;
    pd->isStarted = false;
    pd->facilityContext = NULL; /* Just in case */
    SharemindFacilityMap_init(&pd->pdFacilityMap, &pdk->pdFacilityMap);
    SharemindFacilityMap_init(&pd->pdpiFacilityMap, &pdk->pdpiFacilityMap);
    SHAREMIND_REFS_INIT(pd);
    SHAREMIND_NAMED_REFS_INIT(pd,startedRefs);
    return pd;

SharemindPd_new_fail_2:

    free(pd->name);

SharemindPd_new_fail_1:

    free(pd);

SharemindPd_new_fail_0:

    OOM(pdk->module->modapi);
    SharemindPdk_refs_unref(pdk);
    return NULL;
}

void SharemindPd_free(SharemindPd * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->pdk);

    if (pd->isStarted)
        SharemindPd_stop(pd);

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pd);
    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

    if (likely(pd->conf))
        free(pd->conf);
    free(pd->name);
    SharemindPdk_refs_unref(pd->pdk);

    SharemindFacilityMap_destroy(&pd->pdFacilityMap);
    SharemindFacilityMap_destroy(&pd->pdpiFacilityMap);
    free(pd);
}

bool SharemindPd_is_started(const SharemindPd * pd) {
    assert(pd);
    return pd->isStarted;
}

bool SharemindPd_start(SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->pd_start);

    if (pd->isStarted)
        return true;

    return (*(pd->pdk->module->api->pd_start))(pd);
}

void SharemindPd_stop(SharemindPd * pd) {
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

SharemindPdk * SharemindPd_get_pdk(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    return pd->pdk;
}

SharemindModule * SharemindPd_get_module(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    return pd->pdk->module;
}

SharemindModuleApi * SharemindPd_get_modapi(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);
    return pd->pdk->module->modapi;
}

const char * SharemindPd_get_name(const SharemindPd * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->name[0]);
    return pd->name;
}

const char * SharemindPd_get_conf(const SharemindPd * pd) {
    assert(pd);
    assert(pd->conf);
    assert(pd->conf[0]);
    return pd->conf;
}

void * SharemindPd_get_handle(const SharemindPd * pd) {
    assert(pd);
    return pd->pdHandle;
}

int SharemindPd_set_facility(SharemindPd * pd, const char * name, void * facility, void * context) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&pd->pdFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindPd_get_facility(const SharemindPd * pd, const char * name) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&pd->pdFacilityMap, name);
}

int SharemindPd_set_pdpi_facility(SharemindPd * pd, const char * name, void * facility, void * context) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_set(&pd->pdpiFacilityMap, name, facility, context);
}

const SharemindFacility * SharemindPd_get_pdpi_facility(const SharemindPd * pd, const char * name) {
    assert(pd);
    assert(name);
    assert(name[0]);
    return SharemindFacilityMap_get(&pd->pdpiFacilityMap, name);
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SharemindPd)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS(SharemindPd,startedRefs)
