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
(EXIT)
@EXIT
0;JMP
(main)
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@5
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@10
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@1
D=D+A
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@1
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
@1
A=D+A
D=M
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
@7
D=D-A
@2
M=D
@SP
D=M
@1
M=D
@add
0;JMP
(RET_ADDR_1)
@1
D=M
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@100
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@2
D=D+A
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@80
D=A
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@3
D=D+A
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@1
D=M
@2
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@3
A=D+A
D=M
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
@7
D=D-A
@2
M=D
@SP
D=M
@1
M=D
@add
0;JMP
(RET_ADDR_2)
@1
D=M
@2
D=D+A
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@1
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
@2
A=D+A
D=M
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
@7
D=D-A
@2
M=D
@SP
D=M
@1
M=D
@sub
0;JMP
(RET_ADDR_3)
@1
D=M
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@1
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
(sub)
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
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
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@2
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@1
D=D+A
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@1
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
@1
A=D+A
D=M
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
(add)
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
@0
D=A
@SP
A=M
M=D
@SP
M=M+1
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
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@2
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=M
@1
D=D+A
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
@1
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
@1
A=D+A
D=M
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
