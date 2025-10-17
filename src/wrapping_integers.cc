// wrapping_integers.cc
#include "wrapping_integers.hh"
#include <cstdint>
#include <limits>
#include <algorithm>

using namespace std;

static constexpr uint64_t MOD32 = (1ULL << 32);

Wrap32 Wrap32::wrap(uint64_t n, Wrap32 zero_point) {
    // produce seqno = (zero_point + n) mod 2^32
    uint32_t raw = static_cast<uint32_t>( static_cast<uint64_t>(zero_point.raw_value_) + (n & 0xFFFFFFFFULL) );
    return Wrap32{ raw };
}

uint64_t Wrap32::unwrap(Wrap32 zero_point, uint64_t checkpoint) const {
    // Convert this (a 32-bit seqno) -> the absolute 64-bit seqno closest to checkpoint.
    // offset between zero_point and this (mod 2^32)
    uint32_t off32 = static_cast<uint32_t>( raw_value_ - zero_point.raw_value_ );
    uint64_t offset = static_cast<uint64_t>(off32);

    // base is the multiple-of-2^32 chunk containing checkpoint
    uint64_t base = (checkpoint / MOD32) * MOD32;
    uint64_t candidate = base + offset;

    // consider candidate +/- one wrap and pick the absolute closest to checkpoint
    auto dist = [](uint64_t a, uint64_t b) -> uint64_t { return (a > b) ? (a - b) : (b - a); };

    uint64_t best = candidate;
    uint64_t bestd = dist(candidate, checkpoint);

    // candidate - MOD32 (if possible)
    if (candidate >= MOD32) {
        uint64_t c2 = candidate - MOD32;
        uint64_t d2 = dist(c2, checkpoint);
        if (d2 < bestd) { best = c2; bestd = d2; }
    }

    // candidate + MOD32 (watch overflow rarely, but we are in 64-bit)
    if (candidate + MOD32 >= candidate) {
        uint64_t c3 = candidate + MOD32;
        uint64_t d3 = dist(c3, checkpoint);
        if (d3 < bestd) { best = c3; bestd = d3; }
    }

    return best;
}
