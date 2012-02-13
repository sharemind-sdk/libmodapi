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


static const SharemindFacility * SHAREMIND_PDPI_get_facility_wrapper(SharemindModuleApi0x1PdpiWrapper * w, const char * name) {
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SharemindPdpi_get_facility((SharemindPdpi *) w->internal, name);
}

bool SHAREMIND_PDPI_start_0x1(SharemindPdpi * pdpi) {
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
        .getPdpiFacility = &SHAREMIND_PDPI_get_facility_wrapper,
        .internal = pdpi
    };

    const SharemindPdk * const pdk = pd->pdk;
    const int r = (*((SharemindModuleApi0x1PdpiStartup) pdk->pdpi_startup_impl_or_wrapper))(&pdpiWrapper);
    if (likely(r == 0)) {
        pdpi->pdProcessHandle = pdpiWrapper.pdProcessHandle;
        return true;
    }

    const char * const errorFormatString = "PDPI startup failed with code %d from the module!";
    const size_t len = strlen(errorFormatString) + sizeof(int) * 3; /* -"%d" + '\0' + '-' + 3 for each byte of int */
    char * const errorString = (char *) malloc(len);
    if (likely(errorString))
        snprintf(errorString, len, errorFormatString, r);
    SharemindModuleApi_set_error_with_dynamic_string(pdk->module->modapi, SHAREMIND_MODAPI_PDPI_STARTUP_FAILED, errorString);
    return false;
}

void SHAREMIND_PDPI_stop_0x1(SharemindPdpi * pdpi) {
    assert(pdpi);
    assert(pdpi->pd);
    assert(pdpi->pd->pdk);
    assert(pdpi->pd->pdk->pdpi_shutdown_impl_or_wrapper);

    const SharemindPd * const pd = pdpi->pd;
    SharemindModuleApi0x1PdpiWrapper pdpiWrapper = {
        .pdProcessHandle = pdpi->pdProcessHandle,
        .pdHandle = pd->pdHandle,
        .getPdpiFacility = &SHAREMIND_PDPI_get_facility_wrapper,
        .internal = pdpi
    };
    (*((SharemindModuleApi0x1PdpiShutdown) pd->pdk->pdpi_shutdown_impl_or_wrapper))(&pdpiWrapper);
}
