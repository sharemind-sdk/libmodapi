/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMVM_PD_H
#define SHAREMIND_LIBSMVM_PD_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <stdbool.h>
#include "../refs.h"
#include "facilitymap.h"
#include "modapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct _SMVM_PD {

    /** Protection domain name. */
    char * name;

    /** Protection domain configuration. */
    char * conf;

    /** A handle for protection domain runtime data. */
    void * pdHandle;

    /** Pointer to the protection domain kind. */
    SMVM_PDK * pdk;

    /** Whether this protection domain is started. */
    bool isStarted;

    /** PD facility name to pointer mapping: */
    SMVM_FacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SMVM_FacilityMap pdpiFacilityMap;

    SMVM_REFS_DECLARE_FIELDS

};

SMVM_REFS_DECLARE_FUNCTIONS(SMVM_PD)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMVM_PD_H */
