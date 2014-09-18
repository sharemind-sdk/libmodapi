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
    { .moduleLoad = &SharemindModule_load_0x1,
      .moduleUnload = &SharemindModule_unload_0x1,
      .moduleInit = &SharemindModule_init_0x1,
      .moduleDeinit = &SharemindModule_deinit_0x1,
      .numSyscalls = &SharemindModule_numSyscalls_0x1,
      .syscall = &SharemindModule_syscall_0x1,
      .findSyscall = &SharemindModule_findSyscall_0x1,
      .numPdks = &SharemindModule_numPdks_0x1,
      .pdk = &SharemindModule_pdk_0x1,
      .findPdk = &SharemindModule_findPdk_0x1,
      .startPd = &SharemindPd_start_0x1,
      .stopPd = &SharemindPd_stop_0x1,
      .startPdpi = &SharemindPdpi_start_0x1,
      .stopPdpi = &SharemindPdpi_stop_0x1
    }
};
