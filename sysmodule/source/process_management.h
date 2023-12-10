#pragma once
#include <switch.h>
#include <cstdint>

#define PROCESS_MANAGEMENT_QLAUNCH_TID 0x0100000000001000ULL

/**
 * @brief Functions for managing a switch process
 **/
class ProcessManagement
{
  public:
    static void Initialize();
    static void WaitForQLaunch();
    static std::uint64_t GetCurrentApplicationId();
    static void Exit();
};
