# purely for prototyping the c++ implementation.
# it will propably not be integrated into the python version.

from src.vm_types import *


class Instruction:
    def __init__(self):
        self.op_code: InstructionType = None
        self.args: list[int] = []
        self.reg_flags: list[bool] = []

    def __repr__(self):
        return f"{self.op_code}, {self.args}, {self.reg_flags}"


class lowerer:
    def __init__(self):
        self.insts: list[Instruction] = []
        self.reg_alloc_state = {30: False, 31: False, 32: False, 33: False}

    # returns (val or reg, is_reg)
    def tokenToArg(self, token: Token) -> tuple[int, bool]:
        if token.type == TokenType.IMMEDIATE:
            return (int(token.value), False)
        if token.type == TokenType.REGISTER:
            return (int(token.value), True)
        raise Exception("Cannot convert Token to inst arg")

    def getFreeReg(self) -> int:
        for reg, is_reserved in self.reg_alloc_state.items():
            if is_reserved == False:
                self.reg_alloc_state[reg] = True
                return reg
            
        raise Exception("No free register"
)
    def freeReg(self, reg: int) -> None:
        try:
            self.reg_alloc_state[reg] = False
        except:
            raise Exception(f"r{reg} not a temp register")
        return

    def binOpToInst(self, bin_op: BinOp) -> int:
        inst = Instruction()
        inst.op_code = bin_op.op.type

        left_arg_bundle = None
        left_is_binop = False
        if bin_op.left.binop_flag == True:
            left_reg = self.binOpToInst(bin_op.left)
            left_arg_bundle = (left_reg, True)
            left_is_binop = True
        else:
            left_arg_bundle = self.tokenToArg(bin_op.left)

        right_arg_bundle = None
        right_is_binop = False
        if bin_op.right.binop_flag == True:
            right_reg = self.binOpToInst(bin_op.right)
            right_arg_bundle = (right_reg, True)
            right_is_binop = True
        else:
            right_arg_bundle = self.tokenToArg(bin_op.right)

        ret_reg = self.getFreeReg()

        inst.args.append(ret_reg)
        inst.reg_flags.append(True)
        inst.args.append(left_arg_bundle[0])
        inst.reg_flags.append(left_arg_bundle[1])
        inst.args.append(right_arg_bundle[0])
        inst.reg_flags.append(right_arg_bundle[1])
        
        if left_is_binop:
            self.freeReg(left_arg_bundle[0])
        if right_is_binop:
            self.freeReg(right_arg_bundle[0])

        self.insts.append(inst)
        return ret_reg
        

# bin = BinOp(BinOp(Token(TokenType.REGISTER, "10"), Token(TokenType.PLUS, "+"), Token(TokenType.IMMEDIATE, "2")), Token(TokenType.MINUS, "-"), Token(TokenType.REGISTER, "1"))
bin = BinOp(
    BinOp(
        BinOp(
            BinOp(
                BinOp(Token(TokenType.REGISTER, "10"), Token(TokenType.PLUS, "+"), Token(TokenType.IMMEDIATE, "2")),
                Token(TokenType.MINUS, "-"),
                Token(TokenType.REGISTER, "1"),
            ),
            Token(TokenType.PLUS, "+"),
            Token(TokenType.IMMEDIATE, "3"),
        ),
        Token(TokenType.MINUS, "-"),
        Token(TokenType.REGISTER, "4"),
    ),
    Token(TokenType.PLUS, "+"),
    BinOp(Token(TokenType.REGISTER, "5"), Token(TokenType.MINUS, "-"), Token(TokenType.IMMEDIATE, "6"))
)

lower = lowerer()
lower.binOpToInst(bin)
for inst in lower.insts:
    print(inst)
print(lower.reg_alloc_state)

