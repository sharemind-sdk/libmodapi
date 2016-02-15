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

#ifndef SHAREMIND_LIBMODAPI_APIS_H
#define SHAREMIND_LIBMODAPI_APIS_H

#ifndef SHAREMIND_INTERNAL_
#error SHAREMIND_INTERNAL_
#endif


#include <sharemind/extern_c.h>
#include <stdbool.h>
#include "libmodapi.h"


SHAREMIND_EXTERN_C_BEGIN

typedef struct {

    bool (* const moduleLoad)(SharemindModule * m);

    void (* const moduleUnload)(SharemindModule * m);


    SharemindModuleApiError (* const moduleInit)(SharemindModule * m);

    void (* const moduleDeinit)(SharemindModule * m);


    size_t (* const numSyscalls)(const SharemindModule * m);

    SharemindSyscall * (* const syscall)(const SharemindModule * m,
                                         size_t index);

    SharemindSyscall * (* const findSyscall)(const SharemindModule * m,
                                             const char * signature);


    size_t (* const numPdks)(const SharemindModule * m);

    SharemindPdk * (* const pdk)(const SharemindModule * m, size_t index);

    SharemindPdk * (* const findPdk)(const SharemindModule * m,
                                     const char * name);


    size_t (* const numPds)(const SharemindModule * m);

    SharemindPd * (* const pd)(const SharemindModule * m, size_t index);

    SharemindPd * (* const findPd)(const SharemindModule * m,
                                   const char * name);


    SharemindModuleApiError (* const startPd)(SharemindPd * pd);

    void (* const stopPd)(SharemindPd * pd);

    SharemindModuleApiError (* const startPdpi)(SharemindPdpi * pd);

    void (* const stopPdpi)(SharemindPdpi * pd);

} SharemindApi;

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_APIS_H */
