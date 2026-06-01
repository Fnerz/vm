#ifndef VM_CPP
#define VM_CPP

#include "vm.hpp"
#include <ctime>


void VirtualMachine::resetFlags()
{
    this->lesser_flag = false;
    this->equal_flag = false;
    this->greater_flag = false;
}

void VirtualMachine::reset()
{
    this->registers = {};
    this->memory = {};
    this->stack = {};
    this->call_stack = {};
    this->labels = {};
    this->ic = 0;
    this->run_time_counter = 0;

    this->resetFlags();

    for (int i = 0; i < this->REGISTER_COUNT; i++)
    {
        this->registers.push_back(0);
    }
    return;
}

std::vector<std::string> splitCodeToLines(std::string code)
{
    std::vector<std::string> lines;
    std::string buffer = "";
    for (auto c : code) 
    {
        if (c == '\n')
        {
            lines.push_back(buffer);
            buffer = "";
            continue;
        }
        buffer += c;
    }
    if (buffer.size() > 0)
    {
        lines.push_back(buffer);
    }
    return lines;
}

void VirtualMachine::tokenize(std::string code)
{
    std::vector<std::string> lines = splitCodeToLines(code);
    for (auto line : lines)
    {
        LineLexer lexer(line);
        this->token_lines.push_back(lexer.tokenize());
    }
    return;
}

void VirtualMachine::parse()
{
    for (auto tok_line : this->token_lines)
    {
        LineParser parser(tok_line);
        auto insts = parser.parse(); // parser is expected to return only one instructino atm, but i chose to go the safe rout.
        for (auto inst : insts)
            this->ir_instructions.push_back(inst);
    }
    return;
}

void VirtualMachine::loadCode(std::string code)
{
    std::transform(code.begin(), code.end(), code.begin(),[](unsigned char c){ return std::tolower(c); });
    this->reset();
    this->tokenize(code);
    this->parse();
    for (auto inst : this->ir_instructions)
    {
        std::cout << instructionIrRepr(inst) << std::endl;
    }
    std::cout << "==========================================" << std::endl;
    InstructionLowerer lowerer(this->ir_instructions);
    this->instructions = lowerer.lower();
    std::cout << "==========================================" << std::endl;

    for (auto inst : this->instructions)
    {
        std::cout << instructionRepr(inst) << std::endl;
    }

    return;
}

uint32_t VirtualMachine::resolveAddress(uint32_t arg, ArgType arg_type)
{
    if (arg_type == ArgType::REGISTER)
    {
        return arg;
    }
    else if (arg_type == ArgType::POINTER)
    {
        return this->registers[arg];
    }
    else if (arg_type == ArgType::IMMEDIATE)
    {
        return arg;
    }
    else if (arg_type == ArgType::LABEL_INDEX)
    {
        std::cout << "Cannot resolve address of label index" << std::endl;
        exit(1);
    }
    exit(1); // useless as its never gonna reach this, but g++ is complaining.
}

uint32_t VirtualMachine::resolveValue(uint32_t arg, ArgType arg_type)
{
    if (arg_type == ArgType::REGISTER)
    {
        return this->registers[arg];
    }
    else if (arg_type == ArgType::POINTER)
    {
        return this->registers[arg];
    }
    else if (arg_type == ArgType::IMMEDIATE)
    {
        return arg;
    }
    else if (arg_type == ArgType::LABEL_INDEX)
    {
        std::cout << "Cannot resolve value of label index" << std::endl;
        exit(1);
    }
    exit(1); // useless as its never gonna reach this, but g++ is complaining.
}

