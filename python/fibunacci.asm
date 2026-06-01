; time r20
; mov r3, i9000

; mov r0, i1
; fibunacci:
;     print i2, r2
;     add r2, r0, r1
;     mov r0, r1
;     mov r1, r2
;     bp
;     inc r4
;     cmp r3, r4
;     jge fibunacci
; sub r22, r21, r20
; print r22, r22

time r20
mov r3, i9000000

mov r0, i1
fibunacci:
    ; print i2, r2
    add r2, r0, r1
    mov r0, r1
    mov r1, r2
    bp
    inc r4
    cmp r3, r4
    jge fibunacci
time r21
sub r22, r21, r20
print r22, r22