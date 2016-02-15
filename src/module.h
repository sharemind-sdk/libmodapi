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

#ifndef SHAREMIND_LIBMODAPI_MODULE_H
#define SHAREMIND_LIBMODAPI_MODULE_H

#ifndef SHAREMIND_INTERNAL_
#error SHAREMIND_INTERNAL_
#endif

#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/refs.h>
#include <sharemind/tag.h>
#include <stdint.h>
#include "apis.h"
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"


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
SHAREMIND_LIBMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindModule)

#ifndef NDEBUG
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindModule)
#endif

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_MODULE_H */

