#pragma once

#include <windows.h>
#include <iostream>

class CEnterWindowsCriticalPriorityMode {
public:
    CEnterWindowsCriticalPriorityMode() {
        hProcess = GetCurrentProcess();
        hThread = GetCurrentThread();

        // Save old priorities
        oldProcPriority = GetPriorityClass(hProcess);
        oldThreadPriority = GetThreadPriority(hThread);

        // Elevate with fallback
        if (!SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS)) {
            std::cerr << "Failed to set HIGH process priority, error " << GetLastError()
                << " — falling back to ABOVE_NORMAL.\n";
            if (!SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS)) {
                std::cerr << "Failed to set ABOVE_NORMAL priority, continuing with default.\n";
            }
        }

        if (!SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL)) {
            std::cerr << "Failed to set TIME_CRITICAL thread priority, error " << GetLastError()
                << " — falling back to HIGHEST.\n";
            if (!SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST)) {
                std::cerr << "Failed to set HIGHEST thread priority, continuing with default.\n";
            }
        }

        std::cout << "CriticalPriority: elevated.\n";
    }

    ~CEnterWindowsCriticalPriorityMode() {
        // Restore old priorities
        if (!SetThreadPriority(hThread, oldThreadPriority)) {
            std::cerr << "Failed to restore thread priority, error " << GetLastError() << "\n";
        }
        if (!SetPriorityClass(hProcess, oldProcPriority)) {
            std::cerr << "Failed to restore process priority, error " << GetLastError() << "\n";
        }
        std::cout << "CriticalPriority: restored.\n";
    }

private:
    HANDLE hProcess{};
    HANDLE hThread{};
    DWORD oldProcPriority{};
    int oldThreadPriority{};
};


//// Example usage
//void RunCriticalLoop()
//{
//    CriticalPriority cp;  // Auto-boosts priority here
//
//    for (int i = 0; i < 10; ++i) {
//        Sleep(100); // Simulated work
//        std::cout << "Working iteration " << i << "\n";
//    }
//
//    // cp goes out of scope here, restoring priority
//}
//
//int main()
//{
//    std::cout << "Program started.\n";
//    RunCriticalLoop();
//    std::cout << "Program finished.\n";
//    return 0;
//}
