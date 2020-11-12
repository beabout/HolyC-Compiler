.data
.align 8
.globl main
.text
upper:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $32, %rsp
movq -24(%rbp), %rax
movq $5, %rbx
cmpq %rbx, %rax
setg %al
movb %al, -32(%rbp)
movb -32(%rbp), %al
cmpb $0, %al
je lbl_1
jmp lbl_0
jmp lbl_2
lbl_1: nop
movq -24(%rbp), %rdi
callq printInt
movq -24(%rbp), %rax
movq $1, %rbx
addq %rax, %rbx
movq %rbx, -24(%rbp)
movq -24(%rbp), %rsi
callq upper
jmp lbl_0
lbl_2: nop
lbl_0: addq $32, %rsp
popq %rbp
retq
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
movq $1, %rsi
callq upper
lbl_3: addq $16, %rsp
popq %rbp
retq
