import string
import time
import math
import pygame
from src.types import *
from src.lexer import LineLexer
from src.parser import LineParser

class VmError:
    def __init__(self, vm: VirtualMachine):
        self.vm = vm


class VirtualMachine:
    def __init__(self, code: str, register_count: int = 16, debug: bool = False, step_threw: bool = False, print_step: bool = False):
        self.code: str = code.lower()
        self.register_count: int = register_count
        self.debug: bool = debug
        self.step_threw: bool = step_threw
        self.print_step: bool = print_step

        self.screen: pygame.surface = None
        
        self.output: list[str] = [] # only used for debugger

        self.registers: list[int] = [0 for _ in range(self.register_count)]
        self.memory: dict[int, int] = {}
        self.stack: list[int] = []

        self.lesser_flag: bool = False
        self.greater_flag: bool = False
        self.equal_flag: bool = False

        self.call_stack: list[int] = []
        self.labels: dict[str, int] = {}
        self.ic: int = 0

        self.tokens: list[Token] = []
        self.tok_lines: list[list[Token]] = []
        self.instructions: list[Instruction] = []
        self.tokenize()
        self.parse()

    def reset(self) -> None:
        self.registers = [0 for _ in range(self.register_count)]
        self.memory = {}
        self.stack: list[int] = []
        self.call_stack: list[int] = []
        self.labels: dict[str, int] = {}
        self.ic: int = 0
        self.output: list[str] = []

        self.tokens: list[Token] = []
        self.tok_lines: list[list[Token]] = []
        self.instructions: list[Instruction] = []

        if self.screen:
            pygame.quit()
        self.screen = None


        self.resetFlags()
        return

    def resetFlags(self) -> None:
        self.lesser_flag = False
        self.greater_flag = False
        self.equal_flag = False
        return

    def tokenize(self) -> None:
        for line in self.code.splitlines():
            lexer = LineLexer(line)
            toke = lexer.tokenize()
            self.tokens.extend(toke)
            self.tok_lines.append(toke)

    def parse(self) -> None:
        # parser = Parser(self.tokens, self)
        # self.instructions = parser.parse()
        line = 0
        for i, tok_line in enumerate(self.tok_lines):
            parser = LineParser(tok_line, line, i+1, self)
            insts = parser.parse()
            for inst in insts:
                line += 1
                inst.line_number = i + 1
                self.instructions.append(inst)

        return

    def resolveValue(self, token: Token | BinOp) -> int:
        if not token.binop_flag:
            if token.type == TokenType.REGISTER:
                return self.registers[int(token.value)]
            elif token.type == TokenType.IMMEDIATE:
                return int(token.value)
            else:
                raise Exception(f"Expected value token, got {token}")
        else:
            if token.op.type == TokenType.PLUS:
                return self.resolveValue(token.left) + self.resolveValue(token.right)
            elif token.op.type == TokenType.MINUS:
                return self.resolveValue(token.left) - self.resolveValue(token.right)
            else:
                raise Exception(f"Expected plus or minus operator, got {token.op}")
        
    def resolveAddress(self, token: Token | BinOp) -> int:
        if not token.binop_flag:
            if token.type == TokenType.REGISTER:
                return int(token.value)
            elif token.type == TokenType.IMMEDIATE:
                return int(token.value)
            else:
                raise Exception(f"Expected value token, got {token}")
        else:
            if token.op.type == TokenType.PLUS:
                return self.resolveValue(token.left) + self.resolveValue(token.right)
            elif token.op.type == TokenType.MINUS:
                return self.resolveValue(token.left) - self.resolveValue(token.right)
            else:
                raise Exception(f"Expected plus or minus operator, got {token.op}")

    def step(self) -> bool:
        len_inst: int = len(self.instructions)
        if self.ic >= len_inst:
            return False
        
        inst = self.instructions[self.ic]
        if inst.type == InstructionType.MOV:
            dest = inst.args["dest"]
            src = inst.args["src"]
            
            src_val = self.resolveValue(src)
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] = src_val

        elif inst.type == InstructionType.ADD:
            dest = inst.args["dest"]
            left = inst.args["left"]
            right = inst.args["right"]

            left_val = self.resolveValue(left)
            right_val = self.resolveValue(right)
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] = left_val + right_val

        elif inst.type == InstructionType.SUB:
            dest = inst.args["dest"]
            left = inst.args["left"]
            right = inst.args["right"]

            left_val = self.resolveValue(left)
            right_val = self.resolveValue(right)
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] = left_val - right_val

        elif inst.type == InstructionType.MUL:
            dest = inst.args["dest"]
            left = inst.args["left"]
            right = inst.args["right"]

            left_val = self.resolveValue(left)
            right_val = self.resolveValue(right)
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] = left_val * right_val

        elif inst.type == InstructionType.IDIV:
            dest = inst.args["dest"]
            left = inst.args["left"]
            right = inst.args["right"]

            left_val = self.resolveValue(left)
            right_val = self.resolveValue(right)
            dest_addr = self.resolveAddress(dest)
            if right_val == 0:
                raise Exception("Division by zero")
            self.registers[dest_addr] = left_val // right_val

        elif inst.type == InstructionType.FDIV:
            dest = inst.args["dest"]
            left = inst.args["left"]
            right = inst.args["right"]

            left_val = self.resolveValue(left)
            right_val = self.resolveValue(right)
            dest_addr = self.resolveAddress(dest)
            if right_val == 0:
                raise Exception("Division by zero")
            self.registers[dest_addr] = left_val / right_val

        elif inst.type == InstructionType.MOD:
            dest = inst.args["dest"]
            left = inst.args["left"]
            right = inst.args["right"]

            left_val = self.resolveAddress(left)
            right_val = self.resolveAddress(right)
            dest_addr = self.resolveAddress(self.resolveAddress)

            if right_val == 0:
                raise Exception("Division by zero")
            self.registers[dest_addr] = left_val % right_val 

        elif inst.type == InstructionType.ABS:
            dest = inst.args["dest"]
            src = inst.args["src"]
            
            src_val = self.resolveValue(src)
            dest_addr = self.resolveAddress(dest)

            self.registers[dest_addr] = abs(src_val)

        elif inst.type == InstructionType.CMP:
            left = inst.args["left"]
            right = inst.args["right"]

            left_val = self.resolveValue(left)
            right_val = self.resolveValue(right)

            self.resetFlags()
            if left_val < right_val:
                self.lesser_flag = True
            elif left_val > right_val:
                self.greater_flag = True
            else:
                self.equal_flag = True

        elif inst.type == InstructionType.JMP:
            label = inst.args["label"].value
            if label not in self.labels:
                raise Exception(f"Undefined label: {label}")
            self.ic = self.labels[label]
            return True

        elif inst.type == InstructionType.JE:
            if self.equal_flag:
                label = inst.args["label"].value
                if label not in self.labels:
                    raise Exception(f"Undefined label: {label}")
                self.ic = self.labels[label]
                return True
            
        elif inst.type == InstructionType.JNE:
            if not self.equal_flag:
                label = inst.args["label"].value
                if label not in self.labels:
                    raise Exception(f"Undefined label: {label}")
                self.ic = self.labels[label]
                return True

        elif inst.type == InstructionType.JG:
            if self.greater_flag:
                label = inst.args["label"].value
                if label not in self.labels:
                    raise Exception(f"Undefined label: {label}")
                self.ic = self.labels[label]
                return True

        elif inst.type == InstructionType.JL:
            if self.lesser_flag:
                label = inst.args["label"].value
                if label not in self.labels:
                    raise Exception(f"Undefined label: {label}")
                self.ic = self.labels[label]
                return True

        elif inst.type == InstructionType.JGE:
            if self.greater_flag or self.equal_flag:
                label = inst.args["label"].value
                if label not in self.labels:
                    raise Exception(f"Undefined label: {label}")
                self.ic = self.labels[label]
                return True

        elif inst.type == InstructionType.JLE:
            if self.lesser_flag or self.equal_flag:
                label = inst.args["label"].value
                if label not in self.labels:
                    raise Exception(f"Undefined label: {label}")
                self.ic = self.labels[label]
                return True
            
        elif inst.type == InstructionType.LOAD:
            dest = inst.args["dest"]
            src = inst.args["src"]

            src_addr = self.resolveValue(src)
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] = self.memory.get(src_addr, 0)

        elif inst.type == InstructionType.STORE:
            src = inst.args["src"]
            dest = inst.args["dest"]

            src_val = self.resolveValue(src)
            dest_addr = self.resolveAddress(dest)
            self.memory[dest_addr] = src_val
        
            pass

        elif inst.type == InstructionType.PRINT:
            start = inst.args["start"]
            end = inst.args["end"]
            
            start_val = self.resolveAddress(start)
            end_val = self.resolveAddress(end)
            if start_val == end_val:
                print(self.registers[start_val])
                self.ic += 1
                return True

            values = []
            for i in range(start_val, end_val):
                values.append(self.registers[i])
            
            print(values)

        elif inst.type == InstructionType.BREAKPOINT:
            pass

        elif inst.type == InstructionType.SCREEN:
            rows = inst.args["rows"]
            cols = inst.args["cols"]
            memory_addr = inst.args["memory_addr"]
            self.screen = Screen(self.resolveValue(rows), self.resolveValue(cols), 20, "Grid", self, self.resolveValue(memory_addr))
            self.screen.setup_pygame_window()

        elif inst.type == InstructionType.RENDER:
            if self.screen == None:
                if self.debug:
                    print("Screen not initialized")
                else:
                    raise Exception("Screen not initialized")
            else:
                self.screen.render()

        elif inst.type == InstructionType.CALL:
            label = inst.args["label"].value
            if label not in self.labels:
                raise Exception(f"Undefined label: {label}")
            self.call_stack.append(self.ic+1)
            self.ic = self.labels[label]
            return True

        elif inst.type == InstructionType.RET:
            if not self.call_stack:
                raise Exception(f"Call stack underflow on inst {self.ic}: {self.instructions[self.ic]}")
            self.ic = self.call_stack.pop()
            return True
        
        elif inst.type == InstructionType.PUSH:
            src = inst.args["src"]
            src_val = self.resolveValue(src)
            self.stack.append(src_val)

        elif inst.type == InstructionType.POP:
            dest = inst.args["dest"]
            if not self.stack:
                raise Exception(f"Call stack underflow on inst {self.ic}: {self.instructions[self.ic]}")
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] = self.stack.pop()

        elif inst.type == InstructionType.DEC:
            dest = inst.args["dest"]
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] -= 1
        
        elif inst.type == InstructionType.INC:
            dest = inst.args["dest"]
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] += 1

        elif inst.type == InstructionType.INPUT:
            dest = inst.args["dest"]
            dest_addr = self.resolveAddress(dest)
            user_input = input("Input: ")
            try:
                val = int(user_input)
            except ValueError:
                raise Exception(f"Invalid input: {user_input}")
            self.registers[dest_addr] = val

        elif inst.type == InstructionType.TIME:
            dest = inst.args["dest"]

            t = time.time()
            dest_addr = self.resolveAddress(dest)
            self.registers[dest_addr] = t

        self.ic += 1
        return True

    def run(self) -> None:
        self.ic = 0
        len_inst = len(self.instructions)
        while self.ic < len_inst:
            if self.print_step:
                self.printState()
            if self.step_threw:
                input()
            if self.debug:
                self.printDebug()
            res = self.step()

            if not res:
                break

        return
    