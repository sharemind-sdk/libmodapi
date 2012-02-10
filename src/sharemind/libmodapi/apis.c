/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "apis.h"

#include "module_0x1.h"
#include "pd_0x1.h"
#include "pdpi_0x1.h"


const SHAREMIND_API SHAREMIND_APIs[] = {
    { .module_load = &SHAREMIND_Module_load_0x1, .module_unload = &SHAREMIND_Module_unload_0x1,
      .module_init = &SHAREMIND_Module_init_0x1, .module_deinit = &SHAREMIND_Module_deinit_0x1,
      .module_get_num_syscalls = &SHAREMIND_Module_get_num_syscalls_0x1,
      .module_get_syscall = &SHAREMIND_Module_get_syscall_0x1,
      .module_find_syscall = &SHAREMIND_Module_find_syscall_0x1,
      .module_get_num_pdks = &SHAREMIND_Module_get_num_pdks_0x1,
      .module_get_pdk = &SHAREMIND_Module_get_pdk_0x1,
      .module_find_pdk = &SHAREMIND_Module_find_pdk_0x1,
      .pd_start = &SHAREMIND_PD_start_0x1,
      .pd_stop = &SHAREMIND_PD_stop_0x1,
      .pdpi_start = &SHAREMIND_PDPI_start_0x1,
      .pdpi_stop = &SHAREMIND_PDPI_stop_0x1
    }
};
