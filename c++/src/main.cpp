#include <iostream>
#include <fstream>
#include <SDL3/SDL.h>
#include "./vm/vm/vm.hpp"
#include "./vm/assembler/assembler.hpp"
#include "./vm/assembler/linker.hpp"


int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Pls provide a source file";
        exit(1);
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "VM Display",
        500,
        350,
        0);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_StartTextInput(window);

    bool debug = false;
    bool runBinary = false;
    bool linkMode = false;
    std::string outputName = "";
    std::vector<std::string> positional;

    for (int i = 1; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (arg == "-debug")
        {
            debug = true;
        }
        else if (arg == "-run")
        {
            runBinary = true;
        }
        else if (arg == "-link")
        {
            linkMode = true;
        }
        else if (arg == "-o")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: -o requires an output base name.\n";
                return 1;
            }
            outputName = argv[++i];
        }
        else if (!arg.empty() && arg[0] == '-')
        {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
        }
        else
        {
            positional.push_back(arg);
        }
    }

    if (positional.empty())
    {
        std::cerr << "Usage: " << argv[0] << " [<source.asm> ...] [-link] [-o output] [-run] [-debug]\n";
        std::cerr << "  Single file: " << argv[0] << " source.asm [-o output] [-run] [-debug]\n";
        std::cerr << "  Link files: " << argv[0] << " -link file1.asm file2.asm ... [-o output] [-run] [-debug]\n";
        return 1;
    }

    std::string inputPath = positional[0];
    auto hasBinExtension = [](const std::string &name) {
        return name.size() >= 4 && name.substr(name.size() - 4) == ".bin";
    };

    std::string binaryPath;
    VmAssembler vm_asm("", debug);
    bool shouldRun = false;

    if (linkMode)
    {
        // Linking mode: use all positional arguments as input files
        try
        {
            std::string dumpName = outputName.empty() ? "linked" : outputName;
            
            VmLinker linker(debug);
            linker.linkFiles(positional, debug);
            linker.dumpToBinary(dumpName);
            
            binaryPath = dumpName + ".bin";
            shouldRun = runBinary;
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Linking failed: " << ex.what() << std::endl;
            SDL_StopTextInput(window);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }
    else if (hasBinExtension(inputPath))
    {
        binaryPath = inputPath;
        shouldRun = true;
    }
    else
    {
        std::cout << "Single file compiler" << std::endl;
        std::ifstream file_reader(inputPath);
        if (!file_reader)
        {
            std::cerr << "Failed to open source file: " << inputPath << std::endl;
            return 1;
        }

        std::string code;
        std::string line;
        while (std::getline(file_reader, line))
        {
            code += line;
            code += '\n';
        }
        file_reader.close();

        std::string dumpName;
        if (!outputName.empty())
        {
            dumpName = outputName;
        }
        else
        {
            dumpName = inputPath;
            if (dumpName.size() > 4 && dumpName.substr(dumpName.size() - 4) == ".asm")
            {
                dumpName = dumpName.substr(0, dumpName.size() - 4);
            }
        }

        vm_asm = VmAssembler(code, debug);
        vm_asm.assamble();
        vm_asm.dumpToBinary(dumpName);
        binaryPath = dumpName;
        shouldRun = runBinary;
    }

    if (shouldRun)
    {
        try
        {
            auto words = vm_asm.loadFromBinary(binaryPath);
            VirtualMachine vm;
            vm.loadInstBinary(words);
            vm.run();
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Execution failed: " << ex.what() << std::endl;
            SDL_StopTextInput(window);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    SDL_StopTextInput(window);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}