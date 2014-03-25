/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_SYSCALL_H
#define SHAREMIND_LIBMODAPI_SYSCALL_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#ifndef NDEBUG
#include <sharemind/refs.h>
#endif
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct SharemindSyscall_ {

    #ifndef NDEBUG
    SharemindMutex mutex;
    #endif

    SharemindSyscallWrapper wrapper;

    /** Unique name of the system call. */
    char * name;

    /** Pointer to module providing this syscall. */
    SharemindModule * module;

    #ifndef NDEBUG
    SHAREMIND_REFS_DECLARE_FIELDS
    #endif

};

int SharemindSyscall_init(SharemindSyscall * sc,
                          const char * name,
                          void (* impl)(void),
                          SharemindSyscallCallable wrapper,
                          SharemindModule * m)
        __attribute__ ((nonnull(1, 2, 3, 5)));

void SharemindSyscall_destroy(SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

#ifndef NDEBUG
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindSyscall)
#endif


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_SYSCALL_H */
