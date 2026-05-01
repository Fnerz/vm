#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <iostream>
#include <variant>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>


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
std::string tokenTypeRepr(TokenType tok_type);
std::string tokenTypesRepr(std::vector<TokenType> tok_type);
bool inTokenTypes(TokenType tok_type, std::vector<TokenType> tok_types);


enum InstructionType
{
    MOV,
    ADD,
    SUB,
    MUL,
    IDIV,
    FDIV,
    MOD,
    ABS,
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
    TIME,
    HALT,
};

inline std::map<std::string, InstructionType> STR_TO_INSTRUCTION_TYPE = {
    {"mov", InstructionType::MOV},
    {"add", InstructionType::ADD},
    {"sub", InstructionType::SUB},
    {"mul", InstructionType::MUL},
    {"idiv", InstructionType::IDIV},
    {"fdiv", InstructionType::FDIV},
    {"MOD", InstructionType::MOD},
    {"ABS", InstructionType::ABS},
    {"cmp", InstructionType::CMP},
    {"jmp" ,InstructionType::JMP},
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
    {"input", InstructionType::INPUT},
    {"time", InstructionType::TIME},
    {"halt", InstructionType::HALT},
};

inline std::map<InstructionType, std::string> INST_TYPE_TO_STR = {
    {InstructionType::MOV ,"mov"},
    {InstructionType::ADD ,"add"},
    {InstructionType::SUB ,"sub"},
    {InstructionType::MUL ,"mul"},
    {InstructionType::IDIV ,"idiv"},
    {InstructionType::FDIV ,"fdiv"},
    {InstructionType::MOD ,"MOD"},
    {InstructionType::ABS ,"ABS"},
    {InstructionType::CMP ,"cmp"},
    {InstructionType::JMP ,"jmp"},
    {InstructionType::JE ,"je"},
    {InstructionType::JNE ,"jne"},
    {InstructionType::JG ,"jg"},
    {InstructionType::JL ,"jl"},
    {InstructionType::JGE ,"jge"},
    {InstructionType::JLE ,"jle"},
    {InstructionType::LOAD ,"load"},
    {InstructionType::STORE ,"store"},
    {InstructionType::PRINT ,"print"},
    {InstructionType::BREAKPOINT ,"bp"},
    {InstructionType::SCREEN ,"screen"},
    {InstructionType::RENDER ,"render"},
    {InstructionType::CALL ,"call"},
    {InstructionType::RET ,"ret"},
    {InstructionType::PUSH ,"push"},
    {InstructionType::POP ,"pop"},
    {InstructionType::DEC ,"dec"},
    {InstructionType::INC ,"inc"},
    {InstructionType::INPUT ,"input"},
    {InstructionType::TIME ,"time"},
    {InstructionType::HALT, "halt"},
};
std::string instructionTypeRepr(InstructionType inst_type);

struct Token
{
    TokenType token_type;
    std::string value;
};
std::string tokenRepr(Token tok);

struct BinOp;
typedef std::variant<Token, std::string, std::shared_ptr<BinOp>> Node;
struct BinOp {
    Node left;
    Token op;
    Node right;
};
std::string nodeRepr(Node node);
std::string BinOpRepr(std::shared_ptr<BinOp> bin_op);

/*
Instruction intermediate representation
*/
struct InstructionIr
{
    InstructionType type;
    Node arg1;
    Node arg2;
    Node arg3;
};
std::string instructionIrRepr(InstructionIr inst_ir);

struct Instruction
{
    InstructionType type;
    Token arg1;
    Token arg2;
    Token arg3;
};

inline std::string ALLOWED_SYMBOL_CHARS = "abcdefghijklmnopqrstuvwxyz0123456789_";


#endif