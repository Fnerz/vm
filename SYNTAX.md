# VM Command Syntax Reference
(Ai wrote this)

Complete reference guide for the Virtual Machine assembly-like language. This document covers syntax, all available commands, data types, and execution model.

---

## Table of Contents

1. [General Syntax Rules](#general-syntax-rules)
2. [Arithmetic Commands](#arithmetic-commands)
3. [Increment/Decrement Commands](#incrementdecrement-commands)
4. [Comparison Commands](#comparison-commands)
5. [Jump Commands](#jump-commands)
6. [Function Call Commands](#function-call-commands)
7. [Memory Commands](#memory-commands)
8. [Stack Commands](#stack-commands)
9. [Input/Output Commands](#inputoutput-commands)
10. [Debugging Commands](#debugging-commands)
11. [Graphics Commands](#graphics-commands)
12. [Labels](#labels)
13. [Value Types Reference](#value-types-reference)
14. [Execution Model](#execution-model)
15. [Common Patterns](#common-patterns)
16. [Error Handling](#error-handling)

---

## General Syntax Rules

- **Case-insensitive**: All commands are case-insensitive (`MOV`, `mov`, `Mov` are equivalent)
- **Comments**: Use `;` to start a comment (everything after `;` on a line is ignored)
- **Labels**: Defined with `label_name:` on a line by itself
- **Registers**: Reference with `r0` through `r15` by default (configurable via `-registers` flag)
- **Immediate values**: Prefix with `i` for positive (e.g., `i42` for 42), or `in` for negative (e.g., `in5` for -5)
- **Memory addressing**: Use `[address]` for pointer access (e.g., `[r0]`, `[r0+i5]`, `[i10-r1]`)

### Syntax Example
```
; This is a comment
mov r0, i42          ; Copy 42 into register 0
loop:                ; Define a label
    add r1, r0, i1   ; Perform operation
    jmp loop         ; Jump to label
```

---

## Arithmetic Commands

### MOV - Move/Copy Value
**Syntax**: `mov <dest>, <src>`

**Description**: Copies a value from source to destination.

**Arguments**:
- `dest`: Destination (register or memory address)
- `src`: Source (register, immediate value, or memory address)

**Examples**:
```
mov r0, i42          ; Copy value 42 into r0
mov r1, r0           ; Copy r0 into r1
mov r2, [r3]         ; Load value from memory at address in r3 into r2
mov [r0], i100       ; Store value 100 at memory address in r0
```

---

### ADD - Addition
**Syntax**: `add <dest>, <left>, <right>`

**Description**: Adds two values and stores the result in destination: `dest = left + right`

**Arguments**:
- `dest`: Destination register for result
- `left`: First operand (register, immediate, or memory)
- `right`: Second operand (register, immediate, or memory)

**Examples**:
```
add r0, i5, i10      ; r0 = 5 + 10 = 15
add r1, r0, r2       ; r1 = r0 + r2
add r3, [r0], i4     ; r3 = memory[r0] + 4
```

---

### SUB - Subtraction
**Syntax**: `sub <dest>, <left>, <right>`

**Description**: Subtracts the right value from left and stores in destination: `dest = left - right`

**Arguments**:
- `dest`: Destination register for result
- `left`: First operand (minuend)
- `right`: Second operand (subtrahend)

**Examples**:
```
sub r0, i20, i7      ; r0 = 20 - 7 = 13
sub r1, r0, r2       ; r1 = r0 - r2
sub r2, [r3], i5     ; r2 = memory[r3] - 5
```

---

### MUL - Multiplication
**Syntax**: `mul <dest>, <left>, <right>`

**Description**: Multiplies two values and stores the result in destination: `dest = left * right`

**Arguments**:
- `dest`: Destination register for result
- `left`: First operand (multiplicand)
- `right`: Second operand (multiplier)

**Examples**:
```
mul r0, i6, i7       ; r0 = 6 * 7 = 42
mul r1, r0, i2       ; r1 = r0 * 2
mul r3, [r0], r4     ; r3 = memory[r0] * r4
```

---

### IDIV - Integer Division
**Syntax**: `idiv <dest>, <left>, <right>`

**Description**: Performs integer division (floor division) and stores quotient in destination: `dest = left // right`

**Arguments**:
- `dest`: Destination register for result
- `left`: First operand (dividend)
- `right`: Second operand (divisor, cannot be zero)

**Errors**:
- Throws "Division by zero" exception if right operand is 0

**Examples**:
```
idiv r0, i20, i3     ; r0 = 20 // 3 = 6
idiv r1, r0, r2      ; r1 = r0 // r2
idiv r2, [r0], i4    ; r2 = memory[r0] // 4
```

---

### FDIV - Floating-Point Division
**Syntax**: `fdiv <dest>, <left>, <right>`

**Description**: Performs floating-point division: `dest = left / right`

**Arguments**:
- `dest`: Destination register for result
- `left`: First operand (dividend)
- `right`: Second operand (divisor, cannot be zero)

**Errors**:
- Throws "Division by zero" exception if right operand is 0

**Examples**:
```
fdiv r0, i10, i3     ; r0 = 10 / 3 ≈ 3.333
fdiv r1, r0, i2      ; r1 = r0 / 2
fdiv r2, [r0], r3    ; r2 = memory[r0] / r3
```

---

### MOD - Modulo (Remainder)
**Syntax**: `mod <dest>, <left>, <right>`

**Description**: Computes the remainder of division: `dest = left % right`

**Arguments**:
- `dest`: Destination register for result
- `left`: First operand (dividend)
- `right`: Second operand (divisor, cannot be zero)

**Errors**:
- Throws "Division by zero" exception if right operand is 0

**Examples**:
```
mod r0, i10, i3      ; r0 = 10 % 3 = 1
mod r1, r0, r2       ; r1 = r0 % r2
mod r2, [r0], i5     ; r2 = memory[r0] % 5
```

---

## Increment/Decrement Commands

Simple single-operand arithmetic operations.

### INC - Increment
**Syntax**: `inc <dest>`

**Description**: Increments a register by 1: `dest = dest + 1`

**Arguments**:
- `dest`: Register to increment

**Examples**:
```
inc r0               ; r0 = r0 + 1
inc r5               ; Increment r5
```

---

### DEC - Decrement
**Syntax**: `dec <dest>`

**Description**: Decrements a register by 1: `dest = dest - 1`

**Arguments**:
- `dest`: Register to decrement

**Examples**:
```
dec r0               ; r0 = r0 - 1
dec r5               ; Decrement r5
```

---

## Comparison Commands

### CMP - Compare
**Syntax**: `cmp <left>, <right>`

**Description**: Compares two values and sets comparison flags. Does not modify any registers or memory.

**Flags Set**:
- `lesser_flag`: Set to true if `left < right`
- `equal_flag`: Set to true if `left == right`
- `greater_flag`: Set to true if `left > right`

**Arguments**:
- `left`: First operand to compare
- `right`: Second operand to compare

**Examples**:
```
cmp r0, i10          ; Compare r0 with 10
cmp r0, r1           ; Compare r0 with r1
cmp [r0], i42        ; Compare value at memory[r0] with 42
```

---

## Jump Commands

All jump commands are based on flags set by the `cmp` instruction.

### JMP - Unconditional Jump
**Syntax**: `jmp <label>`

**Description**: Unconditionally jumps to the specified label.

**Arguments**:
- `label`: Target label name

**Examples**:
```
start:
    jmp loop
    i0                ; This line is skipped
loop:
    mov r0, i5
```

---

### JE - Jump if Equal
**Syntax**: `je <label>`

**Description**: Jumps to label if `equal_flag` is set (left == right from last `cmp`).

**Arguments**:
- `label`: Target label name

**Examples**:
```
cmp r0, i10
je equal_handler     ; Jump if r0 == 10
mov r1, i1          ; Execute if not equal
jmp end
equal_handler:
    mov r1, i2
end:
```

---

### JNE - Jump if Not Equal
**Syntax**: `jne <label>`

**Description**: Jumps to label if `equal_flag` is NOT set (left != right from last `cmp`).

**Arguments**:
- `label`: Target label name

**Examples**:
```
cmp r0, i10
jne not_equal_handler  ; Jump if r0 != 10
mov r1, i0            ; Execute if equal
jmp end
not_equal_handler:
    mov r1, i1
end:
```

---

### JG - Jump if Greater
**Syntax**: `jg <label>`

**Description**: Jumps to label only if the `greater_flag` is set (left operand of last `cmp` was greater).

**Arguments**:
- `label`: Target label name

**Examples**:
```
cmp r0, i10
jg is_greater       ; Jump if r0 > 10
mov r1, i0
jmp skip
is_greater:
    mov r1, i1
skip:
```

---

### JL - Jump if Less
**Syntax**: `jl <label>`

**Description**: Jumps to label only if the `lesser_flag` is set (left operand of last `cmp` was less).

**Arguments**:
- `label`: Target label name

**Examples**:
```
cmp r0, i10
jl is_less          ; Jump if r0 < 10
mov r1, i1
jmp skip
is_less:
    mov r1, i0
skip:
```

---

### JGE - Jump if Greater or Equal
**Syntax**: `jge <label>`

**Description**: Jumps to label if `greater_flag` OR `equal_flag` is set (left operand of last `cmp` was >= right operand).

**Arguments**:
- `label`: Target label name

**Examples**:
```
cmp r0, i10
jge is_gte          ; Jump if r0 >= 10
mov r1, i0
jmp skip
is_gte:
    mov r1, i1
skip:
```

---

### JLE - Jump if Less or Equal
**Syntax**: `jle <label>`

**Description**: Jumps to label if `lesser_flag` OR `equal_flag` is set (left operand of last `cmp` was <= right operand).

**Arguments**:
- `label`: Target label name

**Examples**:
```
cmp r0, i10
jle is_lte          ; Jump if r0 <= 10
mov r1, i1
jmp skip
is_lte:
    mov r1, i0
skip:
```

---

## Function Call Commands

Support for subroutine calls with automatic return address management via call stack.

### CALL - Call Subroutine
**Syntax**: `call <label>`

**Description**: Calls a subroutine at the specified label, automatically saving return address.

**Arguments**:
- `label`: Target label name of the subroutine

**Behavior**:
- Pushes current instruction counter + 1 onto call stack
- Jumps to the specified label
- Subroutine ends with `ret` to return

**Errors**:
- Throws "Undefined label" if label doesn't exist

**Examples**:
```
main:
    call subroutine
    ; execution continues here after subroutine returns
    jmp end

subroutine:
    mov r0, i42
    ret
end:
    print
```

---

### RET - Return from Subroutine
**Syntax**: `ret`

**Description**: Returns from a subroutine by popping return address from call stack.

**Arguments**: None

**Behavior**:
- Pops return address from call stack
- Sets instruction counter to return address
- Enables nested subroutine calls

**Errors**:
- Throws "Call stack underflow" if call stack is empty

**Examples**:
```
subroutine:
    add r0, r0, i1
    ret

main:
    mov r0, i5
    call subroutine
    print
```

---

## Memory Commands

Direct memory access and manipulation.

### LOAD - Load from Memory
**Syntax**: `load <dest>, [<address>]`

**Description**: Loads a value from memory at specified address into a register.

**Arguments**:
- `dest`: Destination register to store loaded value
- `address`: Memory address (supports pointer arithmetic)

**Behavior**:
- If address is not in memory, loads 0 (default value)

**Examples**:
```
load r0, [i100]      ; Load from memory[100] into r0
load r1, [r2]        ; Load from memory[r2] into r1
load r3, [r0+i5]     ; Load from memory[r0 + 5] into r3
load r4, [r1-i2]     ; Load from memory[r1 - 2] into r4
```

---

### STORE - Store to Memory
**Syntax**: `store <src>, [<address>]`

**Description**: Stores a value into memory at the specified address.

**Arguments**:
- `src`: Source value to store (register or immediate)
- `address`: Memory address (supports pointer arithmetic)

**Examples**:
```
store r0, [i100]     ; Store r0 into memory[100]
store i42, [r1]      ; Store 42 into memory[r1]
store r2, [r0+i3]    ; Store r2 into memory[r0 + 3]
store i100, [r1-i4]  ; Store 100 into memory[r1 - 4]
```

---

### Pointer Arithmetic

Memory addresses support arithmetic expressions in brackets:

**Syntax**: `[<base> +/- <offset> +/- ... ]`

**Rules**:
- Base can be a register or immediate value
- Can chain multiple additions and subtractions
- Operations are performed at runtime
- Both positive and negative results are valid

**Examples**:
```
load r0, [r1+i5]        ; Address = r1 + 5
store r0, [i100-r1]     ; Address = 100 - r1
load r2, [r3+i10-r4]    ; Address = r3 + 10 - r4
mov r5, [r2-i3]         ; Address = r2 - 3
```

---

## Stack Commands

The VM maintains a Last-In-First-Out (LIFO) stack for temporary data storage.

### PUSH - Push Value onto Stack
**Syntax**: `push <src>`

**Description**: Pushes a value onto the stack.

**Arguments**:
- `src`: Source value to push (register, immediate, or memory address)

**Examples**:
```
push r0             ; Push r0 onto stack
push i42            ; Push value 42 onto stack
push [r1]           ; Push value from memory[r1] onto stack
push [r0+i5]        ; Push value from memory[r0+5] onto stack
```

---

### POP - Pop Value from Stack
**Syntax**: `pop <dest>`

**Description**: Pops a value from the stack into a register.

**Arguments**:
- `dest`: Destination register to store popped value

**Behavior**:
- Values are popped in LIFO order (last pushed is first popped)

**Errors**:
- Throws "Stack underflow" if stack is empty

**Examples**:
```
pop r0              ; Pop top of stack into r0
pop r1              ; Pop next value into r1
push i100
push i200
pop r0              ; r0 = 200
pop r1              ; r1 = 100
```

---

## Input/Output Commands

Commands for reading input and displaying output.


### PRINT - Print Register Range
**Syntax**: `print <start>, <end>`

**Description**: Prints values from a range of registers or a single register.

**Arguments**:
- `start`: Starting register index (register or immediate)
- `end`: Ending register index (register or immediate)

**Behavior**:
- If start == end, prints single register value
- Otherwise prints array of all registers from start to end

**Examples**:
```
mov r0, i10
mov r1, i20
mov r2, i30
print r0, r0       ; Prints: 10
print r0, r2       ; Prints: [10, 20, 30]
```

---

### INPUT - Read User Input
**Syntax**: `input <dest>`

**Description**: Reads an integer from user input and stores it in a register.

**Arguments**:
- `dest`: Destination register to store input value

**Behavior**:
- Prompts user with "Input: " message
- Reads a line from stdin
- Converts to integer and stores in register

**Errors**:
- Throws "Invalid input" if input cannot be parsed as integer

**Examples**:
```
input r0            ; Prompts user, reads integer into r0
input r5            ; Prompts user, reads integer into r5
```

---

## Debugging Commands

Commands for debugging and timing operations.

### BP - Breakpoint
**Syntax**: `bp`

**Description**: Pauses execution only in debugging mode.

**Behavior**:
- Halts VM execution
- Waits for the user to press Enter
- Resumes execution on next input

**Examples**:
```
mov r0, i5
bp                  ; Execution pauses
add r1, r0, i10
bp                  ; Another pause point
```

---

### TIME - Get Current Time
**Syntax**: `time <dest>`

**Description**: Stores the current Unix timestamp (seconds since epoch) into a register.

**Arguments**:
- `dest`: Destination register to store timestamp

**Behavior**:
- Retrieves current system time as floating-point seconds since Unix epoch
- Useful for timing code blocks or profiling

**Examples**:
```
time r0             ; Get current time in r0
; ... do some work ...
time r1             ; Get time again in r1
sub r2, r1, r0      ; Calculate elapsed time in r2
```

---

## Graphics Commands

Grid-based graphics rendering using Pygame. Requires Pygame to be installed.

### SCREEN - Initialize Graphics Screen
**Syntax**: `screen <rows>, <cols>, <memory_addr>`

**Description**: Initializes a graphics window with specified dimensions.

**Arguments**:
- `rows`: Number of rows in the grid (height)
- `cols`: Number of columns in the grid (width)
- `memory_addr`: Starting memory address for pixel data

**Behavior**:
- Creates a Pygame window with grid cells
- Each cell corresponds to a memory location starting from `memory_addr`
- Memory layout: row-major order (left to right, top to bottom)
- Pixel values: > 0 = black cell, ≤ 0 = white cell

**Examples**:
```
screen i10, i10, i0    ; 10x10 grid starting at memory[0]
screen r0, r1, i100    ; Grid size from registers, data at memory[100]
screen i20, i30, i0    ; 20x30 grid
```

---

### RENDER - Render Graphics Screen
**Syntax**: `render`

**Description**: Updates the graphics window to display current memory values as pixels.

**Arguments**: None

**Behavior**:
- Reads pixel data from memory addresses set by `screen`
- Renders grid with black/white cells based on memory values
- Must be called after `screen` initialization
- Can be called multiple times to update display

**Errors**:
- Throws "Screen not initialized" if called before `screen`

**Examples**:
```
screen i6, i6, i0
mov r0, i0          ; Initialize pixels
loop:
    store i1, [r0]  ; Set pixel to black
    inc r0
    cmp r0, i36     ; Check if done (6*6=36)
    jl loop
    render          ; Display the grid
```

---

## Labels

Labels mark positions in code that can be jumped to or called.

**Syntax**: `<label_name>:`

**Rules**:
- Label names are alphanumeric and underscores, case-insensitive
- Labels must end with a colon `:`
- Each label must have a unique name
- Labels are resolved at parse time
- Labels can appear on their own line or before code

**Examples**:
```
loop:
    add r0, r0, i1
    cmp r0, i10
    jl loop         ; Jump back to 'loop' label

function_start:
    mov r1, i42
    ret

end:
    print
```

---

## Value Types Reference

### Registers
- **Format**: `r0`, `r1`, `r2`, ... `r15` (default 16 registers)
- **Count**: Configurable via `-registers` flag when running
- **Storage**: Can hold any integer or floating-point value
- **Initialization**: All registers start at 0
- **Examples**: `mov r0, i42` or `add r1, r0, r2`

### Immediate Values
- **Format**: 
  - Positive: `i<value>` (e.g., `i0`, `i42`, `i100`)
  - Negative: `in<value>` (e.g., `in5` for -5, `in10` for -10)
- **Usage**: Constant values in any instruction
- **Examples**: `mov r0, i42` or `add r1, i5, i10` or `mov r2, in3`

### Memory Addresses
- **Format**: `[<expression>]`
- **Supports**: Arithmetic operations (+, -)
- **Examples**:
  - `[r0]` - value at address in r0
  - `[r0+i5]` - value at (r0 + 5)
  - `[i100]` - value at address 100
  - `[i100-r1]` - value at (100 - r1)

### Stack
- **Type**: LIFO (Last-In-First-Out) data structure
- **Operations**: `push` and `pop` only
- **Usage**: Temporary data storage
- **Errors**: Underflow if popping from empty stack
- **Note**: No direct addressing - can't access arbitrary stack positions

### Call Stack
- **Type**: Internal LIFO stack for return addresses
- **Operations**: Automatic (used by `call` and `ret`)
- **Capacity**: Limited only by available memory
- **Errors**: Underflow if `ret` with no matching `call`

---

## Execution Model

### Program Lifecycle
1. **Initialization**: Registers initialized to 0, memory empty, stacks empty
2. **Tokenization**: Source code split into tokens
3. **Parsing**: Tokens converted to instructions, labels resolved
4. **Execution**: Instructions execute sequentially (unless jumped)
5. **Termination**: Program ends when instruction counter >= total instructions

### Key Components
- **Registers**: 16 fixed-size storage (configurable count)
- **Memory**: Unlimited key-value store (integer keys)
- **Instruction Counter (IC)**: Tracks current instruction (starts at 0)
- **Stack**: LIFO data structure for temporary values
- **Call Stack**: LIFO for return addresses from subroutines
- **Flags**: Comparison flags set by `cmp` instruction
- **Comparison Flags**:
  - `lesser_flag`: Set when left < right
  - `equal_flag`: Set when left == right
  - `greater_flag`: Set when left > right

### Instruction Flow
```
Start (IC=0)
  ↓
Next Instruction
  ↓
Execute Instruction
  ↓
IC += 1 (normal flow) OR return from jump/call
  ↓
Repeat until IC >= instruction count
  ↓
Program End
```

---

## Common Patterns

### Loop
```
mov r0, i0          ; Counter
loop:
    add r0, r0, i1
    cmp r0, i10
    jl loop         ; Continue if r0 < 10
```

### Conditional
```
cmp r0, i5
jg greater_case
mov r1, i0
jmp end
greater_case:
    mov r1, i1
end:
```

### Memory Iteration
```
mov r0, i0          ; Index
mov r1, i0          ; Sum
loop:
    load r2, [r0]   ; Load memory[r0]
    add r1, r1, r2  ; Accumulate
    add r0, r0, i1  ; Next index
    cmp r0, i10
    jl loop
```

### Function Call
```
main:
    push r0          ; Save registers
    push r1
    mov r0, i10      ; Arguments
    call factorial
    pop r1           ; Restore registers
    pop r0
    jmp end

factorial:
    cmp r0, i1
    jle base_case
    push r0          ; Save n
    sub r0, r0, i1   ; n-1
    call factorial   ; Recursive call
    pop r1           ; Restore n
    mul r0, r0, r1   ; n * factorial(n-1)
    ret

base_case:
    mov r0, i1       ; Return 1
    ret

end:
```

---

## Error Handling

The VM raises exceptions for the following conditions:

### Arithmetic Errors
- **Division by zero**: Thrown when `idiv`, `fdiv`, or `mod` divides by zero
- **Example error**: `"Division by zero"`

### Control Flow Errors
- **Undefined label**: Thrown when jumping to or calling a non-existent label
- **Example error**: `"Undefined label: my_label"`

### Stack Errors
- **Stack underflow**: Thrown when `pop` is executed on an empty stack
- **Example error**: `"Stack underflow"`
- **Call stack underflow**: Thrown when `ret` is executed with empty call stack
- **Example error**: `"Call stack underflow"`

### Input Errors
- **Invalid input**: Thrown when `input` receives non-integer value
- **Example error**: `"Invalid input: abc123"`

### Resource Errors
- **Screen not initialized**: Thrown when `render` called before `screen`
- **Example error**: `"Screen not initialized"`

### Parsing Errors
- **Syntax errors**: Invalid instruction format or unexpected tokens
- **Example**: Incorrect number of arguments for an instruction

---

## Notable Features and Behavior

### Arithmetic Operations
- All arithmetic is performed in immediate execution (not delayed)
- Integer division (`idiv`) uses floor division
- Floating-point division (`fdiv`) returns float results
- Modulo operation works with both positive and negative numbers

### Comparison and Flags
- Each `cmp` instruction resets the flags before setting new ones
- Exactly one comparison flag is true after each `cmp`
- Flags persist until the next `cmp` instruction
- Used by conditional jump instructions

### Memory and Registers
- Memory addresses are dynamic (created on first write)
- Uninitialized memory locations default to 0
- Registers persist throughout program execution
- Register count is configurable via command-line flags

### Stack Operations
- LIFO order: last pushed value is first popped
- Stack is separate from memory (not directly addressable)
- Call stack is also separate and used by `call`/`ret`

### Graphics and Display
- Screen rendering requires Pygame installation
- Memory layout uses row-major order
- Pixel colors: positive values = black, non-positive = white
- Graphics window must be initialized before rendering

### Labels and References
- Labels are case-insensitive
- Forward references are allowed (jumping to labels not yet encountered)
- Labels can have the same scope (each must be unique)
- Labels are resolved once during parsing

---

## Command-Line Flags

Run the VM using: `python main.py <file> [options]`

### Available Options
- `-tokens`: Display all parsed tokens
- `-instructions`: Display all parsed instructions
- `-registers <count>`: Set number of registers (default: 64, configurable)
- `-debug`: Launch interactive debugger GUI
- `-step`: Step through execution (pause after each instruction)
- `-print`: Print state before each instruction
- `-profiler`: Profile code execution

### Example Usage
```bash
# Run with 32 registers and view tokens
python main.py myprogram.vm -registers 32 -tokens

# Debug mode with step-through
python main.py myprogram.vm -debug -step

# View instructions before execution
python main.py myprogram.vm -instructions -print
```

---

## Tips and Best Practices

1. **Use labels for clarity**: Makes code more readable than instruction numbers
2. **Save registers before calls**: Use `push`/`pop` to preserve register values
3. **Initialize before use**: Explicitly set register values before operations
4. **Comment complex logic**: Use `;` comments for documentation
5. **Test conditional jumps**: Verify flag behavior with `print` statements
6. **Memory management**: Track memory addresses to avoid conflicts
7. **Nested calls**: Call stack supports multiple levels of nesting
8. **Pointer arithmetic**: Use `[base+offset]` syntax for array-like access
9. **Profile your code**: Use the `-profiler` flag to identify bottlenecks
10. **Start simple**: Test with basic programs before complex control flow

