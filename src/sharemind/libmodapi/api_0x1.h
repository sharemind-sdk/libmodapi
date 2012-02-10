/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBMODAPI_API_0x1_H
#define SHAREMIND_LIBMODAPI_API_0x1_H

#include <sharemind/codeblock.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef SHAREMIND_INTERNAL__
#define ICONST
#else
#define ICONST const
#endif


/*******************************************************************************
  OVERALL MODULE LEVEL
*******************************************************************************/

#define SHAREMIND_MOD_API_VERSIONS(v) const uint32_t SMVM_MOD_api_versions[] = { v, 0x0 }
#define SHAREMIND_MOD_NAME(n) const char * const SMVM_MOD_name = (n)
#define SHAREMIND_MOD_VERSION(v) const uint32_t SMVM_MOD_version = (v)


/*******************************************************************************
  API 0x1 LEVEL
*******************************************************************************/

/* Forward declarations: */
struct _SMVM_MODAPI_0x1_Module_Context;
struct _SMVM_MODAPI_0x1_Reference;
struct _SMVM_MODAPI_0x1_CReference;
struct _SMVM_MODAPI_0x1_Syscall_Context;
struct _SMVM_MODAPI_0x1_Syscall_Definition;
struct _SMVM_MODAPI_0x1_PD_Conf;
struct _SMVM_MODAPI_0x1_PD_Wrapper;
struct _SMVM_MODAPI_0x1_PDPI_Wrapper;
struct _SMVM_MODAPI_0x1_PDK_Definition;


/** Possible return codes returned by the Sharemind module initializer */
typedef enum {
    SMVM_MODAPI_0x1_IC_OK = 0,
    SMVM_MODAPI_0x1_IC_OUT_OF_MEMORY,
    SMVM_MODAPI_0x1_IC_ERROR
} SMVM_MODAPI_0x1_Initializer_Code;

/** A facility with a context. */
typedef struct {
    void * facility;
    void * context;
} SMVM_MODAPI_0x1_Facility;

/** Environment passed to a Sharemind module initializer and deinitializer: */
typedef struct _SMVM_MODAPI_0x1_Module_Context SMVM_MODAPI_0x1_Module_Context;
struct _SMVM_MODAPI_0x1_Module_Context {

    /** Internal pointer, do not use! */
    ICONST void * ICONST internal;

    /**
      A handle for module instance data. Inside SMVM_syscall_context and others,
      this handle is also passed to facilities provided by this module.
    */
    void * moduleHandle;

    /**
      \brief Finds a module specific system facility.
      \param wrapper Pointer to this SMVM_MODAPI_0x1_Module_Context instance.
      \param[in] name Name of the facility.
      \returns a pointer to the facility and its context.
      \retval NULL if no such facility is associated with this module.
    */
    const SMVM_MODAPI_0x1_Facility * (* ICONST getModuleFacility)(SMVM_MODAPI_0x1_Module_Context * w, const char * name);

};

/** Module initializer function signature: */
typedef SMVM_MODAPI_0x1_Initializer_Code (*SMVM_MODAPI_0x1_Module_Initializer)(SMVM_MODAPI_0x1_Module_Context * c);

/** Module deinitializer function signature: */
typedef void (*SMVM_MODAPI_0x1_Module_Deinitializer)(SMVM_MODAPI_0x1_Module_Context * c);


/*******************************************************************************
  SYSTEM CALLS
*******************************************************************************/

/** Mutable references */
typedef struct _SMVM_MODAPI_0x1_Reference SMVM_MODAPI_0x1_Reference;
struct _SMVM_MODAPI_0x1_Reference {

    /** Internal pointer, do not use! */
    ICONST void * ICONST internal;

    /** Pointer to referenced data. */
    void * ICONST pData;

    /** Size of referenced data. */
    ICONST size_t size;

};

/** Constant references */
typedef struct _SMVM_MODAPI_0x1_CReference SMVM_MODAPI_0x1_CReference;
struct _SMVM_MODAPI_0x1_CReference {

    /** Internal pointer, do not use! */
    ICONST void * ICONST internal;

    /** Pointer to referenced data. */
    const void * ICONST pData;

    /** Size of referenced data. */
    ICONST size_t size;

};

/** Possible return codes returned by system calls */
typedef enum {
    SMVM_MODAPI_0x1_SC_OK = 0x00,
    SMVM_MODAPI_0x1_SC_OUT_OF_MEMORY = 0x01,
    SMVM_MODAPI_0x1_SC_INVALID_CALL = 0x02,
    SMVM_MODAPI_0x1_SC_GENERAL_FAILURE = 0x03
} SMVM_MODAPI_0x1_Syscall_Code;

