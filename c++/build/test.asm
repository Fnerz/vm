; time r20

loop:
mov r0 , r29
cmpi r0, i0
je loop
printc r0, r0
jmp loop

