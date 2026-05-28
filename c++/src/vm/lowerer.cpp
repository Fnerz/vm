#ifndef LOWERER_CPP
#define LOWERER_CPP

#include "lowerer.hpp"

InstructionLowerer::InstructionLowerer(std::vector<InstructionIr> ir_insts)
{
    this->ir_insts = ir_insts;
    return;
}

/*
Returns free temp register, and also reserves it.
*/
int InstructionLowerer::getFreeTempReg()
{
    for (auto reg_info : this->temp_regs)
    {
        if (reg_info.second == RegisterState::FREE)
        {
            this->temp_regs[reg_info.first] = RegisterState::RESERVED;
            return reg_info.first;
        }
    }
    std::cout << "No free temp register" << std::endl;
    exit(1);
}

void InstructionLowerer::freeTempReg(int reg)
{
    try
    {
        this->temp_regs[reg] = RegisterState::FREE;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout << reg << " is no temp register\n";
        exit(1);
    }
    return;
}

void InstructionLowerer::freeAllTempRegs()
{
    for (auto reg : this->temp_regs)
    {
        this->temp_regs[reg.first] = RegisterState::FREE;
    }
    return;
}

InstructionType opTokenToInstType(Token tok)
{
    if (tok.type == TokenType::PLUS)
    {
        return InstructionType::ADD;
    }
    else if (tok.type == TokenType::MINUS)
    {
        return InstructionType::SUB;
    }
    std::cout << "Cannot convert " << tokenRepr(tok) << " to InstructionType" << std::endl;
    exit(1);    
}

std::tuple<int, ArgType> tokenToArgBundle(Token tok)
{
    if (tok.type == TokenType::IMMEDIATE)
    {
        return std::make_tuple(std::stoi(tok.value), ArgType::IMMEDIATE);
    }
    else if (tok.type == TokenType::REGISTER)
    {
        return std::make_tuple(std::stoi(tok.value), ArgType::REGISTER);
    }
    else
    {
        std::cout << "Cannot convert \"" << tokenRepr(tok) << "\"" << "(" << tokenTypeRepr(tok.type) << ")" << " to inst arg";
        exit(1);
    }
}

int InstructionLowerer::binOpToInst(std::shared_ptr<BinOp> bin_op)
{
    Instruction lowerd_inst;
    lowerd_inst.type = opTokenToInstType(bin_op->op);

    bool left_is_BinOp = false;
    std::tuple<int, ArgType> left_arg_bundle;
    if (std::holds_alternative<std::shared_ptr<BinOp>>(bin_op->left))
    {
        left_is_BinOp = true;
        int left_reg = this->binOpToInst(std::get<std::shared_ptr<BinOp>>(bin_op->left));
        left_arg_bundle = {left_reg, ArgType::REGISTER};
    }
    else if (std::holds_alternative<Token>(bin_op->left))
    {
        left_arg_bundle = tokenToArgBundle(std::get<Token>(bin_op->left));
    }
    else
    {
        std::cout << "Cannot convert std::string to inst arg";
        exit(1);
    }

    bool right_is_BinOp = false;
    std::tuple<int, ArgType> right_arg_bundle;
    if (std::holds_alternative<std::shared_ptr<BinOp>>(bin_op->right))
    {
        right_is_BinOp = true;
        int right_reg = this->binOpToInst(std::get<std::shared_ptr<BinOp>>(bin_op->right));
        right_arg_bundle = {right_reg, ArgType::POINTER};
    }
    else if (std::holds_alternative<Token>(bin_op->right))
    {
        right_arg_bundle = tokenToArgBundle(std::get<Token>(bin_op->right));
    }
    else
    {
        std::cout << "Cannot convert std::string to inst arg";
        exit(1);
    }

    int ret_reg = this->getFreeTempReg();

    lowerd_inst.args[0] = ret_reg;
    lowerd_inst.arg_types[0] = ArgType::REGISTER;
    lowerd_inst.args[1] = std::get<0>(left_arg_bundle);
    lowerd_inst.arg_types[1] = std::get<1>(left_arg_bundle);
    lowerd_inst.args[2] = std::get<0>(right_arg_bundle);
    lowerd_inst.arg_types[2] = std::get<1>(right_arg_bundle);


    if (left_is_BinOp)
    {
        this->freeTempReg(std::get<0>(left_arg_bundle));
    }
    if (right_is_BinOp)
    {
        this->freeTempReg(std::get<0>(right_arg_bundle));
    }

    this->lowerd_insts.push_back(lowerd_inst);
    return ret_reg;
}


