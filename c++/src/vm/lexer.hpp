#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include "types.hpp"

class Lexer
{
private:
    std::string line = "";
    int i = -1;
    char cur;
    char next;


    std::string getSymbol();
    char advance();
public:
    Lexer(std::string line);

    std::vector<Token> tokenize();


};

#endif