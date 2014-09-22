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

    bool (* const moduleLoad)(SharemindModule * m);

    void (* const moduleUnload)(SharemindModule * m);


    bool (* const moduleInit)(SharemindModule * m);

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

    SharemindPd * (* const findPd)(const SharemindModule * m,
                                   const char * name);


    bool (* const startPd)(SharemindPd * pd);

    void (* const stopPd)(SharemindPd * pd);

    bool (* const startPdpi)(SharemindPdpi * pd);

    void (* const stopPdpi)(SharemindPdpi * pd);

} SharemindApi;

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_APIS_H */
