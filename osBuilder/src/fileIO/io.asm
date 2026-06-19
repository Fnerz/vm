

; r23 = return val
#__DISK_OPERATION:
    store r17, [i5050] ; mode code: 1 = read, 2 = write
    store r18, [i5058] ; ram addr
    store r19, [i5066] ; disk addr
    store r20, [i5074] ; num of bytes
    .await_function_clerence:
    load r24, [i5098] ; busy flag
    cmpi r24, i1
    je .await_function_clerence

    store i1, [i5090] ; ready flag (start func)

    mov r25, i10000 ; max wait size 
    .await_done:
    load r24, [i5106] ; done flag
    cmpi r24, i1 
    je .succsess

    cmpi r25, i0 ; check if max steps in reached
    je .failure
    deci r25

    jmp .await_done ; loop

    .succsess:
    mov r23, i1
    jmp .func_end

    .failure:
    mov r23, i1
    jmp .func_end

    .func_end:
ret
