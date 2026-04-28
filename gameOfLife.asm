; game board size
mov r40, i32 ; rows
mov r41, i32 ; colls
mul r42, r41, r40

mov r20, i50000  ; current board offset
; next board offset
add r21, r20, r40
add r21, r21, i10000 ; some extra buffer cause why not

; consts reserverd regs: r20, r21 r40, r41, r42

boardInit:
    store i0, [r20+r1]
    store i0, [r21+r1]
    
    inc r1

    cmp r42, r1
    jg boardInit


; store i1, [r20+i36]
; store i1, [r20+i21]
; store i1, [r20+i22]
; store i1, [r20+i23]
; store i1, [r20+r41+i1]
; store i1, [r20+r41+i2]
; store i1, [r20+r41+i3]
; store i1, [r20+r41+r41+i3]
; store i1, [r20+r41+i8]
; store i1, [r20+r41+r41+i8]
store i1, [r20+r41+r41+i9]
store i1, [r20+r41+r41+i7]
store i1, [r20+r41+r41+r41+i8]

; store i1, [r0]

mul r0, r41, i2

store i1, [r20+r0+i3]
store i1, [r20+r0+r41+i3]
store i1, [r20+r0+r41+i1]
store i1, [r20+r0+r0+i3]
store i1, [r20+r0+r0+i2]

mov r0, i0

; store i1, [r20+r41+i2]
; store i1, [r20+r41+r41+i2]
; store i1, [r20+r41+r41]
; store i1, [r20+r41+r41+r41+i2]
; store i1, [r20+r41+r41+r41+i1]

jmp main

compPointer:
; assumes:
;    line pointer at r0
;    col pointer at r1
;    discardable value at r2
;    ret value stored at r2
    mul r2, r0, r41
    add r2, r2, r1
    ret


checkNeighbor:
; asummes:
;   usual pointer stucture at r0, r1, r2
;   uses r3
;   pushes return value on to the stack
    ; column boundry check
    cmp r1, i0
    jl noNeighbor
    cmp r1, r41
    jge noNeighbor

    ; general pointer boundry check
    call compPointer
    cmp r2, i0
    jl noNeighbor
    cmp r2, r42
    jge noNeighbor


    call compPointer

    load r3, [r20+r2]
    cmp r3, i1
    jne noNeighbor
    
    ; return function
    push i1
    ret
noNeighbor:
    push i0
    ret




getNeighborCount:
; asummes:
;   usual pointer stucture at r0, r1, r2
;   uses r3, r4
;   pushes return value on to the stack
    push r0 ; save pointer
    push r1

    mov r4, i0

    ; check right
    inc r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; check top right
    dec r0
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; check top
    dec r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; check top left
    dec r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; check left
    inc r0
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; check bot left
    inc r0
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; check bot
    inc r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    ; check bot right
    inc r1
    call compPointer
    call checkNeighbor
    pop r3
    add r4, r4, r3

    pop r1
    pop r0
    push r4
    ret


movBoard:
    load r1, [r21+r0]
    store r1, [r20+r0]
    
    inc r0
    cmp r0, r42
    jle movBoard
    ret


update:
mov r0, i0 ; line pointer
mov r1, i0 ; column pointer
mov r2, i0 ; translated index

updateLoop:
;   usual pointer stucture at r0, r1, r2
;   uses r3

    ; bp
    call compPointer
    call getNeighborCount
    call compPointer
    pop r3


    ; less then two neighbors = dead
    ; two neighbors = dead stays dead, alive stays alive
    ; three neighbors = alive
    ; more then then three neighbors = dead
    cmp r3, i2
    jl dead
    je keepState
    cmp r3, i3
    je alive
    jg dead

alive:
    store i1, [r21+r2]
    jmp endAliveCheck
dead:
    store i0, [r21+r2]
    jmp endAliveCheck

keepState:
    push r0
    load r0, [r20+r2]
    store r0, [r21+r2]
    pop r0

endAliveCheck:

    
    ; inc col counter, if line end is reached inc line counter and reset col counter.
    inc r1
    push r41
    sub r41, r41, i1
    cmp r1, r41    
    jle skipLineInc
    mov r1, i0
    inc r0
skipLineInc:
    pop r41

    cmp r2, r42
    jl updateLoop
    ret






main:
screen r40, r41, [r20]
render
bp
mov r39, i1000
mainLoop:
    ; max 20 steps
    cmp r39, i0
    je end
    dec r39

    mov r0, i0 ; reset pointer
    mov r1, i0
    call update
    mov r0, i0 ; again
    mov r1, i0
    call movBoard

    render
    bp
    sub r30, r31, r30
    jmp mainLoop


end:
