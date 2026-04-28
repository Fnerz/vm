from src.types import *

class LineParser:
    def __init__(self, tokens: list[Token], ic_line: int, src_line: int, vm = None) -> None:
        self.tokens = tokens
        self.i = -1
        self.ic_line = ic_line
        self.src_line = src_line
        self.cur = tokens[0] if tokens else None
        self.next = tokens[1] if len(tokens) > 1 else None
        self.vm = vm
        
        self.advance()

    def advance(self) -> Token:
        self.i += 1
        self.cur = self.tokens[self.i] if self.i < len(self.tokens) else None
        self.next = self.tokens[self.i + 1] if self.i + 1 < len(self.tokens) else None
        return self.cur

    def eat(self, *type: TokenType) -> Token:
        last = self.cur
        if self.cur and self.cur.type in type:
            self.advance()
            return last
        else:
            raise Exception(f"Expected token of type {type}, got {self.cur}")

    def parsePointerArithmetic(self) -> BinOp:
        """
        expects self.cur to be lparen. it passes beyond rparen.
        """
        self.eat(TokenType.LPAREN)

        left = self.cur
        self.eat(TokenType.REGISTER, TokenType.IMMEDIATE)

        uses_arithmetic = False
        while self.cur and self.cur.type in (TokenType.PLUS, TokenType.MINUS):
            uses_arithmetic = True
            op = self.cur
            self.advance()
            right = self.cur
            self.eat(TokenType.REGISTER, TokenType.IMMEDIATE)
            left = BinOp(left, op, right)

        if not uses_arithmetic:
            left = BinOp(left, Token(TokenType.PLUS, "+"), Token(TokenType.IMMEDIATE, "0"))

        self.eat(TokenType.RPAREN)
        return left

    def getValue(self) -> Token | BinOp:
        """
        expects self.cur to be on a value token. it a beyond the full value
        """
        if self.cur and self.cur.type in [TokenType.REGISTER, TokenType.IMMEDIATE]:
            return self.eat(TokenType.REGISTER, TokenType.IMMEDIATE)
        elif self.cur and self.cur.type == TokenType.LPAREN:
            return self.parsePointerArithmetic()
        else:
            raise Exception(f"Expected value token, got {self.cur}")

    def parse(self) -> list[Instruction]:
        self.instructions: list[Instruction] = []

        while self.cur is not None:
            if self.cur.type == TokenType.INST:
                inst = self.cur
                self.advance()
                if inst.value == "mov":
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    mov_inst = Instruction(InstructionType.MOV, {"dest": left, "src": right})
                    self.instructions.append(mov_inst)

                elif inst.value == "add":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    add_inst = Instruction(InstructionType.ADD, {"dest": dest, "left": left, "right": right})
                    self.instructions.append(add_inst)
                
                elif inst.value == "sub":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    sub_inst = Instruction(InstructionType.SUB, {"dest": dest, "left": left, "right": right})
                    self.instructions.append(sub_inst)

                elif inst.value == "mul":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    mul_inst = Instruction(InstructionType.MUL, {"dest": dest, "left": left, "right": right})
                    self.instructions.append(mul_inst)

                elif inst.value == "idiv":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    idiv_inst = Instruction(InstructionType.IDIV, {"dest": dest, "left": left, "right": right})
                    self.instructions.append(idiv_inst)

                elif inst.value == "fdiv":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    fdiv_inst = Instruction(InstructionType.FDIV, {"dest": dest, "left": left, "right": right})
                    self.instructions.append(fdiv_inst)

                elif inst.value == "mod":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    fdiv_inst = Instruction(InstructionType.MOD, {"dest": dest, "left": left, "right": right})
                    self.instructions.append(fdiv_inst)

                elif inst.value == "abs":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    src = self.getValue()
                    abs_inst = Instruction(InstructionType.ABS, {"dest": dest, "src": src})
                    self.instructions.append(abs_inst)

                elif inst.value in ["jmp", "je", "jne", "jg", "jl", "jge", "jle"]:
                    label = self.eat(TokenType.SYMBOL)
                    jmp_inst = Instruction(InstructionType(inst.value), {"label": label})
                    self.instructions.append(jmp_inst)
               
                elif inst.value == "cmp":
                    left = self.getValue()
                    self.eat(TokenType.COMMA)
                    right = self.getValue()
                    cmp_inst = Instruction(InstructionType.CMP, {"left": left, "right": right})
                    self.instructions.append(cmp_inst)

                elif inst.value == "load":
                    dest = self.getValue()
                    self.eat(TokenType.COMMA)
                    src = self.parsePointerArithmetic()
                    load_inst = Instruction(InstructionType.LOAD, {"dest": dest, "src": src})
                    self.instructions.append(load_inst)
                
                elif inst.value == "store":
                    src = self.getValue()
                    self.eat(TokenType.COMMA)
                    dest = self.parsePointerArithmetic()
                    store_inst = Instruction(InstructionType.STORE, {"dest": dest, "src": src})
                    self.instructions.append(store_inst)

                elif inst.value == "print":
                    start = self.getValue()
                    self.eat(TokenType.COMMA)
                    end = self.getValue()
                    self.instructions.append(Instruction(InstructionType.PRINT, {"start": start, "end": end}))

                elif inst.value == "bp":
                    self.instructions.append(Instruction(InstructionType.BREAKPOINT, {}))

                elif inst.value == "call":
                    label = self.eat(TokenType.SYMBOL)
                    call_inst = Instruction(InstructionType.CALL, {"label": label})
                    self.instructions.append(call_inst)

                elif inst.value == "ret":
                    self.instructions.append(Instruction(InstructionType.RET, {}))

                elif inst.value == "screen":
                    rows = self.getValue()
                    self.eat(TokenType.COMMA)
                    cols = self.getValue()
                    self.eat(TokenType.COMMA)
                    memory_addr = self.getValue()
                    self.instructions.append(Instruction(InstructionType.SCREEN, {"rows": rows, "cols": cols, "memory_addr": memory_addr}))

                elif inst.value == "render":
                    self.instructions.append(Instruction(InstructionType.RENDER, {}))

                elif inst.value == "push":
                    src = self.getValue()
                    push_inst = Instruction(InstructionType.PUSH, {"src": src})
                    self.instructions.append(push_inst)

                elif inst.value == "pop":
                    dest = self.getValue()
                    pop_inst = Instruction(InstructionType.POP, {"dest": dest})
                    self.instructions.append(pop_inst)

                elif inst.value == "dec":
                    dest = self.getValue()
                    dec_inst = Instruction(InstructionType.DEC, {"dest": dest})
                    self.instructions.append(dec_inst)
                
                elif inst.value == "inc":
                    dest = self.getValue()
                    inc_inst = Instruction(InstructionType.INC, {"dest": dest})
                    self.instructions.append(inc_inst)

                elif inst.value == "input":
                    dest = self.getValue()
                    input_inst = Instruction(InstructionType.INPUT, {"dest": dest})
                    self.instructions.append(input_inst)

                elif inst.value == "time":
                    dest = self.getValue()
                    time_inst = Instruction(InstructionType.TIME, {"dest": dest})
                    self.instructions.append(time_inst)




            elif self.cur and self.cur.type == TokenType.SYMBOL and self.next and self.next.type == TokenType.COLON:
                label = self.cur.value
                self.vm.labels[label] = self.ic_line
                self.advance()
                self.advance()



            else:
                raise Exception(f"Unexpected token: {self.cur}")

        return self.instructions
