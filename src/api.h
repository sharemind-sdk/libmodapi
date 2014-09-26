/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_API_H
#define SHAREMIND_LIBMODAPI_API_H

#include <sharemind/extern_c.h>
#include <stdint.h>


SHAREMIND_EXTERN_C_BEGIN

/**
  \note This struct needs a name, because otherwise we might get this warning
        for modules written in C:
            warning: non-local variable 'sharemindModuleInfo' with anonymous
            type is questionable in C++
*/
typedef struct SharemindModuleInfo_ {

    /** Unique non-empty name of the module (optionally zero-terminated): */
    const char moduleName[64];

    /** Version of the module: */
    const uint32_t moduleVersion;

    /** Non-empty zero-terminated array of up to 15 supported API versions: */
    const uint32_t supportedVersions[16];

} const SharemindModuleInfo;

#define SHAREMIND_MODULE_API_MODULE_INFO(name, version, ...) \
    extern const SharemindModuleInfo sharemindModuleInfo = { \
        (name), (version), { __VA_ARGS__, 0x0 } \
    }

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBMODAPI_API_H */
