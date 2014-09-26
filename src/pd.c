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
#include "modapi.h"
#include "module.h"
#include "pdk.h"


// Include API list:
#include "apis.c"


SharemindPd * SharemindPdk_newPd(SharemindPdk * pdk,
                                 const char * name,
                                 const char * conf)
{
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->modapi);
    assert(pdk->module->moduleHandle);
    assert(name);
    assert(name[0]);

    if (unlikely(SharemindModuleApi_findPd(pdk->module->modapi, name))) {
        SharemindPdk_setError(
                    pdk,
                    SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN,
                    "Protection domain with identical name already created!");
        goto SharemindPd_new_fail_0;
    }

    SharemindPd * const pd = SharemindPdMap_insertNew(&pdk->pds, name);
    if (unlikely(!pd)) {
        SharemindPdk_setErrorOom(pdk);
        goto SharemindPd_new_fail_0;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(pd)) {
        SharemindPdk_setErrorMie(pdk);
        goto SharemindPd_new_fail_1;
    }

    SHAREMIND_LASTERROR_INIT(pd);
    SHAREMIND_TAG_INIT(pd);

    pd->name = strdup(name);
    if (unlikely(!pd->name)) {
        SharemindPdk_setErrorOom(pdk);
        goto SharemindPd_new_fail_2;
    }

    if (likely(conf && conf[0])) {
        pd->conf = strdup(conf);
        if (!pd->conf) {
            SharemindPdk_setErrorOom(pdk);
            goto SharemindPd_new_fail_3;
        }
    } else {
        pd->conf = NULL;
    }

    pd->pdHandle = NULL; /* Just in case */
    pd->pdk = pdk;
    pd->isStarted = false;
    pd->facilityContext = NULL; /* Just in case */
    SharemindFacilityMap_init(&pd->facilityMap, &pdk->pdFacilityMap);
    SharemindFacilityMap_init(&pd->pdpiFacilityMap, &pdk->pdpiFacilityMap);
    SHAREMIND_REFS_INIT(pd);
    SHAREMIND_NAMED_REFS_INIT(pd,startedRefs);
    return pd;

SharemindPd_new_fail_3:

    free(pd->name);

SharemindPd_new_fail_2:

    SHAREMIND_RECURSIVE_LOCK_DEINIT(pd);

SharemindPd_new_fail_1:

    SharemindPdMap_remove(&pdk->pds, name);

SharemindPd_new_fail_0:

    return NULL;
}

void SharemindPd_free(SharemindPd * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->pdk);

    if (pd->isStarted)
        SharemindPd_stop(pd);

    SHAREMIND_TAG_DESTROY(pd);

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pd);
    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

    if (likely(pd->conf))
        free(pd->conf);
    char * const name = pd->name; // free(name) later

    SharemindFacilityMap_destroy(&pd->facilityMap);
    SharemindFacilityMap_destroy(&pd->pdpiFacilityMap);
    SHAREMIND_RECURSIVE_LOCK_DEINIT(pd);
    SharemindPdMap_remove(&pd->pdk->pds, name);
    free(name);
}

SHAREMIND_LASTERROR_FUNCTIONS_DEFINE(SharemindPd)

bool SharemindPd_isStarted(const SharemindPd * pd) {
    assert(pd);
    SharemindPd_lockConst(pd);
    const bool r = pd->isStarted;
    SharemindPd_unlockConst(pd);
    return r;
}

SharemindModuleApiError SharemindPd_start(SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->startPd);

    SharemindPd_lock(pd);
    if (pd->isStarted) {
        SharemindPd_unlock(pd);
        return SHAREMIND_MODULE_API_OK;
    }

    const SharemindModuleApiError r = (*(pd->pdk->module->api->startPd))(pd);
    if (likely(r == SHAREMIND_MODULE_API_OK))
        pd->isStarted = true;
    SharemindPd_unlock(pd);
    return r;
}

void SharemindPd_stop(SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->stopPd);

    SharemindPd_lock(pd);
    if (pd->isStarted) {
        SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

        (*(pd->pdk->module->api->stopPd))(pd);
        pd->isStarted = false;
    }
    SharemindPd_unlock(pd);
}

SharemindPdk * SharemindPd_pdk(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    return pd->pdk; // No locking: const after SharemindPd_new
}

SharemindModule * SharemindPd_module(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    return pd->pdk->module; // No locking: const after SharemindPd_new
}

SharemindModuleApi * SharemindPd_modapi(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);
    return pd->pdk->module->modapi; // No locking: const after SharemindPd_new
}

const char * SharemindPd_name(const SharemindPd * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->name[0]);
    return pd->name; // No locking: const after SharemindPd_new
}

const char * SharemindPd_conf(const SharemindPd * pd) {
    assert(pd);
    assert(pd->conf);
    assert(pd->conf[0]);
    return pd->conf; // No locking: const after SharemindPd_new
}

void * SharemindPd_handle(const SharemindPd * pd) {
    assert(pd);
    SharemindPd_lockConst(pd);
    void * const r = pd->pdHandle;
    SharemindPd_unlockConst(pd);
    return r;
}

SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(SharemindPd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindPd,pdpi,Pdpi)

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindPd)

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPd)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPd,
                                                           startedRefs)
