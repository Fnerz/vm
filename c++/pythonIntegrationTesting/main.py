from src.VM import VirtualMachine


# with open("test.asm", "r") as f:
#     code = f.read()
code = """mov r3, [i9 + i2 + r0]
mov r0, [i1 + i1]
    print i2, r2
    add r2, r0, r1
    mov r0, r1
    mov r1, r2
    bp
    inc r4
    cmp r3, r4
    jge fibunacci
bp 
"""

vm = VirtualMachine()
vm.loadCode(code)