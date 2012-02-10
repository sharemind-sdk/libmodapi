/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_LIBMODAPI_H
#define SHAREMIND_LIBMODAPI_LIBMODAPI_H

#include <sharemind/preprocessor.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "api_0x1.h"


#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Most recent API aliases
*******************************************************************************/

typedef SHAREMIND_MODAPI_0x1_Syscall SHAREMIND_SyscallCallable;
typedef SHAREMIND_MODAPI_0x1_Syscall_Context SHAREMIND_SyscallContext;
typedef SHAREMIND_MODAPI_0x1_Facility SHAREMIND_Facility;


/*******************************************************************************
  Forward declarations
*******************************************************************************/

struct _SHAREMIND_MODAPI;
typedef struct _SHAREMIND_MODAPI SHAREMIND_MODAPI;

struct SHAREMIND_Module_;
typedef struct SHAREMIND_Module_ SHAREMIND_Module;

struct SHAREMIND_Syscall_;
typedef struct SHAREMIND_Syscall_ SHAREMIND_Syscall;

struct SHAREMIND_PDK_;
typedef struct SHAREMIND_PDK_ SHAREMIND_PDK;

struct SHAREMIND_PD_;
typedef struct SHAREMIND_PD_ SHAREMIND_PD;

struct SHAREMIND_PDPI_;
typedef struct SHAREMIND_PDPI_ SHAREMIND_PDPI;

/*******************************************************************************
  SHAREMIND_MODAPI
*******************************************************************************/

#define SHAREMIND_MODAPI_API_VERSION     1u
#define SHAREMIND_MODAPI_API_MIN_VERSION 1u

