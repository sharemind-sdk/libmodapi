/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__
#define SHAREMIND_LIBMODAPI_FACILITYMAP_C

#include "facilitymap.h"

SHAREMIND_STRINGMAP_DEFINE(SharemindFacilityMapInner,
                           SharemindFacility,
                           malloc,
                           free,
                           strdup,
                           extern)

SHAREMIND_FACILITYMAP_DEFINE(extern)
