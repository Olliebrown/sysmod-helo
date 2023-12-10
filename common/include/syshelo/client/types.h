#pragma once

#ifdef __SWITCH__

#include <switch/types.h>
#include <switch/result.h>

#else

#define R_FAILED(res) ((res) != 0)
#define R_SUCCEEDED(res) ((res) == 0)

typedef std::uint32_t Result;
typedef std::uint32_t u32;
typedef std::int32_t s32;
typedef std::uint64_t u64;
typedef std::uint8_t u8;

#endif
