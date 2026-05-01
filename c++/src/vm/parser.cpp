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
    null_token.token_type = TokenType::null;
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
    if (this->cur.token_type != type)
    {
        std::cout << "Expected token of type " << tokenTypeRepr(type) << ", received " << tokenTypeRepr(this->cur.token_type) << std::endl;
        exit(0);
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
    if (! inTokenTypes(this->cur.token_type, types))
    {
        std::cout << "Expected token of type " << tokenTypesRepr(types) << ", received " << tokenTypeRepr(this->cur.token_type) << std::endl;
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
    this->eats({TokenType::REGISTER, TokenType::IMMEDIATE});


    while (inTokenTypes(this->cur.token_type, {TokenType::PLUS, TokenType::MINUS}))
    {
        BinOp bin_op;
        bin_op.left = left;
        
        bin_op.op = this->cur;
        this->eats({TokenType::PLUS, TokenType::MINUS});
        
        bin_op.right = this->cur;
        this->eats({TokenType::REGISTER, TokenType::IMMEDIATE});
        
        left = std::make_shared<BinOp>(bin_op);
    }

    this->eat(TokenType::RPAREN);
    return left;    
}

/*
Expects to be on either a value token (i.e. register or imm) or on LPAREN. It passes beyond the full value, including if givin, RPAREN. 
*/
Node LineParser::getValue()
{
    if (inTokenTypes(this->cur.token_type, {TokenType::REGISTER, TokenType::IMMEDIATE}))
    {
        Token last = this->cur;
        this->advance();
        return last;
    }
    else if (this->cur.token_type == TokenType::LPAREN)
    {
        return this->parsePointerArithmetic();
    }
    else
    {
        std::cout << "Expected value token, received " << tokenTypeRepr(this->cur.token_type) << " " << tokenRepr(this->cur) << std::endl;
        exit(1);
    }
}

std::vector<InstructionIr> LineParser::parse()
{
    std::vector<InstructionIr> instructions = {};
    while (this->cur.token_type != TokenType::null)
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
        else if (this->cur.value == "add")
        {
            inst.type = InstructionType::ADD;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "sub")
        {
            inst.type = InstructionType::SUB;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "mul")
        {
            inst.type = InstructionType::MUL;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "idiv")
        {
            inst.type = InstructionType::IDIV;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "fdiv")
        {
            inst.type = InstructionType::FDIV;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "mod")
        {
            inst.type = InstructionType::MOD;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg3  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "abs")
        {
            inst.type = InstructionType::MUL;
            this->advance();
            inst.arg1 = this->getValue();
            this->eat(TokenType::COMMA);
            inst.arg2  = this->getValue();
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jmp")  
        {
            inst.type = InstructionType::JMP;
            this->advance();
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "je")  
        {
            inst.type = InstructionType::JE;
            this->advance();
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jne")  
        {
            inst.type = InstructionType::JNE;
            this->advance();
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jg")  
        {
            inst.type = InstructionType::JG;
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jge")  
        {
            inst.type = InstructionType::JGE;
            this->advance();
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jl")  
        {
            inst.type = InstructionType::JL;
            this->advance();
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "jle")  
        {
            inst.type = InstructionType::JLE;
            this->advance();
            inst.arg1 = this->eat(TokenType::SYMBOL);
            instructions.push_back(inst);
            continue;
        }
        else if (this->cur.value == "cmp")
        {
            inst.type = InstructionType::CMP;
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
        else if (this->cur.value == "print")
        {
            inst.type = InstructionType::PRINT;
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
        else if (this->cur.value == "dec")
        {
            inst.type = InstructionType::DEC;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
        }
        else if (this->cur.value == "inc")
        {
            inst.type = InstructionType::INC;
            this->advance();
            inst.arg1 = this->getValue();
            instructions.push_back(inst);
        }
        else if (this->cur.value == "input")
        {
            inst.type = InstructionType::INPUT;
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
        else if (this->cur.token_type == TokenType::SYMBOL)
        {
            inst.type = InstructionType::LABEL;
            inst.arg1 = this->cur;
            this->advance();
            this->eat(TokenType::COLON);
            instructions.push_back(inst);
        }
        else
        {
            std::cout << "Unexpected token: " << tokenTypeRepr(this->cur.token_type) << " " << tokenRepr(this->cur) << std::endl;
            exit(0);
        }
    }
    return instructions;   
}

#endif