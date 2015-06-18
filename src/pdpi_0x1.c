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

#define SHAREMIND_INTERNAL__

#include "pdpi_0x1.h"

#include <assert.h>
#include <sharemind/abort.h>
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

    SharemindModuleApi0x1Error const r =
            (*((SharemindModuleApi0x1PdpiStartup)
               pd->pdk->pdpi_startup_impl_or_wrapper))(&pdpiWrapper);
    switch(r) {
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
        case SHAREMIND_MODULE_API_0x1_INVALID_CALL:
        case SHAREMIND_MODULE_API_0x1_INVALID_PD_CONFIGURATION:
        case SHAREMIND_MODULE_API_0x1_INVALID_MODULE_CONFIGURATION:
            SharemindPdpi_setError(pdpi,
                                   SHAREMIND_MODULE_API_API_ERROR,
                                   "Module returned an unexpected error!");
            return SHAREMIND_MODULE_API_API_ERROR;
        #ifndef __clang__
        default: SHAREMIND_ABORT("lMA P+i1 %d", (int) r);
        #endif
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
