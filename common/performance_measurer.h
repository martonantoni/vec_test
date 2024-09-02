#pragma once

#include <chrono>
#include <iostream>

class cLogPerformance_Guard
{
    std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime = std::chrono::high_resolution_clock::now();
    const char* mName;
public:
    cLogPerformance_Guard(const char* Name) : mName(Name) {}
    ~cLogPerformance_Guard()
    {
        auto EndTime = std::chrono::high_resolution_clock::now();
        auto Elapsed = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - mStartTime);
        std::cout << "\n--- Elapsed " << Elapsed.count() / 1000 << "." << Elapsed.count() % 1000 << " ms in " << mName << " ---\n";
    }
    void waypoint()
    {
        auto EndTime = std::chrono::high_resolution_clock::now();
        auto Elapsed = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - mStartTime);
        std::cout << "+ " << Elapsed.count() / 1000 << "." << Elapsed.count() % 1000 << " ms: ";
    }
};