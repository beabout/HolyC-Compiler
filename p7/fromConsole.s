.data
str_6: .asciz "\n"
str_5: .asciz "result = b && true: "
str_0: .asciz "give me a bool value: "
str_2: .asciz "\n"
str_3: .asciz "result: "
str_1: .asciz "b: "
str_4: .asciz "\n"
.align 8
.globl main
.text
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $48, %rsp
movq %rdi, -40(%rbp)
movq $str_0, %rdi
callq printString
callq getBool
movq %rax, -24(%rbp)
movq $str_1, %rdi
callq printString
movq -24(%rbp), %rdi
callq printBool
movq $str_2, %rdi
callq printString
movq $48, %rax
movq %rax, -32(%rbp)
movq $str_3, %rdi
callq printString
movq -32(%rbp), %rdi
callq printBool
movq $str_4, %rdi
callq printString
movq -24(%rbp), %rax
movq $1, %rbx
andq %rax, %rbx
movq %rbx, -48(%rbp)
movq -48(%rbp), %rax
movq %rax, -32(%rbp)
movq $str_5, %rdi
callq printString
movq -32(%rbp), %rdi
callq printBool
movq $str_6, %rdi
callq printString
lbl_0: addq $48, %rsp
popq %rbp
retq
