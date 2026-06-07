
mov r1, i0 ; string pointer
stringInput:
mov r0, r29

cmpi r0, i0
je stringInput

cmpi r0, i10
je storeText

store r0, [r1]
inci r1
printc r0, r0
jmp stringInput

storeText:
deci r1
; printi r1, r1
printi r1, r1
load r0, [r1+r10-r10]
printc r0, r0
cmpi r0, i0
jge storeText
; open i0, i1, r5
; write r5, i0, r1