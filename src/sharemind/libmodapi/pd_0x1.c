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

static const SharemindFacility * SHAREMIND_PD_get_facility_wrapper(
        SharemindModuleApi0x1PdWrapper * w,
        const char * name)
{
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SharemindPd_get_facility((SharemindPd *) w->internal, name);
}

static inline void SHAREMIND_PD_init_start_stop_wrappers(
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
    pdWrapper->getPdFacility = &SHAREMIND_PD_get_facility_wrapper;
}

bool SharemindPd_start_0x1(SharemindPd * pd) {
    assert(pd);
    assert(!pd->isStarted);

    SharemindModuleApi0x1PdConf pdConf;
    SharemindModuleApi0x1PdWrapper pdWrapper;
    SHAREMIND_PD_init_start_stop_wrappers(pd, &pdConf, &pdWrapper);
    const SharemindPdk * const pdk = pd->pdk;
    pdWrapper.internal = pd;

    typedef SharemindModuleApi0x1PdStartup PdStartup;
    const SharemindModuleApi0x1Error r =
            (*((PdStartup) pdk->pd_startup_impl_or_wrapper))(&pdWrapper);
    if (likely(r == SHAREMIND_MODULE_API_0x1_OK)) {
        pd->pdHandle = pdWrapper.pdHandle;
        pd->isStarted = true;
        return true;
    }

    SharemindModuleApi_set_error_with_static_string(
                pdk->module->modapi,
                SHAREMIND_MODULE_API_PD_STARTUP_FAILED,
                pdStartupErrorToString(r));
    return false;
}

void SharemindPd_stop_0x1(SharemindPd * pd) {
    assert(pd);
    assert(pd->isStarted);
    assert(pd->pdk);
    assert(pd->pdk->pd_shutdown_impl_or_wrapper);

    SharemindModuleApi0x1PdConf pdConf;
    SharemindModuleApi0x1PdWrapper pdWrapper;
    SHAREMIND_PD_init_start_stop_wrappers(pd, &pdConf, &pdWrapper);
    const SharemindPdk * const pdk = pd->pdk;
    pdWrapper.internal = pd;

    (*((SharemindModuleApi0x1PdShutdown) pdk->pd_shutdown_impl_or_wrapper))(
                &pdWrapper);
}
