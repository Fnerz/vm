#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <iostream>
#include <variant>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>


enum class TokenType
{
    null, // not in caps cause of confilictions with the actual NULL
    INST,
    REGISTER,
    POINTER_R,
    POINTER_I,
    IMMEDIATE_I,
    IMMEDIATE_F,
    IMMEDIATE_C,
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


enum class InstructionType
{
    MOV = 1,
    // arithmetic
    ADDI,
    ADDF,
    ADDU,
    SUBI,
    SUBF,
    SUBU,
    MULI,
    MULF,
    MULU,
    DIVI,
    DIVF,
    DIVU,
    MODI,
    MODF,
    ABSI,
    ABSF,

    // comparision
    CMPI,
    CMPU,
    CMPF,

    // label jumps
    JMP,
    JE,
    JNE,
    JG,
    JL,
    JGE,
    JLE,

    // absolute jumps
    JMPA,
    JEA,
    JNEA,
    JGA,
    JLA,
    JGEA,
    JLEA,

    // memory
    LOAD,
    STORE,

    // debug io, will be removed later once io threw display is working
    PRINTI,
    PRINTF,
    PRINTU,
    PRINTC,

    // debug
    BREAKPOINT,

    // screen io. will probably be replaced threw a memroy bus
    SCREEN,
    RENDER,

    // "functions"
    CALL,
    RET,

    // stack
    PUSH,
    POP,

    // increment // decrement
    DECI,
    DECF,
    DECU,
    INCI,
    INCF,
    INCU,

    // sys
    TIME,
    HALT,

    // only used for the assembler
    LABEL,

