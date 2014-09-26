/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "pd_0x1.h"

#include <assert.h>
#include <stdio.h>
#include "modapi.h"
#include "module.h"
#include "pd.h"
#include "pdk.h"

static SHAREMIND_ENUM_DECLARE_TOSTRING_CUSTOMNAME(
        pdStartupErrorToString,
        SharemindModuleApi0x1Error);
static SHAREMIND_ENUM_CUSTOM_DEFINE_CUSTOM_TOSTRING_CUSTOMNAME(
        pdStartupErrorToString,
        SharemindModuleApi0x1Error,
        SHAREMIND_MODULE_API_0x1_ERROR_ENUM,
        "PD startup failed with code ",
        " from the module!")

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
    switch ((*((PdStartup) pdk->pd_startup_impl_or_wrapper))(&pdWrapper)) {
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
        default:
            SharemindPd_setError(pd,
                                 SHAREMIND_MODULE_API_API_ERROR,
                                 "Module returned an unexpected error!");
            return SHAREMIND_MODULE_API_API_ERROR;
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
