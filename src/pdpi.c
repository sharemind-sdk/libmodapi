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

    if (unlikely(SharemindRecursiveMutex_init(&pdpi->mutex) != SHAREMIND_MUTEX_OK)) {
        SharemindPd_setErrorMie(pd);
        goto SharemindPdpi_new_error_2;
    }

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

    SharemindPd_refs_unref(pdpi->pd);
    SharemindFacilityMap_destroy(&pdpi->facilityMap);
    if (unlikely(SharemindRecursiveMutex_destroy(&pdpi->mutex) != SHAREMIND_MUTEX_OK))
        abort();
    free(pdpi);
}

#define DOLOCK(pdpi,lock) \
    if (unlikely(SharemindRecursiveMutex_ ## lock(&(pdpi)->mutex) != SHAREMIND_MUTEX_OK)) { \
        abort(); \
    } else (void) 0
#define LOCK(pdpi) DOLOCK((pdpi),lock)
#define UNLOCK(pdpi) DOLOCK(pdpi,unlock)
#define LOCK_CONST(pdpi) DOLOCK((pdpi),lock_const)
#define UNLOCK_CONST(pdpi) DOLOCK((pdpi),unlock_const)

SHAREMIND_LASTERROR_DEFINE_FUNCTIONS(Pdpi)

bool SharemindPdpi_isStarted(const SharemindPdpi * pdpi) {
    assert(pdpi);
    LOCK_CONST(pdpi);
    const bool r = pdpi->isStarted;
    UNLOCK_CONST(pdpi);
    return r;
}

bool SharemindPdpi_start(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);
    assert(pdpi->pd->pdk->module->modapi);

    LOCK(pdpi);
    if (pdpi->isStarted) {
        UNLOCK(pdpi);
        return true;
    }

    SharemindPd * const pd = pdpi->pd;
    const SharemindModule * const module = pd->pdk->module;
    if (!SharemindPd_startedRefs_ref(pd)) {
        SharemindPdpi_setErrorOor(pdpi);
        UNLOCK(pdpi);
        return false;
    }

    const bool r = (*(module->api->startPdpi))(pdpi);
    if (r) {
        pdpi->isStarted = r;
    } else {
        SharemindPd_startedRefs_unref(pd);
    }
    UNLOCK(pdpi);
    return r;
}

void SharemindPdpi_stop(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->api);

    LOCK(pdpi);
    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pdpi,startedRefs);

    (*(pdpi->pd->pdk->module->api->stopPdpi))(pdpi);

    pdpi->isStarted = false;
    UNLOCK(pdpi);
    SharemindPd_startedRefs_unref(pdpi->pd);
}

void * SharemindPdpi_handle(const SharemindPdpi * pdpi) {
    assert(pdpi);
    LOCK_CONST(pdpi);
    void * const r = pdpi->pdProcessHandle;
    UNLOCK_CONST(pdpi);
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

SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(Pdpi)

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPdpi)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPdpi,startedRefs)
