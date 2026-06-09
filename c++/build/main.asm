
mov r3, i0
getFileName:
load r0, [i4990]
load r1, [i4991]
cmpi r2, i13
je loadFile
cmpi r2, i8
je .backspace
cmpi r0, r2
mov r2, r0
jne .saveChar
cmpi r1, i1
jne getFileName
.saveChar:
printc r0, r0
store r0, [r3]
inci r3
jmp getFileName
.backspace:
store i0, [r3]
deci r3
jmp getFileName

loadFile:
open i0, i2, r4
printi r4, r4
write r4, i200, i18
printi r0, r0
jmp i200


