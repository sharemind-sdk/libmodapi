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
#include <sharemind/preprocessor.h>
#include <stddef.h>
#include "api.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef SHAREMIND_ICONST
#ifdef SHAREMIND_INTERNAL__
#define SHAREMIND_ICONST
#else
#define SHAREMIND_ICONST const
#endif
#endif /* SHAREMIND_ICONST */


/*******************************************************************************
  API 0x1 LEVEL
*******************************************************************************/

/* Forward declarations: */
struct SharemindModuleApi0x1ModuleContext_;
struct SharemindModuleApi0x1Reference_;
struct SharemindModuleApi0x1CReference_;
struct SharemindModuleApi0x1SyscallContext_;
struct SharemindModuleApi0x1SyscallDefinition_;
struct SharemindModuleApi0x1PdConf_;
struct SharemindModuleApi0x1PdWrapper_;
struct SharemindModuleApi0x1PdpiInfo_;
struct SharemindModuleApi0x1PdpiWrapper_;
struct SharemindModuleApi0x1PdkDefinition_;


#if 0
/** Possible return codes returned by the procedures in Sharemind modules. */
typedef enum {

    /** No error. */
    SHAREMIND_MODULE_API_0x1_OK = 0,

    /** A fatal out of memory condition occurred. */
    SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY,

    /** Sharemind acted in an unspecified manner. */
    SHAREMIND_MODULE_API_0x1_SHAREMIND_ERROR,

    /** Programming fault in the module. */
    SHAREMIND_MODULE_API_0x1_MODULE_ERROR,

    /** A general runtime error. */
    SHAREMIND_MODULE_API_0x1_GENERAL_ERROR,

    /** The system call was called improperly by the bytecode. */
    SHAREMIND_MODULE_API_0x1_INVALID_CALL,

    /** A required facility was not provided by Sharemind. */
    SHAREMIND_MODULE_API_0x1_MISSING_FACILITY,

    /** The protection domain configuration given was invalid or erroneous. */
    SHAREMIND_MODULE_API_0x1_INVALID_PD_CONFIGURATION,

    /** The module configuration given was invalid or erroneous. */
    SHAREMIND_MODULE_API_0x1_INVALID_MODULE_CONFIGURATION

} SharemindModuleApi0x1Error;
#endif

#define SHAREMIND_MODULE_API_0x1_ERROR_ENUM \
    ((SHAREMIND_MODULE_API_0x1_OK, = 0)) \
    ((SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY,)) \
    ((SHAREMIND_MODULE_API_0x1_SHAREMIND_ERROR,)) \
    ((SHAREMIND_MODULE_API_0x1_MODULE_ERROR,)) \
    ((SHAREMIND_MODULE_API_0x1_GENERAL_ERROR,)) \
    ((SHAREMIND_MODULE_API_0x1_INVALID_CALL,)) \
    ((SHAREMIND_MODULE_API_0x1_MISSING_FACILITY,)) \
    ((SHAREMIND_MODULE_API_0x1_INVALID_PD_CONFIGURATION,)) \
    ((SHAREMIND_MODULE_API_0x1_INVALID_MODULE_CONFIGURATION,))
SHAREMIND_ENUM_CUSTOM_DEFINE(SharemindModuleApi0x1Error,
                             SHAREMIND_MODULE_API_0x1_ERROR_ENUM);

/** A facility with a context. */
typedef struct {
    void * facility;
    void * context;
} SharemindModuleApi0x1Facility;

/** Environment passed to a Sharemind module initializer and deinitializer: */
typedef struct SharemindModuleApi0x1ModuleContext_
        SharemindModuleApi0x1ModuleContext;

struct SharemindModuleApi0x1ModuleContext_ {

    /** Internal pointer, do not use in modules! Don't! */
    SHAREMIND_ICONST void * SHAREMIND_ICONST internal;

    /**
      A handle for module instance data. Inside SHAREMIND_syscall_context and
      others, this handle is also passed to facilities provided by this module.
    */
    void * moduleHandle;

    /**
      The module configuration string.
      \note Might be NULL if empty.
    */
    const char * SHAREMIND_ICONST conf;

    /**
      \brief Finds a module specific system facility.
      \param wrapper Pointer to the SharemindModuleApi0x1ModuleContext instance.
      \param[in] name Name of the facility.
      \returns a pointer to the facility and its context.
      \retval NULL if no such facility is associated with this module.
    */
    const SharemindModuleApi0x1Facility * (* SHAREMIND_ICONST getModuleFacility)
            (SharemindModuleApi0x1ModuleContext * w,
             const char * name);

};

