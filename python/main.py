import sys
import os
import pygame
import cProfile
from src.screen import Screen
from src.vm import VirtualMachine
from src.debugger import VmDebugger


def printHelp():
    help = """
VM Interpreter Help

Usage: main.py <filename.vm> [options]

Options:
  -tokens                   Display the tokens
  -instructions             Display the instructions
  -registers <count>        Changes number of registers

"""
    print(help)
    return

def printArgErr(self, arg):
    print(f"Invalid argument structure for {arg}.\nRefrence -help for more Information.")
    return

def main(args):
    if len(args) == 0:
        printHelp()
        return
    
    file = args[0]
    if not os.path.isfile(file):
        print(f"{file} Not found")
        exit(1)

    
    show_tokens = False
    show_insts = False
    debug = False
    step_threw = False
    print_step = False
    profiler = False
    register_count = 64
    if len(args) > 1:
        args = args[1:]
        for i, arg in enumerate(args):
            if arg == "-tokens":
                show_tokens = True
            if arg == "-instructions":
                show_insts = True
            if arg == "-registers":
                if i+1 >= len(args):
                    printArgErr(arg)
                    exit(1)
                count = [i+1]
                try:
                    count = int(count)
                except:
                    printArgErr(arg)
                    exit(1)
                register_count = count

            if arg == "-step":
                step_threw = True

            if arg == "-print":
                print_step = True

            if arg == "-debug":
                debug = True
            if arg == "-profiler":
                profiler = True



    with open(file, "r") as f:
        code = f.read()

    vm = VirtualMachine(code, register_count, debug, step_threw, print_step)
    
    if show_tokens:
        for tok in vm.tokens:
            print(tok)
        print("="*35)

    if show_insts:
        for inst in vm.instructions:
            print(inst)

    if debug:
        debugger = VmDebugger(vm, file)
        debugger.initGui()
        debugger.run()


    if profiler:
        with cProfile.Profile() as profile:
            vm.run()
        profile.print_stats()
        profile.dump_stats("./profiling_res.prof")
    else:
        vm.run()

    return



if __name__ == "__main__":
    main(sys.argv[1:])

