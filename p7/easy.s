.data
.align 8
.globl main
.text
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $32, %rsp
movq $23, %rax
movq $3, %rbx
subq %rax, %rbx
movq %rbx, -32(%rbp)
movq -32(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rdi
callq printInt
lbl_0: addq $32, %rsp
popq %rbp
retq
