#ifndef PARSER_CPP
#define PARSER_CPP

#include "parser.hpp"

LineParser::LineParser(std::vector<Token> tokens)
{
    this->tokens = tokens;

    this->advance();
}


Token LineParser::advance()
{
    Token null_token;
    null_token.type = TokenType::null;
    this->i++;
    this->cur = (this->i < this->tokens.size()) ? this->tokens[this->i] : null_token;
    this->next = (this->i+1 < this->tokens.size()) ? this->tokens[this->i+1] : null_token;
    return this->cur;
}

/*
Checks if this->cur.token_type is equal to the proivded value list, and throws an exception if not. it stores this->cur and advances. returns the stores this->cur. 
*/
Token LineParser::eat(TokenType type)
{
    Token last = this->cur;
    if (this->cur.type != type)
    {
        std::cout << "Expected token of type " << tokenTypeRepr(type) << ", received " << tokenTypeRepr(this->cur.type) << std::endl;
        for (auto tok : this->tokens)
        {
            std::cout << tokenRepr(tok) << "(" << tokenTypeRepr(tok.type) << ")" << " ";
        }
        std::cout << std::endl;
        exit(1);
    }
    this->advance();
    return last;
}

/*
Checks if this->cur.token_type is in the provided list, and throws an exception if not. it stores this->cur and advances. returns the stores this->cur. 
*/
Token LineParser::eats(std::vector<TokenType> types)
{
    Token last = this->cur;
    if (! inTokenTypes(this->cur.type, types))
    {
        std::cout << "Expected token of type " << tokenTypesRepr(types) << ", received " << tokenTypeRepr(this->cur.type) << std::endl;
        exit(0);
    }
    this->advance();
    return last;
}

/*
expects cur to be LPAREN. passes beyond RPAREN
*/
Node LineParser::parsePointerArithmetic()
{
    this->eat(TokenType::LPAREN);
    
    Node left = this->cur;
    this->eats({TokenType::REGISTER, TokenType::IMMEDIATE_I, TokenType::IMMEDIATE_F});
    
    while (inTokenTypes(this->cur.type, {TokenType::PLUS, TokenType::MINUS}))
    {
        BinOp bin_op;
        bin_op.left = left;
        
        bin_op.op = this->cur;
        this->eats({TokenType::PLUS, TokenType::MINUS});
        
        bin_op.right = this->cur;
        this->eats({TokenType::REGISTER, TokenType::IMMEDIATE_I, TokenType::IMMEDIATE_I});
        
        left = std::make_shared<BinOp>(bin_op);
    }
    
    this->eat(TokenType::RPAREN);

    if (std::holds_alternative<Token>(left))
    {
        Token &tok = std::get<Token>(left);
        if (tok.type == TokenType::IMMEDIATE_I)
        {
            tok.type = TokenType::POINTER_I;
        }
        else
        {
            tok.type = TokenType::POINTER_R;
        }
    }
    return left;    
}

/*
Expects to be on either a value token (i.e. register or imm) or on LPAREN. It passes beyond the full value, including if givin, RPAREN. 
*/
Node LineParser::getValue()
{
    if (inTokenTypes(this->cur.type, {TokenType::REGISTER, TokenType::IMMEDIATE_I, TokenType::IMMEDIATE_F}))
    {
        Token last = this->cur;
        this->advance();
        return last;
    }
    else if (this->cur.type == TokenType::LPAREN)
    {
        return this->parsePointerArithmetic();
    }
    else
    {
        std::cout << "Expected value token, received " << tokenTypeRepr(this->cur.type) << " " << tokenRepr(this->cur) << std::endl;
        exit(1);
    }
}

