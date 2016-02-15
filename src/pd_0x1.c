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

#include "pd_0x1.h"

#include <assert.h>
#include <sharemind/abort.h>
#include <stdio.h>
#include "modapi.h"
#include "module.h"
#include "pd.h"
#include "pdk.h"


static const SharemindFacility * SharemindPd_facilityWrapper(
        SharemindModuleApi0x1PdWrapper * w,
        const char * name)
{
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SharemindPd_facility((SharemindPd *) w->internal, name);
}

static inline void SharemindPd_initStartStopWrappers(
        SharemindPd * pd,
        SharemindModuleApi0x1PdConf * pdConf,
        SharemindModuleApi0x1PdWrapper * pdWrapper)
{
    assert(pd);
    assert(pd->pdk);
    const SharemindPdk * const pdk = pd->pdk;

    pdConf->pd_name = pd->name;
    pdConf->pdk_index = pdk->pdk_index;
    pdConf->pd_conf_string = pd->conf;

    pdWrapper->pdHandle = pd->pdHandle;
    pdWrapper->moduleHandle = pdk->module->moduleHandle;
    assert(pdWrapper->moduleHandle);
    pdWrapper->conf = pdConf;
    pdWrapper->getPdFacility = &SharemindPd_facilityWrapper;
}

SharemindModuleApiError SharemindPd_start_0x1(SharemindPd * pd) {
    assert(pd);
    assert(!pd->isStarted);

    SharemindModuleApi0x1PdConf pdConf;
    SharemindModuleApi0x1PdWrapper pdWrapper;
    SharemindPd_initStartStopWrappers(pd, &pdConf, &pdWrapper);
    const SharemindPdk * const pdk = pd->pdk;
    pdWrapper.internal = pd;

    typedef SharemindModuleApi0x1PdStartup PdStartup;
    SharemindModuleApi0x1Error const r =
            (*((PdStartup) pdk->pd_startup_impl_or_wrapper))(&pdWrapper);
    switch (r) {
        case SHAREMIND_MODULE_API_0x1_OK:
            pd->pdHandle = pdWrapper.pdHandle;
            return SHAREMIND_MODULE_API_OK;
        #define SHAREMIND_EC(theirs,ours) \
            case SHAREMIND_MODULE_API_0x1_ ## theirs: \
                SharemindPd_setError(pd, \
                                     SHAREMIND_MODULE_API_ ## ours, \
                                     "Module returned " #theirs "!"); \
                return SHAREMIND_MODULE_API_ ## ours
        #define SHAREMIND_EC2(e) SHAREMIND_EC(e,e)
        SHAREMIND_EC2(OUT_OF_MEMORY);
        SHAREMIND_EC2(IMPLEMENTATION_LIMITS_REACHED);
        SHAREMIND_EC2(SHAREMIND_ERROR);
        SHAREMIND_EC2(MODULE_ERROR);
        SHAREMIND_EC(GENERAL_ERROR, MODULE_ERROR);
        SHAREMIND_EC(MISSING_FACILITY, MODULE_ERROR);
        SHAREMIND_EC(INVALID_PD_CONFIGURATION, MODULE_ERROR);
        #undef SHAREMIND_EC2
        #undef SHAREMIND_EC
        case SHAREMIND_MODULE_API_0x1_INVALID_CALL:
        case SHAREMIND_MODULE_API_0x1_INVALID_MODULE_CONFIGURATION:
            SharemindPd_setError(pd,
                                 SHAREMIND_MODULE_API_API_ERROR,
                                 "Module returned an unexpected error!");
            return SHAREMIND_MODULE_API_API_ERROR;
        #ifdef __clang__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcovered-switch-default"
        #endif
        default: SHAREMIND_ABORT("lMA Pi1 %d", (int) r);
        #ifdef __clang__
        #pragma GCC diagnostic pop
        #endif
    }
}

void SharemindPd_stop_0x1(SharemindPd * pd) {
    assert(pd);
    assert(pd->isStarted);
    assert(pd->pdk);
    assert(pd->pdk->pd_shutdown_impl_or_wrapper);

    SharemindModuleApi0x1PdConf pdConf;
    SharemindModuleApi0x1PdWrapper pdWrapper;
    SharemindPd_initStartStopWrappers(pd, &pdConf, &pdWrapper);
    const SharemindPdk * const pdk = pd->pdk;
    pdWrapper.internal = pd;

    (*((SharemindModuleApi0x1PdShutdown) pdk->pd_shutdown_impl_or_wrapper))(
                &pdWrapper);
}
