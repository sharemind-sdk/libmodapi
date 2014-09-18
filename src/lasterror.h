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

#include "libmodapi.h"


#define SHAREMIND_LASTERROR_DECLARE_FIELDS \
    SharemindModuleApiError lastError; \
    const char * lastErrorStaticString;

#define SHAREMIND_LASTERROR_DECLARE_PRIVATE_FUNCTIONS(className) \
    void className ## _setError( \
            className * c, \
            SharemindModuleApiError error, \
            const char * errorString) __attribute__ ((nonnull(1))); \
    void className ## _setErrorOom(className * c) \
            __attribute__ ((nonnull(1))); \
    void className ## _setErrorOor(className * c) \
            __attribute__ ((nonnull(1))); \
    void className ## _setErrorMie(className * c) \
            __attribute__ ((nonnull(1))); \

#define SHAREMIND_LASTERROR_DEFINE_FUNCTIONS(ClassName) \
    SharemindModuleApiError Sharemind ## ClassName ## _lastError( \
            const Sharemind ## ClassName * c) \
    { \
        assert(c); \
        LOCK_CONST(c); \
        const SharemindModuleApiError r = c->lastError; \
        UNLOCK_CONST(c); \
        return r; \
    } \
    const char * Sharemind ## ClassName ## _lastErrorString( \
            const Sharemind ## ClassName * c) \
    { \
        assert(c); \
        const char * r; \
        LOCK_CONST(c); \
        if (unlikely(c->lastError == SHAREMIND_MODULE_API_OK)) { \
            r = NULL; \
        } else { \
            assert(c->lastErrorStaticString); \
            r = c->lastErrorStaticString; \
        } \
        UNLOCK_CONST(c); \
        return r; \
    } \
    void Sharemind ## ClassName ## _clearError(Sharemind ## ClassName * c) { \
        assert(c); \
        LOCK(c); \
        c->lastError = SHAREMIND_MODULE_API_OK; \
        UNLOCK(c); \
    } \
    void Sharemind ## ClassName ## _setError(Sharemind ## ClassName * c, \
                                             SharemindModuleApiError error, \
                                             const char * errorString) \
    { \
        assert(c); \
        assert(error != SHAREMIND_MODULE_API_OK); \
        if (unlikely(!errorString || !errorString[0])) \
            errorString = SharemindModuleApiError_toString(error); \
        assert(errorString); \
        assert(errorString[0]); \
        LOCK(c); \
        c->lastError = error; \
        c->lastErrorStaticString = errorString; \
        UNLOCK(c); \
    } \
    void Sharemind ## ClassName ## _setErrorOom(Sharemind ## ClassName * c) { \
        Sharemind ## ClassName ## _setError(c, \
                                            SHAREMIND_MODULE_API_OUT_OF_MEMORY,\
                                            "Out of memory!"); \
    } \
    void Sharemind ## ClassName ## _setErrorOor(Sharemind ## ClassName * c) { \
        Sharemind ## ClassName ## _setError( \
                c, \
                SHAREMIND_MODULE_API_REFERENCE_OVERFLOW, \
                "Too many references!"); \
    } \
    void Sharemind ## ClassName ## _setErrorMie(Sharemind ## ClassName * c) { \
        Sharemind ## ClassName ## _setError(c, \
                                            SHAREMIND_MODULE_API_MUTEX_ERROR, \
                                            "Mutex initialization error!"); \
    }

#endif /* SHAREMIND_LIBMODAPI_LASTERROR_H */

