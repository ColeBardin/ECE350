@256
D=A
@0
M=D
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@17
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
@IF_EQ_0
D;JEQ
@SP
A=M
M=0
@END_IF_EQ_0
0;JMP
(IF_EQ_0)
@SP
A=M
M=-1
(END_IF_EQ_0)
@SP
M=M+1
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@16
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
@IF_EQ_1
D;JEQ
@SP
A=M
M=0
@END_IF_EQ_1
0;JMP
(IF_EQ_1)
@SP
A=M
M=-1
(END_IF_EQ_1)
@SP
M=M+1
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
@17
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
@IF_EQ_2
D;JEQ
@SP
A=M
M=0
@END_IF_EQ_2
0;JMP
(IF_EQ_2)
@SP
A=M
M=-1
(END_IF_EQ_2)
@SP
M=M+1
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
@891
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
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@892
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
@IF_LT_1
D;JLT
@SP
A=M
M=0
@END_IF_LT_1
0;JMP
(IF_LT_1)
@SP
A=M
M=-1
(END_IF_LT_1)
@SP
M=M+1
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@891
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
@IF_LT_2
D;JLT
@SP
A=M
M=0
@END_IF_LT_2
0;JMP
(IF_LT_2)
@SP
A=M
M=-1
(END_IF_LT_2)
@SP
M=M+1
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
@32766
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
@IF_GT_0
D;JGT
@SP
A=M
M=0
@END_IF_GT_0
0;JMP
(IF_GT_0)
@SP
A=M
M=-1
(END_IF_GT_0)
@SP
M=M+1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@32767
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
@IF_GT_1
D;JGT
@SP
A=M
M=0
@END_IF_GT_1
0;JMP
(IF_GT_1)
@SP
A=M
M=-1
(END_IF_GT_1)
@SP
M=M+1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@32766
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
@IF_GT_2
D;JGT
@SP
A=M
M=0
@END_IF_GT_2
0;JMP
(IF_GT_2)
@SP
A=M
M=-1
(END_IF_GT_2)
@SP
M=M+1
@57
D=A
@SP
A=M
M=D
@SP
M=M+1
@31
D=A
@SP
A=M
M=D
@SP
M=M+1
@53
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
D=M+D
@SP
A=M
M=D
@SP
M=M+1
@112
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
@SP
M=M-1
A=M
M=-M
@SP
M=M+1
@SP
M=M-1
A=M
D=M
@SP
M=M-1
A=M
D=D&M
@SP
A=M
M=D
@SP
M=M+1
@82
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
D=D|M
@SP
A=M
M=D
@SP
M=M+1
@SP
M=M-1
A=M
M=!M
@SP
M=M+1
(EXIT)
@EXIT
0;JMP
