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


int SHAREMIND_Syscall_init(SHAREMIND_Syscall * sc, const char * name, void * impl, SHAREMIND_SyscallCallable wrapper, SHAREMIND_Module * m) {
    assert(sc);
    assert(name);
    assert(impl);
    assert(m);
    assert(m->modapi);

    if (!SHAREMIND_Module_refs_ref(m)) {
        OOR(m->modapi);
        return 0;
    }

    SHAREMIND_REFS_INIT(sc);

    sc->name = strdup(name);
    if (!sc->name) {
        SHAREMIND_Module_refs_unref(m);
        return 0;
    }

    if (wrapper) {
        sc->wrapper.callable = wrapper;
        sc->wrapper.internal = impl;
    } else {
        sc->wrapper.callable = (SHAREMIND_SyscallCallable) impl;
        sc->wrapper.internal = NULL;
    }
    sc->module = m;
    return 1;
}

void SHAREMIND_Syscall_destroy(SHAREMIND_Syscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
    assert(sc->name);
    assert(sc->module);
    SHAREMIND_REFS_ASSERT_IF_REFERENCED(sc);

    free(sc->name);
    SHAREMIND_Module_refs_unref(sc->module);
}

const char * SHAREMIND_Syscall_get_name(const SHAREMIND_Syscall * sc) {
    assert(sc);
    assert(sc->name);
    return sc->name;
}

SHAREMIND_Module * SHAREMIND_Syscall_get_module(const SHAREMIND_Syscall * sc) {
    assert(sc);
    assert(sc->module);
    return sc->module;
}

SHAREMIND_MODAPI * SHAREMIND_Syscall_get_modapi(const SHAREMIND_Syscall * sc) {
    assert(sc);
    assert(sc->module);
    assert(sc->module->modapi);
    return sc->module->modapi;
}

/**
  \note According to the AMD64 ABI, returning _SHAREMIND_SyscallWrapper should be
        fast, because this struct will be returned in %rax and %rdx.
*/
SHAREMIND_SyscallWrapper SHAREMIND_Syscall_get_wrapper(const SHAREMIND_Syscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
    return sc->wrapper;
}

SHAREMIND_REFS_DEFINE_FUNCTIONS(SHAREMIND_Syscall)
