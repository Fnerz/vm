#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <iostream>
#include <variant>
#include <memory>
#include <vector>
#include <map>


enum TokenType
{
    null,
    INST,
    REGISTER,
    IMMEDIATE,
    SYMBOL,
    PLUS,
    MINUS,
    LPAREN,
    RPAREN,
    COLON,
    COMMA,
};


enum InstructionType
{
    MOV,
    ADD,
    SUB,
    MUL,
    IDIV,
    FDIV,
    CMP,
    JMP,
    JE,
    JNE,
    JG,
    JL,
    JGE,
    JLE,
    LOAD,
    STORE,
    PRINT,
    PRINTV,
    BREAKPOINT,
    SCREEN,
    RENDER,
    CALL,
    RET,
    PUSH,
    POP,
    DEC,
    INC,
    INPUT,
};

inline std::map<std::string, InstructionType> STR_TO_INSTRUCTION_TYPE = {
    {"mov", InstructionType::MOV},
    {"add", InstructionType::ADD},
    {"sub", InstructionType::SUB},
    {"mul", InstructionType::MUL},
    {"idiv", InstructionType::IDIV},
    {"fdiv", InstructionType::FDIV},
    {"cmp", InstructionType::CMP},
    {"cmp" ,InstructionType::JMP},
    {"je" ,InstructionType::JE},
    {"jne" ,InstructionType::JNE},
    {"jg" ,InstructionType::JG},
    {"jl" ,InstructionType::JL},
    {"jge" ,InstructionType::JGE},
    {"jle" ,InstructionType::JLE},
    {"load" ,InstructionType::LOAD},
    {"store" ,InstructionType::STORE},
    {"print" ,InstructionType::PRINT},
    {"bp" ,InstructionType::BREAKPOINT},
    {"screen" ,InstructionType::SCREEN},
    {"render" ,InstructionType::RENDER},
    {"call" ,InstructionType::CALL},
    {"ret" ,InstructionType::RET},
    {"push" ,InstructionType::PUSH},
    {"pop" ,InstructionType::POP},
    {"dec" ,InstructionType::DEC},
    {"inc" ,InstructionType::INC},
    {"input", InstructionType::INPUT}
};

struct Token
{
    TokenType token_type;
    std::string value;
};
std::string tokenRepr(Token tok);

struct BinOp;
typedef std::variant<Token, std::shared_ptr<BinOp>> Node;

struct Instruction
{
    InstructionType type;
    std::vector<Node> args;
};

inline std::string ALLOWED_SYMBOL_CHARS = "abcdefghijklmnopqrstuvwxyz0123456789_";


#endif