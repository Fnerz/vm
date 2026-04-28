#ifndef VM_HPP
#define VM_HPP


#include <vector>
#include <map>
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "types.hpp"

namespace py = pybind11;

class VirtualMachine
{
    private:
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
    
    public:
    void reset();


    public:
    
};

#endif