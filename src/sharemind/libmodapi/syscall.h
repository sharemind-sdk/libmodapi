/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSHAREMIND_SYSCALL_H
#define SHAREMIND_LIBSHAREMIND_SYSCALL_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include <sharemind/refs.h>
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct SHAREMIND_Syscall_ {

    SHAREMIND_SyscallWrapper wrapper;

    /** Unique name of the system call. */
    char * name;

    /** Pointer to module providing this syscall. */
    SHAREMIND_Module * module;

    SHAREMIND_REFS_DECLARE_FIELDS

};

int SHAREMIND_Syscall_init(SHAREMIND_Syscall * sc,
                      const char * name,
                      void * impl,
                      SHAREMIND_SyscallCallable wrapper,
                      SHAREMIND_Module * m)
     __attribute__ ((nonnull(1, 2, 3, 5)));

void SHAREMIND_Syscall_destroy(SHAREMIND_Syscall * sc) __attribute__ ((nonnull(1)));

SHAREMIND_REFS_DECLARE_FUNCTIONS(SHAREMIND_Syscall)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSHAREMIND_SYSCALL_H */
