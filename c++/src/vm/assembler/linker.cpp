#ifndef LINKER_CPP
#define LINKER_CPP

#include "linker.hpp"
#include "assembler.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

VmLinker::VmLinker(bool debug)
{
    this->debug = debug;
}

bool VmLinker::isGlobalSymbol(const std::string& name)
{
    return !name.empty() && name[0] == '#';
}

bool VmLinker::isFileLocal(const std::string& name)
{
    return !isGlobalSymbol(name);
}

void VmLinker::linkFiles(const std::vector<std::string>& asm_files, bool debug)
{
    if (asm_files.empty())
    {
        throw std::runtime_error("No assembly files provided to linker");
    }

    int current_offset = 0;

    for (const auto& asm_file : asm_files)
    {
        std::ifstream file(asm_file);
        if (!file)
        {
            throw std::runtime_error("Cannot open file: " + asm_file);
        }

        std::string code((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
        file.close();

        VmAssembler assembler(code, debug);
        assembler.assamble();

        auto instructions = assembler.getInstructions();
        auto symbols = assembler.getSymbolTable();
        auto relocations = assembler.getRelocationTable();

        LinkedFile linked_file;
        linked_file.instructions = instructions;
        linked_file.offset = current_offset;
        linked_file.relocations = relocations;

        for (const auto& [symbol_name, local_index] : symbols)
        {
            int absolute_address = current_offset + local_index;

            if (isGlobalSymbol(symbol_name))
            {
                if (this->global_symbols.find(symbol_name) != this->global_symbols.end())
                {
                    throw std::runtime_error("Global symbol '" + symbol_name + 
                                               "' defined in multiple files");
                }
                this->global_symbols[symbol_name] = absolute_address;

                if (debug)
                {
                    std::cout << "Global symbol: " << symbol_name << " @ " << absolute_address << std::endl;
                }
            }

            linked_file.symbols[symbol_name] = absolute_address;
        }

        if (this->entry_point == -1)
        {
            if (symbols.find("main") != symbols.end())
            {
                this->entry_point = current_offset + symbols["main"];
            }
            else if (this->global_symbols.find("#main") != this->global_symbols.end())
            {
                this->entry_point = this->global_symbols["#main"];
            }
        }

        current_offset += instructions.size();
        this->linked_files.push_back(linked_file);

        if (debug)
        {
            std::cout << "Linked file: " << asm_file << " (" << instructions.size() 
                     << " instructions, offset " << linked_file.offset << ")" << std::endl;
        }
    }

    if (this->entry_point == -1)
    {
        throw std::runtime_error("No main: label found as entry point");
    }

    if (debug)
    {
        std::cout << "Entry point: main @ " << this->entry_point << std::endl;
    }

    // concatenate insts and patch addresses
    for (const auto& linked_file : this->linked_files)
    {
        std::map<int, std::string> relocation_map;
        for (const auto& relocation : linked_file.relocations)
        {
            relocation_map[relocation.index] = relocation.symbol;
        }

        for (int local_index = 0; local_index < static_cast<int>(linked_file.instructions.size()); ++local_index)
        {
            const auto& inst = linked_file.instructions[local_index];
            Instruction patched_inst = inst;

            if ((inst.type == InstructionType::JMP || 
                 inst.type == InstructionType::JE || 
                 inst.type == InstructionType::JNE ||
                 inst.type == InstructionType::JG || 
                 inst.type == InstructionType::JL ||
                 inst.type == InstructionType::JGE || 
                 inst.type == InstructionType::JLE ||
                 inst.type == InstructionType::CALL) &&
                inst.arg_types[0] == ArgType::LABEL_INDEX)
            {
                auto reloc_it = relocation_map.find(local_index);
                if (reloc_it != relocation_map.end())
                {
                    const std::string& symbol = reloc_it->second;
                    int absolute_index;

                    if (isGlobalSymbol(symbol))
                    {
                        auto global_it = this->global_symbols.find(symbol);
                        if (global_it == this->global_symbols.end())
                        {
                            throw std::runtime_error("Undefined global symbol: " + symbol);
                        }
                        absolute_index = global_it->second;
                    }
                    else
                    {
                        auto local_it = linked_file.symbols.find(symbol);
                        if (local_it == linked_file.symbols.end())
                        {
                            throw std::runtime_error("Undefined local symbol: " + symbol);
                        }
                        absolute_index = local_it->second;
                    }
                    patched_inst.args[0] = absolute_index;
                }
                else
                {
                    int local_target = static_cast<int>(inst.args[0]);
                    patched_inst.args[0] = linked_file.offset + local_target;
                }
            }

            this->final_instructions.push_back(patched_inst);
        }
    }

    if (debug)
    {
        std::cout << "Total instructions after linking: " << this->final_instructions.size() << std::endl;
    }
}

void VmLinker::dumpToBinary(std::string name)
{
    if (name.size() < 4 || name.substr(name.size() - 4) != ".bin")
    {
        name += ".bin";
    }

    std::ofstream file(name, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to create binary file: " + name);
    }

    file.write(
        reinterpret_cast<const char*>(this->final_instructions.data()),
        this->final_instructions.size() * sizeof(Instruction)
    );

    file.close();

    if (this->debug)
    {
        std::cout << "================\n";
       for (int i = 0; i < this->final_instructions.size(); i++)
       {
            std::cout << i << " " << instructionRepr(this->final_instructions[i]) << std::endl;
       }
    }

    std::cout << "Linked binary written to: " << name << std::endl;
}

std::vector<Instruction> VmLinker::getInstructions() const
{
    return this->final_instructions;
}

int VmLinker::getEntryPoint() const
{
    return this->entry_point;
}

#endif
