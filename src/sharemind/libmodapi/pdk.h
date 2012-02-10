/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSHAREMIND_PDK_H
#define SHAREMIND_LIBSHAREMIND_PDK_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/refs.h>
#include "facilitymap.h"
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct SharemindPdk_ {

    /**
      The index of the protection domain kind in the
      SHAREMIND_MODAPI_0x1_PDK_Definitions list of the module.
    */
    size_t pdk_index;

    /** Protection domain kind name. */
    char * name;

    /** Pointer to implementation function if current API, otherwise to a wrapper function. */
    void * pd_startup_impl_or_wrapper;

    /** NULL if current API, otherwise pointer to implementation function. */
    void * pd_startup_null_or_impl;

    /** Pointer to implementation function if current API, otherwise to a wrapper function. */
    void * pd_shutdown_impl_or_wrapper;

    /** NULL if current API, otherwise pointer to implementation function. */
    void * pd_shutdown_null_or_impl;

    /** Pointer to implementation function if current API, otherwise to a wrapper function. */
    void * pdpi_startup_impl_or_wrapper;

    /** NULL if current API, otherwise pointer to implementation function. */
    void * pdpi_startup_null_or_impl;

    /** Pointer to implementation function if current API, otherwise to a wrapper function. */
    void * pdpi_shutdown_impl_or_wrapper;

    /** NULL if current API, otherwise pointer to implementation function. */
    void * pdpi_shutdown_null_or_impl;

    /** Pointer to the module which provides this protection domain kind. */
    SharemindModule * module;

    /** PD facility name to pointer mapping: */
    SharemindFacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SharemindFacilityMap pdpiFacilityMap;

    SHAREMIND_REFS_DECLARE_FIELDS

};

int SharemindPdk_init(SharemindPdk * pdk,
                  size_t pdk_index,
                  const char * name,
                  void * pd_startup_impl,
                  void * pd_startup_wrapper,
                  void * pd_shutdown_impl,
                  void * pd_shutdown_wrapper,
                  void * pdpi_startup_impl,
                  void * pdpi_startup_wrapper,
                  void * pdpi_shutdown_impl,
                  void * pdpi_shutdown_wrapper,
                  SharemindModule * module) __attribute__ ((nonnull(1, 3, 4, 6, 8, 10, 12)));
void SharemindPdk_destroy(SharemindPdk * pdk) __attribute__ ((nonnull(1)));

SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindPdk)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSHAREMIND_PDK_H */
