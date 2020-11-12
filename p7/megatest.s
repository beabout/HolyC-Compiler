.data
gbl_global_int: .quad 0
gbl_global_char: .quad 0
gbl_global_bool: .quad 0
.align 8
.globl main
.text
try_to_init:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
movq $999, %rax
movq %rax, (gbl_global_int)
movq $99, %rax
movq %rax, (gbl_global_char)
movq $49, %rax
movq %rax, (gbl_global_bool)
movq (gbl_global_bool), %rax
jmp lbl_0
lbl_0: addq $16, %rsp
popq %rbp
retq
RunTheProgram:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $112, %rsp
callq try_to_init
movq %rax, -64(%rbp)
movq -64(%rbp), %rax
movq %rax, -24(%rbp)
movb -24(%rbp), %al
cmpb $0, %al
je lbl_2
lbl_3: nop
movq (gbl_global_int), %rax
movq $10, %rbx
cmpq %rbx, %rax
setg %al
movb %al, -72(%rbp)
movb -72(%rbp), %al
cmpb $0, %al
je lbl_4
movq (gbl_global_int), %rax
movq $1, %rbx
subq %rbx, %rax
movq %rax, (gbl_global_int)
jmp lbl_3
lbl_4: nop
movq $0, %rax
movq %rax, -32(%rbp)
movq (gbl_global_int), %rax
movq %rax, -40(%rbp)
movq (gbl_global_int), %rax
movq $1, %rbx
addq %rax, %rbx
movq %rbx, -80(%rbp)
movq -80(%rbp), %rax
movq %rax, -48(%rbp)
movq (gbl_global_int), %rax
movq $2, %rbx
movq $0, %rdx
idivq %rbx
movq %rax, -88(%rbp)
movq (gbl_global_int), %rax
movq -88(%rbp), %rbx
subq %rbx, %rax
movq %rax, -96(%rbp)
movq -96(%rbp), %rax
movq %rax, -48(%rbp)
movq -40(%rbp), %rax
movq -48(%rbp), %rbx
imulq %rbx
movq %rax, -104(%rbp)
movq -104(%rbp), %rax
movq -56(%rbp), %rbx
imulq %rbx
movq %rax, -112(%rbp)
movq -112(%rbp), %rax
movq %rax, -32(%rbp)
jmp lbl_1
lbl_2: nop
lbl_1: addq $112, %rsp
popq %rbp
retq
main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
callq RunTheProgram
lbl_5: addq $16, %rsp
popq %rbp
retq
