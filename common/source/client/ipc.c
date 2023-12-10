#define NX_SERVICE_ASSUME_NON_DOMAIN
#include <syshelo/client/ipc.h>
#include <switch.h>
#include <string.h>
#include <stdatomic.h>

static Service g_sysheloSrv;
static atomic_size_t g_refCnt;

bool sysheloIpcRunning()
{
    Handle handle;
    bool running = R_FAILED(smRegisterService(&handle, smEncodeName(SYSHELO_IPC_SERVICE_NAME), false, 1));

    if (!running)
    {
        smUnregisterService(smEncodeName(SYSHELO_IPC_SERVICE_NAME));
    }

  return running;
}

Result sysheloIpcInitialize(void)
{
    Result rc = 0;

    g_refCnt++;

    if (serviceIsActive(&g_sysheloSrv))
        return 0;

    rc = smGetService(&g_sysheloSrv, SYSHELO_IPC_SERVICE_NAME);

    if (R_FAILED(rc)) sysheloIpcExit();

    return rc;
}

void sysheloIpcExit(void)
{
    if (--g_refCnt == 0)
    {
        serviceClose(&g_sysheloSrv);
    }
}

Result sysheloIpcGetAPIVersion(u32* out_ver)
{
    return serviceDispatchOut(&g_sysheloSrv, SysHeloIpcCmd_GetApiVersion, *out_ver);
}

Result sysheloIpcGetVersionString(char* out, size_t len)
{
    return serviceDispatch(&g_sysheloSrv, SysHeloIpcCmd_GetVersionString,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = {{out, len}},
    );
}

Result sysheloIpcGetCurrentContext(SysHeloContext* out_context)
{
    return serviceDispatchOut(&g_sysheloSrv, SysHeloIpcCmd_GetCurrentContext, *out_context);
}

Result sysheloIpcSetEnabled(bool enabled)
{
    u8 enabledRaw = (u8)enabled;
    return serviceDispatchIn(&g_sysheloSrv, SysHeloIpcCmd_SetEnabled, enabledRaw);
}

Result sysheloIpcGetConfigValues(SysHeloConfigValueList* out_configValues)
{
    return serviceDispatchOut(&g_sysheloSrv, SysHeloIpcCmd_GetConfigValues, *out_configValues);
}

Result sysheloIpcSetConfigValues(SysHeloConfigValueList* configValues)
{
    return serviceDispatchIn(&g_sysheloSrv, SysHeloIpcCmd_SetConfigValues, *configValues);
}
