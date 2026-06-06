#include <iostream>
#include <fstream>
// #include <SDL3/SDL.h>
#include "./vm/vm.hpp"
#include "./vm/assembler.hpp"


int main(int argc, char *argv[])
{
    // if (argc == 1)
    // {
    //     std::cout << "Pls provide a source file";
    //     exit(1);
    // }

    // if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    // {
    //     std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    //     return 1;
    // }

    // SDL_Window *window = SDL_CreateWindow(
    //     "VM Display",
    //     640,
    //     640,
    //     0);
    // if (!window)
    // {
    //     std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    //     SDL_Quit();
    //     return 1;
    // }

    // SDL_StartTextInput(window);

    std::string code = "";

    std::ifstream file_reader(argv[1]);
    std::string line;
    while (std::getline(file_reader, line))
    {
        code += line;
        code += '\n';
    }
    file_reader.close();

    bool debug = false;
    for (int i = 2; i < argc; i++)
    {
        if (std::string(argv[i]) == std::string("-debug"))
        {
            debug = true;
        }
    }

    VmAssembler vm_asm(code, debug);
    vm_asm.assamble();
    vm_asm.dumpToBinary("./test");
    auto words = vm_asm.loadFromBinary("./test");
    VirtualMachine vm;
    std::cout << "vm" << std::endl;
    vm.loadInstBinary(words);
    vm.run();

    // std::cout << "====================\n";
    // for (auto inst : insts)
    // {
    //     std::cout << instructionRepr(inst) << std::endl;
    // }

    // std::cout << "====================\n";
    // for (auto word : words)
    // {
    //     std::cout << std::hex << word << std::dec << std::endl;
    // }

    // SDL_StopTextInput(window);
    // SDL_DestroyWindow(window);
    // SDL_Quit();

    return 0;
}