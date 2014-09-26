/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_PDPI_0x1_H
#define SHAREMIND_LIBMODAPI_PDPI_0x1_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include "libmodapi.h"

#include <sharemind/extern_c.h>


SHAREMIND_EXTERN_C_BEGIN

bool SharemindPdpi_start_0x1(SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1), visibility("internal")));

void SharemindPdpi_stop_0x1(SharemindPdpi * pdpi)
        __attribute__ ((nonnull(1), visibility("internal")));

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_PDPI_0x1_H */

