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

#ifndef SHAREMIND_LIBMODAPI_FACILITYMAP_H
#define SHAREMIND_LIBMODAPI_FACILITYMAP_H

#ifndef SHAREMIND_INTERNAL_
#error including an internal header!
#endif

#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/stringmap.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "libmodapi.h"


SHAREMIND_STRINGMAP_DECLARE_BODY(SharemindFacilityMapInner, SharemindFacility)
SHAREMIND_STRINGMAP_DECLARE_strHash(SharemindFacilityMapInner,
                                    inline,
                                    SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_strHash(SharemindFacilityMapInner, inline)
SHAREMIND_STRINGMAP_DECLARE_init(SharemindFacilityMapInner,
                                 inline,
                                 SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_init(SharemindFacilityMapInner, inline)
SHAREMIND_STRINGMAP_DECLARE_destroy(SharemindFacilityMapInner,
                                    inline,,
                                    SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_destroy(SharemindFacilityMapInner,
                                   inline,
                                   SharemindFacility,,
                                   free,)
SHAREMIND_STRINGMAP_DECLARE_get(SharemindFacilityMapInner,
                                inline,
                                SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_get(SharemindFacilityMapInner, inline)
SHAREMIND_STRINGMAP_DECLARE_insertHint(SharemindFacilityMapInner,
                                       inline,
                                       SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_insertHint(SharemindFacilityMapInner, inline)
SHAREMIND_STRINGMAP_DECLARE_emplaceAtHint(
        SharemindFacilityMapInner,
        inline,
        SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_emplaceAtHint(SharemindFacilityMapInner,inline)
SHAREMIND_STRINGMAP_DECLARE_insertAtHint(SharemindFacilityMapInner,
                                         inline,
                                         SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_insertAtHint(SharemindFacilityMapInner,
                                        inline,
                                        strdup,
                                        malloc,
                                        free)
SHAREMIND_STRINGMAP_DECLARE_take(SharemindFacilityMapInner,
                                 inline,
                                 SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_take(SharemindFacilityMapInner, inline)
SHAREMIND_STRINGMAP_DECLARE_remove(SharemindFacilityMapInner,
                                   inline,
                                   SHAREMIND_COMMA visibility("internal"))
SHAREMIND_STRINGMAP_DEFINE_remove(SharemindFacilityMapInner,
                                  inline,
                                  SharemindFacility,
                                  free,)

SHAREMIND_EXTERN_C_BEGIN

typedef const SharemindFacility * (* SharemindFacilityMapNextGetter)(
        void * context,
        const char * name);

typedef struct SharemindFacilityMap_ {
    SharemindFacilityMapInner realMap;
    SharemindFacilityMapNextGetter nextGetter;
    void * nextContext;
} SharemindFacilityMap;

inline const SharemindFacility * SharemindFacilityMap_get(
            const SharemindFacilityMap * fm,
            const char * name)
        __attribute__ ((nonnull(1, 2), visibility("internal")));
inline const SharemindFacility * SharemindFacilityMap_get(
        const SharemindFacilityMap * fm,
        const char * name)
{
    assert(fm);
    assert(name);
    assert(name[0]);
    const SharemindFacilityMapInner_value * const v =
            SharemindFacilityMapInner_get(&fm->realMap, name);
    if (v)
        return &v->value;
    if (fm->nextGetter)
        return (*(fm->nextGetter))(fm->nextContext, name);
    return NULL;
}

const SharemindFacility * SharemindFacilityMap_nextMapGetter(
        void * context,
        const char * name)
        __attribute__ ((nonnull(1, 2), visibility("internal")));

inline void SharemindFacilityMap_init(SharemindFacilityMap * fm,
                                      SharemindFacilityMap * nextMap)
        __attribute__ ((nonnull(1), visibility("internal")));
inline void SharemindFacilityMap_init(SharemindFacilityMap * fm,
                                      SharemindFacilityMap * nextMap)
{
    assert(fm);
    if (nextMap) {
        fm->nextGetter = &SharemindFacilityMap_nextMapGetter;
        fm->nextContext = nextMap;
    } else {
        fm->nextGetter = NULL;
        fm->nextContext = NULL;
    }
    SharemindFacilityMapInner_init(&fm->realMap);
}

inline void SharemindFacilityMap_init_with_getter(
        SharemindFacilityMap * fm,
        SharemindFacilityMapNextGetter nextGetter,
        void * context)
        __attribute__ ((nonnull(1), visibility("internal")));
inline void SharemindFacilityMap_init_with_getter(
        SharemindFacilityMap * fm,
        SharemindFacilityMapNextGetter nextGetter,
        void * context)
{
    assert(fm);
    fm->nextGetter = nextGetter;
    fm->nextContext = context;
    SharemindFacilityMapInner_init(&fm->realMap);
}

inline void SharemindFacilityMap_destroy(SharemindFacilityMap * fm)
        __attribute__ ((nonnull(1), visibility("internal")));
inline void SharemindFacilityMap_destroy(SharemindFacilityMap * fm) {
    assert(fm);
    SharemindFacilityMapInner_destroy(&fm->realMap);
}

SHAREMIND_EXTERN_C_END

#define SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS_(CN,fF,FF) \
    SHAREMIND_EXTERN_C_BEGIN \
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
        const SharemindFacility * const f = \
                SharemindFacilityMap_get(&c->fF ## Map, name); \
        if (unlikely(f)) { \
            r = SHAREMIND_MODULE_API_FACILITY_ALREADY_EXISTS; \
            CN ## _setError(c, r, "Facility with this name already exists!"); \
            goto CN ## _set ## FF ## _exit; \
        } \
        void * const insertHint = \
                SharemindFacilityMapInner_insertHint(&c->fF ## Map.realMap, \
                                                     name); \
        assert(insertHint); \
        SharemindFacilityMapInner_value * const v = \
                SharemindFacilityMapInner_insertAtHint(&c->fF ## Map.realMap, \
                                                       name, \
                                                       insertHint); \
        if (unlikely(!v)) { \
            r = SHAREMIND_MODULE_API_OUT_OF_MEMORY; \
            CN ## _setErrorOom(c); \
            goto CN ## _set ## FF ## _exit; \
        } \
        v->value.facility = facility; \
        v->value.context = context; \
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
    } \
    SHAREMIND_EXTERN_C_END

#define SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS(ClassName,fN,FN) \
    SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS_(ClassName, \
                                             fN ## Facility, \
                                             FN ## Facility)

#define SHAREMIND_DEFINE_SELF_FACILITYMAP_ACCESSORS(ClassName) \
    SHAREMIND_DEFINE_FACILITYMAP_ACCESSORS_(ClassName, facility, Facility)

#endif /* SHAREMIND_LIBMODAPI_FACILITYMAP_H */
