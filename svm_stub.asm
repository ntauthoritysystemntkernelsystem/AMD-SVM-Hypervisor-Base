.686P
.MODEL FLAT, C
.CODE

PUBLIC __svm_vmrun
__svm_vmrun PROC
    mov   rax, rcx
    vmsave [rax]
    vmload [rax]
    vmrun  [rax]
    ret
__svm_vmrun ENDP
END