/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMVM_SYSCALLMAP_H
#define SHAREMIND_LIBSMVM_SYSCALLMAP_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif


#include "../stringmap.h"
#include "syscall.h"


SM_MAP_DECLARE(SMVM_SyscallMap,char *,const char * const,SMVM_Syscall,)

#endif /* SHAREMIND_LIBSMVM_SYSCALLMAP_H */