std::tuple<int, ArgType> InstructionLowerer::nodeToArgBundle(Node node)
{
    if (std::holds_alternative<std::shared_ptr<BinOp>>(node))
    {
        int ret_reg = this->binOpToInst(std::get<std::shared_ptr<BinOp>>(node));
        return std::make_tuple(ret_reg, ArgType::POINTER);
    }
    else if (std::holds_alternative<Token>(node))
    {
        return tokenToArgBundle(std::get<Token>(node));
    }
    else
    {
        std::cout << "Cannot convert std::string to inst arg";
        exit(1);
    }
}


std::vector<Instruction> InstructionLowerer::lower()
{

    for (auto inst : this->ir_insts)
    {
        this->freeAllTempRegs();
        Instruction lower_inst = {};
        lower_inst.type = inst.type;
        
        // 3 arg instructions
        if (
            (inst.type == InstructionType::ADD) ||
            (inst.type == InstructionType::SUB) ||
            (inst.type == InstructionType::MUL) ||
            (inst.type == InstructionType::IDIV) ||
            (inst.type == InstructionType::FDIV) ||
            (inst.type == InstructionType::MOD)
        )
        {
            std::tuple<int, ArgType> arg1_bundle = this->nodeToArgBundle(inst.arg1);
            std::tuple<int, ArgType> arg2_bundle = this->nodeToArgBundle(inst.arg2);
            std::tuple<int, ArgType> arg3_bundle = this->nodeToArgBundle(inst.arg3);

            lower_inst.args[0] = std::get<0>(arg1_bundle);
            lower_inst.arg_types[0] = std::get<1>(arg1_bundle);
            lower_inst.args[1] = std::get<0>(arg2_bundle);
            lower_inst.arg_types[1] = std::get<1>(arg2_bundle);
            lower_inst.args[2] = std::get<0>(arg3_bundle);
            lower_inst.arg_types[2] = std::get<1>(arg3_bundle);
        }
        // 2 arg instructions
        else if (
            (inst.type == InstructionType::MOV) ||
            (inst.type == InstructionType::ABS) ||
            (inst.type == InstructionType::CMP) ||
            (inst.type == InstructionType::LOAD) ||
            (inst.type == InstructionType::STORE) ||
            (inst.type == InstructionType::PRINT))
        {
            std::tuple<int, ArgType> arg1_bundle = this->nodeToArgBundle(inst.arg1);
            std::tuple<int, ArgType> arg2_bundle = this->nodeToArgBundle(inst.arg2);

            lower_inst.args[0] = std::get<0>(arg1_bundle);
            lower_inst.arg_types[0] = std::get<1>(arg1_bundle);
            lower_inst.args[1] = std::get<0>(arg2_bundle);
            lower_inst.arg_types[1] = std::get<1>(arg2_bundle);
        }
        // 1 arg instructions
        else if (
            (inst.type == InstructionType::PUSH) ||
            (inst.type == InstructionType::POP) ||
            (inst.type == InstructionType::DEC) ||
            (inst.type == InstructionType::INC) ||
            (inst.type == InstructionType::TIME))
        {
            std::tuple<int, ArgType> arg1_bundle = this->nodeToArgBundle(inst.arg1);

            lower_inst.args[0] = std::get<0>(arg1_bundle);
            lower_inst.arg_types[0] = std::get<1>(arg1_bundle);
        }
        // instructions with a label
        // 1 arg instructions
        else if (
            (inst.type == InstructionType::JMP) ||
            (inst.type == InstructionType::JE) ||
            (inst.type == InstructionType::JNE) ||
            (inst.type == InstructionType::JG) ||
            (inst.type == InstructionType::JL) ||
            (inst.type == InstructionType::JGE) ||
            (inst.type == InstructionType::JLE))
        {
            std::cout << this-> << std::endl;

        }


        this->lowerd_insts.push_back(lower_inst);
    }

    return this->lowerd_insts;
}



#endif