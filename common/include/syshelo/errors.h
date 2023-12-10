#pragma once

/**
 * @brief Error codes and macros for the sys module.
 */

#define SYSHELO_ERROR_MODULE 388
#define SYSHELO_ERROR(desc) ((SYSHELO_ERROR_MODULE & 0x1FF) | (SysHeloError_##desc & 0x1FFF)<<9)

typedef enum
{
    SysHeloError_Generic = 0,
    SysHeloError_ConfigNotLoaded = 1,
    SysHeloError_ConfigSaveFailed = 2,
} SysHeloError;