/** Module initializer function signature: */
typedef SharemindModuleApi0x1Error (*SharemindModuleApi0x1ModuleInitializer)(
                SharemindModuleApi0x1ModuleContext * c);

#define SHAREMIND_MODULE_API_0x1_INITIALIZER(c) \
    SharemindModuleApi0x1Error sharemind_module_api_0x1_module_init( \
            SharemindModuleApi0x1ModuleContext * c)

/** Module deinitializer function signature: */
typedef void (*SharemindModuleApi0x1ModuleDeinitializer)(
                SharemindModuleApi0x1ModuleContext * c);

#define SHAREMIND_MODULE_API_0x1_DEINITIALIZER(c) \
    void sharemind_module_api_0x1_module_deinit( \
            SharemindModuleApi0x1ModuleContext * c)


/*******************************************************************************
  SYSTEM CALLS
*******************************************************************************/

/** Mutable references */
typedef struct SharemindModuleApi0x1Reference_ SharemindModuleApi0x1Reference;
struct SharemindModuleApi0x1Reference_ {

    /** Internal pointer, do not use in modules! Really! */
    SHAREMIND_ICONST void * SHAREMIND_ICONST internal;

    /** Pointer to referenced data. */
    void * SHAREMIND_ICONST pData;

    /** Size of referenced data. */
    SHAREMIND_ICONST size_t size;

};

/** Constant references */
typedef struct SharemindModuleApi0x1CReference_ SharemindModuleApi0x1CReference;
struct SharemindModuleApi0x1CReference_ {

    /** Internal pointer, do not use in modules! We mean it! */
    SHAREMIND_ICONST void * SHAREMIND_ICONST internal;

    /** Pointer to referenced data. */
    const void * SHAREMIND_ICONST pData;

    /** Size of referenced data. */
    SHAREMIND_ICONST size_t size;

};

/**
  PDPI information returned by get_pdpi_info in the system call context.
*/
typedef struct SharemindModuleApi0x1PdpiInfo_ SharemindModuleApi0x1PdpiInfo;
struct SharemindModuleApi0x1PdpiInfo_ {

    /** The PDPI handle. */
    void * pdpiHandle;

    /** The PD handle. */
    void * pdHandle;

    /** The PDK index. */
    size_t pdkIndex;

    /** The module handle. */
    void * moduleHandle;

};

/** Additional context provided for system calls: */
typedef struct SharemindModuleApi0x1SyscallContext_
        SharemindModuleApi0x1SyscallContext;

struct SharemindModuleApi0x1SyscallContext_ {

    /** Internal pointer, do not use in modules! We're warning you! */
    SHAREMIND_ICONST void * SHAREMIND_ICONST vm_internal;

    /** System call specific data, do not use in modules! Ever! */
    void * syscall_internal;

    /** Process specific data. */
    void * SHAREMIND_ICONST process_internal;

    /**
      A handle to the private data of the module instance. This is the same
      handle as provided to SharemindModuleApi0x1ModuleContext on module
      initialization.
    */
    void * moduleHandle;

    /**
      Used to get access to internal data of protection domain per-process data.
      \param[in] c context
      \param[in] pd_index the protection domain index.
      \returns a pointer to a SharemindModuleApi0x1PdpiInfo structure for the
               PDPI.
      \retval NULL if no PDPI was found.
    */
    const SharemindModuleApi0x1PdpiInfo * (* SHAREMIND_ICONST get_pdpi_info)(
            SharemindModuleApi0x1SyscallContext * c,
            uint64_t pd_index);


    /* Access to public dynamic memory inside the VM process: */
    uint64_t (* SHAREMIND_ICONST publicAlloc)(
            SharemindModuleApi0x1SyscallContext * c,
            uint64_t nBytes);

    int (* SHAREMIND_ICONST publicFree)(SharemindModuleApi0x1SyscallContext * c,
                                        uint64_t ptr);

    size_t (* SHAREMIND_ICONST publicMemPtrSize)(
            SharemindModuleApi0x1SyscallContext * c,
            uint64_t ptr);

    void * (* SHAREMIND_ICONST publicMemPtrData)(
            SharemindModuleApi0x1SyscallContext * c,
            uint64_t ptr);


    /* Access to dynamic memory not exposed to VM instructions: */
    void * (* SHAREMIND_ICONST allocPrivate)(
            SharemindModuleApi0x1SyscallContext * c,
            size_t nBytes);

    int (* SHAREMIND_ICONST freePrivate)(
            SharemindModuleApi0x1SyscallContext * c,
            void * ptr);

    int (* SHAREMIND_ICONST reservePrivate)(
            SharemindModuleApi0x1SyscallContext * c,
            size_t nBytes);

