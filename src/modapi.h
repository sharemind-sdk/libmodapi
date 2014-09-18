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


#include <sharemind/mutex.h>
#include <sharemind/refs.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct SharemindModuleApi_ {

    SharemindMutex mutex;

    SHAREMIND_LASTERROR_DECLARE_FIELDS

    /** Module facility name to pointer mapping: */
    SharemindFacilityMap moduleFacilityMap;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    SHAREMIND_REFS_DECLARE_FIELDS
};

SHAREMIND_LASTERROR_DECLARE_PRIVATE_FUNCTIONS(SharemindModuleApi)
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindModuleApi)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_MODAPI_H */

