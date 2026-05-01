#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include "types.hpp"

class LineLexer
{
private:
    std::string line = "";
    int i = -1;
    char cur;
    char next;


    std::string getSymbol();
    char advance();
public:
    LineLexer(std::string line);

    std::vector<Token> tokenize();


};

#endif