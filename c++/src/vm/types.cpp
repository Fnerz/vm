#ifndef TYPES_CPP
#define TYPES_CPP

#include "./types.hpp"


std::string tokenRepr(Token tok)
{
    std::string prefix = "";
    if (tok.token_type == TokenType::IMMEDIATE)
    {
        prefix = "i";
    }
    else if (tok.token_type == TokenType::REGISTER)
    {
        prefix = "r";
    }
    return prefix + tok.value;
}

#endif