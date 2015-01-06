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

#ifndef SHAREMIND_LIBMODAPI_PD_0x1_H
#define SHAREMIND_LIBMODAPI_PD_0x1_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif


#include <sharemind/extern_c.h>
#include "libmodapi.h"


SHAREMIND_EXTERN_C_BEGIN

SharemindModuleApiError SharemindPd_start_0x1(SharemindPd * pd)
        __attribute__ ((nonnull(1), visibility("internal")));

void SharemindPd_stop_0x1(SharemindPd * pd)
        __attribute__ ((nonnull(1), visibility("internal")));

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_PD_0x1_H */

