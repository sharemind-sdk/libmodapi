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

#ifndef SHAREMIND_LIBMODAPI_SYSCALL_H
#define SHAREMIND_LIBMODAPI_SYSCALL_H

#ifndef SHAREMIND_INTERNAL_
#error including an internal header!
#endif


#include <sharemind/comma.h>
#include <sharemind/extern_c.h>
#include <sharemind/recursive_locks.h>
#include <sharemind/tag.h>
#include <stdbool.h>
#include "libmodapi.h"


SHAREMIND_EXTERN_C_BEGIN


struct SharemindSyscall_ {

    SHAREMIND_RECURSIVE_LOCK_DECLARE_FIELDS;

    SharemindSyscallWrapper wrapper;

    /** Unique name of the system call. */
    char * name;

    /** Pointer to module providing this syscall. */
    SharemindModule * module;

    SHAREMIND_TAG_DECLARE_FIELDS;

};

bool SharemindSyscall_init(SharemindSyscall * sc,
                           const char * name,
                           void (* impl)(void),
                           SharemindModule * m)
        __attribute__ ((nonnull(1, 2, 3, 4)));

void SharemindSyscall_destroy(SharemindSyscall * sc)
        __attribute__ ((nonnull(1)));

SHAREMIND_RECURSIVE_LOCK_FUNCTIONS_DECLARE_DEFINE(
        SharemindSyscall,
        inline,
        SHAREMIND_COMMA visibility("internal"))

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_SYSCALL_H */
