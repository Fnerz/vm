#ifndef TYPES_CPP
#define TYPES_CPP

#include "./types.hpp"


std::string tokenTypeRepr(TokenType tok_type)
{
    std::map<TokenType, std::string> conversion_map = {
    {TokenType::null, "null"},
    {TokenType::INST, "INST"},
    {TokenType::REGISTER, "REGISTER"},
    {TokenType::IMMEDIATE_I, "IMMEDIATE_I"},
    {TokenType::IMMEDIATE_F, "IMMEDIATE_F"},
    {TokenType::IMMEDIATE_C, "IMMEDIATE_C"},
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
    std::string suffix = "";
    if (tok.type == TokenType::IMMEDIATE_I)
    {
        prefix = "i";
    }
    if (tok.type == TokenType::IMMEDIATE_F)
    {
        prefix = "i";
    }
    if (tok.type == TokenType::IMMEDIATE_C)
    {
        prefix = "'";
        suffix = "'";
    }
    else if (tok.type == TokenType::REGISTER)
    {
        prefix = "r";
    }
    return prefix + tok.value + suffix;
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
        return tokenTypeRepr(std::get<Token>(node).type) + " " + tokenRepr(std::get<Token>(node));
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
    ret += " Arg1: ";
    ret += nodeRepr(inst_ir.arg1);
    ret += ", Arg2: ";
    ret += nodeRepr(inst_ir.arg2);
    ret += ", Arg3: ";
    ret += nodeRepr(inst_ir.arg3);
    return ret;
}

std::string instructionRepr(Instruction inst)
{
    std::string ret = instructionTypeRepr(inst.type);
    ret += " ";


    size_t count = sizeof(inst.args) / sizeof(inst.args[0]);
    for (size_t i = 0; i < count; ++i)
    {
        if (inst.arg_types[i] == ArgType::REGISTER)
        {
            ret += "r";
        }
        if (inst.arg_types[i] == ArgType::IMMEDIATE_I)
        {
            ret += "i";
        }
        if (inst.arg_types[i] == ArgType::IMMEDIATE_F)
        {
            ret += "f";
        }
        if (inst.arg_types[i] == ArgType::POINTER_R)
        {
            ret += "p";
        }

        if (inst.arg_types[i] == ArgType::LABEL_INDEX)
        {
            ret += std::to_string(static_cast<int>(inst.args[i]));
        } 
        else
        {
            ret += std::to_string(inst.args[i]);
        }

        if (i+1 != count)
        {
            ret += ", ";
        }
    }
    return ret;
}

bool isAbsoluteJmpInst(InstructionType jmp_inst_type)
{
    return (
        (jmp_inst_type == InstructionType::JMPA)||
        (jmp_inst_type == InstructionType::JEA) ||
        (jmp_inst_type == InstructionType::JNEA)||
        (jmp_inst_type == InstructionType::JGA) ||
        (jmp_inst_type == InstructionType::JLA) ||
        (jmp_inst_type == InstructionType::JGEA)||
        (jmp_inst_type == InstructionType::JLEA));
}

#endif