/** Additional context provided for system calls: */
typedef struct _SMVM_MODAPI_0x1_Syscall_Context SMVM_MODAPI_0x1_Syscall_Context;
struct _SMVM_MODAPI_0x1_Syscall_Context {

    /** Internal pointer, do not use! */
    ICONST void * ICONST internal;

    /** Internal pointer, do not use! */
    ICONST void * ICONST libmodapi_internal;

    /**
      A handle to the private data of the module instance. This is the same
      handle as provided to SMVM_MODAPI_0x1_Module_Context on module
      initialization.
    */
    void * ICONST moduleHandle;

    /**
      Used to get access to internal data of protection domain per-process data
      (see below for pdProcessHandle).
      \param[in] c context
      \param[in] pd_index the protection domain index.
      \param[out] processHandle Where to save the pointer to protection domain per-process data.
      \param[out] pdk_index Where to save the index of the protection domain kind in the module.
      \param[out] moduleHandle Where to save the pointer to protection domain module data.
      \returns an error code or 0 on success.
    */
    int (* ICONST get_pd_process_instance_handle)(SMVM_MODAPI_0x1_Syscall_Context * c,
                                                  uint64_t pd_index,
                                                  void ** pdProcessHandle,
                                                  size_t * pdkIndex,
                                                  void ** moduleHandle);

    /** Access to public dynamic memory inside the VM process: */
    uint64_t (* ICONST publicAlloc)(SMVM_MODAPI_0x1_Syscall_Context * c, size_t nBytes);
    int (* ICONST publicFree)(SMVM_MODAPI_0x1_Syscall_Context * c, uint64_t ptr);
    size_t (* ICONST publicMemPtrSize)(SMVM_MODAPI_0x1_Syscall_Context * c, uint64_t ptr);
    void * (* ICONST publicMemPtrData)(SMVM_MODAPI_0x1_Syscall_Context * c, uint64_t ptr);

    /** Access to dynamic memory not exposed to VM instructions: */
    void * (* ICONST allocPrivate)(SMVM_MODAPI_0x1_Syscall_Context * c, size_t nBytes);
    int (* ICONST freePrivate)(SMVM_MODAPI_0x1_Syscall_Context * c, void * ptr);
    int (* ICONST reservePrivate)(SMVM_MODAPI_0x1_Syscall_Context * c, size_t nBytes);
    int (* ICONST releasePrivate)(SMVM_MODAPI_0x1_Syscall_Context * c, size_t nBytes);

    /* OTHER STUFF */

};

/** System call function signature: */
typedef SMVM_MODAPI_0x1_Syscall_Code (* SMVM_MODAPI_0x1_Syscall)(
    /**
      Pointer to array of regular arguments passed to syscall.
      \warning might be NULL if num_args is zero.
    */
    SHAREMIND_CodeBlock * args,

    /**
      Number of regular arguments given to syscall.
    */
    size_t num_args,

    /**
      Pointer to array of mutable references passed to syscall. NULL if no
      references were given, otherwise an array terminated by a reference with
      the pData field set to NULL, i.e. the array contains at minimum one item
      and the terminator.
    */
    const SMVM_MODAPI_0x1_Reference * refs,

    /**
      Pointer to array of immutable references passed to syscall. NULL if no
      references were given, otherwise an array terminated by a reference with
      the pData field set to NULL, i.e. the array contains at minimum one item
      and the terminator.
    */
    const SMVM_MODAPI_0x1_CReference * crefs,

    /**
      The pointer to where the return value of the syscall should be written, or
      NULL if no return value is expected:
    */
    SHAREMIND_CodeBlock * returnValue,

    /** Additional system call context. */
    SMVM_MODAPI_0x1_Syscall_Context * c
);

/** System call list item:*/
typedef const struct _SMVM_MODAPI_0x1_Syscall_Definition SMVM_MODAPI_0x1_Syscall_Definition;
struct _SMVM_MODAPI_0x1_Syscall_Definition {

    /** Unique name of the system call: */
    const char * const name;

    /** Pointer to the system call implementation: */
    const SMVM_MODAPI_0x1_Syscall syscall_f;

};

/** System call list: */
typedef SMVM_MODAPI_0x1_Syscall_Definition SMVM_MODAPI_0x1_Syscall_Definitions[];


/*******************************************************************************
  PROTECTION DOMAINS
*******************************************************************************/

/** Protection domain configuration */
typedef struct _SMVM_MODAPI_0x1_PD_Conf SMVM_MODAPI_0x1_PD_Conf;
struct _SMVM_MODAPI_0x1_PD_Conf {

