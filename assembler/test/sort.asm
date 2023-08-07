@20 // load c
M=1 // c=1
(WHILEC)
@20 // Load C
D=M // D=c
@EXIT
D;JEQ
@20 // Load c
M=0 // c=0
@18 // load i
M=0 // i=0
(FORN)
@18 // load i
D=M // D=i
@17 // load n
D=D-M // D=i-n
D=D+1 // D=i-n+1
@ENDFOR
D;JGE // jmp to end of for loop if i-n+1 >= 0
@16 // Load A
D=M // D=addr of A0
@18 // load i
A=D+M // A = A0 + i
D=M // D = *(A0+i)
A=A+1 // Increment address
D=D-M // A[i] - A[i+1]
@ENDIF
D;JLE
@16 // Load A
D=M // D = A0
@18 // Load i
D=D+M // D = A0 + i
A=D // A = A0 + i
D=M // D = *(A0 + i)
@19 // load temp
M=D // temp = *(A0 + i)
@16 // Load A
D=M // D = A0
@18 // Load i
A=D+M // A = A0 + i
A=A+1 // A = A0 + i + 1
D=M // D = *(A0 + i + 1)
A=A-1 // A = A0 + i
M=D // set A[i] to be A[i+1]
A=A+1 // A = A0 + i + 1
D=A // D = A0 + i + 1
@21 // load p
M=D // Store A0 + i + 1 into p
@19 // load temp
D=M // val of temp stored in D
@21 // load p
A=M // A = A0 + i + 1
M=D // Store value of temp into A0 + i + 1
@20 // load c
M=1 // set continue to be 1
(ENDIF)
@18 // load i
M=M+1 // Increment i
@FORN
0;JMP
(ENDFOR)
@WHILEC
0;JMP
(EXIT)
@EXIT
0;JMP
