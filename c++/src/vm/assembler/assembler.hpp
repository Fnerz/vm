#ifndef VM_ASSEMBLER_HPP
#define VM_ASSEMBLER_HPP

#include <fstream>
#include "../types.hpp"
#include "lowerer.hpp"
#include "parser.hpp"
#include "lexer.hpp"

class VmAssembler
{
    private:
    std::string code;
    std::vector<std::vector<Token>> token_lines = {};
    std::vector<InstructionIr> ir_instructions = {};
    std::vector<Instruction> instructions = {};
    bool debug;

    void tokenize();
    void parse();
    void lower();
    
    public:
    VmAssembler(std::string code, bool debug = false);
    void assamble();
    void dumpToBinary(std::string name);
    std::vector<uint64_t> loadFromBinary(std::string name);
};

#endif 