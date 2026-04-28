from src.VM import VirtualMachine, Lexer


code = ""
with open("test.asm", "r") as f:
    code = f.read()


lexer = Lexer(code)
lexer.tokenize()