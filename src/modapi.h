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


#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/set.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"
#include "tag.h"


SHAREMIND_EXTERN_C_BEGIN

SHAREMIND_SET_DECLARE(SharemindModulesSet, SharemindModule *,)

struct SharemindModuleApi_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LASTERROR_DECLARE_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    SharemindModulesSet modules;

    /** Module facility name to pointer mapping: */
    SharemindFacilityMap moduleFacilityMap;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

};

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE(SharemindModuleApi);
SHAREMIND_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindModuleApi);


SHAREMIND_EXTERN_C_BEGIN

#endif /* SHAREMIND_LIBMODAPI_MODAPI_H */

