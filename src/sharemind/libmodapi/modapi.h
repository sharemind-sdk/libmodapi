/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_MODAPI_H
#define SHAREMIND_LIBMODAPI_MODAPI_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/refs.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct _SMVM_MODAPI {
    SMVM_MODAPI_Error lastError;
    const char * lastErrorStaticString;
    char * lastErrorDynamicString;

    /** Module facility name to pointer mapping: */
    SMVM_FacilityMap moduleFacilityMap;

    /** PD facility name to pointer mapping: */
    SMVM_FacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SMVM_FacilityMap pdpiFacilityMap;

    SMVM_REFS_DECLARE_FIELDS
};

void SMVM_MODAPI_setErrorWithStaticString(SMVM_MODAPI * modapi,
                                          SMVM_MODAPI_Error error,
                                          const char * errorString) __attribute__ ((nonnull(1)));

#define OOM(modapi) if (1) { SMVM_MODAPI_setErrorWithStaticString((modapi), SMVM_MODAPI_OUT_OF_MEMORY, "Out of memory!"); } else (void) 0
#define OOR(modapi) if (1) { SMVM_MODAPI_setErrorWithStaticString((modapi), SMVM_MODAPI_REFERENCE_OVERFLOW, "Too many references!"); } else (void) 0

bool SMVM_MODAPI_setErrorWithDynamicString(SMVM_MODAPI * modapi,
                                           SMVM_MODAPI_Error error,
                                           const char * errorString) __attribute__ ((nonnull(1)));

SMVM_REFS_DECLARE_FUNCTIONS(SMVM_MODAPI)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_MODAPI_H */

