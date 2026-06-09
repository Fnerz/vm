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
    std::map<std::string, int> symbol_table = {};  // label_name -> instruction_index
    std::vector<RelocationEntry> relocation_table = {};
    bool debug;

    void tokenize();
    void parse();
    void lower();
    
    public:
    VmAssembler(std::string code, bool debug = false);
    void assamble();
    void dumpToBinary(std::string name);
    std::vector<uint64_t> loadFromBinary(std::string name);
    
    // Expose symbols and instructions for linker
    std::vector<Instruction> getInstructions() const;
    std::map<std::string, int> getSymbolTable() const;
    std::vector<RelocationEntry> getRelocationTable() const;
};

#endif 