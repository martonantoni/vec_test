﻿#include <array>
#include <iostream>
#include <algorithm>
#include <vector>

#include "../common/allocation_counter.h"
#include "../common/progress_bar.h"
#include "../common/performance_measurer.h"
#include "../common/xorshift.h"
#include "../common/turbo_list.h"


//template<class T> using TestedVec = std::vector<T>;
template<class T> using TestedVec = TurboList<T>;

cLogPerformance_Guard performanceGuard("main");

template<template<class...> class C> concept HeapContainer =
    requires(C<uint64_t> vec)
{
    { vec.push_back(0) } -> std::same_as<void>;
    { vec.pop_back() } -> std::same_as<void>;
    { vec[0] } -> std::same_as<uint64_t&>;
};
static_assert(HeapContainer<TestedVec>);

template<class T, auto Compare>
class tHeap
{
    TestedVec<T> mHeap;
    size_t mContainerSize = 1;
    static consteval size_t root() { return 1; }
    static size_t leftChild(size_t index) { return 2 * index; }
    static size_t rightChild(size_t index) { return 2 * index + 1; }
    static size_t parent(size_t index) { return index / 2; }
public:
    tHeap() { mHeap.push_back(T{}); }
    const T& top() const { return mHeap[root()]; }
    void push(const T& value);
    void pop();
    size_t size() const { return mContainerSize - 1; }
    bool empty() const { return size() == 0; }
};

template<class T, auto Compare> void tHeap<T, Compare>::push(const T& value)
{
    mHeap.push_back(value);
    ++mContainerSize;
    size_t index = mContainerSize - 1;
    while (index != root() && Compare(mHeap[index], mHeap[parent(index)]))
    {
        std::swap(mHeap[index], mHeap[parent(index)]);
        index = parent(index);
    }
}

template<class T, auto Compare> void tHeap<T, Compare>::pop()
{
    std::swap(mHeap[root()], mHeap[mContainerSize - 1]);
    mHeap.pop_back();
    --mContainerSize;
    size_t index = root();
    size_t lastIndex = mContainerSize - 1;
    for(;;)
    {
        size_t left = leftChild(index);
        size_t right = rightChild(index);
        if(left > lastIndex)
            break;
        if(right > lastIndex)
        {
            if(Compare(mHeap[left], mHeap[index]))
                std::swap(mHeap[left], mHeap[index]);
            break;
        }
        if(Compare(mHeap[left], mHeap[index]))
        {
            if(Compare(mHeap[left], mHeap[right]))
            {
                std::swap(mHeap[left], mHeap[index]);
                index = left;
            }
            else
            {
                std::swap(mHeap[right], mHeap[index]);
                index = right;
            }
        }
        else if(Compare(mHeap[right], mHeap[index]))
        {
            std::swap(mHeap[right], mHeap[index]);
            index = right;
        }
        else
            break;
    }
}

class cSolver
{
    tHeap<uint64_t, std::greater<>{} > mLeft;
    tHeap<uint64_t, std::less<>{}> mRight;
public:
    void push(uint64_t value);
    uint64_t median() const;
};

void cSolver::push(uint64_t value)
{
    if(mLeft.empty() || mLeft.top() >= value)
        mLeft.push(value);
    else
        mRight.push(value);
    if(mLeft.size() > mRight.size() + 1)
    {
        mRight.push(mLeft.top());
        mLeft.pop();
    }
    else if(mRight.size() > mLeft.size())
    {
        mLeft.push(mRight.top());
        mRight.pop();
    }
}

uint64_t cSolver::median() const
{
    if(mLeft.size() == mRight.size())
        return (mLeft.top() + mRight.top()) / 2;
    return mLeft.top();
}

int main()
{
    constexpr int N = 100'000'000;
    cSolver solver;
    uint64_t seed = 123456789;
    uint64_t checksum = 0;
    {
        cProgressBar progressBar(N);
        for (int i = 0; i < N; ++i)
        {
            solver.push(xs64(seed));
            checksum += solver.median();
            progressBar.step();
        }
    }
    std::cout << "Checksum.... ";
    if (checksum != 0x7ecf6291484bbe9)
    {
        std::cout << "FAILED\nexpected 0x7ecf6291484bbe9, got " << std::hex << checksum << std::dec << std::endl;
    }
    else
    {
        std::cout << "PASSED\n";
    }
}