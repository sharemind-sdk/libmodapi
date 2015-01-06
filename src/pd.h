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

#ifndef SHAREMIND_LIBMODAPI_PD_H
#define SHAREMIND_LIBMODAPI_PD_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/refs.h>
#include <sharemind/tag.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"


SHAREMIND_EXTERN_C_BEGIN


struct SharemindPd_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBMODAPI_LASTERROR_FIELDS;
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
SHAREMIND_LIBMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindPd)
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindPd)
SHAREMIND_NAMED_REFS_DECLARE_FUNCTIONS(SharemindPd,startedRefs)

void SharemindPd_destroy(SharemindPd * pd) __attribute__((nonnull(1)));

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_PD_H */
