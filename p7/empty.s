.data
str_1: .asciz "PASS: Inside false || false if block.\n"
str_0: .asciz "PASS: Inside true && false if block.\n"
gbl_result: .quad 0
.align 8
.globl main
.text
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $48, %rsp
movq $49, %rax
movq %rax, -32(%rbp)
movq $2, %rax
movq %rax, -24(%rbp)
movq $1, %rax
movq $1, %rbx
andq %rax, %rbx
movq %rbx, -40(%rbp)
movb -40(%rbp), %al
cmpb $0, %al
je lbl_1
movq $str_0, %rdi
callq printString
lbl_1: nop
movq $0, %rax
movq $1, %rbx
orq %rax, %rbx
movq %rbx, -48(%rbp)
movb -48(%rbp), %al
cmpb $0, %al
je lbl_2
movq $str_1, %rdi
callq printString
lbl_2: nop
movq -32(%rbp), %rdi
callq printByte
lbl_0: addq $48, %rsp
popq %rbp
retq
