#include "vmcb.h"
#include <cstdint>
#include <cstring>

alignas(4096) static uint8_t io_bitmap[4096];
extern "C" uint64_t SetupIoBitmap() {
    std::memset(io_bitmap, 0, sizeof(io_bitmap));
    io_bitmap[0x80 >> 3] |= 1u << (0x80 & 7);
    return reinterpret_cast<uint64_t>(io_bitmap);
}

alignas(4096) static uint8_t msr_bitmap[4096];
extern "C" uint64_t SetupMsrBitmap() {
    std::memset(msr_bitmap, 0xFF, sizeof(msr_bitmap));
    auto clear = [](uint32_t msr) {
        if (msr < 0x2000) {
            msr_bitmap[msr >> 3] &= ~(1u << (msr & 7));
        }
    };
    clear(uint32_t(MSR_EFER));
    clear(uint32_t(MSR_VM_CR));
    clear(uint32_t(MSR_HSAVE_PA));
    clear(uint32_t(MSR_N_CR3));
    return reinterpret_cast<uint64_t>(msr_bitmap);
}