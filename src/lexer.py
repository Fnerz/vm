from src.types import *

class LineLexer:
    def __init__(self, line: str):
        self.line: str = line    
        self.i: int = -1
        self.cur: str | None= line[0] if line else None
        self.next: str | None = line[1] if len(line) > 1 else None

        self.advance()

    def advance(self) -> str:
        self.i += 1
        self.cur = self.line[self.i] if self.i < len(self.line) else None
        self.next = self.line[self.i + 1] if self.i + 1 < len(self.line) else None
        return self.cur

    def tokenize(self) -> list[Token]:
        tokens = []

        while self.cur is not None:
            if self.cur.isspace():
                self.advance()
                continue

            if self.cur in ";":
                break

            elif self.cur in "[":
                tokens.append(Token(TokenType.LPAREN, self.cur))
                self.advance()
                continue

            elif self.cur in "]":
                tokens.append(Token(TokenType.RPAREN, self.cur))
                self.advance()
                continue

            elif self.cur in "+":
                tokens.append(Token(TokenType.PLUS, self.cur))
                self.advance()
                continue

            elif self.cur in "-":
                tokens.append(Token(TokenType.MINUS, self.cur))
                self.advance()
                continue

            elif self.cur in ":":
                tokens.append(Token(TokenType.COLON, self.cur))
                self.advance()
                continue

            elif self.cur in ",":
                tokens.append(Token(TokenType.COMMA, self.cur))
                self.advance()
                continue

            elif self.cur and (self.cur in SYMBOL_ALLOWED_CHARS or self.cur.isdigit()):
                buffer = self.cur
                self.advance()
                while self.cur and (self.cur in SYMBOL_ALLOWED_CHARS or self.cur.isdigit()):
                    buffer += self.cur
                    self.advance()

                if buffer in InstructionType._value2member_map_:
                    tokens.append(Token(TokenType.INST, buffer))
                    continue

                purely_numeric = False
                try:
                    int(buffer)
                    purely_numeric = True
                except:
                    pass
                if purely_numeric:
                    raise Exception("Operator cannot be purely numeric")
                                
                if buffer[0] in "r" and buffer[1:].isdigit():
                    tokens.append(Token(TokenType.REGISTER, buffer[1:]))
                    continue

                if buffer[0] in "i":
                    try:
                        int(buffer[1:])
                        tokens.append(Token(TokenType.IMMEDIATE, buffer[1:]))
                        continue
                    except:
                        if buffer[1] == "n":
                            try:
                                int(buffer[2:])
                                tokens.append(Token(TokenType.IMMEDIATE, str(int(buffer[2:])*-1)))
                                continue
                            except:
                                pass
                

                tokens.append(Token(TokenType.SYMBOL, buffer))
                continue

            else:
                self.advance()

        return tokens
