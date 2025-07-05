#include "vmcb.h"
#include <cstdint>
#include <cstring>
#include <intrin.h>

extern "C" uint64_t SetupPagingAndNested() {
    static alignas(4096) uint64_t pml4[512], pdpt[512], npml4[512], npdpt[512];

    std::memset(pml4, 0, sizeof(pml4));
    std::memset(pdpt, 0, sizeof(pdpt));
    std::memset(npml4, 0, sizeof(npml4));
    std::memset(npdpt, 0, sizeof(npdpt));

    pml4[0] = reinterpret_cast<uint64_t>(pdpt) | 0x3;
    pdpt[0] = 0 | 0x83;

    for (int i = 0; i < 512; i++)
        npdpt[i] = (i << 21) | 0x83;
    npml4[0] = reinterpret_cast<uint64_t>(npdpt) | 0x3;

    __writecr4(__readcr4() | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 10));
    __writemsr(MSR_EFER, __readmsr(MSR_EFER) | (1ULL << 11));
    __writecr3(reinterpret_cast<uint64_t>(pml4));
    __writecr0(__readcr0() | (1ULL << 0) | (1ULL << 31));

    return reinterpret_cast<uint64_t>(npml4);
}
