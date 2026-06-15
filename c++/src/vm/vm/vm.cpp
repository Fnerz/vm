#ifndef VM_CPP
#define VM_CPP

#include "vm.hpp"

VirtualMachine::VirtualMachine() : regs(REGISTER_COUNT),
                                    memory(MEMORY_SIZE),
                                    vdisk(memory, DISK_ADDR) 
{};

void VirtualMachine::resetFlags()
{
    this->lesser_flag = false;
    this->equal_flag = false;
    this->greater_flag = false;
}


uint64_t VirtualMachine::resolveAddress(uint64_t arg, ArgType arg_type)
{
    if (arg_type == ArgType::REGISTER)
    {
        return arg;
    }
    else if (arg_type == ArgType::POINTER_R)
    {
        return this->regs[arg];
    }
    else if (arg_type == ArgType::IMMEDIATE_I)
    {
        return arg;
    }
    else if (arg_type == ArgType::IMMEDIATE_F)
    {
        return arg;
    }
    else if (arg_type == ArgType::IMMEDIATE_C)
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
        return this->regs[arg];
    }
    else if (arg_type == ArgType::POINTER_R)
    {
        return this->regs[arg];
    }
    else if (arg_type == ArgType::IMMEDIATE_I)
    {
        return arg;
    }
    else if (arg_type == ArgType::IMMEDIATE_F)
    {
        return arg;
    }
    else if (arg_type == ArgType::IMMEDIATE_C)
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

void VirtualMachine::interrupt()
{
    this->ic = this->INTERRUPT_JMP_ADDR;
}

bool VirtualMachine::isValidMemoryRange(uint64_t addr, uint64_t count)
{
    return (addr < this->memory.size()) && (count <= this->memory.size() - addr);
}

std::string VirtualMachine::readStringFromMemory(uint64_t addr)
{
    if (!this->isValidMemoryRange(addr, 1))
    {
        std::cerr << "String pointer out of memory range: " << addr << std::endl;
        exit(1);
    }

    std::string result;
    while (addr < this->memory.size())
    {
        char c = static_cast<char>(this->memory[static_cast<size_t>(addr)] & 0xFFull);
        if (c == '\0')
        {
            return result;
        }
        result.push_back(c);
        addr += 1;
    }

    std::cerr << "Unterminated string read from memory" << std::endl;
    exit(1);
}

void VirtualMachine::loadBytesFromMemory(uint64_t addr, char* buffer, uint64_t count)
{
    if (!this->isValidMemoryRange(addr, count))
    {
        std::cerr << "Read buffer out of memory range: " << addr << " + " << count << std::endl;
        exit(1);
    }

    for (uint64_t i = 0; i < count; ++i)
    {
        buffer[i] = static_cast<char>(this->memory[static_cast<size_t>(addr + i)] & 0xFFull);
    }
}

void VirtualMachine::storeBytesToMemory(uint64_t addr, const char* buffer, uint64_t count)
{
    if (!this->isValidMemoryRange(addr, count))
    {
        std::cerr << "Write buffer out of memory range: " << addr << " + " << count << std::endl;
        exit(1);
    }

    for (uint64_t i = 0; i < count; ++i)
    {
        this->memory[static_cast<size_t>(addr + i)] = static_cast<unsigned char>(buffer[i]);
    }
}

