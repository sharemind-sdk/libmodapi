/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSMVM_PDKMAP_H
#define SHAREMIND_LIBSMVM_PDKMAP_H

#ifndef SHAREMIND_INTERNAL__
#error including an internal header!
#endif


#include "../map.h"
#include "pdk.h"


#ifdef __cplusplus
extern "C" {
#endif


SM_MAP_DECLARE(SMVM_PDKMap,char *,const char * const,SMVM_PDK,)


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBSMVM_PDKMAP_H */
