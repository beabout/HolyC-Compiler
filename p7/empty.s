.data
str_1: .asciz "FART\n"
str_0: .asciz "ASSERT: [AND FAILS].\n"
gbl_result: .quad 0
.align 8
.globl main
.text
giveMeFive:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
movq $5, %rax
movq %rax, (gbl_result)
movq $49, %rax
movq $48, %rbx
andq %rax, %rbx
movq %rbx, -24(%rbp)
movb -24(%rbp), %al
cmpb $0, %al
je lbl_1
movq $str_0, %rdi
callq printString
lbl_1: nop
movq $5, %rax
jmp lbl_0
lbl_0: addq $16, %rsp
popq %rbp
retq
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $32, %rsp
movb $49, %al
cmpb $0, %al
je lbl_3
callq giveMeFive
movq %rax, -32(%rbp)
movq -32(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rax
movq $4, %rbx
addq %rax, %rbx
movq %rbx, -40(%rbp)
movq -40(%rbp), %rdi
callq printInt
jmp lbl_4
lbl_3: nop
movq $str_1, %rdi
callq printString
lbl_4: nop
lbl_2: addq $32, %rsp
popq %rbp
retq
