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

#ifndef SHAREMIND_LIBMODAPI_LIBMODAPI_H
#define SHAREMIND_LIBMODAPI_LIBMODAPI_H

#include <sharemind/extern_c.h>
#include <sharemind/lasterror.h>
#include <sharemind/preprocessor.h>
#include <sharemind/tag.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "api_0x1.h"


SHAREMIND_EXTERN_C_BEGIN


/*******************************************************************************
  Module API compatibility macros
*******************************************************************************/

#define SHAREMIND_MODULE_API_API_VERSION     1u
#define SHAREMIND_MODULE_API_API_MIN_VERSION 1u


/*******************************************************************************
  Helper macros
*******************************************************************************/

#define SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(ClassName) \
    SHAREMIND_LASTERROR_PUBLIC_FUNCTIONS_DECLARE(ClassName,, \
                                                 SharemindModuleApiError,)

#define SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS_(ClassName,fF,FF) \
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
    SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS_(ClassName, \
                                                     fN ## Facility, \
                                                     FN ## Facility)

#define SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(ClassName) \
    SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS_(ClassName, \
                                                     facility, \
                                                     Facility)


/*******************************************************************************
  Most recent API aliases
*******************************************************************************/

typedef SharemindModuleApi0x1Syscall SharemindSyscallCallable;
typedef SharemindModuleApi0x1SyscallContext SharemindSyscallContext;
typedef SharemindModuleApi0x1Facility SharemindFacility;


/*******************************************************************************
  Types
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

typedef struct {
    SharemindSyscallCallable callable;
    void * internal;
} SharemindSyscallWrapper;

/**
  WARNING: The following error values currently need to be kept in sync with the
           APIS! So keep care when renaming or reordering these.
*/
#define SHAREMIND_MODULE_API_ERROR_ENUM \
    ((SHAREMIND_MODULE_API_OK, = 0)) \
    ((SHAREMIND_MODULE_API_OUT_OF_MEMORY,)) \
    ((SHAREMIND_MODULE_API_IMPLEMENTATION_LIMITS_REACHED,)) \
    ((SHAREMIND_MODULE_API_MODULE_ERROR,)) \
    ((SHAREMIND_MODULE_API_GENERAL_ERROR,)) \
    ((SHAREMIND_MODULE_API_INVALID_CALL,)) \
    ((SHAREMIND_MODULE_API_MISSING_FACILITY,)) \
    ((SHAREMIND_MODULE_API_INVALID_PD_CONFIGURATION,)) \
    ((SHAREMIND_MODULE_API_INVALID_MODULE_CONFIGURATION,)) \
    ((SHAREMIND_MODULE_API_MUTEX_ERROR, = 10000)) \
    ((SHAREMIND_MODULE_API_UNABLE_TO_OPEN_MODULE,)) \
    ((SHAREMIND_MODULE_API_INVALID_MODULE,)) \
    ((SHAREMIND_MODULE_API_API_NOT_SUPPORTED,)) \
    ((SHAREMIND_MODULE_API_API_ERROR,)) \
    ((SHAREMIND_MODULE_API_DUPLICATE_SYSCALL,)) \
    ((SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN,)) \
    ((SHAREMIND_MODULE_API_DUPLICATE_PROTECTION_DOMAIN_KIND,)) \
    ((SHAREMIND_MODULE_API_FACILITY_ALREADY_EXISTS,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SharemindModuleApiError,
                             SHAREMIND_MODULE_API_ERROR_ENUM);
SHAREMIND_ENUM_DECLARE_TOSTRING(SharemindModuleApiError);


/*******************************************************************************
  SharemindModuleApiContext
*******************************************************************************/

struct SharemindModuleApiContext_;
typedef struct SharemindModuleApiContext_ SharemindModuleApiContext;
struct SharemindModuleApiContext_ {
    void * internal;
    void (* destructor)(SharemindModuleApiContext * context);
    SharemindFacility const * (* moduleFacility)(
            SharemindModuleApiContext * context,
            const char * signature);
    SharemindFacility const * (* pdFacility)(
            SharemindModuleApiContext * context,
            const char * signature);
    SharemindFacility const * (* pdpiFacility)(
            SharemindModuleApiContext * context,
            const char * signature);
};


/*******************************************************************************
  SharemindModuleApi
*******************************************************************************/

SharemindModuleApi * SharemindModuleApi_new(SharemindModuleApiContext * context,
                                            SharemindModuleApiError * error,
                                            const char ** errorStr);

void SharemindModuleApi_free(SharemindModuleApi * modapi)
        __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindModuleApi)

SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModuleApi,
                                               module,
                                               Module);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModuleApi,pd,Pd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModuleApi,pdpi,Pdpi);

SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindModuleApi,,)


size_t SharemindModuleApi_numSyscalls(const SharemindModuleApi * m)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModuleApi_syscall(const SharemindModuleApi * m,
                                              size_t index)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModuleApi_findSyscall(const SharemindModuleApi * m,
                                                  const char * signature)
        __attribute__ ((nonnull(1, 2)));

SharemindSyscallWrapper SharemindModuleApi_syscallWrapper(
        const SharemindModuleApi * m,
        const char * signature) __attribute__ ((nonnull(1, 2)));


