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

Token symbolToToken(std::string symbol)
{
    Token tok;
    if (symbol.length() <= 1)
    {
        tok.token_type = TokenType::null;
        return tok;
    }
    
    if (STR_TO_INSTRUCTION_TYPE.contains(symbol))
    {
        tok.token_type = TokenType::INST;
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
            tok.token_type = TokenType::REGISTER;
            tok.value = shortend_symbol;
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
            tok.token_type = TokenType::IMMEDIATE;
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
                    tok.token_type = TokenType::IMMEDIATE;
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

    tok.token_type = TokenType::SYMBOL;
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
            tok.token_type = TokenType::LPAREN;
            tok.value = "[";
            tokens.push_back(tok);
        }
        else if (this->cur == ']')
        {
            Token tok;
            tok.token_type = TokenType::RPAREN;
            tok.value = "]";
            tokens.push_back(tok);
        }
        else if (this->cur == '+')
        {
            Token tok;
            tok.token_type = TokenType::PLUS;
            tok.value = "+";
            tokens.push_back(tok);
        }
        else if (this->cur == '-')
        {
            Token tok;
            tok.token_type = TokenType::MINUS;
            tok.value = "-";
            tokens.push_back(tok);
        }
        else if (this->cur == ':')
        {
            Token tok;
            tok.token_type = TokenType::COLON;
            tok.value = ":";
            tokens.push_back(tok);
        }
        else if (this->cur == ',')
        {
            Token tok;
            tok.token_type = TokenType::COMMA;
            tok.value = ",";
            tokens.push_back(tok);
        }
        else if (this->cur == ',')
        {
            Token tok;
            tok.token_type = TokenType::COMMA;
            tok.value = ",";
            tokens.push_back(tok);
        }
        else if (ALLOWED_SYMBOL_CHARS.find(this->cur) != std::string::npos)
        {
            std::string symbol = this->getSymbol();
            Token tok = symbolToToken(symbol);
            if (tok.token_type == TokenType::null)
            {
                std::cout << "Invalid symbol: " << symbol << std::endl;
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