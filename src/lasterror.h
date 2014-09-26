/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_LASTERROR_H
#define SHAREMIND_LIBMODAPI_LASTERROR_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include <sharemind/extern_c.h>
#include "libmodapi.h"


#define SHAREMIND_LASTERROR_DECLARE_FIELDS \
    SharemindModuleApiError lastError; \
    const char * lastErrorStaticString

#define SHAREMIND_LASTERROR_INIT(className) \
    do { \
        (className)->lastError = SHAREMIND_MODULE_API_OK; \
        (className)->lastErrorStaticString = NULL; \
    } while(0)

#define SHAREMIND_LASTERROR_PRIVATE_FUNCTIONS_DECLARE(ClassName) \
    SHAREMIND_EXTERN_C_BEGIN \
    void ClassName ## _setError( \
            ClassName * c, \
            SharemindModuleApiError error, \
            const char * errorString) __attribute__ ((nonnull(1))); \
    void ClassName ## _setErrorOom(ClassName * c) \
            __attribute__ ((nonnull(1))); \
    void ClassName ## _setErrorOor(ClassName * c) \
            __attribute__ ((nonnull(1))); \
    void ClassName ## _setErrorMie(ClassName * c) \
            __attribute__ ((nonnull(1))) \
    SHAREMIND_EXTERN_C_END

#define SHAREMIND_LASTERROR_FUNCTIONS_DEFINE(ClassName) \
    SHAREMIND_EXTERN_C_BEGIN \
    SharemindModuleApiError ClassName ## _lastError( \
            const ClassName * c) \
    { \
        assert(c); \
        ClassName ## _lockConst(c); \
        const SharemindModuleApiError r = c->lastError; \
        ClassName ## _unlockConst(c); \
        return r; \
    } \
    const char * ClassName ## _lastErrorString( \
            const ClassName * c) \
    { \
        assert(c); \
        const char * r; \
        ClassName ## _lockConst(c); \
        if (unlikely(c->lastError == SHAREMIND_MODULE_API_OK)) { \
            r = NULL; \
        } else { \
            assert(c->lastErrorStaticString); \
            r = c->lastErrorStaticString; \
        } \
        ClassName ## _unlockConst(c); \
        return r; \
    } \
    void ClassName ## _clearError(ClassName * c) { \
        assert(c); \
        ClassName ## _lock(c); \
        c->lastError = SHAREMIND_MODULE_API_OK; \
        ClassName ## _unlock(c); \
    } \
    void ClassName ## _setError(ClassName * c, \
                                SharemindModuleApiError error, \
                                const char * errorString) \
    { \
        assert(c); \
        assert(error != SHAREMIND_MODULE_API_OK); \
        if (unlikely(!errorString || !errorString[0])) \
            errorString = SharemindModuleApiError_toString(error); \
        assert(errorString); \
        assert(errorString[0]); \
        ClassName ## _lock(c); \
        c->lastError = error; \
        c->lastErrorStaticString = errorString; \
        ClassName ## _unlock(c); \
    } \
    void ClassName ## _setErrorOom(ClassName * c) { \
        ClassName ## _setError(c, \
                               SHAREMIND_MODULE_API_OUT_OF_MEMORY,\
                               "Out of memory!"); \
    } \
    void ClassName ## _setErrorOor(ClassName * c) { \
        ClassName ## _setError( \
                c, \
                SHAREMIND_MODULE_API_IMPLEMENTATION_LIMITS_REACHED, \
                "Too many references!"); \
    } \
    void ClassName ## _setErrorMie(ClassName * c) { \
        ClassName ## _setError(c, \
                               SHAREMIND_MODULE_API_MUTEX_ERROR, \
                               "Mutex initialization error!"); \
    } \
    SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_LASTERROR_H */

