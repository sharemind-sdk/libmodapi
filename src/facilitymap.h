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

inline const SharemindFacility * SharemindFacilityMap_get(
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
    qualifiers const SharemindFacility * SharemindFacilityMap_get( \
            const SharemindFacilityMap * fm, \
            const char * name) \
    { \
        assert(fm); \
        assert(name); \
        assert(name[0]); \
        const SharemindFacility * value = SharemindFacilityMapInner_get( \
                                              &fm->realMap, \
                                              name); \
        if (value) \
            return value; \
        if (fm->nextMap) \
            return SharemindFacilityMap_get(fm->nextMap, name); \
        return NULL; \
    }

#ifndef SHAREMIND_LIBMODAPI_FACILITYMAP_C
SHAREMIND_FACILITYMAP_DEFINE(inline)
#endif /* SHAREMIND_LIBMODAPI_FACILITYMAP_C */

#define SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS__(CN,fF,FF)\
    SharemindModuleApiError CN ## _set ## FF(CN * c, \
                                             const char * name, \
                                             void * facility, \
                                             void * context) \
    { \
        assert(c); \
        assert(name); \
        assert(name[0]); \
        SharemindModuleApiError r = SHAREMIND_MODULE_API_OK; \
        CN ## _lock(c); \
        void * const insertHint = \
                SharemindFacilityMapInner_insertHint(&c->fF ## Map.realMap, \
                                                     name); \
        if (unlikely(!insertHint)) { \
            r = SHAREMIND_MODULE_API_FACILITY_ALREADY_EXISTS; \
            CN ## _setError(c, r, "Facility with this name already exists!"); \
            goto CN ## _set ## FF ## _exit; \
        } \
        SharemindFacility * const value = \
                SharemindFacilityMapInner_insertAtHint(&c->fF ## Map.realMap, \
                                                       name, \
                                                       insertHint); \
        if (unlikely(!value)) { \
            r = SHAREMIND_MODULE_API_OUT_OF_MEMORY; \
            CN ## _setErrorOom(c); \
            goto CN ## _set ## FF ## _exit; \
        } \
        value->facility = facility; \
        value->context = context; \
    CN ## _set ## FF ## _exit: \
        CN ## _unlock(c); \
        return r; \
    } \
    bool CN ## _unset ## FF(CN * c, const char * name) { \
        assert(c); \
        assert(name); \
        assert(name[0]); \
        CN ## _lock(c); \
        const bool r = \
                SharemindFacilityMapInner_remove(&c->fF ## Map.realMap, name); \
        CN ## _unlock(c); \
        return r; \
    } \
    const SharemindFacility * \
    CN ## _ ## fF(const CN * c, const char * name) { \
        assert(c); \
        assert(name); \
        assert(name[0]); \
        CN ## _lockConst(c); \
        const SharemindFacility * const r = \
                SharemindFacilityMap_get(&c->fF ## Map, name); \
        CN ## _unlockConst(c); \
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
