/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "errors.h"
#include "file_utils.h"

Config::Config(std::string path)
{
    this->path = path;
    this->loaded = false;
    this->mtime = 0;
    this->enabled = false;
    for(unsigned int i = 0; i < sysHeloConfigValue_EnumMax; i++)
    {
        this->configValues[i] = sysHeloDefaultConfigValue((SysHeloConfigValue)i);
    }
}

Config::~Config()
{
    std::scoped_lock lock{this->configMutex};
    this->Close();
}

Config* Config::CreateDefault()
{
    return new Config(FILE_CONFIG_DIR "/config.ini");
}

void Config::Load()
{
    FileUtils::LogLine("[cfg] Reading %s", this->path.c_str());

    this->Close();
    this->mtime = this->CheckModificationTime();
    if(!this->mtime)
    {
        FileUtils::LogLine("[cfg] Error finding file");
    }
    else if (!ini_browse(&BrowseIniFunc, this, this->path.c_str()))
    {
        FileUtils::LogLine("[cfg] Error loading file");
    }

    this->loaded = true;
}

void Config::Close()
{
    this->loaded = false;
    for(unsigned int i = 0; i < sysHeloConfigValue_EnumMax; i++)
    {
        this->configValues[i] = sysHeloDefaultConfigValue((SysHeloConfigValue)i);
    }
}

bool Config::Refresh()
{
    std::scoped_lock lock{this->configMutex};
    if (!this->loaded || this->mtime != this->CheckModificationTime())
    {
        this->Load();
        return true;
    }
    return false;
}

bool Config::HasLoaded()
{
    std::scoped_lock lock{this->configMutex};
    return this->loaded;
}

time_t Config::CheckModificationTime()
{
    time_t mtime = 0;
    struct stat st;
    if (stat(this->path.c_str(), &st) == 0)
    {
        mtime = st.st_mtime;
    }

    return mtime;
}

int Config::BrowseIniFunc(const char* section, const char* key, const char* value, void* userdata)
{
    Config* config = (Config*)userdata;
    std::uint64_t input;
    if(!strcmp(section, CONFIG_VAL_SECTION))
    {
        for(unsigned int kval = 0; kval < sysHeloConfigValue_EnumMax; kval++)
        {
            if(!strcmp(key, sysHeloFormatConfigValue((SysHeloConfigValue)kval, false)))
            {
                input = strtoul(value, NULL, 0);
                if(!sysHeloValidConfigValue((SysHeloConfigValue)kval, input))
                {
                    input = sysHeloDefaultConfigValue((SysHeloConfigValue)kval);
                    FileUtils::LogLine("[cfg] Invalid value for key '%s' in section '%s': using default %d", key, section, input);
                }
                config->configValues[kval] = input;
                return 1;
            }
        }

        FileUtils::LogLine("[cfg] Skipping key '%s' in section '%s': Unrecognized config value", key, section);
        return 1;
    }

    return 1;
}

void Config::SetEnabled(bool enabled)
{
    this->enabled = enabled;
}

bool Config::Enabled()
{
    return this->enabled;
}

std::uint64_t Config::GetConfigValue(SysHeloConfigValue kval)
{
    ASSERT_ENUM_VALID(sysHeloConfigValue, kval);

    std::scoped_lock lock{this->configMutex};

    return this->configValues[kval];
}

const char* Config::GetConfigValueName(SysHeloConfigValue kval, bool pretty)
{
    ASSERT_ENUM_VALID(sysHeloConfigValue, kval);

    const char* result = sysHeloFormatConfigValue(kval, pretty);

    return result;
}

void Config::GetConfigValues(SysHeloConfigValueList* out_configValues)
{
    std::scoped_lock lock{this->configMutex};

    for(unsigned int kval = 0; kval <sysHeloConfigValue_EnumMax; kval++)
    {
        out_configValues->values[kval] = this->configValues[kval];
    }
}

bool Config::SetConfigValues(SysHeloConfigValueList* configValues, bool immediate)
{
    std::scoped_lock lock{this->configMutex};

    // String pointer array passed to ini
    const char* iniKeys[sysHeloConfigValue_EnumMax + 1];
    char* iniValues[sysHeloConfigValue_EnumMax + 1];

    // char arrays to build strings
    char valuesStr[sysHeloConfigValue_EnumMax * 0x20];

    // Iteration pointers
    char* sv = &valuesStr[0];
    const char** ik = &iniKeys[0];
    char** iv = &iniValues[0];

    for(unsigned int kval = 0; kval < sysHeloConfigValue_EnumMax; kval++)
    {
        if(!sysHeloValidConfigValue((SysHeloConfigValue)kval, configValues->values[kval]) || configValues->values[kval] == sysHeloDefaultConfigValue((SysHeloConfigValue)kval))
        {
            continue;
        }

        // Put key and value as string
        // And add them to the ini key/value str arrays
        snprintf(sv, 0x20, "%ld", configValues->values[kval]);
        *ik = sysHeloFormatConfigValue((SysHeloConfigValue)kval, false);
        *iv = sv;

        // We used those chars, get to the next ones
        sv += 0x20;
        ik++;
        iv++;
    }

    *ik = NULL;
    *iv = NULL;

    if(!ini_putsection(CONFIG_VAL_SECTION, (const char**)iniKeys, (const char**)iniValues, this->path.c_str()))
    {
        return false;
    }

    // Only actually apply changes in memory after a successful save
    if(immediate)
    {
        for(unsigned int kval = 0; kval < sysHeloConfigValue_EnumMax; kval++)
        {
            if(sysHeloValidConfigValue((SysHeloConfigValue)kval, configValues->values[kval]))
            {
                this->configValues[kval] = configValues->values[kval];
            }
            else
            {
                this->configValues[kval] = sysHeloDefaultConfigValue((SysHeloConfigValue)kval);
            }
        }
    }

    return true;
}
