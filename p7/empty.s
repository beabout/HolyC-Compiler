.data
.align 8
.globl main
lbl_main: bobobobobpushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
lbl_0: add $16, %rsp
popq %rbp
retq
