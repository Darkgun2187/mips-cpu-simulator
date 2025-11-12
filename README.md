# MIPS Single-Cycle CPU Simulator

**Course:** CSC 252 - Computer Organization (Fall 2025)  
**Programming Language:** C  
**Project Type:** Computer Architecture & Systems Programming

## Overview

A complete single-cycle MIPS processor implementation in C that executes 19 instructions across R-format, I-format, and J-format instruction types. The simulator implements the full instruction pipeline (fetch-decode-execute-memory-writeback) and extends the standard MIPS architecture with three custom instructions requiring modified control logic and ALU operations.

## Technical Achievement

This project demonstrates mastery of computer architecture fundamentals by implementing a **fully functional CPU** that can execute real MIPS assembly programs. The simulator accurately models hardware behavior including the ALU, control unit, register file, and memory systems—all implemented in software using low-level C programming techniques.

## Features

### Complete Instruction Pipeline

The simulator implements all five stages of instruction execution:

1. **Fetch:** Retrieve instruction from instruction memory using Program Counter
2. **Decode:** Extract instruction fields (opcode, rs, rt, rd, shamt, funct, immediate) using bitwise operations
3. **Execute:** Perform ALU operations based on decoded control signals
4. **Memory:** Access data memory for load/store instructions
5. **Writeback:** Update register file with computation results

### Supported Instructions

**Standard MIPS Instructions (16):**
- **Arithmetic:** `add`, `addu`, `sub`, `subu`, `addi`, `addiu`
- **Logical:** `and`, `or`, `xor`
- **Comparison:** `slt`, `slti`
- **Memory:** `lw` (load word), `sw` (store word)
- **Control Flow:** `beq` (branch equal), `j` (jump)

**Custom Extension Instructions (3):**
- **BNE (Branch Not Equal):** Conditional branch when registers are not equal
  - Implementation: Modified control signal `extra1` to invert branch condition
  - Opcode: `0x05`
- **LUI (Load Upper Immediate):** Load 16-bit immediate into upper half of register
  - Implementation: Modified ALU input multiplexing with `extra2` control signal
  - Shifts immediate value left by 16 bits
  - Opcode: `0x0f`
- **SLL (Shift Left Logical):** Logical left shift by shift amount
  - Implementation: New ALU operation 5 for shift functionality
  - Uses `shamt` field for shift distance
  - Funct: `0x00` (R-format)

### CPU Components Implemented

**Control Unit:**
- Generates **12+ control signals** from instruction opcodes
- Decodes 32-bit instructions into component fields
- Sets signals for: `ALUsrc`, `RegDst`, `MemRead`, `MemWrite`, `MemToReg`, `RegWrite`, `Branch`, `Jump`
- Custom control signals: `extra1`, `extra2`, `extra3` for instruction extensions

**Arithmetic Logic Unit (ALU):**
- **6 operations:** AND (0), OR (1), ADD/SUB (2), SLT (3), XOR (4), SLL (5)
- Dynamic input multiplexing based on instruction type
- Zero flag generation for conditional branches
- Support for signed/unsigned arithmetic

**Register File:**
- 34 registers total: 32 general-purpose + 2 special (`lo`, `hi` for future multiply/divide support)
- Dual read ports (rs, rt)
- Single write port (rd or rt depending on instruction format)
- Register 0 hardwired to zero

**Memory System:**
- Separate instruction and data memory
- Byte-addressable memory with word-aligned access
- Address translation (byte address → word index)
- Load/store operations with proper data alignment

## Implementation Details

### Bitwise Operation Examples

**Instruction Field Extraction:**
opcode = (instruction >> 26) & 0x3F; // Extract bits 31-26
rs = (instruction >> 21) & 0x1F; // Extract bits 25-21
rt = (instruction >> 16) & 0x1F; // Extract bits 20-16
rd = (instruction >> 11) & 0x1F; // Extract bits 15-11
shamt = (instruction >> 6) & 0x1F; // Extract bits 10-6
funct = instruction & 0x3F; // Extract bits 5-0
imm16 = instruction & 0xFFFF; // Extract bits 15-0


**Sign Extension:**
// Sign-extend 16-bit immediate to 32 bits
imm32 = (imm16 & 0x8000) ? (imm16 | 0xFFFF0000) : imm16;


### Control Logic Design

The control unit uses a **decode-then-set** approach:

1. **Identify format** (R, I, or J) from opcode
2. **Set base control signals** for format type
3. **Refine signals** based on funct field (R-format) or specific opcode (I/J-format)
4. **Handle special cases** with custom control signals

