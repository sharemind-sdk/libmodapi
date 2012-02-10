/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_PD_0x1_H
#define SHAREMIND_LIBMODAPI_PD_0x1_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


bool SHAREMIND_PD_start_0x1(SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));
void SHAREMIND_PD_stop_0x1(SHAREMIND_PD * pd) __attribute__ ((nonnull(1)));


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_PD_0x1_H */

