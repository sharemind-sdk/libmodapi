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


SharemindModuleApiError SHAREMIND_Module_load_0x1(SharemindModule * m) __attribute__ ((nonnull(1)));
void SHAREMIND_Module_unload_0x1(SharemindModule * m) __attribute__ ((nonnull(1)));

SharemindModuleApiError SHAREMIND_Module_init_0x1(SharemindModule * m) __attribute__ ((nonnull(1)));
void SHAREMIND_Module_deinit_0x1(SharemindModule * m) __attribute__ ((nonnull(1)));

size_t SHAREMIND_Module_get_num_syscalls_0x1(const SharemindModule * m) __attribute__ ((nonnull(1)));
SharemindSyscall * SHAREMIND_Module_get_syscall_0x1(const SharemindModule * m, size_t index) __attribute__ ((nonnull(1)));
SharemindSyscall * SHAREMIND_Module_find_syscall_0x1(const SharemindModule * m, const char * signature) __attribute__ ((nonnull(1, 2)));

size_t SHAREMIND_Module_get_num_pdks_0x1(const SharemindModule * m) __attribute__ ((nonnull(1)));
SharemindPdk * SHAREMIND_Module_get_pdk_0x1(const SharemindModule * m, size_t index) __attribute__ ((nonnull(1)));
SharemindPdk * SHAREMIND_Module_find_pdk_0x1(const SharemindModule * m, const char * name) __attribute__ ((nonnull(1, 2)));


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_MODULE_0x1_H */

