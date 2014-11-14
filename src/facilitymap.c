/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "facilitymap.h"


const SharemindFacility * SharemindFacilityMap_nextMapGetter(
        void * context,
        const char * name)
{ return SharemindFacilityMap_get((SharemindFacilityMap *) context, name); }
