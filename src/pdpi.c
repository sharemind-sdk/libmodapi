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


SharemindPdpi * SharemindPd_newPdpi(SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);

    SharemindPdpi * pdpi;

    if (!SharemindPd_refs_ref(pd)) {
        SharemindPd_setErrorOor(pd);
        goto SharemindPdpi_new_error_0;
    }

    pdpi = (SharemindPdpi *) malloc(sizeof(SharemindPdpi));
    if (unlikely(!pdpi)) {
        SharemindPd_setErrorOom(pd);
        goto SharemindPdpi_new_error_1;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(pdpi)) {
        SharemindPd_setErrorMie(pd);
        goto SharemindPdpi_new_error_2;
    }

    SHAREMIND_TAG_INIT(pdpi);

    pdpi->pdProcessHandle = NULL; /* Just in case */
    pdpi->pd = pd;
    pdpi->isStarted = false;
    pdpi->facilityContext = NULL; /* Just in case */
    SharemindFacilityMap_init(&pdpi->facilityMap, &pd->pdpiFacilityMap);
    SHAREMIND_REFS_INIT(pdpi);
    SHAREMIND_NAMED_REFS_INIT(pdpi,startedRefs);
    return pdpi;

SharemindPdpi_new_error_2:

    free(pdpi);

SharemindPdpi_new_error_1:

    SharemindPd_refs_unref(pd);

SharemindPdpi_new_error_0:

    return NULL;
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

    SHAREMIND_TAG_DESTROY(pdpi);

    SharemindPd_refs_unref(pdpi->pd);
    SharemindFacilityMap_destroy(&pdpi->facilityMap);
    SHAREMIND_RECURSIVE_LOCK_DEINIT(pdpi);
    free(pdpi);
}

SHAREMIND_LASTERROR_FUNCTIONS_DEFINE(SharemindPdpi)

bool SharemindPdpi_isStarted(const SharemindPdpi * pdpi) {
    assert(pdpi);
    SharemindPdpi_lockConst(pdpi);
    const bool r = pdpi->isStarted;
    SharemindPdpi_unlockConst(pdpi);
    return r;
}

SharemindModuleApiError SharemindPdpi_start(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);
    assert(pdpi->pd->pdk->module->modapi);

    SharemindPdpi_lock(pdpi);
    if (pdpi->isStarted) {
        SharemindPdpi_unlock(pdpi);
        return true;
    }

    SharemindPd * const pd = pdpi->pd;
    const SharemindModule * const module = pd->pdk->module;
    if (!SharemindPd_startedRefs_ref(pd)) {
        SharemindPdpi_setErrorOor(pdpi);
        SharemindPdpi_unlock(pdpi);
        return SHAREMIND_MODULE_API_IMPLEMENTATION_LIMITS_REACHED;
    }

    const SharemindModuleApiError r = (*(module->api->startPdpi))(pdpi);
    if (likely(r == SHAREMIND_MODULE_API_OK)) {
        pdpi->isStarted = r;
    } else {
        SharemindPd_startedRefs_unref(pd);
    }
    SharemindPdpi_unlock(pdpi);
    return r;
}

void SharemindPdpi_stop(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);

    SharemindPdpi_lock(pdpi);
    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pdpi,startedRefs);

    (*(pdpi->pd->pdk->module->api->stopPdpi))(pdpi);

    pdpi->isStarted = false;
    SharemindPdpi_unlock(pdpi);
    SharemindPd_startedRefs_unref(pdpi->pd);
}

void * SharemindPdpi_handle(const SharemindPdpi * pdpi) {
    assert(pdpi);
    SharemindPdpi_lockConst(pdpi);
    void * const r = pdpi->pdProcessHandle;
    SharemindPdpi_unlockConst(pdpi);
    return r;
}

SharemindPd * SharemindPdpi_pd(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    return pdpi->pd; // No locking: const after SharemindPdpi_new
}

SharemindPdk * SharemindPdpi_pdk(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    return pdpi->pd->pdk; // No locking: const after SharemindPdpi_new
}

SharemindModule * SharemindPdpi_module(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    return pdpi->pd->pdk->module; // No locking: const after SharemindPdpi_new
}

SharemindModuleApi * SharemindPdpi_modapi(const SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);
    // No locking: const after SharemindPdpi_new
    return pdpi->pd->pdk->module->modapi;
}

SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(SharemindPdpi)

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindPdpi)

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPdpi)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPdpi,startedRefs)