bool VirtualMachine::step()
{
    if (this->ic < 0)
    {
        return false;
    }
    this->run_time_counter++;

    this->pollKeyboardEvents();
    char input = this->getKeyboardInput();
    if (input != '\0')
    {
        this->memory[this->KEYBOARD_INPUT_ADDR] = static_cast<uint64_t>(static_cast<unsigned char>(input));
        this->memory[this->KEYDOWN_FLAG_ADDR] = 1;
    }
    else
    {
        this->memory[this->KEYDOWN_FLAG_ADDR] = 0;
    }

    if ((input != '\0') && (this->memory[this->INTERRUPT_FLAG_ADDR] == static_cast<uint8_t>(InterruptType::KEYBOARD)))
    {
        this->interrupt();
    }
    else if (this->memory[this->INTERRUPT_FLAG_ADDR] == static_cast<uint8_t>(InterruptType::TIMED))
    {
        if (this->interrupt_timer != 0 && this->interrupt_timer % this->INTERRUPT_FREQ == 0)
        {
            this->interrupt();
        }
    }

    if (this->run_time_counter % this->DISK_AUTO_WRITE_FREQ)
    {
        this->vdisk.safeImg();
    }
    if (this->run_time_counter % this->DISK_STEP_FREQ)
    {
        this->vdisk.step();
    }

    // std::cout << input << std::endl;

    const Instruction* inst_ptr = reinterpret_cast<const Instruction*>(&this->memory[this->ic * INSTRUCTION_BYTES]);
    Instruction inst = *inst_ptr;
    bool advance_ip = true;
    std::cout << this->ic << " " << instructionRepr(inst) << std::endl;

    switch (inst.type)
    {
        case InstructionType::MOV:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->regs[dest] = src;
            break;
        }
        case InstructionType::ADDI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = add<int64_t>(left, right);
            break;
        }
        case InstructionType::ADDF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = std::bit_cast<uint64_t>(add<double>(left, right));
            break;
        }
        case InstructionType::ADDU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = add<uint64_t>(left, right);
            break;
        }
        case InstructionType::SUBI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = sub<int64_t>(left, right);
            break;
        }
        case InstructionType::SUBF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = std::bit_cast<uint64_t>(sub<double>(left, right));
            break;
        }
        case InstructionType::SUBU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = sub<uint64_t>(left, right);
            break;
        }
        case InstructionType::MULI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = mul<int64_t>(left, right);
            break;
        }
        case InstructionType::MULF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = std::bit_cast<uint64_t>(mul<double>(left, right));
            break;
        }
        case InstructionType::MULU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t left = this->resolveValue(inst.args[1], inst.arg_types[1]);
            uint64_t right = this->resolveValue(inst.args[2], inst.arg_types[2]);
            this->regs[dest] = mul<uint64_t>(left, right);
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
            this->regs[dest] = div<int64_t>(left, right);
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
            this->regs[dest] = std::bit_cast<uint64_t>(div<double>(left, right));
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
            this->regs[dest] = div<uint64_t>(left, right);
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
            this->regs[dest] = mod<int64_t>(left, right);
            break;
        }
        case InstructionType::ABSI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = abs<int64_t>(this->regs[dest]);
            break;
        }
            case InstructionType::ABSF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = std::bit_cast<uint64_t>(abs<double>(this->regs[dest]));
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
            this->ic += inst.args[0];
            return true;
        }
        case InstructionType::JE:
        {
            if (this->equal_flag)
            {
                this->ic += inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JNE:
        {
            if (!this->equal_flag)
            {
                this->ic += inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JG:
        {
            if (this->greater_flag)
            {
                this->ic += inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JL:
        {
            if (this->lesser_flag)
            {
                this->ic += inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JGE:
        {
            if (this->greater_flag || this->equal_flag)
            {
                this->ic += inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JLE:
        {
            if (this->lesser_flag || this->equal_flag)
            {
                this->ic += inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JMPA:
        {
            this->ic = inst.args[0];
            return true;
        }
        case InstructionType::JEA:
        {
            if (this->equal_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JNEA:
        {
            if (!this->equal_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JGA:
        {
            if (this->greater_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JLA:
        {
            if (this->lesser_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JGEA:
        {
            if (this->greater_flag || this->equal_flag)
            {
                this->ic = inst.args[0];
                return true;
            }
            break;
        }
        case InstructionType::JLEA:
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
            uint64_t dst_addr = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src_addr = this->resolveValue(inst.args[1], inst.arg_types[1]);
            load<uint64_t>(dst_addr, src_addr);
            break;
        }
        case InstructionType::STORE:
        {
            uint64_t src_val = this->resolveValue(inst.args[0], inst.arg_types[0]);
            uint64_t dst_addr = this->resolveAddress(inst.args[1], inst.arg_types[1]); 
            store<uint64_t>(src_val, dst_addr);
            break;
        }
        case InstructionType::PRINTI:
        {
            uint64_t start = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t end = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            print<uint64_t>(start, end);
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
        case InstructionType::PRINTC:
        {
            uint64_t start = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t end = this->resolveAddress(inst.args[1], inst.arg_types[1]);
            if (start == end)
            {
                std::cout << static_cast<char>(this->regs[start]);
            }
            else
            {
                for (uint64_t idx = start; idx < end; ++idx)
                {
                    std::cout << static_cast<char>(this->regs[idx]);
                }
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
            this->ic += inst.args[0];
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
            this->regs[dest] = this->stack.back();
            this->stack.pop_back();
            break;
        }
        case InstructionType::DECI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = dec<int64_t>(this->regs[dest]);
            break;
        }
        case InstructionType::DECF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = std::bit_cast<uint64_t>(dec<double>(this->regs[dest]));
            break;
        }
        case InstructionType::DECU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = dec<uint64_t>(this->regs[dest]);
            break;
        }
        case InstructionType::INCI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = inc<int64_t>(this->regs[dest]);
            break;
        }
        case InstructionType::INCF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = std::bit_cast<uint64_t>(inc<double>(this->regs[dest]));
            break;
        }
        case InstructionType::INCU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            this->regs[dest] = inc<uint64_t>(this->regs[dest]);
            break;
        }
        case InstructionType::TIME:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            std::time_t now = std::time(nullptr);
            this->regs[dest] = std::bit_cast<uint64_t>(static_cast<double>(now));
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
            this->regs[dest] = std::bit_cast<uint64_t>(dv);
            break;
        }
        case InstructionType::ITOU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->regs[dest] = static_cast<uint64_t>(static_cast<int64_t>(std::bit_cast<int64_t>(src)));
            break;
        }
        case InstructionType::FTOI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->regs[dest] = static_cast<uint64_t>(static_cast<int64_t>(std::bit_cast<double>(src)));
            break;
        }
        case InstructionType::FTOU:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->regs[dest] = static_cast<uint64_t>(static_cast<uint64_t>(std::bit_cast<double>(src)));
            break;
        }
        case InstructionType::UTOF:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            double dv = static_cast<double>(src);
            this->regs[dest] = std::bit_cast<uint64_t>(dv);
            break;
        }
        case InstructionType::UTOI:
        {
            uint64_t dest = this->resolveAddress(inst.args[0], inst.arg_types[0]);
            uint64_t src = this->resolveValue(inst.args[1], inst.arg_types[1]);
            this->regs[dest] = static_cast<uint64_t>(static_cast<int64_t>(src));
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

void VirtualMachine::pollKeyboardEvents()
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
                this->keyboard_input_queue.push_back(static_cast<char>(key));
                continue;
                break;
            }
            case SDL_EVENT_TEXT_INPUT:
                if (e.text.text[0] != '\0')
                {
                    this->keyboard_input_queue.push_back(e.text.text[0]);
                }
                break;
            default:
                break;
        }
    }
}

char VirtualMachine::getKeyboardInput()
{
    if (this->keyboard_input_queue.empty())
    {
        return '\0';
    }

    char input = this->keyboard_input_queue.front();
    this->keyboard_input_queue.pop_front();
    return input;
}

void VirtualMachine::loadInstBinary(std::vector<uint8_t> bytes)
{
    if (bytes.size() % INSTRUCTION_BYTES != 0)
    {
        std::cerr << "Binary instruction stream size is not aligned to Instruction size" << std::endl;
        exit(1);
    }

    for (int i = 0; i < static_cast<int>(bytes.size()); i++)
    {
        this->memory[i] = bytes[i];
    }
    return;
}

#endif