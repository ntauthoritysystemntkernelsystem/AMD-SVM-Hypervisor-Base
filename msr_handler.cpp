#include "vmcb.h"
#include <intrin.h>
#include <cstdint>

extern "C" void panic();

extern "C" void HandleMsrExit(Vmcb* vmcb) {
    auto& s = vmcb->state;
    uint32_t msr = uint32_t(s.exit_info1);

    if (s.exit_code == 0x0C) {
        uint64_t val = __readmsr(msr);
        s.rax = uint32_t(val);
        s.rdx = uint32_t(val >> 32);
    } else if (s.exit_code == 0x3F) {
        uint64_t val = (uint64_t(s.rdx) << 32) | s.rax;
        __writemsr(msr, val);
        if (msr == MSR_EFER)
            vmcb->control.vmcb_clean_bits |= (1ULL << 0);
    } else {
        panic();
    }
}