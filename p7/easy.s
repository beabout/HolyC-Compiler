.data
str_1: .asciz "in else bih"
str_0: .asciz "wuddup bih"
gbl_hey: .quad 0
.align 8
.globl main
.text
main: nop
fun_main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
movq $1, %rax
movq %rax, (gbl_hey)
movq (gbl_hey), %rax
cmpq $0, %rax
je lbl_1
movq $str_0, %rdi
callq printString
jmp lbl_2
lbl_1: nop
movq $str_1, %rdi
callq printString
lbl_2: nop
lbl_0: addq $16, %rsp
popq %rbp
retq
