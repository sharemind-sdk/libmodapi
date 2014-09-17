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

typedef SharemindModuleApi0x1Syscall SharemindSyscallCallable;
typedef SharemindModuleApi0x1SyscallContext SharemindSyscallContext;
typedef SharemindModuleApi0x1Facility SharemindFacility;


/*******************************************************************************
  Forward declarations
*******************************************************************************/

struct SharemindModuleApi_;
typedef struct SharemindModuleApi_ SharemindModuleApi;

struct SharemindModule_;
typedef struct SharemindModule_ SharemindModule;

struct SharemindSyscall_;
typedef struct SharemindSyscall_ SharemindSyscall;

struct SharemindPdk_;
typedef struct SharemindPdk_ SharemindPdk;

struct SharemindPd_;
typedef struct SharemindPd_ SharemindPd;

struct SharemindPdpi_;
typedef struct SharemindPdpi_ SharemindPdpi;

/*******************************************************************************
  SharemindModuleApi
*******************************************************************************/

#define SHAREMIND_MODULE_API_API_VERSION     1u
#define SHAREMIND_MODULE_API_API_MIN_VERSION 1u

#define SHAREMIND_MODULE_API_ERROR_ENUM \
    ((SHAREMIND_MODULE_API_OK, = 0)) \
    ((SHAREMIND_MODULE_API_OUT_OF_MEMORY,)) \
    ((SHAREMIND_MODULE_API_REFERENCE_OVERFLOW,)) \
    ((SHAREMIND_MODULE_API_MUTEX_ERROR,)) \
    ((SHAREMIND_MODULE_API_UNABLE_TO_OPEN_MODULE,)) \
    ((SHAREMIND_MODULE_API_INVALID_MODULE,)) \
    ((SHAREMIND_MODULE_API_API_NOT_SUPPORTED,)) \
    ((SHAREMIND_MODULE_API_API_ERROR,)) \
    ((SHAREMIND_MODULE_API_DUPLICATE_SYSCALL,)) \
    ((SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN,)) \
    ((SHAREMIND_MODULE_API_SHAREMIND_ERROR,)) \
    ((SHAREMIND_MODULE_API_MODULE_ERROR,)) \
    ((SHAREMIND_MODULE_API_PD_STARTUP_FAILED,)) \
    ((SHAREMIND_MODULE_API_PDPI_STARTUP_FAILED,)) \
    ((SHAREMIND_MODULE_API_ERROR_COUNT,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SharemindModuleApiError,
                             SHAREMIND_MODULE_API_ERROR_ENUM);
SHAREMIND_ENUM_DECLARE_TOSTRING(SharemindModuleApiError);


SharemindModuleApi * SharemindModuleApi_new(void);

void SharemindModuleApi_free(SharemindModuleApi * modapi)
        __attribute__ ((nonnull(1)));

SharemindModuleApiError SharemindModuleApi_get_last_error(
        const SharemindModuleApi * modapi) __attribute__ ((nonnull(1)));

const char * SharemindModuleApi_get_last_error_string(
        const SharemindModuleApi * modapi) __attribute__ ((nonnull(1)));

void SharemindModuleApi_clear_error(SharemindModuleApi * modapi)
        __attribute__ ((nonnull(1)));


bool SharemindModuleApi_set_module_facility(SharemindModuleApi * modapi,
                                            const char * name,
                                            void * facility,
                                            void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindModuleApi_unset_module_facility(SharemindModuleApi * modapi,
                                              const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindModuleApi_get_module_facility(
        const SharemindModuleApi * modapi,
        const char * name) __attribute__ ((nonnull(1,2)));

bool SharemindModuleApi_set_pd_facility(SharemindModuleApi * modapi,
                                        const char * name,
                                        void * facility,
                                        void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindModuleApi_unset_pd_facility(SharemindModuleApi * modapi,
                                          const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindModuleApi_get_pd_facility(
        const SharemindModuleApi * modapi,
        const char * name) __attribute__ ((nonnull(1,2)));

bool SharemindModuleApi_set_pdpi_facility(SharemindModuleApi * modapi,
                                          const char * name,
                                          void * facility,
                                          void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindModuleApi_unset_pdpi_facility(SharemindModuleApi * modapi,
                                            const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindModuleApi_get_pdpi_facility(
        const SharemindModuleApi * modapi,
        const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SharemindModule
*******************************************************************************/

SharemindModule * SharemindModule_new(SharemindModuleApi * modapi,
                                      const char * filename,
                                      const char * conf)
        __attribute__ ((nonnull(1, 2)));

void SharemindModule_free(SharemindModule * m) __attribute__ ((nonnull(1)));


SharemindModuleApiError SharemindModule_mod_init(SharemindModule * m)
        __attribute__ ((nonnull(1)));

void SharemindModule_mod_deinit(SharemindModule * m)
        __attribute__ ((nonnull(1)));

bool SharemindModule_mod_is_initialized(const SharemindModule * m)
        __attribute__ ((nonnull(1)));


const char * SharemindModule_get_filename(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

const char * SharemindModule_get_name(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

const char * SharemindModule_get_conf(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

uint32_t SharemindModule_get_api_version_in_use(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

void * SharemindModule_get_handle(const SharemindModule * m)
        __attribute__ ((nonnull(1)));


SharemindModuleApi * SharemindModule_get_modapi(const SharemindModule * m)
        __attribute__ ((nonnull(1)));


size_t SharemindModule_get_num_syscalls(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModule_get_syscall(const SharemindModule * m,
                                               size_t index)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModule_find_syscall(const SharemindModule * m,
                                                const char * signature)
        __attribute__ ((nonnull(1, 2)));


size_t SharemindModule_get_num_pdks(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModule_get_pdk(const SharemindModule * m,
                                       size_t index)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModule_find_pdk(const SharemindModule * m,
                                        const char * name)
        __attribute__ ((nonnull(1, 2)));


bool SharemindModule_set_facility(SharemindModule * m,
                                  const char * name,
                                  void * facility,
                                  void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindModule_unset_facility(SharemindModule * m, const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindModule_get_facility(
        const SharemindModule * m,
        const char * name) __attribute__ ((nonnull(1,2)));

bool SharemindModule_set_pd_facility(SharemindModule * m,
                                     const char * name,
                                     void * facility,
                                     void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindModule_unset_pd_facility(SharemindModule * m, const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindModule_get_pd_facility(
        const SharemindModule * m,
        const char * name) __attribute__ ((nonnull(1,2)));

bool SharemindModule_set_pdpi_facility(SharemindModule * m,
                                       const char * name,
                                       void * facility,
                                       void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindModule_unset_pdpi_facility(SharemindModule * m, const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindModule_get_pdpi_facility(
        const SharemindModule * m,
        const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SharemindSyscall
*******************************************************************************/

const char * SharemindSyscall_get_signature(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindSyscall_get_module(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindSyscall_get_modapi(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));


typedef struct {
    SharemindSyscallCallable callable;
    void (* internal)(void);
} SharemindSyscallWrapper;

SharemindSyscallWrapper SharemindSyscall_get_wrapper(
        const SharemindSyscall * sc) __attribute__ ((nonnull(1)));


/*******************************************************************************
  SharemindPdk
*******************************************************************************/

const char * SharemindPdk_get_name(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPdk_get_module(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPdk_get_modapi(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

size_t SharemindPdk_get_index(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));


bool SharemindPdk_set_pd_facility(SharemindPdk * pdk,
                                  const char * name,
                                  void * facility,
                                  void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindPdk_unset_pd_facility(SharemindPdk * pdk, const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindPdk_get_pd_facility(const SharemindPdk * pdk,
                                                       const char * name)
        __attribute__ ((nonnull(1,2)));

bool SharemindPdk_set_pdpi_facility(SharemindPdk * pdk,
                                    const char * name,
                                    void * facility,
                                    void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindPdk_unset_pdpi_facility(SharemindPdk * pdk, const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindPdk_get_pdpi_facility(
        const SharemindPdk * pdk,
        const char * name) __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SharemindPd
*******************************************************************************/

SharemindPd * SharemindPd_new(SharemindPdk * pdk,
                              const char * name,
                              const char * conf)
        __attribute__ ((nonnull(1, 2)));

void SharemindPd_free(SharemindPd * pd) __attribute__ ((nonnull(1)));


SharemindPdk * SharemindPd_get_pdk(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPd_get_module(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPd_get_modapi(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));


const char * SharemindPd_get_name(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

const char * SharemindPd_get_conf(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));


void * SharemindPd_get_handle(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));


bool SharemindPd_is_started(const SharemindPd * pd)
    __attribute__ ((nonnull(1)));

bool SharemindPd_start(SharemindPd * pd) __attribute__ ((nonnull(1)));
void SharemindPd_stop(SharemindPd * pd) __attribute__ ((nonnull(1)));


bool SharemindPd_set_facility(SharemindPd * pd,
                              const char * name,
                              void * facility,
                              void * context) __attribute__ ((nonnull(1,2)));

bool SharemindPd_unset_facility(SharemindPd * pd, const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindPd_get_facility(const SharemindPd * pd,
                                                   const char * name)
        __attribute__ ((nonnull(1,2)));

bool SharemindPd_set_pdpi_facility(SharemindPd * pd,
                                   const char * name,
                                   void * facility,
                                   void * context)
        __attribute__ ((nonnull(1,2)));

bool SharemindPd_unset_pdpi_facility(SharemindPd * pd, const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindPd_get_pdpi_facility(const SharemindPd * pd,
                                                        const char * name)
        __attribute__ ((nonnull(1,2)));


/*******************************************************************************
  SharemindPdpi
*******************************************************************************/

SharemindPdpi * SharemindPdpi_new(SharemindPd * pd)
        __attribute__ ((nonnull(1)));

void SharemindPdpi_free(SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));


void * SharemindPdpi_get_handle(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindPdpi_get_pd(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindPdpi_get_pdk(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPdpi_get_module(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPdpi_get_modapi(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));


bool SharemindPdpi_is_started(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

bool SharemindPdpi_start(SharemindPdpi * pdpi) __attribute__ ((nonnull(1)));
void SharemindPdpi_stop(SharemindPdpi * pdpi) __attribute__ ((nonnull(1)));


bool SharemindPdpi_set_facility(SharemindPdpi * pdpi,
                                const char * name,
                                void * facility,
                                void * context) __attribute__ ((nonnull(1,2)));

bool SharemindPdpi_unset_facility(SharemindPdpi * pdpi,
                                  const char * name)
        __attribute__ ((nonnull(1,2)));

const SharemindFacility * SharemindPdpi_get_facility(const SharemindPdpi * pdpi,
                                                     const char * name)
        __attribute__ ((nonnull(1,2)));

#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* SHAREMIND_LIBMODAPI_LIBMODAPI_H */