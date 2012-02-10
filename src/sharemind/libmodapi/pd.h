/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSHAREMIND_PD_H
#define SHAREMIND_LIBSHAREMIND_PD_H

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


struct SHAREMIND_PD_ {

    /** Protection domain name. */
    char * name;

    /** Protection domain configuration. */
    char * conf;

    /** Pointer to the PD facility context. */
    void * facilityContext;

    /** A handle for protection domain runtime data. */
    void * pdHandle;

    /** Pointer to the protection domain kind. */
    SHAREMIND_PDK * pdk;

    /** Whether this protection domain is started. */
    bool isStarted;

    /** PD facility name to pointer mapping: */
    SHAREMIND_FacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SHAREMIND_FacilityMap pdpiFacilityMap;

    SHAREMIND_REFS_DECLARE_FIELDS
    SHAREMIND_NAMED_REFS_DECLARE_FIELDS(startedRefs)

};

SHAREMIND_REFS_DECLARE_FUNCTIONS(SHAREMIND_PD)
SHAREMIND_NAMED_REFS_DECLARE_FUNCTIONS(SHAREMIND_PD,startedRefs)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSHAREMIND_PD_H */
