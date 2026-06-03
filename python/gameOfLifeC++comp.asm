; =========================
; INIT
; =========================

; game board size
mov r20, i32        ; rows
mov r21, i32        ; cols
mul r22, r20, r21   ; total size

mov r23, i50000     ; board A
add r24, r23, r20
add r24, r24, i10000

; =========================
; BOARD INIT
; =========================

mov r1, i0

boardInit:
    store i0, [r23+r1]
    store i0, [r24+r1]

    inc r1
    cmp r22, r1
    jg boardInit


; =========================
; SEED PATTERN
; =========================

mul r0, r21, i2

store i1, [r23+r0+i3]
store i1, [r23+r0+r21+i3]
store i1, [r23+r0+r21+i1]
store i1, [r23+r0+r0+i3]
store i1, [r23+r0+r0+i2]

mov r0, i0

jmp main


; =========================
; compPointer
; r0=row, r1=col -> r2=index
; =========================

compPointer:
    mul r2, r0, r21
    add r2, r2, r1
    ret


; =========================
; checkNeighbor
; =========================

checkNeighbor:
    cmp r1, i0
    jl noNeighbor
    cmp r1, r21
    jge noNeighbor

    call compPointer
    cmp r2, i0
    jl noNeighbor
    cmp r2, r22
    jge noNeighbor

    call compPointer

    load r3, [r23+r2]
    cmp r3, i1
    jne noNeighbor

    push i1
    ret

noNeighbor:
    push i0
    ret


; =========================
; getNeighborCount
; =========================

getNeighborCount:
    push r0
    push r1

    mov r4, i0

    ; right
    inc r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; top right
    dec r0
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; top
    dec r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; top left
    dec r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; left
    inc r0
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; bottom left
    inc r0
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; bottom
    inc r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; bottom right
    inc r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    pop r1
    pop r0

    push r4
    ret


; =========================
; movBoard
; =========================

movBoard:
    load r1, [r24+r0]
    store r1, [r23+r0]

    inc r0
    cmp r0, r22
    jle movBoard
    ret


; =========================
; UPDATE
; =========================

update:
mov r0, i0
mov r1, i0
mov r2, i0

updateLoop:

    call compPointer
    call getNeighborCount
    call compPointer
    pop r3

    cmp r3, i2
    jl dead
    je keepState
    cmp r3, i3
    je alive
    jg dead

alive:
    store i1, [r24+r2]
    jmp endAliveCheck

dead:
    store i0, [r24+r2]
    jmp endAliveCheck

keepState:
    push r0
    load r0, [r23+r2]
    store r0, [r24+r2]
    pop r0

endAliveCheck:

    inc r1
    push r21
    sub r21, r21, i1
    cmp r1, r21
    jle skipLineInc
    mov r1, i0
    inc r0
skipLineInc:
    pop r21

    inc r2
    cmp r2, r22
    jl updateLoop

    ret


; =========================
; MAIN
; =========================

main:
bp
mov r25, i1000

mainLoop:
    cmp r25, i0
    je end
    dec r25

    time r10

    mov r0, i0
    mov r1, i0
    call update

    mov r0, i0
    call movBoard

    time r11
    sub r11, r11, r10
    print r11, r11

    bp
    jmp mainLoop

end: