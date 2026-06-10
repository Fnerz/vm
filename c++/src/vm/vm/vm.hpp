#ifndef VM_HPP
#define VM_HPP


#include <vector>
#include <deque>
#include <map>
#include <stdio.h>
#include <string>
#include <ctime>
#include <bit>
#include <cmath>
#include <fcntl.h>
#include <iostream>
#include <SDL3/SDL.h>
#include "../types.hpp"

enum class InterruptType
{
    OFF = 0,
    TIMED = 1,
    KEYBOARD = 2,
};

class VirtualMachine
{
    private:


    // r30 - r33 reserved for temp storage. r30 is used for the pointer arithmetic accumulator. r31-r33 are just chilling for now.
    static constexpr int REGISTER_COUNT = 34; 
    const int MEMORY_SIZE = 8000000; // 8 MB 
    std::vector<uint64_t> registers = std::vector<uint64_t>(REGISTER_COUNT);
    std::vector<uint8_t> memory = std::vector<uint8_t>(MEMORY_SIZE);
    std::vector<uint64_t> stack = {};
    std::vector<int> call_stack = {};
    
    bool lesser_flag = false;
    bool equal_flag = false;
    bool greater_flag = false;
    
    int ic = 0;
    int instruction_count = 0;
    static constexpr int INSTRUCTION_BYTES = sizeof(Instruction) / sizeof(uint8_t);

    int run_time_counter = 0;
    
    void resetFlags();

    uint64_t resolveAddress(uint64_t arg, ArgType arg_type);
    uint64_t resolveValue(uint64_t arg, ArgType arg_type);

    bool isValidMemoryRange(uint64_t addr, uint64_t count);
    std::string readStringFromMemory(uint64_t addr);
    void loadBytesFromMemory(uint64_t addr, char* buffer, uint64_t count);
    void storeBytesToMemory(uint64_t addr, const char* buffer, uint64_t count);

    const int KEYBOARD_INPUT_ADDR = 4990;
    const int KEYDOWN_FLAG_ADDR = 4991;
    std::deque<char> keyboard_input_queue = {};
    void pollKeyboardEvents();
    char getKeyboardInput();

    const int INTERRUPT_FLAG_ADDR = 5000;
    const int INTERRUPT_JMP_ADDR = 6000;
    const int INTERRUPT_FREQUENCY = 1000;
    int interrupt_timer = 0;
    void interrupt();

    const int DISK_AUTO_WRITE_FREQUENCY = 5000000;


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
    template <typename T>
    void load(uint64_t dst_addr, uint64_t src_addr)
    {
        T tmp = 0;
        memcpy(&tmp,
            &this->memory[static_cast<int>(src_addr)],
            sizeof(T));
        this->registers[dst_addr] = tmp;
        return;
    }
    template <typename T>
    void store(T src_val, uint64_t dst_addr)
    {
        for (size_t i = 0; i < sizeof(T); i++)
        {
            uint8_t byte = static_cast<uint8_t>(src_val >> (i * 8));
            this->memory[dst_addr + i] = byte;
        }
        return;
    }



    public:
    void testFunc();
    bool step();
    void run();
    void loadInstBinary(std::vector<uint8_t> bytes);
};

#endif