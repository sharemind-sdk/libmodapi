/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMMOD_LIBSMMOD_H
#define SHAREMIND_LIBSMMOD_LIBSMMOD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../preprocessor.h"
#include "api_0x1.h"


#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Most recent API aliases
*******************************************************************************/

typedef SMVM_MODAPI_0x1_Syscall SMVM_SyscallCallable;
typedef SMVM_MODAPI_0x1_Syscall_Context SMVM_SyscallContext;


/*******************************************************************************
  Forward declarations
*******************************************************************************/

struct _SMVM_MODAPI;
typedef struct _SMVM_MODAPI SMVM_MODAPI;

struct _SMVM_Module;
typedef struct _SMVM_Module SMVM_Module;

struct _SMVM_Syscall;
typedef struct _SMVM_Syscall SMVM_Syscall;

struct _SMVM_PDK;
typedef struct _SMVM_PDK SMVM_PDK;

struct _SMVM_PD;
typedef struct _SMVM_PD SMVM_PD;

struct _SMVM_PDPI;
typedef struct _SMVM_PDPI SMVM_PDPI;

/*******************************************************************************
  SMVM_MODAPI
*******************************************************************************/

#define SMVM_MODAPI_API_VERSION     1u
#define SMVM_MODAPI_API_MIN_VERSION 1u

#define SMVM_ENUM_MODAPI_Error \
    ((SMVM_MODAPI_OK, = 0)) \
    ((SMVM_MODAPI_OUT_OF_MEMORY,)) \
    ((SMVM_MODAPI_UNABLE_TO_OPEN_MODULE,)) \
    ((SMVM_MODAPI_INVALID_MODULE,)) \
    ((SMVM_MODAPI_API_NOT_SUPPORTED,)) \
    ((SMVM_MODAPI_API_ERROR,)) \
    ((SMVM_MODAPI_DUPLICATE_SYSCALL,)) \
    ((SMVM_MODAPI_DUPLICATE_PROTECTION_DOMAIN,)) \
    ((SMVM_MODAPI_MODULE_ERROR,)) \
    ((SMVM_MODAPI_PD_STARTUP_FAILED,)) \
    ((SMVM_MODAPI_PDPI_STARTUP_FAILED,)) \
    ((SMVM_MODAPI_ERROR_COUNT,))
SM_ENUM_CUSTOM_DEFINE(SMVM_MODAPI_Error, SMVM_ENUM_MODAPI_Error);
SM_ENUM_DECLARE_TOSTRING(SMVM_MODAPI_Error);

SMVM_MODAPI * SMVM_MODAPI_new(void);
void SMVM_MODAPI_free(SMVM_MODAPI * modapi) __attribute__ ((nonnull(1)));
SMVM_MODAPI_Error SMVM_MODAPI_lastError(const SMVM_MODAPI * modapi) __attribute__ ((nonnull(1)));
const char * SMVM_MODAPI_lastErrorString(const SMVM_MODAPI * modapi) __attribute__ ((nonnull(1)));
void SMVM_MODAPI_clearError(SMVM_MODAPI * modapi) __attribute__ ((nonnull(1)));

