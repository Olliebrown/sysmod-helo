#pragma once

#include <stdint.h>

// Whenever the IPC commands listed below change, increment this version number
#define SYSHELO_IPC_API_VERSION 1
#define SYSHELO_IPC_SERVICE_NAME "sys:helo"

// Enumeration of custom commands that can be sent over IPC
// NOTE: Client functions for these commands are declared in
//       common/include/syshelo/client/ipc.h and defined in
//       common/source/syshelo/client/ipc.cpp
enum SysHeloIpcCmd
{
    SysHeloIpcCmd_GetApiVersion = 0,
    SysHeloIpcCmd_GetVersionString = 1,
    SysHeloIpcCmd_GetCurrentContext = 2,
    SysHeloIpcCmd_Exit = 3,
    SysHeloIpcCmd_SetEnabled = 4,
    SysHeloIpcCmd_GetConfigValues = 5,
    SysHeloIpcCmd_SetConfigValues = 6
};

// NOTE: Define any structures needed to pass data between processes here
