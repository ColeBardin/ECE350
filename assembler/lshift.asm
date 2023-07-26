@16 // Load A
D=M // D<-A
@18 // Load R
M=D // R<-A
(LOOP)
@17 // Load s
D=M // D<-s
@EXIT
D;JLE // Exit if S <= 0
@18 // Load R
D=M // D<-R
M=M+D // R = 2R
@17 // Load s
M=M-1 // Decrement s
@LOOP
0;JMP
(EXIT)
@EXIT // infinite loop
0;JMP

