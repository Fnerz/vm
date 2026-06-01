#ifndef VM_HPP
#define VM_HPP


#include <vector>
#include <map>
#include <string>
#include "types.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "lowerer.hpp"

class VirtualMachine
{
    private:
    std::vector<std::vector<Token>> token_lines = {};
    std::vector<InstructionIr> ir_instructions = {};

    const int REGISTER_COUNT = 34; // r30 - r33 reserved for temp storage. r30 is used for the pointer arithmetic accumulator. r31-r33 are just chilling for now.
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

    int run_time_counter = 0;
    
    void tokenize(std::string code);
    void parse();
    void resetFlags();

    uint32_t resolveAddress(uint32_t arg, ArgType arg_type);
    uint32_t resolveValue(uint32_t arg, ArgType arg_type);

    
    public:
    void reset();
    void loadCode(std::string code);
    void testFunc();
    bool step();
    void run();
    std::vector<uint32_t> getRegisters();
    std::map<int, uint32_t> getMemory();


};

#endif