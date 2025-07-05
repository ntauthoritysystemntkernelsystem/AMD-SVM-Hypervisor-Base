#include "vmcb.h"
#include <intrin.h>
#include <cstdint>
#include <cstring>

extern "C" void __svm_vmrun(Vmcb*);
extern "C" uint64_t SetupIoBitmap(), SetupMsrBitmap(), SetupPagingAndNested();
extern "C" void panic();

bool IsSvmSupported() {
    int regs[4];
    __cpuid(regs, 0x8000000A);
    return (regs[0] & 1) != 0;
}

void EnableSvm() {
    __writemsr(MSR_EFER, __readmsr(MSR_EFER) | (1ULL << 12));
    __writemsr(MSR_VM_CR, __readmsr(MSR_VM_CR) & ~1ULL);
}

extern "C" void kernel_main() {
    if (!IsSvmSupported()) panic();
    EnableSvm();

    uint64_t nCr3  = SetupPagingAndNested();
    uint64_t io_pa = SetupIoBitmap();
    uint64_t msr_pa= SetupMsrBitmap();

    static Vmcb vmcb;
    static alignas(4096) uint8_t hsave_area[4096];
    std::memset(&vmcb, 0, sizeof(vmcb));

    __writemsr(MSR_HSAVE_PA, reinterpret_cast<uint64_t>(hsave_area));
    __writemsr(MSR_N_CR3, nCr3);

    auto& ctl = vmcb.control;
    ctl.asid = 1;
    ctl.tlb_control = 1;
    ctl.nested_ctl = NP_ENABLE;
    ctl.iopm_base_pa = uint32_t(io_pa);
    ctl.msrpm_base_pa = uint32_t(msr_pa);
    ctl.vmcb_clean_bits = (1ULL<<0) | (1ULL<<1);
    ctl.intercept_instruction1 = (1ULL<<0)|(1ULL<<1);
    ctl.intercept_instruction2 = (1ULL<<2)|(1ULL<<3);

    auto& s = vmcb.state;
    s.cs_sel = 0x08; s.cs_attr = 0xA09B; s.cs_limit = UINT32_MAX; s.cs_base = 0;
    s.ss_sel = 0x10; s.ss_attr = 0xC093; s.ss_limit = UINT32_MAX; s.ss_base = 0;
    s.ds_sel = s.es_sel = 0x10; s.ds_attr = s.es_attr = 0xC093;
    s.fs_sel = s.gs_sel = 0; s.fs_attr = s.gs_attr = 0;
    s.fs_limit = s.gs_limit = 0;

    s.gdtr_base = s.ldtr_base = s.idtr_base = 0;
    s.rflags = 2;
    s.rip = 0x300000;
    s.rsp = 0x400000;
    s.cr0 = __readcr0();
    s.cr3 = __readcr3();
    s.cr4 = __readcr4();
    s.efer = __readmsr(MSR_EFER);

    std::memset(reinterpret_cast<void*>(0x400000), 0, 4096);
    uint8_t* guest = reinterpret_cast<uint8_t*>(0x300000);
    guest[0] = 0x0F; guest[1] = 0xA2; // CPUID
    guest[2] = 0xF4;                 // HLT

    __svm_vmrun(&vmcb);
}