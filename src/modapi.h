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

#ifndef SHAREMIND_LIBMODAPI_MODAPI_H
#define SHAREMIND_LIBMODAPI_MODAPI_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/set.h>
#include <sharemind/tag.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"


SHAREMIND_EXTERN_C_BEGIN

SHAREMIND_SET_DECLARE_DEFINE(SharemindModulesSet,
                             SharemindModule *,
                             ((uintptr_t) key),
                             SHAREMIND_SET_KEY_EQUALS_voidptr,
                             SHAREMIND_SET_KEY_LESS_THAN_voidptr,
                             SHAREMIND_SET_KEYCOPY_REGULAR,
                             SHAREMIND_SET_KEYFREE_REGULAR,
                             malloc,
                             free,
                             inline,
                             SHAREMIND_COMMA visibility("internal"))

struct SharemindModuleApi_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBMODAPI_LASTERROR_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    SharemindModulesSet modules;

    /** Module facility name to pointer mapping: */
    SharemindFacilityMap moduleFacilityMap;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    SharemindModuleApiContext * context;

};

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindModuleApi,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LIBMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindModuleApi)


SHAREMIND_EXTERN_C_BEGIN

#endif /* SHAREMIND_LIBMODAPI_MODAPI_H */

