#pragma once

#include <atomic>

template <typename T> class tAllocationTracer : public std::allocator<T>
{
    inline static std::atomic<int> mAllocationCount = 0;
public:
    using value_type = T;

    T* allocate(std::size_t n) 
    {
        ++mAllocationCount;
        return std::allocator<T>::allocate(n);
    }

    void deallocate(T* p, std::size_t n) noexcept 
    {
        std::allocator<T>::deallocate(p, n);
    }

    static int allocationCount() { return mAllocationCount; }
};