/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_PD_H
#define SHAREMIND_LIBMODAPI_PD_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/refs.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"
#include "tag.h"


SHAREMIND_EXTERN_C_BEGIN


struct SharemindPd_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LASTERROR_DECLARE_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    /** Protection domain name. */
    char * name;

    /** Protection domain configuration. */
    char * conf;

    /** Pointer to the PD facility context. */
    void * facilityContext;

    /** A handle for protection domain runtime data. */
    void * pdHandle;

    /** Pointer to the protection domain kind. */
    SharemindPdk * pdk;

    /** Whether this protection domain is started. */
    bool isStarted;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap facilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    SHAREMIND_REFS_DECLARE_FIELDS
    SHAREMIND_NAMED_REFS_DECLARE_FIELDS(startedRefs)

};

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindPd,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindPd);
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindPd)
SHAREMIND_NAMED_REFS_DECLARE_FUNCTIONS(SharemindPd,startedRefs)


SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_PD_H */
