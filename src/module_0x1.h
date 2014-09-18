/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_MODULE_0x1_H
#define SHAREMIND_LIBMODAPI_MODULE_0x1_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include "libmodapi.h"

#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


bool SharemindModule_load_0x1(SharemindModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));

void SharemindModule_unload_0x1(SharemindModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));


bool SharemindModule_init_0x1(SharemindModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));

void SharemindModule_deinit_0x1(SharemindModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));


size_t SharemindModule_numSyscalls_0x1(const SharemindModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));

SharemindSyscall * SharemindModule_syscall_0x1(const SharemindModule * m,
                                               size_t index)
        __attribute__ ((nonnull(1), visibility("internal")));

SharemindSyscall * SharemindModule_findSyscall_0x1(const SharemindModule * m,
                                                   const char * signature)
        __attribute__ ((nonnull(1, 2), visibility("internal")));


size_t SharemindModule_numPdks_0x1(const SharemindModule * m)
        __attribute__ ((nonnull(1), visibility("internal")));

SharemindPdk * SharemindModule_pdk_0x1(const SharemindModule * m, size_t index)
        __attribute__ ((nonnull(1), visibility("internal")));

SharemindPdk * SharemindModule_findPdk_0x1(const SharemindModule * m,
                                           const char * name)
        __attribute__ ((nonnull(1, 2), visibility("internal")));


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_MODULE_0x1_H */