    // conversion
    ITOF,
    ITOU,
    FTOI,
    FTOU,
    UTOF,
    UTOI,
};

inline std::map<std::string, InstructionType> STR_TO_INSTRUCTION_TYPE = {
    {"mov", InstructionType::MOV},
    {"addi", InstructionType::ADDI},
    {"addf", InstructionType::ADDF},
    {"addu", InstructionType::ADDU},
    {"subi", InstructionType::SUBI},
    {"subf", InstructionType::SUBF},
    {"subu", InstructionType::SUBU},
    {"muli", InstructionType::MULI},
    {"mulf", InstructionType::MULF},
    {"mulu", InstructionType::MULU},
    {"div", InstructionType::DIVI},
    {"divf", InstructionType::DIVF},
    {"modi", InstructionType::MODI},
    {"modf", InstructionType::MODF},
    {"ABSI", InstructionType::ABSI},
    {"ABSF", InstructionType::ABSF},
    {"cmpi", InstructionType::CMPI},
    {"cmpu", InstructionType::CMPU},
    {"cmpf", InstructionType::CMPF},
    {"jmp" ,InstructionType::JMP},
    {"je" ,InstructionType::JE},
    {"jne" ,InstructionType::JNE},
    {"jg" ,InstructionType::JG},
    {"jl" ,InstructionType::JL},
    {"jge" ,InstructionType::JGE},
    {"jle" ,InstructionType::JLE},
    {"jmpa", InstructionType::JMPA},
    {"jea", InstructionType::JEA},
    {"jnea", InstructionType::JNEA},
    {"jga", InstructionType::JGA},
    {"jla", InstructionType::JLA},
    {"jgea", InstructionType::JGEA},
    {"jlea", InstructionType::JLEA},
    {"load" ,InstructionType::LOAD},
    {"store" ,InstructionType::STORE},
    {"printi" ,InstructionType::PRINTI},
    {"printf" ,InstructionType::PRINTF},
    {"printc" ,InstructionType::PRINTC},
    {"printu" ,InstructionType::PRINTU},
    {"bp" ,InstructionType::BREAKPOINT},
    {"screen" ,InstructionType::SCREEN},
    {"render" ,InstructionType::RENDER},
    {"call" ,InstructionType::CALL},
    {"ret" ,InstructionType::RET},
    {"push" ,InstructionType::PUSH},
    {"pop" ,InstructionType::POP},
    {"deci" ,InstructionType::DECI},
    {"decf" ,InstructionType::DECF},
    {"decu" ,InstructionType::DECU},
    {"inci" ,InstructionType::INCI},
    {"incf" ,InstructionType::INCF},
    {"incu" ,InstructionType::INCU},
    {"time", InstructionType::TIME},
    {"hlt", InstructionType::HALT},
    {"label", InstructionType::LABEL},
    {"itof", InstructionType::ITOF},
    {"itou", InstructionType::ITOU},
    {"foti", InstructionType::FTOI},
    {"ftou", InstructionType::FTOU},
    {"utof", InstructionType::UTOF},
    {"utoi", InstructionType::UTOI},
};

inline std::map<InstructionType, std::string> INST_TYPE_TO_STR = {
    {InstructionType::MOV, "mov"},
    {InstructionType::ADDI, "addi"},
    {InstructionType::ADDF, "addf"},
    {InstructionType::ADDU, "addu"},
    {InstructionType::SUBI, "subi"},
    {InstructionType::SUBF, "subf"},
    {InstructionType::SUBU, "subu"},
    {InstructionType::MULI, "muli"},
    {InstructionType::MULF, "mulf"},
    {InstructionType::MULU, "mulu"},
    {InstructionType::DIVI, "idiv"},
    {InstructionType::DIVF, "fdiv"},
    {InstructionType::MODI, "modi"},
    {InstructionType::MODF, "modf"},
    {InstructionType::ABSI, "ABSI"},
    {InstructionType::ABSF, "ABSF"},
    {InstructionType::CMPI, "cmpi"},
    {InstructionType::CMPU, "cmpu"},
    {InstructionType::CMPF, "cmpf"},
    {InstructionType::JMP  ,"jmp"},
    {InstructionType::JE  ,"je"},
    {InstructionType::JNE  ,"jne"},
    {InstructionType::JG  ,"jg"},
    {InstructionType::JL  ,"jl"},
    {InstructionType::JGE  ,"jge"},
    {InstructionType::JLE  ,"jle"},
    { InstructionType::JMPA, "jmpa"},
    { InstructionType::JEA, "jea"},
    { InstructionType::JNEA, "jnea"},
    { InstructionType::JGA, "jga"},
    { InstructionType::JLA, "jla"},
    { InstructionType::JGEA, "jgea"},
    { InstructionType::JLEA, "jlea"},
    {InstructionType::LOAD, "load"},
    {InstructionType::STORE, "store"},
    {InstructionType::PRINTI, "printi"},
    {InstructionType::PRINTF, "printf"},
    {InstructionType::PRINTC, "printc"},
    {InstructionType::PRINTU, "printu"},
    {InstructionType::BREAKPOINT  ,"bp"},
    {InstructionType::SCREEN ,"screen"},
    {InstructionType::RENDER ,"render"},
    {InstructionType::CALL, "call"},
    {InstructionType::RET  ,"ret"},
    {InstructionType::PUSH, "push"},
    {InstructionType::POP, "pop"},
    {InstructionType::DECI, "deci"},
    {InstructionType::DECF, "decf"},
    {InstructionType::DECU, "decu"},
    {InstructionType::INCI, "inci"},
    {InstructionType::INCF, "incf"},
    {InstructionType::INCU, "incu"},
    {InstructionType::TIME, "time"},
    {InstructionType::HALT, "hlt"},
    {InstructionType::LABEL, "label"},
    {InstructionType::ITOF, "itof"},
    {InstructionType::ITOU, "itou"},
    {InstructionType::FTOI, "ftoi"},
    {InstructionType::FTOU, "ftou"},
    {InstructionType::UTOF, "utof"},
    {InstructionType::UTOI, "utoi"},
};
std::string instructionTypeRepr(InstructionType inst_type);

struct Token
{
    TokenType type;
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

struct RelocationEntry
{
    int index;
    std::string symbol;
};

enum class ArgType
{
    REGISTER,
    IMMEDIATE_I,
    IMMEDIATE_F,
    IMMEDIATE_C,
    POINTER_R,
    POINTER_I,
    LABEL_INDEX,
};

struct Instruction
{
    InstructionType type;
    uint64_t args[3];
    ArgType arg_types[3];
};
std::string instructionRepr(Instruction inst);

inline std::string ALLOWED_SYMBOL_CHARS = ".#abcdefghijklmnopqrstuvwxyz0123456789_";

bool isAbsoluteJmpInst(InstructionType jmp_inst_type);

#endif