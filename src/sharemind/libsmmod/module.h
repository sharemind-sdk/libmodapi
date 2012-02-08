/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMMOD_MODULE_H
#define SHAREMIND_LIBSMMOD_MODULE_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/refs.h>
#include <stdint.h>
#include "apis.h"
#include "facilitymap.h"
#include "libsmmod.h"


#ifdef __cplusplus
extern "C" {
#endif


struct _SMVM_Module {
    void * handle;
    char * filename;
    char * name;
    uint32_t apiVersion;
    uint32_t version;
    const SMVM_API * api;

    void * apiData;
    void * moduleHandle;

    bool isInitialized;
    SMVM_MODAPI * modapi;

    /** Module facility name to pointer mapping: */
    SMVM_FacilityMap moduleFacilityMap;

    /** PD facility name to pointer mapping: */
    SMVM_FacilityMap pdFacilityMap;

    /** PDPI facility name to pointer mapping: */
    SMVM_FacilityMap pdpiFacilityMap;


    SMVM_REFS_DECLARE_FIELDS
};

SMVM_REFS_DECLARE_FUNCTIONS(SMVM_Module)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMMOD_MODULE_H */

