#pragma once

inline uint64_t xs64(uint64_t& state)
{
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 0x2545F4914F6CDD1DULL;
}

inline uint32_t xs32(uint64_t& state)
{
    return xs64(state) >> 32;
}

inline void xsPertubeSeed(uint64_t& seed, uint64_t pertube)
{
    seed ^= seed == pertube ? 0 : pertube;
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
}