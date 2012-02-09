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


#ifdef __cplusplus
extern "C" {
#endif


SMVM_MODAPI_Error SMVM_Module_load_0x1(SMVM_Module * m) __attribute__ ((nonnull(1)));
void SMVM_Module_unload_0x1(SMVM_Module * m) __attribute__ ((nonnull(1)));

SMVM_MODAPI_Error SMVM_Module_init_0x1(SMVM_Module * m) __attribute__ ((nonnull(1)));
void SMVM_Module_deinit_0x1(SMVM_Module * m) __attribute__ ((nonnull(1)));

size_t SMVM_Module_get_num_syscalls_0x1(const SMVM_Module * m) __attribute__ ((nonnull(1)));
SMVM_Syscall * SMVM_Module_get_syscall_0x1(const SMVM_Module * m, size_t index) __attribute__ ((nonnull(1)));
SMVM_Syscall * SMVM_Module_find_syscall_0x1(const SMVM_Module * m, const char * signature) __attribute__ ((nonnull(1, 2)));

size_t SMVM_Module_get_num_pdks_0x1(const SMVM_Module * m) __attribute__ ((nonnull(1)));
SMVM_PDK * SMVM_Module_get_pdk_0x1(const SMVM_Module * m, size_t index) __attribute__ ((nonnull(1)));
SMVM_PDK * SMVM_Module_find_pdk_0x1(const SMVM_Module * m, const char * name) __attribute__ ((nonnull(1, 2)));


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_MODULE_0x1_H */

