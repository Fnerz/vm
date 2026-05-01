#ifndef VM_CPP
#define VM_CPP

#include "vm.hpp"


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
        auto insts = parser.parse();
        for (auto inst : insts)
            this->ir_instructions.push_back(inst);
    }
    return;
}

void VirtualMachine::loadCode(std::string code)
{
    this->tokenize(code);
    for (auto tok_line : this->token_lines) // debug
    {
        for (auto tok : tok_line)
        {
            std::cout << tokenRepr(tok) << std::endl;
        }
        std::cout << "+++++" << std::endl;
    }
    std::cout << "===============" << std::endl;
    this->parse();
    for (auto inst : this->ir_instructions)
    {
        std::cout << instructionIrRepr(inst) << std::endl;
    }
    return;
}



#endif