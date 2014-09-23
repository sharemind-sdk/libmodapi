/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_PDK_H
#define SHAREMIND_LIBMODAPI_PDK_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/locks.h>
#include <sharemind/map.h>
#include <stdbool.h>
#include "facilitymap.h"
#include "lasterror.h"
#include "libmodapi.h"
#include "tag.h"


#ifdef __cplusplus
extern "C" {
#endif

SHAREMIND_STRINGMAP_DECLARE(SharemindPdMap,SharemindPd,)

struct SharemindPdk_ {

    SHAREMIND_LOCK_DECLARE_FIELDS;
    SHAREMIND_LASTERROR_DECLARE_FIELDS;
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

SHAREMIND_LOCK_FUNCTIONS_DECLARE(SharemindPdk);
SHAREMIND_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(SharemindPdk);


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_PDK_H */
