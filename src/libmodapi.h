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
  Helper macros
*******************************************************************************/

#define SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(ClassName) \
    SharemindModuleApiError ClassName ## _lastError(const ClassName * c) \
            __attribute__ ((nonnull(1))); \
    const char * ClassName ## _lastErrorString(const ClassName * c) \
            __attribute__ ((nonnull(1))); \
    void ClassName ## _clearError(ClassName * c) \
            __attribute__ ((nonnull(1)))

#define SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS__(ClassName,fF,FF) \
    SharemindModuleApiError ClassName ## _set ## FF(ClassName * c, \
                                                    const char * name, \
                                                    void * facility, \
                                                    void * context) \
            __attribute__ ((nonnull(1,2))); \
    bool ClassName ## _unset ## FF(ClassName * c, \
                                   const char * name) \
            __attribute__ ((nonnull(1,2))); \
    const SharemindFacility * ClassName ## _ ## fF(const ClassName * c, \
                                                   const char * name) \
            __attribute__ ((nonnull(1,2))) \

#define SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(ClassName,fN,FN) \
    SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS__(ClassName, \
                                                     fN ## Facility, \
                                                     FN ## Facility)

#define SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(ClassName) \
    SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS__(ClassName, \
                                                     facility, \
                                                     Facility)

#define SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS(ClassName) \
    void * ClassName ## _tag(const ClassName * c) \
            __attribute__ ((nonnull(1))); \
    void * ClassName ## _releaseTag(ClassName * c) \
            __attribute__ ((nonnull(1))); \
    void ClassName ## _destroyTag(ClassName * c) \
            __attribute__ ((nonnull(1))); \
    SharemindTagDestructor ClassName ## _tagDestructor(const ClassName * c) \
            __attribute__ ((nonnull(1))); \
    void ClassName ## _setTagWithDestructor( \
            ClassName * c, void * tag, SharemindTagDestructor tagDestructor) \
            __attribute__ ((nonnull(1))); \
    void ClassName ## _setTag(ClassName * c, void * tag) \
            __attribute__ ((nonnull(1))); \
    void ClassName ## _setTagDestructor(ClassName * c, \
                                        SharemindTagDestructor tagDestructor) \
            __attribute__ ((nonnull(1))) \


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

typedef void (* SharemindTagDestructor)(void *);

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
    ((SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN_KIND,)) \
    ((SHAREMIND_MODULE_API_SHAREMIND_ERROR,)) \
    ((SHAREMIND_MODULE_API_MODULE_ERROR,)) \
    ((SHAREMIND_MODULE_API_PD_STARTUP_FAILED,)) \
    ((SHAREMIND_MODULE_API_PDPI_STARTUP_FAILED,)) \
    ((SHAREMIND_MODULE_API_ERROR_COUNT,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SharemindModuleApiError,
                             SHAREMIND_MODULE_API_ERROR_ENUM);
SHAREMIND_ENUM_DECLARE_TOSTRING(SharemindModuleApiError);


SharemindModuleApi * SharemindModuleApi_new(SharemindModuleApiError * error,
                                            const char ** errorStr);

void SharemindModuleApi_free(SharemindModuleApi * modapi)
        __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindModuleApi);

SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModuleApi,
                                               module,
                                               Module);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModuleApi,pd,Pd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModuleApi,pdpi,Pdpi);

SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS(SharemindModuleApi);

SharemindSyscall * SharemindModuleApi_findSyscall(const SharemindModuleApi * m,
                                                  const char * signature)
        __attribute__ ((nonnull(1, 2)));

SharemindPdk * SharemindModuleApi_findPdk(const SharemindModuleApi * m,
                                          const char * name)
        __attribute__ ((nonnull(1, 2)));

SharemindPd * SharemindModuleApi_findPd(const SharemindModuleApi * m,
                                        const char * name)
        __attribute__ ((nonnull(1, 2)));


SharemindModule * SharemindModuleApi_newModule(SharemindModuleApi * modapi,
                                               const char * filename,
                                               const char * conf)
        __attribute__ ((nonnull(1, 2)));


/*******************************************************************************
  SharemindModule
*******************************************************************************/

void SharemindModule_free(SharemindModule * m) __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindModule);


