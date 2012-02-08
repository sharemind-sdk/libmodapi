/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMMOD_FACILITYMAP_H
#define SHAREMIND_LIBSMMOD_FACILITYMAP_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/stringmap.h>
#include <stdlib.h>
#include "libsmmod.h"


SM_STRINGMAP_DECLARE(SMVM_FacilityMapInner,SMVM_Facility,inline)
SM_STRINGMAP_DEFINE(SMVM_FacilityMapInner,SMVM_Facility,malloc,free,strdup,inline)


#ifdef __cplusplus
extern "C" {
#endif

typedef struct SMVM_FacilityMap_ {
    SMVM_FacilityMapInner realMap;
    struct SMVM_FacilityMap_ * nextMap;
} SMVM_FacilityMap;

inline void SMVM_FacilityMap_init(SMVM_FacilityMap * fm, SMVM_FacilityMap * nextMap) __attribute__ ((nonnull(1)));
inline void SMVM_FacilityMap_destroy(SMVM_FacilityMap * fm) __attribute__ ((nonnull(1)));
inline int SMVM_FacilityMap_set(SMVM_FacilityMap * fm, const char * name, void * facility, void * context) __attribute__ ((nonnull(1,2)));
inline const SMVM_Facility * SMVM_FacilityMap_get(const SMVM_FacilityMap * fm, const char * name) __attribute__ ((nonnull(1,2)));


inline void SMVM_FacilityMap_init(SMVM_FacilityMap * fm, SMVM_FacilityMap * nextMap) {
    assert(fm);
    fm->nextMap = nextMap;
    SMVM_FacilityMapInner_init(&fm->realMap);
}

inline void SMVM_FacilityMap_destroy(SMVM_FacilityMap * fm) {
    assert(fm);
    SMVM_FacilityMapInner_destroy(&fm->realMap);
}

inline int SMVM_FacilityMap_set(SMVM_FacilityMap * fm, const char * name, void * facility, void * context) {
    assert(fm);
    assert(name);
    assert(name[0]);
    SMVM_Facility * value = SMVM_FacilityMapInner_get_or_insert(&fm->realMap, name);
    if (unlikely(!value))
        return 0;

    value->facility = facility;
    value->context = context;
    return 1;
}

inline const SMVM_Facility * SMVM_FacilityMap_get(const SMVM_FacilityMap * fm, const char * name) {
    assert(fm);
    assert(name);
    assert(name[0]);
    const SMVM_Facility * value = SMVM_FacilityMapInner_get_const(&fm->realMap, name);
    if (value)
        return value;

    if (fm->nextMap)
        return SMVM_FacilityMap_get(fm->nextMap, name);

    return NULL;
}

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMMOD_FACILITYMAP_H */
