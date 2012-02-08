/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMVM_PDK_H
#define SHAREMIND_LIBSMVM_PDK_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/refs.h>
#include "facilitymap.h"
#include "libsmmod.h"


#ifdef __cplusplus
extern "C" {
#endif


struct _SMVM_PDK {

    /**
      The index of the protection domain kind in the
      SMVM_MODAPI_0x1_PDK_Definitions list of the module.
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
    SMVM_Module * module;

    /** PD facility name to pointer mapping: */
    SMVM_FacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SMVM_FacilityMap pdpiFacilityMap;

    SMVM_REFS_DECLARE_FIELDS

};

int SMVM_PDK_init(SMVM_PDK * pdk,
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
                  SMVM_Module * module) __attribute__ ((nonnull(1, 3, 4, 6, 8, 10, 12)));
void SMVM_PDK_destroy(SMVM_PDK * pdk) __attribute__ ((nonnull(1)));

SMVM_REFS_DECLARE_FUNCTIONS(SMVM_PDK)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMVM_PDK_H */
