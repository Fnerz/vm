; time r20


; mov r10, i100
; mov r0, i1
; itou r0, r0
; itou r1, r1
; itou r2, r2
; fibu:
;     addu r2, r0, r1
;     mov r0, r1
;     mov r1, r2

;     deci r10
;     cmpi r10, i0
;     jg fibu

; time r21
; subf r22, r21, r20
; printf r22, r22

mov r0, i1
printi r0, r0
hlt
printi r0, r0