bool VirtualMachine::step()
{
    if (this->ic < 0 || this->ic >= this->instructions.size())
    {
        return false;
    }
    this->run_time_counter++;

    Instruction inst = this->instructions[this->ic];
    bool advance_ip = true;

    switch (inst.type)
    {
        case InstructionType::MOV:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->registers[dest] = src;
            break;
        }
        case InstructionType::ADD:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint32_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = left + right;
            break;
        }
        case InstructionType::SUB:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint32_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = left - right;
            break;
        }
        case InstructionType::MUL:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint32_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = left * right;
            break;
        }
        case InstructionType::IDIV:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint32_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            if (right == 0)
            {
                std::cerr << "Integer division by zero" << std::endl;
                exit(1);
            }
            this->registers[dest] = left / right;
            break;
        }
        case InstructionType::FDIV:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint32_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            if (right == 0)
            {
                std::cerr << "Floating division by zero" << std::endl;
                exit(1);
            }
            double result = static_cast<double>(left) / static_cast<double>(right);
            this->registers[dest] = static_cast<uint32_t>(result);
            break;
        }
        case InstructionType::MOD:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint32_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            if (right == 0)
            {
                std::cerr << "Modulo by zero" << std::endl;
                exit(1);
            }
            this->registers[dest] = left % right;
            break;
        }
        case InstructionType::ABS:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            int32_t src = static_cast<int32_t>(this->resolveValue(inst.args[1], inst.arg_types[1]));
            this->registers[dest] = static_cast<uint32_t>(src < 0 ? -src : src);
            break;
        }
        case InstructionType::CMP:
        {
            uint32_t left = this->resolveValue(inst.args[0], inst.arg_types[0]);
            uint32_t right = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->resetFlags();
            if (left < right)
            {
                this->lesser_flag = true;
            }
            else if (left > right)
            {
                this->greater_flag = true;
            }
            else
            {
                this->equal_flag = true;
            }
            break;
        }
        case InstructionType::JMP:
        {
            this->ic = inst.args[0];
            return true;
        }
        case InstructionType::JE:
        {
            if (this->equal_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JNE:
        {
            if (!this->equal_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JG:
        {
            if (this->greater_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JL:
        {
            if (this->lesser_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JGE:
        {
            if (this->greater_flag || this->equal_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JLE:
        {
            if (this->lesser_flag || this->equal_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::LOAD:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t src_addr = this->resolveValue(inst.args[1], inst.arg_types[1]);
            auto it = this->memory.find(static_cast<int>(src_addr));
            this->registers[dest] = (it != this->memory.end()) ? it->second : 0;
            break;
        }
        case InstructionType::STORE:
        {
            uint32_t src = this->resolveValue(inst.args[0], inst.arg_types[0]);
            uint32_t dest = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            this->memory[static_cast<int>(dest)] = src;
            break;
        }
        case InstructionType::PRINT:
        {
            uint32_t start = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint32_t end = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            if (start == end)
            {
                std::cout << this->registers[start] << std::endl;
            }
            else
            {
                std::cout << "[";
                for (uint32_t idx = start; idx < end; ++idx)
                {
                    std::cout << this->registers[idx];
                    if (idx + 1 < end)
                    {
                        std::cout << ", ";
                    }
                }
                std::cout << "]" << std::endl;
            }
            break;
        }
        case InstructionType::BREAKPOINT:
        {
            break;
        }
        case InstructionType::CALL:
        {
            this->call_stack.push_back(this->ic + 1);
            this->ic = inst.args[0];
            return true;
        }
        case InstructionType::RET:
        {
            if (this->call_stack.empty())
            {
                std::cerr << "Call stack underflow on RET" << std::endl;
                exit(1);
            }
            this->ic = this->call_stack.back();
            this->call_stack.pop_back();
            return true;
        }
        case InstructionType::PUSH:
        {
            uint32_t src = this->resolveValue(inst.args[0], inst.arg_types[0]);
            this->stack.push_back(src);
            break;
        }
        case InstructionType::POP:
        {
            if (this->stack.empty())
            {
                std::cerr << "Stack underflow on POP" << std::endl;
                exit(1);
            }
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] = this->stack.back();
            this->stack.pop_back();
            break;
        }
        case InstructionType::DEC:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] -= 1;
            break;
        }
        case InstructionType::INC:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] += 1;
            break;
        }
        case InstructionType::TIME:
        {
            uint32_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            std::time_t now = std::time(nullptr);
            this->registers[dest] = static_cast<uint32_t>(now);
            break;
        }
        case InstructionType::HALT:
        {
            return false;
        }
        default:
        {
            std::cerr << "Unsupported instruction at step " << this->ic << std::endl;
            exit(1);
        }
    }

    if (advance_ip)
    {
        this->ic += 1;
    }
    return true;
}

void VirtualMachine::run()
{
    bool res = true;

    while (res)
    {
        res = this->step();
    }
    std::cout << "Execution has finished after " << this->run_time_counter << " steps\n";
    return;
}

std::vector<uint32_t> VirtualMachine::getRegisters()
{
    return this->registers;
}

std::map<int, uint32_t> VirtualMachine::getMemory()
{
    return this->memory;
}

#endif