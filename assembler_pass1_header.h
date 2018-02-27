#ifndef _ASSEM1_HEADER_FILE_H_
#define _ASSEM1_HEADER_FILE_H_

#define MAX_INSTRUCTIONS 100	
#define MAX_OPCODES 60
/*
 * For assembler pass 1, an intermediate file is generated along with SYMTAB (symbol table)
*/

/*
#
#
#	PREPARE THE REQUIRED DATASTRUCTURES FOR ASSEMBLER PASS 1
#
#
*/
/*An instruction contains LABEL, OPCODE and OPERAND*/
struct instruction{
	char* LABEL;
	char* OPCODE;
	char* OPERAND;
};
typedef struct instruction instruction;

/*SYMTAB is a data structure which will be used to store the symbols (labels) and their addresses (in hex)*/
struct symbol_table{
	char* LABEL;
	int address;
};
typedef struct symbol_table symbol_table;

/* This structure is used to store and write (to a file) intermediate instructions containing addresses */
struct intermediate_instruction{
	int address;
	instruction ins;
	char* objCode;
};
typedef struct intermediate_instruction intermediate_instruction;

/*OPTAB is a data structure which will be used to validate the read opcodes from the source file*/
struct opcode_table{
	char* mnemonic;
	char* OPCODE;
};
typedef struct opcode_table opcode_table;

/*
#
#
#	PREPARE THE REQUIRED PARAMETERS AND VARIABLES FOR ASSEMBLER PASS 1
#
#
*/
/*location counter*/
int LOCCTR; 
 /*the starting address is by default equal to zero*/
extern int STARTADR;
/*this contains the intermediate instructions to be written into the intermediate file*/
intermediate_instruction* intermediateInstructions;	
/*this is the symbol table*/
symbol_table* SYMTAB;	
/* total number of instructions (lines) present in the given assembly source file will be stored in this variable */
int numberOfInstructions; 
/*Error flag*/
extern int errorFlag;
/*this is the opcode table*/
opcode_table* OPTAB;
/* total number of opcodes used in OPCODE.txt (this is actually used to create the malloc(opcode))*/
int numberOfOpcodes;
/*
#
#
#	DECLARE THE REQUIRED UTILITY FUNCTIONS FOR ASSEMBLER PASS 1
#
#
*/
extern instruction readInstruction(char* line);

extern instruction* getAllInstructions(char sourceFilename[]);

extern void writeIntermediateInstructions(int lineNumber,int addr,instruction ins);

extern int searchSYMTAB(char* label,int symtabIndex);

extern void writeSYMTAB();

extern void readOPTAB();

extern int searchOPTAB(char* opcode);

extern int lengthOfConstantInBytes(char* constant);

extern void assembler_pass1();

#endif //_ASSEM1_HEADER_FILE_H_
