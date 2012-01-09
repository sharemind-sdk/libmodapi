/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMVM_SYSCALL_H
#define SHAREMIND_LIBSMVM_SYSCALL_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include "../refs.h"
#include "modapi.h"


#ifdef __cplusplus
extern "C" {
#endif


struct _SMVM_Syscall {

    /** Pointer to implementation function if current API, otherwise to a wrapper function. */
    void * impl_or_wrapper;

    /** NULL if current API, otherwise pointer to implementation function. */
    void * null_or_impl;

    /** Unique name of the system call. */
    char * name;

    /** Pointer to module providing this syscall. */
    SMVM_Module * module;

    SMVM_REFS_DECLARE_FIELDS

};

int SMVM_Syscall_init(SMVM_Syscall * sc,
                      const char * name,
                      void * impl,
                      void * wrapper,
                      SMVM_Module * m)
     __attribute__ ((nonnull(1, 2, 3, 5)));

void SMVM_Syscall_destroy(SMVM_Syscall * sc) __attribute__ ((nonnull(1)));

SMVM_REFS_DECLARE_FUNCTIONS(SMVM_Syscall)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMVM_SYSCALL_H */
