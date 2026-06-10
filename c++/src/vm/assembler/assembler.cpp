#ifndef VM_ASSEMBLER_CPP
#define VM_ASSEMBLER_CPP

#include "assembler.hpp"

VmAssembler::VmAssembler(std::string code, bool debug)
{
    this->code = code;
    this->debug = debug;
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

void VmAssembler::tokenize()
{
    std::vector<std::string> lines = splitCodeToLines(this->code);
    for (auto line : lines)
    {
        LineLexer lexer(line);
        this->token_lines.push_back(lexer.tokenize());
    }
    return;
}

void VmAssembler::parse()
{
    for (auto tok_line : this->token_lines)
    {
        LineParser parser(tok_line);
        auto insts = parser.parse(); // parser is expected to return only one instruction atm, but i chose to go the safe route.
        for (auto inst : insts)
            this->ir_instructions.push_back(inst);
    }
    return;
}

void VmAssembler::lower()
{
    InstructionLowerer lowerer(this->ir_instructions);
    this->instructions = lowerer.lower();
    this->symbol_table = lowerer.getSymbolTable();
    this->relocation_table = lowerer.getRelocationEntries();
}

static std::string ensureBinaryPath(std::string name)
{
    if (name.size() >= 4 && name.substr(name.size() - 4) == ".bin")
    {
        return name;
    }
    return name + std::string(".bin");
}

void VmAssembler::assamble()
{
    std::transform(this->code.begin(), this->code.end(), this->code.begin(),[](unsigned char c){ return std::tolower(c); }); // make everything lower case.
    this->tokenize();
    this->parse();
    if  (this->debug)
    {
        for (auto inst : this->ir_instructions)
        {
            std::cout << instructionIrRepr(inst) << std::endl;
        }
        std::cout << "==========================================" << std::endl;
    }
    this->lower();
    if (this->debug)
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

void VmAssembler::dumpToBinary(std::string name)
{
    std::string path = ensureBinaryPath(name);
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to create binary file: " + path);
    }
    file.write(
        reinterpret_cast<const char*>(this->instructions.data()),
        this->instructions.size() * sizeof(Instruction)
    );

    return;
}

std::vector<uint8_t> VmAssembler::loadFromBinary(std::string name)
{
    std::string path = ensureBinaryPath(name);
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        throw std::runtime_error("Failed to open binary file: " + path);
    }

    std::streamsize file_size = file.tellg();
    if (file_size < 0 || file_size % sizeof(uint8_t) != 0)
    {
        throw std::runtime_error("Invalid binary file size: " + path);
    }

    std::vector<uint8_t> words(static_cast<size_t>(file_size) / sizeof(uint8_t));
    file.seekg(0, std::ios::beg);
    file.read(
        reinterpret_cast<char*>(words.data()),
        words.size() * sizeof(uint8_t)
    );

    if (!file)
    {
        throw std::runtime_error("Failed to read binary file: " + path);
    }

    return words;
}

std::vector<Instruction> VmAssembler::getInstructions() const
{
    return this->instructions;
}

std::map<std::string, int> VmAssembler::getSymbolTable() const
{
    return this->symbol_table;
}

std::vector<RelocationEntry> VmAssembler::getRelocationTable() const
{
    return this->relocation_table;
}

#endif 
