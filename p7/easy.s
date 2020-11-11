.data
str_0: .asciz "wuddup bih"
.align 8
.globl main
.text
main: nop
fun_main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $8, %rsp
movq $1, %rax
movq %rax, -24(%rbp)
movb -24(%rbp), %rax
cmpb $0, %rax
je lbl_1
movq $str_0, %rdi
callq printString
lbl_1: nop
lbl_0: addq $8, %rsp
popq %rbp
retq
