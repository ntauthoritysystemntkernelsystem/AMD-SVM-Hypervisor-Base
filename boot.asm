BITS 64
SECTION .bss
  ALIGN 4096
stack_space:  resb 0x10000
stack_top:

SECTION .text
GLOBAL _start
EXTERN kernel_main:PROC

_start:
    mov   rax, stack_top
    and   rax, -16
    mov   rsp, rax

    cli
    mov   eax, 0x08         ; CS
    push  rax
    lea   rax, [rel _start_after_cs]
    push  rax
    retf                    ; far jump to 64-bit CS:RIP

_start_after_cs:
    mov   ax, 0x10
    mov   ds, ax
    mov   es, ax
    mov   ss, ax
    mov   fs, ax
    mov   gs, ax

    call  kernel_main

.halt:
    int3
    ud2
    jmp   .halt