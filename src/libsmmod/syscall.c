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


int SMVM_Syscall_init(SMVM_Syscall * sc, const char * name, void * impl, SMVM_SyscallCallable wrapper, SMVM_Module * m) {
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
        sc->wrapper.callable = wrapper;
        sc->wrapper.internal = impl;
    } else {
        sc->wrapper.callable = (SMVM_SyscallCallable) impl;
        sc->wrapper.internal = NULL;
    }
    sc->module = m;
    sc->facilityContext = NULL; /* Just in case */
    SMVM_FacilityMap_init(&sc->syscallFacilityMap, &m->syscallFacilityMap);
    return 1;
}

void SMVM_Syscall_destroy(SMVM_Syscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
    assert(sc->name);
    assert(sc->module);
    SMVM_REFS_ASSERT_IF_REFERENCED(sc);

    SMVM_FacilityMap_destroy(&sc->syscallFacilityMap);

    free(sc->name);
    SMVM_Module_unref(sc->module);
}

const char * SMVM_Syscall_get_name(const SMVM_Syscall * sc) {
    assert(sc);
    assert(sc->name);
    return sc->name;
}

SMVM_Module * SMVM_Syscall_get_module(const SMVM_Syscall * sc) {
    assert(sc);
    assert(sc->module);
    return sc->module;
}

/**
  \note According to the AMD64 ABI, returning _SMVM_SyscallWrapper should be
        fast, because this struct will be returned in %rax and %rdx.
*/
SMVM_SyscallWrapper SMVM_Syscall_get_wrapper(const SMVM_Syscall * sc) {
    assert(sc);
    assert(sc->wrapper.callable);
    return sc->wrapper;
}

SMVM_REFS_DEFINE_FUNCTIONS(SMVM_Syscall)
