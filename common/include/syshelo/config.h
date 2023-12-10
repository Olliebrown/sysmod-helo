#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Configuration values and functions for the sys module
 * 
 * A global file for handling the configuration of your sys module. This is shared between
 * the actual sys module and the overlay and possibly the manager app. Update this file when
 * you need to store different configuration values for later retrieval.
 * 
 * NOTE: Serializing of this data is handled elsewhere.
 **/

// Enumeration of all configuration properties
typedef enum {
    sysHeloConfigValue_TickIntervalMs = 0,
    sysHeloConfigValue_EnumMax
} SysHeloConfigValue;

// Array of all current configuration values (all are uint64 type)
typedef struct {
    uint64_t values[sysHeloConfigValue_EnumMax];
} SysHeloConfigValueList;

// Map a configuration property enum to a string
static inline const char* sysHeloFormatConfigValue(SysHeloConfigValue val, bool pretty)
{
    switch(val)
    {
        case sysHeloConfigValue_TickIntervalMs:
            return pretty ? "Tick Interval (ms)" : "tick_interval_ms";
        default:
            return NULL;
    }
}

// Get default config property value
static inline uint64_t sysHeloDefaultConfigValue(SysHeloConfigValue val)
{
    switch(val)
    {
        case sysHeloConfigValue_TickIntervalMs:
            return 1000ULL;
        default:
            return 0ULL;
    }
}

// Validate a config property value
static inline uint64_t sysHeloValidConfigValue(SysHeloConfigValue val, uint64_t input)
{
    switch(val)
    {
        case sysHeloConfigValue_TickIntervalMs:
            return input > 0;
        default:
            return false;
    }
}
