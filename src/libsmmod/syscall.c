/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define _SHAREMIND_INTERNAL

#include "syscall.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


int SMVM_Syscall_init(SMVM_Syscall * sc, const char * name, void * impl, void * wrapper, SMVM_Module * m) {
    assert(sc);
    assert(name);
    assert(impl);

    if (!SMVM_Module_ref(m))
        return 0;

    SMVM_REFS_INIT(sc);

    sc->name = strdup(name);
    if (!sc->name) {
        SMVM_Module_unref(m);
        return 0;
    }

    if (wrapper) {
        sc->impl_or_wrapper = wrapper;
        sc->null_or_impl = impl;
    } else {
        sc->impl_or_wrapper = impl;
        sc->null_or_impl = NULL;
    }
    sc->module = m;
    return 1;
}

void SMVM_Syscall_destroy(SMVM_Syscall * sc) {
    assert(sc);
    assert(sc->impl_or_wrapper);
    assert(sc->name);
    SMVM_REFS_ASSERT_IF_REFERENCED(sc);

    free(sc->name);
    SMVM_Module_unref(sc->module);
}

const char * SMVM_Syscall_get_name(const SMVM_Syscall * sc) {
    assert(sc);
    return sc->name;
}

SMVM_Module * SMVM_Syscall_get_module(const SMVM_Syscall * sc) {
    assert(sc);
    return sc->module;
}

void * SMVM_Syscall_get_impl_or_wrapper(const SMVM_Syscall * sc) {
    assert(sc);
    return sc->impl_or_wrapper;
}

void * SMVM_Syscall_get_null_or_impl(const SMVM_Syscall * sc) {
    assert(sc);
    return sc->null_or_impl;
}

SMVM_REFS_DEFINE_FUNCTIONS(SMVM_Syscall)
