main:
mov r17, i1
mov r18, i20016 ; ic = 417
mov r19, i0
mov r20, i96
call #__DISK_OPERATION
printi r0, r0
; jmpa i417
hlt