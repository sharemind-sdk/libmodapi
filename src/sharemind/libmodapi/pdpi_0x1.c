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


static SHAREMIND_ENUM_DECLARE_TOSTRING_CUSTOMNAME(
        pdpiStartupErrorToString,
        SharemindModuleApi0x1Error);
static SHAREMIND_ENUM_CUSTOM_DEFINE_CUSTOM_TOSTRING_CUSTOMNAME(
        pdpiStartupErrorToString,
        SharemindModuleApi0x1Error,
        SHAREMIND_MODULE_API_0x1_ERROR_ENUM,
        "PDPI startup failed with code ",
        " from the module!")

static const SharemindFacility * SHAREMIND_PDPI_get_facility_wrapper(SharemindModuleApi0x1PdpiWrapper * w, const char * name) {
    assert(w);
    assert(w->internal);
    assert(name);
    assert(name[0]);
    return SharemindPdpi_get_facility((SharemindPdpi *) w->internal, name);
}

bool SharemindPdpi_start_0x1(SharemindPdpi * pdpi) {
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
    const SharemindModuleApi0x1Error r = (*((SharemindModuleApi0x1PdpiStartup) pdk->pdpi_startup_impl_or_wrapper))(&pdpiWrapper);
    if (likely(r == SHAREMIND_MODULE_API_0x1_OK)) {
        pdpi->pdProcessHandle = pdpiWrapper.pdProcessHandle;
        return true;
    }

    SharemindModuleApi_set_error_with_static_string(
                pdk->module->modapi,
                SHAREMIND_MODULE_API_PDPI_STARTUP_FAILED,
                pdpiStartupErrorToString(r));
    return false;
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
        .getPdpiFacility = &SHAREMIND_PDPI_get_facility_wrapper,
        .internal = pdpi
    };
    const SharemindModuleApi0x1Error r = (*((SharemindModuleApi0x1PdpiShutdown) pd->pdk->pdpi_shutdown_impl_or_wrapper))(&pdpiWrapper);
    if (r != SHAREMIND_MODULE_API_0x1_OK) {
        /** \todo log return status. */
    }
}
