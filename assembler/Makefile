all: asm

asm: asm.c insLL.c instruction.C
	gcc -o asm asm.c insLL.c instruction.c -Wall -Werror -Wpedantic -Wgnu-binary-literal 

clean:
	rm *.o asm
