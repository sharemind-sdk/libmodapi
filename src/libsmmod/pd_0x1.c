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
#include "module.h"
#include "pd.h"
#include "pdk.h"


static void * SMVM_PD_get_facility_wrapper(SMVM_MODAPI_0x1_PD_Wrapper * w, const char * name) {
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SMVM_PD_get_facility((SMVM_PD *) w->internal, name);
}

static inline void SMVM_PD_init_start_stop_wrappers(SMVM_PD * pd,
                                                    SMVM_MODAPI_0x1_PD_Conf * pdConf,
                                                    SMVM_MODAPI_0x1_PD_Wrapper * pdWrapper)
{
    assert(pd);
    assert(pd->pdk);
    const SMVM_PDK * const pdk = pd->pdk;

    pdConf->pd_name = pd->name;
    pdConf->pdk_index = pdk->pdk_index;
    pdConf->pd_conf_string = pd->conf;

    pdWrapper->pdHandle = pd->pdHandle;
    pdWrapper->moduleHandle = pdk->module->moduleHandle;
    assert(pdWrapper->moduleHandle);
    pdWrapper->conf = pdConf;
    pdWrapper->getPdFacility = &SMVM_PD_get_facility_wrapper;
}

bool SMVM_PD_start_0x1(SMVM_PD * pd) {
    assert(pd);
    assert(!pd->isStarted);

    SMVM_MODAPI_0x1_PD_Conf pdConf;
    SMVM_MODAPI_0x1_PD_Wrapper pdWrapper;
    SMVM_PD_init_start_stop_wrappers(pd, &pdConf, &pdWrapper);
    const SMVM_PDK * const pdk = pd->pdk;
    pdWrapper.internal = pd;

    const int r = (*((SMVM_MODAPI_0x1_PD_Startup) pdk->pd_startup_impl_or_wrapper))(&pdWrapper);
    if (likely(r == 0)) {
        pd->pdHandle = pdWrapper.pdHandle;
        pd->isStarted = true;
        return true;
    }

    const char * const errorFormatString = "PD startup failed with code %d from the module!";
    const size_t len = strlen(errorFormatString) + sizeof(int) * 3; /* -"%d" + '\0' + '-' + 3 for each byte of int */
    char * const errorString = (char *) malloc(len);
    if (likely(errorString))
        snprintf(errorString, len, errorFormatString, r);
    SMVM_MODAPI_setErrorWithDynamicString(pdk->module->modapi, SMVM_MODAPI_PD_STARTUP_FAILED, errorString);
    return false;
}

void SMVM_PD_stop_0x1(SMVM_PD * pd) {
    assert(pd);
    assert(pd->isStarted);
    assert(pd->pdk);
    assert(pd->pdk->pd_shutdown_impl_or_wrapper);


    SMVM_MODAPI_0x1_PD_Conf pdConf;
    SMVM_MODAPI_0x1_PD_Wrapper pdWrapper;
    SMVM_PD_init_start_stop_wrappers(pd, &pdConf, &pdWrapper);
    const SMVM_PDK * const pdk = pd->pdk;
    pdWrapper.internal = pd;

    (*((SMVM_MODAPI_0x1_PD_Shutdown) pdk->pd_shutdown_impl_or_wrapper))(&pdWrapper);
}
