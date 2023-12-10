/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#include <switch.h>
#include <stdexcept>
#include <cstdarg>

#define ERROR_THROW(format, ...) Errors::ThrowException(format "\n  in %s:%u", ##__VA_ARGS__, __FILE__, __LINE__)
#define ERROR_RESULT_THROW(rc, format, ...) ERROR_THROW(format "\n  RC: [0x%x] %04d-%04d", ##__VA_ARGS__, rc, R_MODULE(rc), R_DESCRIPTION(rc))
#define ASSERT_RESULT_OK(rc, format, ...)                                   \
    if (R_FAILED(rc))                                                       \
    {                                                                       \
        ERROR_RESULT_THROW(rc, "ASSERT_RESULT_OK: " format, ##__VA_ARGS__); \
    }
#define ASSERT_ENUM_VALID(n, v)              \
    if(!SYSHELO_ENUM_VALID(n, v)) {           \
        ERROR_THROW("No such %s: %u", #n, v); \
    }

#define SYSHELO_ERROR_MODULE 388
#define SYSHELO_ERROR(desc) ((SYSHELO_ERROR_MODULE & 0x1FF) | (SysHeloError_##desc & 0x1FFF)<<9)

typedef enum
{
    SysHeloError_Generic = 0,
    SysHeloError_ConfigNotLoaded = 1,
    SysHeloError_ConfigSaveFailed = 2,
} SysHeloError;

class Errors
{
  public:
    static void ThrowException(const char* format, ...);

  protected:
    static const char* FormatMessage(const char* format, va_list args);
};
