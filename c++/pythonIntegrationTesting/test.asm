mov r3, i9  

mov r0, i1
fibunacci:
    print i2, r2
    add r2, r0, r1
    mov r0, r1
    mov r1, r2
    bp
    inc r4
    cmp r3, r4
    jge fibunacci
bp 