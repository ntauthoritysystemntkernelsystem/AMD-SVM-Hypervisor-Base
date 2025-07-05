#include "vmcb.h"
#include <intrin.h>
#include <cstdint>

extern "C" void panic();
extern "C" void HandleMsrExit(Vmcb*);
extern "C" void __svm_vmrun(Vmcb*);

extern "C" void VmExitHandler(Vmcb* vmcb) {
    auto& s = vmcb->state;

    switch (s.exit_code) {
        case 0x72:  // CPUID
        case 0x7A:  // IOIO
        case 0xF4:  // HLT
            s.rip = s.next_rip = s.rip + s.exit_instruction_length;
            break;
        case 0x0C:  // RDMSR
        case 0x3F:  // WRMSR
            HandleMsrExit(vmcb);
            s.rip = s.next_rip = s.rip + s.exit_instruction_length;
            break;
        default:
            panic();
    }

    __svm_vmrun(vmcb);
}