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

    int getFreeTempReg();
    void freeTempReg(int reg);
    void freeAllTempRegs();
    int binOpToInst(std::shared_ptr<BinOp> bin_op);
    std::tuple<int, ArgType> nodeToArgBundle(Node node);

    public:
    InstructionLowerer(std::vector<InstructionIr> ir_insts);
    std::vector<Instruction> lower();

};

#endif