std::vector<InstructionIr> LineParser::parse()
{
    std::vector<InstructionIr> instructions = {};
    while (this->cur.type != TokenType::null)
    {
        InstructionIr inst;
        if (this->cur.value == "mov")
        {
            inst.type = InstructionType::MOV;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "addi")
        {
            inst.type = InstructionType::ADDI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "addf")
        {
            inst.type = InstructionType::ADDF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "addu")
        {
            inst.type = InstructionType::ADDU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "subi")
        {
            inst.type = InstructionType::SUBI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "subf")
        {
            inst.type = InstructionType::SUBF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "subu")
        {
            inst.type = InstructionType::SUBU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "muli")
        {
            inst.type = InstructionType::MULI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "mulf")
        {
            inst.type = InstructionType::MULF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "mulu")
        {
            inst.type = InstructionType::MULU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "divi")
        {
            inst.type = InstructionType::DIVI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "divf")
        {
            inst.type = InstructionType::DIVF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "divu")
        {
            inst.type = InstructionType::DIVU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "modi")
        {
            inst.type = InstructionType::MODI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "modf")
        {
            inst.type = InstructionType::MODF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "absi")
        {
            inst.type = InstructionType::ABSI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "absf")
        {
            inst.type = InstructionType::ABSF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "printi")
        {
            inst.type = InstructionType::PRINTI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "printf")
        {
            inst.type = InstructionType::PRINTF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "printc")
        {
            inst.type = InstructionType::PRINTC;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "printu")
        {
            inst.type = InstructionType::PRINTU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "open")
        {
            inst.type = InstructionType::OPEN;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "read")
        {
            inst.type = InstructionType::READ;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "write")
        {
            inst.type = InstructionType::WRITE;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "close")
        {
            inst.type = InstructionType::CLOSE;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "deci")
        {
            inst.type = InstructionType::DECI;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "decf")
        {
            inst.type = InstructionType::DECF;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "decu")
        {
            inst.type = InstructionType::DECU;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "inci")
        {
            inst.type = InstructionType::INCI;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "incf")
        {
            inst.type = InstructionType::INCF;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "incu")
        {
            inst.type = InstructionType::INCU;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jmp")  
        {
            inst.type = InstructionType::JMP;
            this->advance();
            inst.arg1 = this->eats({TokenType::SYMBOL, TokenType::IMMEDIATE_I});
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "je")  
        {
            inst.type = InstructionType::JE;
            this->advance();
            inst.arg1 = this->eats({TokenType::SYMBOL, TokenType::IMMEDIATE_I});
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jne")  
        {
            inst.type = InstructionType::JNE;
            this->advance();
            inst.arg1 = this->eats({TokenType::SYMBOL, TokenType::IMMEDIATE_I});
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jg")  
        {
            inst.type = InstructionType::JG;
            this->advance();
            inst.arg1 = this->eats({TokenType::SYMBOL, TokenType::IMMEDIATE_I});
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jge")  
        {
            inst.type = InstructionType::JGE;
            this->advance();
            inst.arg1 = this->eats({TokenType::SYMBOL, TokenType::IMMEDIATE_I});
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jl")  
        {
            inst.type = InstructionType::JL;
            this->advance();
            inst.arg1 = this->eats({TokenType::SYMBOL, TokenType::IMMEDIATE_I});
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jle")  
        {
            inst.type = InstructionType::JLE;
            this->advance();
            inst.arg1 = this->eats({TokenType::SYMBOL, TokenType::IMMEDIATE_I});
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "cmpi")
        {
            inst.type = InstructionType::CMPI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "cmpf")
        {
            inst.type = InstructionType::CMPF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "cmpu")
        {
            inst.type = InstructionType::CMPU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "load")
        {
            inst.type = InstructionType::LOAD;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "store")
        {
            inst.type = InstructionType::STORE;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "bp")
        {
            inst.type = InstructionType::BREAKPOINT;
            this->advance();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "call")  
        {
            inst.type = InstructionType::CALL;
            this->advance();
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "ret")  
        {
            inst.type = InstructionType::RET;
            this->advance();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "push")
        {
            inst.type = InstructionType::PUSH;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
        }
        else if (this->cur.value == "pop")
        {
            inst.type = InstructionType::POP;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
        }
        else if (this->cur.value == "time")
        {
            inst.type = InstructionType::TIME;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
        }
        else if (this->cur.value == "itof")
        {
            inst.type = InstructionType::ITOF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "itou")
        {
            inst.type = InstructionType::ITOU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "ftoi")
        {
            inst.type = InstructionType::FTOI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "ftou")
        {
            inst.type = InstructionType::FTOU;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "utof")
        {
            inst.type = InstructionType::UTOF;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "utoi")
        {
            inst.type = InstructionType::UTOI;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "hlt")
        {
            inst.type = InstructionType::HALT;
            this->advance();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.type == TokenType::SYMBOL)
        {
            inst.type = InstructionType::LABEL;
            inst.arg1 = this->cur.value;
            this->advance();
            this->eat(TokenType::COLON);
            instructions.push_back(inst);
        }
        else
        {
            std::cout << "Unexpected token: " << tokenTypeRepr(this->cur.type) << " " << tokenRepr(this->cur) << std::endl;
            for (auto tok : this->tokens)
            {
                std::cout << tokenRepr(tok) << " ";
            }
            std::cout << std::endl;
            exit(0);
        }
    }
    return instructions;   
}

#endif