**Example: BNE vs BEQ Differentiation**
Both use the same ALU operation (subtraction with `bNegate=1`), but:
- BEQ: `branch=1`, `extra1=0` → branch if `zero=1`
- BNE: `branch=1`, `extra1=1` → branch if `zero=0`

### Memory Address Translation
// PC is byte address, but instruction memory is word array
WORD instruction = instructionMemory[curPC / 4];

// Data memory also requires byte-to-word conversion
int wordAddress = aluResult / 4;
WORD data = dataMemory[wordAddress];


## Project Structure
mips-cpu-simulator/
├── sim4.c # Complete CPU implementation
├── sim4.h # Header with structs and function prototypes
├── test_extraInst_BNE.c # BNE instruction test case
├── test_extraInst_BNE.out # Expected output for BNE
├── test_extraInst_LUI.c # LUI instruction test case
├── test_extraInst_LUI.out # Expected output for LUI
├── test_extraInst_SLL.c # SLL instruction test case
├── test_extraInst_SLL.out # Expected output for SLL
└── sim4.pdf # Assignment specification


## How to Compile and Run

### Compilation
gcc -std=c99 -Wall -o sim4 sim4.c test_extraInst_BNE.c


### Execution
./sim4 < test_input.txt


### Running Test Cases

Test BNE instruction
gcc -std=c99 -o sim4 sim4.c test_extraInst_BNE.c
./sim4 > output.txt
diff output.txt test_extraInst_BNE.out

Test LUI instruction
gcc -std=c99 -o sim4 sim4.c test_extraInst_LUI.c
./sim4 > output.txt
diff output.txt test_extraInst_LUI.out

Test SLL instruction
gcc -std=c99 -o sim4 sim4.c test_extraInst_SLL.c
./sim4 > output.txt
diff output.txt test_extraInst_SLL.out


## Skills Demonstrated

### Low-Level Systems Programming
- Pointer arithmetic and array manipulation
- Direct memory access patterns
- Bitwise operations (masking, shifting, AND, OR, XOR)
- Unsigned vs signed integer handling

### Computer Architecture
- CPU datapath and control unit design
- Instruction format understanding (R, I, J types)
- Pipeline stage implementation
- Hardware multiplexer simulation in software

### Algorithm Design
- Control signal generation from opcodes
- ALU operation selection logic
- Branch target address calculation
- Memory address translation algorithms

### Software Engineering
- Modular function decomposition
- Struct-based data organization
- Clean separation of concerns (fetch, decode, execute, memory, writeback)
- Comprehensive testing with multiple test cases

## Design Decisions

### Custom Instruction Extensions

**Challenge:** Extend CPU without breaking existing functionality

**Solution:**
- Used spare control signal bits (`extra1`, `extra2`, `extra3`) to differentiate custom behaviors
- Added new ALU operation (5) for shift functionality
- Modified existing MUX inputs rather than adding new hardware

### ALU Input Multiplexing

**Challenge:** LUI and SLL require non-standard ALU inputs

**Solution:**
- **LUI:** Set ALU input 1 to 0, input 2 to `(imm16 << 16)`, use ADD operation
- **SLL:** Set ALU input 1 to `rt` value, input 2 to `shamt`, use new shift operation

This approach reuses existing ALU operations while supporting new instructions.

## Testing

The project includes comprehensive test cases for all three custom instructions:

- **test_extraInst_BNE.c:** Tests branch-not-equal control flow
- **test_extraInst_LUI.c:** Tests upper immediate loading with various values
- **test_extraInst_SLL.c:** Tests shift operations with different shift amounts

Each test case includes:
- Assembly-level test code
- Expected register file states
- Expected memory states
- Expected program counter values

## Constraints Met

- **Exact opcode compliance:** All instructions use standard MIPS opcodes
- **Control signal consistency:** Ordinary instructions use standard control bits (all `extra` bits = 0)
- **Don't-care handling:** All unused control signals set to 0
- **Invalid instruction handling:** Returns 0 from `fill_CPUControl()` for unrecognized opcodes

## Learning Outcomes

This project provided deep understanding of:
- How high-level programming constructs (if, while, +, -, etc.) are implemented at the hardware level
- The role of the control unit in orchestrating CPU components
- Trade-offs between hardware complexity and instruction flexibility
- The relationship between instruction encoding and control signal generation

---

**Author:** Saketh Katta  
**University:** University of Arizona  
**Major:** Computer Science (Junior)  
**Course:** CSC 252 - Computer Organization
