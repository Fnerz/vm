main:
call #func1
call #func2
printi r0, r2
hlt
; mov r4, i100 ; str start addr
; mov r3, r4   ; pointer for storing
; getFileName:
; load r0, [i4990] ; last key
; load r1, [i4991] ; key down

; cmpi r2, i13 ; enter
; je storeFile

; cmpi r2, i8 ; backspace
; je .backspace

; cmpi r0, r2 ; comp current key with last key
; mov r2, r0
; jne .saveChar ; diffrent key

; cmpi r1, i1 ; same key, but key is held down
; jne getFileName

; .saveChar:
; printc r0, r0
; store r0, [r3]
; inci r3
; store i0, [r3]
; jmp getFileName

; .backspace:
; store i0, [r3]
; deci r3
; jmp getFileName



storeFile:



