/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMVM_API_0x1_H
#define SHAREMIND_LIBSMVM_API_0x1_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif

#include "libsmmod.h"


#ifdef __cplusplus
extern "C" {
#endif

SMVM_MODAPI_Error loadModule_0x1(SMVM_Module * m);
void unloadModule_0x1(SMVM_Module * m);

SMVM_MODAPI_Error initModule_0x1(SMVM_Module * m);
void deinitModule_0x1(SMVM_Module * m);

size_t getNumSyscalls_0x1(const SMVM_Module * m);
SMVM_Syscall * getSyscall_0x1(const SMVM_Module * m, size_t index);
SMVM_Syscall * findSyscall_0x1(const SMVM_Module * m, const char * signature);

size_t getNumPdks_0x1(const SMVM_Module * m);
SMVM_PDK * getPdk_0x1(const SMVM_Module * m, size_t index);
SMVM_PDK * findPdk_0x1(const SMVM_Module * m, const char * name);

int startPd_0x1(const SMVM_PD * pd);
void stopPd_0x1(const SMVM_PD * pd);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMVM_API_0x1_H */