SharemindModuleApiError SharemindModule_init(SharemindModule * m)
        __attribute__ ((nonnull(1)));

void SharemindModule_deinit(SharemindModule * m)
        __attribute__ ((nonnull(1)));

bool SharemindModule_isInitialized(const SharemindModule * m)
        __attribute__ ((nonnull(1)));


const char * SharemindModule_filename(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

const char * SharemindModule_name(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

const char * SharemindModule_conf(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

uint32_t SharemindModule_apiVersionInUse(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

void * SharemindModule_handle(const SharemindModule * m)
        __attribute__ ((nonnull(1)));


SharemindModuleApi * SharemindModule_modapi(const SharemindModule * m)
        __attribute__ ((nonnull(1)));


size_t SharemindModule_numSyscalls(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModule_syscall(const SharemindModule * m,
                                           size_t index)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModule_findSyscall(const SharemindModule * m,
                                               const char * signature)
        __attribute__ ((nonnull(1, 2)));


size_t SharemindModule_numPdks(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModule_pdk(const SharemindModule * m, size_t index)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModule_findPdk(const SharemindModule * m,
                                       const char * name)
        __attribute__ ((nonnull(1, 2)));

SharemindPd * SharemindModule_findPd(const SharemindModule * m,
                                     const char * name)
        __attribute__ ((nonnull(1, 2)));


SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(SharemindModule);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModule,pd,Pd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModule,pdpi,Pdpi);

SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS(SharemindModule);


/*******************************************************************************
  SharemindSyscall
*******************************************************************************/

const char * SharemindSyscall_signature(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindSyscall_module(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindSyscall_modapi(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));


typedef struct {
    SharemindSyscallCallable callable;
    void (* internal)(void);
} SharemindSyscallWrapper;

SharemindSyscallWrapper SharemindSyscall_wrapper(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS(SharemindSyscall);


/*******************************************************************************
  SharemindPdk
*******************************************************************************/

SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindPdk);


const char * SharemindPdk_name(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPdk_module(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPdk_modapi(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

size_t SharemindPdk_index(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindPdk_findPd(const SharemindPdk * pdk, const char * name)
        __attribute__ ((nonnull(1, 2)));


SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindPdk,pd,Pd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindPdk,pdpi,Pdpi);

SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS(SharemindPdk);


SharemindPd * SharemindPdk_newPd(SharemindPdk * pdk,
                                 const char * name,
                                 const char * conf)
        __attribute__ ((nonnull(1, 2)));


/*******************************************************************************
  SharemindPd
*******************************************************************************/

void SharemindPd_free(SharemindPd * pd) __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindPd);

SharemindPdk * SharemindPd_pdk(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPd_module(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPd_modapi(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));


const char * SharemindPd_name(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

const char * SharemindPd_conf(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));


void * SharemindPd_handle(const SharemindPd * pd) __attribute__ ((nonnull(1)));


bool SharemindPd_isStarted(const SharemindPd * pd) __attribute__ ((nonnull(1)));

bool SharemindPd_start(SharemindPd * pd) __attribute__ ((nonnull(1)));
void SharemindPd_stop(SharemindPd * pd) __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(SharemindPd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindPd,pdpi,Pdpi);

SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS(SharemindPd);


SharemindPdpi * SharemindPd_newPdpi(SharemindPd * pd)
        __attribute__ ((nonnull(1)));


/*******************************************************************************
  SharemindPdpi
*******************************************************************************/

void SharemindPdpi_free(SharemindPdpi * pdpi) __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindPdpi);


void * SharemindPdpi_handle(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindPdpi_pd(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindPdpi_pdk(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPdpi_module(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPdpi_modapi(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));


bool SharemindPdpi_isStarted(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

bool SharemindPdpi_start(SharemindPdpi * pdpi) __attribute__ ((nonnull(1)));
void SharemindPdpi_stop(SharemindPdpi * pdpi) __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(SharemindPdpi);

SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS(SharemindPdpi);


/*******************************************************************************
  Clean up local macros
*******************************************************************************/

#undef SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS__
#undef SHAREMIND_LIBMODAPI_DECLARE_TAG_FUNCTIONS


#ifdef __cplusplus
} /* extern "C" { */
#endif


#endif /* SHAREMIND_LIBMODAPI_LIBMODAPI_H */
