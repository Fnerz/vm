#ifndef VM_CPP
#define VM_CPP

#include "vm.hpp"


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
    this->reset();
    this->tokenize(code);
    // for (auto tok_line : this->token_lines) // debug
    // {
    //     for (auto tok : tok_line)
    //     {
    //         std::cout << tokenRepr(tok) << std::endl;
    //     }
    //     std::cout << "+++++" << std::endl;
    // }
    // std::cout << "===============" << std::endl;
    this->parse();
    for (auto inst : this->ir_instructions)
    {
        std::cout << instructionIrRepr(inst) << std::endl;
    }
    
    InstructionLowerer lowerer(this->ir_instructions);
    this->instructions = lowerer.lower();

    for (auto inst : this->instructions)
    {
        std::cout << instructionRepr(inst) << std::endl;
    }

    return;
}

void VirtualMachine::testFunc()
{
    InstructionIr inst_ir;
    inst_ir.type = InstructionType::MOV;
    inst_ir.arg1 = Token{TokenType::REGISTER, "10"};

    auto plus_expr = std::make_shared<BinOp>();
    plus_expr->left = Token{TokenType::REGISTER, "10"};
    plus_expr->op = Token{TokenType::PLUS, "+"};
    plus_expr->right = Token{TokenType::IMMEDIATE, "2"};

    auto full_expr = std::make_shared<BinOp>();
    full_expr->left = plus_expr;
    full_expr->op = Token{TokenType::MINUS, "-"};
    full_expr->right = Token{TokenType::REGISTER, "1"};

    inst_ir.arg2 = full_expr;
    inst_ir.arg3 = full_expr;
    // inst_ir.arg3 = Token{TokenType::REGISTER, "4"};

    std::cout << instructionIrRepr(inst_ir) << std::endl;

    InstructionLowerer lowerer(std::vector({inst_ir}));
    
    std::vector<Instruction> l_insts = lowerer.lower();
    for (auto li: l_insts)
    {
        std::cout << instructionRepr(li) << std::endl;
    }
};

#endif