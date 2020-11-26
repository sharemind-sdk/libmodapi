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

#include "facilitymap.h"


// Instantiations for inline functions:
SHAREMIND_STRINGMAP_DECLARE_strHash(SharemindFacilityMapInner,,)
SHAREMIND_STRINGMAP_DECLARE_init(SharemindFacilityMapInner,,)
SHAREMIND_STRINGMAP_DECLARE_destroy(SharemindFacilityMapInner,,,)
SHAREMIND_STRINGMAP_DECLARE_get(SharemindFacilityMapInner,,)
SHAREMIND_STRINGMAP_DECLARE_insertHint(SharemindFacilityMapInner,,)
SHAREMIND_STRINGMAP_DECLARE_emplaceAtHint(SharemindFacilityMapInner,,)
SHAREMIND_STRINGMAP_DECLARE_insertAtHint(SharemindFacilityMapInner,,)
SHAREMIND_STRINGMAP_DECLARE_take(SharemindFacilityMapInner,,)
SHAREMIND_STRINGMAP_DECLARE_remove(SharemindFacilityMapInner,,)
SharemindFacility const * SharemindFacilityMap_get(
            SharemindFacilityMap const * fm,
            char const * name);
void SharemindFacilityMap_init(SharemindFacilityMap * fm,
                               SharemindFacilityMap * nextMap);
void SharemindFacilityMap_init_with_getter(
        SharemindFacilityMap * fm,
        SharemindFacilityMapNextGetter nextGetter,
        void * context);
void SharemindFacilityMap_destroy(SharemindFacilityMap * fm);


const SharemindFacility * SharemindFacilityMap_nextMapGetter(
        void * context,
        const char * name)
{ return SharemindFacilityMap_get((SharemindFacilityMap *) context, name); }
