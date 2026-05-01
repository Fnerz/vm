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

    std::vector<uint8_t> registers = {};
    std::map<int, uint8_t> memory = {};
    std::vector<uint8_t> stack = {};
    std::vector<int> call_stack = {};
    std::map<std::string, int> labels = {};
    
    bool lesser_flag = false;
    bool equal_flag = false;
    bool greater_flag = false;
    
    int ic = 0;
    std::vector<Instruction> instructions;
    
    void tokenize(std::string code);
    void parse();
    
    public:
    void reset();
    void loadCode(std::string code);

};

#endif