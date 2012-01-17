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


const SMVM_API SMVM_APIs[] = {
    { .module_load = &SMVM_Module_load_0x1, .module_unload = &SMVM_Module_unload_0x1,
      .module_init = &SMVM_Module_init_0x1, .module_deinit = &SMVM_Module_deinit_0x1,
      .module_get_num_syscalls = &SMVM_Module_get_num_syscalls_0x1,
      .module_get_syscall = &SMVM_Module_get_syscall_0x1,
      .module_find_syscall = &SMVM_Module_find_syscall_0x1,
      .module_get_num_pdks = &SMVM_Module_get_num_pdks_0x1,
      .module_get_pdk = &SMVM_Module_get_pdk_0x1,
      .module_find_pdk = &SMVM_Module_find_pdk_0x1,
      .pd_start = &SMVM_PD_start_0x1,
      .pd_stop = &SMVM_PD_stop_0x1,
      .pdpi_start = &SMVM_PDPI_start_0x1,
      .pdpi_stop = &SMVM_PDPI_stop_0x1
    }
};
