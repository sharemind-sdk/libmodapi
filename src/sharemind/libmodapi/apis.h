/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_APIS_H
#define SHAREMIND_LIBMODAPI_APIS_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <stdbool.h>
#include "libmodapi.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    SharemindModuleApiError (* const module_load)(SharemindModule * m);
    void (* const module_unload)(SharemindModule * m);

    SharemindModuleApiError (* const module_init)(SharemindModule * m);
    void (* const module_deinit)(SharemindModule * m);

    size_t (* const module_get_num_syscalls)(const SharemindModule * m);
    SharemindSyscall * (* const module_get_syscall)(const SharemindModule * m, size_t index);
    SharemindSyscall * (* const module_find_syscall)(const SharemindModule * m, const char * signature);

    size_t (* const module_get_num_pdks)(const SharemindModule * m);
    SharemindPdk * (* const module_get_pdk)(const SharemindModule * m, size_t index);
    SharemindPdk * (* const module_find_pdk)(const SharemindModule * m, const char * name);

    bool (* const pd_start)(SharemindPd * pd);
    void (* const pd_stop)(SharemindPd * pd);

    bool (* const pdpi_start)(SharemindPdpi * pd);
    void (* const pdpi_stop)(SharemindPdpi * pd);
} SHAREMIND_API;

extern const SHAREMIND_API SHAREMIND_APIs[];

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_APIS_H */
