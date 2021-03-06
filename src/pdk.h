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

#ifndef SHAREMIND_LIBMODAPI_PDK_H
#define SHAREMIND_LIBMODAPI_PDK_H

#ifndef SHAREMIND_INTERNAL_
#error including an internal header!
#endif

#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/map.h>
#include <sharemind/tag.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"
#include "pd.h"


SHAREMIND_EXTERN_C_BEGIN

SHAREMIND_STRINGMAP_DECLARE_BODY(SharemindPdMap, SharemindPd)
SHAREMIND_STRINGMAP_DECLARE_strHash(SharemindPdMap,
                                    inline,
                                    SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_strHash(SharemindPdMap,inline)

struct SharemindPdk_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;
    SHAREMIND_LIBMODAPI_LASTERROR_FIELDS;
    SHAREMIND_TAG_DECLARE_FIELDS;

    /**
      The index of the protection domain kind in the
      SharemindModuleApi0x1PdkDefinitions list of the module.
    */
    size_t pdk_index;

    /** Protection domain kind name. */
    char * name;

    /**
      Pointer to implementation function if current API, otherwise to a wrapper
      function.
    */
    void (* pd_startup_impl_or_wrapper)(void);

    /** NULL if current API, otherwise pointer to implementation function. */
    void (* pd_startup_null_or_impl)(void);

    /**
      Pointer to implementation function if current API, otherwise to a wrapper
      function.
    */
    void (* pd_shutdown_impl_or_wrapper)(void);

    /** NULL if current API, otherwise pointer to implementation function. */
    void (* pd_shutdown_null_or_impl)(void);

    /**
      Pointer to implementation function if current API, otherwise to a wrapper
      function.
    */
    void (* pdpi_startup_impl_or_wrapper)(void);

    /** NULL if current API, otherwise pointer to implementation function. */
    void (* pdpi_startup_null_or_impl)(void);

    /**
      Pointer to implementation function if current API, otherwise to a wrapper
      function.
    */
    void (* pdpi_shutdown_impl_or_wrapper)(void);

    /** NULL if current API, otherwise pointer to implementation function. */
    void (* pdpi_shutdown_null_or_impl)(void);

    /** Pointer to the module which provides this protection domain kind. */
    SharemindModule * module;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    /** PD map. */
    SharemindPdMap pds;

};


bool SharemindPdk_init(SharemindPdk * pdk,
                       size_t pdk_index,
                       const char * name,
                       void (* pd_startup_impl)(void),
                       void (* pd_startup_wrapper)(void),
                       void (* pd_shutdown_impl)(void),
                       void (* pd_shutdown_wrapper)(void),
                       void (* pd_process_startup_impl)(void),
                       void (* pd_process_startup_wrapper)(void),
                       void (* pd_process_shutdown_impl)(void),
                       void (* pd_process_shutdown_wrapper)(void),
                       SharemindModule * module)
        __attribute__ ((nonnull(1, 3, 4, 6, 8, 10, 12)));

void SharemindPdk_destroy(SharemindPdk * pdk) __attribute__ ((nonnull(1)));

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindPdk,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_LIBMODAPI_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindPdk)

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_PDK_H */
