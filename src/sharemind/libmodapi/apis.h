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
    SHAREMIND_MODAPI_Error (* const module_load)(SHAREMIND_Module * m);
    void (* const module_unload)(SHAREMIND_Module * m);

    SHAREMIND_MODAPI_Error (* const module_init)(SHAREMIND_Module * m);
    void (* const module_deinit)(SHAREMIND_Module * m);

    size_t (* const module_get_num_syscalls)(const SHAREMIND_Module * m);
    SHAREMIND_Syscall * (* const module_get_syscall)(const SHAREMIND_Module * m, size_t index);
    SHAREMIND_Syscall * (* const module_find_syscall)(const SHAREMIND_Module * m, const char * signature);

    size_t (* const module_get_num_pdks)(const SHAREMIND_Module * m);
    SHAREMIND_PDK * (* const module_get_pdk)(const SHAREMIND_Module * m, size_t index);
    SHAREMIND_PDK * (* const module_find_pdk)(const SHAREMIND_Module * m, const char * name);

    bool (* const pd_start)(SHAREMIND_PD * pd);
    void (* const pd_stop)(SHAREMIND_PD * pd);

    bool (* const pdpi_start)(SHAREMIND_PDPI * pd);
    void (* const pdpi_stop)(SHAREMIND_PDPI * pd);
} SHAREMIND_API;

extern const SHAREMIND_API SHAREMIND_APIs[];

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_APIS_H */
