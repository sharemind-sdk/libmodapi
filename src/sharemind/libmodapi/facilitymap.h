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
#include <stdlib.h>
#include "libmodapi.h"


SHAREMIND_STRINGMAP_DECLARE(SharemindFacilityMapInner,SharemindFacility,inline)
SHAREMIND_STRINGMAP_DEFINE(SharemindFacilityMapInner,SharemindFacility,malloc,free,strdup,inline)


#ifdef __cplusplus
extern "C" {
#endif

typedef struct SharemindFacilityMap_ {
    SharemindFacilityMapInner realMap;
    struct SharemindFacilityMap_ * nextMap;
} SharemindFacilityMap;

inline void SharemindFacilityMap_init(SharemindFacilityMap * fm, SharemindFacilityMap * nextMap) __attribute__ ((nonnull(1)));
inline void SharemindFacilityMap_destroy(SharemindFacilityMap * fm) __attribute__ ((nonnull(1)));
inline int SharemindFacilityMap_set(SharemindFacilityMap * fm, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
inline const SharemindFacility * SharemindFacilityMap_get(const SharemindFacilityMap * fm, const char * name) __attribute__ ((nonnull(1,2)));


inline void SharemindFacilityMap_init(SharemindFacilityMap * fm, SharemindFacilityMap * nextMap) {
    assert(fm);
    fm->nextMap = nextMap;
    SharemindFacilityMapInner_init(&fm->realMap);
}

inline void SharemindFacilityMap_destroy(SharemindFacilityMap * fm) {
    assert(fm);
    SharemindFacilityMapInner_destroy(&fm->realMap);
}

inline int SharemindFacilityMap_set(SharemindFacilityMap * fm, const char * name, void * facility, void * context) {
    assert(fm);
    assert(name);
    assert(name[0]);
    SharemindFacility * value = SharemindFacilityMapInner_get_or_insert(&fm->realMap, name);
    if (unlikely(!value))
        return 0;

    value->facility = facility;
    value->context = context;
    return 1;
}

inline const SharemindFacility * SharemindFacilityMap_get(const SharemindFacilityMap * fm, const char * name) {
    assert(fm);
    assert(name);
    assert(name[0]);
    const SharemindFacility * value = SharemindFacilityMapInner_get_const(&fm->realMap, name);
    if (value)
        return value;

    if (fm->nextMap)
        return SharemindFacilityMap_get(fm->nextMap, name);

    return NULL;
}

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_FACILITYMAP_H */
