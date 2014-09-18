/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_FACILITYMAP_H
#define SHAREMIND_LIBMODAPI_FACILITYMAP_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/stringmap.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "libmodapi.h"


SHAREMIND_STRINGMAP_DECLARE(SharemindFacilityMapInner,SharemindFacility,inline)
#ifndef SHAREMIND_LIBMODAPI_FACILITYMAP_C
SHAREMIND_STRINGMAP_DEFINE(SharemindFacilityMapInner,
                           SharemindFacility,
                           malloc,
                           free,
                           strdup,
                           inline)
#endif /* SHAREMIND_LIBMODAPI_FACILITYMAP_C */


#ifdef __cplusplus
extern "C" {
#endif

typedef struct SharemindFacilityMap_ {
    SharemindFacilityMapInner realMap;
    struct SharemindFacilityMap_ * nextMap;
} SharemindFacilityMap;


inline void SharemindFacilityMap_init(SharemindFacilityMap * fm,
                                      SharemindFacilityMap * nextMap)
        __attribute__ ((nonnull(1)));

inline void SharemindFacilityMap_destroy(SharemindFacilityMap * fm)
        __attribute__ ((nonnull(1)));

inline bool SharemindFacilityMap_set(SharemindFacilityMap * fm,
                                     const char * name,
                                     void * facility,
                                     void * context)
        __attribute__ ((nonnull(1,2)));

inline bool SharemindFacilityMap_unset(SharemindFacilityMap * fm,
                                       const char * name)
        __attribute__ ((nonnull(1,2)));

inline const SharemindFacility * SharemindFacilityMap_get(
            const SharemindFacilityMap * fm,
            const char * name) __attribute__ ((nonnull(1,2)));

inline const SharemindFacility * SharemindFacilityMap_getNoRecurse(
            const SharemindFacilityMap * fm,
            const char * name) __attribute__ ((nonnull(1,2)));


#define SHAREMIND_FACILITYMAP_DEFINE(qualifiers) \
    qualifiers void SharemindFacilityMap_init(SharemindFacilityMap * fm, \
                                              SharemindFacilityMap * nextMap) \
    { \
        assert(fm); \
        fm->nextMap = nextMap; \
        SharemindFacilityMapInner_init(&fm->realMap); \
    } \
    qualifiers void SharemindFacilityMap_destroy(SharemindFacilityMap * fm) { \
        assert(fm); \
        SharemindFacilityMapInner_destroy(&fm->realMap); \
    } \
    qualifiers bool SharemindFacilityMap_set(SharemindFacilityMap * fm, \
                                             const char * name, \
                                             void * facility, \
                                             void * context) \
    { \
        assert(fm); \
        assert(name); \
        assert(name[0]); \
        SharemindFacility * value = SharemindFacilityMapInner_get_or_insert( \
                                        &fm->realMap, \
                                        name); \
        if (unlikely(!value)) \
            return false; \
        value->facility = facility; \
        value->context = context; \
        return true; \
    } \
    qualifiers bool SharemindFacilityMap_unset(SharemindFacilityMap * fm, \
                                               const char * name) \
    { \
        assert(fm); \
        assert(name); \
        assert(name[0]); \
        return SharemindFacilityMapInner_remove(&fm->realMap, name); \
    } \
    qualifiers const SharemindFacility * SharemindFacilityMap_get( \
            const SharemindFacilityMap * fm, \
            const char * name) \
    { \
        assert(fm); \
        assert(name); \
        assert(name[0]); \
        const SharemindFacility * value = SharemindFacilityMapInner_get_const( \
                                              &fm->realMap, \
                                              name); \
        if (value) \
            return value; \
        if (fm->nextMap) \
            return SharemindFacilityMap_get(fm->nextMap, name); \
        return NULL; \
    } \
    qualifiers const SharemindFacility * SharemindFacilityMap_getNoRecurse( \
            const SharemindFacilityMap * fm, \
            const char * name) \
    { \
        assert(fm); \
        assert(name); \
        assert(name[0]); \
        return SharemindFacilityMapInner_get_const(&fm->realMap, name); \
    }

#ifndef SHAREMIND_LIBMODAPI_FACILITYMAP_C
SHAREMIND_FACILITYMAP_DEFINE(inline)
#endif /* SHAREMIND_LIBMODAPI_FACILITYMAP_C */

#define SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS__(CN,fF,FF)\
    SharemindModuleApiError \
    Sharemind ## CN ## _set ## FF(Sharemind ## CN * c, \
                                  const char * name, \
                                  void * facility, \
                                  void * context) \
    { \
        assert(c); \
        assert(name); \
        assert(name[0]); \
        LOCK(c); \
        const bool r = SharemindFacilityMap_set(&c->fF ## Map, \
                                                name, \
                                                facility, \
                                                context); \
        UNLOCK(c); \
        if (!r) { \
            Sharemind ## CN ## _setErrorOom(c); \
            return SHAREMIND_MODULE_API_OUT_OF_MEMORY; \
        } \
        return SHAREMIND_MODULE_API_OK; \
    } \
    bool Sharemind ## CN ## _unset ## FF(Sharemind ## CN * c, \
                                         const char * name) \
    { \
        assert(c); \
        assert(name); \
        assert(name[0]); \
        LOCK(c); \
        const bool r = \
               SharemindFacilityMap_unset(&c->fF ## Map, name); \
        UNLOCK(c); \
        return r; \
    } \
    const SharemindFacility * \
    Sharemind ## CN ## _ ## fF(const Sharemind ## CN * c, const char * name) { \
        assert(c); \
        assert(name); \
        assert(name[0]); \
        LOCK_CONST(c); \
        const SharemindFacility * const r = \
                SharemindFacilityMap_get(&c->fF ## Map, name); \
        UNLOCK_CONST(c); \
        return r; \
    }

#define SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(ClassName,fN,FN) \
    SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS__(ClassName, \
                                             fN ## Facility, \
                                             FN ## Facility)

#define SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(ClassName) \
    SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS__(ClassName, facility, Facility)

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_FACILITYMAP_H */
