#pragma once

// view-and-pasted with minor fixes from https://www.youtube.com/watch?v=VbwgVNxN_fg 

template<class T> class TurboList
{
    T* old = nullptr;
    T* nex;

    uint32_t mid = 0;
    uint32_t end = 0;
    uint32_t capacity;

    void grow_and_insert(T elem) noexcept
    {
        if (old)
            free(old);
        old = nex;
        mid = end;
        capacity *= 2;
        nex = (T*)malloc(capacity * sizeof(T));
        push_back(elem);
    }
public:
    TurboList() noexcept:
        capacity(16)
    {
        nex = (T*)malloc(capacity * sizeof(T));
    }
    ~TurboList() noexcept
    {
        if (old)
            free(old);
        if(nex)
            free(nex);
    }

    T& operator[](uint32_t index) noexcept
    {
        return (index < mid) ? old[index] : nex[index];
    }
    T& operator[](uint32_t index) const noexcept
    {
        return (index < mid) ? old[index] : nex[index];
    }

    void push_back(T elem) noexcept
    {
        if (end < capacity) [[likely]]
        {
            bool has_mid = mid > 0;
            mid -= has_mid;
            nex[mid] = has_mid ? old[mid] : nex[mid];
            nex[end++] = elem;
        }
        else
        {
            grow_and_insert(elem);
        }
    }
    void pop_back() noexcept
    {
        if (end > 0)
        {
            --end;
            if (end == mid)
                mid = 0;
        }
    }
};