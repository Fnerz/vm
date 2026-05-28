from src.VM import VirtualMachine


# with open("test.asm", "r") as f:
#     code = f.read()
code = """mov r3, i9  

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
"""

# code = """
# add [r3+i2+i1+r5], [i1-i3+r22], [r0+r0-r1]
# ;mov r3, r1


# """

vm = VirtualMachine()
vm.loadCode(code)
vm.run()
# vm.testFunc()

