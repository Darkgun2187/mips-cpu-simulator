/* sim4.c
 * Name: Saketh Katta
 * 
 * Sim 4 - Single-Cycle CPU (Milestone 2)
 * Implements a complete single-cycle MIPS processor with 3 extra instructions:
 * - BNE (Branch Not Equal)
 * - LUI (Load Upper Immediate)
 * - SLL (Shift Left Logical)
 */

#include "sim4.h"


/* ====== MILESTONE 1 FUNCTIONS ====== */


/*
 * Extracts all fields from a 32-bit MIPS instruction
 */
void extract_instructionFields(WORD instruction, InstructionFields *fieldsOut)
{
    // Extract all fields from the 32-bit instruction
    
    // opcode: bits 31-26
    fieldsOut->opcode = (instruction >> 26) & 0x3F;
    
    // rs: bits 25-21
    fieldsOut->rs = (instruction >> 21) & 0x1F;
    
    // rt: bits 20-16
    fieldsOut->rt = (instruction >> 16) & 0x1F;
    
    // rd: bits 15-11
    fieldsOut->rd = (instruction >> 11) & 0x1F;
    
    // shamt: bits 10-6
    fieldsOut->shamt = (instruction >> 6) & 0x1F;
    
    // funct: bits 5-0
    fieldsOut->funct = instruction & 0x3F;
    
    // imm16: bits 15-0
    fieldsOut->imm16 = instruction & 0xFFFF;
    
    // imm32: sign-extended version of imm16
    fieldsOut->imm32 = signExtend16to32(fieldsOut->imm16);
    
    // address: bits 25-0
    fieldsOut->address = instruction & 0x3FFFFFF;
}


/*
 * Decodes instruction and sets all CPU control signals
 * Returns 1 if valid instruction, 0 otherwise
 */
