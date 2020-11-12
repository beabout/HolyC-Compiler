.data
str_7: .asciz "hello\n"
str_6: .asciz "\n"
str_5: .asciz "\n"
str_3: .asciz "\n"
str_4: .asciz "\n"
str_2: .asciz "\n"
str_1: .asciz "\n"
str_0: .asciz "\n"
.align 8
.globl main
.text
Jumble:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $64, %rsp
movq %rdi, -24(%rbp)
movq %rsi, -32(%rbp)
movq %rdx, -40(%rbp)
movq %rcx, -48(%rbp)
movq %r8, -56(%rbp)
movq %r9, -64(%rbp)
movq -24(%rbp), %rdi
callq printInt
movq $str_0, %rdi
callq printString
movq -32(%rbp), %rdi
callq printInt
movq $str_1, %rdi
callq printString
movq -40(%rbp), %rdi
callq printInt
movq $str_2, %rdi
callq printString
movq -48(%rbp), %rdi
callq printInt
movq $str_3, %rdi
callq printString
movq -56(%rbp), %rdi
callq printInt
movq $str_4, %rdi
callq printString
movq -64(%rbp), %rdi
callq printInt
movq $str_5, %rdi
callq printString
movq 0(%rbp), %rdi
callq printInt
movq $str_6, %rdi
callq printString
jmp lbl_0
lbl_0: addq $64, %rsp
popq %rbp
retq
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
movq $12, %rdi
movq $23, %rsi
movq $34, %rdx
movq $43, %rcx
movq $51, %r8
movq $63, %r9
movq $75, %r11
pushq %r11
callq Jumble
movq $str_7, %rdi
callq printString
movq $0, %rax
jmp lbl_1
lbl_1: addq $16, %rsp
popq %rbp
retq
