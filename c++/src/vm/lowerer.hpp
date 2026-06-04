#ifndef LOWERER_HPP
#define LOWERER_HPP

#include <map>
#include <tuple>
#include "types.hpp"

enum RegisterState
{
    FREE,
    RESERVED
};

struct JmpInstBundle
{
    InstructionType type;
    std::string label;
    int index;
};
std::string jmpInstBundleRepr(JmpInstBundle bundle);


class InstructionLowerer
{
    private:
    std::vector<InstructionIr> ir_insts = {};
    std::vector<Instruction> lowerd_insts = {};
    std::map<int, RegisterState> temp_regs = {
                                    {30, RegisterState::FREE},
                                    {31, RegisterState::FREE},
                                    {32, RegisterState::FREE},
                                    {33, RegisterState::FREE}};
    std::map<std::string, int>  label_indexes = {};
    std::vector<JmpInstBundle> unresolved_jmp_insts = {};

    int getFreeTempReg();
    void freeTempReg(int reg);
    void freeAllTempRegs();
    int binOpToInst(std::shared_ptr<BinOp> bin_op);
    std::tuple<uint64_t, ArgType> nodeToArgBundle(Node node);

    public:
    InstructionLowerer(std::vector<InstructionIr> ir_insts);
    std::vector<Instruction> lower();

};

#endif