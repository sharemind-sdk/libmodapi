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

#define SHAREMIND_INTERNAL_

#include "modapi.h"

#include <stdlib.h>


SHAREMIND_ENUM_CUSTOM_DEFINE_TOSTRING(SharemindModuleApiError,
                                      SHAREMIND_MODULE_API_ERROR_ENUM)

// Instantiations for inline set functions:
SHAREMIND_SET_DECLARE_FUNCTIONS(SharemindModulesSet, SharemindModule *,,)

SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline size_t,
        SharemindModulesSet,
        numSyscalls,,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline size_t,
        SharemindModulesSet,
        numSyscalls,,
        size_t result = 0u;,
        result,
        assert(item->key);
        const size_t r = SharemindModule_numSyscalls(item->key);
        if (SIZE_MAX - r <= result)
            return SIZE_MAX;
        result += r;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindSyscall *,
        SharemindModulesSet,
        syscall,
        SHAREMIND_COMMA size_t index,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindSyscall *,
        SharemindModulesSet,
        syscall,
        SHAREMIND_COMMA size_t index,,
        NULL,
        assert(item->key);
        const size_t n = SharemindModule_numSyscalls(item->key);
        if (index < n)
            return SharemindModule_syscall(item->key, index);
        index -= n;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindSyscall *,
        SharemindModulesSet,
        findSyscall,
        SHAREMIND_COMMA const char * signature,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindSyscall *,
        SharemindModulesSet,
        findSyscall,
        SHAREMIND_COMMA const char * signature,,
        NULL,
        assert(item->key);
        SharemindSyscall * const result =
                SharemindModule_findSyscall(item->key, signature);
        if (result)
            return result;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindSyscallWrapper,
        SharemindModulesSet,
        findSyscallWrapper,
        SHAREMIND_COMMA const char * signature,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindSyscallWrapper,
        SharemindModulesSet,
        findSyscallWrapper,
        SHAREMIND_COMMA const char * signature,
        static const SharemindSyscallWrapper nullWrapper =
                { NULL SHAREMIND_COMMA NULL };,
        nullWrapper,
        assert(item->key);
        SharemindSyscall * const sc =
                SharemindModule_findSyscall(item->key, signature);
        if (sc) {
            const SharemindSyscallWrapper result = {
                SharemindSyscall_wrapper(sc).callable,
                SharemindModule_handle(item->key)
            };
            return result;
        })
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline size_t,
        SharemindModulesSet,
        numPdks,,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline size_t,
        SharemindModulesSet,
        numPdks,,
        size_t result = 0u;,
        result,
        assert(item->key);
        const size_t r = SharemindModule_numPdks(item->key);
        if (SIZE_MAX - r <= result)
            return SIZE_MAX;
        result += r;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindPdk *,
        SharemindModulesSet,
        pdk,
        SHAREMIND_COMMA size_t index,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindPdk *,
        SharemindModulesSet,
        pdk,
        SHAREMIND_COMMA size_t index,,
        NULL,
        assert(item->key);
        const size_t n = SharemindModule_numPdks(item->key);
        if (index < n)
            return SharemindModule_pdk(item->key, index);
        index -= n;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindPdk *,
        SharemindModulesSet,
        findPdk,
        SHAREMIND_COMMA const char * signature,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindPdk *,
        SharemindModulesSet,
        findPdk,
        SHAREMIND_COMMA const char * signature,,
        NULL,
        assert(item->key);
        SharemindPdk * const result =
                SharemindModule_findPdk(item->key, signature);
        if (result)
            return result;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline size_t,
        SharemindModulesSet,
        numPds,,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline size_t,
        SharemindModulesSet,
        numPds,,
        size_t result = 0u;,
        result,
        assert(item->key);
        const size_t r = SharemindModule_numPds(item->key);
        if (SIZE_MAX - r <= result)
            return SIZE_MAX;
        result += r;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindPd *,
        SharemindModulesSet,
        pd,
        SHAREMIND_COMMA size_t index,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindPd *,
        SharemindModulesSet,
        pd,
        SHAREMIND_COMMA size_t index,,
        NULL,
        assert(item->key);
        const size_t n = SharemindModule_numPds(item->key);
        if (index < n)
            return SharemindModule_pd(item->key, index);
        index -= n;)
SHAREMIND_SET_DECLARE_FOREACH_WITH_INLINE(
        static inline SharemindPd *,
        SharemindModulesSet,
        findPd,
        SHAREMIND_COMMA const char * signature,)
SHAREMIND_SET_DEFINE_FOREACH_WITH_INLINE(
        static inline SharemindPd *,
        SharemindModulesSet,
        findPd,
        SHAREMIND_COMMA const char * signature,,
        NULL,
        assert(item->key);
        SharemindPd * const result =
                SharemindModule_findPd(item->key, signature);
        if (result)
            return result;)


