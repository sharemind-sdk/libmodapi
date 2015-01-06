/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
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
      .numPds = &SharemindModule_numPds_0x1,
      .pd = &SharemindModule_pd_0x1,
      .findPd = &SharemindModule_findPd_0x1,
      .startPd = &SharemindPd_start_0x1,
      .stopPd = &SharemindPd_stop_0x1,
      .startPdpi = &SharemindPdpi_start_0x1,
      .stopPdpi = &SharemindPdpi_stop_0x1
    }
};
