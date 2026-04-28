from __future__ import annotations
from enum import Enum

class TokenType(Enum):
    INST = "INST"
    REGISTER = "REGISTER"
    IMMEDIATE = "IMMEDIATE"
    STACK = "STACK"
    SYMBOL = "SYMBOL"
    PLUS = "PLUS"
    MINUS = "MINUS"
    LPAREN = "LPAREN"
    RPAREN = "RPAREN"
    COLON = "COLON"
    COMMA = "COMMA"

class InstructionType(Enum):
    MOV = "mov"
    ADD = "add"
    SUB = "sub"
    MUL = "mul"
    IDIV = "idiv"
    FDIV = "fdiv"
    MOD = "mod"
    ABS = "abs"
    CMP = "cmp"
    JMP = "jmp"
    JE = "je"
    JNE = "jne"
    JG = "jg"
    JL  = "jl"
    JGE = "jge"
    JLE = "jle"
    LOAD = "load"
    STORE = "store"
    PRINT = "print"
    BREAKPOINT = "bp"
    SCREEN = "screen"
    RENDER = "render"
    CALL = "call"
    RET = "ret"
    PUSH = "push"
    POP = "pop"
    DEC = "dec"
    INC = "inc"
    INPUT = "input"
    TIME = "time"

class Token:
    def __init__(self, type: TokenType, value: str | Token):
        self.type: TokenType = type
        self.value: str | Token = value
        self.binop_flag: bool = False # used to resolve value

    def __repr__(self):
        if self.type == TokenType.IMMEDIATE:
            return f"i{self.value}"
        if self.type == TokenType.REGISTER:
            return f"r{self.value}"
        
        return f"{self.value}"

class Instruction:
    def __init__(self, type: InstructionType, args: dict[str, Token | BinOp]):
        self.type = type
        self.args = args
        self.line_number: int = 0

    def __repr__(self):
        ret: str = f"{self.type.value} "
        for i, a in enumerate(self.args):
            ret += f"{self.args[a].__repr__()}"
            if i != len(self.args)-1:
                ret += ","
            ret += f" "

        return ret

class BinOp:
    def __init__(self, left: Token | BinOp, op: Token, right: Token | BinOp):
        self.left = left
        self.op = op
        self.right = right
        self.binop_flag: bool = True # uesd to resolve value

    def __repr__(self):
        return f"[{self.left} {self.op} {self.right}]"


SYMBOL_ALLOWED_CHARS = "abcdefghijklmnopqrstuvwyxz_"