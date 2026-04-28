# most of the gui is written by chatgpt. i did all the logic though
import os
import string
import tkinter as tk
from tkinter import ttk

from src.types import *
from src.vm import VirtualMachine

class VmState:
    def __init__(self, vm: VirtualMachine):
        self.vm = vm

        self.registers: list[int] = []
        self.memory: dict[int, int] = {}
        self.stack: list[int] = []

        self.lesser_flag: bool = False
        self.greater_flag: bool = False
        self.equal_flag: bool = False

        self.call_stack: list[int] = []
        self.ic: int = None

        self.takeSnapshot()

    def takeSnapshot(self) -> None:
        self.registers = self.vm.registers.copy()
        self.memory = self.vm.memory.copy()
        self.stack = self.vm.stack.copy()
        
        self.lesser_flag = self.vm.lesser_flag
        self.greater_flag = self.vm.greater_flag
        self.equal_flag = self.vm.equal_flag

        self.call_stack = self.vm.call_stack.copy()
        self.ic = self.vm.ic

    def loadSnapshot(self) -> None:
        self.vm.registers = self.registers.copy()
        self.vm.memory = self.memory.copy()
        self.vm.stack = self.stack.copy()
        
        self.vm.lesser_flag = self.lesser_flag
        self.vm.greater_flag = self.greater_flag
        self.vm.equal_flag = self.equal_flag

        self.vm.call_stack = self.call_stack.copy()
        self.vm.ic = self.ic


