.data
str_6: .asciz "\n"
str_5: .asciz "\n"
str_4: .asciz "\n"
str_3: .asciz "\n"
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
subq $80, %rsp
movq %rdi, %rdi
callq printInt
movq $str_0, %rdi
callq printString
movq %rsi, %rdi
callq printInt
movq $str_1, %rdi
callq printString
movq %rdx, %rdi
callq printInt
movq $str_2, %rdi
callq printString
movq %rcx, %rdi
callq printInt
movq $str_3, %rdi
callq printString
movq %r8, %rdi
callq printInt
movq $str_4, %rdi
callq printString
movq %r9, %rdi
callq printInt
movq $str_5, %rdi
callq printString
movq -48(%rbp), %rdi
callq printInt
movq $str_6, %rdi
callq printString
movq %rdi, %rax
jmp lbl_0
lbl_0: addq $80, %rsp
popq %rbp
retq

main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $32, %rsp
movq $12, %rdi
movq $23, %rsi
movq $34, %rdx
movq $43, %rcx
movq $51, %r8
movq $63, %r9
movq $75, -48(%rbp)
movq $87, -56(%rbp)
callq Jumble
movq %rax, -32(%rbp)
movq -32(%rbp), %rax
movq %rax, -24(%rbp)
movq $0, %rax
jmp lbl_1
lbl_1: addq $32, %rsp
popq %rbp
retq
