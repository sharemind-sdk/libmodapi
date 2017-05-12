/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#define SHAREMIND_INTERNAL_

#include "pd.h"

#include <stdlib.h>
#include "modapi.h"
#include "module.h"
#include "pdk.h"


SHAREMIND_STRINGMAP_DECLARE_insertHint(SharemindPdMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_insertHint(SharemindPdMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_emplaceAtHint(SharemindPdMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_emplaceAtHint(SharemindPdMap, static inline)
SHAREMIND_STRINGMAP_DECLARE_take(SharemindPdMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_take(SharemindPdMap,static inline)
SHAREMIND_STRINGMAP_DECLARE_remove(SharemindPdMap,static inline,)
SHAREMIND_STRINGMAP_DEFINE_remove(SharemindPdMap,
                                  static inline,
                                  SharemindPd,
                                  free,
                                  SharemindPd_destroy(value);)

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

    SharemindPdMap_detail * d;
    SharemindPd * pd;

    if (unlikely(!(d = malloc(sizeof(SharemindPdMap_detail))))) {
        SharemindPdk_setErrorOom(pdk);
        goto SharemindPd_new_fail_0;
    }

    if (unlikely(!(d->v.key = strdup(name)))) {
        SharemindPdk_setErrorOom(pdk);
        goto SharemindPd_new_fail_1;
    }

    pd = &d->v.value;

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(pd)) {
        SharemindPdk_setErrorMie(pdk);
        goto SharemindPd_new_fail_2;
    }

    SHAREMIND_LIBMODAPI_LASTERROR_INIT(pd);
    SHAREMIND_TAG_INIT(pd);

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

    SharemindModuleApi_lock(pdk->module->modapi);
    if (unlikely(SharemindModuleApi_findPd(pdk->module->modapi, name))) {
        SharemindPdk_setError(
                    pdk,
                    SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN,
                    "Protection domain with identical name already created!");
        goto SharemindPd_new_fail_5;
    }

    SharemindModule_lock(pdk->module);
    SharemindPdk_lock(pdk);
    {
        void * const hint = SharemindPdMap_insertHint(&pdk->pds, name);
        assert(hint);
        SharemindPdMap_emplaceAtHint(&pdk->pds, d, hint);
    }
    SharemindPdk_unlock(pdk);
    SharemindModule_unlock(pdk->module);
    SharemindModuleApi_unlock(pdk->module->modapi);
    return pd;

SharemindPd_new_fail_5:

    SharemindModuleApi_unlock(pdk->module->modapi);
    SharemindFacilityMap_destroy(&pd->pdpiFacilityMap);
    SharemindFacilityMap_destroy(&pd->facilityMap);
    free(pd->conf);

SharemindPd_new_fail_4:

    free(pd->name);

SharemindPd_new_fail_3:

    SHAREMIND_RECURSIVE_LOCK_DEINIT(pd);

SharemindPd_new_fail_2:

    free(d->v.key);

SharemindPd_new_fail_1:

    free(d);

SharemindPd_new_fail_0:

    return NULL;
}

void SharemindPd_destroy(SharemindPd * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->pdk);

    if (pd->isStarted)
        SharemindPd_stop(pd);

    SHAREMIND_TAG_DESTROY(pd);

    SHAREMIND_REFS_ASSERT_IF_REFERENCED(pd);
    SHAREMIND_NAMED_REFS_ASSERT_IF_REFERENCED(pd,startedRefs);

    free(pd->conf);
    free(pd->name);

    SharemindFacilityMap_destroy(&pd->facilityMap);
    SharemindFacilityMap_destroy(&pd->pdpiFacilityMap);
    SHAREMIND_RECURSIVE_LOCK_DEINIT(pd);
}

void SharemindPd_free(SharemindPd * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->pdk);
    SharemindPdMap_remove(&pd->pdk->pds, pd->name);
}

SHAREMIND_LIBMODAPI_LASTERROR_FUNCTIONS_DEFINE(SharemindPd)

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
    return pd->pdk; // No locking: const after SharemindPdk_newPd.
}

SharemindModule * SharemindPd_module(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    return pd->pdk->module; // No locking: const after SharemindPdk_newPd.
}

SharemindModuleApi * SharemindPd_moduleApi(const SharemindPd * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);
    // No locking: const after SharemindPdk_newPd:
    return pd->pdk->module->modapi;
}

const char * SharemindPd_name(const SharemindPd * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->name[0]);
    return pd->name; // No locking: const after SharemindPdk_newPd.
}

const char * SharemindPd_conf(const SharemindPd * pd) {
    assert(pd);
    assert(pd->conf);
    assert(pd->conf[0]);
    return pd->conf; // No locking: const after SharemindPdk_newPd.
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

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindPd,)

SHAREMIND_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPd)
SHAREMIND_NAMED_REFS_DEFINE_FUNCTIONS_WITH_RECURSIVE_MUTEX(SharemindPd,
                                                           startedRefs)
