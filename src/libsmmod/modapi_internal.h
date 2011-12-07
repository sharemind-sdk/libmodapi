/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMMOD_MODAPI_INTERNAL_H
#define SHAREMIND_LIBSMMOD_MODAPI_INTERNAL_H

#ifndef _SHAREMIND_INTERNAL
#error _SHAREMIND_INTERNAL
#endif

#include <stdbool.h>
#include <stdint.h>
#include "modapi.h"


#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  SMVM_MODAPI
*******************************************************************************/

struct _SMVM_MODAPI {
    SMVM_MODAPI_Error lastError;
    const char * lastErrorStaticString;
    char * lastErrorDynamicString;
};

void SMVM_MODAPI_setErrorWithStaticString(SMVM_MODAPI * modapi,
                                          SMVM_MODAPI_Error error,
                                          const char * errorString) __attribute__ ((nonnull(1)));

#define OOM(modapi) if (1) { SMVM_MODAPI_setErrorWithStaticString((modapi), SMVM_MODAPI_OUT_OF_MEMORY, "Out of memory!"); } else (void) 0

bool SMVM_MODAPI_setErrorWithDynamicString(SMVM_MODAPI * modapi,
                                           SMVM_MODAPI_Error error,
                                           const char * errorString) __attribute__ ((nonnull(1)));


/*******************************************************************************
  SMVM_Module
*******************************************************************************/

struct _SMVM_Module {
    void * handle;
    char * filename;
    char * name;
    uint32_t apiVersion;
    uint32_t version;

    void * apiData;
    void * moduleHandle;

    bool isInitialized;
    SMVM_MODAPI * modapi;
};

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMMOD_MODAPI_INTERNAL_H */
