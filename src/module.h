/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_MODULE_H
#define SHAREMIND_LIBMODAPI_MODULE_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/refs.h>
#include <stdint.h>
#include "apis.h"
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"
#include "tag.h"


SHAREMIND_EXTERN_C_BEGIN


struct SharemindModule_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBMODAPI_LASTERROR_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    void * libHandle;
    char * filename;
    char * name;
    char * conf;
    uint32_t apiVersion;
    uint32_t version;
    const SharemindApi * api;

    void * apiData;
    void * moduleHandle;

    bool isInitialized;
    SharemindModuleApi * modapi;

    /** Module facility name to pointer mapping: */
    SharemindFacilityMap facilityMap;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    #ifndef NDEBUG
    SHAREMIND_REFS_DECLARE_FIELDS
    #endif

};

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindModule,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LIBMODAPI_LASTERROR_PRIVATE_FUNCTIONS(SharemindModule)

#ifndef NDEBUG
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindModule)
#endif

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_MODULE_H */