class VmDebugger:
    def __init__(self, vm: VirtualMachine, file_name: str) -> None:
        self.vm: VirtualMachine = vm
        self.file_name: str = file_name
        self.tok_lines: list[list[Token]] = self.vm.tok_lines.copy()
        self.last_regs: list[int] = self.vm.registers.copy()

        self.max_snapshtos: int = 100
        self.snapshots: list[VmState] = []


    def initGui(self, x_size = 900, y_size = 600) -> None:
        self.window = tk.Tk()
        self.window.protocol("WM_DELETE_WINDOW", self.windowDeconstructor)
        self.window.title(f"VM Debugger - {self.file_name}")
        self.window.geometry(f"{x_size}x{y_size}")
        
        # Store window dimensions for tracking changes
        self.last_width = x_size
        self.last_height = y_size
        
        # Configure grid weights for responsiveness
        self.window.grid_rowconfigure(0, weight=0)  # Control buttons row - fixed height
        self.window.grid_rowconfigure(1, weight=0)  # Header row - fixed height
        self.window.grid_rowconfigure(2, weight=1)  # Content row - expandable
        self.window.grid_columnconfigure(0, weight=3)  # Instructions - 3x wider
        self.window.grid_columnconfigure(1, weight=0)  # Scrollbar - minimal
        self.window.grid_columnconfigure(2, weight=1)  # Side panel
        self.window.grid_rowconfigure(3, weight=0)  # Bottom text box
        self.window.grid_rowconfigure(4, weight=0)  # Entry field
        
        # Control panel frame at top
        self.control_frame = ttk.Frame(self.window)
        self.control_frame.grid(row=0, column=0, columnspan=3, sticky="ew", padx=5, pady=5)
        
        # Control buttons
        self.step_btn = tk.Button(self.control_frame, text="Step", command=self.onStep)
        self.step_over_btn = tk.Button(self.control_frame, text="Step Over", command=self.onStepOver)
        self.step_out_btn = tk.Button(self.control_frame, text="Step Out", command=self.onStepOut)
        self.back_btn = tk.Button(self.control_frame, text="Back", command=self.onBack)
        self.go_btn = tk.Button(self.control_frame, text="Go", command=self.onGo)
        self.reload_btn = tk.Button(self.control_frame, text="Reload", command=self.onReload)

        self.step_btn.pack(side="left", padx=5)
        self.step_over_btn.pack(side="left", padx=5)
        self.step_out_btn.pack(side="left", padx=5)
        self.back_btn.pack(side="left", padx=5)
        self.go_btn.pack(side="left", padx=5)
        self.reload_btn.pack(side="left", padx=5)

        # Bottom text box
        self.output_text = tk.Text(self.window, height=5, wrap="word")
        self.output_text.grid(row=3, column=0, columnspan=3, sticky="ew", padx=5, pady=(5, 0))
        self.output_text.config(state="disabled")

        # Entry field below text box 
        self.console_entry = tk.Entry(self.window)
        self.console_entry.grid(row=4, column=0, columnspan=3, sticky="ew", padx=5, pady=(0, 5))

        # Main instruction panel header
        ttk.Label(self.window, text="Instructions", font=("Arial", 10, "bold")).grid(
            row=1, column=0, sticky="w", padx=5, pady=5
        )
        
        # Instruction treeview
        self.instruction_tree = ttk.Treeview(
            self.window, 
            columns=("IC", "Instruction", "Line"), 
            show="headings",
            selectmode="none"
        )
        self.instruction_tree.column("IC", width=40, anchor="w")
        self.instruction_tree.column("Instruction", width=100, anchor="w")
        self.instruction_tree.column("Line", width=40, anchor="w")
        self.instruction_tree.heading("IC", text="IC")
        self.instruction_tree.heading("Instruction", text="Instruction")
        self.instruction_tree.heading("Line", text="Line")
        self.instruction_tree.grid(row=2, column=0, sticky="nsew", padx=5, pady=5)
        
        # Scrollbar for instructions
        inst_scroll = ttk.Scrollbar(self.window, orient="vertical", command=self.instruction_tree.yview)
        inst_scroll.grid(row=2, column=1, sticky="ns", pady=5)
        self.instruction_tree.configure(yscroll=inst_scroll.set)
        
        # Side panel frame
        self.side_frame = ttk.Frame(self.window)
        self.side_frame.grid(row=1, column=2, rowspan=2, sticky="nsew", padx=5, pady=5)
        self.side_frame.grid_rowconfigure(1, weight=1)
        self.side_frame.grid_rowconfigure(3, weight=1)
        self.side_frame.grid_rowconfigure(5, weight=1)
        self.side_frame.grid_rowconfigure(7, weight=1)
        self.side_frame.grid_rowconfigure(9, weight=1)
        self.side_frame.grid_columnconfigure(0, weight=1)
        
        # Register panel header
        ttk.Label(self.side_frame, text="Registers", font=("Arial", 10, "bold")).grid(
            row=0, column=0, sticky="w"
        )
        
        # Register text widget
        self.reg_text = tk.Text(self.side_frame, wrap="none", font=("Courier", 9))
        self.reg_text.grid(row=1, column=0, sticky="nsew")
        
        # Memory panel header
        ttk.Label(self.side_frame, text="Memory", font=("Arial", 10, "bold")).grid(
            row=2, column=0, sticky="w", pady=(10, 0)
        )
        
        # Memory text widget
        self.mem_text = tk.Text(self.side_frame, wrap="none", font=("Courier", 9))
        self.mem_text.grid(row=3, column=0, sticky="nsew")
        
        # Stack panel header
        ttk.Label(self.side_frame, text="Stack", font=("Arial", 10, "bold")).grid(
            row=4, column=0, sticky="w", pady=(10, 0)
        )
        
        # Stack text widget
        self.stack_text = tk.Text(self.side_frame, wrap="none", font=("Courier", 9))
        self.stack_text.grid(row=5, column=0, sticky="nsew")
        
        # Flags panel header
        self.flags_label = tk.Label(self.side_frame, text="Flags", font=("Arial", 10))
        self.flags_label.grid(row=6, column=0, sticky="w", pady=(10, 0))
        
        
        # Call Stack panel header
        ttk.Label(self.side_frame, text="Call Stack", font=("Arial", 10, "bold")).grid(
            row=8, column=0, sticky="w", pady=(10, 0)
        )
        
        # Call Stack text widget
        self.call_stack_text = tk.Text(self.side_frame, wrap="none", font=("Courier", 9))
        self.call_stack_text.grid(row=9, column=0, sticky="nsew")
        
        # Populate initial data
        self.updateGui()
        
        # Bind resize event
        self.window.bind("<Configure>", self.onWindowResize)
        
        return

    def onWindowResize(self, event) -> None:
        if event.width != self.last_width or event.height != self.last_height:
            self.last_width = event.width
            self.last_height = event.height
        return

    def updateGui(self) -> None:
        self.updateInstructions()
        self.updateRegisters()
        self.updateMemory()
        self.updateStack()
        self.updateFlags()
        self.updateCallStack()
        self.updateButtons()
        return

    def updateInstructions(self) -> None:
        for item in self.instruction_tree.get_children():
            self.instruction_tree.delete(item)
        
        for i, inst in enumerate(self.vm.instructions):
            key = None
            if i in self.vm.labels.values():
                key = [k for k in self.vm.labels if self.vm.labels[k] == i][0] 
                self.instruction_tree.insert("", "end", values=(f" {i}", f"{key}:", f"{inst.line_number}"))

            inst_str = inst.__repr__()
            if inst_str[:2] in ["jm", "jn", "je", "jg", "jl"]:
                inst_str += f"({self.vm.labels[inst.args["label"].__repr__().strip()]})"
            
            if i == self.vm.ic:
                current_line_idx = len(self.instruction_tree.get_children()) 

            self.instruction_tree.insert("", "end", values=(f" {i}", f"{inst_str}", f"{inst.line_number}"))

        item = self.instruction_tree.get_children()[current_line_idx]
        self.instruction_tree.see(item)
        self.instruction_tree.selection_set(item)
        return

    def genRegisterText(self, space_per_reg: int, regs_per_line: int) -> list[str]:

        lines: list[str] = []
        
        buffer: str = ""
        i: int = 0
        while i < len(self.vm.registers):
            element = f"r{i}: {self.vm.registers[i]}"
            if len(element) > space_per_reg:
                element = f"{element[:space_per_reg-3]}..."
            buffer += f"{element}{" "*(space_per_reg-len(element))}"
            if (i+1) % regs_per_line == 0 and i != 0:
                lines.append(buffer)
                buffer = ""
            i += 1

        return lines

    def getChangedRegs(self) -> list[int]:
        """returns the indcies of the registers that differ to self.last_regs"""
        return [i for i in range(self.vm.register_count) if self.last_regs[i] != self.vm.registers[i]]
    
    def updateRegisters(self) -> None:
        space_per_reg: int = 20
        regs_per_line: int = 4

        self.reg_text.config(state="normal")
        self.reg_text.delete("1.0", "end")
        
        reg_text = self.genRegisterText(space_per_reg, regs_per_line)
        for line in reg_text:
            self.reg_text.insert("end", line + "\n")


        self.reg_text.tag_config("reset", background="white")
        self.reg_text.tag_add("reset", f"1.0", f"end") 

        self.reg_text.tag_config("reg_change", background="light green")
        changed_regs = self.getChangedRegs()
        for cr in changed_regs:
            cr += 1
            line = (cr // regs_per_line) + 1
            slot = (cr % regs_per_line) - 1
            idx = slot * space_per_reg

            self.reg_text.tag_add("reg_change", f"{line}.{idx}", f"{line}.{idx+space_per_reg}") 

                
        self.reg_text.config(state="disabled")
        return

    def updateMemory(self) -> None:
        self.mem_text.config(state="normal")
        self.mem_text.delete("1.0", "end")
        
        # Show first 8 memory locations
        for i in self.vm.memory:
            addr = self.vm.memory[i]
            self.mem_text.insert("end", f"[{i}]: {addr}\n")
        
        self.mem_text.config(state="disabled")
        return

    def updateStack(self) -> None:
        self.stack_text.config(state="normal")
        self.stack_text.delete("1.0", "end")
        
        # Show stack items from top to bottom
        if self.vm.stack:
            for i, val in enumerate(reversed(self.vm.stack)):
                self.stack_text.insert("end", f"{val}\n")
        else:
            self.stack_text.insert("end", "")
        
        self.stack_text.config(state="disabled")
        return

    def updateFlags(self) -> None:
        self.flags_label.config(text=f"Flags\tlesser: {self.vm.lesser_flag}\tequal: {self.vm.equal_flag}\tgreater: {self.vm.greater_flag}")

        return

    def updateCallStack(self) -> None:
        self.call_stack_text.config(state="normal")
        self.call_stack_text.delete("1.0", "end")
        
        # Show call stack items from top to bottom
        if self.vm.call_stack:
            for i, val in enumerate(reversed(self.vm.call_stack)):
                self.call_stack_text.insert("end", f"{val}\n")
        else:
            self.call_stack_text.insert("end", "")
        
        self.call_stack_text.config(state="disabled")
        return

    def disableButton(self, btn: tk.Button) -> None:
        btn["state"] = "disabled"
        btn.config(back = "#ACACAC")

    def enableButton(self, btn: tk.Button) -> None:
        btn["state"] = "normal"
        btn.config(back = "SystemButtonFace")

    def updateButtons(self) -> None:

        if len(self.vm.call_stack) == 0:
            self.disableButton(self.step_out_btn)
        else:
            self.enableButton(self.step_out_btn)

        if len(self.snapshots) == 0:
            self.disableButton(self.back_btn)
        else:
            self.enableButton(self.back_btn)

    def windowDeconstructor(self) -> None:
        self.window.destroy()
        exit(0)
        
    def stepWrapper(self, update_gui: bool = True) -> None:
        if update_gui:
            self.last_regs = self.vm.registers.copy()

        snapshot = VmState(self.vm)
        self.snapshots.append(snapshot)
        if len(self.snapshots) > self.max_snapshtos:
            self.snapshots.pop(0)

        res = self.vm.step()
        if update_gui:
            self.updateGui()
        return res

    def onStep(self) -> None:
        self.stepWrapper()
        pass

    def onStepOver(self) -> None:
        if (self.vm.ic < len(self.vm.instructions) and
            self.vm.instructions[self.vm.ic].type == InstructionType.CALL):
            self.stepWrapper()
            self.onStepOut()
        else:
            self.stepWrapper()

    def onStepOut(self) -> None:
        if len(self.vm.call_stack) == 0:
            return
        
        call_stack_size: int = len(self.vm.call_stack)
        while (self.vm.ic < len(self.vm.instructions) and 
               len(self.vm.call_stack) > (call_stack_size - 1)):
            self.stepWrapper()

    def onBack(self) -> None:
        if len(self.snapshots) == 0:
            return
        
        self.last_regs = self.vm.registers.copy()
        snapshot = self.snapshots.pop()
        snapshot.loadSnapshot()
        self.updateGui()
        return

    def onGo(self) -> None:
        done_step = False
        while (self.vm.instructions[self.vm.ic] and 
              (self.vm.instructions[self.vm.ic].type != InstructionType.BREAKPOINT or not done_step)):
            done_step = True
            self.stepWrapper(update_gui=False)
        self.updateGui()
        pass

    def onReload(self) -> None:
        self.vm.reset()

        self.reg_text.tag_config("reset", background="white")
        self.reg_text.tag_add("reset", f"1.0", f"end") 
        self.last_regs = self.vm.registers.copy()
        
        code = ""
        with open(self.file_name, "r") as f:
            code = f.read()
        self.vm.code = code.lower()
        self.vm.tokenize()
        self.vm.parse()

        self.updateGui()

    def run(self) -> None:
        self.window.mainloop()
