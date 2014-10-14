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


#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <stdbool.h>
#include "libmodapi.h"
#include "tag.h"


SHAREMIND_EXTERN_C_BEGIN


struct SharemindSyscall_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;

    SharemindSyscallWrapper wrapper;

    /** Unique name of the system call. */
    char * name;

    /** Pointer to module providing this syscall. */
    SharemindModule * module;

    SHAREMIND_TAG_DECLARE_FIELDS;

};

bool SharemindSyscall_init(SharemindSyscall * sc,
                           const char * name,
                           void (* impl)(void),
                           SharemindModule * m)
        __attribute__ ((nonnull(1, 2, 3, 4)));

void SharemindSyscall_destroy(SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindSyscall,
        inline,
        SHAREMIND_COMMA visibility("internal"))

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_SYSCALL_H */