SharemindModuleApi * SharemindModuleApi_new(SharemindModuleApiContext * context,
                                            SharemindModuleApiError * error,
                                            const char ** errorStr)
{
    SharemindModuleApi * const modapi =
            (SharemindModuleApi *) malloc(sizeof(SharemindModuleApi));
    if (unlikely(!modapi)) {
        if (error)
            (*error) = SHAREMIND_MODULE_API_OUT_OF_MEMORY;
        if (errorStr)
            (*errorStr) = "Out of memory!";
        return NULL;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(modapi)) {
        free(modapi);
        if (error)
            (*error) = SHAREMIND_MODULE_API_MUTEX_ERROR;
        if (errorStr)
            (*errorStr) = "Mutex initialization error!";
        return NULL;
    }

    SHAREMIND_LIBMODAPI_LASTERROR_INIT(modapi);
    SHAREMIND_TAG_INIT(modapi);

    SharemindModulesSet_init(&modapi->modules);
#define SHAREMIND_LIBMODAPI_MODAPI_FACILITYMAP_INIT(name) \
    do { \
        if (context && context->name ## Facility) { \
            SharemindFacilityMap_init_with_getter( \
                &modapi->name ## FacilityMap, \
                (SharemindFacilityMapNextGetter) context->name ## Facility, \
                context); \
        } else { \
            SharemindFacilityMap_init_with_getter(&modapi->name ## FacilityMap,\
                                                  NULL, \
                                                  NULL); \
        } \
    } while (0)

    SHAREMIND_LIBMODAPI_MODAPI_FACILITYMAP_INIT(module);
    SHAREMIND_LIBMODAPI_MODAPI_FACILITYMAP_INIT(pd);
    SHAREMIND_LIBMODAPI_MODAPI_FACILITYMAP_INIT(pdpi);

    modapi->context = context;

    return modapi;
}

void SharemindModuleApi_free(SharemindModuleApi * modapi) {
    assert(modapi);

    SHAREMIND_TAG_DESTROY(modapi);

    while (modapi->modules.size > 0u)
        SharemindModule_free(*SharemindModulesSet_at(&modapi->modules, 0u));

    SHAREMIND_RECURSIVE_LOCK_DEINIT(modapi);

    SharemindFacilityMap_destroy(&modapi->moduleFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdFacilityMap);
    SharemindFacilityMap_destroy(&modapi->pdpiFacilityMap);

    if (modapi->context && modapi->context->destructor)
        (*(modapi->context->destructor))(modapi->context);

    free(modapi);
}

size_t SharemindModuleApi_numSyscalls(const SharemindModuleApi * m) {
    assert(m);
    SharemindModuleApi_lockConst(m);
    const size_t n = SharemindModulesSet_foreach_with_numSyscalls(&m->modules);
    SharemindModuleApi_lockConst(m);
    return n;
}

SharemindSyscall * SharemindModuleApi_syscall(const SharemindModuleApi * m,
                                              size_t index)
{
    assert(m);
    SharemindModuleApi_lockConst(m);
    SharemindSyscall * const sc =
            SharemindModulesSet_foreach_with_syscall(&m->modules, index);
    SharemindModuleApi_unlockConst(m);
    return sc;
}

SharemindSyscall * SharemindModuleApi_findSyscall(const SharemindModuleApi * m,
                                                  const char * signature)
{
    assert(m);
    SharemindModuleApi_lockConst(m);
    SharemindSyscall * const sc =
            SharemindModulesSet_foreach_with_findSyscall(&m->modules, signature);
    SharemindModuleApi_unlockConst(m);
    return sc;
}

SharemindSyscallWrapper SharemindModuleApi_syscallWrapper(
        const SharemindModuleApi * m,
        const char * signature)
{
    assert(m);
    SharemindModuleApi_lockConst(m);
    const SharemindSyscallWrapper sc =
            SharemindModulesSet_foreach_with_findSyscallWrapper(&m->modules,
                                                                signature);
    SharemindModuleApi_unlockConst(m);
    return sc;
}

size_t SharemindModuleApi_numPdks(const SharemindModuleApi * m) {
    assert(m);
    SharemindModuleApi_lockConst(m);
    const size_t n = SharemindModulesSet_foreach_with_numPdks(&m->modules);
    SharemindModuleApi_unlockConst(m);
    return n;
}

SharemindPdk * SharemindModuleApi_pdk(const SharemindModuleApi * m,
                                      size_t index)
{
    assert(m);
    SharemindModuleApi_lockConst(m);
    SharemindPdk * const pdk =
            SharemindModulesSet_foreach_with_pdk(&m->modules, index);
    SharemindModuleApi_unlockConst(m);
    return pdk;
}

SharemindPdk * SharemindModuleApi_findPdk(const SharemindModuleApi * m,
                                          const char * name)
{
    assert(m);
    SharemindModuleApi_lockConst(m);
    SharemindPdk * const pdk =
            SharemindModulesSet_foreach_with_findPdk(&m->modules, name);
    SharemindModuleApi_unlockConst(m);
    return pdk;
}

size_t SharemindModuleApi_numPds(const SharemindModuleApi * m) {
    assert(m);
    SharemindModuleApi_lockConst(m);
    const size_t n = SharemindModulesSet_foreach_with_numPds(&m->modules);
    SharemindModuleApi_unlockConst(m);
    return n;
}

SharemindPd * SharemindModuleApi_pd(const SharemindModuleApi * m, size_t index){
    assert(m);
    SharemindModuleApi_lockConst(m);
    SharemindPd * const pd =
            SharemindModulesSet_foreach_with_pd(&m->modules, index);
    SharemindModuleApi_unlockConst(m);
    return pd;
}

SharemindPd * SharemindModuleApi_findPd(const SharemindModuleApi * m,
                                        const char * name)
{
    assert(m);
    SharemindModuleApi_lockConst(m);
    SharemindPd * const r =
            SharemindModulesSet_foreach_with_findPd(&m->modules, name);
    SharemindModuleApi_unlockConst(m);
    return r;
}

// Instantiations for inline lock functions:
SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE(SharemindModuleApi,,)

SHAREMIND_LIBMODAPI_LASTERROR_FUNCTIONS_DEFINE(SharemindModuleApi)

SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,module,Module)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pd,Pd)
SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(SharemindModuleApi,pdpi,Pdpi)

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindModuleApi,)