    int (* SHAREMIND_ICONST releasePrivate)(
            SharemindModuleApi0x1SyscallContext * c,
            size_t nBytes);

    /* OTHER STUFF */

};

/** System call function signature: */
typedef SharemindModuleApi0x1Error (* SharemindModuleApi0x1Syscall)(
    /**
      Pointer to array of regular arguments passed to syscall.
      \warning might be NULL if num_args is zero.
    */
    SharemindCodeBlock * args,

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
    const SharemindModuleApi0x1Reference * refs,

    /**
      Pointer to array of immutable references passed to syscall. NULL if no
      references were given, otherwise an array terminated by a reference with
      the pData field set to NULL, i.e. the array contains at minimum one item
      and the terminator.
    */
    const SharemindModuleApi0x1CReference * crefs,

    /**
      The pointer to where the return value of the syscall should be written, or
      NULL if no return value is expected:
    */
    SharemindCodeBlock * returnValue,

    /** Additional system call context. */
    SharemindModuleApi0x1SyscallContext * c
);
#define SHAREMIND_MODULE_API_0x1_SYSCALL(name,args,argc,refs,crefs,retVal,c) \
    SharemindModuleApi0x1Error name( \
        SharemindCodeBlock * args, \
        size_t argc, \
        const SharemindModuleApi0x1Reference * refs, \
        const SharemindModuleApi0x1CReference * crefs, \
        SharemindCodeBlock * retVal, \
        SharemindModuleApi0x1SyscallContext * c)

#define SHAREMIND_MODULE_API_0x1_SYSCALL_SIGNATURE_BUFFER_SIZE 256u

/** System call list item:*/
typedef struct {

    /**
      Unique non-empty name of the system call (optionally zero-terminated):
    */
    const char signature[
            SHAREMIND_MODULE_API_0x1_SYSCALL_SIGNATURE_BUFFER_SIZE];

    /** Pointer to the system call implementation: */
    const SharemindModuleApi0x1Syscall fptr;

} const SharemindModuleApi0x1SyscallDefinition;
#define SHAREMIND_MODULE_API_0x1_SYSCALL_DEFINITION(signature,fptr) \
    { (signature), (fptr) }

/** System call list: */
typedef SharemindModuleApi0x1SyscallDefinition const
        SharemindModuleApi0x1SyscallDefinitions[];

#define SHAREMIND_MODULE_API_0x1_SYSCALL_DEFINITIONS(...) \
    extern const SharemindModuleApi0x1SyscallDefinitions \
            sharemindModuleApi0x1SyscallDefinitions = \
    { \
        __VA_ARGS__ , \
        { "", NULL } \
    }


/*******************************************************************************
  PROTECTION DOMAINS
*******************************************************************************/

/** Protection domain configuration */
typedef struct SharemindModuleApi0x1PdConf_ SharemindModuleApi0x1PdConf;
struct SharemindModuleApi0x1PdConf_ {

    /** The unique name of the protection domain. */
    const char * SHAREMIND_ICONST pd_name;

    /**
      The index of the protection domain kind in the
      SharemindModuleApi0x1PdkDefinitions list of the module.
    */
    SHAREMIND_ICONST size_t pdk_index;

    /**
      The protection domain configuration string.
      \note Might be NULL if empty.
    */
    const char * SHAREMIND_ICONST pd_conf_string;

};

/** Protection-domain instance specific data wrapper. */
typedef struct SharemindModuleApi0x1PdWrapper_ SharemindModuleApi0x1PdWrapper;
struct SharemindModuleApi0x1PdWrapper_ {

    /** Internal pointer, do not use in modules! Never ever ever! */
    SHAREMIND_ICONST void * SHAREMIND_ICONST internal;

    /** A handle for protection domain runtime data. */
    void * pdHandle;

    /**
      A handle to the private data of the module instance. This is the same
      handle as provided to SharemindModuleApi0x1ModuleContext on module
      initialization.
    */
    void * SHAREMIND_ICONST moduleHandle;

    /** A handle to the configuration of the protection domain. */
    const SharemindModuleApi0x1PdConf * SHAREMIND_ICONST conf;

    /**
      \brief Finds a protection-domain specific system facility.
      \param wrapper Pointer to this SharemindModuleApi0x1PdWrapper instance.
      \param[in] name Name of the facility.
      \returns a pointer to the facility and its context.
      \retval NULL if no such facility is associated with this protection
                   domain.
    */
    const SharemindModuleApi0x1Facility * (* SHAREMIND_ICONST getPdFacility)(
            SharemindModuleApi0x1PdWrapper * w,
            const char * name);

