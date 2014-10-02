/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_PDPI_H
#define SHAREMIND_LIBMODAPI_PDPI_H

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

struct SharemindPdpi_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBMODAPI_LASTERROR_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    /** A handle for protection domain runtime data. */
    void * pdProcessHandle;

    /** Pointer to the protection domain kind. */
    SharemindPd * pd;

    bool isStarted;

    /** Pointer to the PDPI facility context. */
    void * facilityContext;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap facilityMap;

    SHAREMIND_REFS_DECLARE_FIELDS
    SHAREMIND_NAMED_REFS_DECLARE_FIELDS(startedRefs)

};

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindPdpi,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LIBMODAPI_LASTERROR_PRIVATE_FUNCTIONS(SharemindPdpi)
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindPdpi)
SHAREMIND_NAMED_REFS_DECLARE_FUNCTIONS(SharemindPdpi,startedRefs)

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_PDPI_H */
