I asked Dr. Kandasamy if I could write my assembler in C instead of Python. He said it was okay.

I included a Makefile that can compile the executable. 

You can assemble a single file by: ./asm fileToAssemble.asm

The bash script unitTest.sh will assemble all .asm files in the 'test' subdir and compare the output hack files to respective files ending with _gen.

The files with _gen were either given to us from the project source code, or I used the Nand2Tetris assembler to generate them.