size_t SharemindModuleApi_numPdks(const SharemindModuleApi * m)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModuleApi_pdk(const SharemindModuleApi * m,
                                      size_t index)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModuleApi_findPdk(const SharemindModuleApi * m,
                                          const char * name)
        __attribute__ ((nonnull(1, 2)));


size_t SharemindModuleApi_numPds(const SharemindModuleApi * m)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindModuleApi_pd(const SharemindModuleApi * m, size_t index)
        __attribute__ ((nonnull(1)));

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


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindModule)

SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(SharemindModule);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModule,pd,Pd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindModule,pdpi,Pdpi);

SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindModule,,)


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


SharemindModuleApi * SharemindModule_moduleApi(const SharemindModule * m)
        __attribute__ ((nonnull(1)));


size_t SharemindModule_numSyscalls(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModule_syscall(const SharemindModule * m,
                                           size_t index)
        __attribute__ ((nonnull(1)));

SharemindSyscall * SharemindModule_findSyscall(const SharemindModule * m,
                                               const char * signature)
        __attribute__ ((nonnull(1, 2)));

SharemindSyscallWrapper SharemindModule_syscallWrapper(
        const SharemindModule * m,
        const char * signature) __attribute__ ((nonnull(1, 2)));


size_t SharemindModule_numPdks(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModule_pdk(const SharemindModule * m, size_t index)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindModule_findPdk(const SharemindModule * m,
                                       const char * name)
        __attribute__ ((nonnull(1, 2)));


size_t SharemindModule_numPds(const SharemindModule * m)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindModule_pd(const SharemindModule * m, size_t index)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindModule_findPd(const SharemindModule * m,
                                     const char * name)
        __attribute__ ((nonnull(1, 2)));


/*******************************************************************************
  SharemindSyscall
*******************************************************************************/

SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindSyscall,,)


const char * SharemindSyscall_signature(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindSyscall_module(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindSyscall_moduleApi(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SharemindSyscallWrapper SharemindSyscall_wrapper(const SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));


/*******************************************************************************
  SharemindPdk
*******************************************************************************/

SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindPdk)

SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindPdk,pd,Pd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindPdk,pdpi,Pdpi);

SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindPdk,,)


const char * SharemindPdk_name(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPdk_module(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPdk_moduleApi(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

size_t SharemindPdk_index(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));


size_t SharemindPdk_numPds(const SharemindPdk * pdk)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindPdk_pd(const SharemindPdk * pdk, size_t index)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindPdk_findPd(const SharemindPdk * pdk, const char * name)
        __attribute__ ((nonnull(1, 2)));


SharemindPd * SharemindPdk_newPd(SharemindPdk * pdk,
                                 const char * name,
                                 const char * conf)
        __attribute__ ((nonnull(1, 2)));


/*******************************************************************************
  SharemindPd
*******************************************************************************/

void SharemindPd_free(SharemindPd * pd) __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindPd)

SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(SharemindPd);
SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS(SharemindPd,pdpi,Pdpi);

SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindPd,,)


bool SharemindPd_isStarted(const SharemindPd * pd) __attribute__ ((nonnull(1)));

SharemindModuleApiError SharemindPd_start(SharemindPd * pd)
        __attribute__ ((nonnull(1)));
void SharemindPd_stop(SharemindPd * pd) __attribute__ ((nonnull(1)));


SharemindPdk * SharemindPd_pdk(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPd_module(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPd_moduleApi(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

const char * SharemindPd_name(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

const char * SharemindPd_conf(const SharemindPd * pd)
        __attribute__ ((nonnull(1)));

void * SharemindPd_handle(const SharemindPd * pd) __attribute__ ((nonnull(1)));


SharemindPdpi * SharemindPd_newPdpi(SharemindPd * pd)
        __attribute__ ((nonnull(1)));


/*******************************************************************************
  SharemindPdpi
*******************************************************************************/

void SharemindPdpi_free(SharemindPdpi * pdpi) __attribute__ ((nonnull(1)));


SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS(SharemindPdpi)

SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS(SharemindPdpi);

SHAREMIND_TAG_FUNCTIONS_DECLARE(SharemindPdpi,,)


bool SharemindPdpi_isStarted(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindModuleApiError SharemindPdpi_start(SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));
void SharemindPdpi_stop(SharemindPdpi * pdpi) __attribute__ ((nonnull(1)));


void * SharemindPdpi_handle(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindPd * SharemindPdpi_pd(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindPdk * SharemindPdpi_pdk(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindModule * SharemindPdpi_module(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));

SharemindModuleApi * SharemindPdpi_moduleApi(const SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1)));


/*******************************************************************************
  Clean up local macros
*******************************************************************************/

#undef SHAREMIND_LIBMODAPI_DECLARE_ERROR_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_DECLARE_SELF_FACILITY_FUNCTIONS
#undef SHAREMIND_LIBMODAPI_DECLARE_FACILITY_FUNCTIONS_


SHAREMIND_EXTERN_C_END


#endif /* SHAREMIND_LIBMODAPI_LIBMODAPI_H */
