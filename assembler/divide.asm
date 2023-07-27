@18 // Load Q
M = 0 // Q = 0
@16 // Load A
D=M // D<-A
@19 // Load R
M=D // R<-A
(LOOP)
@19 // Load R
D=M // D<-R
@17 // Load B
D=D-M // D<-R-B
@EXIT
D;JLT // Exit if R < B
@18 // Load Q
M=M+1 // Increment Q
@17 // Load B
D=M // D<-B
@19 // Load R
M=M-D // R<-R-B
@LOOP // Load LOOP
0;JMP // Jump to LOOP
(EXIT)
@EXIT // infinite loop
0;JMP

