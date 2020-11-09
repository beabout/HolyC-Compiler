lbl_main: pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
lbl_0: add $8, %rsp
popq %rbp
retq



	as empty.s -o prog.o
        ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
	  /usr/lib/x86_64-linux-gnu/crt1.o \
	  /usr/lib/x86_64-linux-gnu/crti.o \
	  -lc \
	  prog.o \
	  stdholeyc.o \
	  /usr/lib/x86_64-linux-gnu/crtn.o \
        -o prog.exe