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


int SharemindSyscall_init(SharemindSyscall * sc, const char * name, void * impl, SharemindSyscallCallable wrapper, SharemindModule * m) {
    assert(sc);
    assert(name);
    assert(impl);
    assert(m);
    assert(m->modapi);

    if (!SharemindModule_refs_ref(m)) {
        OOR(m->modapi);
        return 0;
    }

    SHAREMIND_REFS_INIT(sc);

    sc->name = strdup(name);
    if (!sc->name) {
        SharemindModule_refs_unref(m);
        return 0;
    }

    if (wrapper) {
        sc->wrapper.callable = wrapper;
        sc->wrapper.internal = impl;
    } else {
        sc->wrapper.callable = (SharemindSyscallCallable) impl;
        sc->wrapper.internal = NULL;
    }
    sc->module = m;
    return 1;
}

void SharemindSyscall_destroy(SharemindSyscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
    assert(sc->name);
    assert(sc->module);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(sc);

    free(sc->name);
    SharemindModule_refs_unref(sc->module);
}

const char * SharemindSyscall_get_signature(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->name);
    return sc->name;
}

SharemindModule * SharemindSyscall_get_module(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->module);
    return sc->module;
}

SharemindModuleApi * SharemindSyscall_get_modapi(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->module);
    assert(sc->module->modapi);
    return sc->module->modapi;
}

/**
  \note According to the AMD64 ABI, returning _SHAREMIND_SyscallWrapper should be
        fast, because this struct will be returned in %rax and %rdx.
*/
SharemindSyscallWrapper SharemindSyscall_get_wrapper(const SharemindSyscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
    return sc->wrapper;
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SharemindSyscall)