#define SHAREMIND_ENUM_MODAPI_Error \
    ((SHAREMIND_MODAPI_OK, = 0)) \
    ((SHAREMIND_MODAPI_OUT_OF_MEMORY,)) \
    ((SHAREMIND_MODAPI_REFERENCE_OVERFLOW,)) \
    ((SHAREMIND_MODAPI_UNABLE_TO_OPEN_MODULE,)) \
    ((SHAREMIND_MODAPI_INVALID_MODULE,)) \
    ((SHAREMIND_MODAPI_API_NOT_SUPPORTED,)) \
    ((SHAREMIND_MODAPI_API_ERROR,)) \
    ((SHAREMIND_MODAPI_DUPLICATE_SYSCALL,)) \
    ((SHAREMIND_MODAPI_DUPLICATE_PROTECTION_DOMAIN,)) \
    ((SHAREMIND_MODAPI_MODULE_ERROR,)) \
    ((SHAREMIND_MODAPI_PD_STARTUP_FAILED,)) \
    ((SHAREMIND_MODAPI_PDPI_STARTUP_FAILED,)) \
    ((SHAREMIND_MODAPI_ERROR_COUNT,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SHAREMIND_MODAPI_Error, SHAREMIND_ENUM_MODAPI_Error);
SHAREMIND_ENUM_DECLARE_TOSTRING(SHAREMIND_MODAPI_Error);

SHAREMIND_MODAPI * SHAREMIND_MODAPI_new(void);
void SHAREMIND_MODAPI_free(SHAREMIND_MODAPI * modapi) __attribute__ ((nonnull(1)));
SHAREMIND_MODAPI_Error SHAREMIND_MODAPI_lastError(const SHAREMIND_MODAPI * modapi) __attribute__ ((nonnull(1)));
const char * SHAREMIND_MODAPI_lastErrorString(const SHAREMIND_MODAPI * modapi) __attribute__ ((nonnull(1)));
void SHAREMIND_MODAPI_clearError(SHAREMIND_MODAPI * modapi) __attribute__ ((nonnull(1)));

int SHAREMIND_MODAPI_set_module_facility(SHAREMIND_MODAPI * modapi, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_MODAPI_get_module_facility(const SHAREMIND_MODAPI * modapi, const char * name) __attribute__ ((nonnull(1,2)));
int SHAREMIND_MODAPI_set_pd_facility(SHAREMIND_MODAPI * modapi, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_MODAPI_get_pd_facility(const SHAREMIND_MODAPI * modapi, const char * name) __attribute__ ((nonnull(1,2)));
int SHAREMIND_MODAPI_set_pdpi_facility(SHAREMIND_MODAPI * modapi, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_MODAPI_get_pdpi_facility(const SHAREMIND_MODAPI * modapi, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SHAREMIND_Module
*******************************************************************************/

SHAREMIND_Module * SHAREMIND_Module_new(SHAREMIND_MODAPI * modapi, const char * filename) __attribute__ ((nonnull(1, 2)));
void SHAREMIND_Module_free(SHAREMIND_Module * m) __attribute__ ((nonnull(1)));

SHAREMIND_MODAPI_Error SHAREMIND_Module_mod_init(SHAREMIND_Module * m) __attribute__ ((nonnull(1)));
void SHAREMIND_Module_mod_deinit(SHAREMIND_Module * m) __attribute__ ((nonnull(1)));

const char * SHAREMIND_Module_get_name(const SHAREMIND_Module * m) __attribute__ ((nonnull(1)));
uint32_t SHAREMIND_Module_get_api_version_in_use(const SHAREMIND_Module * m) __attribute__ ((nonnull(1)));
void * SHAREMIND_Module_get_handle(const SHAREMIND_Module * m) __attribute__ ((nonnull(1)));

SHAREMIND_MODAPI * SHAREMIND_Module_get_modapi(const SHAREMIND_Module * m) __attribute__ ((nonnull(1)));

size_t SHAREMIND_Module_get_num_syscalls(const SHAREMIND_Module * m) __attribute__ ((nonnull(1)));
SHAREMIND_Syscall * SHAREMIND_Module_get_syscall(const SHAREMIND_Module * m, size_t index) __attribute__ ((nonnull(1)));
SHAREMIND_Syscall * SHAREMIND_Module_find_syscall(const SHAREMIND_Module * m, const char * signature) __attribute__ ((nonnull(1, 2)));

size_t SHAREMIND_Module_get_num_pdks(const SHAREMIND_Module * m) __attribute__ ((nonnull(1)));
SHAREMIND_PDK * SHAREMIND_Module_get_pdk(const SHAREMIND_Module * m, size_t index) __attribute__ ((nonnull(1)));
SHAREMIND_PDK * SHAREMIND_Module_find_pdk(const SHAREMIND_Module * m, const char * name) __attribute__ ((nonnull(1, 2)));

int SHAREMIND_Module_set_facility(SHAREMIND_Module * m, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_Module_get_facility(const SHAREMIND_Module * m, const char * name) __attribute__ ((nonnull(1,2)));
int SHAREMIND_Module_set_pd_facility(SHAREMIND_Module * m, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_Module_get_pd_facility(const SHAREMIND_Module * m, const char * name) __attribute__ ((nonnull(1,2)));
int SHAREMIND_Module_set_pdpi_facility(SHAREMIND_Module * m, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_Module_get_pdpi_facility(const SHAREMIND_Module * m, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SHAREMIND_Syscall
*******************************************************************************/

const char * SHAREMIND_Syscall_get_name(const SHAREMIND_Syscall * sc) __attribute__ ((nonnull(1)));
SHAREMIND_Module * SHAREMIND_Syscall_get_module(const SHAREMIND_Syscall * sc) __attribute__ ((nonnull(1)));
SHAREMIND_MODAPI * SHAREMIND_Syscall_get_modapi(const SHAREMIND_Syscall * sc) __attribute__ ((nonnull(1)));

typedef struct {
    SHAREMIND_SyscallCallable callable;
    void * internal;
} SHAREMIND_SyscallWrapper;

SHAREMIND_SyscallWrapper SHAREMIND_Syscall_get_wrapper(const SHAREMIND_Syscall * sc) __attribute__ ((nonnull(1)));


/*******************************************************************************
  SHAREMIND_PDK
*******************************************************************************/

const char * SHAREMIND_PDK_get_name(const SHAREMIND_PDK * pdk) __attribute__ ((nonnull(1)));
SHAREMIND_Module * SHAREMIND_PDK_get_module(const SHAREMIND_PDK * pdk) __attribute__ ((nonnull(1)));
SHAREMIND_MODAPI * SHAREMIND_PDK_get_modapi(const SHAREMIND_PDK * pdk) __attribute__ ((nonnull(1)));
size_t SHAREMIND_PDK_get_index(const SHAREMIND_PDK * pdk) __attribute__ ((nonnull(1)));

int SHAREMIND_PDK_set_pd_facility(SHAREMIND_PDK * pdk, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_PDK_get_pd_facility(const SHAREMIND_PDK * pdk, const char * name) __attribute__ ((nonnull(1,2)));
int SHAREMIND_PDK_set_pdpi_facility(SHAREMIND_PDK * pdk, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_PDK_get_pdpi_facility(const SHAREMIND_PDK * pdk, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SHAREMIND_PD
*******************************************************************************/

SHAREMIND_PD * SHAREMIND_PD_new(SHAREMIND_PDK * pdk, const char * name, const char * conf) __attribute__ ((nonnull(1, 2)));
void SHAREMIND_PD_free(SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));

SHAREMIND_PDK * SHAREMIND_PD_get_pdk(const SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));
SHAREMIND_Module * SHAREMIND_PD_get_module(const SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));
SHAREMIND_MODAPI * SHAREMIND_PD_get_modapi(const SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));

const char * SHAREMIND_PD_get_name(const SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));
const char * SHAREMIND_PD_get_conf(const SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));

void * SHAREMIND_PD_get_handle(const SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));

bool SHAREMIND_PD_is_started(const SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));
bool SHAREMIND_PD_start(SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));
void SHAREMIND_PD_stop(SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));

int SHAREMIND_PD_set_facility(SHAREMIND_PD * pd, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_PD_get_facility(const SHAREMIND_PD * pd, const char * name) __attribute__ ((nonnull(1,2)));
int SHAREMIND_PD_set_pdpi_facility(SHAREMIND_PD * pd, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_PD_get_pdpi_facility(const SHAREMIND_PD * pd, const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SHAREMIND_PDPI
*******************************************************************************/

SHAREMIND_PDPI * SHAREMIND_PDPI_new(SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));
void SHAREMIND_PDPI_free(SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));

void * SHAREMIND_PDPI_get_handle(const SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));
SHAREMIND_PD * SHAREMIND_PDPI_get_pd(const SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));
SHAREMIND_PDK * SHAREMIND_PDPI_get_pdk(const SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));
SHAREMIND_Module * SHAREMIND_PDPI_get_module(const SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));
SHAREMIND_MODAPI * SHAREMIND_PDPI_get_modapi(const SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));

bool SHAREMIND_PDPI_is_started(const SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));
bool SHAREMIND_PDPI_start(SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));
void SHAREMIND_PDPI_stop(SHAREMIND_PDPI * pdpi) __attribute__ ((nonnull(1)));

int SHAREMIND_PDPI_set_facility(SHAREMIND_PDPI * pdpi, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
const SHAREMIND_Facility * SHAREMIND_PDPI_get_facility(const SHAREMIND_PDPI * pdpi, const char * name) __attribute__ ((nonnull(1,2)));

#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* SHAREMIND_LIBMODAPI_LIBMODAPI_H */
