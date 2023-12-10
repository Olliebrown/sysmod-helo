// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

// Utilities for errors and logging
#include "errors.h"
#include "file_utils.h"

// Size of the inner heap (adjust as necessary).
#define INNER_HEAP_SIZE 0x80000

#ifdef __cplusplus
extern "C" {
#endif

// Sysmodules should not use applet*.
u32 __nx_applet_type = AppletType_None;

// Sysmodules will normally only want to use one FS session.
u32 __nx_fs_num_sessions = 1;

// Newlib heap configuration function (makes malloc/free work).
void __libnx_initheap(void)
{
    static u8 inner_heap[INNER_HEAP_SIZE];
    extern void* fake_heap_start;
    extern void* fake_heap_end;

    // Configure the newlib heap.
    fake_heap_start = inner_heap;
    fake_heap_end   = inner_heap + sizeof(inner_heap);
}

// Service initialization.
void __appInit(void)
{
    Result rc;

    // Open a service manager session.
    rc = smInitialize();
    if (R_FAILED(rc)) {
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
    }

    // Retrieve the current version of Horizon OS.
    rc = setsysInitialize();
    if (R_SUCCEEDED(rc)) {
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc)) {
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        }
        setsysExit();
    }
}

// Service deinitialization.
void __appExit(void)
{
    smExit();
}

#ifdef __cplusplus
}
#endif

// Main program entrypoint
int main(int argc, char* argv[])
{
    // File utility initialization
    Result rc = FileUtils::Initialize();
    if (R_FAILED(rc))
    {
        fatalThrow(rc);
        return 1;
    }
    
    try
    {
        // More initialization
        FileUtils::LogLine("Initializing ...");

        // Log that initialization is done
        FileUtils::LogLine("Ready");
        
        // Main loop / core
        // while (true)
        // {
        // }

        // Shutdown & cleanup
    }
    catch (const std::exception &ex)
    {
        FileUtils::LogLine("[!] %s", ex.what());
    }
    catch (...)
    {
        std::exception_ptr p = std::current_exception();
        FileUtils::LogLine("[!?] %s", p ? p.__cxa_exception_type()->name() : "...");
    }

    // Finalize file utilities
    FileUtils::LogLine("Exit");
    svcSleepThread(1000000ULL);
    FileUtils::Exit();

    // Exit success
    return 0;
}
