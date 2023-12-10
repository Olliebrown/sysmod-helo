#pragma once
#include <syshelo.h>

#include <switch.h>
#include <stdexcept>
#include <cstdarg>

#define ERROR_THROW(format, ...) Errors::ThrowException(format "\n  in %s:%u", ##__VA_ARGS__, __FILE__, __LINE__)
#define ERROR_RESULT_THROW(rc, format, ...) ERROR_THROW(format "\n  RC: [0x%x] %04d-%04d", ##__VA_ARGS__, rc, R_MODULE(rc), R_DESCRIPTION(rc))
#define SYSHELO_ENUM_VALID(n, v) ((v) < n##_EnumMax)
#define ASSERT_RESULT_OK(rc, format, ...)                                   \
    if (R_FAILED(rc))                                                       \
    {                                                                       \
        ERROR_RESULT_THROW(rc, "ASSERT_RESULT_OK: " format, ##__VA_ARGS__); \
    }
#define ASSERT_ENUM_VALID(n, v)              \
    if(!SYSHELO_ENUM_VALID(n, v)) {           \
        ERROR_THROW("No such %s: %u", #n, v); \
    }

class Errors
{
  public:
    static void ThrowException(const char* format, ...);

  protected:
    static const char* FormatMessage(const char* format, va_list args);
};
