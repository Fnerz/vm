#ifndef VM_HPP
#define VM_HPP


#include <vector>
#include <map>
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "types.hpp"
#include "lexer.hpp"
#include "parser.hpp"

namespace py = pybind11;

class VirtualMachine
{
    private:
    std::vector<std::vector<Token>> token_lines = {};
    std::vector<InstructionIr> ir_instructions = {};

    const int REGISTER_COUNT = 33; 
    std::vector<uint32_t> registers = {};
    std::map<int, uint32_t> memory = {};
    std::vector<uint32_t> stack = {};
    std::vector<int> call_stack = {};
    std::map<std::string, int> labels = {};
    
    bool lesser_flag = false;
    bool equal_flag = false;
    bool greater_flag = false;
    
    int ic = 0;
    std::vector<Instruction> instructions;
    
    void tokenize(std::string code);
    void parse();
    void resetFlags();
    
    public:
    void reset();
    void loadCode(std::string code);

};

#endif