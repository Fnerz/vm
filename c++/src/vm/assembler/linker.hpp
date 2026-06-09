#ifndef LINKER_HPP
#define LINKER_HPP

#include <string>
#include <vector>
#include <map>
#include "../types.hpp"

struct Symbol
{
    std::string name;     
    int address;   
    bool is_global;       
    std::string original_name;
};

struct LinkedFile
{
    std::vector<Instruction> instructions;
    std::map<std::string, int> symbols;  
    std::vector<RelocationEntry> relocations;
    int offset; // offset + jmp addr = final addr
};

class VmLinker
{
    private:
    bool debug = false;

    std::vector<LinkedFile> linked_files = {};
    std::vector<Instruction> final_instructions = {};
    std::map<std::string, int> global_symbols = {};
    int entry_point = -1;

    bool isGlobalSymbol(const std::string& name);
    bool isFileLocal(const std::string& name);

    public:
    VmLinker(bool debug = false);
    
    std::vector<Instruction> getInstructions() const;
    int getEntryPoint() const;
    void linkFiles(const std::vector<std::string>& asm_files, bool debug = false);
    
    void dumpToBinary(std::string name);
    
};

#endif