    /** The unique name of the protection domain. */
    const char * ICONST pd_name;

    /**
      The index of the protection domain kind in the
      SMVM_MODAPI_0x1_PDK_Definitions list of the module.
    */
    ICONST size_t pdk_index;

    /** The protection domain configuration string. */
    const char * ICONST pd_conf_string;

};

/** Protection-domain instance specific data wrapper. */
typedef struct _SMVM_MODAPI_0x1_PD_Wrapper SMVM_MODAPI_0x1_PD_Wrapper;
struct _SMVM_MODAPI_0x1_PD_Wrapper {

    /** Internal pointer, do not use! */
    ICONST void * ICONST internal;

    /** A handle for protection domain runtime data. */
    void * pdHandle;

    /**
      A handle to the private data of the module instance. This is the same
      handle as provided to SMVM_MODAPI_0x1_Module_Context on module
      initialization.
    */
    void * ICONST moduleHandle;

    /** A handle to the configuration of the protection domain. */
    const SMVM_MODAPI_0x1_PD_Conf * ICONST conf;

    /**
      \brief Finds a protection-domain specific system facility.
      \param wrapper Pointer to this SMVM_MODAPI_0x1_PD_Wrapper instance.
      \param[in] name Name of the facility.
      \returns a pointer to the facility and its context.
      \retval NULL if no such facility is associated with this protection domain.
    */
    const SMVM_MODAPI_0x1_Facility * (* ICONST getPdFacility)(SMVM_MODAPI_0x1_PD_Wrapper * w, const char * name);

    /* OTHER STUFF */

};

/** Protection-domain instance process instance specific data wrapper. */
typedef struct _SMVM_MODAPI_0x1_PDPI_Wrapper SMVM_MODAPI_0x1_PDPI_Wrapper;
struct _SMVM_MODAPI_0x1_PDPI_Wrapper {

    /** Internal pointer, do not use! */
    ICONST void * ICONST internal;

    /** A handle for protection domain per-process data. */
    void * pdProcessHandle;

    /**
      A handle for protection domain instance data. This is the same handle as
      provided to SMVM_MODAPI_0x1_PD_Wrapper on protection domain initialization.
    */
    void * ICONST pdHandle;

    /**
      \brief Finds a system facility specific to the protection domain and process.
      \param wrapper Pointer to this SMVM_MODAPI_0x1_PDPI_Wrapper instance.
      \param[in] name Name of the facility.
      \returns a pointer to the facility and its context.
      \retval NULL if no such facility is associated with this protection domain process instance.
    */
    const SMVM_MODAPI_0x1_Facility * (* ICONST getPdpiFacility)(SMVM_MODAPI_0x1_PDPI_Wrapper * w, const char * name);

    /* OTHER STUFF */

};

/** Protection domain initialization function signature */
typedef int (* SMVM_MODAPI_0x1_PD_Startup)(SMVM_MODAPI_0x1_PD_Wrapper *);

/** Protection domain deinitialization function signature */
typedef void (* SMVM_MODAPI_0x1_PD_Shutdown)(SMVM_MODAPI_0x1_PD_Wrapper *);

/** Protection domain process initialization function signature */
typedef int (* SMVM_MODAPI_0x1_PDPI_Startup)(SMVM_MODAPI_0x1_PDPI_Wrapper *);

/** Protection domain process deinitialization function signature */
typedef void (* SMVM_MODAPI_0x1_PDPI_Shutdown)(SMVM_MODAPI_0x1_PDPI_Wrapper *);

/** Protection domain kind list item: */
typedef const struct _SMVM_MODAPI_0x1_PDK_Definition SMVM_MODAPI_0x1_PDK_Definition;
struct _SMVM_MODAPI_0x1_PDK_Definition {

    /** Unique name of the protection domain kind: */
    const char * const name;

    /** Pointer to the protection domain initialization implementation: */
    const SMVM_MODAPI_0x1_PD_Startup pd_startup_f;

    /** Pointer to the protection domain deinitialization implementation: */
    const SMVM_MODAPI_0x1_PD_Shutdown pd_shutdown_f;

    /** Pointer to the protection domain process initialization implementation: */
    const SMVM_MODAPI_0x1_PDPI_Startup pdpi_startup_f;

    /** Pointer to the protection domain process deinitialization implementation: */
    const SMVM_MODAPI_0x1_PDPI_Shutdown pdpi_shutdown_f;

};

/** Protection domain kind list: */
typedef SMVM_MODAPI_0x1_PDK_Definition SMVM_MODAPI_0x1_PDK_Definitions[];


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_API_0x1_H */
