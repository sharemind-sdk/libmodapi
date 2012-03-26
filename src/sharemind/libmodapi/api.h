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

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {

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


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_API_H */
