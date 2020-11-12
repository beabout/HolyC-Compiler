.data
str_7: .asciz "ASSERT: [GTE PASSES].\n"
str_8: .asciz "ASSERT: [LTE FAILS].\n"
str_5: .asciz "ASSERT: [NEQ FAILS].\n"
str_9: .asciz "GOODBYE\n"
str_6: .asciz "ASSERT: [OR PASSES].\n"
str_0: .asciz "Hello\n"
str_1: .asciz "ASSERT: [LT PASSES].\n"
str_4: .asciz "ASSERT: [EQ PASSES].\n"
str_3: .asciz "ASSERT: [AND FAILS].\n"
str_2: .asciz "ASSERT: [GT PASSES].\n"
gbl_result: .quad 0
.align 8
.globl main
.text
SayHello:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $80, %rsp
movq $0, %rax
movq %rax, (gbl_result)
movq $str_0, %rdi
callq printString
movq (gbl_result), %rdi
callq printInt
movq $1, %rax
movq $2, %rbx
cmpq %rbx, %rax
setl %al
movb %al, -24(%rbp)
movb -24(%rbp), %al
cmpb $0, %al
je lbl_1
movq $str_1, %rdi
callq printString
lbl_1: nop
movq $4, %rax
movq $3, %rbx
cmpq %rbx, %rax
setg %al
movb %al, -32(%rbp)
movb -32(%rbp), %al
cmpb $0, %al
je lbl_2
movq $str_2, %rdi
callq printString
lbl_2: nop
movq $1, %rax
movq $0, %rbx
andq %rax, %rbx
movq %rbx, -40(%rbp)
movb -40(%rbp), %al
cmpb $0, %al
je lbl_3
movq $str_3, %rdi
callq printString
lbl_3: nop
movq $62, %rax
movq $62, %rbx
cmpq %rax, %rbx
sete %al
movb %al, -48(%rbp)
movb -48(%rbp), %al
cmpb $0, %al
je lbl_4
movq $str_4, %rdi
callq printString
lbl_4: nop
movq $51, %rax
movq $51, %rbx
cmpq %rax, %rbx
setne %al
movb %al, -56(%rbp)
movb -56(%rbp), %al
cmpb $0, %al
je lbl_5
movq $str_5, %rdi
callq printString
lbl_5: nop
movq $1, %rax
movq $0, %rbx
orq %rax, %rbx
movq %rbx, -64(%rbp)
movb -64(%rbp), %al
cmpb $0, %al
je lbl_6
movq $str_6, %rdi
callq printString
lbl_6: nop
movq $62, %rax
movq $52, %rbx
cmpq %rbx, %rax
setge %al
movb %al, -72(%rbp)
movb -72(%rbp), %al
cmpb $0, %al
je lbl_7
movq $str_7, %rdi
callq printString
lbl_7: nop
movq $22, %rax
movq $2, %rbx
cmpq %rbx, %rax
setle %al
movb %al, -80(%rbp)
movb -80(%rbp), %al
cmpb $0, %al
je lbl_8
movq $str_8, %rdi
callq printString
lbl_8: nop
lbl_0: addq $80, %rsp
popq %rbp
retq
SayGoodbye:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
movq $str_9, %rdi
callq printString
lbl_9: addq $16, %rsp
popq %rbp
retq
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $32, %rsp
movb $1, %al
cmpb $0, %al
je lbl_11
callq SayHello
jmp lbl_12
lbl_11: nop
callq SayGoodbye
lbl_12: nop
lbl_10: addq $32, %rsp
popq %rbp
retq
