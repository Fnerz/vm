#ifndef LOWERER_CPP
#define LOWERER_CPP

#include "lowerer.hpp"


std::string jmpInstBundleRepr(JmpInstBundle bundle)
{
    std::string ret = "";
    ret += "type: ";
    ret += instructionTypeRepr(bundle.type);
    ret += "\nlabel: ";
    ret += bundle.label;
    ret += "\nindex: ";
    ret += std::to_string(bundle.index);
    ret += "\n";
    return ret;
}

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
        return InstructionType::ADDI;
    }
    else if (tok.type == TokenType::MINUS)
    {
        return InstructionType::SUBI;
    }
    std::cout << "Cannot convert " << tokenRepr(tok) << " to InstructionType" << std::endl;
    exit(1);    
}

std::tuple<uint64_t, ArgType> tokenToArgBundle(Token tok)
{
    if (tok.type == TokenType::IMMEDIATE_I)
    {
        return std::make_tuple(std::stoi(tok.value), ArgType::IMMEDIATE_I);
    }
    if (tok.type == TokenType::IMMEDIATE_F)
    {
        return std::make_tuple(std::bit_cast<uint64_t>(static_cast<double>(std::stof(tok.value))), ArgType::IMMEDIATE_F);
    }
    if (tok.type == TokenType::IMMEDIATE_C)
    {
        uint64_t value = static_cast<uint64_t>(static_cast<unsigned char>(tok.value[0]));
        return std::make_tuple(value, ArgType::IMMEDIATE_C);
    }
    if (tok.type == TokenType::POINTER_R)
    {
        return std::make_tuple(std::stoi(tok.value), ArgType::POINTER_R);
    }
    if (tok.type == TokenType::POINTER_I)
    {
        return std::make_tuple(std::stoi(tok.value), ArgType::IMMEDIATE_I);
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
    std::tuple<uint64_t, ArgType> left_arg_bundle;
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
    std::tuple<uint64_t, ArgType> right_arg_bundle;
    if (std::holds_alternative<std::shared_ptr<BinOp>>(bin_op->right))
    {
        right_is_BinOp = true;
        int right_reg = this->binOpToInst(std::get<std::shared_ptr<BinOp>>(bin_op->right));
        right_arg_bundle = {right_reg, ArgType::POINTER_R};
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


std::tuple<uint64_t, ArgType> InstructionLowerer::nodeToArgBundle(Node node)
{
    if (std::holds_alternative<std::shared_ptr<BinOp>>(node))
    {
        uint64_t ret_reg = this->binOpToInst(std::get<std::shared_ptr<BinOp>>(node));
        return std::make_tuple(ret_reg, ArgType::POINTER_R);
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
    int jmp_inst_correction = 0; /* since the jmp insts are inserted later on, when all labels are resolved,
    and the labels use this->lower_insts.size() for their index we need to account for the missing jmp insts.*/
    std::string current_scope = "";
    std::vector<Instruction> lowerd_insts = {};
    if (this->ir_insts.empty())
    {
        return lowerd_insts;
    }
    for (size_t i = 0; i < this->ir_insts.size(); ++i)
    {
        const InstructionIr &inst = this->ir_insts[i];
        this->freeAllTempRegs();
        Instruction lower_inst = {};
        lower_inst.type = inst.type;
        
        // 3 arg instructions
        if (
            (inst.type == InstructionType::ADDI) ||
            (inst.type == InstructionType::ADDF) ||
            (inst.type == InstructionType::ADDU) ||
            (inst.type == InstructionType::SUBI) ||
            (inst.type == InstructionType::SUBF) ||
            (inst.type == InstructionType::SUBU) ||
            (inst.type == InstructionType::MULI) ||
            (inst.type == InstructionType::MULF) ||
            (inst.type == InstructionType::MULU) ||
            (inst.type == InstructionType::DIVI) ||
            (inst.type == InstructionType::DIVF) ||
            (inst.type == InstructionType::DIVU) ||
            (inst.type == InstructionType::MODI) ||
            (inst.type == InstructionType::MODF))
        {
            std::tuple<uint64_t, ArgType> arg1_bundle = this->nodeToArgBundle(inst.arg1);
            std::tuple<uint64_t, ArgType> arg2_bundle = this->nodeToArgBundle(inst.arg2);
            std::tuple<uint64_t, ArgType> arg3_bundle = this->nodeToArgBundle(inst.arg3);

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
            (inst.type == InstructionType::ABSI) ||
            (inst.type == InstructionType::ABSF) ||
            (inst.type == InstructionType::CMPI) ||
            (inst.type == InstructionType::CMPU) ||
            (inst.type == InstructionType::CMPF) ||
            (inst.type == InstructionType::LOAD) ||
            (inst.type == InstructionType::STORE) ||
            (inst.type == InstructionType::PRINTI)||
            (inst.type == InstructionType::PRINTF)||
            (inst.type == InstructionType::PRINTC)||
            (inst.type == InstructionType::PRINTU)||
            (inst.type == InstructionType::ITOF)||
            (inst.type == InstructionType::ITOU)||
            (inst.type == InstructionType::FTOI)||
            (inst.type == InstructionType::FTOU)||
            (inst.type == InstructionType::UTOF)||
            (inst.type == InstructionType::UTOI))
        {
            std::tuple<uint64_t, ArgType> arg1_bundle = this->nodeToArgBundle(inst.arg1);
            std::tuple<uint64_t, ArgType> arg2_bundle = this->nodeToArgBundle(inst.arg2);

            lower_inst.args[0] = std::get<0>(arg1_bundle);
            lower_inst.arg_types[0] = std::get<1>(arg1_bundle);
            lower_inst.args[1] = std::get<0>(arg2_bundle);
            lower_inst.arg_types[1] = std::get<1>(arg2_bundle);
        }
        // 1 arg instructions
        else if (
            (inst.type == InstructionType::PUSH) ||
            (inst.type == InstructionType::POP)  ||
            (inst.type == InstructionType::DECI) ||
            (inst.type == InstructionType::INCI) ||
            (inst.type == InstructionType::TIME))
        {
            std::tuple<uint64_t, ArgType> arg1_bundle = this->nodeToArgBundle(inst.arg1);

            lower_inst.args[0] = std::get<0>(arg1_bundle);
            lower_inst.arg_types[0] = std::get<1>(arg1_bundle);
        }
        // instructions with a label
        else if (
            (inst.type == InstructionType::JMP) ||
            (inst.type == InstructionType::JE)  ||
            (inst.type == InstructionType::JNE) ||
            (inst.type == InstructionType::JG)  ||
            (inst.type == InstructionType::JL)  ||
            (inst.type == InstructionType::JGE) ||
            (inst.type == InstructionType::JLE) ||
            (inst.type == InstructionType::JMPA)||
            (inst.type == InstructionType::JEA) ||
            (inst.type == InstructionType::JNEA)||
            (inst.type == InstructionType::JGA) ||
            (inst.type == InstructionType::JLA) ||
            (inst.type == InstructionType::JGEA)||
            (inst.type == InstructionType::JLEA)||
            (inst.type == InstructionType::CALL))
        {
            std::string raw_label;
            if (std::holds_alternative<Token>(inst.arg1))
            {
                Token arg = std::get<Token>(inst.arg1);
                JmpInstBundle jmp_inst_bundle = {};
                jmp_inst_bundle.type = inst.type;
                if (arg.type == TokenType::IMMEDIATE_I)
                {
                    jmp_inst_bundle.label = arg.value;
                    jmp_inst_bundle.index = this->lowerd_insts.size() + jmp_inst_correction;

                    this->label_indexes[arg.value] = std::stoi(arg.value);
                    this->unresolved_jmp_insts.push_back(jmp_inst_bundle);
                    jmp_inst_correction++;
                    continue;
                }
                if (arg.type == TokenType::REGISTER)
                {
                    jmp_inst_bundle.label = "reg";
                    jmp_inst_bundle.index = this->lowerd_insts.size() + jmp_inst_correction;
                    jmp_inst_bundle.absolute_jmp_addr = std::stoi(arg.value);

                    this->unresolved_jmp_insts.push_back(jmp_inst_bundle);
                    jmp_inst_correction++;
                    continue;
                }
                raw_label = arg.value;
            }
            else if (std::holds_alternative<std::string>(inst.arg1))
            {
                raw_label = std::get<std::string>(inst.arg1);
            }
            else
            {
                std::cerr << "Cannot resolve label from instruction arg variant" << std::endl;
                exit(1);
            }

            if (!raw_label.empty() && raw_label[0] == '.')
            {
                if (current_scope.empty())
                {
                    std::cerr << "Local label '" << raw_label << "' used outside of a scope" << std::endl;
                    exit(1);
                }
                raw_label = current_scope + raw_label;
            }

            JmpInstBundle jmp_inst_bundle = {};
            jmp_inst_bundle.type = inst.type;
            jmp_inst_bundle.label = raw_label;
            jmp_inst_bundle.index = this->lowerd_insts.size() + jmp_inst_correction;
            
            this->unresolved_jmp_insts.push_back(jmp_inst_bundle);
            
            jmp_inst_correction++;
            continue;
        }
        if (inst.type == InstructionType::LABEL)
        {
            std::string raw_label = std::get<std::string>(inst.arg1);
            std::string scoped_label = raw_label;
            if (!raw_label.empty() && raw_label[0] == '.')
            {
                if (current_scope.empty())
                {
                    std::cerr << "Local label '" << raw_label << "' declared outside of a scope" << std::endl;
                    exit(1);
                }
                scoped_label = current_scope + raw_label;
            }
            else
            {
                current_scope = raw_label;
            }

            int idx = this->lowerd_insts.size()+jmp_inst_correction;
            this->label_indexes[scoped_label] = idx;
            continue;
        }

        this->lowerd_insts.push_back(lower_inst);
    }

    // insert jmp instructions
    for (auto& jmp_inst_bundle : this->unresolved_jmp_insts)
    {
        Instruction lowerd_jmp = {};

        lowerd_jmp.type = jmp_inst_bundle.type;
        std::string label = jmp_inst_bundle.label;
        auto it = this->label_indexes.find(label);
        if ((it == this->label_indexes.end()) && (label != "reg") )
        {
            // External global label reference; keep a placeholder and relocate later
            if (!label.empty() && label[0] == '#')
            {
                lowerd_jmp.args[0] = 0;
                lowerd_jmp.arg_types[0] = ArgType::LABEL_INDEX;
                this->relocation_entries.push_back({jmp_inst_bundle.index, label});
                this->lowerd_insts.insert(this->lowerd_insts.begin() + jmp_inst_bundle.index, lowerd_jmp);
                continue;
            }
            std::cout << "Cannot resolve address of label " << label << std::endl;
            exit(1);
        }
        int global_idx = it->second;
        int idx = global_idx - jmp_inst_bundle.index;
        if (isAbsoluteJmpInst(jmp_inst_bundle.type))
        {
            idx = jmp_inst_bundle.absolute_jmp_addr;
        }
        lowerd_jmp.args[0] = idx;
        if (label == "reg")
        {

            lowerd_jmp.arg_types[0] = ArgType::REGISTER;
        }
        else
        {
            lowerd_jmp.arg_types[0] = ArgType::LABEL_INDEX;
        }
        
        std::cout << instructionRepr(lowerd_jmp) << std::endl;
        this->lowerd_insts.insert(this->lowerd_insts.begin() + jmp_inst_bundle.index, lowerd_jmp);
    }

    return this->lowerd_insts;
}

std::vector<RelocationEntry> InstructionLowerer::getRelocationEntries() const
{
    return this->relocation_entries;
}

std::map<std::string, int> InstructionLowerer::getSymbolTable() const
{
    return this->label_indexes;
}

#endif