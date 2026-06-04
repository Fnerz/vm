#ifndef LineLexer_CPP
#define LineLexer_CPP

#include "Lexer.hpp"


LineLexer::LineLexer(std::string line)
{
    this->line = line;
    this->advance();
    return;
}

char LineLexer::advance()
{
    this->i += 1;
    this->cur = (this->line.length() > this->i) ? this->line[this->i] : '\0';
    this->next = (this->line.length() > this->i+1) ? this->line[this->i+1] : '\0';
    return this->cur;
}

bool isSpace(char c)
{
    return (c == ' ' || c == '\t');
}


std::string LineLexer::getSymbol()
{
    std::string buffer = "";
    while (ALLOWED_SYMBOL_CHARS.find(this->cur) != std::string::npos)
    {
        buffer += this->cur;
        this->advance();
    }
    return buffer;
}

void userEndChoice()
{
    std::cout << "using this register can result in undefined and/or unwanted behavior" << std::endl;
    std::cout << "do you want to continue? y/n" << std::endl;
    char choice;
    while (true)
    {
        std::cin >> choice;
        if (choice == 'y')
        {
            break;
        }
        if (choice == 'n')
        {
            exit(1);
        }
    }
    return;
}

void checkRegisterSafety(Token tok)
{
    int reg = std::stoi(tok.value);
    if (reg == 29)
    {
        std::cout << "r29 is reserved for keyboard input" << std::endl;
        userEndChoice();
    }
    if (reg >= 30 && reg <= 33)
    {
        std::cout << "r30-r33 are reserved for temp values" << std::endl;
        userEndChoice();
    }
    if (reg > 33)
    {
        std::cout << "no such register r" << reg << ". Safe registers are r0-r28" << std::endl;
        exit(1);
    }
}

Token symbolToToken(std::string symbol)
{
    Token tok;
    if (symbol.length() <= 1)
    {
        tok.type = TokenType::null;
        return tok;
    }
    
    if (STR_TO_INSTRUCTION_TYPE.contains(symbol))
    {
        tok.type = TokenType::INST;
        tok.value = symbol;
        return tok;
    }

    std::string shortend_symbol = "";
    if (symbol[0] == 'r')
    {
        try
        {
            shortend_symbol = symbol.substr(1);
            std::stoi(shortend_symbol);
            tok.type = TokenType::REGISTER;
            tok.value = shortend_symbol;
            checkRegisterSafety(tok);
            return tok;
        }
        catch(const std::exception& e)
        {
        }
    }
    else if (symbol[0] == 'i')
    {
        try
        {
            shortend_symbol = symbol.substr(1);
            std::stoi(shortend_symbol); 
            tok.type = TokenType::IMMEDIATE_I;
            tok.value = shortend_symbol;
            return tok;
        }
        catch(const std::exception& e)
        {
            if (symbol[1] == 'n')
            {
                try
                {
                    shortend_symbol = symbol.substr(2);
                    std::stoi(shortend_symbol);
                    tok.type = TokenType::IMMEDIATE_I;
                    tok.value = "-";
                    tok.value += shortend_symbol;
                    return tok;
                }
                catch(const std::exception& e)
                {
                }
            }
        }
    }
    else if (symbol[0] == 'f')
    {
        try
        {
            shortend_symbol = symbol.substr(1);
            std::stof(shortend_symbol); 
            tok.type = TokenType::IMMEDIATE_F;
            tok.value = shortend_symbol;
            return tok;
        }
        catch(const std::exception& e)
        {
            if (symbol[1] == 'n')
            {
                try
                {
                    shortend_symbol = symbol.substr(2);
                    std::stof(shortend_symbol);
                    tok.type = TokenType::IMMEDIATE_F;
                    tok.value = "-";
                    tok.value += shortend_symbol;
                    return tok;
                }
                catch(const std::exception& e)
                {
                }
            }
        }
    }

    tok.type = TokenType::SYMBOL;
    tok.value = symbol;

    return tok;
}



std::vector<Token> LineLexer::tokenize()
{
    std::vector<Token> tokens = {};

    while (this->cur != '\0')
    {
        if (isSpace(this->cur))
        {
            this->advance();
            continue;
        }
        else if (this->cur == ';')
        {
            break;
        }
        else if (this->cur == '[')
        {
            Token tok;
            tok.type = TokenType::LPAREN;
            tok.value = "[";
            tokens.push_back(tok);
        }
        else if (this->cur == ']')
        {
            Token tok;
            tok.type = TokenType::RPAREN;
            tok.value = "]";
            tokens.push_back(tok);
        }
        else if (this->cur == '+')
        {
            Token tok;
            tok.type = TokenType::PLUS;
            tok.value = "+";
            tokens.push_back(tok);
        }
        else if (this->cur == '-')
        {
            Token tok;
            tok.type = TokenType::MINUS;
            tok.value = "-";
            tokens.push_back(tok);
        }
        else if (this->cur == ':')
        {
            Token tok;
            tok.type = TokenType::COLON;
            tok.value = ":";
            tokens.push_back(tok);
        }
        else if (this->cur == ',')
        {
            Token tok;
            tok.type = TokenType::COMMA;
            tok.value = ",";
            tokens.push_back(tok);
        }
        else if (ALLOWED_SYMBOL_CHARS.find(this->cur) != std::string::npos)
        {
            std::string symbol = this->getSymbol();
            Token tok = symbolToToken(symbol);
            if (tok.type == TokenType::null)
            {
                std::cout << "Invalid symbol in line " << this->i << ":" << symbol << std::endl;
                exit(1);
            }
            tokens.push_back(tok);
            continue;
        }
        this->advance();
    }
    return tokens;
}


#endif