/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "pdpi_0x1.h"

#include <assert.h>
#include <stdio.h>
#include "modapi.h"
#include "module.h"
#include "pd.h"
#include "pdk.h"
#include "pdpi.h"


static const SharemindFacility * SharemindPdpi_facilityWrapper(
        SharemindModuleApi0x1PdpiWrapper * w,
        const char * name)
{
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SharemindPdpi_facility((SharemindPdpi *) w->internal, name);
}

SharemindModuleApiError SharemindPdpi_start_0x1(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->module);
    assert(pdpi->pd->pdk->module->modapi);
    assert(pdpi->pd->pdk->pdpi_startup_impl_or_wrapper);

    const SharemindPd * const pd = pdpi->pd;
    SharemindModuleApi0x1PdpiWrapper pdpiWrapper = {
        .pdProcessHandle = NULL, /* Just in case */
        .pdHandle = pd->pdHandle,
        .getPdpiFacility = &SharemindPdpi_facilityWrapper,
        .internal = pdpi
    };

    switch((*((SharemindModuleApi0x1PdpiStartup)
                  pd->pdk->pdpi_startup_impl_or_wrapper))(&pdpiWrapper))
    {
        case SHAREMIND_MODULE_API_0x1_OK:
            pdpi->pdProcessHandle = pdpiWrapper.pdProcessHandle;
            return SHAREMIND_MODULE_API_OK;
        #define SHAREMIND_EC(theirs,ours) \
            case SHAREMIND_MODULE_API_0x1_ ## theirs: \
                SharemindPdpi_setError(pdpi, \
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
        #undef SHAREMIND_EC2
        #undef SHAREMIND_EC
        default:
            SharemindPdpi_setError(pdpi,
                                   SHAREMIND_MODULE_API_API_ERROR,
                                   "Module returned an unexpected error!");
            return SHAREMIND_MODULE_API_API_ERROR;
    }
}

void SharemindPdpi_stop_0x1(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->pdpi_shutdown_impl_or_wrapper);

    const SharemindPd * const pd = pdpi->pd;
    SharemindModuleApi0x1PdpiWrapper pdpiWrapper = {
        .pdProcessHandle = pdpi->pdProcessHandle,
        .pdHandle = pd->pdHandle,
        .getPdpiFacility = &SharemindPdpi_facilityWrapper,
        .internal = pdpi
    };
    typedef SharemindModuleApi0x1PdpiShutdown PdpiShutdown;
    (*((PdpiShutdown) pd->pdk->pdpi_shutdown_impl_or_wrapper))(&pdpiWrapper);
}
