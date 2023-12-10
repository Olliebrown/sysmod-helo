#include "helo_manager.h"
#include <cstring>
#include "file_utils.h"
#include "process_management.h"
#include "errors.h"

HeloManager::HeloManager()
{
    this->config = Config::CreateDefault();
    this->context = new SysHeloContext;
    this->context->applicationId = 0;
    this->context->enabled = false;
    this->running = false;
}

HeloManager::~HeloManager()
{
    delete this->config;
    delete this->context;
}

SysHeloContext HeloManager::GetCurrentContext()
{
    std::scoped_lock lock{this->contextMutex};
    return *this->context;
}

Config* HeloManager::GetConfig()
{
    return this->config;
}

void HeloManager::SetRunning(bool running)
{
    this->running = running;
}

bool HeloManager::Running()
{
    return this->running;
}

void HeloManager::Tick()
{
    std::scoped_lock lock{this->contextMutex};
    if (this->RefreshContext() || this->config->Refresh())
    {
    }
}

void HeloManager::WaitForNextTick()
{
    svcSleepThread(this->GetConfig()->GetConfigValue(sysHeloConfigValue_TickIntervalMs) * 1000000ULL);
}

bool HeloManager::RefreshContext()
{
    bool hasChanged = false;

    bool enabled = this->GetConfig()->Enabled();
    if(enabled != this->context->enabled)
    {
        this->context->enabled = enabled;
        FileUtils::LogLine("[mgr] " TARGET " status: %s", enabled ? "enabled" : "disabled");
        hasChanged = true;
    }

    std::uint64_t applicationId = ProcessManagement::GetCurrentApplicationId();
    if (applicationId != this->context->applicationId)
    {
        FileUtils::LogLine("[mgr] TitleID change: %016lX", applicationId);
        this->context->applicationId = applicationId;
        hasChanged = true;
    }

    if(hasChanged)
    {
        this->WaitForNextTick();
    }

    return hasChanged;
}
