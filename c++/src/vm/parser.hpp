#ifndef PARSER_HPP
#define PARSER_HPP

#include <algorithm>
#include "types.hpp"

class LineParser
{
private:
    std::vector<Token> tokens = {};
    int i = -1;
    Token cur;
    Token next;

    Token advance();
    Token eat(TokenType type);
    Token eats(std::vector<TokenType> types);

    Node parsePointerArithmetic();
    Node getValue();
    
    public:
    LineParser(std::vector<Token> tokens);
    std::vector<InstructionIr> parse();
};

#endif