void SMVM_MODAPI_set_facility_context(SMVM_MODAPI * modapi, void * facilityContext) __attribute__ ((nonnull(1)));
void * SMVM_MODAPI_get_facility_context(const SMVM_MODAPI * modapi) __attribute__ ((nonnull(1)));
int SMVM_MODAPI_set_module_facility(SMVM_MODAPI * modapi, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_MODAPI_get_module_facility(const SMVM_MODAPI * modapi, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_MODAPI_set_syscall_facility(SMVM_MODAPI * modapi, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_MODAPI_get_syscall_facility(const SMVM_MODAPI * modapi, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_MODAPI_set_pd_facility(SMVM_MODAPI * modapi, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_MODAPI_get_pd_facility(const SMVM_MODAPI * modapi, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_MODAPI_set_pdpi_facility(SMVM_MODAPI * modapi, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_MODAPI_get_pdpi_facility(const SMVM_MODAPI * modapi, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SMVM_Module
*******************************************************************************/

SMVM_Module * SMVM_Module_new(SMVM_MODAPI * modapi, const char * filename) __attribute__ ((nonnull(1, 2)));
void SMVM_Module_free(SMVM_Module * m) __attribute__ ((nonnull(1)));

SMVM_MODAPI_Error SMVM_Module_mod_init(SMVM_Module * m) __attribute__ ((nonnull(1)));
void SMVM_Module_mod_deinit(SMVM_Module * m) __attribute__ ((nonnull(1)));

const char * SMVM_Module_get_name(const SMVM_Module * m) __attribute__ ((nonnull(1)));
uint32_t SMVM_Module_get_api_version_in_use(const SMVM_Module * m) __attribute__ ((nonnull(1)));
void * SMVM_Module_get_handle(const SMVM_Module * m) __attribute__ ((nonnull(1)));

SMVM_MODAPI * SMVM_Module_get_modapi(const SMVM_Module * m) __attribute__ ((nonnull(1)));

size_t SMVM_Module_get_num_syscalls(const SMVM_Module * m) __attribute__ ((nonnull(1)));
SMVM_Syscall * SMVM_Module_get_syscall(const SMVM_Module * m, size_t index) __attribute__ ((nonnull(1)));
SMVM_Syscall * SMVM_Module_find_syscall(const SMVM_Module * m, const char * signature) __attribute__ ((nonnull(1, 2)));

size_t SMVM_Module_get_num_pdks(const SMVM_Module * m) __attribute__ ((nonnull(1)));
SMVM_PDK * SMVM_Module_get_pdk(const SMVM_Module * m, size_t index) __attribute__ ((nonnull(1)));
SMVM_PDK * SMVM_Module_find_pdk(const SMVM_Module * m, const char * name) __attribute__ ((nonnull(1, 2)));

void SMVM_Module_set_facility_context(SMVM_Module * m, void * facilityContext) __attribute__ ((nonnull(1)));
void * SMVM_Module_get_facility_context(const SMVM_Module * m) __attribute__ ((nonnull(1)));
int SMVM_Module_set_facility(SMVM_Module * m, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_Module_get_facility(const SMVM_Module * m, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_Module_set_syscall_facility(SMVM_Module * m, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_Module_get_syscall_facility(const SMVM_Module * m, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_Module_set_pd_facility(SMVM_Module * m, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_Module_get_pd_facility(const SMVM_Module * m, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_Module_set_pdpi_facility(SMVM_Module * m, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_Module_get_pdpi_facility(const SMVM_Module * m, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SMVM_Syscall
*******************************************************************************/

const char * SMVM_Syscall_get_name(const SMVM_Syscall * sc) __attribute__ ((nonnull(1)));
SMVM_Module * SMVM_Syscall_get_module(const SMVM_Syscall * sc) __attribute__ ((nonnull(1)));
SMVM_MODAPI * SMVM_Syscall_get_modapi(const SMVM_Syscall * sc) __attribute__ ((nonnull(1)));

typedef struct _SMVM_SyscallWrapper {
    SMVM_SyscallCallable callable;
    void * internal;
} SMVM_SyscallWrapper;

SMVM_SyscallWrapper SMVM_Syscall_get_wrapper(const SMVM_Syscall * sc) __attribute__ ((nonnull(1)));

void SMVM_Syscall_set_facility_context(SMVM_Syscall * sc, void * facilityContext) __attribute__ ((nonnull(1)));
void * SMVM_Syscall_get_facility_context(const SMVM_Syscall * sc) __attribute__ ((nonnull(1)));
int SMVM_Syscall_set_facility(SMVM_Syscall * sc, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_Syscall_get_facility(const SMVM_Syscall * sc, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SMVM_PDK
*******************************************************************************/

const char * SMVM_PDK_get_name(const SMVM_PDK * pdk) __attribute__ ((nonnull(1)));
SMVM_Module * SMVM_PDK_get_module(const SMVM_PDK * pdk) __attribute__ ((nonnull(1)));
SMVM_MODAPI * SMVM_PDK_get_modapi(const SMVM_PDK * pdk) __attribute__ ((nonnull(1)));
size_t SMVM_PDK_get_index(const SMVM_PDK * pdk) __attribute__ ((nonnull(1)));

void SMVM_PDK_set_facility_context(SMVM_PDK * pdk, void * facilityContext) __attribute__ ((nonnull(1)));
void * SMVM_PDK_get_facility_context(const SMVM_PDK * pdk) __attribute__ ((nonnull(1)));
int SMVM_PDK_set_pd_facility(SMVM_PDK * pdk, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_PDK_get_pd_facility(const SMVM_PDK * pdk, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_PDK_set_pdpi_facility(SMVM_PDK * pdk, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_PDK_get_pdpi_facility(const SMVM_PDK * pdk, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SMVM_PD
*******************************************************************************/

SMVM_PD * SMVM_PD_new(SMVM_PDK * pdk, const char * name, const char * conf) __attribute__ ((nonnull(1, 2)));
void SMVM_PD_free(SMVM_PD * pd) __attribute__ ((nonnull(1)));

SMVM_PDK * SMVM_PD_get_pdk(const SMVM_PD * pd) __attribute__ ((nonnull(1)));
SMVM_Module * SMVM_PD_get_module(const SMVM_PD * pd) __attribute__ ((nonnull(1)));
SMVM_MODAPI * SMVM_PD_get_modapi(const SMVM_PD * pd) __attribute__ ((nonnull(1)));

const char * SMVM_PD_get_name(const SMVM_PD * pd) __attribute__ ((nonnull(1)));
const char * SMVM_PD_get_conf(const SMVM_PD * pd) __attribute__ ((nonnull(1)));

void * SMVM_PD_get_handle(const SMVM_PD * pd) __attribute__ ((nonnull(1)));

bool SMVM_PD_is_started(const SMVM_PD * pd) __attribute__ ((nonnull(1)));
bool SMVM_PD_start(SMVM_PD * pd) __attribute__ ((nonnull(1)));
void SMVM_PD_stop(SMVM_PD * pd) __attribute__ ((nonnull(1)));

void SMVM_PD_set_facility_context(SMVM_PD * pd, void * facilityContext) __attribute__ ((nonnull(1)));
void * SMVM_PD_get_facility_context(const SMVM_PD * pd) __attribute__ ((nonnull(1)));
int SMVM_PD_set_facility(SMVM_PD * pd, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_PD_get_facility(const SMVM_PD * pd, const char * name) __attribute__ ((nonnull(1,2)));
int SMVM_PD_set_pdpi_facility(SMVM_PD * pd, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_PD_get_pdpi_facility(const SMVM_PD * pd, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SMVM_PDPI
*******************************************************************************/

SMVM_PDPI * SMVM_PDPI_new(SMVM_PD * pd) __attribute__ ((nonnull(1)));
void SMVM_PDPI_free(SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));

void * SMVM_PDPI_get_handle(const SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));
SMVM_PD * SMVM_PDPI_get_pd(const SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));
SMVM_PDK * SMVM_PDPI_get_pdk(const SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));
SMVM_Module * SMVM_PDPI_get_module(const SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));
SMVM_MODAPI * SMVM_PDPI_get_modapi(const SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));

bool SMVM_PDPI_is_started(const SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));
bool SMVM_PDPI_start(SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));
void SMVM_PDPI_stop(SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));

void SMVM_PDPI_set_facility_context(SMVM_PDPI * pdpi, void * facilityContext) __attribute__ ((nonnull(1)));
void * SMVM_PDPI_get_facility_context(const SMVM_PDPI * pdpi) __attribute__ ((nonnull(1)));
int SMVM_PDPI_set_facility(SMVM_PDPI * pdpi, const char * name, void * facility) __attribute__ ((nonnull(1,2)));
void * SMVM_PDPI_get_facility(const SMVM_PDPI * pdpi, const char * name) __attribute__ ((nonnull(1,2)));

#ifdef __cplusplus
} /* extern "C" { */
#endif


#ifdef SHAREMIND_INTERNAL__
#include "modapi.h"
#endif

#endif /* SHAREMIND_LIBSMMOD_LIBSMMOD_H */
