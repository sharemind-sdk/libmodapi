/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#define SHAREMIND_INTERNAL__

#include "pd.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../likely.h"
#include "modapi_0x1.h"
#include "modapi_internal.h"
#include "pd.h"
#include "pdk.h"


SMVM_PD * SMVM_PD_new(SMVM_PDK * pdk, const char * name, const char * conf) {
    assert(pdk);
    assert(pdk->module);
    assert(pdk->module->moduleHandle);
    assert(name);
    assert(name[0]);

    if (!SMVM_PDK_ref(pdk))
        return NULL;

    SMVM_PD * const pd = (SMVM_PD *) malloc(sizeof(SMVM_PD));
    if (unlikely(!pd))
        goto SMVM_PD_new_fail_0;

    pd->name = strdup(name);
    if (unlikely(!pd->name))
        goto SMVM_PD_new_fail_1;

    if (likely(conf && conf[0])) {
        pd->conf = strdup(conf);
        if (!pd->conf)
            goto SMVM_PD_new_fail_2;
    } else {
        pd->conf = NULL;
    }

    pd->pdk = pdk;
    pd->isStarted = false;
    SMVM_REFS_INIT(pd);
    return pd;

SMVM_PD_new_fail_2:

    free(pd->name);

SMVM_PD_new_fail_1:

    free(pd);

SMVM_PD_new_fail_0:

    OOM(pdk->module->modapi);
    SMVM_PDK_unref(pdk);
    return NULL;
}

void SMVM_PD_free(SMVM_PD * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->pdk);
    SMVM_REFS_ASSERT_IF_REFERENCED(pd);

    if (likely(pd->conf))
        free(pd->conf);
    free(pd->name);
    SMVM_PDK_unref(pd->pdk);
    free(pd);
}

static inline void SMVM_PD_init_start_stop_wrappers(SMVM_PD * pd,
                                                    SMVM_MODAPI_0x1_PD_Conf * pdConf,
                                                    SMVM_MODAPI_0x1_PD_Wrapper * pdWrapper)
{
    assert(pd);
    assert(pd->pdk);
    const SMVM_PDK * const pdk = pd->pdk;

    pdConf->pd_name = pd->name;
    pdConf->pdk_index = pdk->pdk_index;
    pdConf->pd_conf_string = pd->conf;

    pdWrapper->pdHandle = pd->pdHandle;
    pdWrapper->moduleHandle = pdk->module->moduleHandle;
    assert(pdWrapper->moduleHandle);
    pdWrapper->conf = pdConf;
}

bool SMVM_PD_is_started(const SMVM_PD * pd) {
    assert(pd);
    return pd->isStarted;
}

bool SMVM_PD_start(SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);
    assert(pd->pdk->module->moduleHandle);

    if (pd->isStarted)
        return true;

    SMVM_MODAPI_0x1_PD_Conf pdConf;
    SMVM_MODAPI_0x1_PD_Wrapper pdWrapper;
    SMVM_PD_init_start_stop_wrappers(pd, &pdConf, &pdWrapper);
    const SMVM_PDK * const pdk = pd->pdk;
    pdWrapper.internal = pdk->pd_startup_null_or_impl;

    const int r = (*((SMVM_MODAPI_0x1_PD_Startup) pdk->pd_startup_impl_or_wrapper))(&pdWrapper);
    if (likely(r == 0)) {
        pd->pdHandle = pdWrapper.pdHandle;
        pd->isStarted = true;
        return true;
    }

    const char * const errorFormatString = "PD startup failed with code %d from the module!";
    const size_t len = strlen(errorFormatString) + sizeof(int) * 3; /* -"%d" + '\0' + '-' + 3 for each byte of int */
    char * const errorString = (char *) malloc(len);
    if (likely(errorString))
        snprintf(errorString, len, errorFormatString, r);
    SMVM_MODAPI_setErrorWithDynamicString(pdk->module->modapi, SMVM_MODAPI_PD_STARTUP_FAILED, errorString);
    return false;

}

void SMVM_PD_stop(SMVM_PD * pd) {
    assert(pd);
    assert(pd->pdk);

    if (!pd->isStarted)
        return;

    SMVM_MODAPI_0x1_PD_Conf pdConf;
    SMVM_MODAPI_0x1_PD_Wrapper pdWrapper;
    SMVM_PD_init_start_stop_wrappers(pd, &pdConf, &pdWrapper);
    const SMVM_PDK * const pdk = pd->pdk;
    pdWrapper.internal = pdk->pd_shutdown_null_or_impl;

    (*((SMVM_MODAPI_0x1_PD_Shutdown) pdk->pd_shutdown_impl_or_wrapper))(&pdWrapper);
    pd->isStarted = false;
}

SMVM_PDK * SMVM_PD_get_pdk(const SMVM_PD * pd) {
    assert(pd);
    return pd->pdk;
}

const char * SMVM_PD_get_name(const SMVM_PD * pd) {
    assert(pd);
    assert(pd->name);
    assert(pd->name[0]);
    return pd->name;
}

bool SMVM_PD_set_name(SMVM_PD * pd, const char * name) {
    assert(pd);
    assert(pd->name);
    assert(pd->name[0]);
    assert(name);
    assert(name[0]);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);

    const size_t newNameLength = strlen(name);
    char * const newName = (char *) realloc(pd->name, newNameLength + 1);
    if (unlikely(!newName)) {
        OOM(pd->pdk->module->modapi);
        return false;
    }

    strcpy(newName, name);
    pd->name = newName;
    return true;
}

const char * SMVM_PD_get_conf(const SMVM_PD * pd) {
    assert(pd);
    assert(pd->conf);
    assert(pd->conf[0]);
    return pd->conf;
}

bool SMVM_PD_set_conf(SMVM_PD * pd, const char * conf) {
    assert(pd);
    assert(pd->pdk);
    assert(pd->pdk->module);
    assert(pd->pdk->module->modapi);

    if (likely(conf && conf[0])) {
        const size_t newConfLength = strlen(conf);
        char * const newConf = (char *) realloc(pd->conf, newConfLength + 1);
        if (unlikely(!newConf)) {
            OOM(pd->pdk->module->modapi);
            return false;
        }

        strcpy(newConf, conf);
        pd->conf = newConf;
    } else if (likely(pd->conf)) {
        free(pd->conf);
        pd->conf = NULL;
    }
    return true;
}

void * SMVM_PD_get_handle(const SMVM_PD * pd) {
    assert(pd);
    return pd->pdHandle;
}

SMVM_REFS_DEFINE_FUNCTIONS(SMVM_PD)