    /* OTHER STUFF */

};

/** Protection-domain instance process instance specific data wrapper. */
typedef struct SharemindModuleApi0x1PdpiWrapper_
        SharemindModuleApi0x1PdpiWrapper;

struct SharemindModuleApi0x1PdpiWrapper_ {

    /** Internal pointer, do not use in modules! Please! */
    SHAREMIND_ICONST void * SHAREMIND_ICONST internal;

    /** A handle for protection domain per-process data. */
    void * pdProcessHandle;

    /**
      A handle for protection domain instance data. This is the same handle as
      provided to SharemindModuleApi0x1PdWrapper on protection domain
      initialization.
    */
    void * SHAREMIND_ICONST pdHandle;

    /**
      \brief Finds a system facility specific to the protection domain and
             process.
      \param wrapper Pointer to this SharemindModuleApi0x1PdpiWrapper instance.
      \param[in] name Name of the facility.
      \returns a pointer to the facility and its context.
      \retval NULL if no such facility is associated with this protection domain
              process instance.
    */
    const SharemindModuleApi0x1Facility * (* SHAREMIND_ICONST getPdpiFacility)(
            SharemindModuleApi0x1PdpiWrapper * w,
            const char * name);

    /* OTHER STUFF */

};

/** Protection domain initialization function signature */
typedef SharemindModuleApi0x1Error (* SharemindModuleApi0x1PdStartup)(
        SharemindModuleApi0x1PdWrapper *);

#define SHAREMIND_MODULE_API_0x1_PD_STARTUP(name,wrapper) \
    SharemindModuleApi0x1Error name(SharemindModuleApi0x1PdWrapper * wrapper)


/** Protection domain deinitialization function signature */
typedef void (* SharemindModuleApi0x1PdShutdown)(
        SharemindModuleApi0x1PdWrapper *);

#define SHAREMIND_MODULE_API_0x1_PD_SHUTDOWN(name,wrapper) \
    void name(SharemindModuleApi0x1PdWrapper * wrapper)


/** Protection domain process initialization function signature */
typedef SharemindModuleApi0x1Error (* SharemindModuleApi0x1PdpiStartup)(
        SharemindModuleApi0x1PdpiWrapper *);

#define SHAREMIND_MODULE_API_0x1_PDPI_STARTUP(name,wrapper) \
    SharemindModuleApi0x1Error name(SharemindModuleApi0x1PdpiWrapper * wrapper)


/** Protection domain process deinitialization function signature */
typedef void (* SharemindModuleApi0x1PdpiShutdown)(
        SharemindModuleApi0x1PdpiWrapper *);

#define SHAREMIND_MODULE_API_0x1_PDPI_SHUTDOWN(name,wrapper) \
    void name(SharemindModuleApi0x1PdpiWrapper * wrapper)

#define SHAREMIND_MODULE_API_0x1_PDK_NAME_BUFFER_SIZE 256u

/** Protection domain kind list item: */
typedef struct {

    /**
      Unique non-empty name of the protection domain kind (optionally zero-
      terminated):
    */
    const char name[SHAREMIND_MODULE_API_0x1_PDK_NAME_BUFFER_SIZE];

    /** Pointer to the protection domain initialization implementation: */
    const SharemindModuleApi0x1PdStartup pd_startup_f;

    /** Pointer to the protection domain deinitialization implementation: */
    const SharemindModuleApi0x1PdShutdown pd_shutdown_f;

    /**
      Pointer to the protection domain process initialization implementation:
    */
    const SharemindModuleApi0x1PdpiStartup pdpi_startup_f;

    /**
      Pointer to the protection domain process deinitialization implementation:
    */
    const SharemindModuleApi0x1PdpiShutdown pdpi_shutdown_f;

} const SharemindModuleApi0x1PdkDefinition;

#define SHAREMIND_MODULE_API_0x1_PDK_DEFINITION(name,pdC,pdD,pdpiC,pdpiD) \
    { (name), (pdC), (pdD), (pdpiC), (pdpiD) }


/** Protection domain kind list: */
typedef SharemindModuleApi0x1PdkDefinition const
        SharemindModuleApi0x1PdkDefinitions[];

#define SHAREMIND_MODULE_API_0x1_PDK_DEFINITIONS(...) \
    extern const SharemindModuleApi0x1PdkDefinitions \
            sharemindModuleApi0x1PdkDefinitions = \
    { \
        __VA_ARGS__, \
        { "", NULL, NULL, NULL, NULL } \
    }

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* SHAREMIND_LIBMODAPI_API_0x1_H */
