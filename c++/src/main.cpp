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

    // std::string code = "";

    // std::ifstream file_reader(argv[1]);
    // std::string line;
    // while (std::getline(file_reader, line))
    // {
    //     code += line;
    //     code += '\n';
    // }
    // file_reader.close();

    // bool debug = false;
    // for (int i = 2; i < argc; i++)
    // {
    //     if (std::string(argv[i]) == std::string("-debug"))
    //     {
    //         debug = true;
    //     }
    // }

    // VirtualMachine vm;
    // vm.loadCode(code, debug);
    // vm.run();

    return 0;
}