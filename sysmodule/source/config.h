#pragma once
#include <atomic>
#include <ctime>
#include <map>
#include <mutex>
#include <initializer_list>
#include <string>
#include <switch.h>
#include <minIni.h>
#include <nxExt.h>

#include "syshelo.h"

#define CONFIG_VAL_SECTION "values"

class Config
{
  public:
    Config(std::string path);
    virtual ~Config();

    static Config* CreateDefault();

    bool Refresh();

    void SetEnabled(bool enabled);
    bool Enabled();

    std::uint64_t GetConfigValue(SysHeloConfigValue val);
    const char* GetConfigValueName(SysHeloConfigValue val, bool pretty);
    void GetConfigValues(SysHeloConfigValueList* out_configValues);
    bool SetConfigValues(SysHeloConfigValueList* configValues, bool immediate);

  protected:
    void Load();
    void Close();

    time_t CheckModificationTime();
    static int BrowseIniFunc(const char* section, const char* key, const char* value, void* userdata);

    bool loaded;
    std::string path;
    time_t mtime;
    LockableMutex configMutex;
    LockableMutex overrideMutex;
    std::atomic_bool enabled;
    std::uint64_t configValues[sysHeloConfigValue_EnumMax];
};
