#include "ipc_service.h"
#include <cstring>
#include <switch.h>
#include "file_utils.h"
#include "errors.h"

IpcService::IpcService(HeloManager* heloMgr)
{
    std::int32_t priority;
    Result rc = svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    ASSERT_RESULT_OK(rc, "svcGetThreadPriority");
    rc = ipcServerInit(&this->server, SYSHELO_IPC_SERVICE_NAME, 42);
    ASSERT_RESULT_OK(rc, "ipcServerInit");
    rc = threadCreate(&this->thread, &IpcService::ProcessThreadFunc, this, NULL, 0x2000, priority, -2);
    ASSERT_RESULT_OK(rc, "threadCreate");

    this->running = false;
    this->heloMgr = heloMgr;
}

void IpcService::SetRunning(bool running)
{
    std::scoped_lock lock{this->threadMutex};
    if(this->running == running)
    {
        return;
    }

    this->running = running;

    if(running)
    {
        FileUtils::LogLine("[ipc] ipcServerProcess: Starting IPC thread");
        Result rc = threadStart(&this->thread);
        ASSERT_RESULT_OK(rc, "threadStart");
    }
    else
    {
        FileUtils::LogLine("[ipc] ipcServerProcess: Stopping IPC thread");
        svcCancelSynchronization(this->thread.handle);
        threadWaitForExit(&this->thread);
    }
}

IpcService::~IpcService()
{
    this->SetRunning(false);
    Result rc = threadClose(&this->thread);
    ASSERT_RESULT_OK(rc, "threadClose");
    rc = ipcServerExit(&this->server);
    ASSERT_RESULT_OK(rc, "ipcServerExit");
}

void IpcService::ProcessThreadFunc(void* arg)
{
    Result rc;
    IpcService* ipcSrv = (IpcService*)arg;
    FileUtils::LogLine("[ipc] ipcServerProcess: Initiating IPC thread event loop");
    while(true)
    {
        rc = ipcServerProcess(&ipcSrv->server, &IpcService::ServiceHandlerFunc, arg);
        if(R_FAILED(rc))
        {
            if(rc == KERNELRESULT(Cancelled))
            {
                return;
            }
            if(rc != KERNELRESULT(ConnectionClosed))
            {
                FileUtils::LogLine("[ipc] ipcServerProcess: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
            }
        }
    }
}

Result IpcService::ServiceHandlerFunc(void* arg, const IpcServerRequest* r, u8* out_data, size_t* out_dataSize)
{
    IpcService* ipcSrv = (IpcService*)arg;

    switch(r->data.cmdId)
    {
        case SysHeloIpcCmd_GetApiVersion:
            *out_dataSize = sizeof(u32);
            return ipcSrv->GetApiVersion((u32*)out_data);

        case SysHeloIpcCmd_GetVersionString:
            if(r->hipc.meta.num_recv_buffers >= 1)
            {
                return ipcSrv->GetVersionString(
                    (char*)hipcGetBufferAddress(r->hipc.data.recv_buffers),
                    hipcGetBufferSize(r->hipc.data.recv_buffers)
                );
            }
            break;

        case SysHeloIpcCmd_GetCurrentContext:
            *out_dataSize = sizeof(SysHeloContext);
            return ipcSrv->GetCurrentContext((SysHeloContext*)out_data);

        case SysHeloIpcCmd_Exit:
            return ipcSrv->Exit();

        case SysHeloIpcCmd_SetEnabled:
            if(r->data.size >= sizeof(std::uint8_t))
            {
                return ipcSrv->SetEnabled((std::uint8_t*)r->data.ptr);
            }
            break;

        case SysHeloIpcCmd_GetConfigValues:
            // TODO: Set out_dataSize to largest possible size returned
            *out_dataSize = sizeof(uint64_t);
            return ipcSrv->GetConfigValues((SysHeloConfigValueList*)out_data);

        case SysHeloIpcCmd_SetConfigValues:
            if(r->data.size >= sizeof(SysHeloConfigValueList))
            {
                return ipcSrv->SetConfigValues((SysHeloConfigValueList*)r->data.ptr);
            }
            break;
    }

    return SYSHELO_ERROR(Generic);
}

Result IpcService::GetApiVersion(u32* out_version)
{
    *out_version = SYSHELO_IPC_API_VERSION;

    return 0;
}

Result IpcService::GetVersionString(char* out_buf, size_t bufSize)
{
    if(bufSize)
    {
        strncpy(out_buf, TARGET_VERSION, bufSize-1);
    }

    return 0;
}

Result IpcService::GetCurrentContext(SysHeloContext* out_ctx)
{
    // Retrieve current context and assign to out_ctx
    *out_ctx = this->heloMgr->GetCurrentContext();

    return 0;
}

Result IpcService::Exit()
{
    // Stop main service
    this->heloMgr->SetRunning(false);

    return 0;
}

Result IpcService::SetEnabled(std::uint8_t* enabled)
{
    // Retrieve config and update enabled value
    Config* config = this->heloMgr->GetConfig();
    config->SetEnabled(*enabled);

    return 0;
}

Result IpcService::GetConfigValues(SysHeloConfigValueList* out_configValues)
{
    // Retrieve config and return config values
    Config* config = this->heloMgr->GetConfig();
    if(!config->HasLoaded())
    {
        return SYSHELO_ERROR(ConfigNotLoaded);
    }

    config->GetConfigValues(out_configValues);
    return 0;
}

Result IpcService::SetConfigValues(SysHeloConfigValueList* configValues)
{
    // Retrieve config and update config values
    Config* config = this->heloMgr->GetConfig();
    if(!config->HasLoaded())
    {
        return SYSHELO_ERROR(ConfigNotLoaded);
    }

    SysHeloConfigValueList configValuesCopy = *configValues;
    if(!config->SetConfigValues(&configValuesCopy, true))
    {
        return SYSHELO_ERROR(ConfigSaveFailed);
    }

    return 0;
}
