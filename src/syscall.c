/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "syscall.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "modapi.h"
#include "module.h"


bool SharemindSyscall_init(SharemindSyscall * sc,
                           const char * name,
                           void (* impl)(void),
                           SharemindModule * m)
{
    assert(sc);
    assert(name);
    assert(impl);
    assert(m);
    assert(m->modapi);

    #ifndef NDEBUG
    if (!SharemindModule_refs_ref(m)) {
        SharemindModule_setErrorOor(m);
        goto SharemindSyscall_init_error_0;
    }

    if (!SHAREMIND_RECURSIVE_LOCK_INIT(sc)) {
        SharemindModule_setErrorMie(m);
        goto SharemindSyscall_init_error_1;
    }
    #endif
    SHAREMIND_TAG_INIT(sc);

    sc->name = strdup(name);
    if (!sc->name) {
        SharemindModule_setErrorOom(m);
        goto SharemindSyscall_init_error_2;
    }

    sc->wrapper.callable = (SharemindSyscallCallable) impl;
    sc->wrapper.internal = NULL;
    sc->module = m;
    return true;

SharemindSyscall_init_error_2:

    #ifndef NDEBUG
    SharemindModule_refs_unref(m);
    #endif

SharemindSyscall_init_error_1:

    SHAREMIND_RECURSIVE_LOCK_DEINIT(sc);

SharemindSyscall_init_error_0:

    return false;
}

void SharemindSyscall_destroy(SharemindSyscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
    assert(sc->name);
    assert(sc->module);

    SHAREMIND_TAG_DESTROY(sc);
    free(sc->name);
    #ifndef NDEBUG
    SharemindModule_refs_unref(sc->module);
    #endif
    SHAREMIND_RECURSIVE_LOCK_DEINIT(sc);
}

const char * SharemindSyscall_signature(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->name);
    return sc->name; // No locking: const after SharemindSyscall_init.
}

SharemindModule * SharemindSyscall_module(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->module);
    return sc->module; // No locking: const after SharemindSyscall_init.
}

SharemindModuleApi * SharemindSyscall_moduleApi(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->module);
    assert(sc->module->modapi);
    return sc->module->modapi; // No locking: const after SharemindSyscall_init.
}

/**
  \note According to the AMD64 ABI, returning _SHAREMIND_SyscallWrapper should
        be fast, because this struct will be returned in %rax and %rdx.
*/
SharemindSyscallWrapper SharemindSyscall_wrapper(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
     // No locking: const after SharemindSyscall_init:
    const SharemindSyscallWrapper result = {
        sc->wrapper.callable,
        SharemindModule_handle(sc->module)
    };
    return result;
}

SHAREMIND_TAG_FUNCTIONS_DEFINE(SharemindSyscall,)
