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

    SharemindPd * pd;

    if (!SharemindPdk_refs_ref(pdk)) {
        SharemindPdk_setErrorOor(pdk);
        goto SharemindPd_new_fail_0;
    }

    pd = (SharemindPd *) malloc(sizeof(SharemindPd));
    if (unlikely(!pd)) {
        SharemindPdk_setErrorOom(pdk);
        goto SharemindPd_new_fail_1;
    }

    if (unlikely(SharemindRecursiveMutex_init(&pd->mutex) != SHAREMIND_MUTEX_OK)) {
        SharemindPdk_setErrorMie(pdk);
        goto SharemindPd_new_fail_2;
    }

    pd->name = strdup(name);
    if (unlikely(!pd->name)) {
        SharemindPdk_setErrorOom(pdk);
        goto SharemindPd_new_fail_3;
    }

    if (likely(conf && conf[0])) {
        pd->conf = strdup(conf);
        if (!pd->conf) {
            SharemindPdk_setErrorOom(pdk);
            goto SharemindPd_new_fail_4;
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

SharemindPd_new_fail_4:

    free(pd->name);

SharemindPd_new_fail_3:

    if (unlikely(SharemindRecursiveMutex_destroy(&pd->mutex) != SHAREMIND_MUTEX_OK))
        abort();

SharemindPd_new_fail_2:

    free(pd);

SharemindPd_new_fail_1:

    SharemindPdk_refs_unref(pdk);

SharemindPd_new_fail_0:

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

    SharemindFacilityMap_destroy(&pd->facilityMap);
    SharemindFacilityMap_destroy(&pd->pdpiFacilityMap);
    if (unlikely(SharemindRecursiveMutex_destroy(&pd->mutex) != SHAREMIND_MUTEX_OK))
        abort();
    free(pd);
}

#define DOLOCK(pd,lock) \
    if (unlikely(SharemindRecursiveMutex_ ## lock(&(pd)->mutex) != SHAREMIND_MUTEX_OK)) { \
        abort(); \
    } else (void) 0
#define LOCK(pd) DOLOCK((pd),lock)
#define UNLOCK(pd) DOLOCK((pd),unlock)
#define LOCK_CONST(pd) DOLOCK((pd),lock_const)
#define UNLOCK_CONST(pd) DOLOCK((pd),unlock_const)

SHAREMIND_LASTERROR_DEFINE_FUNCTIONS(Pd)

bool SharemindPd_isStarted(const SharemindPd * pd) {
    assert(pd);
    LOCK_CONST(pd);
    const bool r = pd->isStarted;
    UNLOCK_CONST(pd);
    return r;
}

bool SharemindPd_start(SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->startPd);

    LOCK(pd);
    if (pd->isStarted) {
        UNLOCK(pd);
        return true;
    }

    const bool r = (*(pd->pdk->module->api->startPd))(pd);
    UNLOCK(pd);
    return r;
}

void SharemindPd_stop(SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->api);
    assert(pd->pdk->module->api->stopPd);

    LOCK(pd);
    if (pd->isStarted) {
        SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

        (*(pd->pdk->module->api->stopPd))(pd);
        pd->isStarted = false;
    }
    UNLOCK(pd);
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
    LOCK_CONST(pd);
    void * const r = pd->pdHandle;
    UNLOCK_CONST(pd);
    return r;
}

SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(Pd,pdpi,Pdpi)

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPd)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPd,startedRefs)
