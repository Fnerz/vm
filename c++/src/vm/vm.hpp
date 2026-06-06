#ifndef VM_HPP
#define VM_HPP


#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <bit>
#include <cmath>
#include <iostream>
#include <SDL3/SDL.h>
#include "types.hpp"

class VirtualMachine
{
    private:


    // r30 - r33 reserved for temp storage. r30 is used for the pointer arithmetic accumulator. r31-r33 are just chilling for now.
    // 29 reserved for keyboard input
    static constexpr int REGISTER_COUNT = 34; 
    std::vector<uint64_t> registers = std::vector<uint64_t>(REGISTER_COUNT);
    std::vector<uint64_t> memory = std::vector<uint64_t>(1000000);
    std::vector<uint64_t> stack = {};
    std::vector<int> call_stack = {};
    
    bool lesser_flag = false;
    bool equal_flag = false;
    bool greater_flag = false;
    
    int ic = 0;
    int instruction_count = 0;
    static constexpr int INSTRUCTION_WORDS = sizeof(Instruction) / sizeof(uint64_t);

    int run_time_counter = 0;
    
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
        return std::bit_cast<T>(a)+static_cast<T>(1);
    }
    template<typename T>
    T dec(uint64_t a)
    {
        return std::bit_cast<T>(a)-static_cast<T>(1);
    }


    public:
    void testFunc();
    bool step();
    void run();
    void loadInstBinary(std::vector<uint64_t> words);
};

#endif