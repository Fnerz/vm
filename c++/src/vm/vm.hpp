#ifndef VM_HPP
#define VM_HPP


#include <vector>
#include <map>
#include <string>
#include <bit>
#include <cmath>
#include <iostream>
#include <SDL3/SDL.h>
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
    std::vector<uint64_t> registers = {};
    std::map<int, uint64_t> memory = {};
    std::vector<uint64_t> stack = {};
    std::vector<int> call_stack = {};
    
    bool lesser_flag = false;
    bool equal_flag = false;
    bool greater_flag = false;
    
    int ic = 0;
    std::vector<Instruction> instructions;

    int run_time_counter = 0;
    
    void tokenize(std::string code);
    void parse();
    void resetFlags();

    uint64_t resolveAddress(uint64_t arg, ArgType arg_type);
    uint64_t resolveValue(uint64_t arg, ArgType arg_type);

    char getKeyboardInput();


    template<typename T>
    T add(uint64_t a, uint64_t b)
    {
        return std::bit_cast<T>(a) + std::bit_cast<T>(b);
    }
    template<typename T>
    T sub(uint64_t a, uint64_t b)
    {
        return std::bit_cast<T>(a) - std::bit_cast<T>(b);
    }
    template<typename T>
    T mul(uint64_t a, uint64_t b)
    {
        return std::bit_cast<T>(a) * std::bit_cast<T>(b);
    }
    template<typename T>
    T div(uint64_t a, uint64_t b)
    {
        return std::bit_cast<T>(a) / std::bit_cast<T>(b);
    }
    template<typename T>
    T mod(uint64_t a, uint64_t b)
    {
        return std::bit_cast<T>(a) % std::bit_cast<T>(b);
    }
    template<typename T>
    T abs(uint64_t a)
    {
        return std::abs(std::bit_cast<T>(a));

    }
    template<typename T>
    void print(uint64_t start, uint64_t end)
    {
        if (start == end)
        {
            std::cout << std::bit_cast<T>(this->registers[start]) << std::endl;
        }
        else
        {
            std::cout << "[";
            for (uint64_t idx = start; idx < end; ++idx)
            {
                std::cout << this->registers[idx];
                if (idx + 1 < end)
                {
                    std::cout << ", ";
                }
            }
            std::cout << "]" << std::endl;
        }
    }
    template<typename T>
    T inc(uint64_t a)
    {
        return std::bit_cast<T>(a)+1;
    }
    template<typename T>
    T dec(uint64_t a)
    {
        return std::bit_cast<T>(a)-1;
    }


    public:
    void reset();
    void loadCode(std::string code, bool debug);
    void testFunc();
    bool step();
    void run();
    std::vector<uint64_t> getRegisters();
    std::map<int, uint64_t> getMemory();


};

#endif