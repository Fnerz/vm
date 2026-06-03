#include <iostream>
#include <fstream>
#include "./vm/vm.hpp"


int main(int argc, char *argv[])
{
    // if (argc == 1)
    // {
    //     std::cout << "Pls provide a source file";
    //     exit(1);
    // }

    std::string code = "";

    // std::ifstream file_reader(argv[1]);
    std::ifstream file_reader("./test.asm");
    std::string line;
    while (std::getline(file_reader, line))
    {
        code += line;
        code += '\n';
    }
    file_reader.close();

    VirtualMachine vm;
    vm.loadCode(code);
    vm.run();

    return 0;
}