int fill_CPUControl(InstructionFields *fields, CPUControl *controlOut)
{
    // Initialize all control signals to 0
    controlOut->ALUsrc = 0;
    controlOut->ALU.op = 0;
    controlOut->ALU.bNegate = 0;
    controlOut->memRead = 0;
    controlOut->memWrite = 0;
    controlOut->memToReg = 0;
    controlOut->regDst = 0;
    controlOut->regWrite = 0;
    controlOut->branch = 0;
    controlOut->jump = 0;
    controlOut->extra1 = 0;
    controlOut->extra2 = 0;
    controlOut->extra3 = 0;
    
    // Checks for R-format instructions
    if (fields->opcode == 0x00) {
        
        if (fields->funct == 0x00) {  // sll (EXTRA INSTRUCTION)
            controlOut->ALU.op = 5;      // ALU op 5 for shift
            controlOut->ALU.bNegate = 0;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x20) {  // add
            controlOut->ALU.op = 2;
            controlOut->ALU.bNegate = 0;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x21) {  // addu
            controlOut->ALU.op = 2;
            controlOut->ALU.bNegate = 0;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x22) {  // sub
            controlOut->ALU.op = 2;
            controlOut->ALU.bNegate = 1;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x23) {  // subu
            controlOut->ALU.op = 2;
            controlOut->ALU.bNegate = 1;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x24) {  // and
            controlOut->ALU.op = 0;
            controlOut->ALU.bNegate = 0;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x25) {  // or
            controlOut->ALU.op = 1;
            controlOut->ALU.bNegate = 0;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x26) {  // xor
            controlOut->ALU.op = 4;
            controlOut->ALU.bNegate = 0;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else if (fields->funct == 0x2a) {  // slt
            controlOut->ALU.op = 3;
            controlOut->ALU.bNegate = 1;
            controlOut->regDst = 1;
            controlOut->regWrite = 1;
            controlOut->ALUsrc = 0;
        } else {
            // Invalid funct - returns 0, leaves all control bits at 0
            return 0;
        }
        return 1;
    }
    
    // I-format and J-format instructions
    if (fields->opcode == 0x02) {  // j
        controlOut->jump = 1;
        return 1;
    } else if (fields->opcode == 0x04) {  // beq
        controlOut->ALU.op = 2;
        controlOut->ALU.bNegate = 1;
        controlOut->ALUsrc = 0;
        controlOut->branch = 1;
        return 1;
    } else if (fields->opcode == 0x05) {  // bne (EXTRA INSTRUCTION)
        controlOut->ALU.op = 2;
        controlOut->ALU.bNegate = 1;
        controlOut->ALUsrc = 0;
        controlOut->branch = 1;
        controlOut->extra1 = 1;  // Identify BNE from BEQ
        return 1;
    } else if (fields->opcode == 0x08) {  // addi
        controlOut->ALU.op = 2;
        controlOut->ALU.bNegate = 0;
        controlOut->ALUsrc = 1;
        controlOut->regWrite = 1;
        controlOut->regDst = 0;
        return 1;
    } else if (fields->opcode == 0x09) {  // addiu
        controlOut->ALU.op = 2;
        controlOut->ALU.bNegate = 0;
        controlOut->ALUsrc = 1;
        controlOut->regWrite = 1;
        controlOut->regDst = 0;
        return 1;
    } else if (fields->opcode == 0x0a) {  // slti
        controlOut->ALU.op = 3;
        controlOut->ALU.bNegate = 1;
        controlOut->ALUsrc = 1;
        controlOut->regWrite = 1;
        controlOut->regDst = 0;
        return 1;
    } else if (fields->opcode == 0x0f) {  // lui (EXTRA INSTRUCTION)
        controlOut->ALU.op = 2;         // Use ADD operation
        controlOut->ALU.bNegate = 0;
        controlOut->ALUsrc = 1;
        controlOut->regWrite = 1;
        controlOut->regDst = 0;
        controlOut->extra2 = 1;  // Signal LUI operation
        return 1;
    } else if (fields->opcode == 0x23) {  // lw
        controlOut->ALU.op = 2;
        controlOut->ALU.bNegate = 0;
        controlOut->ALUsrc = 1;
        controlOut->memRead = 1;
        controlOut->memToReg = 1;
        controlOut->regWrite = 1;
        controlOut->regDst = 0;
        return 1;
    } else if (fields->opcode == 0x2b) {  // sw
        controlOut->ALU.op = 2;
        controlOut->ALU.bNegate = 0;
        controlOut->ALUsrc = 1;
        controlOut->memWrite = 1;
        return 1;
    }
    
    return 0;  // invalid opcode
}


/* ====== MILESTONE 2 FUNCTIONS ====== */


/*
 * Retrieves instruction at the current PC from instruction memory
 */
WORD getInstruction(WORD curPC, WORD *instructionMemory)
{
    // PC is a byte address so convert to word index
    return instructionMemory[curPC / 4];
}

/*
 * Determines first input to ALU based on control signals
 * and handles the special cases for LUI and SLL instructions
 */
WORD getALUinput1(CPUControl *controlIn,
                  InstructionFields *fieldsIn,
                  WORD rsVal, WORD rtVal, WORD reg32, WORD reg33,
                  WORD oldPC)
{
    // LUI: input1 is 0
    if (controlIn->extra2 == 1) {
        return 0;
    }
    
    // SLL: shift rt by shamt, so input1 is rt
    if (controlIn->ALU.op == 5) {
        return rtVal;
    }
    
    // For ordinary instructions ALU input 1 is rsVal
    return rsVal;
}

/*
 * Determines second input to ALU based on control signals
 * Implements ALUsrc MUX with special handling for SLL and LUI
 */
WORD getALUinput2(CPUControl *controlIn,
                  InstructionFields *fieldsIn,
                  WORD rsVal, WORD rtVal, WORD reg32, WORD reg33,
                  WORD oldPC)
{
    // SLL: use shamt as shift amount
    if (controlIn->ALU.op == 5) {
        return fieldsIn->shamt;
    }
    
    // LUI: load upper immediate (shift left by 16 bits)
    if (controlIn->extra2 == 1) {
        return fieldsIn->imm16 << 16;
    }
    
    // Normal MUX behavior
    if (controlIn->ALUsrc == 0) {
        return rtVal;
    } else {
        return fieldsIn->imm32;
    }
}


/*
 * Performs ALU operation on two inputs based on control signals
 * Supports AND, OR, ADD, SUB, XOR, SLT, and SLL operations
 */
void execute_ALU(CPUControl *controlIn,
                 WORD input1, WORD input2,
                 ALUResult *aluResultOut)
{
    WORD result;
    
    // Handles bNegate for subtraction
    WORD input2_final = input2;
    if (controlIn->ALU.bNegate == 1) {
        input2_final = -input2;
    }
    
    // Performs operation based on ALU.op
    switch (controlIn->ALU.op) {
        case 0:  // AND
            result = input1 & input2_final;
            break;
        case 1:  // OR
            result = input1 | input2_final;
            break;
        case 2:  // ADD (SUB if bNegate=1)
            result = input1 + input2_final;
            break;
        case 3:  // SLT
            // Compare the original inputs
            if ((int)input1 < (int)input2) {
                result = 1;
            } else {
                result = 0;
            }
            break;
        case 4:  // XOR
            result = input1 ^ input2_final;
            break;
        case 5:  // SLL - EXTRA INSTRUCTION
            result = input1 << input2;
            break;
        default:
            result = 0;
            break;
    }
    
    // Sets the zero flag
    if (result == 0) {
        aluResultOut->zero = 1;
    } else {
        aluResultOut->zero = 0;
    }
    
    // Stores the ALU results
    aluResultOut->result = result;
    aluResultOut->extra = 0;
}


/*
 * Handles memory read and write operations based on control signals
 */
void execute_MEM(CPUControl *controlIn,
                 ALUResult *aluResultIn,
                 WORD rsVal, WORD rtVal,
                 WORD *memory,
                 MemResult *resultOut)
{
    // Converts byte address to word index
    int address = aluResultIn->result / 4;
    
    // Handle memory read
    if (controlIn->memRead == 1) {
        resultOut->readVal = memory[address];
    } else {
        resultOut->readVal = 0;
    }
    
    // Handle memory write
    if (controlIn->memWrite == 1) {
        memory[address] = rtVal;
    }
}


/*
 * Computes next PC value for execution, branches, and jumps
 */
WORD getNextPC(InstructionFields *fields, CPUControl *controlIn, int aluZero,
               WORD rsVal, WORD rtVal,
               WORD oldPC)
{
    // Jump instruction
    if (controlIn->jump == 1) {
        WORD pcPlus4 = oldPC + 4;
        WORD upper4 = pcPlus4 & 0xF0000000;
        WORD jumpTarget = (fields->address << 2) & 0x0FFFFFFF;
        return upper4 | jumpTarget;
    }
    
    // Branch instruction (beq or bne)
    if (controlIn->branch == 1) {
        int takeBranch;
        
        if (controlIn->extra1 == 1) {
            // BNE: (zero flag is 0) - EXTRA INSTRUCTION
            takeBranch = (aluZero == 0);
        } else {
            // BEQ: (zero flag is 1)
            takeBranch = (aluZero == 1);
        }
        
        if (takeBranch) {
            return (oldPC + 4) + (fields->imm32 << 2);
        } else {
            return oldPC + 4;
        }
    }
    
    // Normal instruction: PC = PC + 4
    return oldPC + 4;
}


/*
 * Performs register write-back stage.
 * Writes result to destination register if regWrite is enabled
 */
void execute_updateRegs(InstructionFields *fields, CPUControl *controlIn,
                        ALUResult *aluResultIn, MemResult *memResultIn,
                        WORD *regs)
{
    // Only writes if regWrite is enabled
    if (controlIn->regWrite == 0) {
        return;
    }
    
    // Determines destination register
    int destReg;
    if (controlIn->regDst == 1) {
        destReg = fields->rd;  // R-format
    } else {
        destReg = fields->rt;  // I-format
    }
    
    // Determines value to write
    WORD writeValue;
    if (controlIn->memToReg == 1) {
        writeValue = memResultIn->readVal;  // From memory (lw)
    } else {
        writeValue = aluResultIn->result;   // From ALU
    }
    
    // Write to register (but never write to register 0)
    if (destReg != 0) {
        regs[destReg] = writeValue;
    }
}
