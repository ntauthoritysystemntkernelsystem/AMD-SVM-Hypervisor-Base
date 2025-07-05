#pragma once
#include <cstdint>
#include <cstddef>

constexpr uint64_t
    MSR_EFER      = 0xC0000080ULL,
    MSR_VM_CR     = 0xC0010114ULL,
    MSR_HSAVE_PA  = 0xC0010117ULL,
    MSR_N_CR3     = 0xC0010116ULL,
    NP_ENABLE     = 1ULL << 0;

#pragma pack(push, 1)
struct ControlArea {
    uint32_t intercept_cr_read, intercept_cr_write;
    uint32_t intercept_dr_read, intercept_dr_write;
    uint32_t intercept_exceptions;
    uint64_t intercept_instruction1, intercept_instruction2;

    uint32_t iopm_base_pa;
    uint32_t reserved_io;
    uint32_t msrpm_base_pa;
    uint32_t reserved_msr;

    uint64_t tsc_offset;
    uint32_t asid;
    uint8_t  tlb_control, reserved0[3];
    uint64_t nested_ctl;
    uint64_t avic_apic_bar, vh_save_pa;
    uint8_t  reserved1[1];
    uint64_t vmcb_clean_bits;
    uint8_t  reserved2[
        0x400 - (offsetof(ControlArea, vmcb_clean_bits) + sizeof(uint64_t))
    ];
};

struct StateSaveArea {
    uint16_t es_sel, es_attr; uint32_t es_limit; uint64_t es_base;
    uint16_t cs_sel, cs_attr; uint32_t cs_limit; uint64_t cs_base;
    uint16_t ss_sel, ss_attr; uint32_t ss_limit; uint64_t ss_base;
    uint16_t ds_sel, ds_attr; uint32_t ds_limit; uint64_t ds_base;
    uint16_t fs_sel, fs_attr; uint32_t fs_limit; uint64_t fs_base;
    uint16_t gs_sel, gs_attr; uint32_t gs_limit; uint64_t gs_base;

    uint16_t gdtr_limit, ldtr_limit, idtr_limit, pad;
    uint64_t gdtr_base, ldtr_base, idtr_base;

    uint64_t rip, rsp, rflags;
    uint64_t cr0, cr2, cr3, cr4, efer;

    uint32_t exit_code, exit_info1;
    uint64_t exit_info2;
    uint64_t exit_int_info, exit_int_info_err;
    uint32_t exit_instruction_length, pad_len;

    uint64_t next_rip;
    uint64_t int_ctl, int_vector;
    uint64_t int_state, int_shadow, exit_interrupt_vector;

    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    uint8_t reserved3[
        0xC00 - (offsetof(StateSaveArea, r15) + sizeof(uint64_t))
    ];
};

struct alignas(4096) Vmcb {
    ControlArea control;
    StateSaveArea state;
};

static_assert(sizeof(Vmcb) == 0x1000, "VMCB must be 4 KiB");
static_assert(offsetof(Vmcb, state) == 0x400, "StateSaveArea must be at offset 0x400");
#pragma pack(pop)