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


static const SharemindApi SharemindApis[] = {
    { .module_load = &SharemindModule_load_0x1,
      .module_unload = &SharemindModule_unload_0x1,
      .module_init = &SharemindModule_init_0x1,
      .module_deinit = &SharemindModule_deinit_0x1,
      .module_get_num_syscalls = &SharemindModule_get_num_syscalls_0x1,
      .module_get_syscall = &SharemindModule_get_syscall_0x1,
      .module_find_syscall = &SharemindModule_find_syscall_0x1,
      .module_get_num_pdks = &SharemindModule_get_num_pdks_0x1,
      .module_get_pdk = &SharemindModule_get_pdk_0x1,
      .module_find_pdk = &SharemindModule_find_pdk_0x1,
      .pd_start = &SharemindPd_start_0x1,
      .pd_stop = &SharemindPd_stop_0x1,
      .pdpi_start = &SharemindPdpi_start_0x1,
      .pdpi_stop = &SharemindPdpi_stop_0x1
    }
};
