#pragma once

#include "types.h"
#include "../config.h"
#include "../ipc.h"

bool sysheloIpcRunning();
Result sysheloIpcInitialize(void);
void sysheloIpcExit(void);

Result sysheloIpcGetAPIVersion(u32* out_ver);
Result sysheloIpcGetVersionString(char* out, size_t len);
Result sysheloIpcGetCurrentContext(SysHeloContext* out_context);
Result sysheloIpcSetEnabled(bool enabled);
Result sysheloIpcExitCmd();
Result sysheloIpcGetConfigValues(SysHeloConfigValueList* out_configValues);
Result sysheloIpcSetConfigValues(SysHeloConfigValueList* configValues);
