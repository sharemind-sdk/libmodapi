/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_LASTERROR_H
#define SHAREMIND_LIBMODAPI_LASTERROR_H

#ifndef SHAREMIND_INTERNAL__
#error SHAREMIND_INTERNAL__
#endif

#include <sharemind/lasterror.h>
#include "libmodapi.h"


#define SHAREMIND_LIBMODAPI_LASTERROR_FIELDS \
    SHAREMIND_LASTERROR_DECLARE_FIELDS(SharemindModuleApiError)

#define SHAREMIND_LIBMODAPI_LASTERROR_INIT(className) \
    SHAREMIND_LASTERROR_INIT(className, SHAREMIND_MODULE_API_OK)

#define SHAREMIND_LIBMODAPI_LASTERROR_PRIVATE_FUNCTIONS(ClassName) \
    SHAREMIND_LASTERROR_PRIVATE_FUNCTIONS_DECLARE_DEFINE( \
            ClassName, \
            inline, \
            SharemindModuleApiError, \
            SHAREMIND_MODULE_API_OK,) \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DECLARE_DEFINE( \
            ClassName, \
            Oom, \
            inline, \
            SHAREMIND_MODULE_API_OUT_OF_MEMORY, \
            "Out of memory!",) \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DECLARE_DEFINE( \
            ClassName, \
            Oor, \
            inline, \
            SHAREMIND_MODULE_API_IMPLEMENTATION_LIMITS_REACHED, \
            "Implementation limits reached!",) \
    SHAREMIND_LASTERROR_PRIVATE_SHORTCUT_DECLARE_DEFINE( \
            ClassName, \
            Mie, \
            inline, \
            SHAREMIND_MODULE_API_MUTEX_ERROR, \
            "Mutex initialization error!",)

#define SHAREMIND_LIBMODAPI_LASTERROR_FUNCTIONS_DEFINE(ClassName) \
    SHAREMIND_LASTERROR_PUBLIC_FUNCTIONS_DEFINE( \
            ClassName,, \
            SharemindModuleApiError, \
            SHAREMIND_MODULE_API_OK)

#endif /* SHAREMIND_LIBMODAPI_LASTERROR_H */

