.data
str_7: .asciz "\n"
str_3: .asciz "\n"
str_1: .asciz "give me a bool value: "
str_6: .asciz "result: "
str_4: .asciz "b: "
str_5: .asciz "\n"
str_2: .asciz "Here ya go: "
str_0: .asciz "give me a character: "
.align 8
.globl main
.text
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $64, %rsp
movq $str_0, %rdi
callq printString
callq getChar
movq %rax, -32(%rbp)
movq -32(%rbp), %rdi
callq printByte
movq $str_1, %rdi
callq printString
callq getBool
movq %rax, -40(%rbp)
movq $str_2, %rdi
callq printString
movq -40(%rbp), %rdi
callq printByte
movq $str_3, %rdi
callq printString
movq -40(%rbp), %rax
movq $1, %rbx
andq %rax, %rbx
movq %rbx, -56(%rbp)
movq -56(%rbp), %rax
movq %rax, -48(%rbp)
movq $str_4, %rdi
callq printString
movq -40(%rbp), %rdi
callq printByte
movq $str_5, %rdi
callq printString
movq $str_6, %rdi
callq printString
movq -48(%rbp), %rdi
callq printByte
movq $str_7, %rdi
callq printString
lbl_0: addq $64, %rsp
popq %rbp
retq
