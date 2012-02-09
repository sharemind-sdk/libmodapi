/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMVM_PDPI_H
#define SHAREMIND_LIBSMVM_PDPI_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/refs.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif

struct _SMVM_PDPI {

    /** A handle for protection domain runtime data. */
    void * pdProcessHandle;

    /** Pointer to the protection domain kind. */
    SMVM_PD * pd;

    bool isStarted;

    /** Pointer to the PDPI facility context. */
    void * facilityContext;

    /** PDPI facility name to pointer mapping: */
    SMVM_FacilityMap pdpiFacilityMap;

    SMVM_REFS_DECLARE_FIELDS
    SMVM_NAMED_REFS_DECLARE_FIELDS(startedRefs)

};

SMVM_REFS_DECLARE_FUNCTIONS(SMVM_PDPI)
SMVM_NAMED_REFS_DECLARE_FUNCTIONS(SMVM_PDPI,startedRefs)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMVM_PDPI_H */
