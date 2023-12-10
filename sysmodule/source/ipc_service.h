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
#include <nxExt.h>

class IpcService
{
  public:
    IpcService();
    virtual ~IpcService();
    void SetRunning(bool running);

  protected:
    static void ProcessThreadFunc(void* arg);
    static Result ServiceHandlerFunc(void* arg, const IpcServerRequest* r, std::uint8_t* out_data, size_t* out_dataSize);

    Result GetApiVersion(u32* out_version);
    Result GetVersionString(char* out_buf, size_t bufSize);
    Result SetEnabled(std::uint8_t* enabled);
    Result Exit();

    bool running;
    Thread thread;
    LockableMutex threadMutex;
    IpcServer server;
};
