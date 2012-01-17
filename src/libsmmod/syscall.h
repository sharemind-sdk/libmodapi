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
#include "facilitymap.h"
#include "libsmmod.h"


#ifdef __cplusplus
extern "C" {
#endif


struct _SMVM_Syscall {

    SMVM_SyscallWrapper wrapper;

    /** Unique name of the system call. */
    char * name;

    /** Pointer to module providing this syscall. */
    SMVM_Module * module;

    /** Pointer to the system call facility context. */
    void * facilityContext;

    /** System call facility name to pointer mapping: */
    SMVM_FacilityMap syscallFacilityMap;

    SMVM_REFS_DECLARE_FIELDS

};

int SMVM_Syscall_init(SMVM_Syscall * sc,
                      const char * name,
                      void * impl,
                      SMVM_SyscallCallable wrapper,
                      SMVM_Module * m)
     __attribute__ ((nonnull(1, 2, 3, 5)));

void SMVM_Syscall_destroy(SMVM_Syscall * sc) __attribute__ ((nonnull(1)));

SMVM_REFS_DECLARE_FUNCTIONS(SMVM_Syscall)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMVM_SYSCALL_H */
