@18 // Load C
M = 0 // C = 0
@17 // Load B
D=M // D<-B
@LOOP
D;JGE
@16 // Load A
M=-M // A = -A
@17 // Load B
M=-M // B = -B
(LOOP)
@17 // Load B
D=M // D<-B
@EXIT
D;JLE // Exit if B <= 0
@18 // Load C
D=M // D<-C
@16 // Load A
D=D+M // D = C+A
@17 // Load B
M=M-1 // Dec B
@18 // Load C
M=D // C<-D
@LOOP // Load LOOP
0;JMP // Jump to LOOP
(EXIT)
@EXIT // infinite loop
0;JMP

