(init)
@256
D=A
@0
M=D
@RET_ADDR_0
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=M
@SP
A=M
M=D
@SP
M=M+1
@3
D=M
@SP
A=M
M=D
@SP
M=M+1
@4
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@5
D=D-A
@2
M=D
@SP
D=M
@1
M=D
@main
0;JMP
(RET_ADDR_0)
(WHILE)
@WHILE
0;JMP
(main)
@6
D=A
@SP
A=M
M=D
@SP
M=M+1
@RET_ADDR_1
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=M
@SP
A=M
M=D
@SP
M=M+1
@3
D=M
@SP
A=M
M=D
@SP
M=M+1
@4
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@6
D=D-A
@2
M=D
@SP
D=M
@1
M=D
@fibonacci
0;JMP
(RET_ADDR_1)
@1
D=M
@R14
M=D
@R14
D=M
@5
A=D-A
D=M
@R15
M=D
@SP
M=M-1
A=M
D=M
@2
A=M
M=D
@2
D=M+1
@SP
M=D
@R14
D=M
@1
A=D-A
D=M
@4
M=D
@R14
D=M
@2
A=D-A
D=M
@3
M=D
@R14
D=M
@3
A=D-A
D=M
@2
M=D
@R14
D=M
@4
A=D-A
D=M
@1
M=D
@R15
A=M
0;JMP
(fibonacci)
@2
D=M
A=D
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
M=M-1
A=M
D=M
@SP
M=M-1
A=M
D=M-D
@IF_LT_0
D;JLT
@SP
A=M
M=0
@END_IF_LT_0
0;JMP
(IF_LT_0)
@SP
A=M
M=-1
(END_IF_LT_0)
@SP
M=M+1
@SP
M=M-1
A=M
D=M
@IF_TRUE
D;JNE
@IF_FALSE
0;JMP
(IF_TRUE)
@2
D=M
A=D
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@R14
M=D
@R14
D=M
@5
A=D-A
D=M
@R15
M=D
@SP
M=M-1
A=M
D=M
@2
A=M
M=D
@2
D=M+1
@SP
M=D
@R14
D=M
@1
A=D-A
D=M
@4
M=D
@R14
D=M
@2
A=D-A
D=M
@3
M=D
@R14
D=M
@3
A=D-A
D=M
@2
M=D
@R14
D=M
@4
A=D-A
D=M
@1
M=D
@R15
A=M
0;JMP
(IF_FALSE)
@2
D=M
A=D
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
M=M-1
A=M
D=M
@SP
M=M-1
A=M
D=M-D
@SP
A=M
M=D
@SP
M=M+1
@RET_ADDR_2
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=M
@SP
A=M
M=D
@SP
M=M+1
@3
D=M
@SP
A=M
M=D
@SP
M=M+1
@4
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@6
D=D-A
@2
M=D
@SP
D=M
@1
M=D
@fibonacci
0;JMP
(RET_ADDR_2)
@2
D=M
A=D
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
M=M-1
A=M
D=M
@SP
M=M-1
A=M
D=M-D
@SP
A=M
M=D
@SP
M=M+1
@RET_ADDR_3
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=M
@SP
A=M
M=D
@SP
M=M+1
@3
D=M
@SP
A=M
M=D
@SP
M=M+1
@4
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@6
D=D-A
@2
M=D
@SP
D=M
@1
M=D
@fibonacci
0;JMP
(RET_ADDR_3)
@SP
M=M-1
A=M
D=M
@SP
M=M-1
A=M
D=M+D
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@R14
M=D
@R14
D=M
@5
A=D-A
D=M
@R15
M=D
@SP
M=M-1
A=M
D=M
@2
A=M
M=D
@2
D=M+1
@SP
M=D
@R14
D=M
@1
A=D-A
D=M
@4
M=D
@R14
D=M
@2
A=D-A
D=M
@3
M=D
@R14
D=M
@3
A=D-A
D=M
@2
M=D
@R14
D=M
@4
A=D-A
D=M
@1
M=D
@R15
A=M
0;JMP