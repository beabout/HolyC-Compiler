.data
.align 8
.globl main
.text
Jumble:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $128, %rsp
movq -24(%rbp), %rax
movq -32(%rbp), %rbx
imulq %rbx
movq %rax, -72(%rbp)
movq -72(%rbp), %rax
movq -40(%rbp), %rbx
imulq %rbx
movq %rax, -80(%rbp)
movq -80(%rbp), %rax
movq -48(%rbp), %rbx
imulq %rbx
movq %rax, -88(%rbp)
movq -88(%rbp), %rax
movq -56(%rbp), %rbx
imulq %rbx
movq %rax, -96(%rbp)
movq -96(%rbp), %rax
movq -64(%rbp), %rbx
imulq %rbx
movq %rax, -104(%rbp)
movq -104(%rbp), %rax
movq , %rbx
imulq %rbx
movq %rax, -112(%rbp)
movq -112(%rbp), %rax
movq , %rbx
imulq %rbx
movq %rax, -120(%rbp)
movq %rax, -120(%rbp)
jmp lbl_0
lbl_0: addq $128, %rsp
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
movq $75, -999(%rbp)
movq $87, -999(%rbp)
callq Jumble
movq -32(%rbp), %rax
movq -32(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rdi
callq printInt
lbl_1: addq $32, %rsp
popq %rbp
retq
