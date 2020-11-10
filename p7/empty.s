.data
.align 8
.globl main
main: nop
fun_main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
lbl_0: add $16, %rsp
popq %rbp
retq
