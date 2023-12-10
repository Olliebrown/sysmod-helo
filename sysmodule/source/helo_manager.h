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

#include <atomic>
#include <syshelo.h>

#include "config.h"
#include <nxExt/cpp/lockable_mutex.h>

class HeloManager
{
  public:
    HeloManager();
    virtual ~HeloManager();

    SysHeloContext GetCurrentContext();
    Config* GetConfig();

    void SetRunning(bool running);
    bool Running();
    void Tick();
    void WaitForNextTick();

  protected:
    bool RefreshContext();

    std::atomic_bool running;
    LockableMutex contextMutex;
    Config* config;
    SysHeloContext* context;
};
