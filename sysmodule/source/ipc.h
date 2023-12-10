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

#include <stdint.h>

#define SYSHELO_IPC_API_VERSION 1
#define SYSHELO_IPC_SERVICE_NAME "sys:helo"

enum SysHeloIpcCmd
{
    SysHeloIpcCmd_GetApiVersion = 0,
    SysHeloIpcCmd_GetVersionString = 1,
    SysHeloIpcCmd_Exit = 2,
    SysHeloIpcCmd_SetEnabled = 3
};
