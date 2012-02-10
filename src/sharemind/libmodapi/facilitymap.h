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


SHAREMIND_STRINGMAP_DECLARE(SHAREMIND_FacilityMapInner,SHAREMIND_Facility,inline)
SHAREMIND_STRINGMAP_DEFINE(SHAREMIND_FacilityMapInner,SHAREMIND_Facility,malloc,free,strdup,inline)


#ifdef __cplusplus
extern "C" {
#endif

typedef struct SHAREMIND_FacilityMap_ {
    SHAREMIND_FacilityMapInner realMap;
    struct SHAREMIND_FacilityMap_ * nextMap;
} SHAREMIND_FacilityMap;

inline void SHAREMIND_FacilityMap_init(SHAREMIND_FacilityMap * fm, SHAREMIND_FacilityMap * nextMap) __attribute__ ((nonnull(1)));
inline void SHAREMIND_FacilityMap_destroy(SHAREMIND_FacilityMap * fm) __attribute__ ((nonnull(1)));
inline int SHAREMIND_FacilityMap_set(SHAREMIND_FacilityMap * fm, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
inline const SHAREMIND_Facility * SHAREMIND_FacilityMap_get(const SHAREMIND_FacilityMap * fm, const char * name) __attribute__ ((nonnull(1,2)));


inline void SHAREMIND_FacilityMap_init(SHAREMIND_FacilityMap * fm, SHAREMIND_FacilityMap * nextMap) {
    assert(fm);
    fm->nextMap = nextMap;
    SHAREMIND_FacilityMapInner_init(&fm->realMap);
}

inline void SHAREMIND_FacilityMap_destroy(SHAREMIND_FacilityMap * fm) {
    assert(fm);
    SHAREMIND_FacilityMapInner_destroy(&fm->realMap);
}

inline int SHAREMIND_FacilityMap_set(SHAREMIND_FacilityMap * fm, const char * name, void * facility, void * context) {
    assert(fm);
    assert(name);
    assert(name[0]);
    SHAREMIND_Facility * value = SHAREMIND_FacilityMapInner_get_or_insert(&fm->realMap, name);
    if (unlikely(!value))
        return 0;

    value->facility = facility;
    value->context = context;
    return 1;
}

inline const SHAREMIND_Facility * SHAREMIND_FacilityMap_get(const SHAREMIND_FacilityMap * fm, const char * name) {
    assert(fm);
    assert(name);
    assert(name[0]);
    const SHAREMIND_Facility * value = SHAREMIND_FacilityMapInner_get_const(&fm->realMap, name);
    if (value)
        return value;

    if (fm->nextMap)
        return SHAREMIND_FacilityMap_get(fm->nextMap, name);

    return NULL;
}

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_FACILITYMAP_H */
