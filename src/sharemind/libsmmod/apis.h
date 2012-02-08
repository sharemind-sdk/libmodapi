/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMMOD_APIS_H
#define SHAREMIND_LIBSMMOD_APIS_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <stdbool.h>
#include "libsmmod.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    SMVM_MODAPI_Error (* const module_load)(SMVM_Module * m);
    void (* const module_unload)(SMVM_Module * m);

    SMVM_MODAPI_Error (* const module_init)(SMVM_Module * m);
    void (* const module_deinit)(SMVM_Module * m);

    size_t (* const module_get_num_syscalls)(const SMVM_Module * m);
    SMVM_Syscall * (* const module_get_syscall)(const SMVM_Module * m, size_t index);
    SMVM_Syscall * (* const module_find_syscall)(const SMVM_Module * m, const char * signature);

    size_t (* const module_get_num_pdks)(const SMVM_Module * m);
    SMVM_PDK * (* const module_get_pdk)(const SMVM_Module * m, size_t index);
    SMVM_PDK * (* const module_find_pdk)(const SMVM_Module * m, const char * name);

    bool (* const pd_start)(SMVM_PD * pd);
    void (* const pd_stop)(SMVM_PD * pd);

    bool (* const pdpi_start)(SMVM_PDPI * pd);
    void (* const pdpi_stop)(SMVM_PDPI * pd);
} SMVM_API;

extern const SMVM_API SMVM_APIs[];

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMMOD_APIS_H */
