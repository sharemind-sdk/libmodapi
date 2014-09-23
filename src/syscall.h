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

#include <sharemind/recursive_locks.h>
#ifndef NDEBUG
#include <sharemind/refs.h>
#endif
#include <stdbool.h>
#include "libmodapi.h"
#include "tag.h"


#ifdef __cplusplus
extern "C" {
#endif


struct SharemindSyscall_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;

    SharemindSyscallWrapper wrapper;

    /** Unique name of the system call. */
    char * name;

    /** Pointer to module providing this syscall. */
    SharemindModule * module;

    #ifndef NDEBUG
    SHAREMIND_REFS_DECLARE_FIELDS
    #endif

    SHAREMIND_TAG_DECLARE_FIELDS;

};

bool SharemindSyscall_init(SharemindSyscall * sc,
                           const char * name,
                           void (* impl)(void),
                           SharemindModule * m)
        __attribute__ ((nonnull(1, 2, 3, 4)));

void SharemindSyscall_destroy(SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE(SharemindSyscall);

#ifndef NDEBUG
SHAREMIND_REFS_DECLARE_FUNCTIONS(SharemindSyscall)
#endif


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_SYSCALL_H */
