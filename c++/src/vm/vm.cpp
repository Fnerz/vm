#ifndef VM_CPP
#define VM_CPP

#include "vm.hpp"
#include <ctime>
#include <bit>


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

void VirtualMachine::loadCode(std::string code, bool debug)
{
    std::transform(code.begin(), code.end(), code.begin(),[](unsigned char c){ return std::tolower(c); });
    this->reset();
    this->tokenize(code);
    this->parse();
    if  (debug)
    {
        for (auto inst : this->ir_instructions)
        {
            std::cout << instructionIrRepr(inst) << std::endl;
        }
        std::cout << "==========================================" << std::endl;
    }
    InstructionLowerer lowerer(this->ir_instructions);
    this->instructions = lowerer.lower();
    if (debug)
    {
        for (int i = 0; i < this->instructions.size(); i++)
        {
            auto inst = this->instructions[i];
            std::cout << i << "\t" << instructionRepr(inst) << std::endl;
        }
        std::cout << "==========================================" << std::endl;
    }


    return;
}

uint64_t VirtualMachine::resolveAddress(uint64_t arg, ArgType arg_type)
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

uint64_t VirtualMachine::resolveValue(uint64_t arg, ArgType arg_type)
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

    char input = this->getKeyboardInput();
    // std::cout << input << std::endl;

    Instruction inst = this->instructions[this->ic];
    bool advance_ip = true;

    switch (inst.type)
    {
        case InstructionType::MOV:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->registers[dest] = src;
            break;
        }
        case InstructionType::ADDI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = add<int64_t>(left, right);
            break;
        }
        case InstructionType::ADDF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = std::bit_cast<uint64_t>(add<double>(left, right));
            break;
        }
        case InstructionType::ADDU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = add<uint64_t>(left, right);
            break;
        }
        case InstructionType::SUBI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = sub<int64_t>(left, right);
            break;
        }
        case InstructionType::SUBF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = std::bit_cast<uint64_t>(sub<double>(left, right));
            break;
        }
        case InstructionType::SUBU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = sub<uint64_t>(left, right);
            break;
        }
        case InstructionType::MULI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = mul<int64_t>(left, right);
            break;
        }
        case InstructionType::MULF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = std::bit_cast<uint64_t>(mul<double>(left, right));
            break;
        }
        case InstructionType::MULU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->registers[dest] = mul<uint64_t>(left, right);
            break;
        }
        case InstructionType::DIVI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            if (right == 0)
            {
                std::cerr << "Integer division by zero" << std::endl;
                exit(1);
            }
            this->registers[dest] = div<int64_t>(left, right);
            break;
        }
        case InstructionType::DIVF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            if (right == 0)
            {
                std::cerr << "Floating division by zero" << std::endl;
                exit(1);
            }
            this->registers[dest] = std::bit_cast<uint64_t>(div<double>(left, right));
            break;
        }
        case InstructionType::DIVU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            if (right == 0)
            {
                std::cerr << "Floating division by zero" << std::endl;
                exit(1);
            }
            this->registers[dest] = div<uint64_t>(left, right);
            break;
        }
        case InstructionType::MODI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            if (right == 0)
            {
                std::cerr << "Modulo by zero" << std::endl;
                exit(1);
            }
            this->registers[dest] = mod<int64_t>(left, right);
            break;
        }
        case InstructionType::ABSI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] = abs<int64_t>(this->registers[dest]);
            break;
        }
            case InstructionType::ABSF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] = std::bit_cast<uint64_t>(abs<double>(this->registers[dest]));
            break;
        }
        case InstructionType::CMP:
        {
            uint64_t left = this->resolveValue(inst.args[0], inst.arg_types[0]);
            uint64_t right = this->resolveValue(inst.args[1], inst.arg_types[1]);
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
        case InstructionType::CMPI:
        {
            int64_t left = static_cast<int64_t>(this->resolveValue(inst.args[0], inst.arg_types[0]));
            int64_t right = static_cast<int64_t>(this->resolveValue(inst.args[1], inst.arg_types[1]));
            this->resetFlags();
            if (left < right) this->lesser_flag = true;
            else if (left > right) this->greater_flag = true;
            else this->equal_flag = true;
            break;
        }
        case InstructionType::CMPU:
        {
            uint64_t left = this->resolveValue(inst.args[0], inst.arg_types[0]);
            uint64_t right = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->resetFlags();
            if (left < right) this->lesser_flag = true;
            else if (left > right) this->greater_flag = true;
            else this->equal_flag = true;
            break;
        }
        case InstructionType::CMPF:
        {
            double left = std::bit_cast<double>(this->resolveValue(inst.args[0], inst.arg_types[0]));
            double right = std::bit_cast<double>(this->resolveValue(inst.args[1], inst.arg_types[1]));
            this->resetFlags();
            if (left < right) this->lesser_flag = true;
            else if (left > right) this->greater_flag = true;
            else this->equal_flag = true;
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
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src_addr = this->resolveValue(inst.args[1], inst.arg_types[1]);
            auto it = this->memory.find(static_cast<int>(src_addr));
            this->registers[dest] = (it != this->memory.end()) ? it->second : 0;
            break;
        }
        case InstructionType::STORE:
        {
            uint64_t src = this->resolveValue(inst.args[0], inst.arg_types[0]);
            uint64_t dest = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            this->memory[static_cast<int>(dest)] = src;
            break;
        }
        case InstructionType::PRINTI:
        {
            uint64_t start = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t end = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            print<int64_t>(start, end);
            break;
        }
        case InstructionType::PRINTF:
        {
            uint64_t start = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t end = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            print<double>(start, end);
            break;
        }

        case InstructionType::PRINTU:
        {
            uint64_t start = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t end = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            print<uint64_t>(start, end);
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
            uint64_t src = this->resolveValue(inst.args[0], inst.arg_types[0]);
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
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] = this->stack.back();
            this->stack.pop_back();
            break;
        }
        case InstructionType::DECI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] -= dec<int64_t>(this->registers[dest]);
            break;
        }
        case InstructionType::DECF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] = std::bit_cast<uint64_t>(dec<double>(this->registers[dest]));
            break;
        }
        case InstructionType::DECU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] -= dec<uint64_t>(this->registers[dest]);
            break;
        }
        case InstructionType::INCI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] += inc<int64_t>(this->registers[dest]);
            break;
        }
        case InstructionType::INCF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] = std::bit_cast<uint64_t>(inc<double>(this->registers[dest]));
            break;
        }
        case InstructionType::INCU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->registers[dest] += inc<uint64_t>(this->registers[dest]);
            break;
        }
        case InstructionType::TIME:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            std::time_t now = std::time(nullptr);
            this->registers[dest] = std::bit_cast<uint64_t>(static_cast<double>(now));
            break;
        }
        case InstructionType::HALT:
        {
            return false;
        }
        case InstructionType::ITOF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            double dv = static_cast<double>(static_cast<int64_t>(src));
            this->registers[dest] = std::bit_cast<uint64_t>(dv);
            break;
        }
        case InstructionType::ITOU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->registers[dest] = static_cast<uint64_t>(static_cast<int64_t>(std::bit_cast<int64_t>(src)));
            break;
        }
        case InstructionType::FTOI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->registers[dest] = static_cast<uint64_t>(static_cast<int64_t>(std::bit_cast<double>(src)));
            break;
        }
        case InstructionType::FTOU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->registers[dest] = static_cast<uint64_t>(static_cast<uint64_t>(std::bit_cast<double>(src)));
            break;
        }
        case InstructionType::UTOF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            double dv = static_cast<double>(src);
            this->registers[dest] = std::bit_cast<uint64_t>(dv);
            break;
        }
        case InstructionType::UTOI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->registers[dest] = static_cast<uint64_t>(static_cast<int64_t>(src));
            break;
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

std::vector<uint64_t> VirtualMachine::getRegisters()
{
    return this->registers;
}

std::map<int, uint64_t> VirtualMachine::getMemory()
{
    return this->memory;
}

char VirtualMachine::getKeyboardInput()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_EVENT_QUIT:
                exit(1);
            case SDL_EVENT_KEY_DOWN:
            {
                SDL_Keycode key = e.key.key;
                if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
                {
                    return '\n';
                }
                if (key == SDLK_BACKSPACE)
                {
                    return '\b';
                }
                if (key == SDLK_TAB)
                {
                    return '\t';
                }
                if (key >= SDLK_SPACE && key <= SDLK_Z)
                {
                    return static_cast<char>(key);
                }
                break;
            }
            case SDL_EVENT_TEXT_INPUT:
                if (e.text.text[0] != '\0')
                {
                    return e.text.text[0];
                }
                break;
            default:
                break;
        }
    }
    return '\0';
}


#endif