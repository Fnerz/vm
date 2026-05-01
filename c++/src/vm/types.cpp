#ifndef TYPES_CPP
#define TYPES_CPP

#include "./types.hpp"


std::string tokenTypeRepr(TokenType tok_type)
{
    std::map<TokenType, std::string> conversion_map = {
    {TokenType::null, "null"},
    {TokenType::INST, "INST"},
    {TokenType::REGISTER, "REGISTER"},
    {TokenType::IMMEDIATE, "IMMEDIATE"},
    {TokenType::SYMBOL, "SYMBOL"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::COLON, "COLON"},
    {TokenType::COMMA, "COMMA"},
    };
    return conversion_map[tok_type];
}

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

std::string tokenTypesRepr(std::vector<TokenType> tok_types)
{
    std::string ret = "[";

    for (int i = 0; i < tok_types.size(); i++)
    {
        ret += tokenTypeRepr(tok_types[i]);
        if (i+1 != tok_types.size())
        {
            ret += ", ";
        }
    }

    return ret + "]";
}

bool inTokenTypes(TokenType tok_type, std::vector<TokenType> tok_types)
{
    auto it = std::find(tok_types.begin(), tok_types.end(), tok_type);
    return (it != tok_types.end());
}


std::string nodeRepr(Node node)
{
    if (std::holds_alternative<std::string>(node))
    {
        return (std::get<std::string>(node));
    }
    else if (std::holds_alternative<Token>(node))
    {
        return tokenTypeRepr(std::get<Token>(node).token_type) + " " + tokenRepr(std::get<Token>(node));
    }
    else if (std::holds_alternative<std::shared_ptr<BinOp>>(node))
    {
        return BinOpRepr(std::get<std::shared_ptr<BinOp>>(node));
    }
    return "Empty node";
}


std::string BinOpRepr(std::shared_ptr<BinOp> bin_op)
{
    std::string ret = "";
    ret += nodeRepr(bin_op->left);
    ret += " ";
    ret += tokenRepr(bin_op->op);
    ret += " ";
    ret += nodeRepr(bin_op->right);
    return "[" + ret + "]";
}

std::string instructionTypeRepr(InstructionType inst_type)
{
    return INST_TYPE_TO_STR[inst_type];
}

std::string instructionIrRepr(InstructionIr inst_ir)
{
    std::string ret = "";
    ret += instructionTypeRepr(inst_ir.type);
    ret += ", Arg1: ";
    ret += nodeRepr(inst_ir.arg1);
    ret += ", Arg2: ";
    ret += nodeRepr(inst_ir.arg2);
    ret += ", Arg3: ";
    ret += nodeRepr(inst_ir.arg3);
    return ret;
}

#endif